#ifndef __IE_PREPROCESS_HPP__
#define __IE_PREPROCESS_HPP__

#include <map>

#include "opencv2/core.hpp"
#include "ie_errcode.hpp"
#include "ie_params.hpp"

/**
 * 功能描述:    前处理
 * 
 * 输入参数： src               输入图像
 * 输出参数： dst               输出图像
 * 输入参数： dst_w             目标宽度
 * 输入参数： dst_h             目标高度
 * 输入参数： rgb_order         RGB通道顺序
 * 输入参数： format            图像格式，NCHW/NHWC
 * 输入参数： mean_values       均值数组，长度为3
 * 输入参数： std_values        方差数组，长度为3
 * 返回参数：IE_PREPROCESS_ERR_CODE 
 */
IE_ERR_CODE ie_preprocess(const cv::Mat& src, cv::Mat& dst, 
                            int dst_w, int dst_h, 
                            IE_PREPROCESS_RGB_ORDER rgb_order, 
                            IE_PREPROCESS_FORMAT format, 
                            std::vector<float> mean_values, 
                            std::vector<float> std_values);

#endif