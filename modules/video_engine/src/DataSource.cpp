#include "DataSource.h"

bool RtspData::init(const std::string& rtsp_addr){

    m_rtsp_addr = rtsp_addr;

    m_rtsp.init(rtsp_addr);


    #if defined (__PLATFORM_RV__) || (__PLATFORM_RKPRO__) ||(__PLATFORM_RK__)
        decode_ptr = std::make_shared<RKMPPDecode>();
        printf("m_data_source is Rtsp Data\n");
    #else
        printf("no decod module\n");
        return false;
    #endif

    decode_ptr->set_codec_type(m_rtsp.m_code_type);

    decode_ptr->init();
    
    decode_ptr->set_cb(std::bind(&RtspData::data_callback,this, std::placeholders::_1));
}

bool RtspData::read(cv::Mat& cv_img){
    if (!m_rtsp.read_packet(decode_ptr, cv_img)){
        // m_rtsp.release();
        // m_rtsp.init(m_rtsp_addr);


        return false;
    }

    if (m_rtsp.is_videoidx())
    {
        decode_ptr->decode(m_rtsp.m_packet,cv_img);
    }
    m_rtsp.release_packet();
    return true;
}


bool RtspData::data_callback(cv::Mat& image){
    // std::cout<<image.cols<<"  ----------------- "<<image.rows<<std::endl;
}

