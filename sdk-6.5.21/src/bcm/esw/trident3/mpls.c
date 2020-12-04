/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls.c
 * Purpose: Handle trident3 specific MPLS APIs
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <bcm/error.h>

#ifdef INCLUDE_L3
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_SPECIAL_LABEL_SUPPORT

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <include/bcm_int/esw/mpls.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trident3.h>

/*
 * External Linkages.
 */
#define MPLS_INFO(_unit_)   (&_bcm_tr_mpls_bk_info[_unit_])

/*
 * Global Defines.
 */
bcmi_mpls_special_label_control_t *bcmi_special_label_precedence_state[BCM_MAX_NUM_UNITS];

/*
 * Structure to store REF counts for egress special labels.
 */
bcmi_mpls_special_label_egress_t bcmi_special_label_egress_state[BCM_MAX_NUM_UNITS] = {{ 0 }};

/************************************
 ********  Enf Of Headers ***********
 */
/*
 * Function:
 *      bcmi_mpls_special_label_identifier_sw_state_init
 * Purpose:
 *      Initialize software state for special label routines.
 *
 * Parameters:
 *      unit       - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_mpls_special_label_identifier_sw_state_init(int unit)
{

    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint32 *spl_label_buf = NULL;
    int buf_size = 0;
    bcmi_mpls_special_label_control_t *ptr;
    int rv = BCM_E_NONE;
    int i = 0;
    special_label_control_entry_t *label_entry;

    mem = SPECIAL_LABEL_CONTROLm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem) * ((index_max - index_min ) + 1);
    ptr = bcmi_special_label_precedence_state[unit];

    spl_label_buf = soc_cm_salloc(unit, buf_size,
                      "SPECIAL_LABEL_CONTROL buffer");
    if (NULL == spl_label_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, spl_label_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, spl_label_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {

        label_entry = soc_mem_table_idx_to_pointer(unit,
            mem, special_label_control_entry_t *, spl_label_buf, i);

        if (soc_mem_field32_get(unit, mem, label_entry, VALIDf) != 0x1) {
            continue;
        }

        ptr[i].valid = 1;
        ptr[i].label_value =
            soc_mem_field32_get(unit, mem, label_entry, LABEL_VALUEf);
        ptr[i].label_mask =
            soc_mem_field32_get(unit, mem, label_entry, LABEL_VALUE_MASKf);
        ptr[i].label_type =
            soc_mem_field32_get(unit, mem, label_entry, CURRENT_LABEL_TYPEf);

        if (!SOC_WARM_BOOT(unit)) {
            ptr[i].ref_count++;
            ptr[i].cancun_added = 1;
        }
    }

    bcmi_special_label_egress_state[unit].special_label_ref_count = 0;
    bcmi_special_label_egress_state[unit].entropy_label_ref_count = 0;
    soc_cm_sfree(unit, spl_label_buf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mpls_special_label_deinit
 * Purpose:
 *      Cleans software state for special label routines.
 *
 * Parameters:
 *      unit       - (IN) Device Number
 * Returns:
 *      None
 */
void
bcmi_mpls_special_label_deinit(int unit)
{
    if (bcmi_special_label_precedence_state[unit]) {
        soc_cm_sfree(unit, bcmi_special_label_precedence_state[unit]);
        bcmi_special_label_precedence_state[unit] = NULL;
    }

}

/*
 * Function:
 *      bcmi_mpls_special_label_init
 * Purpose:
 *      Initializes software state for special label routines.
 *
 * Parameters:
 *      unit       - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_mpls_special_label_init(int unit)
{
    int num_entry;

    num_entry = soc_mem_index_count(unit, SPECIAL_LABEL_CONTROLm);

    /* First clean if some old state is still not freed */
    bcmi_mpls_special_label_deinit(unit);

    bcmi_special_label_precedence_state[unit] = soc_cm_salloc(unit, 
            (num_entry * sizeof(bcmi_mpls_special_label_control_t)),
            "software state to set precedence of special labels");

    if (bcmi_special_label_precedence_state[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(bcmi_special_label_precedence_state[unit], 0,
        (num_entry * sizeof(bcmi_mpls_special_label_control_t)));
    
    /* 
     * Sync the state with h/w.
     * Currently, CanCun is writing some default entries at start.
     * it will be difficult to maintain refcount if user also adds 
     * similar entries and then deletes them.
     * therefore SDK will read those entries first and then keep
     * track of them so there is no affect if user deletes their entry.
     * this will also be helpful during warmboot recovery.
     */
    bcmi_mpls_special_label_identifier_sw_state_init(unit);

    return BCM_E_NONE;
}

int
bcmi_mpls_special_label_egress_match(int unit,
            bcm_mpls_special_label_type_t label_type,
            void * read_value,
            void * new_value)
{
    uint64 val64;
    uint64 val64_r;
    uint32 val = 0;
    uint32 val_r = 0;

    if (label_type == bcmMplsSpecialLabelTypeSpecial) {
        COMPILER_64_ZERO(val64);
        COMPILER_64_ZERO(val64_r);

        val64 = *((uint64 *)(new_value));
        val64_r = *((uint64 *)(read_value));

        if (COMPILER_64_EQ(val64_r, val64)) {
            return TRUE;
        }

    } else {
        val  =  *((uint32 *)(new_value));
        val_r  =  *((uint32 *)(read_value));

        if (val == val_r) {
            return TRUE;
        }
    }

    return FALSE;
}



/*
 * Function:
 *      bcmi_mpls_special_label_egress_add 
 * Purpose:
 *      Add an MPLS entropy/special label in
 *      MPLS stack for egress tunnel encap.
 * Parameters:
 *      unit       - (IN) Device Number
 *      label_type - (IN) type of label
 *      label_info - (IN) Info about special label.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_mpls_special_label_egress_add(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    uint64 val64_r;
    uint32 val = 0;
    uint32 val_r = 0;
    bcmi_mpls_special_label_egress_t *ptr;

    ptr = (&(bcmi_special_label_egress_state[unit]));


    if (!BCMI_MPLS_LABEL_ID_VALID(label_info.label_value)) {
        return BCM_E_PARAM;
    }

    if (!BCMI_MPLS_EXP_VALID(label_info.exp)) {
        return BCM_E_PARAM;
    }

    if (!BCMI_MPLS_TTL_VALID(label_info.ttl)) {
        return BCM_E_PARAM;
    }


    /* Only Special and entropy labels can be added here*/
    if ((label_type != bcmMplsSpecialLabelTypeSpecial) &&
        (label_type != bcmMplsSpecialLabelTypeEntropy)) {

        return BCM_E_PARAM;
    }

    if (label_type == bcmMplsSpecialLabelTypeSpecial) {

        COMPILER_64_ZERO(val64);
        COMPILER_64_ZERO(val64_r);
        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_SPECIAL_LABEL_CONTROLr(unit, &(val64_r)));

        soc_reg64_field32_set(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, &val64,
            EXPf, label_info.exp);
        soc_reg64_field32_set(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, &val64,
            TTLf, label_info.ttl);

         soc_reg64_field32_set(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, &val64,
            LABEL_VALUEf, label_info.label_value);

        if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP) {
            soc_reg64_field32_set(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, &val64,
                EXP_SOURCEf, 1);
        }
        if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL) {
            soc_reg64_field32_set(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, &val64,
                TTL_SOURCEf, 1);
        }

        if (bcmi_mpls_special_label_egress_match
            (unit, label_type, (void *)(&val64_r), (void *)(&val64))) {
            /* Similar entry exists. Add an refcount */
            ptr->special_label_ref_count++;
            return BCM_E_NONE;
        } else if (ptr->special_label_ref_count) {
            return BCM_E_RESOURCE;
        }

        rv = WRITE_EGR_MPLS_SPECIAL_LABEL_CONTROLr(unit, val64);   
        if (BCM_SUCCESS(rv)) {
            ptr->special_label_ref_count++;
        }

    } else if (label_type == bcmMplsSpecialLabelTypeEntropy) {

        /* Only reserved labels are allowed here */
        if (label_info.label_value > 0xf) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_ENTROPY_LABEL_CONTROLr(unit, &val_r));

        soc_reg_field_set(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, &val,
            EXPf, label_info.exp);
        soc_reg_field_set(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, &val,
            TTLf, label_info.ttl);

         soc_reg_field_set(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, &val,
            LABEL_OFFSETf, label_info.label_value);

        if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP) {
            soc_reg_field_set(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, &val,
                EXP_SOURCEf, 1);
        }
        if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL) {
            soc_reg_field_set(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, &val,
                TTL_SOURCEf, 1);
        }
        if (bcmi_mpls_special_label_egress_match
            (unit, label_type, (void *) (&(val_r)), (void *)(&val))) {
            /* Similar entry exists. Add an refcount */
            ptr->entropy_label_ref_count++;
            return BCM_E_NONE;
        } else if (ptr->entropy_label_ref_count) {
            return BCM_E_RESOURCE;
        }

        rv = WRITE_EGR_MPLS_ENTROPY_LABEL_CONTROLr(unit, val);
        if (BCM_SUCCESS(rv)) {
            ptr->entropy_label_ref_count++;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmi_mpls_special_label_egress_get
 * Purpose:
 *      Get an MPLS entropy/special label in
 *      MPLS stack for egress tunnel encap.
 * Parameters:
 *      unit       - (IN) Device Number
 *      label_type - (IN) type of label
 *      label_info - (OUT) Info about special label.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_mpls_special_label_egress_get(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t *label_info)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    uint32 val = 0;
    bcmi_mpls_special_label_egress_t *ptr;

    COMPILER_64_ZERO(val64);

    ptr = (&(bcmi_special_label_egress_state[unit]));

    if (label_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Only Special and entropy labels can be added here*/
    if ((label_type != bcmMplsSpecialLabelTypeSpecial) &&
        (label_type != bcmMplsSpecialLabelTypeEntropy)) {

        return BCM_E_PARAM;
    }

    if (label_type == bcmMplsSpecialLabelTypeSpecial) {
         if (!(ptr->special_label_ref_count)) {
            return BCM_E_NOT_FOUND;
        }

        COMPILER_64_ZERO(val64);
        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_SPECIAL_LABEL_CONTROLr(unit, &val64));

        label_info->exp = soc_reg64_field32_get(unit,
            EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64, EXPf);

        label_info->ttl = soc_reg64_field32_get(unit,
            EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64, TTLf);

        label_info->label_value = soc_reg64_field32_get(unit,
            EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64, LABEL_VALUEf);

        if (soc_reg64_field32_get(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64,
                EXP_SOURCEf)) {

            label_info->flags |= BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP;
        }

        if (soc_reg64_field32_get(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64,
                TTL_SOURCEf)) {

            label_info->flags |= BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL;
        }

    } else if (label_type == bcmMplsSpecialLabelTypeEntropy) {
         if (!(ptr->entropy_label_ref_count)) {
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_ENTROPY_LABEL_CONTROLr(unit, &val));

        label_info->exp = soc_reg_field_get(unit,
            EGR_MPLS_ENTROPY_LABEL_CONTROLr, val, EXPf);

        label_info->ttl = soc_reg_field_get(unit,
            EGR_MPLS_ENTROPY_LABEL_CONTROLr, val, TTLf);

        label_info->label_value = soc_reg_field_get(unit,
            EGR_MPLS_ENTROPY_LABEL_CONTROLr, val, LABEL_OFFSETf);

        if (soc_reg_field_get(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, val,
                EXP_SOURCEf)) {

            label_info->flags |= BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP;
        }

        if (soc_reg_field_get(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, val,
                TTL_SOURCEf)) {

            label_info->flags |= BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmi_mpls_special_label_egress_delete
 * Purpose:
 *      Delete an MPLS entropy/special label in
 *      MPLS stack for egress tunnel encap.
 * Parameters:
 *      unit       - (IN) Device Number
 *      label_type - (IN) type of label
 *      label_info - (IN) Info about special label.
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_mpls_special_label_egress_delete(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    uint32 val = 0;
    bcmi_mpls_special_label_egress_t *ptr;

    COMPILER_64_ZERO(val64);

    ptr = (&(bcmi_special_label_egress_state[unit]));

    /* Only Special and entropy labels can be added here*/
    if ((label_type != bcmMplsSpecialLabelTypeSpecial) &&
        (label_type != bcmMplsSpecialLabelTypeEntropy)) {

        return BCM_E_PARAM;
    }

    if (label_type == bcmMplsSpecialLabelTypeSpecial) {

        /* If there is no entry to delete then reutrn not found. */
        if (!(ptr->special_label_ref_count)) {
            return BCM_E_NOT_FOUND;
        }


        COMPILER_64_ZERO(val64);
        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_SPECIAL_LABEL_CONTROLr(unit, &val64));

        if (label_info.exp != soc_reg64_field32_get(unit,
            EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64, EXPf)) {
            return BCM_E_NOT_FOUND;
        }

        if (label_info.ttl != soc_reg64_field32_get(unit,
            EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64, TTLf)) {
            return BCM_E_NOT_FOUND;
        }

        if (label_info.label_value != soc_reg64_field32_get(unit,
            EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64, LABEL_VALUEf)) {
            return BCM_E_NOT_FOUND;
        }

        if (soc_reg64_field32_get(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64,
                EXP_SOURCEf)) {

            if (!(label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP)) {
                return BCM_E_NOT_FOUND;
            }
        } else if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP) {
            return BCM_E_NOT_FOUND;
        }


        if (soc_reg64_field32_get(unit, EGR_MPLS_SPECIAL_LABEL_CONTROLr, val64,
                TTL_SOURCEf)) {

            if (!(label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL)) {
                return BCM_E_NOT_FOUND;
            }
        } else if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL) {
            return BCM_E_NOT_FOUND;
        }

        ptr->special_label_ref_count--;
        if (ptr->special_label_ref_count > 0) {
            return BCM_E_NONE;
        }

        COMPILER_64_ZERO(val64);
        rv = WRITE_EGR_MPLS_SPECIAL_LABEL_CONTROLr(unit, val64);

    } else {
        /* If there is no entry to delete then reutrn not found. */
        if (!(ptr->entropy_label_ref_count)) {
            return BCM_E_NOT_FOUND;
        }

       BCM_IF_ERROR_RETURN(READ_EGR_MPLS_ENTROPY_LABEL_CONTROLr(unit, &val));

        if (label_info.exp != soc_reg_field_get(unit,
            EGR_MPLS_ENTROPY_LABEL_CONTROLr, val, EXPf)) {
            return BCM_E_NOT_FOUND;
        }

        if (label_info.ttl != soc_reg_field_get(unit,
            EGR_MPLS_ENTROPY_LABEL_CONTROLr, val, TTLf)) {
            return BCM_E_NOT_FOUND;
        }

        if (label_info.label_value != soc_reg_field_get(unit,
            EGR_MPLS_ENTROPY_LABEL_CONTROLr, val, LABEL_OFFSETf)) {
            return BCM_E_NOT_FOUND;
        }

        if (soc_reg_field_get(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, val,
                EXP_SOURCEf)) {

            if (!(label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP)) {
                return BCM_E_NOT_FOUND;
            }
        } else if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_EXP) {
            return BCM_E_NOT_FOUND;
        }


        if (soc_reg_field_get(unit, EGR_MPLS_ENTROPY_LABEL_CONTROLr, val,
                TTL_SOURCEf)) {

            if (!(label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL)) {
                return BCM_E_NOT_FOUND;
            }
        } else if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_USE_TABLE_TTL) {
            return BCM_E_NOT_FOUND;
        }


        ptr->entropy_label_ref_count--;
        if (ptr->entropy_label_ref_count > 0) {
            return BCM_E_NONE;
        }
        val = 0;
        rv = WRITE_EGR_MPLS_ENTROPY_LABEL_CONTROLr(unit, val);
    }

    return rv;
}
/*
 * Function:
 *      bcmi_mpls_special_label_egress_delete_all
 * Purpose:
 *      Delete MPLS entropy/special labels in
 *      MPLS stack for egress tunnel encap.
 * Parameters:
 *      unit       - (IN) Device Number
 *      label_type - (IN) type of label
 *      label_info - (IN) Info about special label.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_mpls_special_label_egress_delete_all(int unit)

{

    uint64 val64;
    uint32 val = 0;
    bcmi_mpls_special_label_egress_t *ptr;

    /* pick up the local state. */
    ptr = (&(bcmi_special_label_egress_state[unit]));

    /* First delete the special label.*/
    COMPILER_64_ZERO(val64);
    BCM_IF_ERROR_RETURN(WRITE_EGR_MPLS_SPECIAL_LABEL_CONTROLr(unit, val64));
    ptr->special_label_ref_count = 0;

    /* Now delete the Entropy label.*/
    BCM_IF_ERROR_RETURN(WRITE_EGR_MPLS_ENTROPY_LABEL_CONTROLr(unit, val));
    ptr->entropy_label_ref_count = 0;

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcmi_mpls_special_label_egress_traverse
 * Purpose:
 *      Traverse MPLS entropy/special labels in
 *      MPLS stack for egress tunnel encap.
 * Parameters:
 *      unit       - (IN) Device Number
 *      label_type - (IN) type of label
 *      cb        - (IN) callback from user.
 *      user_data  - (INOUT) user provided data
 * Returns:
 *      BCM_E_XXX
 */

int 
bcmi_mpls_special_label_egress_traverse(
    int unit, 
    bcm_mpls_special_label_egress_traverse_cb cb, 
    void *user_data)
{
    int rv = BCM_E_NONE;
    bcm_mpls_special_label_t label_info;
    int i = 0;
    bcmi_mpls_special_label_egress_t *ptr;
    bcm_mpls_special_label_type_t label_type[2] = 
                        {bcmMplsSpecialLabelTypeSpecial,
                         bcmMplsSpecialLabelTypeEntropy};

    ptr = (&(bcmi_special_label_egress_state[unit]));


    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    /* run loop for both label types to get info */
    for (; i<2; i++) {

    if (label_type[i] == bcmMplsSpecialLabelTypeSpecial) {
        if (ptr->special_label_ref_count == 0) {
            continue;
        }
    } else {
        if (ptr->entropy_label_ref_count == 0) {
            continue;
        }
    }
    sal_memset(&label_info, 0, sizeof(label_info));

    BCM_IF_ERROR_RETURN(
        bcmi_mpls_special_label_egress_get(unit,
            label_type[i], &label_info));

    rv = cb(unit, &(label_type[i]), &label_info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
        return rv;
    }
#endif
    }

    return rv;
}

/*
 * Function:
 *      bcmi_mpls_special_label_push_action_set
 * Description:
 *      Set mpls push action in MPLS objects.
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      element             - (IN) MPLS object
 *      push_type           - (OUT) push action
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_push_action_set(int unit,
            bcm_mpls_special_label_push_element_t *element,
            bcm_mpls_special_label_push_type_t push_type)
{
    int vp=0xFFFF;
    ing_dvp_table_entry_t dvp;
    bcm_if_t nh_index;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    egr_l3_next_hop_entry_t egr_nh;
    int rv = BCM_E_NONE;
    int vc_swap_idx = 0, new_vc_swap_index = -1;
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int action = 0;

    if (element == NULL) {
        return BCM_E_PARAM;
    }

    if (push_type > bcmMplsSpecialLabelPushSpecialPlusEntropy) {
        return BCM_E_PARAM;
    }

    if (element->gport != BCM_GPORT_INVALID) {

        if (!BCM_GPORT_IS_MPLS_PORT(element->gport)) {
            return BCM_E_PARAM;
        }

        vp = BCM_GPORT_MPLS_PORT_ID_GET(element->gport);

        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* Get nh index from the DVP table */
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    } else {

        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, element->egr_obj_id) ||
            BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, element->egr_obj_id)) {

            /* Extract next hop index from unipath egress object. */
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, element->egr_obj_id)) {
                nh_index = element->egr_obj_id - BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                nh_index = element->egr_obj_id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
        } else {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                          nh_index, &egr_nh));

    /* First check if this is the correct MPLS next hop */
    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf) != 1) {
        return BCM_E_PARAM;
    }

    vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                          MPLS__VC_AND_SWAP_INDEXf);
 
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                          MEM_BLOCK_ANY, vc_swap_idx, &vc_entry));


    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                          &vc_entry, SPECIAL_LABEL_PUSH_TYPEf, push_type);

    /* Action for sw state management */
    action = mpls_info->egr_vc_swap_lbl_action[vc_swap_idx];

    /* Now we need to look if someone has similar entry like this */
    if ( mpls_info->vc_swap_ref_count[vc_swap_idx] > 1) {

        /* we have more objects pointing to it. so we can not update it just like that.
         * Look for some similar entry.
         * The new index is ref counted inside the below method only.
         * just remove the ref-count from old index once everything is set.
         */
        BCM_IF_ERROR_RETURN(_bcm_tr_mpls_get_vc_and_swap_table_index (unit, 0,
                           NULL, NULL, NULL, &vc_entry,
                           action,
                           &new_vc_swap_index));

        if ((new_vc_swap_index != -1) && (new_vc_swap_index != vc_swap_idx)) {
            /* write vc_entry into the new index */
            rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                          MEM_BLOCK_ALL, new_vc_swap_index, &vc_entry);

            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            /* Got new index. Point NH to it.*/
            soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh,
                          MPLS__VC_AND_SWAP_INDEXf, new_vc_swap_index);

            /* Lock the memory before writing. */
            soc_mem_lock(unit, EGR_L3_NEXT_HOPm);

            /* Write new index in NH */
            rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                           nh_index, &egr_nh);

            soc_mem_unlock(unit, EGR_L3_NEXT_HOPm);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
        if (new_vc_swap_index != -1) {
            /* Remove the reference from old index */
            (void)_bcm_tr_mpls_vc_and_swap_ref_count_adjust(unit,
                vc_swap_idx, -1);

            mpls_info->egr_vc_swap_lbl_action[new_vc_swap_index] = action;
        }
    } else {

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                          MEM_BLOCK_ALL, vc_swap_idx, &vc_entry));
    }

    return rv;

cleanup:

    if (new_vc_swap_index != -1) {
        (void)_bcm_tr_mpls_vc_and_swap_ref_count_adjust (unit,
                             new_vc_swap_index, -1);
        (void) _bcm_tr_mpls_vc_and_swap_table_index_reset (unit,
                             new_vc_swap_index);
    }


    return rv;
}

/*
 * Function:
 *      bcmi_mpls_special_label_push_action_get
 * Description:
 *      Get mpls push action in MPLS objects.
 *      going into MPLS tunnel.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      element             - (IN) MPLS object
 *      push_type           - (OUT) push action
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_push_action_get(int unit,
            bcm_mpls_special_label_push_element_t *element,
            bcm_mpls_special_label_push_type_t *push_type)
{
    int vp=0xFFFF;
    ing_dvp_table_entry_t dvp;
    bcm_if_t nh_index;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    egr_l3_next_hop_entry_t egr_nh;
    int vc_swap_idx = 0;

    if (element == NULL) {
        return BCM_E_PARAM;
    }

    if (push_type == NULL) {
        return BCM_E_PARAM;
    }

    if (element->gport != BCM_GPORT_INVALID) {

        if (!BCM_GPORT_IS_MPLS_PORT(element->gport)) {
            return BCM_E_PARAM;
        }

        vp = BCM_GPORT_MPLS_PORT_ID_GET(element->gport);

        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* Get nh index from the DVP table */
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    } else {
        nh_index = element->egr_obj_id;

        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, element->egr_obj_id) ||
            BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, element->egr_obj_id)) {

            /* Extract next hop index from unipath egress object. */
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, element->egr_obj_id)) {
                nh_index = element->egr_obj_id - BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                nh_index = element->egr_obj_id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
        } else {
            return BCM_E_PARAM;
        }
    }

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &egr_nh));

    /* First check if this is the correct MPLS next hop */
    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf) != 1) {
        return BCM_E_PARAM;
    }

    vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                      MPLS__VC_AND_SWAP_INDEXf);
      
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                      MEM_BLOCK_ANY, vc_swap_idx, &vc_entry));

    *push_type = soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_entry, SPECIAL_LABEL_PUSH_TYPEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mpls_special_label_move_entries
 * Description:
 *      In order to keep precedence of labels, we
 *      need to move entries.
 *
 * Parameters:
 *      unit          - (IN) unit number
 *      from          - (IN) Move from
 *      to            - (OUT) Move to
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_move_entries(int unit, int from, int to)
{

    special_label_control_entry_t spl_label_entry;
    soc_mem_t mem = SPECIAL_LABEL_CONTROLm;
    int i = 0;
    uint32 total_moves = 0;
    int num_entry;
    bcmi_mpls_special_label_control_t *ptr;

    ptr = bcmi_special_label_precedence_state[unit];

    total_moves = ((to>from) ? (to - from) : (from - to));
    num_entry = soc_mem_index_count(unit, SPECIAL_LABEL_CONTROLm);
    if (total_moves > num_entry) {
        return BCM_E_PARAM;
    }

if (from != -1) {
    if (to < from) {
        /* move entries up first*/
        while (total_moves) {

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, (to + i + 1), 
                    &spl_label_entry));

            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, (to + i), &spl_label_entry));

            sal_memcpy((&(ptr[to + i])), (&(ptr[to + i + 1])), 
                sizeof(bcmi_mpls_special_label_control_t));

            total_moves--;
            i++;
       }
    } else {
        while (total_moves) {

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, 
                    (to - i - 1), &spl_label_entry));

            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, 
                    (to - i), &spl_label_entry));

            sal_memcpy((&(ptr[to - i])), (&(ptr[to - i - 1])),
                sizeof(bcmi_mpls_special_label_control_t));

            total_moves--;
            i++;
        }
    }
    sal_memset((&(ptr[from])), 0,
        sizeof(bcmi_mpls_special_label_control_t));
} 
   
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mpls_special_label_make_slot
 * Description:
 *      Make slot for label identifiers in order of precedence.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      mpls_label       - (IN) label value
 *      mpls_label_mask  - (IN) Mask of label
 *      slot             - (OUT) slot to add entry
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_make_slot(int unit, uint32 mpls_label,
                    uint32 mpls_label_mask, int *slot)
{

    soc_mem_t mem;
    bcmi_mpls_special_label_control_t *ptr;
    int num_entry = 0, i;
    int free_slot = -1, move_start = -1;

    mem = SPECIAL_LABEL_CONTROLm;
    ptr = bcmi_special_label_precedence_state[unit];

    num_entry = soc_mem_index_count(unit, mem);

    /* First get the slot where it should go */
    for (i = 0; i < num_entry; i++) {

        if (!(ptr[i].valid)) {
            if (free_slot == -1) {
                free_slot = i;
            }
            continue;
        }

        if ((mpls_label & mpls_label_mask) <
            (ptr[i].label_value &  ptr[i].label_mask)) {
            continue;
        }

        if ((mpls_label & mpls_label_mask) ==
            (ptr[i].label_value & ptr[i].label_mask)) {
            if (mpls_label < ptr[i].label_value) {
                continue;
            }
        }

        if (free_slot != -1) {
            break;
        }

        /* if we are here then we have got the slot where we can put values*/
        if (move_start == -1) {
            move_start = i;
        }

    }

    if (free_slot == -1) {
        /* we do not have any free slot left */
        return BCM_E_RESOURCE;
    }

    /* If there is nothing to move then just return free index */
    *slot = ((move_start == -1) ? free_slot : move_start);

    /* When free index is before move index, then we need to move
     * one less entry as logic will give index to smaller entry.
     * In case free index is after move index, we will have to manage
     * entries from move index so we are okay.
     */
    move_start = ((free_slot < move_start) ? (move_start - 1) : move_start);
    bcmi_mpls_special_label_move_entries(unit, move_start, free_slot);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_mpls_special_label_match_entry
 * Description:
 *      Match incoming entry with label identifiers in h/w.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label type       - (IN) Type of label to match
 *      mpls_label       - (IN) label value
 *      mpls_label_mask  - (IN) Mask of label
 *      slot             - (OUT) matched slot
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int
bcmi_mpls_special_label_match_entry(int unit, int label_type, uint32 mpls_label,
                    uint32 mpls_label_mask, int *slot)
{ 

    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    bcmi_mpls_special_label_control_t *ptr;
    int i = 0;

    mem = SPECIAL_LABEL_CONTROLm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    ptr = bcmi_special_label_precedence_state[unit];

    for (i = index_min; i <= index_max; i++) {

        if (!(ptr[i].valid)) {
            continue;
        }

        if (ptr[i].label_type != label_type) {
            continue;
        }

        if ((mpls_label == ptr[i].label_value) &&
            (mpls_label_mask == ptr[i].label_mask)) {
            *slot = i;
            return BCM_E_NONE;
        }

    }

    return BCM_E_NOT_FOUND;
}
/*
 * Function:
 *      bcmi_mpls_special_label_get_slot 
 * Description:
 *      Get slot to add identifier entry of special label.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_value         - (IN) MPLS Label
 *      label_mask          - (IN) MPLS mask
 *      slot                - (OUT) slot number
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_get_slot(int unit, int label_type,
                    bcm_mpls_special_label_t label_info,  uint32 mpls_label,
                    uint32 mpls_label_mask, int *slot, int *new_slot)
{
    int rv = BCM_E_NONE;
    special_label_control_entry_t entry;

    /* first match the entry and check if matching entry is there */
    rv = bcmi_mpls_special_label_match_entry(unit, label_type, 
                mpls_label, mpls_label_mask, slot);
    if (BCM_SUCCESS(rv)) {
        /* Match the complete entry */
        BCM_IF_ERROR_RETURN(READ_SPECIAL_LABEL_CONTROLm(unit, MEM_BLOCK_ALL,
            *slot, &entry));

        if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
            CURRENT_LABEL_HASH_ACTIONf)) {

            if (!(label_info.flags & 
                BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_HASH)) {
                return BCM_E_EXISTS;
            }
        } else if (label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_HASH) {

                return BCM_E_EXISTS;
        }


        if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
            NEXT_LABEL_HASH_ACTIONf)) {

            if (!(label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_HASH)) {
                return BCM_E_EXISTS;
            }
        } else if (label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_HASH) {
                return BCM_E_EXISTS;
        }

        if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
           CURRENT_LABEL_LOOKUP_ACTIONf)) {

            if (!(label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_LKUP)) {
                return BCM_E_EXISTS;
            }
        } else if (label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_LKUP) {
                return BCM_E_EXISTS;
        }


        if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
            NEXT_LABEL_LOOKUP_ACTIONf)) {

            if (!(label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_LKUP)) {
                return BCM_E_EXISTS;
            }
        } else if (label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_LKUP) {
                return BCM_E_EXISTS;
        }


        if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
            LABEL_INHERIT_ENf)) {

            if (!(label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_INHERITANCE_ENABLE)) {
                return BCM_E_EXISTS;
            }
       } else if (label_info.flags &
                BCM_MPLS_SPECIAL_LABEL_INHERITANCE_ENABLE) {
                return BCM_E_EXISTS;
        }

    }

    if (rv != BCM_E_NOT_FOUND) {
        *new_slot = 0;
        return rv;
    }

    /*
     * We have not got matching entry.
     * we need to check if we can fit it somewhere.
     */
    rv = bcmi_mpls_special_label_make_slot(unit, mpls_label, mpls_label_mask,
                slot);
    *new_slot = 1;
    return rv;
}

/*
 * Function:
 *      bcmi_mpls_special_label_identifier_add
 * Description:
 *      Add identifier entry of special label.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_identifier_add(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{

    uint32 mpls_label = 0, mpls_label_mask = 0;
    int rv = BCM_E_NONE;
    special_label_control_entry_t entry;
    int slot = -1, new_slot = -1;
    bcmi_mpls_special_label_control_t *ptr;
    int bos_value = 0;
    int bos_mask = 0;

    if (!BCMI_MPLS_LABEL_ID_VALID(label_info.label_value)) {
        return BCM_E_PARAM;
    }

    if (!BCMI_MPLS_EXP_VALID(label_info.exp)) {
        return BCM_E_PARAM;
    }

    if (!BCMI_MPLS_TTL_VALID(label_info.ttl)) {
        return BCM_E_PARAM;
    }

    /* Check for range of mask also. */
    if (label_info.label_mask > BCMI_MPLS_LABEL_MASK_MAX) {
        return BCM_E_PARAM;
    }

    if (label_info.exp_mask > BCMI_MPLS_EXP_MASK_MAX) {
        return BCM_E_PARAM;
    }

    if (label_info.ttl_mask > BCMI_MPLS_TTL_MASK_MAX) {
        return BCM_E_PARAM;
    }

    if (label_type > bcmMplsSpecialLabelTypeEntropy) {
        return BCM_E_PARAM;
    }

    ptr = bcmi_special_label_precedence_state[unit];

    bos_value = ((label_info.flags & BCM_MPLS_SPECIAL_LABEL_BOS_PRESENT) ? 1:0);
    bos_mask  = ((label_info.flags & BCM_MPLS_SPECIAL_LABEL_BOS_MASKED) ? 1:0);

    mpls_label = BCMI_MPLS_CONSTRUCT_LABEL(
                 label_info.label_value,label_info.exp,label_info.ttl,
                 bos_value);

    mpls_label_mask = BCMI_MPLS_CONSTRUCT_LABEL(
                      label_info.label_mask, label_info.exp_mask,
                      label_info.ttl_mask,
                      bos_mask);

    /* get matching entry or new slot for adding new entry */
    BCM_IF_ERROR_RETURN(bcmi_mpls_special_label_get_slot(unit, label_type, label_info,
                    mpls_label, mpls_label_mask, &slot, &new_slot));

    /* Check if we found some old slot */
    if (!new_slot) {
        ptr[slot].ref_count++;
        return BCM_E_NONE;
    }
    /* For new slots, start writing the entry */
    sal_memset(&entry, 0, sizeof(special_label_control_entry_t));
    soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry, LABEL_VALUEf, 
                    mpls_label);
    soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry, LABEL_VALUE_MASKf,
                    mpls_label_mask);

    soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry, CURRENT_LABEL_TYPEf,
                    label_type);

    if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_HASH) {
        soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry,
                    CURRENT_LABEL_HASH_ACTIONf, 1);
    }

    if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_HASH) {
        soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry,
                    NEXT_LABEL_HASH_ACTIONf, 1);
    }

    if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_LKUP) {
        soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry,
                    CURRENT_LABEL_LOOKUP_ACTIONf, 1);
    }

    if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_LKUP) {
        soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry,
                    NEXT_LABEL_LOOKUP_ACTIONf, 1);
    }
    if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_INHERITANCE_ENABLE) {
        soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry,
            LABEL_INHERIT_ENf, 1);
    }

    if (label_type == bcmMplsSpecialLabelTypeNone) {
        if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_INHERITANCE_ENABLE) {
            soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry,
                    INHERITANCE_TYPEf, 0);
        }
    } else {
        if (label_info.flags & BCM_MPLS_SPECIAL_LABEL_INHERITANCE_ENABLE) {
            soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry,
                    INHERITANCE_TYPEf, 1);
        }
    }

    soc_SPECIAL_LABEL_CONTROLm_field32_set(unit, &entry, VALIDf, 1);

    rv = WRITE_SPECIAL_LABEL_CONTROLm(unit, MEM_BLOCK_ALL, slot, &entry);
    if (BCM_SUCCESS(rv)) {
        /* Clear the s/w state first */
        sal_memset((&(ptr[slot])), 0, sizeof(bcmi_mpls_special_label_control_t));

        if (rv == BCM_E_NONE) {
            ptr[slot].label_value = mpls_label;
            ptr[slot].label_mask  = mpls_label_mask;
            ptr[slot].label_type  = label_type;
            ptr[slot].valid = 1;
            ptr[slot].ref_count = 1;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmi_mpls_special_label_identifier_get
 * Description:
 *      Get identifier entry of special label.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_identifier_get(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t *label_info)
{
    int slot = -1;
    special_label_control_entry_t entry;
    uint32 mpls_label = 0;
    uint32 mpls_label_mask = 0;
    int bos_value = 0;
    int bos_mask = 0;

    if (label_info == NULL) {
        return BCM_E_PARAM;
    }

    if (label_type > bcmMplsSpecialLabelTypeEntropy) {
        return BCM_E_PARAM;
    }

    bos_value = 
        ((label_info->flags & BCM_MPLS_SPECIAL_LABEL_BOS_PRESENT) ? 1:0);
    bos_mask  = 
        ((label_info->flags & BCM_MPLS_SPECIAL_LABEL_BOS_MASKED) ? 1:0);

    mpls_label = BCMI_MPLS_CONSTRUCT_LABEL(
                 label_info->label_value, label_info->exp, label_info->ttl,
                 bos_value);

    mpls_label_mask = BCMI_MPLS_CONSTRUCT_LABEL(
                      label_info->label_mask, label_info->exp_mask,
                      label_info->ttl_mask,
                      bos_mask);

    /* get matching entry or new slot for adding new entry */
    BCM_IF_ERROR_RETURN(bcmi_mpls_special_label_match_entry(unit, label_type,
        mpls_label, mpls_label_mask, &slot));

    sal_memset(&entry, 0, sizeof(special_label_control_entry_t));
    BCM_IF_ERROR_RETURN(READ_SPECIAL_LABEL_CONTROLm(unit, MEM_BLOCK_ALL, 
        slot, &entry)); 

    if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry, 
        CURRENT_LABEL_HASH_ACTIONf)) {

        label_info->flags |= BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_HASH;
    }

    if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
        NEXT_LABEL_HASH_ACTIONf)) {
        label_info->flags |= BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_HASH;
    }

    if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry, 
        CURRENT_LABEL_LOOKUP_ACTIONf)) {

        label_info->flags |= BCM_MPLS_SPECIAL_LABEL_CURR_LABEL_EXCLUDE_LKUP;
    }

    if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
        NEXT_LABEL_LOOKUP_ACTIONf)) {
        label_info->flags |= BCM_MPLS_SPECIAL_LABEL_NEXT_LABEL_EXCLUDE_LKUP;
    }

    if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
        LABEL_INHERIT_ENf)) {
        label_info->flags |=  BCM_MPLS_SPECIAL_LABEL_INHERITANCE_ENABLE;
    }

    if (soc_SPECIAL_LABEL_CONTROLm_field32_get(unit, &entry,
                INHERITANCE_TYPEf)) {

        label_info->flags |= BCM_MPLS_SPECIAL_LABEL_INHERITANCE_ENABLE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mpls_special_label_identifier_delete
 * Description:
 *      Delete identifiers of special label.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_identifier_delete(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info)
{
    int slot = -1;
    int rv = BCM_E_NONE;
    special_label_control_entry_t entry;
    bcmi_mpls_special_label_control_t *ptr;
    uint32 mpls_label = 0;
    uint32 mpls_label_mask = 0;
    int bos_value = 0;
    int bos_mask = 0;

    ptr = bcmi_special_label_precedence_state[unit];

    if (label_type > bcmMplsSpecialLabelTypeEntropy) {
        return BCM_E_PARAM;
    }

    bos_value = ((label_info.flags & BCM_MPLS_SPECIAL_LABEL_BOS_PRESENT) ? 1:0);
    bos_mask  = ((label_info.flags & BCM_MPLS_SPECIAL_LABEL_BOS_MASKED) ? 1:0);

    mpls_label = BCMI_MPLS_CONSTRUCT_LABEL(
                 label_info.label_value,label_info.exp,label_info.ttl,
                 bos_value);

    mpls_label_mask = BCMI_MPLS_CONSTRUCT_LABEL(
                    label_info.label_mask, label_info.exp_mask,
                    label_info.ttl_mask,
                    bos_mask);

    /* get matching entry or new slot for adding new entry */
    BCM_IF_ERROR_RETURN(bcmi_mpls_special_label_match_entry(unit, label_type,
                    mpls_label, mpls_label_mask, &slot));

    ptr[slot].ref_count--;
    if (ptr[slot].ref_count > 0) {
        /* more applications are still using this. Do not remove*/
        return BCM_E_NONE;
    }
    /* If we are here then nobody else has programmed this*/
    sal_memset(&(ptr[slot]), 0, sizeof(bcmi_mpls_special_label_control_t));
    /* Make the s/w entry as 0 */
    sal_memset(&entry, 0, sizeof(special_label_control_entry_t));
    /* Write the cleared entry into h/w */
    rv = WRITE_SPECIAL_LABEL_CONTROLm(unit, MEM_BLOCK_ALL, slot, &entry);

    return rv;
}

/*
 * Function:
 *      bcmi_mpls_special_label_identifier_delete_all
 * Description:
 *      Delete all identifiers ofs pecial labels.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      label_type          - (IN) MPLS Label
 *      label_info          - (IN) MPLS Gport
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_identifier_delete_all(int unit)
{
    int i=0;
    special_label_control_entry_t entry;
    bcmi_mpls_special_label_control_t *ptr;
    int num_entry;

    num_entry = soc_mem_index_count(unit, SPECIAL_LABEL_CONTROLm);

    ptr = bcmi_special_label_precedence_state[unit];

    for (; i<num_entry; i++) {
        if (ptr[i].cancun_added) {
            /* if this is added by cancun then
               make ref_count to 1 and do not do much. */
            ptr[i].ref_count = 1;
            continue;
        }

        /* If we are here then nobody else has programmed this*/
        sal_memset(&(ptr[i]), 0, sizeof(bcmi_mpls_special_label_control_t));
        /* Make the s/w entry as 0 */
        sal_memset(&entry, 0, sizeof(special_label_control_entry_t));
        /* Write the cleared entry into h/w */
        BCM_IF_ERROR_RETURN(
            WRITE_SPECIAL_LABEL_CONTROLm(unit, MEM_BLOCK_ALL, i, &entry));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_mpls_special_label_identifier_traverse
 * Description:
 *      Traverse and call user call back for all special labels identifiers.
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      cb                  - (IN) user callback
 *      user data           - (IN) option user data
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_mpls_special_label_identifier_traverse(int unit,
            bcm_mpls_special_label_identifier_traverse_cb cb, void *user_data)
{
    int i=0;
    bcmi_mpls_special_label_control_t *ptr;
    int num_entry;
    bcm_mpls_special_label_t label_info;
    int rv = BCM_E_NONE;
    bcm_mpls_special_label_type_t label_type;

    num_entry = soc_mem_index_count(unit, SPECIAL_LABEL_CONTROLm);

    ptr = bcmi_special_label_precedence_state[unit];

    for (; i<num_entry; i++) {
       sal_memset(&label_info, 0, sizeof(bcm_mpls_special_label_t));
        if (!(ptr[i].valid)) {
            continue;
        }
        label_info.label_value = BCMI_MPLS_LABEL_VAL_FROM_LABEL_GET(ptr[i].label_value);
        label_info.label_mask  = BCMI_MPLS_LABEL_VAL_FROM_LABEL_GET(ptr[i].label_mask);

        label_info.exp = BCMI_MPLS_EXP_FROM_LABEL_GET(ptr[i].label_value);
        label_info.exp_mask = BCMI_MPLS_EXP_FROM_LABEL_GET(ptr[i].label_mask);

        label_info.ttl = BCMI_MPLS_TTL_FROM_LABEL_GET(ptr[i].label_value);
        label_info.ttl_mask = BCMI_MPLS_TTL_FROM_LABEL_GET(ptr[i].label_mask);
        label_type = ptr[i].label_type;

        if (BCMI_MPLS_EXP_FROM_LABEL_GET(ptr[i].label_value)) {
            label_info.flags |= BCM_MPLS_SPECIAL_LABEL_BOS_PRESENT;
        }        

        if (BCMI_MPLS_EXP_FROM_LABEL_GET(ptr[i].label_mask)) {
            label_info.flags |= BCM_MPLS_SPECIAL_LABEL_BOS_MASKED;
        }        

        bcmi_mpls_special_label_identifier_get(unit, label_type, &label_info);
 
        rv = cb(unit, &label_type, &label_info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#endif
    }
    return rv;
}

#endif /* BCM_SPECIAL_LABEL_SUPPORT */
#endif /* BCM_MPLS_SUPPORT */
#endif /* INCLUDE_L3 */
