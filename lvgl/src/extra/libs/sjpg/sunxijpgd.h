/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : jpegdecode.h
 * Description : jpegdecode
 * History :
 *
 */


#ifndef JPEG_DECODE
#define JPEG_DECODE

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lv_conf_internal.h"
#if LV_USE_SJPG && defined(USE_HARDWARE_JPEGDECODER)

#include "vdecoder.h"           //* video decode library in "libcedarc/include/"
#include "memoryAdapter.h"

typedef enum JpegDecodeScaleDownRatio{
    JPEG_DECODE_SCALE_DOWN_1      = 0, /*no scale down*/
    JPEG_DECODE_SCALE_DOWN_2      = 1, /*scale down 1/2*/
    JPEG_DECODE_SCALE_DOWN_4      = 2, /*scale down 1/4*/
    JPEG_DECODE_SCALE_DOWN_8      = 3, /*scale down 1/8*/
}JpegDecodeScaleDownRatio;

typedef enum JpegDecodeOutputDataType
{
    JpegDecodeOutputDataNV21 = 1,
    JpegDecodeOutputDataNV12 = 2,
    JpegDecodeOutputDataYU12 = 3,
    JpegDecodeOutputDataYV12 = 4,
    JpegDecodeOutputDataRGB565 = 5,
    JpegDecodeOutputDataARGB8888 = 6,
}JpegDecodeOutputDataType;

typedef struct ImgFrame
{
    uint32_t mWidth;                // the width which contains aligned buffer
    uint32_t mHeight;               // the height which contains aligned buffer
    uint32_t mDisplayWidth;         // the actural frame width
    uint32_t mDisplayHeight;        // the actural frame height
    uint32_t mYuvSize;              // Number of bytes in mYuvData
    uint8_t* mYuvData;              // Actual YUV data
    uint32_t mRGBSize;              // Number of bytes in mRGBData
    uint8_t* mRGBData;              // Actual RGB565 or ARGB8888 data
    int32_t  mRotationAngle;	    // rotation angle, clockwise
}ImgFrame;

typedef struct JpegDecoderContext
{
    VideoDecoder*      mVideoDecoder;
    ImgFrame           mImgFrame;
    char*              mSrcBuf;
    int                mSrcBufLen;
    int                mScaleDownEn;
    int                mHorizonScaleDownRatio;
    int                mVerticalScaleDownRatio;
    int                mDecodedPixFormat;
    int                mOutputDataType;
    struct ScMemOpsS  *memops;
}JpegDecoderContext;

typedef void* JpegDecoder;

int sunxijpgd_load_file(char** out, int* outsize, const char* filename);
JpegDecoder* JpegDecoderCreate();
void JpegDecoderDestory(JpegDecoder* v);
void JpegDecoderSetDataSourceBuf(JpegDecoder* v, char* buffer, int bufLen,
		JpegDecodeScaleDownRatio scaleRatio, JpegDecodeOutputDataType outputType);
ImgFrame *JpegDecoderGetFrame(JpegDecoder* v);
/* 仅释放本次解码占用的 VideoDecoder（含 ION SBM 缓冲），保留 jpegdecoder 单例本身。
 * 每张图绘完后必须调用，否则 ION DMA 堆会持续累积导致 CMA OOM。 */
void JpegDecoderReleaseSession(JpegDecoder* v);

/*
 * 把 GetFrame 刚刚生产的 RGB 缓冲从单例里"摘走"，所有权交给调用方。
 *
 * 摘走之后：
 *   - p->mImgFrame.mRGBData 被置 NULL
 *   - 下次 JpegDecoderGetFrame 不会再 free 这块（旧实现一进 GetFrame 就把
 *     上一张的 mRGBData free 掉，多 dsc 共存时直接 use-after-free）
 *   - 调用方必须用 JpegDecoderFreeFrameBuffer 释放
 *
 * 配合 LV_IMG_CACHE_DEF_SIZE > 0 时必须用这个，否则 cache 一开就崩。
 *
 * 失败返回 NULL（GetFrame 没成功 / mRGBData 已经为空）。
 */
uint8_t* JpegDecoderDetachFrameBuffer(JpegDecoder* v,
                                      uint32_t* out_display_w,
                                      uint32_t* out_display_h);

/*
 * 释放 JpegDecoderDetachFrameBuffer 返回的缓冲，按编译宏自动选择
 * sunxifb_mem_free / lv_mem_free。NULL 安全。
 */
void JpegDecoderFreeFrameBuffer(uint8_t* buf);

#endif /*LV_USE_SJPG*/

#ifdef __cplusplus
}
#endif

#endif
