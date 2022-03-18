// create by z 2022.3.16

#include "VideoInput.hpp"

void VideoInput::open(){

}

void VideoInput::open(const std::string& rtsp_addr){
    #if defined (__PLATFORM_X86__) 
        printf("PLATFORM x86 cann't read image\n");
    #elif defined (__PLATFORM_HISI__)
        printf("PLATFORM HISI cann't read image\n");
    #elif defined (__PLATFORM_RV__) || (__PLATFORM_RKPRO__) ||(__PLATFORM_RK__)
        //  = new EngineRKNN(param);
        m_data_source = std::make_shared<RtspData>();
        printf("m_data_source is Rtsp Data\n");
    #else
        printf("cann't read image\n");
    #endif

    m_data_source->init(rtsp_addr);
    return ;
}

bool VideoInput::read(cv::Mat& cv_image){
    return m_data_source->read(cv_image);
}

void VideoInput::release(){

}



