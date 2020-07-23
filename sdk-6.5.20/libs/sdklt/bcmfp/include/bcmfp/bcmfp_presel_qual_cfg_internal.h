/*! \file bcmfp_presel_qual_cfg_internal.h
 *
 * Structures, defines to hold and access FP presel qualifier configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_PRESEL_QUAL_CFG_INTERNAL_H
#define BCMFP_PRESEL_QUAL_CFG_INTERNAL_H

#include <bcmfp/bcmfp_ext_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BCMFP_PRESEL_QUAL_CFG_DECL                \
    int _rv_;                                     \
    bcmfp_qual_cfg_t _qual_cfg_

/*
 * Typedef:
 *    BCMFP_QUAL_CFG_ADD
 * Purpose:
 *    Qualifier ibus information add macros.
 */
#define BCMFP_PRESEL_QUAL_CFG_ADD(_unit_,                              \
                           _stage_,                                    \
                           _qual_id_,                                  \
                           _qual_flags_,                               \
                           _ctrl_sel_,                                 \
                           _ctrl_sel_val_,                             \
                           _sec0_, _sec0_val_,                         \
                           _sec1_, _sec1_val_,                         \
                           _sec2_, _sec2_val_,                         \
                           _sec3_, _sec3_val_,                         \
                           _sec4_, _sec4_val_,                         \
                           _sec5_, _sec5_val_,                         \
                           _sec6_, _sec6_val_,                         \
                           _sec7_, _sec7_val_,                         \
                           _width0_, _width1_,                         \
                           _width2_, _width3_,                         \
                           _width4_, _width5_,                         \
                           _width6_, _width7_,                         \
                           _chunk_offset0_, _chunk_offset1_,           \
                           _chunk_offset2_, _chunk_offset3_,           \
                           _chunk_offset4_, _chunk_offset5_,           \
                           _chunk_offset6_, _chunk_offset7_,           \
                           _bus_offset0_, _bus_offset1_,               \
                           _bus_offset2_, _bus_offset3_,               \
                           _bus_offset4_, _bus_offset5_,               \
                           _bus_offset6_, _bus_offset7_,               \
                           _num_chunks_)                               \
            do {                                                             \
                _rv_ = bcmfp_presel_qual_cfg_t_init(_unit_, &(_qual_cfg_));  \
                if (SHR_FAILURE(_rv_)) {                                     \
                    SHR_ERR_EXIT(_rv_);                               \
                }                                                            \
                (_qual_cfg_).ctrl_sel                   = (_ctrl_sel_);      \
                (_qual_cfg_).ctrl_sel_val               = (_ctrl_sel_val_);  \
                (_qual_cfg_).e_params[0].section      = (_sec0_);            \
                (_qual_cfg_).e_params[0].sec_val      = (_sec0_val_);        \
                (_qual_cfg_).e_params[0].chunk_offset = (_chunk_offset0_);   \
                (_qual_cfg_).e_params[0].width        = (_width0_);          \
                (_qual_cfg_).e_params[0].bus_offset   = (_bus_offset0_);     \
                (_qual_cfg_).e_params[1].section      = (_sec1_);            \
                (_qual_cfg_).e_params[1].sec_val      = (_sec1_val_);        \
                (_qual_cfg_).e_params[1].chunk_offset = (_chunk_offset1_);   \
                (_qual_cfg_).e_params[1].width        = (_width1_);          \
                (_qual_cfg_).e_params[1].bus_offset   = (_bus_offset1_);     \
                (_qual_cfg_).e_params[2].section      = (_sec2_);            \
                (_qual_cfg_).e_params[2].sec_val      = (_sec2_val_);        \
                (_qual_cfg_).e_params[2].chunk_offset = (_chunk_offset2_);   \
                (_qual_cfg_).e_params[2].width        = (_width2_);          \
                (_qual_cfg_).e_params[2].bus_offset   = (_bus_offset2_);     \
                (_qual_cfg_).e_params[3].section      = (_sec3_);            \
                (_qual_cfg_).e_params[3].sec_val      = (_sec3_val_);        \
                (_qual_cfg_).e_params[3].chunk_offset = (_chunk_offset3_);   \
                (_qual_cfg_).e_params[3].width        = (_width3_);          \
                (_qual_cfg_).e_params[3].bus_offset   = (_bus_offset3_);     \
                (_qual_cfg_).e_params[4].section      = (_sec4_);            \
                (_qual_cfg_).e_params[4].sec_val      = (_sec4_val_);        \
                (_qual_cfg_).e_params[4].chunk_offset = (_chunk_offset4_);   \
                (_qual_cfg_).e_params[4].width        = (_width4_);          \
                (_qual_cfg_).e_params[4].bus_offset   = (_bus_offset4_);     \
                (_qual_cfg_).e_params[5].section      = (_sec5_);            \
                (_qual_cfg_).e_params[5].sec_val      = (_sec5_val_);        \
                (_qual_cfg_).e_params[5].chunk_offset = (_chunk_offset5_);   \
                (_qual_cfg_).e_params[5].width        = (_width5_);          \
                (_qual_cfg_).e_params[5].bus_offset   = (_bus_offset5_);     \
                (_qual_cfg_).e_params[6].section      = (_sec6_);            \
                (_qual_cfg_).e_params[6].sec_val      = (_sec6_val_);        \
                (_qual_cfg_).e_params[6].chunk_offset = (_chunk_offset6_);   \
                (_qual_cfg_).e_params[6].width        = (_width6_);          \
                (_qual_cfg_).e_params[6].bus_offset   = (_bus_offset6_);     \
                (_qual_cfg_).e_params[7].section      = (_sec7_);            \
                (_qual_cfg_).e_params[7].sec_val      = (_sec7_val_);        \
                (_qual_cfg_).e_params[7].chunk_offset = (_chunk_offset7_);   \
                (_qual_cfg_).e_params[7].width        = (_width7_);          \
                (_qual_cfg_).e_params[7].bus_offset   = (_bus_offset7_);     \
                (_qual_cfg_).size = ((_width0_) + (_width1_) + (_width2_) +  \
                                    (_width3_) + (_width4_) + (_width5_) +   \
                                    (_width6_) + (_width7_));                \
                (_qual_cfg_).num_chunks = (_num_chunks_);                    \
                _rv_ = bcmfp_presel_qual_cfg_insert((_unit_),                \
                                             (_stage_),                      \
                                             (_qual_id_),                    \
                                             &(_qual_cfg_));                 \
                if (SHR_FAILURE(_rv_)) {                                     \
                    SHR_ERR_EXIT(_rv_);                               \
                }                                                            \
            } while(0)

#define BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(_unit_,                  \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_)                \
            BCMFP_PRESEL_QUAL_CFG_ADD(_unit_,                         \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               BCMFP_EXT_CTRL_SEL_DISABLE, 0,         \
                               _sec0_,  _sec0_val_,                   \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _width0_,                              \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _chunk_offset0_,                       \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _bus_offset0_,                         \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               1)

#endif /* BCMFP_PRESEL_QUAL_CFG_INTERNAL_H */
