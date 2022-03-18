
#if defined (__PLATFORM_X86__)

#ifndef __IE_ENGINE_TENGINE_HPP__
#define __IE_ENGINE_TENGINE_HPP__

#include "ie_engine.hpp"

#include "tengine/c_api.h"

class EngineTengine : public IE_Engine
{
public:
    EngineTengine(const IE_Param& param):
        IE_Engine(param)
    {
        m_engine_type = ENGINE_TYPE_TENGINE;
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
    static bool g_is_init;                  // 全局初始化
    graph_t m_graph;                        // 模型
    tensor_t m_input_tensor;                // 输入Tensor
    std::vector<tensor_t> m_output_tensors; // 输出Tensor
};

#endif // __IE_ENGINE_TENGINE_HPP__

#endif // PLATFORM GUARD