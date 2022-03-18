
#include "infer_engine_api.h"

#include "engine/engine_tengine.hpp"
#include "engine/engine_nnie.hpp"
#include "engine/engine_rknn.hpp"

/**
 * 功能描述: 工厂模式：生成 engine 
 * 
 * 输入参数： param 
 * 输出参数： errcode 
 * 返回参数：IE_Engine* 
 */
IE_Engine* create_engine(const char* model_path, const IE_Param& param, int* errcode)
{
    IE_Engine* engine;
    #if defined (__PLATFORM_X86__) || (__PLATFORM_RK__)
        engine = new EngineTengine(param);
    #elif defined (__PLATFORM_HISI__)
        engine = new EngineNNIE(param);
    #elif defined (__PLATFORM_RV__) || (__PLATFORM_RKPRO__)
        engine = new EngineRKNN(param);
    #else
        return nullptr;
    #endif

    IE_ERR_CODE ret = engine->init(model_path);
    if (NULL != errcode)
    {
        *errcode = ret;
    }
    return engine;
}

/**
 * 功能描述: 运行引擎
 * 
 * 输入参数： engine 
 * 输出参数： outputs 
 * 返回参数：int 
 */
int run_engine(IE_Engine* engine, const cv::Mat& input_img, std::vector<IE_Blob>& outputs)
{
    if (nullptr == engine)
    {
        return -1;
    }

    IE_ERR_CODE errcode = engine->run(input_img);
    if (IE_SUCCESS != errcode)
    {
        return (int)errcode;
    }
    

    outputs = engine->get_outputs();
    return IE_SUCCESS;
}

/**
 * 功能描述: 销毁引擎
 * 
 * 输入参数： engine 
 * 返回参数：
 */
void destroy_engine(IE_Engine* engine)
{
    delete engine;
}