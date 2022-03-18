
#if defined (__PLATFORM_RV__)|| (__PLATFORM_RKPRO__)

#include "engine/engine_rknn.hpp"

/**
 * 功能描述: 从文件读模型
 * 
 * 输入参数： model_path 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineRKNN::read_model_from_file(const char* model_path)
{
    // 1. 获取模型指针和大小
    FILE *fp = fopen(model_path, "rb");
    if (fp == NULL)
    {
        return IE_INIT_RKNN_OPEN_FILE_FAIL;
    }
    fseek(fp, 0, SEEK_END);
    int model_len = ftell(fp);
    unsigned char *model = (unsigned char *)malloc(model_len);
    fseek(fp, 0, SEEK_SET);
    if (model_len != fread(model, 1, model_len, fp))
    {
        free(model);
        return IE_INIT_RKNN_READ_MODEL_FAIL;
    }

    if (fp)
    {
        fclose(fp);
    }
    printf("read model\n");
    
    // 2. 加载模型
    int ret = rknn_init(&m_ctx, model, model_len, RKNN_FLAG_PRIOR_MEDIUM); // | RKNN_FLAG_COLLECT_PERF_MASK);
    if (ret < 0)
    {
        return IE_INIT_RKNN_INIT_MODEL_FAIL;
    }
    printf("load model successfully\n");

    return IE_SUCCESS;
}

/**
 * 功能描述: 分配输入内存
 * 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineRKNN::get_input_attr()
{
    //  获取输入属性
    rknn_tensor_attr input_attr;
    input_attr.index = 0;
    int ret = rknn_query(m_ctx, RKNN_QUERY_INPUT_ATTR, &input_attr, sizeof(input_attr));
    if (ret < 0)
    {
        return IE_INIT_RKNN_QUERY_INPUT_FAIL;
    }

    // 获取输入Tensor shape
    printf("dim 1 2 3 are: %d %d %d \n", input_attr.dims[2], input_attr.dims[1], input_attr.dims[0]);
    m_input_shape = IE_Shape(input_attr.dims[2], input_attr.dims[1], input_attr.dims[0]);    

    return IE_SUCCESS;
}

/**
 * 功能描述: 获取输出属性
 * 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineRKNN::get_output_attr()
{
    rknn_input_output_num io_num;
    int ret = rknn_query(m_ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret < 0)
    {
        return IE_INIT_RKNN_QUERY_INOUT_FAIL;
    }

    m_input_num = io_num.n_input;
    m_output_num = io_num.n_output;
    m_outputs = (rknn_output*)malloc(sizeof(rknn_output) * m_output_num);

    for (int i = 0; i < m_output_num; i++)
    {
        rknn_tensor_attr output_attr;
        output_attr.index = i;
        ret = rknn_query(m_ctx, RKNN_QUERY_OUTPUT_ATTR, &output_attr, sizeof(output_attr));
        if (ret < 0)
        {
            return IE_INIT_RKNN_QUERY_OUTPUT_FAIL;
        }
        
        // 计算size
        int size = 1;
        for (int k = 0; k < output_attr.n_dims; k++)
        {
            size *= output_attr.dims[k];
        }

        m_outputs[i].index = i;
        m_outputs[i].want_float = 1;
        m_outputs[i].is_prealloc = 1;
        m_outputs[i].buf = malloc(sizeof(float) * size);
        m_outputs[i].size = sizeof(float) * size;

        m_output_blobs.emplace_back(output_attr.name, output_attr.dims[2], output_attr.dims[1], output_attr.dims[0]);
        m_output_shapes.emplace_back(output_attr.dims[2], output_attr.dims[1], output_attr.dims[0]);
    }
    return IE_SUCCESS;
}

/**
 * 功能描述: 初始化模型
 * 
 * 输入参数： model_path 模型路径
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineRKNN::init(const char* model_path)
{
    IE_ERR_CODE err;
    err = read_model_from_file(model_path);
    if (IE_SUCCESS != err)
    {
        return err;
    }

    err = get_input_attr();
    if (IE_SUCCESS != err)
    {
        return err;
    }

    err = get_output_attr();
    if (IE_SUCCESS != err)
    {
        return err;
    }

    return IE_SUCCESS;
}

/**
 * 功能描述: 运行推理
 * 
 * 输入参数： input_img         输入图像
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineRKNN::run(const cv::Mat& input_img)
{
    IE_ERR_CODE err;
    // 前处理
    err = preprocess(input_img);
    if (IE_SUCCESS != err)
    {
        return err;
    }

    int ret = 0;

    // 设置输入数据
    int elem_size = m_input_blob.type() == CV_8UC1 ? 1 : 4;
    rknn_input inputs[1];
    inputs[0].index = 0;
    inputs[0].buf = m_input_blob.data;
    inputs[0].size = elem_size * m_input_shape.c * m_input_shape.h * m_input_shape.w;
    inputs[0].pass_through = 1;
    // pass_through = 0时需要设置以下值，RKNN会对输入数据做转换
    inputs[0].type = m_input_blob.type() == CV_8UC1 ? RKNN_TENSOR_UINT8 : RKNN_TENSOR_FLOAT32;
    inputs[0].fmt = RKNN_TENSOR_NCHW;
    ret = rknn_inputs_set(m_ctx, 1, inputs);
    if (ret < 0)
    {
        return IE_INIT_RKNN_SET_INPUT_FAIL;
    }

    

    // 推理
    ret = rknn_run(m_ctx, NULL);
    if (ret < 0)
    {
        return IE_INFER_RKNN_RUN_FAIL;
    }


    // 获取输出
    ret = rknn_outputs_get(m_ctx, m_output_num, m_outputs, NULL);
    if (ret < 0)
    {
        return IE_INFER_RKNN_GET_OUTPUT_FAIL;
    }
    // return IE_SUCCESS;

    // 拷贝输出
    for (int i = 0; i < m_output_num; i++)
    {
        memcpy(m_output_blobs[i].data, m_outputs[i].buf, m_outputs[i].size);
    }

    return IE_SUCCESS;
}

/**
 * 功能描述: 释放模型
 * 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineRKNN::release(void)
{
    IE_ERR_CODE err;
    if (rknn_outputs_release(m_ctx, m_output_num, m_outputs) < 0)
    {
        return IE_RELEASE_RKNN_RELEASE_OUTPUT_FAIL;
    }

    rknn_destroy(m_ctx);
    return IE_SUCCESS;
}


#endif // PLATFORM GUARD