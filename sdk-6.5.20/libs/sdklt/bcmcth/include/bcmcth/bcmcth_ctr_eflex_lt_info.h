/*! \file bcmcth_ctr_eflex_lt_info.h
 *
 * This file contains defines which are internal to CTR EFLEX and used to
 * get information about LTs with variant support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_EFLEX_LT_INFO_H
#define BCMCTH_CTR_EFLEX_LT_INFO_H

#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief NPL variant dependent FLEX_STATE_xxx_ACTION_PROFILE
 * LT fid info extended structure
 */
typedef struct state_eflex_action_profile_lt_info_ext_s {
    /*! INSTANCE fid */
    bcmlrd_fid_t instance_fid;

    /*! EGR_INSTANCE fid */
    bcmlrd_fid_t egr_instance_fid;

    /*! EGR_INSTANCE fid valid */
    bool egr_instance_fid_valid;

    /*! UPDATE_COMPARE_A fid */
    bcmlrd_fid_t update_comp_a_fid;

    /*! UPDATE_COMPARE_B fid */
    bcmlrd_fid_t update_comp_b_fid;

    /*! TRUE_UPDATE_MODE_A fid */
    bcmlrd_fid_t upd_true_mode_a_fid;

    /*! TRUE_UPDATE_MODE_B fid */
    bcmlrd_fid_t upd_true_mode_b_fid;

    /*! FALSE_UPDATE_MODE_A fid */
    bcmlrd_fid_t upd_false_mode_a_fid;

    /*! FALSE_UPDATE_MODE_B fid */
    bcmlrd_fid_t upd_false_mode_b_fid;

    /*! VAL_A_COMP_FLEX_STATE_xxx_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_id_comp_a_fid;

    /*! VAL_B_COMP_FLEX_STATE_xxx_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_id_comp_b_fid;

    /*! VAL_A_UPD_TRUE_FLEX_STATE_xxx_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_id_upd_true_a_fid;

    /*! VAL_B_UPD_TRUE_FLEX_STATE_xxx_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_id_upd_true_b_fid;

    /*! VAL_A_UPD_FALSE_FLEX_STATE_xxx_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_id_upd_false_a_fid;

    /*! VAL_B_UPD_FALSE_FLEX_STATE_xxx_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_id_upd_false_b_fid;

    /*! CTR_SRC_COMP_A fid */
    bcmlrd_fid_t ctr_src_comp_a_fid;

    /*! CTR_SRC_COMP_B fid */
    bcmlrd_fid_t ctr_src_comp_b_fid;

    /*! CTR_SRC_UPD_TRUE_A fid */
    bcmlrd_fid_t ctr_src_upd_true_a_fid;

    /*! CTR_SRC_UPD_TRUE_B fid */
    bcmlrd_fid_t ctr_src_upd_true_b_fid;

    /*! CTR_SRC_UPD_FALSE_A fid */
    bcmlrd_fid_t ctr_src_upd_false_a_fid;

    /*! CTR_SRC_UPD_FALSE_B fid */
    bcmlrd_fid_t ctr_src_upd_false_b_fid;

    /*! VAL_A_MEM_UPDATE fid */
    bcmlrd_fid_t a_mem_update_fid;

    /*! VAL_B_MEM_UPDATE fid */
    bcmlrd_fid_t b_mem_update_fid;

    /*! VAL_A_BUS_UPDATE fid */
    bcmlrd_fid_t a_bus_update_fid;

    /*! VAL_B_BUS_UPDATE fid */
    bcmlrd_fid_t b_bus_update_fid;

    /*! TRUTH_TABLE_EN fid */
    bcmlrd_fid_t truth_table_en_fid;

    /*! TRUTH_TABLE_INSTANCE fid */
    bcmlrd_fid_t truth_table_instance_fid;
} state_eflex_action_profile_lt_info_ext_t;

/*!
 * \brief NPL variant dependent CTR_xxx_EFLEX_ACTION_PROFILE
 * LT sid and fid info structure
 */
typedef struct ctr_eflex_action_profile_lt_info_s {
    /*! ACTION_PROFILE LT sid */
    bcmlrd_sid_t lt_sid;

    /*! ACTION_PROFILE_ID fid */
    bcmlrd_fid_t action_profile_id_fid;

    /*! ACTION fid */
    bcmlrd_fid_t action_fid;

    /*! ZONE fid */
    bcmlrd_fid_t zone_fid;

    /*! PIPE fid */
    bcmlrd_fid_t pipe_idx_fid;

    /*! POOL_ID fid */
    bcmlrd_fid_t pool_fid;

    /*! BASE_INDEX_AUTO fid */
    bcmlrd_fid_t base_idx_auto_fid;

    /*! BASE_INDEX fid */
    bcmlrd_fid_t base_idx_fid;

    /*! BASE_INDEX_OPER fid */
    bcmlrd_fid_t base_idx_oper_fid;

    /*! SHADOW fid */
    bcmlrd_fid_t shadow_fid;

    /*! NUM_COUNTERS fid */
    bcmlrd_fid_t num_ctrs_fid;

    /*! NUM_COUNTERS_OPER fid */
    bcmlrd_fid_t num_ctrs_oper_fid;

    /*! MODE fid */
    bcmlrd_fid_t ctr_mode_fid;

    /*! UPDATE_MODE_A fid */
    bcmlrd_fid_t ctr_update_mode_a_fid;

    /*! UPDATE_MODE_B fid */
    bcmlrd_fid_t ctr_update_mode_b_fid;

    /*! OBJ fid */
    bcmlrd_fid_t obj_fid;

    /*! INDEX_CTR_xxx_EFLEX_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_profile_id_index_fid;

    /*! VAL_A_CTR_xxx_EFLEX_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_profile_id_a_fid;

    /*! VAL_B_CTR_xxx_EFLEX_OPERAND_PROFILE_ID fid */
    bcmlrd_fid_t op_profile_id_b_fid;

    /*! COUNT_ON_RULE_DROP fid */
    bcmlrd_fid_t count_rule_drop_fid;

    /*! CTR_SRC_A fid */
    bcmlrd_fid_t ctr_src_a_fid;

    /*! CTR_SRC_B fid */
    bcmlrd_fid_t ctr_src_b_fid;

    /*! CTR_VAL_DATA fid */
    bcmlrd_fid_t ctr_val_data_fid;

    /*! EVICT_COMPARE fid */
    bcmlrd_fid_t evict_comp_fid;

    /*! EVICT_RESET fid */
    bcmlrd_fid_t evict_reset_fid;

    /*! COUNT_ON_HW_EXCP_DROP fid */
    bcmlrd_fid_t count_hw_drop_fid;

    /*! COUNT_ON_MIRROR fid */
    bcmlrd_fid_t count_mirror_egress_fid;

    /*! Flex state extended ACTION_PROFILE info */
    state_eflex_action_profile_lt_info_ext_t state_ext;
} ctr_eflex_action_profile_lt_info_t;
/*!
 * \brief NPL variant dependent CTR_xxx_EFLEX_OBJ_INFO
 * LT sid and fid info structure
 */
typedef struct ctr_eflex_obj_info_lt_info_s {
    /*! OBJ_INFO LT sid */
    bcmlrd_sid_t lt_sid;

    /*! OBJ_INFO LT supported */
    bool supported;

    /*! OBJ fid */
    bcmlrd_fid_t obj_fid;

    /*! MIN_BIT fid */
    bcmlrd_fid_t min_bit_fid;

    /*! MAX_BIT fid */
    bcmlrd_fid_t max_bit_fid;
} ctr_eflex_obj_info_lt_info_t;

/*!
 * \brief NPL variant dependent CTR_xxx_EFLEX_OBJ_QUANTIZATION
 * LT sid and fid info structure
 */
typedef struct ctr_eflex_obj_quant_lt_info_s {
    /*! OBJ_QUANTIZATION LT sid */
    bcmlrd_sid_t lt_sid;

    /*! OBJ_QUANTIZATION LT supported */
    bool supported;

    /*! PIPE fid */
    bcmlrd_fid_t pipe_idx_fid;

    /*! INSTANCE fid */
    bcmlrd_fid_t instance_fid;

    /*! QUANTIZE fid */
    bcmlrd_fid_t quantize_fid;

    /*! OBJ fid */
    bcmlrd_fid_t obj_fid;

    /*! OBJ_0_1_MODE fid */
    bcmlrd_fid_t obj_0_1_mode_fid;

    /*! OBJ_2_3_MODE fid */
    bcmlrd_fid_t obj_2_3_mode_fid;
} ctr_eflex_obj_quant_lt_info_t;

#endif /* BCMCTH_CTR_EFLEX_LT_INFO_H */
