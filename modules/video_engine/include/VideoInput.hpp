// create by z 2022.3.16


/**
 * 数据来源模块，目前支持从rtsp读取
 * 1. rtsp rk平台使用rk_mpp模块解码
 * 
*/
#ifndef __VIDEO_INPUT_ENGINE_HPP__
#define __VIDEO_INPUT_ENGINE_HPP__
#include <string>
#include <memory>
#include <opencv2/core/core.hpp>

#include "DataSource.h"

class VideoInput
{
public:
    VideoInput(){}
    VideoInput(const std::string& rtsp_addr):m_rtsp_addr(rtsp_addr){}
    VideoInput(const VideoInput& video_input) = delete;
    ~VideoInput(){}

    void open();

    void open(const std::string& rtsp_addr);

    bool read(cv::Mat& cv_image);

    void release();

    void data_cb(cv::Mat& image);

private:
    std::string m_rtsp_addr;
    std::shared_ptr<DataSource> m_data_source;
};

#endif

