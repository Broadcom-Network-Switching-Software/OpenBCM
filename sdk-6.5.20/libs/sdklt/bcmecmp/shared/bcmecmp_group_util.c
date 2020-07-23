/*! \file bcmecmp_group_util.c
 *
 * ECMP group shared functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmecmp/bcmecmp_util.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_types.h>
#include <bcmecmp/bcmecmp_member_dest.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief ECMP_UNDERLAY LT entry fields fill routine.
 *
 * Parse ECMP_UNDERLAY logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in_fld Logical table database input field array.
 * \param [out] ltentry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c ltentry.
 */
int
bcmecmp_underlay_grp_lt_fields_fill(int unit,
                                    const void *in_fld,
                                    void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_lt_entry_t *lt_entry = ltentry;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    switch (fid) {
        case ECMP_UNDERLAYt_ECMP_IDf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (bcmecmp_id_t)fval;
            break;

        case ECMP_UNDERLAYt_LB_MODEf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE);
            lt_entry->lb_mode = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_NHOP_SORTEDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED);
            lt_entry->nhop_sorted = fval;
            break;

        case ECMP_UNDERLAYt_MAX_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
            lt_entry->max_paths = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_NHOP_IDf:
            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_UNDERLAYt_MON_AGM_POOLf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_POOL);
            lt_entry->agm_pool = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_MON_AGM_IDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_ID);
            lt_entry->agm_id = (uint32_t) fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_OVERLAY LT entry fields fill routine.
 *
 * Parse ECMP_OVERLAY logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in_fld Logical table database input field array.
 * \param [out] ltentry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c ltentry.
 */
int
bcmecmp_overlay_grp_lt_fields_fill(int unit,
                                   const void *in_fld,
                                   void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_lt_entry_t *lt_entry = ltentry;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    switch (fid) {
        case ECMP_OVERLAYt_ECMP_IDf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (bcmecmp_id_t) fval;
            break;

        case ECMP_OVERLAYt_LB_MODEf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE);
            lt_entry->lb_mode = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_NHOP_SORTEDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED);
            lt_entry->nhop_sorted = fval;
            break;

        case ECMP_OVERLAYt_MAX_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
            lt_entry->max_paths = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_ECMP_NHOPf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP);
            lt_entry->ecmp_nhop = fval;
            break;

        case ECMP_OVERLAYt_ECMP_UNDERLAY_IDf:
            /*
             * Ignore default value for ECMP_UNDERLAY_ID when mismatch with LB.
             * This is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->uecmp_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID);
            lt_entry->uecmp_id[idx] = (int) fval;

            /* Increment parsed ECMP_UNDERLAY_IDs count. */
            lt_entry->uecmp_ids_count += 1;
            break;

        case ECMP_OVERLAYt_RH_ECMP_UNDERLAY_IDf:
            /*
             * Ignore default value for RH_ECMP_UNDERLAY_ID when mismatch with
             *  LB. This is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->uecmp_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_UECMP_ID);
            lt_entry->uecmp_id[idx] = (int) fval;

            /* Increment parsed ECMP_UNDERLAY_IDs count. */
            lt_entry->uecmp_ids_count += 1;
            break;

        case ECMP_OVERLAYt_NHOP_IDf:
            /*
             * Ignore default value for NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_OVERLAYt_RH_NHOP_IDf:
            /*
             * Ignore default value for RH_NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_OVERLAYt_RH_SIZEf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE);
            lt_entry->rh_size_enc = (uint32_t)fval;
            lt_entry->rh_entries_cnt = (uint32_t)(1 <<
                                                    lt_entry->rh_size_enc);
                break;

        case ECMP_OVERLAYt_RH_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS);
            lt_entry->rh_num_paths = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_RH_RANDOM_SEEDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED);
            lt_entry->rh_rand_seed = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_MON_AGM_POOLf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_POOL);
            lt_entry->agm_pool = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_MON_AGM_IDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_ID);
            lt_entry->agm_id = (uint32_t) fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether the given group is in use and returns the status.
 *
 * \param [in] unit Unit number.
 * \param [in] grp_id Group ID.
 * \param [in] memb_dest Specifies whether this group is member destination.
 * \param [in] level Specifies whether this call is for level 0 or 1.
 * \param [in] gtype Group type.
 * \param [out] in_use Specifies whether this group is in use.
 * \param [out] component Component ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to get the group information.
 */
int
bcmecmp_grp_in_use(int unit,
                   uint32_t grp_id,
                   bool memb_dest,
                   int level,
                   bcmecmp_grp_type_t gtype,
                   bool *in_use,
                   uint32_t *component)
{
    uint32_t comp_id;
    shr_itbm_list_hdl_t list_hdl;
    uint16_t ref_cnt;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in_use, SHR_E_PARAM);
    SHR_NULL_CHECK(component, SHR_E_PARAM);
    *in_use = FALSE;
    *component = BCMMGMT_MAX_COMP_ID;

    if (!memb_dest) {
        list_hdl = BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit, group, gtype));
        if (list_hdl != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_elem_state_get(list_hdl,
                                              SHR_ITBM_INVALID,
                                              grp_id,
                                              in_use,
                                              &comp_id));
            *component = comp_id;
        } else {
            ref_cnt = BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group, gtype),
                                          grp_id);
            *in_use = (ref_cnt > 0) ? TRUE : FALSE;
            *component = BCMMGMT_ECMP_COMP_ID;
        }
    } else {
        if (level == 0) {
            if (BCMECMP_MEMBER0_GRP_NUM_PATHS(unit, grp_id) > 0) {
                *in_use = TRUE;
            }
        } else {
            if (BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) > 0) {
                *in_use = TRUE;
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether the given group is in use in bake up HA.
 *
 * \param [in] unit Unit number.
 * \param [in] grp_id Group ID.
 * \param [in] memb_dest Specifies whether this group is member destination.
 * \param [in] level Specifies whether this call is for level 0 or 1.
 * \param [in] gtype Group type.
 * \param [out] in_use Specifies whether this group is in use.
 * \param [out] component Component ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to get the group information.
 */
int
bcmecmp_grp_in_use_bk(int unit,
                      uint32_t grp_id,
                      bool memb_dest,
                      int level,
                      bcmecmp_grp_type_t gtype,
                      bool *in_use,
                      uint32_t *component)
{
    shr_itbm_list_hdl_t list_hdl;
    uint16_t ref_cnt;
    bcmecmp_info_t *ecmp_info;
    bcmecmp_grp_attr_t *grp_attr = NULL;
    bcmecmp_hw_entry_attr_t *hw_ent_attr_base;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in_use, SHR_E_PARAM);
    SHR_NULL_CHECK(component, SHR_E_PARAM);
    *in_use = FALSE;
    *component = BCMMGMT_MAX_COMP_ID;

    if (!memb_dest) {
        ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
        list_hdl = ecmp_info->group[gtype].list_hdl;
        if (list_hdl != NULL) {
            grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_bk_arr) + grp_id;
            *in_use = grp_attr->in_use_itbm;
        } else {
            hw_ent_attr_base = (bcmecmp_hw_entry_attr_t *)
                               (ecmp_info->group[gtype].ent_bk_arr) + grp_id;
            ref_cnt = hw_ent_attr_base->ref_cnt;
            *in_use = (ref_cnt > 0) ? TRUE : FALSE;
            *component = BCMMGMT_ECMP_COMP_ID;
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates HA memory for storing HW entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of hw entry array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmecmp_hw_entry_ha_alloc(int unit,
                          bool warm,
                          uint32_t array_size,
                          uint8_t submod_id,
                          uint32_t ref_sign,
                          bcmecmp_hw_entry_ha_blk_t **alloc_ptr,
                          uint32_t *out_ha_alloc_sz)
{
    bcmecmp_hw_entry_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size;

    SHR_FUNC_ENTER(unit);
    /* compiler treat bcmecmp_hw_entry_ha_blk_t.array as empty array(size=0) */
    in_req_size = sizeof(bcmecmp_hw_entry_attr_t) * array_size +
                  sizeof(bcmecmp_hw_entry_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_ECMP_COMP_ID,
                                    submod_id,
                                    "ecmpHWEntry",
                                    &ha_alloc_sz);
    SHR_NULL_CHECK(hw_array_ptr, SHR_E_MEMORY);
    SHR_NULL_CHECK(hw_array_ptr->array, SHR_E_MEMORY);

    /*
     * Check if actual size of allocated HA block is smaller than
     * requested size.
     */
    if (ha_alloc_sz < in_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!warm) {
        sal_memset(hw_array_ptr, 0, ha_alloc_sz);
        hw_array_ptr->signature = ref_sign;
        hw_array_ptr->array_size = array_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_ECMP_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMECMP_HW_ENTRY_HA_BLK_T_ID));
    } else {
        SHR_IF_ERR_MSG_EXIT
            ((hw_array_ptr->signature != ref_sign
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tWB hw enty: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               ref_sign,
               hw_array_ptr->signature));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t hw entry: Signature=0x%-8x HA addr=0x%p\n"),
                 hw_array_ptr->signature,
                 (void*)hw_array_ptr));
    *alloc_ptr = hw_array_ptr;
    *out_ha_alloc_sz = ha_alloc_sz;
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates HA memory for storing group attr.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of group attr array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmecmp_grp_attr_ha_alloc(int unit,
                          bool warm,
                          uint32_t array_size,
                          uint8_t submod_id,
                          uint32_t ref_sign,
                          bcmecmp_grp_ha_blk_t **alloc_ptr,
                          uint32_t *out_ha_alloc_sz)
{
    bcmecmp_grp_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size = 0;
    SHR_FUNC_ENTER(unit);
    /* compiler treat bcmecmp_grp_ha_blk_t.array as empty array(size=0) */
    in_req_size = sizeof(bcmecmp_grp_attr_t) * array_size +
                  sizeof(bcmecmp_grp_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_ECMP_COMP_ID,
                                    submod_id,
                                    "ecmpGrpAttr",
                                    &ha_alloc_sz);
    SHR_NULL_CHECK(hw_array_ptr, SHR_E_MEMORY);
    SHR_NULL_CHECK(hw_array_ptr->array, SHR_E_MEMORY);

    /*
     * Check if actual size of allocated HA block is smaller than
     * requested size.
     */
    if (ha_alloc_sz < in_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!warm) {
        sal_memset(hw_array_ptr, 0, ha_alloc_sz);
        hw_array_ptr->signature = ref_sign;
        hw_array_ptr->array_size = array_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_ECMP_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMECMP_GRP_HA_BLK_T_ID));
    } else {
        SHR_IF_ERR_MSG_EXIT
            ((hw_array_ptr->signature != ref_sign
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tWB grp attr: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               ref_sign,
               hw_array_ptr->signature));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t grp attr: Signature=0x%-8x HA addr=0x%p\n"),
                 hw_array_ptr->signature,
                 (void*)hw_array_ptr));
    *alloc_ptr = hw_array_ptr;
    *out_ha_alloc_sz = ha_alloc_sz;
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates HA memory for storing
 *  flex HW entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [in/out] alloc_ptr ptr of ptr of hw entry array info.
 * \param [in/out] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmecmp_flex_hw_entry_ha_alloc(int unit,
                          bool warm,
                          uint32_t array_size, uint8_t submod_id,
                          uint32_t ref_sign,
                          bcmecmp_flex_hw_entry_ha_blk_t **alloc_ptr,
                          uint32_t *out_ha_alloc_sz)
{
    bcmecmp_flex_hw_entry_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size;

    SHR_FUNC_ENTER(unit);
    /* compiler treat bcmecmp_hw_entry_ha_blk_t.array as empty array(size=0) */
    in_req_size = sizeof(bcmecmp_flex_hw_entry_attr_t) * array_size +
                  sizeof(bcmecmp_flex_hw_entry_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_ECMP_COMP_ID,
                                    submod_id,
                                    "ecmpFlexHWEntry",
                                    &ha_alloc_sz);
    SHR_NULL_CHECK(hw_array_ptr, SHR_E_MEMORY);
    SHR_NULL_CHECK(hw_array_ptr->array, SHR_E_MEMORY);

    /*
     * Check if actual size of allocated HA block is smaller than
     * requested size.
     */
    if (ha_alloc_sz < in_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!warm) {
        sal_memset(hw_array_ptr, 0, ha_alloc_sz);
        hw_array_ptr->signature = ref_sign;
        hw_array_ptr->array_size = array_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_ECMP_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMECMP_FLEX_HW_ENTRY_HA_BLK_T_ID));
    } else {
        SHR_IF_ERR_MSG_EXIT
            ((hw_array_ptr->signature != ref_sign
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tWB hw enty: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               ref_sign,
               hw_array_ptr->signature));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t hw entry: Signature=0x%-8x HA addr=0x%p\n"),
                 hw_array_ptr->signature,
                 (void*)hw_array_ptr));
    *alloc_ptr = hw_array_ptr;
    *out_ha_alloc_sz = ha_alloc_sz;
    exit:
        SHR_FUNC_EXIT();
}


/*!
 * \brief This function allocates HA memory for storing flex group attr.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [in/out] alloc_ptr ptr of ptr of group attr array info.
 * \param [in/out] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmecmp_flex_grp_rh_attr_ha_alloc(int unit,
                                  bool warm,
                                  uint32_t array_size, uint8_t submod_id,
                                  uint32_t ref_sign,
                                  bcmecmp_flex_grp_rh_ha_blk_t **alloc_ptr,
                                  uint32_t *out_ha_alloc_sz)
{
    bcmecmp_flex_grp_rh_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size = 0;
    SHR_FUNC_ENTER(unit);

    /* compiler treat bcmecmp_rh_grp_ha_blk_t.array as empty array(size=0) */
    in_req_size = sizeof(bcmecmp_flex_grp_rh_attr_t) * array_size +
                  sizeof(bcmecmp_flex_grp_rh_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_ECMP_COMP_ID,
                                    submod_id,
                                    "ecmpFlexGrpRhAttr",
                                    &ha_alloc_sz);
    SHR_NULL_CHECK(hw_array_ptr, SHR_E_MEMORY);
    SHR_NULL_CHECK(hw_array_ptr->array, SHR_E_MEMORY);

    /*
     * Check if actual size of allocated HA block is smaller than
     * requested size.
     */
    if (ha_alloc_sz < in_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!warm) {
        sal_memset(hw_array_ptr, 0, ha_alloc_sz);
        hw_array_ptr->signature = ref_sign;
        hw_array_ptr->array_size = array_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_ECMP_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMECMP_FLEX_GRP_RH_HA_BLK_T_ID));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t RH grp attr: Signature=0x%-8x HA addr=0x%p\n"),
                 hw_array_ptr->signature,
                 (void*)hw_array_ptr));
    *alloc_ptr = hw_array_ptr;
    *out_ha_alloc_sz = ha_alloc_sz;
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates HA memory for storing flex group attr.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [in/out] alloc_ptr ptr of ptr of group attr array info.
 * \param [in/out] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmecmp_flex_grp_attr_ha_alloc(int unit,
                               bool warm,
                               uint32_t array_size, uint8_t submod_id,
                              uint32_t ref_sign,
                              bcmecmp_flex_grp_ha_blk_t **alloc_ptr,
                              uint32_t *out_ha_alloc_sz)
{
    bcmecmp_flex_grp_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size = 0;
    SHR_FUNC_ENTER(unit);
    /* compiler treat bcmecmp_grp_ha_blk_t.array as empty array(size=0) */
    in_req_size = sizeof(bcmecmp_flex_grp_attr_t) * array_size +
                  sizeof(bcmecmp_flex_grp_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_ECMP_COMP_ID,
                                    submod_id,
                                    "ecmpFlexGrpAttr",
                                    &ha_alloc_sz);
    SHR_NULL_CHECK(hw_array_ptr, SHR_E_MEMORY);
    SHR_NULL_CHECK(hw_array_ptr->array, SHR_E_MEMORY);

    /*
     * Check if actual size of allocated HA block is smaller than
     * requested size.
     */
    if (ha_alloc_sz < in_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!warm) {
        sal_memset(hw_array_ptr, 0, ha_alloc_sz);
        hw_array_ptr->signature = ref_sign;
        hw_array_ptr->array_size = array_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_ECMP_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMECMP_FLEX_GRP_HA_BLK_T_ID));
    } else {
        SHR_IF_ERR_MSG_EXIT
            ((hw_array_ptr->signature != ref_sign
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tWB grp attr: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               ref_sign,
               hw_array_ptr->signature));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t grp attr: Signature=0x%-8x HA addr=0x%p\n"),
                 hw_array_ptr->signature,
                 (void*)hw_array_ptr));
    *alloc_ptr = hw_array_ptr;
    *out_ha_alloc_sz = ha_alloc_sz;
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates HA memory for storing group attr.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of group attr array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmecmp_flex_member_dest_grp_attr_ha_alloc(int unit,
                                           bool warm,
                                           uint32_t array_size,
                                           uint8_t submod_id,
                                           uint32_t ref_sign,
                                           bcmecmp_member_dest_grp_ha_blk_t **alloc_ptr,
                                           uint32_t *out_ha_alloc_sz)
{
    bcmecmp_member_dest_grp_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size = 0;
    SHR_FUNC_ENTER(unit);
    /*
     * compiler treat bcmecmp_member_dest_grp_ha_blk_t.array as empty
     * array(size=0)
     */
    in_req_size = sizeof(bcmecmp_member_dest_grp_attr_t) * array_size +
                  sizeof(bcmecmp_member_dest_grp_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_ECMP_MEMBER_DEST_COMP_ID,
                                    submod_id,
                                    "ecmpMbrDestGrpAttr",
                                    &ha_alloc_sz);
    SHR_NULL_CHECK(hw_array_ptr, SHR_E_MEMORY);
    SHR_NULL_CHECK(hw_array_ptr->array, SHR_E_MEMORY);

    /*
     * Check if actual size of allocated HA block is smaller than
     * requested size.
     */
    if (ha_alloc_sz < in_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!warm) {
        sal_memset(hw_array_ptr, 0, ha_alloc_sz);
        hw_array_ptr->signature = ref_sign;
        hw_array_ptr->array_size = array_size;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_ECMP_MEMBER_DEST_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMECMP_MEMBER_DEST_GRP_HA_BLK_T_ID));
    } else {
        SHR_IF_ERR_MSG_EXIT
            ((hw_array_ptr->signature != ref_sign
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tWB grp attr: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               ref_sign,
               hw_array_ptr->signature));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t grp attr: Signature=0x%-8x HA addr=0x%p\n"),
                 hw_array_ptr->signature,
                 (void*)hw_array_ptr));
    *alloc_ptr = hw_array_ptr;
    *out_ha_alloc_sz = ha_alloc_sz;
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Insert a group into the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [in] grp_id The group which will be inserted into the group list.
 * \param [in] min2max Sort groups from min to max into the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 * \retval BCMECMP_E_MEMORY Allocate resource failed.
 */
int
bcmecmp_grp_list_insert(int unit,
                        bcmecmp_grp_list_t *grp_list,
                        bcmecmp_id_t grp_id,
                        bool min2max)
{
    bcmecmp_grp_node_t *new_node = NULL;
    bcmecmp_grp_node_t *cur_node = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(grp_list, SHR_E_PARAM);

    BCMECMP_ALLOC(new_node, sizeof(bcmecmp_grp_node_t), "bcmecmpGrpList");
    new_node->grp_id = grp_id;
    new_node->next = NULL;

    if (min2max) {
        if (grp_list->head == NULL ||
            BCMECMP_GRP_MEMB_TBL_START_IDX(unit, new_node->grp_id) <=
            BCMECMP_GRP_MEMB_TBL_START_IDX(unit, grp_list->head->grp_id)) {
            new_node->next = grp_list->head;
            grp_list->head = new_node;
        } else {
            cur_node = grp_list->head;

            while (cur_node->next != NULL &&
                   BCMECMP_GRP_MEMB_TBL_START_IDX(unit, new_node->grp_id) >
                   BCMECMP_GRP_MEMB_TBL_START_IDX(unit, cur_node->grp_id)) {
                cur_node = cur_node->next;
            }

            new_node->next = cur_node->next;
            cur_node->next = new_node;
        }
    } else {
        if (grp_list->head == NULL ||
            BCMECMP_GRP_MEMB_TBL_START_IDX(unit, new_node->grp_id) >=
            BCMECMP_GRP_MEMB_TBL_START_IDX(unit, grp_list->head->grp_id)) {
            new_node->next = grp_list->head;
            grp_list->head = new_node;
        } else {
            cur_node = grp_list->head;

            while (cur_node->next != NULL &&
                   BCMECMP_GRP_MEMB_TBL_START_IDX(unit, new_node->grp_id) <
                   BCMECMP_GRP_MEMB_TBL_START_IDX(unit, cur_node->grp_id)) {
                cur_node = cur_node->next;
            }

            new_node->next = cur_node->next;
            cur_node->next = new_node;
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a group from the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [in] grp_id The group which will be deleted from the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 */
int
bcmecmp_grp_list_delete(int unit,
                        bcmecmp_grp_list_t *grp_list,
                        bcmecmp_id_t grp_id)
{
    bcmecmp_grp_node_t *pre_node = NULL;
    bcmecmp_grp_node_t *cur_node = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(grp_list, SHR_E_PARAM);

    if (grp_list->head == NULL) {
        SHR_EXIT();
    }

    if (grp_list->head->grp_id == grp_id) {
        cur_node = grp_list->head;
        grp_list->head = grp_list->head->next;
    } else {
        pre_node = grp_list->head;
        cur_node = pre_node->next;
        while (cur_node != NULL) {
            if (cur_node->grp_id == grp_id) {
                break;
            }

            pre_node = cur_node;
            cur_node = cur_node->next;
        }

        if (cur_node != NULL) {
            pre_node->next = cur_node->next;
        }
    }

    BCMECMP_FREE(cur_node);

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Get the head of the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [out] grp_id The group at the head of the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 * \retval BCMECMP_E_FAIL The group is empty.
 */
int
bcmecmp_grp_list_get_head(int unit,
                          bcmecmp_grp_list_t *grp_list,
                          bcmecmp_id_t *grp_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(grp_list, SHR_E_PARAM);
    SHR_NULL_CHECK(grp_id, SHR_E_PARAM);

    if (grp_list->head == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    *grp_id = grp_list->head->grp_id;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 */
int
bcmecmp_grp_list_destroy(int unit, bcmecmp_grp_list_t *grp_list)
{
    bcmecmp_grp_node_t *cur_node = NULL;
    bcmecmp_grp_node_t *tmp_node = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(grp_list, SHR_E_PARAM);

    cur_node = grp_list->head;
    while (cur_node != NULL) {
        tmp_node = cur_node;
        cur_node = cur_node->next;

        BCMECMP_FREE(tmp_node);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Update a group in the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [in] grp_id The group which will be inserted into the group list.
 * \param [in] min2max Sort groups from min to max into the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 * \retval BCMECMP_E_MEMORY Allocate resource failed.
 */
int
bcmecmp_grp_list_update(int unit,
                        bcmecmp_grp_list_t *grp_list,
                        bcmecmp_id_t grp_id,
                        bool min2max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_grp_list_delete(unit, grp_list, grp_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_grp_list_insert(unit, grp_list, grp_id, min2max));

    exit:
        SHR_FUNC_EXIT();
}

uint32_t
bcmecmp_itbm_mstart_index_get(int unit,
                              bcmecmp_grp_type_t gtype,
                              uint32_t weight_mode,
                              uint32_t bucket,
                              uint32_t first)
{
    uint32_t memb_per_ent, tbl_sz, imin, imax, free_idx;
    uint32_t offset = 0;
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_info = NULL;
    fn_shr_grp_type_get_t shr_grp_func = NULL;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "gtype %d weight_mode %d bucket %d first %d.\n"),
                 gtype, weight_mode, bucket, first));

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_info = &(ecmp_ctrl->ecmp_info);
    if (bcmecmp_grp_is_member_replication(gtype, weight_mode)) {
        if ((shr_grp_func = ecmp_ctrl->ecmp_drv->shr_grp_type_get)) {
        /* MEMBER_REPLICATION member is configured in member share view. */
             gtype = shr_grp_func(gtype);
        }
    }
    tbl_sz = ecmp_info->member[gtype].tbl_size;
    imax = ecmp_info->member[gtype].imax;
    imin = ecmp_info->member[gtype].imin;
    memb_per_ent = tbl_sz / (imax - imin + 1);
    while (memb_per_ent >>= 1) {
        offset++;
    }
    free_idx = (bucket << offset) | first;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "mstart_idx %d.\n"),
                 free_idx));

    return free_idx;
}

int
bcmecmp_itbm_bucket_first_get(int unit,
                              bcmecmp_grp_type_t gtype,
                              uint32_t weight_mode,
                              uint32_t mstart_idx,
                              uint32_t *bucket,
                              uint32_t *first)
{
    uint32_t memb_per_ent, tbl_sz, imin, imax;
    uint32_t offset = 0;
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_info = NULL;
    fn_shr_grp_type_get_t shr_grp_func = NULL;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "gtype %d weight_mode %d mstart_idx %d.\n"),
                 gtype, weight_mode, mstart_idx));
    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_info = &(ecmp_ctrl->ecmp_info);
    if (bcmecmp_grp_is_member_replication(gtype, weight_mode)) {
        if ((shr_grp_func = ecmp_ctrl->ecmp_drv->shr_grp_type_get)) {
        /* MEMBER_REPLICATION member is configured in member share view. */
             gtype = shr_grp_func(gtype);
        }
    }
    tbl_sz = ecmp_info->member[gtype].tbl_size;
    imax = ecmp_info->member[gtype].imax;
    imin = ecmp_info->member[gtype].imin;
    memb_per_ent = tbl_sz / (imax - imin + 1);
    while (memb_per_ent >>= 1) {
        offset++;
    }
    *first = mstart_idx & ((1 << offset) -1);
    *bucket = mstart_idx >> offset;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bucket %d, first %d.\n"),
                 *bucket, *first));

    return SHR_E_NONE;
}

int
bcmecmp_pt_write(int unit,
                 bcmecmp_pt_op_info_t *op_info,
                 uint32_t *buf)
{
    uint32_t *rsp_ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_write[drd_sid=%d idx=%d]: trans_id=0x%x "
                          "req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_pt_read(int unit,
                bcmecmp_pt_op_info_t *op_info,
                uint32_t *buf)
{
    uint32_t *ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */
    uint32_t flags_0, flags_1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);
    flags_0 = (uint32_t) req_flags;
    flags_1 = (uint32_t) (req_flags >> 32);
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-in[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x "
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));

    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }

    /*
     * PTM Values:
     *  - BCMPTM_LTID_RSP_NOT_SUPPORTED 0xFFFFFFFD
     *  - BCMPTM_LTID_RSP_HW 0xFFFFFFFF
     */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-out[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x "
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_pt_ireq_read(int unit,
                     bcmltd_sid_t lt_id,
                     bcmdrd_sid_t pt_id,
                     int index,
                     void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint64_t            req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index    = index;
    pt_info.tbl_inst = BCMECMP_ALL_PIPES_INST;
    rsp_entry_wsize  = bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit,
                              req_flags,
                              pt_id,
                              &pt_info,
                              NULL,
                              rsp_entry_wsize,
                              lt_id,
                              entry_data,
                              &rsp_ltid,
                              &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_rh_groups_preconfig_with_itbm(int unit)
{
    int ecmp_id, gstart_idx, gend_idx;
    bcmecmp_info_t *ecmp_info;
    bcmecmp_grp_attr_t *grp_attr, *grp_attr_base;
    int gtype;
    const char *const lb_name[BCMECMP_LB_MODE_COUNT] = BCMECMP_LB_MODE_NAME;

    SHR_FUNC_ENTER(unit);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    grp_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr);
    gstart_idx = 0;
    gend_idx = ecmp_info->grp_arr_sz;
    for (ecmp_id = gstart_idx; ecmp_id < gend_idx; ecmp_id++) {
        grp_attr = grp_attr_base + ecmp_id;
        /* check if the HA memory has the group configured. */
        if (!grp_attr->in_use_itbm) {
            continue;
        }

        /*
         * For RH-ECMP groups, load balanced members state must be recovered
         * prior to members update operation. Check and set the pre_config
         * flag for this type of group.
         */
        if (grp_attr->lb_mode == BCMECMP_LB_MODE_RESILIENT
            && grp_attr->num_paths != 0) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "ECMP_ID[LB_MODE=%s,NUM_PATHS=%u]=%u:"
                                 " in-use.\n"),
                      lb_name[grp_attr->lb_mode],
                      grp_attr->num_paths,
                      ecmp_id));
            /* Set pre-config flag for RH-ECMP group. */
            gtype = grp_attr->grp_type;
            BCMECMP_ENT_PRECONFIG_SET
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id);
        }
    }

    SHR_FUNC_EXIT();
}
