#ifndef X264_RDO_H
#define X264_RDO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "common/common.h"

static inline int ssd_mb(x264_t* h);
static int x264_rd_cost_mb(x264_t* h, int i_lambda2);
static uint64_t x264_rd_cost_i4x4(x264_t* h, int i_lambda2, int i4, int i_mode);
static uint64_t x264_rd_cost_i8x8(x264_t* h, int i_lambda2, int i8, int i_mode);
static uint64_t x264_rd_cost_i8x8_chroma(x264_t* h, int i_lambda2, int i_mode,
                                         int b_dct);

uint64_t x264_rd_cost_part(x264_t* h, int i_lambda2, int i4, int i_pixel);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif
