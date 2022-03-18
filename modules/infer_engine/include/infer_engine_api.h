#ifndef __INFER_ENGINE_API_H__
#define __INFER_ENGINE_API_H__

#include "ie_engine.hpp"

/**
 * 功能描述: 工厂模式：生成 engine 
 * 
 * 输入参数： param 
 * 输出参数： errcode 
 * 返回参数：IE_Engine* 
 */
IE_Engine* create_engine(const char* model_path, const IE_Param& param, int* errcode);

/**
 * 功能描述: 运行引擎
 * 
 * 输入参数： engine 
 * 输出参数： outputs 
 * 返回参数：int 
 */
int run_engine(IE_Engine* engine, const cv::Mat& input_img, std::vector<IE_Blob>& outputs);

/**
 * 功能描述: 销毁引擎
 * 
 * 输入参数： engine 
 * 返回参数：
 */
void destroy_engine(IE_Engine* engine);

#endif // __INFER_ENGINE_API_H__