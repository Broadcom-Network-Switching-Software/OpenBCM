/*! \file bcmfp_action_cfg_internal.h
 *
 * Structures, defines to hold and access FP action configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_ACTION_CFG_INTERNAL_H
#define BCMFP_ACTION_CFG_INTERNAL_H

#include <bcmfp/bcmfp_action_internal.h>
#include <bcmfp/bcmfp_pdd.h>

/*! Invalid Action Value. */
#define BCMFP_INVALID_ACTION_VALUE     (-1)

/*! Max number of parts group can be configured into */
#define BCMFP_ACTION_MAX_PARTS 3

/* BCMFP_ACTION_* macros require the following declaration
 * in any function which uses them.
 */
#define BCMFP_ACTION_CFG_DECL \
    int _rv_;         \
    bcmfp_action_cfg_t _action_cfg_

/* Add Action ifno to stage. */
#define BCMFP_ACTION_CFG_ADD(_unit_,                                             \
                             _stage_,                                            \
                             _action_,                                           \
                             _flags_,                                            \
                             _aset_,                                             \
                             _offset1_,                                          \
                             _width1_,                                           \
                             _value1_,                                           \
                             _offset2_,                                          \
                             _width2_,                                           \
                             _value2_,                                           \
                             _offset3_,                                          \
                             _width3_,                                           \
                             _value3_,                                           \
                             _offset4_,                                          \
                             _width4_,                                           \
                             _value4_,                                           \
                             _offset5_,                                          \
                             _width5_,                                           \
                             _value5_)                                           \
            do {                                                                 \
                _rv_ = bcmfp_action_cfg_t_init(unit, &(_action_cfg_));           \
                SHR_IF_ERR_EXIT(_rv_);                                           \
                (_action_cfg_).flags                 |= (_flags_);               \
                (_action_cfg_).offset_cfg.offset[0]   = (_offset1_);             \
                (_action_cfg_).offset_cfg.width[0]    = (_width1_);              \
                (_action_cfg_).offset_cfg.value[0]    = (_value1_);              \
                (_action_cfg_).offset_cfg.offset[1]   = (_offset2_);             \
                (_action_cfg_).offset_cfg.width[1]    = (_width2_);              \
                (_action_cfg_).offset_cfg.value[1]    = (_value2_);              \
                (_action_cfg_).offset_cfg.offset[2]   = (_offset3_);             \
                (_action_cfg_).offset_cfg.width[2]    = (_width3_);              \
                (_action_cfg_).offset_cfg.value[2]    = (_value3_);              \
                (_action_cfg_).offset_cfg.offset[3]   = (_offset4_);             \
                (_action_cfg_).offset_cfg.width[3]    = (_width4_);              \
                (_action_cfg_).offset_cfg.value[3]    = (_value4_);              \
                (_action_cfg_).offset_cfg.offset[4]   = (_offset5_);             \
                (_action_cfg_).offset_cfg.width[4]    = (_width5_);              \
                (_action_cfg_).offset_cfg.value[4]    = (_value5_);              \
                (_action_cfg_).next      = NULL;                                 \
                _rv_ = bcmfp_action_cfg_insert((_unit_),                         \
                                               (_stage_),                        \
                                               (_action_),                       \
                                               &(_action_cfg_));                 \
                SHR_IF_ERR_EXIT(_rv_);                                           \
            } while (0)

/* Add single offset, width, value for action. */
#define BCMFP_ACTION_CFG_ADD_ONE(_unit_, _stage_, _action_, _flags_, _aset_,     \
                                 _offset1_, _width1_, _value1_)                  \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                          (_flags_), (_aset_),                   \
                                          (_offset1_), (_width1_),               \
                                          (_value1_), 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                                          0, 0, 0);

#define BCMFP_ACTION_CFG_ADD_TWO(_unit_, _stage_, _action_, _flags_, _aset_,     \
                                 _offset1_, _width1_, _value1_,                  \
                                 _offset2_, _width2_, _value2_)                  \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                          (_flags_), (_aset_),                   \
                                          (_offset1_), (_width1_),               \
                                          (_value1_), (_offset2_), (_width2_),   \
                                          (_value2_), 0, 0, 0, 0, 0, 0,          \
                                          0, 0, 0);

#define BCMFP_ACTION_CFG_ADD_THREE(_unit_, _stage_, _action_, _flags_, _aset_,   \
                                        _offset1_, _width1_, _value1_,           \
                                        _offset2_, _width2_, _value2_,           \
                                        _offset3_, _width3_, _value3_)           \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                          (_flags_), (_aset_),                   \
                                          (_offset1_), (_width1_),               \
                                          (_value1_), (_offset2_), (_width2_),   \
                                          (_value2_), (_offset3_), (_width3_),   \
                                          (_value3_), 0, 0, 0,                   \
                                          0, 0, 0);

#define BCMFP_ACTION_CFG_ADD_FOUR(_unit_, _stage_, _action_, _flags_, _aset_,    \
                                        _offset1_, _width1_, _value1_,           \
                                        _offset2_, _width2_, _value2_,           \
                                        _offset3_, _width3_, _value3_,           \
                                        _offset4_, _width4_, _value4_)           \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                      (_flags_), (_aset_),                       \
                                      (_offset1_), (_width1_),                   \
                                      (_value1_), (_offset2_), (_width2_),       \
                                      (_value2_), (_offset3_), (_width3_),       \
                                      (_value3_), (_offset4_), (_width4_),       \
                                      (_value4_), 0, 0, 0)

#define BCMFP_ACTION_CFG_ADD_FIVE(_unit_, _stage_, _action_, _flags_, _aset_,    \
                                        _offset1_, _width1_, _value1_,           \
                                        _offset2_, _width2_, _value2_,           \
                                        _offset3_, _width3_, _value3_,           \
                                        _offset4_, _width4_, _value4_,           \
                                        _offset5_, _width5_, _value5_)           \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                      (_flags_), (_aset_),                       \
                                      (_offset1_), (_width1_),                   \
                                      (_value1_), (_offset2_), (_width2_),       \
                                      (_value2_), (_offset3_), (_width3_),       \
                                      (_value3_), (_offset4_), (_width4_),       \
                                      (_value4_), (_offset5_), (_width5_),       \
                                      (_value5_))

/* Action conflict check macro. */
#define BCMFP_ACTIONS_CONFLICT(_val_)\
    if (action == _val_) {\
        return (SHR_E_CONFIG);\
    }

#define BCMFP_ACTION_CHECK(width, param)\
    if ((unsigned)((1 << width) - 1) < param) {\
        return SHR_E_PARAM;\
    }

#define BCMFP_ACTION_OFFSET_GET(_unit_, _stage_, _action_, _a_offset_)\
    do {\
        SHR_IF_ERR_EXIT(bcmfp_action_offset_get((_unit_), (_stage_),\
                    (_action_), (_a_offset_)));\
    } while (0)

#define BCMFP_ACTION_VIEWTYPE_OFFSET_GET(_unit_, _stage_, _action_, \
        _a_offset_,_a_viewtype_) \
    do {\
        SHR_IF_ERR_EXIT(bcmfp_action_viewtype_offset_get((_unit_), (_stage_),\
                    (_action_), (_a_offset_),(_a_viewtype_)));\
    } while (0)

#define BCMFP_ACTION_WIDTH_GET(width, action, stage)\
    if (NULL != stage) {\
        width = stage->action_cfg_db->\
        action_cfg[action]->offset_cfg.width[0];\
    } else {\
        return SHR_E_FAIL;\
    }

#define BCMFP_ACTION_SET(_unit_, _buf_, _a_offset_)\
    do {\
        SHR_IF_ERR_EXIT(bcmfp_action_value_set((_unit_), (_buf_),\
                    (_a_offset_)));\
    } while (0)

/*
 * Typedef:
 *   BCMFP_ACTION_CFG_INIT
 * Purpose:
 *    To re-init the bcmfp_action_cfg_t structure.
 */
#define BCMFP_ACTION_CFG_INIT(_unit_)                                      \
    _rv_ = bcmfp_action_cfg_t_init(_unit_, &(_action_cfg_));               \
    if (SHR_FAILURE(_rv_)) {                                               \
        SHR_ERR_EXIT(_rv_);                                         \
    }

/*
 * Typedef:
 *   BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD
 * Purpose:
 *   To update the fids of the actions conflicting
 *   in single wide mode alone.
 */
#define BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD(_index_,     \
                                                         _fid_)       \
         (_action_cfg_).single_wide_conflict_fid[_index_] = (_fid_);  \
         if (_index_ >= BCMFP_ACTION_MAX_SINGLEWIDE_CONFLICT) {       \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                            \
         }

/*
 * Typedef:
 *   BCMFP_ACTION_CFG_OFFSET_ADD
 * Purpose:
 *   To update offset and width of the
 *   bcmfp_action_cfg_t variable based on the index.
 */
#define BCMFP_ACTION_CFG_OFFSET_ADD(_index_,                              \
                                   _container_,                           \
                                   _offset_,                              \
                                   _width_,                               \
                                   _value_)                               \
         (_action_cfg_).offset_cfg.phy_cont_id[_index_] = (_container_);  \
         (_action_cfg_).offset_cfg.offset[_index_]   = (_offset_);        \
         (_action_cfg_).offset_cfg.width[_index_]    = (_width_);         \
         (_action_cfg_).offset_cfg.value[_index_]    = (_value_);

/*
 * Typedef:
 *   BCMFP_ACTION_CFG_VIEWTYPE_ADD
 * Purpose:
 *   To update view type of the
 *   bcmfp_action_cfg_t variable based on the index.
 */
#define BCMFP_ACTION_CFG_VIEWTYPE_ADD(_viewtype_)  \
         (_action_cfg_).viewtype = (_viewtype_);

/*
 * Typedef:
 *   BCMFP_ACTION_CFG_OFFSET_SBR_CONT_ID_ADD
 * Purpose:
 *   To update SBR CONT_ID of the
 *   bcmfp_action_cfg_t variable based on the index.
 */
#define BCMFP_ACTION_CFG_OFFSET_SBR_CONT_ID_ADD(_index_,                      \
                                                _container_)                  \
         (_action_cfg_).offset_cfg.sbr_phy_cont_id[_index_] = (_container_);

/*
 * Typedef:
 *   BCMFP_ACTION_CFG_NAME_ADD
 * Purpose:
 *   To update action name of the
 *   bcmfp_action_cfg_t variable based on the index.
 */
#define BCMFP_ACTION_CFG_NAME_ADD(_name_)                                \
         (_action_cfg_).action_name = (_name_);

/*
 * Typedef:
 *  BCMFP_ACTION_CFG_INSERT()
 * Purpose:
 *   To update action configuration its value in the
 *   bcmfp_action_cfg_t structure.
 */
#define BCMFP_ACTION_CFG_INSERT(_unit_, _stage_,                          \
                                _action_id_, _action_flags_)              \
                (_action_cfg_).flags  |= (_action_flags_);                \
                _rv_ = bcmfp_action_cfg_insert((_unit_),                  \
                                             (_stage_),                   \
                                             (_action_id_),               \
                                             &(_action_cfg_));            \
                if (SHR_FAILURE(_rv_)) {                                  \
                    SHR_ERR_EXIT(_rv_);                            \
                }

#define BCMFP_ACTION_DATA_CLEAR(_data_) \
        sal_memset((_data_), 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t))

#endif /* BCMFP_ACTION_CFG_INTERNAL_H */

