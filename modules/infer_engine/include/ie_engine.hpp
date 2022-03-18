#ifndef __IE_ENGINE_HPP__
#define __IE_ENGINE_HPP__

#include <map>
#include "opencv2/core.hpp"

#include "ie_errcode.hpp"
#include "ie_blob.hpp"
#include "ie_params.hpp"

/**
 * 功能描述: 引擎类型
 * 
 */
enum ENGINE_TYPE_E
{
    ENGINE_TYPE_TENGINE,
    ENGINE_TYPE_NNIE,
    ENGINE_TYPE_RKNN
};

/**
 * 功能描述: 推理引擎
 * 
 */
class IE_Engine
{
public:
    IE_Engine(const IE_Param& param):
        m_infer_param(param)
    {

    }

    virtual ~IE_Engine() 
    {
        release();
    }

    /**
     * 功能描述: 初始化模型
     * 
     * 输入参数： model_path 模型路径
     * 返回参数：IE_ERR_CODE 
     */
    virtual IE_ERR_CODE init(const char* model_path) = 0;

    /**
     * 功能描述: 运行推理
     * 
     * 输入参数： input_img         输入图像
     * 返回参数：IE_ERR_CODE 
     */
    virtual IE_ERR_CODE run(const cv::Mat& input_img) = 0;

    /**
     * 功能描述: 释放模型
     * 
     * 返回参数：IE_ERR_CODE 
     */
    virtual IE_ERR_CODE release(void)
    {
        return IE_SUCCESS;
    }

    /**
     * 功能描述: 引擎类型
     * 
     * 返回参数：int 
     */
    virtual int type()
    {
        return m_engine_type;
    }

    /**
     * 功能描述: 获取输出
     * 
     * 返回参数：std::vector<IE_Blob> 
     */
    virtual std::vector<IE_Blob> get_outputs()
    {
        return m_output_blobs;
    }

    /**
     * 功能描述: 获取参数
     * 
     * 返回参数：IE_Param& 
     */
    IE_Param param()
    {
        return m_infer_param;
    }

    /**
     * 功能描述: 获取输入尺寸
     * 
     * 返回参数：IE_Shape 
     */
    IE_Shape input_shape() const
    {
        return m_input_shape;
    }

    /**
     * 功能描述: 获取输出尺寸
     * 
     * 返回参数：std::vector<IE_Shape> 
     */
    std::vector<IE_Shape> output_shapes() const
    {
        return m_output_shapes;
    }

protected:
    /**
     * 功能描述: 前处理
     * 
     * 输入参数： input_img 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE preprocess(const cv::Mat& input_img);

protected:
    // 输出Tensor
    std::vector<IE_Blob> m_output_blobs;
    // 输入数量
    int m_input_num;
    // 输出数量
    int m_output_num;
    // 输入Shape
    IE_Shape m_input_shape;
    // 输出Shape
    std::vector<IE_Shape> m_output_shapes;
    // 前后处理参数
    IE_Param m_infer_param;
    // 前处理后的图像
    cv::Mat m_input_blob;
    // 引擎类型
    int m_engine_type;
};

#endif // __IE_ENGINE_HPP__