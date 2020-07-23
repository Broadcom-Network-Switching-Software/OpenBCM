/*! \file bcmcth_trunk_vp_lt_utils.c
 *
 * This file contains TRUNK_VP custom table handler utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_trunk_vp_lt_utils.h>
#include <bcmcth/bcmcth_trunk_vp_types.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK
/*******************************************************************************
 * Private functions
 */
/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Compares two integer values and returns the status.
 *
 * \param [in] a First integer value to compare.
 * \param [in] b Second integer value to compare with.
 *
 * \returns BCMCTH_TRUNK_VP_CMP_GREATER When first value is greater than second.
 * \returns BCMCTH_TRUNK_VP_CMP_EQUAL When both values are equal.
 * \returns BCMCTH_TRUNK_VP_CMP_LESS When first value is lesser than second.
 */
int
bcmcth_trunk_vp_cmp_int(const void *a, const void *b)
{
    int first;  /* Fist integer value to compare. */
    int second; /* Second integer value to compare. */

    first = *(int *)a;
    second = *(int *)b;

    if (first < second) {
        return (BCMCTH_TRUNK_VP_CMP_LESS);
    } else if (first > second) {
        return (BCMCTH_TRUNK_VP_CMP_GREATER);
    }
    return (BCMCTH_TRUNK_VP_CMP_EQUAL);
}

/*!
 * \brief Compares member table start indices of the defragmentation blocks.
 *
 * \param [in] a First defragmentation block to compare.
 * \param [in] b Second defragmentation block to compare with.
 *
 * \returns BCMCTH_TRUNK_VP_CMP_GREATER When first value is greater than second.
 * \returns BCMCTH_TRUNK_VP_CMP_EQUAL When both values are equal.
 * \returns BCMCTH_TRUNK_VP_CMP_LESS When first value is lesser than second.
 */
int
bcmcth_trunk_vp_defrag_grps_cmp(const void *a, const void *b)
{
    bcmcth_trunk_vp_grp_defrag_t *first;  /* Fist defragmentation block. */
    bcmcth_trunk_vp_grp_defrag_t *second; /* Second defragmentation block. */

    first = (bcmcth_trunk_vp_grp_defrag_t *)a;
    second = (bcmcth_trunk_vp_grp_defrag_t *)b;

    if (first->mstart_idx < second->mstart_idx) {
        return (BCMCTH_TRUNK_VP_CMP_LESS);
    } else if (first->mstart_idx > second->mstart_idx) {
        return (BCMCTH_TRUNK_VP_CMP_GREATER);
    }
    return (BCMCTH_TRUNK_VP_CMP_EQUAL);
}
/*!
 * \brief This function allocates HA memory for storing HW entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [in] ha_blk_id ha block ID string.
 * \param [in/out] alloc_ptr ptr of ptr of hw entry array info.
 * \param [in/out] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmcth_trunk_vp_hw_entry_ha_alloc(int unit,
                          bool warm,
                          uint32_t array_size, uint8_t submod_id,
                          uint32_t ref_sign,
                          const char *ha_blk_id,
                          bcmcth_trunk_vp_hw_entry_ha_blk_t **alloc_ptr,
                          uint32_t *out_ha_alloc_sz)
{
    bcmcth_trunk_vp_hw_entry_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size;

    SHR_FUNC_ENTER(unit);
    /*
     * Compiler treat bcmcth_trunk_vp_hw_entry_ha_blk_t.array
     * as empty array(size=0).
     */
    in_req_size = sizeof(bcmcth_trunk_vp_hw_entry_attr_t) * array_size +
                  sizeof(bcmcth_trunk_vp_hw_entry_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_TRUNK_COMP_ID,
                                    submod_id,
                                    ha_blk_id,
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
            (bcmissu_struct_info_report(unit, BCMMGMT_TRUNK_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMCTH_TRUNK_VP_HW_ENTRY_HA_BLK_T_ID));
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
 * \param [in] ha_blk_id ha block ID string.
 * \param [in/out] alloc_ptr ptr of ptr of group attr array info.
 * \param [in/out] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
int
bcmcth_trunk_vp_group_attr_ha_alloc(int unit,
                            bool warm,
                            uint32_t array_size, uint8_t submod_id,
                            uint32_t ref_sign,
                            const char *ha_blk_id,
                            bcmcth_trunk_vp_grp_ha_blk_t **alloc_ptr,
                            uint32_t *out_ha_alloc_sz)
{
    bcmcth_trunk_vp_grp_ha_blk_t *hw_array_ptr = NULL;
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t in_req_size = 0;
    SHR_FUNC_ENTER(unit);
    /*
     * Compiler treat bcmcth_trunk_vp_grp_ha_blk_t.array
     * as empty array(size=0).
     */
    in_req_size = sizeof(bcmcth_trunk_vp_grp_attr_t) * array_size +
                  sizeof(bcmcth_trunk_vp_grp_ha_blk_t);
    ha_alloc_sz = in_req_size;
    hw_array_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_TRUNK_COMP_ID,
                                    submod_id,
                                    ha_blk_id,
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
            (bcmissu_struct_info_report(unit, BCMMGMT_TRUNK_COMP_ID,
                                        submod_id, 0,
                                        in_req_size, 1,
                                        BCMCTH_TRUNK_VP_GRP_HA_BLK_T_ID));
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
 * \brief LT entry input parameters validate function.
 *
 * Validate LT entry input parameter values based on the LT operation type.
 *
 * \param [in]   unit Unit number.
 * \param [in]   opcode LT entry event.
 * \param [in]   trunk_vp_id ECMP identifier.
 * \param [in]   gtype ECMP group type.
 * \param [out]  in_use Group in use.
 * \param [out]  comp_id component using the group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmcth_trunk_vp_grp_in_use(int unit,
                           uint32_t trunk_vp_id,
                           bcmcth_trunk_vp_grp_type_t gtype,
                           bool *in_use,
                           uint32_t *comp_id)
{
    uint32_t tmp_comp_id;

    SHR_FUNC_ENTER(unit);
    *in_use = FALSE;
    if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                      group, gtype))) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_elem_state_get
                (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                 SHR_ITBM_INVALID,
                 trunk_vp_id,
                 in_use,
                 &tmp_comp_id));
        if (comp_id) {
            *comp_id = tmp_comp_id;
        }
    }
    exit:
        SHR_FUNC_EXIT();
}
