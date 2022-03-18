#if defined (__PLATFORM_HISI__)

#ifndef __ENGINE_NNIE_HPP__
#define __ENGINE_NNIE_HPP__

#include "ie_engine.hpp"

#include <vector>
#include <string>
#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_dsp.h"
#include "hi_nnie.h"
#include "mpi_nnie.h"

class EngineNNIE : public IE_Engine
{
public:
    EngineNNIE(const IE_Param& param):
        IE_Engine(param)
    {
        m_engine_type = ENGINE_TYPE_NNIE;
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
     * 功能描述: 从文件读取模型s
     * 
     * 输入参数： model_path 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE read_model_from_file(const char* model_path);

    /**
     * 功能描述: 获取 input size 
     * 
     * 输入参数： p_stModel 
     */
    void get_input_size(const SVP_NNIE_MODEL_S& p_stModel);

    /**
     * 功能描述: 分配blob内存
     * 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE malloc_blob_mem(void);

    /**
     * 功能描述: 释放blob内存
     * 
     */
    void free_blob_mem(void);

    /**
     * 功能描述: 分配任务内存
     * 
     * 返回参数：IE_ERR_CODE 
     */
    IE_ERR_CODE malloc_task_mem(void);

    /**
     * 功能描述: 释放任务内存
     * 
     */
    void free_task_mem(void);

private:
    bool m_is_tb_added;                 // Task buffer记录在案标志

    SVP_NNIE_MODEL_S m_model;           // NNIE模型
    int m_seg_num;                      // 网络分段数

    SVP_SRC_MEM_INFO_S m_model_buf;     // 模型Buffer
    SVP_SRC_MEM_INFO_S m_task_buf;      // 任务Buffer
    SVP_SRC_MEM_INFO_S m_tmp_buf;       // 辅助Buffer

    SVP_BLOB_S m_src_blob;              // 输入层数据
    SVP_BLOB_S* m_dst_blobs;            // 输出层数据

    SVP_NNIE_FORWARD_CTRL_S m_fw_ctrl;  // 前向控制信息
};

#endif // __ENGINE_NNIE_HPP__

#endif // PLATFORM GUARD