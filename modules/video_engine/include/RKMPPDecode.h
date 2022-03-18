#ifndef __RKMPPDECODE_H__
#define __RKMPPDECODE_H__


#include "utils.h"
#include "rk_mpi.h"
#include "rk_mpi_cmd.h"
#include "rk_vdec_cfg.h"
#include "mpp_log.h"
#include "mpp_mem.h"
#include "mpp_env.h"
#include "mpp_time.h"
#include "mpp_common.h"
#include "mpp_frame.h"
#include "mpp_buffer_impl.h"
#include "mpp_frame_impl.h"
#include "mpp_trie.h"
#include "mpp_dec_cfg.h"
extern "C" {
#include <libavformat/avformat.h>
};
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Decode.h"

#define ROK 0
#define RERR -1

#define MAX_DEC_ERROR_NUM (30) // 错误帧数超过当前值则重新初始化MPP
#define MAX_DEC_ERROR_TRY_NUM (5) // 超过当前次数未能成功发送，则丢弃当前帧


class RKMPPDecode : public Decode
{
public:
    RKMPPDecode():
                ctx(NULL),
				mpi(NULL),
				eos(0),
				packet(NULL),
				frame(NULL),
				param(NULL),
				need_split(1),
				type(MPP_VIDEO_CodingAVC),/*h264*/
				frm_grp(NULL),
				pkt_grp(NULL),
				mpi_cmd(MPP_CMD_BASE),
				frame_count(1),
				frame_num(0),
				max_usage(0),
				fp_output(NULL),
				m_nDecPutErrNum(0),
				m_bInit(false)
    {

    }
    ~RKMPPDecode()
    {
        deinit();
    }


//  public:
//     //单例模式
// 	static RKMPPDecode* getInstance() {
// 		static RKMPPDecode decode_c;
// 		return &decode_c;
// 	}

    int init();

    int decode(AVPacket* av_packet, cv::Mat& image);

    int set_cb(const rtsp_callback_t& cb){ cb_ = cb;}

    void set_codec_type(CODE_TYPE t) { 
		if (t == CODE_TYPE::VIDEO_TYPE_H264)
			type = MPP_VIDEO_CodingAVC;
		else if(t == CODE_TYPE::VIDEO_TYPE_H265)
			type = MPP_VIDEO_CodingHEVC;
	}

    //转码, 将yuv转码为rgb的格式
	void YUV420SP2Mat(cv::Mat& image);

	void deinit();

 private:
	MppCtx ctx;
	MppApi* mpi;
	RK_U32 eos;
	MppBufferGroup frm_grp;
	MppBufferGroup pkt_grp;
	MppPacket packet;
	MppFrame frame;
	MppParam param;
	RK_U32 need_split;
	MpiCmd mpi_cmd;
	RK_S32 frame_count;
	RK_S32 frame_num;
	size_t max_usage;
	MppCodingType type;
	FILE* fp_output;

	rtsp_callback_t cb_;
	std::vector<std::string> m_strSnVec;
	int m_nDecPutErrNum;
	volatile bool m_bInit;
 public:
	cv::Mat rgbImg;
};

#endif