#include <iostream>

#include "RKMPPDecode.h"

static int num = 0;

int RKMPPDecode::init() {
	// if (m_bInit) {
	// 	std::cout<<__FUNCTION__<<__LINE__ << "initMpp : " << m_bInit<<std::endl;
	// 	return 0;
	// }

	int ret = mpp_create(&ctx, &mpi);

	if (MPP_OK != ret) {
		mpp_err("mpp_create failed\n");
		deinit();
		return -1;
	}

	
	RK_U32 need_split   = 1;
	param = &need_split;
	ret = mpi->control(ctx, MPP_DEC_SET_PARSER_SPLIT_MODE, param);
	if (MPP_OK != ret) {
		mpp_err("mpi->control failed\n");
		deinit();
		return -1;
	}

    param = &need_split;
	ret = mpi->control(ctx,MPP_SET_INPUT_BLOCK, param);
	if (MPP_OK != ret) {
		mpp_err("mpi->control failed\n");
		deinit();
		return -2;
	}

	ret = mpp_init(ctx, MPP_CTX_DEC, type);
	if (MPP_OK != ret) {
		mpp_err("mpp_init failed\n");
		deinit();
		return -1;
	}

	m_bInit = true;
	return 0;
}

// int RKMPPDecode::initMppMtx()
// {
// 	// m_QueMtx.lock();
// 	initMpp();
// 	return 0;
// 	// m_QueMtx.unlock();
// }

void RKMPPDecode::deinit() {
	if (packet) {
		mpp_packet_deinit(&packet);
		packet = NULL;
	}

	if (frame) {
		mpp_frame_deinit(&frame);
		frame = NULL;
	}

	if (ctx) {
		mpp_destroy(ctx);
		ctx = NULL;
	}


	if (pkt_grp) {
		mpp_buffer_group_put(pkt_grp);
		pkt_grp = NULL;
	}

	if (frm_grp) {
		mpp_buffer_group_put(frm_grp);
		frm_grp = NULL;
	}
	printf("deinit 11111111111111111111111111111111111111111111111111111111111111\n");
}

int RKMPPDecode::decode(AVPacket *av_packet, cv::Mat &image) {
	RK_U32 pkt_done = 0;
	RK_U32 pkt_eos = 0;
	RK_U32 err_info = 0;
	MPP_RET ret = MPP_OK;
	RK_U32 nPutErrNum = 0;
	// use av_packet->data initialize mpp packet


	if(m_nDecPutErrNum >= MAX_DEC_ERROR_NUM)
	{
		m_nDecPutErrNum = 0;
		m_bInit = false;
		init();
		std::cout<< "m_nDecPutErrNum too mach: " << m_nDecPutErrNum <<" ;goto deinit "<<std::endl;
		return ret;
	}

	// printf("data is: %x, size is: %d\n", &av_packet->data, av_packet->size);
	ret = mpp_packet_init(&packet, av_packet->data, av_packet->size);
	if (ret != MPP_OK)
	{
		std::cout<<"mpp_packet_init error"<<std::endl;
	}

	mpp_packet_set_pts(packet, av_packet->pts);
	// std::cout << " packet->pts " << av_packet->pts << std::endl;
	bool first = true;
	num++;

	// std::cout << "******************************** start decode one package "
	// 						 "****************************\n";

	do {
		RK_S32 times = 5;
		// send the packet first if packet is not done
		if (!pkt_done) {
			ret = mpi->decode_put_packet(ctx, packet);
			if (MPP_OK == ret) {
				pkt_done = 1;
				if (m_nDecPutErrNum > 1)
				{
					m_nDecPutErrNum --;
				}
			} else {
				nPutErrNum++;
				std::cout << " decode put package error...."<<ret<<std::endl;
			}
		}

		// then get all available frame and release
		do {
			RK_S32 get_frm = 0;
			RK_U32 frm_eos = 0;

		try_again:
			ret = mpi->decode_get_frame(ctx, &frame);
			if (MPP_ERR_TIMEOUT == ret) {
				if (times > 0) {
					std::cout<< "decode_get_frame timeout....."<<std::endl;

					times--;
					msleep(2);
					goto try_again;
				}

				std::cout << "decode_get_frame failed too much time\n"<<std::endl;
			}
			if (MPP_OK != ret) {
				std::cout << " decode get frame error.  " << ret<<std::endl;
				break;
			}

			if (frame) {
				if (mpp_frame_get_info_change(frame)) {
					RK_U32 width = mpp_frame_get_width(frame);
					RK_U32 height = mpp_frame_get_height(frame);
					RK_U32 hor_stride = mpp_frame_get_hor_stride(frame);
					RK_U32 ver_stride = mpp_frame_get_ver_stride(frame);
					RK_U32 buf_size = mpp_frame_get_buf_size(frame);

					mpp_log("decode_get_frame get info changed found\n");
					mpp_log(
							"decoder require buffer w:h [%d:%d] stride [%d:%d] buf_size %d",
							width, height, hor_stride, ver_stride, buf_size);


					ret = mpp_buffer_group_get_internal(&frm_grp, MPP_BUFFER_TYPE_ION);
					if (ret) {
						mpp_err("get mpp buffer group  failed ret %d\n", ret);
						break;
					}
					mpi->control(ctx, MPP_DEC_SET_EXT_BUF_GROUP, frm_grp);

					mpi->control(ctx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
				} else {
					err_info =
							mpp_frame_get_errinfo(frame) | mpp_frame_get_discard(frame);
					if (err_info) {
						mpp_log("decoder_get_frame get err info:%d discard:%d.\n",
						mpp_frame_get_errinfo(frame), mpp_frame_get_discard(frame));
					}

					frame_count++;
					// printf("--- frame_count is: %d\n", frame_count);

					if (!err_info) {
						YUV420SP2Mat(image);
						// if (!image.empty()) {
						// 	int nIndex = mpp_frame_get_pts(frame);
						// 	cb_(image);
						// }
					}
				}
				frm_eos = mpp_frame_get_eos(frame);
				mpp_frame_deinit(&frame);

				frame = NULL;
				get_frm = 1;
				first = false;
			} else {
				// std::cout<< "first frame: " << first << ", frame is null"<<std::endl;
				;
			}

			// try get runtime frame memory usage
			if (frm_grp) {
				size_t usage = mpp_buffer_group_usage(frm_grp);
				if (usage > max_usage) max_usage = usage;
			}
			// if last packet is send but last frame is not found continue
			if (pkt_eos && pkt_done && !frm_eos) {
				msleep(10);
				continue;
			}

			if (frm_eos) {
				mpp_log("found last frame\n");
				break;
			}

			// std::cout<< "frame_num " << frame_num << " eos: " << eos<<std::endl;

			if (get_frm) continue;
			break;
		} while (1);

		// std::cout << "pkt_done " << pkt_done<<std::endl;
		if (pkt_done) break;

		//连续 MAX_DEC_ERROR_TRY_NUM 次 decode_put_packet 失败, 丢弃当前帧, 防止陷入死循环
		if (nPutErrNum >= MAX_DEC_ERROR_TRY_NUM)
		{
			std::cout << "decode put package error discard frame, m_nDecPutErrNum: " << m_nDecPutErrNum<<std::endl;
			m_nDecPutErrNum;
			break;
		}

		/*
		 * why sleep here:
		 * mpi->decode_put_packet will failed when packet in internal queue is
		 * full,waiting the package is consumed .Usually hardware decode one
		 * frame which resolution is 1080p needs 2 ms,so here we sleep 3ms
		 * * is enough.
		 */
		msleep(3);
	} while (1);
	if (packet)
	{
    	mpp_packet_deinit(&packet);
		packet = NULL;
	}
	return ret;
}

// int RKMPPDecode::decodeMtx(AVPacket* av_packet, cv::Mat& image, const std::string &p_sn)
// {
// 	// m_QueMtx.lock();
// 	int nRet = decode(av_packet, image, p_sn);
// 	// m_QueMtx.unlock();
// 	return nRet;
// }

void RKMPPDecode::YUV420SP2Mat(cv::Mat &image) {
	RK_U32 width = 0;
	RK_U32 height = 0;
	RK_U32 h_stride = 0;
	RK_U32 v_stride = 0;

	MppBuffer buffer = NULL;
	RK_U8 *base = NULL;

	width = mpp_frame_get_width(frame);
	height = mpp_frame_get_height(frame);
	h_stride = mpp_frame_get_hor_stride(frame);
	v_stride = mpp_frame_get_ver_stride(frame);
	buffer = mpp_frame_get_buffer(frame);
	// std::cout << "mpp_frame_get_pts :"  << mpp_frame_get_pts(frame) << std::endl;
	if(width <= 0 || height <= 0 || NULL == buffer)
	{
		std::cout << "[YUV420SP2Mat] error return width: "<< width << "; height: " << height << ";" << h_stride << ";" << v_stride<<std::endl;
		return;
	}


	base = (RK_U8 *)mpp_buffer_get_ptr(buffer);
	RK_U32 buf_size = mpp_frame_get_buf_size(frame);
	size_t base_length = mpp_buffer_get_size(buffer);

	if(base_length <= 0 || buf_size <= 0 || NULL == base)
	{
		std::cout<< "[YUV420SP2Mat] error return base_length: "<< base_length << "; buf_size: " << buf_size<<std::endl;
		return;
	}

	//std::cout << width << " " << height << " " << h_stride << " " << v_stride
	// 					<< " " << buf_size << " " << base_length<<std::endl;

	RK_U32 i;
	RK_U8 *base_y = base;
	RK_U8 *base_c = base + h_stride * v_stride;

	cv::Mat yuvImg;
	yuvImg.create(height * 3 / 2, width, CV_8UC1);

	//转为YUV420p格式
	int idx = 0;
	for (i = 0; i < height; i++, base_y += h_stride) {
		//        fwrite(base_y, 1, width, fp);
		memcpy(yuvImg.data + idx, base_y, width);
		idx += width;
	}
	for (i = 0; i < height / 2; i++, base_c += h_stride) {
		//        fwrite(base_c, 1, width, fp);
		memcpy(yuvImg.data + idx, base_c, width);
		idx += width;
	}
	//这里的转码需要转为RGB 3通道， RGBA四通道则不能检测成功
	// cv::cvtColor(yuvImg, rgbImg, CV_YUV420sp2RGB);
	cv::cvtColor(yuvImg, image, CV_YUV420sp2RGB);
	// std::cout << "yuv image width, height:" << yuvImg.cols << " " <<
	// yuvImg.rows
	// 					<< ",rgbimg [ " << image.cols << " " << image.rows << "]"<<std::endl;
}
