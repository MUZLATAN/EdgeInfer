#include "ie_engine.hpp"
#include "ie_preprocess.hpp"

/**
 * 功能描述: 前处理
 * 
 * 输入参数： input_img 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE IE_Engine::preprocess(const cv::Mat& input_img)
{
    return ie_preprocess(input_img, m_input_blob, 
                m_input_shape.w, m_input_shape.h,
                m_infer_param.pre_param.rgb_order,
                m_infer_param.pre_param.format,
                m_infer_param.pre_param.mean_values,
                m_infer_param.pre_param.std_values);
}