#ifndef __IE_PARAMS_HPP__
#define __IE_PARAMS_HPP__

#include <vector>

/**
 * 功能描述: RGB顺序
 * 
 */
enum class IE_PREPROCESS_RGB_ORDER
{
    BGR,
    RGB
};

/**
 * 功能描述: 输入格式
 * 
 */
enum class IE_PREPROCESS_FORMAT
{
    NCHW,
    NHWC
};

// 锚框
typedef std::vector<std::vector<std::vector<float> > >      IE_PriorBoxParam;

// 输出图大小
typedef std::vector<std::vector<float> >        IE_FeatureMapShape;

/**
 * 功能描述: NMS类型
 * 
 */
enum class NMS_TYPE_E
{
    hard,
    blending
};

/**
 * 功能描述: 前处理参数
 * 
 */
class IE_PreParam
{
public:
    IE_PreParam(IE_PREPROCESS_RGB_ORDER rgb_order_=IE_PREPROCESS_RGB_ORDER::RGB,
                IE_PREPROCESS_FORMAT format_=IE_PREPROCESS_FORMAT::NCHW,
                std::vector<float> mean_vals_=std::vector<float>(), 
                std::vector<float> std_vals_=std::vector<float>()):
        rgb_order(rgb_order_),
        format(format_),
        mean_values(mean_vals_),
        std_values(std_vals_)
    {

    }

    IE_PreParam(const IE_PreParam& other):
        rgb_order(other.rgb_order),
        format(other.format),
        mean_values(other.mean_values),
        std_values(other.std_values)
    {

    }
    
public:
    IE_PREPROCESS_RGB_ORDER rgb_order;
    IE_PREPROCESS_FORMAT format;
    std::vector<float> mean_values;
    std::vector<float> std_values;
};

/**
 * 功能描述: 后处理参数
 * 
 */
class IE_PostParam
{
public:
    IE_PostParam(const IE_PriorBoxParam& pb_param_ = {}):
        pb_param(pb_param_),
        nms_thresh(0.3f),
        nms_type(NMS_TYPE_E::hard),
        center_var(0.1f),
        size_var(0.2f)
    {

    }

    IE_PostParam(const IE_PostParam& other):
        pb_param(other.pb_param),
        nms_thresh(other.nms_thresh),
        nms_type(other.nms_type),
        center_var(other.center_var),
        size_var(other.size_var)
    {

    }

public:
    IE_PriorBoxParam pb_param;
    IE_FeatureMapShape fm_shapes;
    int origin_w, origin_h;
    int net_w, net_h;
    float score_thresh;
    float nms_thresh;
    NMS_TYPE_E nms_type;
    float center_var, size_var;
};

/**
 * 功能描述: 推理参数，包含了前后处理
 * 
 */
class IE_Param
{
public:
    IE_Param(const IE_PreParam& pre_param_ = IE_PreParam(), const IE_PostParam& post_param_ = IE_PostParam()):
        pre_param(pre_param_),
        post_param(post_param_)
    {

    }

    IE_Param(const IE_Param& other):
        pre_param(other.pre_param),
        post_param(other.post_param)
    {
        
    }

public:
    IE_PreParam pre_param;
    IE_PostParam post_param;
};

#endif // __IE_PARAMS_HPP__