#ifndef __DATASOURCE_H__
#define __DATASOURCE_H__

#include <memory>

#include <opencv2/core/core.hpp>
#include "RtspClient.h"
#include "Decode.h"
#include "RKMPPDecode.h"


class DataSource{
public:
    DataSource()
    {

    }
    virtual ~DataSource()
    {

    }

    virtual bool init(const std::string& param) = 0;

    virtual bool read(cv::Mat& cv_img) = 0;
};


class RtspData : public DataSource{
public:
    RtspData(){}

    ~RtspData(){

    }

    bool init(const std::string& rtsp_addr);

    bool read(cv::Mat& cv_img);

    // bool set_cb(rtsp_callback_t cb_);

    bool data_callback(cv::Mat& image);

private:
    RtspClient m_rtsp;
    std::string m_rtsp_addr;
    std::shared_ptr<Decode> decode_ptr;

};


#endif