
#if defined (__PLATFORM_RV__) ||(__PLATFORM_RKPRO__)

#ifndef __IE_ENGINE_RKNN_HPP__
#define __IE_ENGINE_RKNN_HPP__

#include "ie_engine.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rknn_api.h"


class EngineRKNN : public IE_Engine
{
public:
    EngineRKNN(const IE_Param& param):
        IE_Engine(param)
    {
        m_engine_type = ENGINE_TYPE_RKNN;
    }

    /**
     * 功能描述: 初始化模型
     * 
     * 输入参数： model_path 模型路径
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE init(const char* model_path);

    /**
     * 功能描述: 运行推理
     * 
     * 输入参数： input_img         输入图像
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE run(const cv::Mat& input_img);

    /**
     * 功能描述: 释放模型
     * 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE release(void);

private:
    /**
     * 功能描述: 从文件读模型
     * 
     * 输入参数： model_path 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE read_model_from_file(const char* model_path);

    /**
     * 功能描述: 获取输入属性
     * 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE get_input_attr();

    /**
     * 功能描述: 获取输出属性
     * 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE get_output_attr();

private:
    rknn_context m_ctx;
    rknn_output* m_outputs;
};

#endif // __IE_ENGINE_RKNN_HPP__

#endif // PLATFORM GUARD