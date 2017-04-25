/*****************************************************************************
 * analyse.h: h264 encoder library
 *****************************************************************************
 * Copyright (C) 2003-2008 x264 project
 *
 * Authors: Laurent Aimar <fenrir@via.ecp.fr>
 *          Loren Merritt <lorenm@u.washington.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/

#ifndef X264_ANALYSE_H
#define X264_ANALYSE_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "common/common.h"
#include "macroblock.h"
#include "me.h"
#include "ratecontrol.h"
#include "rdo.h"

void x264_macroblock_analyse(x264_t* h);
// void x264_slicetype_decide(x264_t* h);

typedef struct {
  /* 16x16 */
  int i_ref;
  int i_rd16x16;
  x264_me_t me16x16;

  /* 8x8 */
  int i_cost8x8;
  /* [ref][0] is 16x16 mv, [ref][1..4] are 8x8 mv from partition [0..3] */
  DECLARE_ALIGNED_4(int16_t mvc[32][5][2]);
  x264_me_t me8x8[4];

  /* Sub 4x4 */
  int i_cost4x4[4]; /* cost per 8x8 partition */
  x264_me_t me4x4[4][4];

  /* Sub 8x4 */
  int i_cost8x4[4]; /* cost per 8x8 partition */
  x264_me_t me8x4[4][2];

  /* Sub 4x8 */
  int i_cost4x8[4]; /* cost per 8x8 partition */
  x264_me_t me4x8[4][2];

  /* 16x8 */
  int i_cost16x8;
  x264_me_t me16x8[2];

  /* 8x16 */
  int i_cost8x16;
  x264_me_t me8x16[2];

} x264_mb_analysis_list_t;

typedef struct {
  /* conduct the analysis using this lamda and QP */
  int i_lambda;
  int i_lambda2;
  int i_qp;
  int16_t* p_cost_mv;
  int i_mbrd;

  /* I: Intra part */
  /* Take some shortcuts in intra search if intra is deemed unlikely */
  int b_fast_intra;
  int b_try_pskip;

  /* Luma part */
  int i_satd_i16x16;
  int i_satd_i16x16_dir[7];
  int i_predict16x16;

  int i_satd_i8x8;
  int i_satd_i8x8_dir[12][4];
  int i_predict8x8[4];

  int i_satd_i4x4;
  int i_predict4x4[16];

  int i_satd_pcm;

  /* Chroma part */
  int i_satd_i8x8chroma;
  int i_satd_i8x8chroma_dir[4];
  int i_predict8x8chroma;

  /* II: Inter part P/B frame */
  x264_mb_analysis_list_t l0;
  x264_mb_analysis_list_t l1;

  int i_cost16x16bi; /* used the same ref and mv as l0 and l1 (at least for now)
                        */
  int i_cost16x16direct;
  int i_cost8x8bi;
  int i_cost8x8direct[4];
  int i_cost16x8bi;
  int i_cost8x16bi;
  int i_rd16x16bi;
  int i_rd16x16direct;
  int i_rd16x8bi;
  int i_rd8x16bi;
  int i_rd8x8bi;

  int i_mb_partition16x8[2]; /* mb_partition_e */
  int i_mb_partition8x16[2];
  int i_mb_type16x8; /* mb_class_e */
  int i_mb_type8x16;

  int b_direct_available;

} x264_mb_analysis_t;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif
