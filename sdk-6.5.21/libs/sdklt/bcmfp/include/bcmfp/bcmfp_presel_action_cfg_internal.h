/*! \file bcmfp_presel_action_cfg_internal.h
 *
 * Structures, defines to hold and access FP presel action configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_PRESEL_ACTION_CFG_INTERNAL_H
#define BCMFP_PRESEL_ACTION_CFG_INTERNAL_H

#include <bcmfp/bcmfp_action_internal.h>


/* BCMFP_ACTION_* macros require the following declaration
 * in any function which uses them.
 */
#define BCMFP_PRESEL_ACTION_CFG_DECL \
    int _rv_;         \
    bcmfp_action_cfg_t _action_cfg_

/* Add Action ifno to stage. */
#define BCMFP_PRESEL_ACTION_CFG_ADD(_unit_,                                    \
                             _stage_,                                          \
                             _action_,                                         \
                             _flags_,                                          \
                             _offset1_,                                        \
                             _width1_,                                         \
                             _value1_,                                         \
                             _offset2_,                                        \
                             _width2_,                                         \
                             _value2_,                                         \
                             _offset3_,                                        \
                             _width3_,                                         \
                             _value3_,                                         \
                             _offset4_,                                        \
                             _width4_,                                         \
                             _value4_)                                         \
            do {                                                               \
                _rv_ = bcmfp_presel_action_cfg_t_init(unit, &(_action_cfg_));  \
                if (SHR_FAILURE(_rv_)) {                                       \
                    SHR_ERR_EXIT(_rv_);                                 \
                }                                                              \
                (_action_cfg_).flags                 |= (_flags_);             \
                (_action_cfg_).offset_cfg.offset[0]   = (_offset1_);           \
                (_action_cfg_).offset_cfg.width[0]    = (_width1_);            \
                (_action_cfg_).offset_cfg.value[0]    = (_value1_);            \
                (_action_cfg_).offset_cfg.offset[1]   = (_offset2_);           \
                (_action_cfg_).offset_cfg.width[1]    = (_width2_);            \
                (_action_cfg_).offset_cfg.value[1]    = (_value2_);            \
                (_action_cfg_).offset_cfg.offset[2]   = (_offset3_);           \
                (_action_cfg_).offset_cfg.width[2]    = (_width3_);            \
                (_action_cfg_).offset_cfg.value[2]    = (_value3_);            \
                (_action_cfg_).offset_cfg.offset[3]   = (_offset4_);           \
                (_action_cfg_).offset_cfg.width[3]    = (_width4_);            \
                (_action_cfg_).offset_cfg.value[3]    = (_value4_);            \
                (_action_cfg_).next      = NULL;                               \
                _rv_ = bcmfp_presel_action_cfg_insert((_unit_),                \
                                               (_stage_),                      \
                                               (_action_),                     \
                                               &(_action_cfg_));               \
                if (SHR_FAILURE(_rv_)) {                                       \
                    SHR_ERR_EXIT(_rv_);                                 \
                }                                                              \
            } while (0)

/* Add single offset, width, value for action. */
#define BCMFP_PRESEL_ACTION_CFG_ADD_ONE(_unit_, _stage_, _action_, _flags_,    \
                                _offset1_, _width1_, _value1_)                 \
                    BCMFP_PRESEL_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),\
                                         (_flags_), (_offset1_), (_width1_),   \
                                         (_value1_), 0, 0, 0, 0, 0, 0, 0, 0, 0)

#endif /* BCMFP_PRESEL_ACTION_CFG_INTERNAL_H */
