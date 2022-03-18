#if defined (__PLATFORM_X86__) || (__PLATFORM_RK__)

#include "engine/engine_tengine.hpp"

// 全局初始化标志
bool EngineTengine::g_is_init = false;

/**
 * 功能描述: 初始化模型
 * 
 * 输入参数： model_path 模型路径
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineTengine::init(const char* model_path)
{
    // 全局初始化
    if (!g_is_init)
    {
        if (0 != init_tengine())
        {
            g_is_init = false;
            return IE_INIT_TENGINE_INIT_FAIL;
        }
        else
        {
            g_is_init = true;
        }
    }

    // 检查参数
    if (model_path == NULL)
    {
        return IE_INIT_MODEL_PATH_NULL;
    }

    // 建图
    m_graph = create_graph(NULL, "tengine", model_path);
    if (NULL == m_graph)
    {
        return IE_INIT_TENGINE_CREATE_GRAPH_FAIL;
    }

    // 获取输入Tensor数量
    m_input_num = get_graph_input_node_number(m_graph);

    // 获取输入Tensor
    m_input_tensor = get_graph_input_tensor(m_graph, 0, 0);
    if (NULL == m_input_tensor)
    {
        return IE_INIT_TENGINE_GET_INPUT_TENSOR_FAIL;
    }

    // 获取输入Tensor shape
    int dims[4];
    int n_dims = 4;
    if (n_dims != get_tensor_shape(m_input_tensor, dims, n_dims))
    {
        return IE_INIT_TENGINE_GET_INPUT_TENSOR_SHAPE_FAIL;
    }
    else
    {
        m_input_shape = IE_Shape(dims[1], dims[2], dims[3]);
    }

    // 获取输出Tensor数量
    m_output_num = get_graph_output_node_number(m_graph);
    m_output_tensors.resize(m_output_num);

    for (int i = 0; i < m_output_num; i++)
    {
        tensor_t output_tensor = get_graph_output_tensor(m_graph, i, 0);
        if (NULL == output_tensor)
        {
            return IE_INIT_TENGINE_GET_OUTPUT_TENSOR_FAIL;
        }

        int dims[4];
        int n_dims = 4;
        if (n_dims != get_tensor_shape(output_tensor, dims, n_dims))
        {
            return IE_INIT_TENGINE_GET_OUTPUT_TENSOR_SHAPE_FAIL;
        }

        const char* name = get_tensor_name(output_tensor);

        m_output_shapes.emplace_back(dims[1], dims[2], dims[3]);
        m_output_blobs.emplace_back(name, dims[1], dims[2], dims[3]);
        m_output_tensors[i] = get_graph_output_tensor(m_graph, i, 0);
    }

    return IE_SUCCESS;
}

/**
 * 功能描述: 运行推理
 * 
 * 输入参数： input_img         输入图像
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineTengine::run(const cv::Mat& input_img)
{
    IE_ERR_CODE err;
    // 前处理
    err = preprocess(input_img);
    if (IE_SUCCESS != err)
    {
        return err;
    }

    // 放入数据
    int input_data_size = m_input_shape.c * m_input_shape.h * m_input_shape.w * sizeof(float);
    if (set_tensor_buffer(m_input_tensor, m_input_blob.data, input_data_size) < 0)
    {
        return IE_INFER_TENGINE_SET_INPUT_BUFFER_FAIL;
    }

    // 推理
    if (run_graph(m_graph, 1) < 0)
    {
        return IE_INFER_TENGINE_RUN_GRAPH_FAIL;
    }

    // 拷贝输出数据
    for (int i = 0; i < m_output_num; i++)
    {
        int output_data_size = m_output_shapes[i].c * m_output_shapes[i].h * m_output_shapes[i].w * sizeof(float);
        void* output_data = get_tensor_buffer(m_output_tensors[i]);
        memcpy(m_output_blobs[i].data, output_data, output_data_size);
    }

    return IE_SUCCESS;
}

/**
 * 功能描述: 释放模型
 * 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineTengine::release(void)
{
    if (destroy_graph(m_graph) < 0)
    {
        return IE_RELEASE_TENGINE_DESTROY_GRAPH_FAIL;
    }
    return IE_SUCCESS;
}

#endif // PLATFORM GUARD