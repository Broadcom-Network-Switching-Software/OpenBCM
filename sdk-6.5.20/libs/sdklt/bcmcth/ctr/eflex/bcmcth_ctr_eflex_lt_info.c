/*! \file bcmcth_ctr_eflex_lt_info.c
 *
 * BCMCTH Enhanced Flex Counter LT sid and fid info routines
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>

#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Initialize info for ACTION_PROFILE LTs with variant support */
static int
bcmcth_eflex_action_profile_lt_info_init(int unit,
                                         bool ingress,
                                         eflex_comp_t comp)
{
    int rv;
    size_t num_fid = 0, i;
    const char *lt_name = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_action_profile_lt_info_t *lt_info = NULL;
    const bcmlrd_map_t *eflex_map = NULL;
    bcmlrd_table_attrib_t t_attrib = {0};
    bcmlrd_client_field_info_t *f_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    if (ingress) {
        if (comp == CTR_EFLEX) {
            lt_name = "CTR_ING_EFLEX_ACTION_PROFILE";
        } else if (comp == STATE_EFLEX) {
            lt_name = "FLEX_STATE_ING_ACTION_PROFILE";
        }
        lt_info = &ctrl->ing_action_lt_info;
    } else {
        if (comp == CTR_EFLEX) {
            lt_name = "CTR_EGR_EFLEX_ACTION_PROFILE";
        } else if (comp == STATE_EFLEX) {
            lt_name = "FLEX_STATE_EGR_ACTION_PROFILE";
        }
        lt_info = &ctrl->egr_action_lt_info;
    }

    /* Get LT sid from table attributes */
    SHR_IF_ERR_EXIT(bcmlrd_table_attributes_get(unit, lt_name, &t_attrib));

    rv = bcmlrd_map_get(unit, t_attrib.id, &eflex_map);

    if (SHR_SUCCESS(rv) && eflex_map) {
        lt_info->lt_sid = t_attrib.id;
        num_fid = t_attrib.number_of_fields;

        SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
                  "bcmcthEflexActionInfoAllocFields");
        SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_fields_def_get(unit,
                                         lt_name,
                                         num_fid,
                                         f_info,
                                         &num_fid));

        for (i = 0; i < num_fid; i++) {
            if (!sal_strcmp(f_info[i].name, "ACTION")) {
                lt_info->action_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "ZONE")) {
                lt_info->zone_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "PIPE")) {
                lt_info->pipe_idx_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "POOL_ID")) {
                lt_info->pool_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "BASE_INDEX_AUTO")) {
                lt_info->base_idx_auto_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "BASE_INDEX")) {
                lt_info->base_idx_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "BASE_INDEX_OPER")) {
                lt_info->base_idx_oper_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "SHADOW")) {
                lt_info->shadow_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "NUM_COUNTERS")) {
                lt_info->num_ctrs_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "MODE")) {
                lt_info->ctr_mode_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "UPDATE_MODE_A")) {
                lt_info->ctr_update_mode_a_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "UPDATE_MODE_B")) {
                lt_info->ctr_update_mode_b_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "OBJ")) {
                lt_info->obj_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "COUNT_ON_RULE_DROP")) {
                lt_info->count_rule_drop_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_A")) {
                lt_info->ctr_src_a_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_B")) {
                lt_info->ctr_src_b_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "CTR_VAL_DATA")) {
                lt_info->ctr_val_data_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "EVICT_COMPARE")) {
                lt_info->evict_comp_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "EVICT_RESET")) {
                lt_info->evict_reset_fid = f_info[i].id;
            } else if (!sal_strcmp(f_info[i].name, "COUNT_ON_HW_EXCP_DROP")) {
                lt_info->count_hw_drop_fid = f_info[i].id;
            } else if (comp == CTR_EFLEX) {
                if (!sal_strcmp(f_info[i].name, "NUM_COUNTERS_OPER")) {
                    lt_info->num_ctrs_oper_fid = f_info[i].id;
                }

                if (ingress == true) {
                    /* Ingress flex counter fields */
                    if (!sal_strcmp(f_info[i].name,
                                    "CTR_ING_EFLEX_ACTION_PROFILE_ID")) {
                        lt_info->action_profile_id_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "INDEX_CTR_ING_EFLEX_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_index_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "VAL_B_CTR_ING_EFLEX_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_b_fid = f_info[i].id;
                    }
                } else {
                    /* Egress flex counter fields */
                    if (!sal_strcmp(f_info[i].name,
                                    "CTR_EGR_EFLEX_ACTION_PROFILE_ID")) {
                        lt_info->action_profile_id_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "INDEX_CTR_EGR_EFLEX_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_index_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "VAL_B_CTR_EGR_EFLEX_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_b_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name, "COUNT_ON_MIRROR")) {
                        lt_info->count_mirror_egress_fid = f_info[i].id;
                    }
                }
            } else if (comp == STATE_EFLEX) {
                if (!sal_strcmp(f_info[i].name, "UPDATE_COMPARE_A")) {
                    lt_info->state_ext.update_comp_a_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "UPDATE_COMPARE_B")) {
                    lt_info->state_ext.update_comp_b_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "TRUE_UPDATE_MODE_A")) {
                    lt_info->state_ext.upd_true_mode_a_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "TRUE_UPDATE_MODE_B")) {
                    lt_info->state_ext.upd_true_mode_b_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "FALSE_UPDATE_MODE_A")) {
                    lt_info->state_ext.upd_false_mode_a_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "FALSE_UPDATE_MODE_B")) {
                    lt_info->state_ext.upd_false_mode_b_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_COMP_A")) {
                    lt_info->state_ext.ctr_src_comp_a_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_COMP_B")) {
                    lt_info->state_ext.ctr_src_comp_b_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_UPD_TRUE_A")) {
                    lt_info->state_ext.ctr_src_upd_true_a_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_UPD_TRUE_B")) {
                    lt_info->state_ext.ctr_src_upd_true_b_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_UPD_FALSE_A")) {
                    lt_info->state_ext.ctr_src_upd_false_a_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "CTR_SRC_UPD_FALSE_B")) {
                    lt_info->state_ext.ctr_src_upd_false_b_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "VAL_A_MEM_UPDATE")) {
                    lt_info->state_ext.a_mem_update_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "VAL_B_MEM_UPDATE")) {
                    lt_info->state_ext.b_mem_update_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "VAL_A_BUS_UPDATE")) {
                    lt_info->state_ext.a_bus_update_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "VAL_B_BUS_UPDATE")) {
                    lt_info->state_ext.b_bus_update_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "TRUTH_TABLE_EN")) {
                    lt_info->state_ext.truth_table_en_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name,
                                       "TRUTH_TABLE_INSTANCE")) {
                    lt_info->state_ext.truth_table_instance_fid = f_info[i].id;
                }

                if (ingress) {
                    /* Ingress flex state fields */
                    if (!sal_strcmp(f_info[i].name,
                                    "FLEX_STATE_ING_ACTION_PROFILE_ID")) {
                        lt_info->action_profile_id_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "INDEX_FLEX_STATE_ING_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_index_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                            "VAL_A_COMP_FLEX_STATE_ING_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_comp_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                            "VAL_B_COMP_FLEX_STATE_ING_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_comp_b_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_A_UPD_TRUE_FLEX_STATE_ING_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_true_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_B_UPD_TRUE_FLEX_STATE_ING_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_true_b_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_A_UPD_FALSE_FLEX_STATE_ING_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_false_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_B_UPD_FALSE_FLEX_STATE_ING_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_false_b_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name, "INSTANCE")) {
                        lt_info->state_ext.instance_fid = f_info[i].id;
                    }
                } else {
                    /* Egress flex state fields */
                    if (!sal_strcmp(f_info[i].name,
                                    "FLEX_STATE_EGR_ACTION_PROFILE_ID")) {
                        lt_info->action_profile_id_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                                "INDEX_FLEX_STATE_EGR_OPERAND_PROFILE_ID")) {
                        lt_info->op_profile_id_index_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                            "VAL_A_COMP_FLEX_STATE_EGR_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_comp_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                            "VAL_B_COMP_FLEX_STATE_EGR_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_comp_b_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_A_UPD_TRUE_FLEX_STATE_EGR_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_true_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_B_UPD_TRUE_FLEX_STATE_EGR_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_true_b_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_A_UPD_FALSE_FLEX_STATE_EGR_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_false_a_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name,
                        "VAL_B_UPD_FALSE_FLEX_STATE_EGR_OPERAND_PROFILE_ID")) {
                        lt_info->state_ext.op_id_upd_false_b_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name, "COUNT_ON_MIRROR")) {
                        lt_info->count_mirror_egress_fid = f_info[i].id;
                    } else if (!sal_strcmp(f_info[i].name, "INSTANCE")) {
                        lt_info->state_ext.egr_instance_fid = f_info[i].id;
                        lt_info->state_ext.egr_instance_fid_valid = true;
                    }
                }
            } /* end (comp == STATE_EFLEX)*/
        } /* end FOR */
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

static int
bcmcth_eflex_obj_info_lt_info_init(int unit, bool ingress, eflex_comp_t comp)
{
    int rv;
    size_t num_fid = 0, i;
    const char *lt_name = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_obj_info_lt_info_t *lt_info = NULL;
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    bcmlrd_table_attrib_t t_attrib = {0};
    bcmlrd_client_field_info_t *f_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    if (ingress) {
        if (comp == CTR_EFLEX) {
            lt_name = "CTR_ING_EFLEX_OBJ_INFO";
        } else if (comp == STATE_EFLEX) {
            lt_name = "FLEX_STATE_ING_OBJ_INFO";
        }
        lt_info = &ctrl->ing_obj_info_lt_info;
    } else {
        if (comp == CTR_EFLEX) {
            lt_name = "CTR_EGR_EFLEX_OBJ_INFO";
        } else if (comp == STATE_EFLEX) {
            lt_name = "FLEX_STATE_EGR_OBJ_INFO";
        }
        lt_info = &ctrl->egr_obj_info_lt_info;
    }

    /* Get LT sid from table attributes */
    rv = bcmlrd_table_attributes_get(unit, lt_name, &t_attrib);

    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, t_attrib.id, &ctr_eflex_map);

        if (SHR_SUCCESS(rv) && ctr_eflex_map) {
            lt_info->supported = true;
            lt_info->lt_sid = t_attrib.id;
            num_fid = t_attrib.number_of_fields;

            SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
                      "bcmcthEflexObjInfoAllocFields");
            SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlrd_table_fields_def_get(unit,
                                             lt_name,
                                             num_fid,
                                             f_info,
                                             &num_fid));


            for (i = 0; i < num_fid; i++) {
                if (!sal_strcmp(f_info[i].name, "OBJ")) {
                    lt_info->obj_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "MIN_BIT")) {
                    lt_info->min_bit_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "MAX_BIT")) {
                    lt_info->max_bit_fid = f_info[i].id;
                }
            } /* end FOR */
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

static int
bcmcth_eflex_obj_quant_lt_info_init(int unit, bool ingress, eflex_comp_t comp)
{
    int rv;
    size_t num_fid = 0, i;
    const char *lt_name = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_obj_quant_lt_info_t *lt_info = NULL;
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    bcmlrd_table_attrib_t t_attrib = {0};
    bcmlrd_client_field_info_t *f_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    if (ingress) {
        if (comp == CTR_EFLEX) {
            lt_name = "CTR_ING_EFLEX_OBJ_QUANTIZATION";
        } else if (comp == STATE_EFLEX) {
            lt_name = "FLEX_STATE_ING_OBJ_QUANTIZATION";
        }
        lt_info = &ctrl->ing_obj_quant_lt_info;
    } else {
        if (comp == CTR_EFLEX) {
            lt_name = "CTR_EGR_EFLEX_OBJ_QUANTIZATION";
        } else if (comp == STATE_EFLEX) {
            lt_name = "FLEX_STATE_EGR_OBJ_QUANTIZATION";
        }
        lt_info = &ctrl->egr_obj_quant_lt_info;
    }

    /* Get LT sid from table attributes */
    rv = bcmlrd_table_attributes_get(unit, lt_name, &t_attrib);

    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, t_attrib.id, &ctr_eflex_map);

        if (SHR_SUCCESS(rv) && ctr_eflex_map) {
            lt_info->supported = true;
            lt_info->lt_sid = t_attrib.id;
            num_fid = t_attrib.number_of_fields;

            SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
                      "bcmcthEflexObjInfoAllocFields");
            SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlrd_table_fields_def_get(unit,
                                             lt_name,
                                             num_fid,
                                             f_info,
                                             &num_fid));


            for (i = 0; i < num_fid; i++) {
                if (!sal_strcmp(f_info[i].name, "PIPE")) {
                    lt_info->pipe_idx_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "INSTANCE")) {
                    lt_info->instance_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "QUANTIZE")) {
                    lt_info->quantize_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "OBJ")) {
                    lt_info->obj_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "OBJ_0_1_MODE")) {
                    lt_info->obj_0_1_mode_fid = f_info[i].id;
                } else if (!sal_strcmp(f_info[i].name, "OBJ_2_3_MODE")) {
                    lt_info->obj_2_3_mode_fid = f_info[i].id;
                }
            } /* end FOR */
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*! Initialize info for CTR_xxx_EFLEX LTs with variant support */
int
bcmcth_ctr_eflex_lt_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize CTR_ING_EFLEX_ACTION_PROFILE LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_action_profile_lt_info_init(unit, true, CTR_EFLEX));

    /* Initialize CTR_EGR_EFLEX_ACTION_PROFILE LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_action_profile_lt_info_init(unit, false, CTR_EFLEX));

    /* Initialize CTR_ING_EFLEX_OBJ_INFO LT info */
    SHR_IF_ERR_EXIT(bcmcth_eflex_obj_info_lt_info_init(unit, true, CTR_EFLEX));

    /* Initialize CTR_EGR_EFLEX_OBJ_INFO LT info */
    SHR_IF_ERR_EXIT(bcmcth_eflex_obj_info_lt_info_init(unit, false, CTR_EFLEX));

    /* Initialize CTR_ING_EFLEX_OBJ_QUANTIZATION LT info */
    SHR_IF_ERR_EXIT(bcmcth_eflex_obj_quant_lt_info_init(unit, true, CTR_EFLEX));

    /* Initialize CTR_EGR_EFLEX_OBJ_QUANTIZATION LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_obj_quant_lt_info_init(unit, false, CTR_EFLEX));

exit:
    SHR_FUNC_EXIT();
}

/*! Initialize info for FLEX_STATE_xxx LTs with variant support */
int
bcmcth_state_eflex_lt_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize FLEX_STATE_ING_ACTION_PROFILE LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_action_profile_lt_info_init(unit, true, STATE_EFLEX));

    /* Initialize FLEX_STATE_EGR_ACTION_PROFILE LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_action_profile_lt_info_init(unit, false, STATE_EFLEX));

    /* Initialize FLEX_STATE_ING_OBJ_INFO LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_obj_info_lt_info_init(unit, true, STATE_EFLEX));

    /* Initialize FLEX_STATE_EGR_OBJ_INFO LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_obj_info_lt_info_init(unit, false, STATE_EFLEX));

    /* Initialize FLEX_STATE_ING_OBJ_QUANTIZATION LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_obj_quant_lt_info_init(unit, true, STATE_EFLEX));

    /* Initialize FLEX_STATE_EGR_OBJ_QUANTIZATION LT info */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_obj_quant_lt_info_init(unit, false, STATE_EFLEX));

exit:
    SHR_FUNC_EXIT();
}
