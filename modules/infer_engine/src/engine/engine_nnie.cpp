#if defined (__PLATFORM_HISI__)

#include "engine/engine_nnie.hpp"

#include <unistd.h>

// easy free
#define IE_HISI_MMZ_Free(phy, vir)\
do{\
	if ((0 != (phy)) && (0 != (vir)))\
	{\
		HI_MPI_SYS_MmzFree((phy),(void*)(vir));\
		(phy) = 0;\
		(vir) = 0;\
	}\
}while(0)

#define IE_HISI_ALIGN_BYTES      16           // 字节对齐数目
#define IE_HISI_QUANT            4096.f       // 量化基准

/******** 通用函数 ********/
// 字节对齐
HI_U32 IE_HISI_Align(HI_U32 pu32Size, HI_U16 pu16Align = IE_HISI_ALIGN_BYTES)
{
	HI_U32 u32Stride = pu32Size + (pu16Align - pu32Size % pu16Align) % pu16Align;
	return u32Stride;
}

// 分配MMZ内存
HI_S32 IE_HISI_MMZ_Malloc(HI_CHAR *pszMmb, HI_CHAR *pszZone, HI_U64 *pu64PhyAddr, HI_VOID **ppvVirAddr, HI_U32 u32Size)
{
    HI_S32 s32Ret = HI_SUCCESS;
	s32Ret = HI_MPI_SYS_MmzAlloc(pu64PhyAddr, ppvVirAddr, pszMmb, pszZone, u32Size);
	return s32Ret;
}

// 分配可Cache的MMZ内存
HI_S32 IE_HISI_MMZ_MallocCached(HI_CHAR *pszMmb, HI_CHAR *pszZone, HI_U64 *pu64PhyAddr, HI_VOID **ppvVirAddr, HI_U32 u32Size)
{
    HI_S32 s32Ret = HI_SUCCESS;
	s32Ret = HI_MPI_SYS_MmzAlloc_Cached(pu64PhyAddr, ppvVirAddr, pszMmb, pszZone, u32Size);
	return s32Ret;
}

// 刷新Cache
HI_S32 IE_HISI_FlushCache(HI_U64 u64PhyAddr, HI_VOID *pvVirAddr, HI_U32 u32Size)
{
    HI_S32 s32Ret = HI_SUCCESS;
	s32Ret = HI_MPI_SYS_MmzFlushCache(u64PhyAddr, pvVirAddr,u32Size);
	return s32Ret;
}

// 反量化
void IE_HISI_Dequant(IE_Blob& dst_blob, const SVP_BLOB_S& src_blob)
{
    const int h = dst_blob.h;
    const int w = dst_blob.w;
    const int c = dst_blob.c;
    const int total_size = h * w * c;
    const int stride = src_blob.u32Stride / sizeof(float);
    bool continous = (stride == w);
    int* src_data = (int*)src_blob.u64VirAddr;
    float* dst_data = dst_blob.data;
    const int min_step = IE_HISI_ALIGN_BYTES / sizeof(float);

    // printf("name: %s  type: %d\n", dst_blob.name.c_str(), src_blob.enType);

    // 宽度小于min_step，naiive
    if (w < min_step)
    {
        // printf("dq\n");
        for (int i = 0; i < h * c; i++)
        {
            for (int k = 0; k < w; k++)
            {
                dst_data[k] = src_data[k] / IE_HISI_QUANT;
            }
            src_data += stride;
            dst_data += w;
        }
    }
    else
    {
#ifdef __ARM__ // armv7
        float32x4_t f = vdupq_n_f32(1.0f / IE_HISI_QUANT);
        int32x4_t a;
        float32x4_t b;
        int i = 0, k = 0;
        if (continous) // 宽度的内存排列刚好对齐，此时最快
        {
            // printf("continous\n");
            int neon_len = total_size / 32;
            // 整除32，汇编优化
            if (neon_len >= 1)
            {
                float dq = 1.0f / IE_HISI_QUANT;
            __asm__ volatile(
                "vdup.f32    q8, %[dq]             \n"
                "0:                                \n"
                // cache 128字节
                "pld         [%[src_data], #128]   \n"

                // 加载32个int
                "vld1.s32    {d0-d1},   [%[src_data]]! \n"
                "vld1.s32    {d2-d3},   [%[src_data]]! \n"
                "vld1.s32    {d4-d5},   [%[src_data]]! \n"
                "vld1.s32    {d6-d7},   [%[src_data]]! \n"
                "vld1.s32    {d8-d9},   [%[src_data]]! \n"
                "vld1.s32    {d10-d11}, [%[src_data]]! \n"
                "vld1.s32    {d12-d13}, [%[src_data]]! \n"
                "vld1.s32    {d14-d15}, [%[src_data]]! \n"

                // 转成float
                "vcvt.f32.s32 q0, q0 \n"
                "vcvt.f32.s32 q1, q1 \n"
                "vcvt.f32.s32 q2, q2 \n"
                "vcvt.f32.s32 q3, q3 \n"
                "vcvt.f32.s32 q4, q4 \n"
                "vcvt.f32.s32 q5, q5 \n"
                "vcvt.f32.s32 q6, q6 \n"
                "vcvt.f32.s32 q7, q7 \n"

                // src_data * dq
                "vmul.f32    q9,  q0, q8 \n"
                "vmul.f32    q10, q1, q8 \n"
                "vmul.f32    q11, q2, q8 \n"
                "vmul.f32    q12, q3, q8 \n"
                "vmul.f32    q13, q4, q8 \n"
                "vmul.f32    q14, q5, q8 \n"
                "vmul.f32    q15, q6, q8 \n"
                "vmul.f32    q0,  q7, q8 \n"

                "subs        %[neon_len], #1       \n"

                // store to dst_data
                "vst1.f32    {d18-d19},   [%[dst_data]]! \n"
                "vst1.f32    {d20-d21},   [%[dst_data]]! \n"
                "vst1.f32    {d22-d23},   [%[dst_data]]! \n"
                "vst1.f32    {d24-d25},   [%[dst_data]]! \n"
                "vst1.f32    {d26-d27},   [%[dst_data]]! \n"
                "vst1.f32    {d28-d29},   [%[dst_data]]! \n"
                "vst1.f32    {d30-d31},   [%[dst_data]]! \n"
                "vst1.f32    {d0-d1},     [%[dst_data]]! \n"

                "bgt    0b  \n"
                
                :[src_data]     "+r"(src_data),
                 [dst_data]     "+r"(dst_data),
                 [neon_len]     "+r"(neon_len)
                :[dq]           "r"(dq)
                :"cc", "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
                int left_num = total_size - total_size / 32 * 32;
                // printf("left num: %d\n", left_num);
                for (k = 0; k <= left_num - 4; k+=4)
                {
                    a = vld1q_s32(src_data);
                    b = vcvtq_f32_s32(a);
                    b = vmulq_f32(b, f);
                    vst1q_f32(dst_data, b);
                    src_data+=4;
                    dst_data+=4;
                }
                for (; left_num > 0 && k < left_num; k++, src_data++, dst_data++)
                {
                    *dst_data = *src_data / IE_HISI_QUANT;
                }
            }
            else
            {
                for (k = 0; k <= total_size - 4; k+=4)
                {
                    a = vld1q_s32(src_data);
                    b = vcvtq_f32_s32(a);
                    b = vmulq_f32(b, f);
                    vst1q_f32(dst_data, b);
                    src_data+=4;
                    dst_data+=4;
                }
                for (; k < total_size; k++, src_data++, dst_data++)
                {
                    *dst_data = *src_data / IE_HISI_QUANT;
                }
            }
        }
        else // 内存不连续
        {
            for (i = 0; i < h * c; i++)
            {
                // 源数据行首指针
                int* src_data_line = src_data + i * stride;
                for (k = 0; k <= w - 4; k+=4)
                {
                    a = vld1q_s32(src_data_line);
                    b = vcvtq_f32_s32(a);
                    b = vmulq_f32(b, f);
                    vst1q_f32(dst_data, b);
                    src_data_line+=4;
                    dst_data+=4;
                }
                for (; k < w; k++, src_data_line++, dst_data++)
                {
                    *dst_data = *src_data_line / IE_HISI_QUANT;
                }
            }
        }
#else
        for (int i = 0; i < h * c; i++)
        {
            for (int k = 0; k < w; k++)
            {
                dst_data[k] = src_data[k] / IE_HISI_QUANT;
            }
            src_data += stride;
            dst_data += w;
        }
#endif
    }
}

/**
 * 功能描述: 从文件读取模型s
 * 
 * 输入参数： model_path 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineNNIE::read_model_from_file(const char* model_path)
{
    HI_SL slFileSize = 0;           // 文件大小
    HI_U64 u64PhyAddr = 0;          // 物理地址
	HI_U8 *pu8VirAddr = NULL;       // 虚拟地址

    // 读取wk文件
    FILE* fp = fopen(model_path, "rb");
    if (NULL == fp)     // 打开失败
    {
        return IE_INIT_NNIE_OPEN_FILE_FAIL;
    }
    // 跳转到文件末尾
    fseek(fp, 0L, SEEK_END);

    // 获取文件内容大小
    slFileSize = ftell(fp);

    // 跳转到文件开头
    fseek(fp, 0L, SEEK_SET);

    // 分配模型内存
    if (HI_SUCCESS != HI_MPI_SYS_MmzAlloc((HI_U64*)&u64PhyAddr, (void**)&pu8VirAddr, model_path, NULL, slFileSize))
    {
        IE_HISI_MMZ_Free(m_model_buf.u64PhyAddr, m_model_buf.u64VirAddr);
        m_model_buf.u32Size = 0;
        return IE_INIT_NNIE_MALLOC_MODEL_BUFFER_FAIL;
    }

    // 给模型Buffer赋值
    m_model_buf.u32Size = (HI_U32)slFileSize;
	m_model_buf.u64PhyAddr = u64PhyAddr;
	m_model_buf.u64VirAddr = (HI_U64)pu8VirAddr;
    // 读取wk文件内容到Buffer
    fread(pu8VirAddr, slFileSize, 1, fp);

	// 加载NNIE模型
	if (HI_SUCCESS != HI_MPI_SVP_NNIE_LoadModel(&m_model_buf, &m_model))
    {
        IE_HISI_MMZ_Free(m_model_buf.u64PhyAddr, m_model_buf.u64VirAddr);
        m_model_buf.u32Size = 0;
        return IE_INIT_NNIE_LOAD_MODEL_FAIL;
    }

    // 获取模型分段数
    m_seg_num = m_model.u32NetSegNum;

    // 关闭文件
	fclose(fp);

    return IE_SUCCESS;
}

/**
 * 功能描述: 获取 input size 
 * 
 * 输入参数： model 
 */
void EngineNNIE::get_input_size(const SVP_NNIE_MODEL_S& model)
{
    SVP_NNIE_SEG_S seg = model.astSeg[0];
    SVP_NNIE_NODE_S& input_node = seg.astSrcNode[0];
    
    m_input_shape = IE_Shape(input_node.unShape.stWhc.u32Chn,
            input_node.unShape.stWhc.u32Height,
            input_node.unShape.stWhc.u32Width);
}


/**
 * 功能描述: 分配blob内存
 * 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineNNIE::malloc_blob_mem(void)
{
    HI_U64 u64PhyAddr = 0;                              // 物理地址
	HI_U8 *pu8VirAddr = NULL;                           // 虚拟地址
    int nChn, nWidth, nHeight, nStride, nSize;          // 数据长度
    int nElemSize;                                      // 字节数
    char szMmb[128] = {0};                              // MMB名称

    // -------- 第0段 --------
    SVP_NNIE_SEG_S stSeg = m_model.astSeg[0];
    // ---- SRC ----
    nChn = stSeg.astSrcNode[0].unShape.stWhc.u32Chn;
    nHeight = stSeg.astSrcNode[0].unShape.stWhc.u32Height;
    nWidth = stSeg.astSrcNode[0].unShape.stWhc.u32Width;
    nElemSize = 1;
    nStride = IE_HISI_Align(nWidth * nElemSize);
    nSize = nStride * nHeight * nChn;

    // 分配Blob内存
    if (HI_SUCCESS != IE_HISI_MMZ_MallocCached((HI_CHAR*)"SEG0_SRC0", NULL, (HI_U64*)&u64PhyAddr, (void**)&pu8VirAddr, nSize))
    {
        return IE_INIT_NNIE_MALLOC_BLOB_FAIL;
    }
    memset(pu8VirAddr, 0, nSize);
    if (HI_SUCCESS != IE_HISI_FlushCache(u64PhyAddr, (void*)pu8VirAddr, nSize))
    {
        return IE_INIT_NNIE_FLUSH_CACHE_FAIL;
    }

    m_input_num = stSeg.u16SrcNum;

    // 填写Blob信息
    m_src_blob.enType = SVP_BLOB_TYPE_U8;
    m_src_blob.u32Stride = nStride;
    m_src_blob.u64PhyAddr = u64PhyAddr;
    m_src_blob.u64VirAddr = (HI_U64)pu8VirAddr;
    m_src_blob.u32Num = 1;
    m_src_blob.unShape.stWhc.u32Chn = nChn;
    m_src_blob.unShape.stWhc.u32Height = nHeight;
    m_src_blob.unShape.stWhc.u32Width = nWidth;

    // ---- DST ----
    m_output_num = stSeg.u16DstNum;
    m_dst_blobs = (SVP_BLOB_S*)malloc(m_output_num * sizeof(SVP_BLOB_S));
    m_output_blobs.resize(m_output_num);
    m_output_shapes.resize(m_output_num);

    for (int i = 0; i < m_output_num; i++)
    {
        nChn = stSeg.astDstNode[i].unShape.stWhc.u32Chn;
        nHeight = stSeg.astDstNode[i].unShape.stWhc.u32Height;
        nWidth = stSeg.astDstNode[i].unShape.stWhc.u32Width;
        nElemSize = sizeof(float);
        nStride = IE_HISI_Align(nWidth * nElemSize);
        nSize = nStride * nHeight * nChn;

        // 分配Blob内存
        sprintf(szMmb, (HI_CHAR*)"SEG0_DST%d", i);
        if (HI_SUCCESS != IE_HISI_MMZ_MallocCached(szMmb, NULL, (HI_U64*)&u64PhyAddr, (void**)&pu8VirAddr, nSize))
        {
            return IE_INIT_NNIE_MALLOC_BLOB_FAIL;
        }
        memset(pu8VirAddr, 0, nSize);
        if (HI_SUCCESS != IE_HISI_FlushCache(u64PhyAddr, (void*)pu8VirAddr, nSize))
        {
            return IE_INIT_NNIE_FLUSH_CACHE_FAIL;
        }

        // 填写Blob信息
        m_dst_blobs[i].enType = stSeg.astDstNode[i].enType;
        m_dst_blobs[i].u32Stride = nStride;
        m_dst_blobs[i].u64PhyAddr = u64PhyAddr;
        m_dst_blobs[i].u64VirAddr = (HI_U64)pu8VirAddr;
        m_dst_blobs[i].u32Num = 1;
        m_dst_blobs[i].unShape.stWhc.u32Chn = nChn;
        m_dst_blobs[i].unShape.stWhc.u32Height = nHeight;
        m_dst_blobs[i].unShape.stWhc.u32Width = nWidth;

        // 添加反量化输出
        m_output_blobs.emplace_back(stSeg.astDstNode[i].szName, nChn, nHeight, nWidth);
        m_output_shapes.emplace_back(nChn, nHeight, nWidth);
    }
    
    return IE_SUCCESS;
}

/**
 * 功能描述: 分配任务内存
 * 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineNNIE::malloc_task_mem(void)
{
    HI_U64 u64PhyAddr = 0;          // 物理地址
	HI_U8 *pu8VirAddr = NULL;       // 虚拟地址
    HI_U32 u32TskBufSizeArr[m_seg_num];     // 各段任务内存大小
    HI_U32 u32TaskBufSize = 0;      // 最大任务内存大小
    HI_U32 u32TmpBufSize = 0;       // 辅助内存大小
    char szMmb[128] = {0};          // Mmb名称

    // 获取网络任务需要的任务内存
    if (HI_SUCCESS != HI_MPI_SVP_NNIE_GetTskBufSize(1, 0, &m_model, u32TskBufSizeArr, m_seg_num))
    {
        return IE_INIT_NNIE_GET_TASK_BUF_FAIL;
    }
    // 找最大的一段内存
    for (int i = 0; i < m_seg_num; i++)
    {
        if (u32TskBufSizeArr[i] > u32TaskBufSize)
        {
            u32TaskBufSize = u32TskBufSizeArr[i];
        }
    }

    // 申请任务内存
    sprintf(szMmb, (HI_CHAR*)"TASK_BUFFER");
    if (HI_SUCCESS != IE_HISI_MMZ_MallocCached(szMmb, NULL, (HI_U64*)&u64PhyAddr, (void**)&pu8VirAddr, u32TaskBufSize))
    {
        return IE_INIT_NNIE_MALLOC_BUF_FAIL;
    }
    m_task_buf.u32Size = u32TaskBufSize;
    m_task_buf.u64PhyAddr = u64PhyAddr;
    m_task_buf.u64VirAddr = (HI_U64)pu8VirAddr;
    memset(pu8VirAddr, 0, u32TaskBufSize);
    if (HI_SUCCESS != IE_HISI_FlushCache(u64PhyAddr, (void*)pu8VirAddr, u32TaskBufSize))
    {
        return IE_INIT_NNIE_FLUSH_CACHE_FAIL;
    }
    // 记录TskBuf地址信息
    if (HI_SUCCESS == HI_MPI_SVP_NNIE_AddTskBuf(&m_task_buf))
    {
        m_is_tb_added = true;
    }
    else
    {
        m_is_tb_added = false;
    }

    // 申请辅助内存
    u32TmpBufSize = m_model.u32TmpBufSize;
    if (HI_SUCCESS != IE_HISI_MMZ_MallocCached((HI_CHAR*)"TEMP_BUFFER", NULL, (HI_U64*)&u64PhyAddr, (void**)&pu8VirAddr, u32TmpBufSize))
    {
        return IE_INIT_NNIE_MALLOC_BUF_FAIL;
    }
    m_tmp_buf.u32Size = u32TmpBufSize;
	m_tmp_buf.u64PhyAddr = u64PhyAddr;
	m_tmp_buf.u64VirAddr = (HI_U64)pu8VirAddr;
    memset(pu8VirAddr, 0, u32TmpBufSize);
    if (HI_SUCCESS != IE_HISI_FlushCache(u64PhyAddr, (void*)pu8VirAddr, u32TmpBufSize))
    {
        return IE_INIT_NNIE_FLUSH_CACHE_FAIL;
    }

    return IE_SUCCESS;
}

/**
 * 功能描述: 释放blob内存
 * 
 */
void EngineNNIE::free_blob_mem(void)
{
    IE_HISI_MMZ_Free(m_src_blob.u64PhyAddr, m_src_blob.u64VirAddr);
    for (size_t i = 0; i < m_output_num; i++)
    {
        IE_HISI_MMZ_Free(m_dst_blobs[i].u64PhyAddr, m_dst_blobs[i].u64VirAddr);
    }
    free(m_dst_blobs);
    m_dst_blobs = NULL;
}

/**
 * 功能描述: 释放任务内存
 * 
 */
void EngineNNIE::free_task_mem(void)
{
    // 移除TskBuf地址信息
    if (m_is_tb_added)
        HI_MPI_SVP_NNIE_RemoveTskBuf(&m_task_buf);
    IE_HISI_MMZ_Free(m_task_buf.u64PhyAddr, m_task_buf.u64VirAddr);
    m_task_buf.u32Size = 0;
    
    // 释放辅助内存
    IE_HISI_MMZ_Free(m_tmp_buf.u64PhyAddr, m_tmp_buf.u64VirAddr);
    m_tmp_buf.u32Size = 0;
}

/**
 * 功能描述: 初始化模型
 * 
 * 输入参数： model_path 模型路径
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineNNIE::init(const char* model_path)
{
    IE_ERR_CODE err;
    // 1.从文件读模型
    err = read_model_from_file(model_path);
    if (IE_SUCCESS != err)
    {
        return err;
    }

    // 2.获取输入尺寸
    get_input_size(m_model);

    // 3.分配Blob内存
    err = malloc_blob_mem();
    if (IE_SUCCESS != err)
    {
        return err;
    }

    // 4.分配任务内存
    err = malloc_task_mem();
    if (IE_SUCCESS != err)
    {
        return err;
    }

    // 5.设置控制信息
    m_fw_ctrl.enNnieId = SVP_NNIE_ID_0;
    m_fw_ctrl.stTmpBuf = m_tmp_buf;
    m_fw_ctrl.stTskBuf = m_task_buf;
    m_fw_ctrl.u32DstNum = m_output_num;
    m_fw_ctrl.u32NetSegId = 0;
    m_fw_ctrl.u32SrcNum = m_input_num;

    return IE_SUCCESS;
}

/**
 * 功能描述: 运行推理
 * 
 * 输入参数： input_img         输入图像
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineNNIE::run(const cv::Mat& input_img)
{
    IE_ERR_CODE err;
    // 前处理
    err = preprocess(input_img);
    if (IE_SUCCESS != err)
    {
        return err;
    }

    SVP_NNIE_HANDLE nnie_handle;        // NNIE Handle
    HI_BOOL bFinish = HI_FALSE;         // 任务完成标志
    int nTimeOut = 0;                   // 超时次数

    // 给输入Blob
    // auto t1=std::chrono::steady_clock::now();
    HI_U8* pu8VirAddr = (HI_U8*)m_src_blob.u64VirAddr;      // Blob的虚拟地址
    // Blob尺寸
    int nBlobStep = m_src_blob.u32Stride;
    int nBlobWidth = m_src_blob.unShape.stWhc.u32Width;
    int nBlobHeight = m_src_blob.unShape.stWhc.u32Height;
    int nBlobChn = m_src_blob.unShape.stWhc.u32Chn;
    memcpy(pu8VirAddr, m_input_blob.data, nBlobChn * nBlobHeight * nBlobWidth);
    IE_HISI_FlushCache(m_src_blob.u64PhyAddr, (void*)pu8VirAddr, nBlobStep * nBlobHeight * nBlobChn);
    // auto t2=std::chrono::steady_clock::now();
    // auto dr_ms=std::chrono::duration<double,std::milli>(t2-t1).count();
    // printf("wrap_input_layer take %f ms\n", dr_ms);

    // 跑第一段网络
    IE_HISI_FlushCache(m_task_buf.u64PhyAddr, (void*)m_task_buf.u64VirAddr, m_task_buf.u32Size);
    if (HI_SUCCESS != HI_MPI_SVP_NNIE_Forward(&nnie_handle, &m_src_blob, &m_model, 
        m_dst_blobs, &m_fw_ctrl, HI_TRUE))
    {
        return IE_INFER_NNIE_FORWARD_FAIL;
    }

    // 等待完成
    while (HI_ERR_SVP_NNIE_QUERY_TIMEOUT == HI_MPI_SVP_NNIE_Query(SVP_NNIE_ID_0, nnie_handle, &bFinish, HI_TRUE))
    {
        usleep(100);
        nTimeOut++;
        if (nTimeOut > 5)
        {
            return IE_INFER_NNIE_FORWARD_TIMEOUT;
        }
    }
    nTimeOut = 0;
    // 失败
    if (!bFinish)
    {
        return IE_INFER_NNIE_QUERY_FAIL;
    }

    // 刷新第一段输出
    for (int i = 0; i < m_output_num; i++)
    {
        int u32Size = m_dst_blobs[i].u32Stride * m_dst_blobs[i].unShape.stWhc.u32Height * m_dst_blobs[i].unShape.stWhc.u32Chn;
        IE_HISI_FlushCache(m_dst_blobs[i].u64PhyAddr, (void*)m_dst_blobs[i].u64VirAddr, u32Size);
    }
    
    // 反量化
    // printf("Dequnt\n");
    // t1=std::chrono::steady_clock::now();
    for (int i = 0; i < m_output_num; i++)
    {
        IE_HISI_Dequant(m_output_blobs[i], m_dst_blobs[i]);
    }
    // t2=std::chrono::steady_clock::now();
    // dr_ms=std::chrono::duration<double,std::milli>(t2-t1).count();
    // std::cout << "dequant take " << dr_ms << " ms " << std::endl;

    return IE_SUCCESS;
}

/**
 * 功能描述: 释放模型
 * 
 * 返回参数：IE_ERR_CODE 
 */
IE_ERR_CODE EngineNNIE::release(void)
{
    // 卸载模型
    HI_MPI_SVP_NNIE_UnloadModel(&m_model);
    IE_HISI_MMZ_Free(m_model_buf.u64PhyAddr, m_model_buf.u64VirAddr);
    m_model_buf.u32Size = 0;

    // 释放任务内存
    free_task_mem();
    
    // 释放Blob内存
    free_blob_mem();

    return IE_SUCCESS;
}

#endif // PLATFORM GUARD