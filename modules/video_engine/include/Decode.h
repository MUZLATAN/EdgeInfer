#ifndef __DECODE_H__
#define __DECODE_H__

#include <opencv2/core/core.hpp>

extern "C" {
#include <libavformat/avformat.h>
};

#include <functional>

using rtsp_callback_t = std::function<void(cv::Mat&)>;

typedef enum{
    VIDEO_TYPE_H264  = 0,
    VIDEO_TYPE_H265
}CODE_TYPE;


class Decode
{
public:
    Decode()
    {

    }
    virtual ~Decode()
    {

    }

    virtual int init() = 0;

    virtual int decode(AVPacket* av_packet, cv::Mat& image) = 0;

    virtual int set_cb(const rtsp_callback_t& cb) = 0;

    virtual void set_codec_type(CODE_TYPE t) = 0;
};



//
// class HIMPPDecode : public Decode
// {

// }


#endif