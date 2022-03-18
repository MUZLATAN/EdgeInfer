#ifndef __RTSPCLIENT_H__
#define __RTSPCLIENT_H__

#include <string>
#include <memory>
#include <iostream>
#include "Decode.h"

extern "C" {
#include <libavformat/avformat.h>
};





class RtspClient
{
public:
    RtspClient(){
        ffmpeg_set();
    }
    RtspClient(const std::string rtsp_addr_):
                m_rtsp_addr(rtsp_addr_)
    {
        ffmpeg_set();
    }


    RtspClient(RtspClient& rtspc_) = delete;


    ~RtspClient(){
        release();
    }

    int init(const std::string& rtsp_addr_);

    bool read_packet(std::shared_ptr<Decode> decode_c, cv::Mat& image);

    bool is_videoidx();

    void release_packet();

    void release(){
        av_free(m_packet);
		avformat_close_input(&m_pFormatCtx);
    }

private:
    void ffmpeg_set(){
        m_pFormatCtx = NULL;
		m_options = NULL;
		m_packet = NULL;
		m_videoindex = -1;
		m_codec_id = AV_CODEC_ID_NONE;
        av_register_all();
		avformat_network_init();
    }

public:
    AVPacket * m_packet;
    std::shared_ptr<Decode> decodec;
    CODE_TYPE m_code_type;

private:
    std::string m_rtsp_addr;
    AVFormatContext* m_pFormatCtx;
	AVDictionary * m_options;
	int m_videoindex;
	enum AVCodecID m_codec_id;
    

    

    
    
};

#endif