#include "RtspClient.h"


int RtspClient::init(const std::string& rtsp_addr_)
{
	m_rtsp_addr = rtsp_addr_;
    if (m_pFormatCtx) {
		avformat_close_input(&m_pFormatCtx);
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
	}
	if (m_packet) {
		av_free(m_packet);
		m_packet = NULL;
	}

	//设置缓存大小,1080p可将值跳到最大
	av_dict_set(&m_options, "buffer_size", "1024000", 0);
	// tcp open
	av_dict_set(&m_options, "rtsp_transport", "tcp", 0);
	// set timeout unit: us
	av_dict_set(&m_options, "stimeout", "5000000", 0);
	// set max delay
	av_dict_set(&m_options, "max_delay", "500000", 0);

	m_pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&m_pFormatCtx, rtsp_addr_.c_str(), NULL, &m_options) !=
			0) {
        std::cout << "Couldn't open input stream.\n";
		return -1;
	}

	// get video stream info
	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0) {
		std::cout << "Couldn't find stream information.\n";
		return -2;
	}

	// find stream video codec type
	unsigned int i = 0;
	for (i = 0; i < m_pFormatCtx->nb_streams; i++) {
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			m_videoindex = i;
			m_codec_id = m_pFormatCtx->streams[i]->codec->codec_id;
			break;
		}
	}
	std::cout<<"video index is: "<<m_videoindex<<std::endl;

	if (m_videoindex == -1) {
        std::cout << "Didn't find a video stream.";
		return -3;
	}

	// 申请空间，存放的每一帧数据 （h264、h265）
	m_packet = (AVPacket*)av_malloc(sizeof(AVPacket));

	if (AV_CODEC_ID_H264 == m_codec_id) 
	{
		m_code_type = CODE_TYPE::VIDEO_TYPE_H264;
		std::cout << "video stream decode type is h264\n"<<std::endl;
	} 
	else if (AV_CODEC_ID_HEVC == m_codec_id) 
	{
		m_code_type = CODE_TYPE::VIDEO_TYPE_H265;
		std::cout << m_codec_id << " ,video stream decode type is h265"<<std::endl;
	}


	return 0;
}

bool RtspClient::read_packet(std::shared_ptr<Decode> decode_c, cv::Mat& image){

	if (av_read_frame(m_pFormatCtx, m_packet) < 0) {
		// std::cout <<" av read frame error..\n";
		return false;
	}

	// if (m_packet->stream_index == m_videoindex) {
	// 	// RKMPPDecode::getInstance()->decode(packet_, image, "");
	// 	decode_c->decode(m_packet, image);
	// 	std::cout<<m_packet->size<<std::endl;
	// 	bool bRet = true;
	// 	av_packet_unref(m_packet); 
	// 	return bRet;
	// }
	// av_packet_unref(m_packet);
	
	return true;
}

bool RtspClient::is_videoidx()
{
	return m_packet->stream_index == m_videoindex;
}

void RtspClient::release_packet()
{
	av_packet_unref(m_packet);
}



