#ifndef __IE_ERRCODE_HPP__
#define __IE_ERRCODE_HPP__

#include <map>

/**
 * 功能描述: 错误码
 * 
 */
enum IE_ERR_CODE
{
    IE_SUCCESS                      =       0,

    // 初始化
    IE_INIT_MODEL_PATH_NULL,

    // 前处理
    IE_PREPROCESS_IMAGE_EMPTY,
    IE_PREPROCESS_DST_SIZE,
    IE_PREPROCESS_IMAGE_CHANNEL,
    IE_PREPROCESS_RESIZE_FAIL,

    // 初始化: TENGINE
    IE_INIT_TENGINE_INIT_FAIL,
    IE_INIT_TENGINE_CREATE_GRAPH_FAIL,
    IE_INIT_TENGINE_GET_INPUT_TENSOR_FAIL,
    IE_INIT_TENGINE_GET_OUTPUT_TENSOR_FAIL,
    IE_INIT_TENGINE_GET_INPUT_TENSOR_SHAPE_FAIL,
    IE_INIT_TENGINE_GET_OUTPUT_TENSOR_SHAPE_FAIL,

    // 初始化: NNIE
    IE_INIT_NNIE_OPEN_FILE_FAIL,
    IE_INIT_NNIE_MALLOC_MODEL_BUFFER_FAIL,
    IE_INIT_NNIE_LOAD_MODEL_FAIL,
    IE_INIT_NNIE_MALLOC_BLOB_FAIL,
    IE_INIT_NNIE_FLUSH_CACHE_FAIL,
    IE_INIT_NNIE_GET_TASK_BUF_FAIL,
    IE_INIT_NNIE_MALLOC_BUF_FAIL,

    // 初始化: RKNN
    IE_INIT_RKNN_OPEN_FILE_FAIL,
    IE_INIT_RKNN_READ_MODEL_FAIL,
    IE_INIT_RKNN_INIT_MODEL_FAIL,
    IE_INIT_RKNN_QUERY_INPUT_FAIL,
    IE_INIT_RKNN_QUERY_INOUT_FAIL,
    IE_INIT_RKNN_QUERY_OUTPUT_FAIL,
    IE_INIT_RKNN_SET_INPUT_FAIL,

    // 推理: TENGINE
    IE_INFER_TENGINE_SET_INPUT_BUFFER_FAIL,
    IE_INFER_TENGINE_RUN_GRAPH_FAIL,

    // 推理: NNIE
    IE_INFER_NNIE_FORWARD_FAIL,
    IE_INFER_NNIE_FORWARD_TIMEOUT,
    IE_INFER_NNIE_QUERY_FAIL,

    // 推理: RKNN
    IE_INFER_RKNN_RUN_FAIL,
    IE_INFER_RKNN_GET_OUTPUT_FAIL,

    // 释放: RKNN
    IE_RELEASE_RKNN_RELEASE_OUTPUT_FAIL,

    // 释放：TENGINE
    IE_RELEASE_TENGINE_DESTROY_GRAPH_FAIL
};

/**
 * 功能描述: 前处理错误码与字符串对应
 * 
 */
typedef std::map<int, const char*> IE_ERR_CODE_TABLE_TYPE;
const IE_ERR_CODE_TABLE_TYPE IE_ERR_TABLE  {
    { IE_SUCCESS, "success" },

    // 前处理
    { IE_PREPROCESS_IMAGE_EMPTY,            "preprocess failed! image is empty!!!"         }, 
    { IE_PREPROCESS_DST_SIZE,               "preprocess failed! dst_w <= 0 || dst_h <= 0"  },
    { IE_PREPROCESS_IMAGE_CHANNEL,          "preprocess failed! image channel incorrect"   },
    { IE_PREPROCESS_RESIZE_FAIL,            "preprocess failed! resize failed"             },
    
    // 初始化
    { IE_INIT_MODEL_PATH_NULL,                      "init failed! model path is null"},

    // 初始化: TENGINE
    { IE_INIT_TENGINE_INIT_FAIL,                    "tengine init failed! init_tengine() failed!"},
    { IE_INIT_TENGINE_CREATE_GRAPH_FAIL,            "tengine init failed! create_graph failed!"},
    { IE_INIT_TENGINE_GET_INPUT_TENSOR_FAIL,        "tengine init failed! get_input_tensor failed!"},
    { IE_INIT_TENGINE_GET_OUTPUT_TENSOR_FAIL,       "tengine init failed! get_output_tensor failed!"},
    { IE_INIT_TENGINE_GET_INPUT_TENSOR_SHAPE_FAIL,  "tengine init failed! get input tensor shape failed!"},
    { IE_INIT_TENGINE_GET_OUTPUT_TENSOR_SHAPE_FAIL, "tengine init failed! get output tensor shape failed!"},

    // 初始化: NNIE
    { IE_INIT_NNIE_OPEN_FILE_FAIL,                  "nnie init failed! open file failed!"   },
    { IE_INIT_NNIE_MALLOC_MODEL_BUFFER_FAIL,        "nnie init failed! malloc model buffer failed!"   },            
    { IE_INIT_NNIE_LOAD_MODEL_FAIL,                 "nnie init failed! load model failed!"   },
    { IE_INIT_NNIE_MALLOC_BLOB_FAIL,                "nnie init failed! malloc blob failed!"   },    
    { IE_INIT_NNIE_FLUSH_CACHE_FAIL,                "nnie init failed! flush cache failed!"   },    
    { IE_INIT_NNIE_GET_TASK_BUF_FAIL,               "nnie init failed! get task buffer failed!"   },    
    { IE_INIT_NNIE_MALLOC_BUF_FAIL,                 "nnie init failed! malloc buffer failed!"   },

    // 初始化: RKNN
    { IE_INIT_RKNN_OPEN_FILE_FAIL,                  "rknn init failed! open file failed!" },
    { IE_INIT_RKNN_READ_MODEL_FAIL,                 "rknn init failed! read model failed!" },
    { IE_INIT_RKNN_INIT_MODEL_FAIL,                 "rknn init failed! init model failed!" },
    { IE_INIT_RKNN_QUERY_INPUT_FAIL,                "rknn init failed! query init failed!" },
    { IE_INIT_RKNN_QUERY_INOUT_FAIL,                "rknn init failed! query inout failed!" },
    { IE_INIT_RKNN_QUERY_OUTPUT_FAIL,               "rknn init failed! query output failed!" },
    { IE_INIT_RKNN_SET_INPUT_FAIL,                  "rknn init failed! set input failed!" },

    // 推理
    { IE_INFER_TENGINE_SET_INPUT_BUFFER_FAIL,       "tengine inference failed! set_input_buffer failed!" },
    { IE_INFER_TENGINE_RUN_GRAPH_FAIL,              "tengine inference failed! run_graph failed!"},

    // 释放：RKNN
    { IE_RELEASE_RKNN_RELEASE_OUTPUT_FAIL,          "rknn release failed! rknn_release_output failed!"},
    // 释放：TENGINE
    { IE_RELEASE_TENGINE_DESTROY_GRAPH_FAIL,        "tengine release failed! destroy_graph failed!"},
};

/**
 * 功能描述: 错误码转字符串
 * 
 * 输入参数： code 
 * 返回参数：const char* 
 */
const char* IE_err2str(int code);

#endif // __IE_ERRCODE_HPP__