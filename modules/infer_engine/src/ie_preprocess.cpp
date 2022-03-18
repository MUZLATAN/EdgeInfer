
#include "ie_preprocess.hpp"

#include "opencv2/imgproc.hpp"

/**
 * 功能描述: HWC2CHW
 * 
 * 输入参数： image 
 * 返回参数：cv::Mat 
 */
cv::Mat hwc2chw(const cv::Mat &image)
{
    std::vector<cv::Mat> rgb_images;
    cv::split(image, rgb_images);

    // Stretch one-channel images to vector
    cv::Mat m_flat_r = rgb_images[0].reshape(1,1);
    cv::Mat m_flat_g = rgb_images[1].reshape(1,1);
    cv::Mat m_flat_b = rgb_images[2].reshape(1,1);

    // Now we can rearrange channels if need
    cv::Mat matArray[] = { m_flat_r, m_flat_g, m_flat_b};
    
    cv::Mat flat_image;
    // Concatenate three vectors to one
    cv::hconcat( matArray, 3, flat_image );
    return flat_image;
}

/**
 * 功能描述:    前处理
 * 
 * 输入参数： src               输入图像
 * 输出参数： dst               输出图像
 * 输入参数： dst_w             目标宽度
 * 输入参数： dst_h             目标高度
 * 输入参数： rgb_order         RGB通道顺序
 * 输入参数： format            图像格式，NCHW/NHWC
 * 输入参数： mean_values       均值数组，长度为3
 * 输入参数： std_values        方差数组，长度为3
 * 返回参数：IE_ERR_CODE
 */
IE_ERR_CODE ie_preprocess(const cv::Mat& src, cv::Mat& dst, 
                                int dst_w, int dst_h, 
                                IE_PREPROCESS_RGB_ORDER rgb_order, 
                                IE_PREPROCESS_FORMAT format, 
                                std::vector<float> mean_values, 
                                std::vector<float> std_values)
{
    // 检查参数
    if (src.empty())
    {
        return IE_PREPROCESS_IMAGE_EMPTY;
    }

    if (src.channels() != 1 && src.channels() != 3)
    {
        return IE_PREPROCESS_IMAGE_CHANNEL;
    }

    if (dst_w <= 0 || dst_h <= 0)
    {
        return IE_PREPROCESS_DST_SIZE;
    }

    // resize
    cv::resize(src, dst, cv::Size(dst_w, dst_h));

#ifndef __PLATFORM_HISI__
    // 通道反转
    if (IE_PREPROCESS_RGB_ORDER::RGB == rgb_order
         && src.channels() == 3)
    {
        cv::cvtColor(dst, dst, CV_BGR2RGB);
    }
#endif

    // 格式转换
    if (IE_PREPROCESS_FORMAT::NCHW == format
        && src.channels() == 3)
    {
        dst = hwc2chw(dst);
    }
    else
    {
        dst = dst.reshape(1, 1);
    }

#ifndef __PLATFORM_HISI__
    // 减均值除方差
    if (!mean_values.empty() || !std_values.empty())
    {
        dst.convertTo(dst, CV_32FC1);
    }

#ifdef __PLATFORM_RK__
    if (dst.type() != CV_32FC1)
    {
        dst.convertTo(dst, CV_32FC1);
    }
#endif
    
    if (!mean_values.empty())
    {
        float* data = (float*)dst.data;
        for (int i = 0; i < 3; i++)
        {
            for (int n = 0; n < dst_w * dst_h; n++, data++)
            {
                *data -= mean_values[i];
            }
        }
    }
    
    if (!std_values.empty())
    {
        float* data = (float*)dst.data;
        for (int i = 0; i < 3; i++)
        {
            for (int n = 0; n < dst_w * dst_h; n++, data++)
            {
                *data /= std_values[i];
            }
        }
    }
#endif

    return IE_SUCCESS;
}                                    