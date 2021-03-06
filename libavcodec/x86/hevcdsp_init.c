/*
 * Copyright (c) 2013 Seppo Tomperi
 *
 * This file is part of ffmpeg.
 *
 * ffmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ffmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ffmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"
#include "libavutil/cpu.h"
#include "libavutil/x86/asm.h"
#include "libavutil/x86/cpu.h"
#include "libavcodec/get_bits.h" /* required for hevcdsp.h GetBitContext */
#include "libavcodec/hevcdsp.h"
#include "libavcodec/x86/hevcdsp.h"
#include "libavcodec/hevc_defs.h"

/***********************************/

#define MCQ_FUNC(DIR, DEPTH, OPT)                                        \
    void ff_put_hevc_mc_pixels_ ## DIR ## _ ## DEPTH ## _ ## OPT(int16_t *dst, ptrdiff_t dststride, uint8_t *_src, ptrdiff_t _srcstride, int width, int height, int mx, int my);

#define MCQ_FUNCS(type, depth) \
   MCQ_FUNC( 2, depth, sse4)    \
   MCQ_FUNC( 4, depth, sse4)    \
   MCQ_FUNC( 8, depth, sse4)    \
   MCQ_FUNC(16, depth, sse4)

MCQ_FUNCS(uint8_t,   8)

/***********************************/
/* deblocking */

#define LFC_FUNC(DIR, DEPTH, OPT)                                        \
void ff_hevc_ ## DIR ## _loop_filter_chroma_ ## DEPTH ## _ ## OPT(uint8_t *_pix, ptrdiff_t _stride, int *_tc, uint8_t *_no_p, uint8_t *_no_q);

#define LFL_FUNC(DIR, DEPTH, OPT)                                        \
void ff_hevc_ ## DIR ## _loop_filter_luma_ ## DEPTH ## _ ## OPT(uint8_t *_pix, ptrdiff_t stride, int *_beta, int *_tc, \
uint8_t *_no_p, uint8_t *_no_q);

#define LFC_FUNCS(type, depth) \
LFC_FUNC(h, depth, sse2)  \
LFC_FUNC(v, depth, sse2)

#define LFL_FUNCS(type, depth) \
LFL_FUNC(h, depth, ssse3)  \
LFL_FUNC(v, depth, ssse3)

LFC_FUNCS(uint8_t,   8)
LFC_FUNCS(uint8_t,  10)
LFL_FUNCS(uint8_t,   8)
LFL_FUNCS(uint8_t,  10)


#define mc_rep_func(name, bitd, step, W) \
void ff_hevc_put_hevc_##name##W##_##bitd##_sse4(int16_t *dst, ptrdiff_t dststride,uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) \
{ \
    int i;  \
    uint8_t *src;   \
    uint16_t *_dst; \
    for(i=0; i < W ; i+= step ){    \
        src= _src+(i*((bitd+7)/8));            \
        _dst= dst+i;                        \
    ff_hevc_put_hevc_##name##step##_##bitd##_sse4(_dst, dststride, src, _srcstride, height, mx, my, width);   \
    }   \
}

mc_rep_func(pel_pixels, 8, 16, 64);
mc_rep_func(pel_pixels, 8, 16, 48);
mc_rep_func(pel_pixels, 8, 16, 32);

mc_rep_func(pel_pixels,10,  8, 64);
mc_rep_func(pel_pixels,10,  8, 48);
mc_rep_func(pel_pixels,10,  8, 32);
mc_rep_func(pel_pixels,10,  8, 24);
mc_rep_func(pel_pixels,10,  8, 16);

mc_rep_func(epel_h, 8, 16, 64);
mc_rep_func(epel_h, 8, 16, 48);
mc_rep_func(epel_h, 8, 16, 32);

mc_rep_func(epel_h,10,  8, 64);
mc_rep_func(epel_h,10,  8, 48);
mc_rep_func(epel_h,10,  8, 32);
mc_rep_func(epel_h,10,  8, 24);
mc_rep_func(epel_h,10,  8, 16);


mc_rep_func(epel_v, 8, 16, 64);
mc_rep_func(epel_v, 8, 16, 48);
mc_rep_func(epel_v, 8, 16, 32);

mc_rep_func(epel_v,10,  8, 64);
mc_rep_func(epel_v,10,  8, 48);
mc_rep_func(epel_v,10,  8, 32);
mc_rep_func(epel_v,10,  8, 24);
mc_rep_func(epel_v,10,  8, 16);

mc_rep_func(epel_hv, 8,  8, 64);
mc_rep_func(epel_hv, 8,  8, 48);
mc_rep_func(epel_hv, 8,  8, 32);
mc_rep_func(epel_hv, 8,  8, 24);
mc_rep_func(epel_hv, 8,  8, 16);

mc_rep_func(epel_hv,10,  8, 64);
mc_rep_func(epel_hv,10,  8, 48);
mc_rep_func(epel_hv,10,  8, 32);
mc_rep_func(epel_hv,10,  8, 24);
mc_rep_func(epel_hv,10,  8, 16);


mc_rep_func(qpel_h, 8, 16, 64);
mc_rep_func(qpel_h, 8, 16, 48);
mc_rep_func(qpel_h, 8, 16, 32);

mc_rep_func(qpel_h,10,  8, 64);
mc_rep_func(qpel_h,10,  8, 48);
mc_rep_func(qpel_h,10,  8, 32);
mc_rep_func(qpel_h,10,  8, 24);
mc_rep_func(qpel_h,10,  8, 16);


mc_rep_func(qpel_v, 8, 16, 64);
mc_rep_func(qpel_v, 8, 16, 48);
mc_rep_func(qpel_v, 8, 16, 32);

mc_rep_func(qpel_v,10,  8, 64);
mc_rep_func(qpel_v,10,  8, 48);
mc_rep_func(qpel_v,10,  8, 32);
mc_rep_func(qpel_v,10,  8, 24);
mc_rep_func(qpel_v,10,  8, 16);





void ff_hevcdsp_init_x86(HEVCDSPContext *c, const int bit_depth)
{
    int mm_flags = av_get_cpu_flags();

    if (bit_depth == 8) {
        if (EXTERNAL_MMX(mm_flags)) {
            /*if (mm_flags & AV_CPU_FLAG_CMOV)
                c->h264_luma_dc_dequant_idct = ff_h264_luma_dc_dequant_idct_mmx; */

            if (EXTERNAL_MMXEXT(mm_flags)) {
#if ARCH_X86_32 && HAVE_MMXEXT_EXTERNAL
                /* MMEXT optimizations */
#endif /* ARCH_X86_32 && HAVE_MMXEXT_EXTERNAL */

#ifdef __SSE2__
                if (EXTERNAL_SSE2(mm_flags)) {
                    c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_8_sse2;
                    c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_8_sse2;
                }
#endif //__SSE2__
#ifdef __SSSE3__
                if (EXTERNAL_SSSE3(mm_flags)) {

                    c->transform_4x4_luma_add = ff_hevc_transform_4x4_luma_add_8_sse4;
                    c->transform_add[0] = ff_hevc_transform_4x4_add_8_sse4;
                    c->transform_add[1] = ff_hevc_transform_8x8_add_8_sse4;
                    c->transform_add[2] = ff_hevc_transform_16x16_add_8_sse4;
                    c->transform_add[3] = ff_hevc_transform_32x32_add_8_sse4;


                    c->put_weighted_pred_avg[0] = ff_hevc_put_weighted_pred_avg2_8_sse;
                    c->put_weighted_pred_avg[1] = ff_hevc_put_weighted_pred_avg4_8_sse;
                    c->put_weighted_pred_avg[2] = ff_hevc_put_weighted_pred_avg8_8_sse;
                    c->put_weighted_pred_avg[3] = ff_hevc_put_weighted_pred_avg16_8_sse;
                    c->put_weighted_pred_avg[4] = ff_hevc_put_weighted_pred_avg16_8_sse;
                    c->put_weighted_pred_avg[5] = ff_hevc_put_weighted_pred_avg16_8_sse;


                    PEL_LINK(c->put_hevc_qpel, 1, 0, 1, qpel_h4 ,  8);
                    PEL_LINK(c->put_hevc_qpel, 3, 0, 1, qpel_h8 ,  8);
                    PEL_LINK(c->put_hevc_qpel, 4, 0, 1, qpel_h12,  8);
                    PEL_LINK(c->put_hevc_qpel, 5, 0, 1, qpel_h16,  8);
                    PEL_LINK(c->put_hevc_qpel, 6, 0, 1, qpel_h24,  8);
                    PEL_LINK(c->put_hevc_qpel, 7, 0, 1, qpel_h32,  8);
                    PEL_LINK(c->put_hevc_qpel, 8, 0, 1, qpel_h48,  8);
                    PEL_LINK(c->put_hevc_qpel, 9, 0, 1, qpel_h64,  8);
                    PEL_LINK(c->put_hevc_qpel, 1, 1, 0, qpel_v4 ,  8);
                    PEL_LINK(c->put_hevc_qpel, 3, 1, 0, qpel_v8 ,  8);
                    PEL_LINK(c->put_hevc_qpel, 4, 1, 0, qpel_v12,  8);
                    PEL_LINK(c->put_hevc_qpel, 5, 1, 0, qpel_v16,  8);
                    PEL_LINK(c->put_hevc_qpel, 6, 1, 0, qpel_v24,  8);
                    PEL_LINK(c->put_hevc_qpel, 7, 1, 0, qpel_v32,  8);
                    PEL_LINK(c->put_hevc_qpel, 8, 1, 0, qpel_v48,  8);
                    PEL_LINK(c->put_hevc_qpel, 9, 1, 0, qpel_v64,  8);

                    PEL_LINK_SSE(c->put_hevc_qpel, 1, 1, 1, qpel_hv4 ,  8);
                    PEL_LINK_SSE(c->put_hevc_qpel, 3, 1, 1, qpel_hv8 ,  8);
                    PEL_LINK_SSE(c->put_hevc_qpel, 4, 1, 1, qpel_hv12,  8);
                    PEL_LINK_SSE(c->put_hevc_qpel, 5, 1, 1, qpel_hv16,  8);
                    PEL_LINK_SSE(c->put_hevc_qpel, 6, 1, 1, qpel_hv24,  8);
                    PEL_LINK_SSE(c->put_hevc_qpel, 7, 1, 1, qpel_hv32,  8);
                    PEL_LINK_SSE(c->put_hevc_qpel, 8, 1, 1, qpel_hv48,  8);
                    PEL_LINK_SSE(c->put_hevc_qpel, 9, 1, 1, qpel_hv64,  8);

#if ARCH_X86_64
                    c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_8_ssse3;
                    c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_8_ssse3;
#endif
                }
#endif //__SSSE3__
#ifdef __SSE4_1__

                if (EXTERNAL_SSE4(mm_flags)) {
                    c->weighted_pred[0]         = ff_hevc_weighted_pred2_8_sse;
                    c->weighted_pred[1]         = ff_hevc_weighted_pred4_8_sse;
                    c->weighted_pred[2]         = ff_hevc_weighted_pred8_8_sse;
                    c->weighted_pred[3]         = ff_hevc_weighted_pred16_8_sse;
                    c->weighted_pred[4]         = ff_hevc_weighted_pred16_8_sse;
                    c->weighted_pred[5]         = ff_hevc_weighted_pred16_8_sse;
                    c->weighted_pred_avg[0]     = ff_hevc_weighted_pred_avg2_8_sse;
                    c->weighted_pred_avg[1]     = ff_hevc_weighted_pred_avg4_8_sse;
                    c->weighted_pred_avg[2]     = ff_hevc_weighted_pred_avg8_8_sse;
                    c->weighted_pred_avg[3]     = ff_hevc_weighted_pred_avg16_8_sse;
                    c->weighted_pred_avg[4]     = ff_hevc_weighted_pred_avg16_8_sse;
                    c->weighted_pred_avg[5]     = ff_hevc_weighted_pred_avg16_8_sse;

                    c->put_unweighted_pred[0]   = ff_hevc_put_unweighted_pred2_8_sse;
                    c->put_unweighted_pred[1]   = ff_hevc_put_unweighted_pred4_8_sse;
                    c->put_unweighted_pred[2]   = ff_hevc_put_unweighted_pred8_8_sse;
                    c->put_unweighted_pred[3]   = ff_hevc_put_unweighted_pred16_8_sse;
                    c->put_unweighted_pred[4]   = ff_hevc_put_unweighted_pred16_8_sse;
                    c->put_unweighted_pred[5]   = ff_hevc_put_unweighted_pred16_8_sse;
                    PEL_LINK(c->put_hevc_qpel, 1, 0, 0, pel_pixels4 ,  8);
                    PEL_LINK(c->put_hevc_qpel, 3, 0, 0, pel_pixels8 ,  8);
                    PEL_LINK(c->put_hevc_qpel, 4, 0, 0, pel_pixels12,  8);
                    PEL_LINK(c->put_hevc_qpel, 5, 0, 0, pel_pixels16,  8);
                    PEL_LINK(c->put_hevc_qpel, 6, 0, 0, pel_pixels24,  8);
                    PEL_LINK(c->put_hevc_qpel, 7, 0, 0, pel_pixels32,  8);
                    PEL_LINK(c->put_hevc_qpel, 8, 0, 0, pel_pixels48,  8);
                    PEL_LINK(c->put_hevc_qpel, 9, 0, 0, pel_pixels64,  8);

                    PEL_LINK(c->put_hevc_epel, 1, 0, 0, pel_pixels4 ,  8);
                    PEL_LINK(c->put_hevc_epel, 3, 0, 0, pel_pixels8 ,  8);
                    PEL_LINK(c->put_hevc_epel, 4, 0, 0, pel_pixels12,  8);
                    PEL_LINK(c->put_hevc_epel, 5, 0, 0, pel_pixels16,  8);
                    PEL_LINK(c->put_hevc_epel, 6, 0, 0, pel_pixels24,  8);
                    PEL_LINK(c->put_hevc_epel, 7, 0, 0, pel_pixels32,  8);
                    PEL_LINK(c->put_hevc_epel, 8, 0, 0, pel_pixels48,  8);
                    PEL_LINK(c->put_hevc_epel, 9, 0, 0, pel_pixels64,  8);
                    PEL_LINK(c->put_hevc_epel, 1, 0, 1, epel_h4 ,  8);
                    PEL_LINK(c->put_hevc_epel, 3, 0, 1, epel_h8 ,  8);
                    PEL_LINK(c->put_hevc_epel, 4, 0, 1, epel_h12,  8);
                    PEL_LINK(c->put_hevc_epel, 5, 0, 1, epel_h16,  8);
                    PEL_LINK(c->put_hevc_epel, 6, 0, 1, epel_h24,  8);
                    PEL_LINK(c->put_hevc_epel, 7, 0, 1, epel_h32,  8);
                    PEL_LINK(c->put_hevc_epel, 8, 0, 1, epel_h48,  8);
                    PEL_LINK(c->put_hevc_epel, 9, 0, 1, epel_h64,  8);
                    PEL_LINK(c->put_hevc_epel, 1, 1, 0, epel_v4 ,  8);
                    PEL_LINK(c->put_hevc_epel, 3, 1, 0, epel_v8 ,  8);
                    PEL_LINK(c->put_hevc_epel, 4, 1, 0, epel_v12,  8);
                    PEL_LINK(c->put_hevc_epel, 5, 1, 0, epel_v16,  8);
                    PEL_LINK(c->put_hevc_epel, 6, 1, 0, epel_v24,  8);
                    PEL_LINK(c->put_hevc_epel, 7, 1, 0, epel_v32,  8);
                    PEL_LINK(c->put_hevc_epel, 8, 1, 0, epel_v48,  8);
                    PEL_LINK(c->put_hevc_epel, 9, 1, 0, epel_v64,  8);

                    PEL_LINK(c->put_hevc_epel, 1, 1, 1, epel_hv4 ,  8);
                    PEL_LINK(c->put_hevc_epel, 3, 1, 1, epel_hv8 ,  8);
                    PEL_LINK(c->put_hevc_epel, 4, 1, 1, epel_hv12,  8);
                    PEL_LINK(c->put_hevc_epel, 5, 1, 1, epel_hv16,  8);
                    PEL_LINK(c->put_hevc_epel, 6, 1, 1, epel_hv24,  8);
                    PEL_LINK(c->put_hevc_epel, 7, 1, 1, epel_hv32,  8);
                    PEL_LINK(c->put_hevc_epel, 8, 1, 1, epel_hv48,  8);
                    PEL_LINK(c->put_hevc_epel, 9, 1, 1, epel_hv64,  8);

                    c->transform_skip     = ff_hevc_transform_skip_8_sse;
                    c->sao_edge_filter[0] = ff_hevc_sao_edge_filter_0_8_sse;
                    c->sao_edge_filter[1] = ff_hevc_sao_edge_filter_1_8_sse;
                    c->sao_band_filter    = ff_hevc_sao_band_filter_0_8_sse;

#ifdef SVC_EXTENSION
                    c->upsample_base_layer_frame = ff_upsample_base_layer_frame_sse;
                    c->upsample_h_base_layer_frame = ff_upsample_base_layer_frame_sse_h;
                    c->upsample_v_base_layer_frame = ff_upsample_base_layer_frame_sse_v;
#endif


                }
#endif //__SSE4_1__
                if (EXTERNAL_AVX(mm_flags)) {
                }
            }
        }
    } else if (bit_depth == 10) {
        if (EXTERNAL_MMX(mm_flags)) {
            if (EXTERNAL_MMXEXT(mm_flags)) {
#if ARCH_X86_32
#endif /* ARCH_X86_32 */
#ifdef __SSE2__
                if (EXTERNAL_SSE2(mm_flags)) {
                    c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_10_sse2;
                    c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_10_sse2;
#if HAVE_ALIGNED_STACK
                    /*stuff that requires aligned stack */
#endif /* HAVE_ALIGNED_STACK */
                }
#endif /* __SSE2__ */
#ifdef __SSSE3__
                if (EXTERNAL_SSSE3(mm_flags)) {
#if ARCH_X86_64
                    c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_10_ssse3;
                    c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_10_ssse3;
#endif
                }
#endif //__SSSE3__
#ifdef __SSE4_1__
                if (EXTERNAL_SSE4(mm_flags)) {
                    c->transform_4x4_luma_add   = ff_hevc_transform_4x4_luma_add_10_sse4;
                    c->transform_add[0]         = ff_hevc_transform_4x4_add_10_sse4;
                    c->transform_add[1]         = ff_hevc_transform_8x8_add_10_sse4;
                    c->transform_add[2]         = ff_hevc_transform_16x16_add_10_sse4;
                    c->transform_add[3]         = ff_hevc_transform_32x32_add_10_sse4;

                    c->put_unweighted_pred[0]   = ff_hevc_put_unweighted_pred2_10_sse;
                    c->put_unweighted_pred[1]   = ff_hevc_put_unweighted_pred4_10_sse;
                    c->put_unweighted_pred[2]   = ff_hevc_put_unweighted_pred8_10_sse;
                    c->put_unweighted_pred[3]   = ff_hevc_put_unweighted_pred8_10_sse;
                    c->put_unweighted_pred[4]   = ff_hevc_put_unweighted_pred8_10_sse;
                    c->put_unweighted_pred[5]   = ff_hevc_put_unweighted_pred8_10_sse;
                    c->put_weighted_pred_avg[0] = ff_hevc_put_weighted_pred_avg2_10_sse;
                    c->put_weighted_pred_avg[1] = ff_hevc_put_weighted_pred_avg4_10_sse;
                    c->put_weighted_pred_avg[2] = ff_hevc_put_weighted_pred_avg8_10_sse;
                    c->put_weighted_pred_avg[3] = ff_hevc_put_weighted_pred_avg8_10_sse;
                    c->put_weighted_pred_avg[4] = ff_hevc_put_weighted_pred_avg8_10_sse;
                    c->put_weighted_pred_avg[5] = ff_hevc_put_weighted_pred_avg8_10_sse;
                    c->weighted_pred[0]         = ff_hevc_weighted_pred2_10_sse;
                    c->weighted_pred[1]         = ff_hevc_weighted_pred4_10_sse;
                    c->weighted_pred[2]         = ff_hevc_weighted_pred8_10_sse;
                    c->weighted_pred[3]         = ff_hevc_weighted_pred8_10_sse;
                    c->weighted_pred[4]         = ff_hevc_weighted_pred8_10_sse;
                    c->weighted_pred[5]         = ff_hevc_weighted_pred8_10_sse;
                    c->weighted_pred_avg[0]     = ff_hevc_weighted_pred_avg2_10_sse;
                    c->weighted_pred_avg[1]     = ff_hevc_weighted_pred_avg4_10_sse;
                    c->weighted_pred_avg[2]     = ff_hevc_weighted_pred_avg8_10_sse;
                    c->weighted_pred_avg[3]     = ff_hevc_weighted_pred_avg8_10_sse;
                    c->weighted_pred_avg[4]     = ff_hevc_weighted_pred_avg8_10_sse;
                    c->weighted_pred_avg[5]     = ff_hevc_weighted_pred_avg8_10_sse;

                    PEL_LINK(c->put_hevc_epel, 1, 0, 0, pel_pixels4 , 10);
                    PEL_LINK(c->put_hevc_epel, 3, 0, 0, pel_pixels8 , 10);
                    PEL_LINK(c->put_hevc_epel, 4, 0, 0, pel_pixels12, 10);
                    PEL_LINK(c->put_hevc_epel, 5, 0, 0, pel_pixels16, 10);
                    PEL_LINK(c->put_hevc_epel, 6, 0, 0, pel_pixels24, 10);
                    PEL_LINK(c->put_hevc_epel, 7, 0, 0, pel_pixels32, 10);
                    PEL_LINK(c->put_hevc_epel, 8, 0, 0, pel_pixels48, 10);
                    PEL_LINK(c->put_hevc_epel, 9, 0, 0, pel_pixels64, 10);
                    PEL_LINK(c->put_hevc_epel, 1, 0, 1, epel_h4 , 10);
                    PEL_LINK(c->put_hevc_epel, 3, 0, 1, epel_h8 , 10);
                    PEL_LINK(c->put_hevc_epel, 4, 0, 1, epel_h12, 10);
                    PEL_LINK(c->put_hevc_epel, 5, 0, 1, epel_h16, 10);
                    PEL_LINK(c->put_hevc_epel, 6, 0, 1, epel_h24, 10);
                    PEL_LINK(c->put_hevc_epel, 7, 0, 1, epel_h32, 10);
                    PEL_LINK(c->put_hevc_epel, 8, 0, 1, epel_h48, 10);
                    PEL_LINK(c->put_hevc_epel, 9, 0, 1, epel_h64, 10);
                    PEL_LINK(c->put_hevc_epel, 1, 1, 0, epel_v4 , 10);
                    PEL_LINK(c->put_hevc_epel, 3, 1, 0, epel_v8 , 10);
                    PEL_LINK(c->put_hevc_epel, 4, 1, 0, epel_v12, 10);
                    PEL_LINK(c->put_hevc_epel, 5, 1, 0, epel_v16, 10);
                    PEL_LINK(c->put_hevc_epel, 6, 1, 0, epel_v24, 10);
                    PEL_LINK(c->put_hevc_epel, 7, 1, 0, epel_v32, 10);
                    PEL_LINK(c->put_hevc_epel, 8, 1, 0, epel_v48, 10);
                    PEL_LINK(c->put_hevc_epel, 9, 1, 0, epel_v64, 10);
                    PEL_LINK(c->put_hevc_epel, 1, 1, 1, epel_hv4 , 10);
                    PEL_LINK(c->put_hevc_epel, 3, 1, 1, epel_hv8 , 10);
                    PEL_LINK(c->put_hevc_epel, 4, 1, 1, epel_hv12, 10);
                    PEL_LINK(c->put_hevc_epel, 5, 1, 1, epel_hv16, 10);
                    PEL_LINK(c->put_hevc_epel, 6, 1, 1, epel_hv24, 10);
                    PEL_LINK(c->put_hevc_epel, 7, 1, 1, epel_hv32, 10);
                    PEL_LINK(c->put_hevc_epel, 8, 1, 1, epel_hv48, 10);
                    PEL_LINK(c->put_hevc_epel, 9, 1, 1, epel_hv64, 10);

                    PEL_LINK(c->put_hevc_qpel, 1, 0, 0, pel_pixels4 , 10);
                    PEL_LINK(c->put_hevc_qpel, 3, 0, 0, pel_pixels8 , 10);
                    PEL_LINK(c->put_hevc_qpel, 4, 0, 0, pel_pixels12, 10);
                    PEL_LINK(c->put_hevc_qpel, 5, 0, 0, pel_pixels16, 10);
                    PEL_LINK(c->put_hevc_qpel, 6, 0, 0, pel_pixels24, 10);
                    PEL_LINK(c->put_hevc_qpel, 7, 0, 0, pel_pixels32, 10);
                    PEL_LINK(c->put_hevc_qpel, 8, 0, 0, pel_pixels48, 10);
                    PEL_LINK(c->put_hevc_qpel, 9, 0, 0, pel_pixels64, 10);
                    PEL_LINK(c->put_hevc_qpel, 1, 0, 1, qpel_h4 , 10);
                    PEL_LINK(c->put_hevc_qpel, 3, 0, 1, qpel_h8 , 10);
                    PEL_LINK(c->put_hevc_qpel, 4, 0, 1, qpel_h12, 10);
                    PEL_LINK(c->put_hevc_qpel, 5, 0, 1, qpel_h16, 10);
                    PEL_LINK(c->put_hevc_qpel, 6, 0, 1, qpel_h24, 10);
                    PEL_LINK(c->put_hevc_qpel, 7, 0, 1, qpel_h32, 10);
                    PEL_LINK(c->put_hevc_qpel, 8, 0, 1, qpel_h48, 10);
                    PEL_LINK(c->put_hevc_qpel, 9, 0, 1, qpel_h64, 10);
                    PEL_LINK(c->put_hevc_qpel, 1, 1, 0, qpel_v4 , 10);
                    PEL_LINK(c->put_hevc_qpel, 3, 1, 0, qpel_v8 , 10);
                    PEL_LINK(c->put_hevc_qpel, 4, 1, 0, qpel_v12, 10);
                    PEL_LINK(c->put_hevc_qpel, 5, 1, 0, qpel_v16, 10);
                    PEL_LINK(c->put_hevc_qpel, 6, 1, 0, qpel_v24, 10);
                    PEL_LINK(c->put_hevc_qpel, 7, 1, 0, qpel_v32, 10);
                    PEL_LINK(c->put_hevc_qpel, 8, 1, 0, qpel_v48, 10);
                    PEL_LINK(c->put_hevc_qpel, 9, 1, 0, qpel_v64, 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 1, 1, 1, qpel_hv4 , 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 3, 1, 1, qpel_hv8 , 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 4, 1, 1, qpel_hv12, 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 5, 1, 1, qpel_hv16, 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 6, 1, 1, qpel_hv24, 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 7, 1, 1, qpel_hv32, 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 8, 1, 1, qpel_hv48, 10);
                    PEL_LINK_SSE(c->put_hevc_qpel, 9, 1, 1, qpel_hv64, 10);

                    c->sao_edge_filter[0] = ff_hevc_sao_edge_filter_0_10_sse;
                    c->sao_edge_filter[1] = ff_hevc_sao_edge_filter_1_10_sse;
                    c->sao_band_filter    = ff_hevc_sao_band_filter_0_10_sse;
                }
#endif //__SSE4_1__
                if (EXTERNAL_AVX(mm_flags)) {
                }
            }
        }
    }
}
