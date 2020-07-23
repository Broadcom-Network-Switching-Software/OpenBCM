/** \file vlan_range.c
 * 
 *
 * VLAN compression for DNX.
 * Implment VLAN compression relate apis in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/field/field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/algo/vlan/algo_vlan.h>

#define VLAN_RANGE_ENTRY(unit)                   (dnx_data_vlan.vlan_translate.nof_vlan_range_entries_get(unit))
#define VLAN_RANGES_PER_ENTRY(unit)              (dnx_data_vlan.vlan_translate.nof_vlan_ranges_per_entry_get(unit))

/**
 * \brief - Update vlan range info in the HW
 *
 * \param [in] unit - unit id
 * \param [in] vlan_range_profile - index of vlan range
 * \param [in] template - vlan ranges to update
 * \param [in] is_outer - TRUE for outer vlan, FALSE for inner vlan
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_range_info_hw_set(
    int unit,
    uint32 vlan_range_profile,
    vlan_range_profile_template_t * template,
    int is_outer)
{
    uint32 entry_handle_id;
    int low_vid, high_vid;
    int vlan_range_index;
    dbal_fields_e vlan_range_low_field, vlan_range_high_field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (is_outer)
    {
        vlan_range_low_field = DBAL_FIELD_OUTER_VLAN_RANGE_LOWER;
        vlan_range_high_field = DBAL_FIELD_OUTER_VLAN_RANGE_UPPER;
    }
    else
    {
        vlan_range_low_field = DBAL_FIELD_INNER_VLAN_RANGE_LOWER;
        vlan_range_high_field = DBAL_FIELD_INNER_VLAN_RANGE_UPPER;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_RANGE_COMPRESSION_TABLE, &entry_handle_id));
    for (vlan_range_index = 0; vlan_range_index < VLAN_RANGES_PER_ENTRY(unit); vlan_range_index++)
    {
        low_vid = template->vlan_range[vlan_range_index].lower;
        high_vid = template->vlan_range[vlan_range_index].upper;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ENTRY_ID, vlan_range_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_RANGE_ID, vlan_range_index);

        dbal_entry_value_field32_set(unit, entry_handle_id, vlan_range_low_field, INST_SINGLE, low_vid);
        dbal_entry_value_field32_set(unit, entry_handle_id, vlan_range_high_field, INST_SINGLE, high_vid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get vlan range entry index and mask for a given vlan domain from HW.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vlan_domain - vlan domain to get data from HW.
 * \param [out] vlan_domain_attribute_info - Vlan domain attribute (entry index and mask) info get from HW.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *   None.
 */
static shr_error_e
dnx_vlan_range_start_entry_hw_get(
    int unit,
    int vlan_domain,
    dnx_vlan_domain_info_t * vlan_domain_attribute_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    sal_memset(vlan_domain_attribute_info, 0, sizeof(dnx_vlan_domain_info_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_DOMAIN_ATTR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RANGE_OUTER_INDEX, INST_SINGLE,
                               &vlan_domain_attribute_info->vlan_range_outer_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RANGE_INNER_INDEX, INST_SINGLE,
                               &vlan_domain_attribute_info->vlan_range_inner_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RANGE_OUTER_MASK, INST_SINGLE,
                               &vlan_domain_attribute_info->vlan_range_outer_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RANGE_INNER_MASK, INST_SINGLE,
                               &vlan_domain_attribute_info->vlan_range_inner_mask);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
/**
 * \brief - Set vlan range entry index and mask for a given vlan domain to HW.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vlan_domain - vlan domain to set date to HW.
 * \param [in] vlan_domain_attribute_info - Vlan domain attribute (entry index and mask) info set to HW.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *   None.
 */

static shr_error_e
dnx_vlan_range_start_entry_hw_set(
    int unit,
    int vlan_domain,
    dnx_vlan_domain_info_t * vlan_domain_attribute_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_DOMAIN_ATTR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_OUTER_MASK, INST_SINGLE,
                                 vlan_domain_attribute_info->vlan_range_outer_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_INNER_MASK, INST_SINGLE,
                                 vlan_domain_attribute_info->vlan_range_inner_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_OUTER_INDEX, INST_SINGLE,
                                 vlan_domain_attribute_info->vlan_range_outer_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_INNER_INDEX, INST_SINGLE,
                                 vlan_domain_attribute_info->vlan_range_inner_index);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
/**
 * \brief - Check if vlan range is overlap
 *
 * \param [in] unit - Relevant unit
 * \param [in] new_vlan_range - vlan range for check
 * \param [in] template - vlan ranges template for check
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_range_overlap_verify(
    int unit,
    dnx_vlan_range_info_t * new_vlan_range,
    vlan_range_profile_template_t * template)
{
    int vlan_range_index;
    int vlan_range_low, vlan_range_high;
    int is_full;
    SHR_FUNC_INIT_VARS(unit);
    is_full = TRUE;
    for (vlan_range_index = 0; vlan_range_index < VLAN_RANGES_PER_ENTRY(unit); vlan_range_index++)
    {
        vlan_range_low = template->vlan_range[vlan_range_index].lower;
        vlan_range_high = template->vlan_range[vlan_range_index].upper;
        if ((vlan_range_low == VLAN_RANGE_DEFAULT) && (vlan_range_high == VLAN_RANGE_DEFAULT))
        {
            is_full = FALSE;
        }
        else if (!((new_vlan_range->vlan_range_lower > vlan_range_high)
                   || (new_vlan_range->vlan_range_upper < vlan_range_low)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "%s Vlan ranges[%d,%d] overlap with exist range[%d,%d]",
                         new_vlan_range->is_outer ? "OUTER" : "INNER",
                         new_vlan_range->vlan_range_lower, new_vlan_range->vlan_range_upper,
                         vlan_range_low, vlan_range_high);
        }

    }
    if (is_full)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "%s Vlan ranges Profile template is full",
                     new_vlan_range->is_outer ? "OUTER" : "INNER");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - calculate the mask of vlan range template
 *
 * \param [in] unit - unit id
 * \param [in] template - vlan range template to get th mask
 * \param [out] mask - return the mask of vlan_range_profile_template
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_vlan_range_template_mask_get(
    int unit,
    vlan_range_profile_template_t * template,
    uint32 *mask)
{
    int vlan_range_index;
    int vlan_range_low, vlan_range_high;
    *mask = 0;
    for (vlan_range_index = 0; vlan_range_index < VLAN_RANGES_PER_ENTRY(unit); vlan_range_index++)
    {
        vlan_range_low = template->vlan_range[vlan_range_index].lower;
        vlan_range_high = template->vlan_range[vlan_range_index].upper;
        if ((vlan_range_high != VLAN_RANGE_DEFAULT) && (vlan_range_low != VLAN_RANGE_DEFAULT))
        {
            *mask |= 0x1 << vlan_range_index;
        }
    }
}
/**
 * \brief - Check if vlan range is overlap for same vlan domain
 *
 * \param [in] unit - Relevant unit
 * \param [in] vlan_range - vlan range index
 * \param [in,out] template - vlan ranges need to update
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_range_template_update(
    int unit,
    dnx_vlan_range_info_t * vlan_range,
    vlan_range_profile_template_t * template)
{
    int index, vlan_range_index, is_add;
    int vlan_range_low, vlan_range_high;
    SHR_FUNC_INIT_VARS(unit);
    is_add = vlan_range->is_add;
    /*
     *Vlan ranges is ascending order by low_vid
     *1.For add, Find the index to insert new range
     *2.For remove, move the range
     */
    for (vlan_range_index = 0; vlan_range_index < VLAN_RANGES_PER_ENTRY(unit); vlan_range_index++)
    {
        vlan_range_low = template->vlan_range[vlan_range_index].lower;
        vlan_range_high = template->vlan_range[vlan_range_index].upper;
        /*
         *Add a Vlan range
         *1.Found the position index to insert new range.
         *2.Move the ranges [index,last-1] to [index+1,last] make free space for new range.
         *3.insert the new range to position index.
         */
        if (is_add)
        {
            if (vlan_range_low == VLAN_RANGE_DEFAULT)
            {
                template->vlan_range[vlan_range_index].lower = vlan_range->vlan_range_lower;
                template->vlan_range[vlan_range_index].upper = vlan_range->vlan_range_upper;
                break;
            }
            else if (vlan_range->vlan_range_lower > vlan_range_low)
            {
                for (index = VLAN_RANGES_PER_ENTRY(unit) - 2; index >= vlan_range_index; index--)
                {
                    template->vlan_range[index + 1].lower = template->vlan_range[index].lower;
                    template->vlan_range[index + 1].upper = template->vlan_range[index].upper;
                }
                template->vlan_range[vlan_range_index].lower = vlan_range->vlan_range_lower;
                template->vlan_range[vlan_range_index].upper = vlan_range->vlan_range_upper;
                break;
            }
        }
        else
        {
            /*
             *Remove a Vlan range
             *1.Set the range value to default value.
             *2.Move the left range(s) to fill the removed range.
             *3.Set the last valid range to default value.
             */
            if ((vlan_range_high == vlan_range->vlan_range_upper) && (vlan_range_low == vlan_range->vlan_range_lower))
            {
                template->vlan_range[vlan_range_index].lower = VLAN_RANGE_DEFAULT;
                template->vlan_range[vlan_range_index].upper = VLAN_RANGE_DEFAULT;
                for (index = vlan_range_index; index < VLAN_RANGES_PER_ENTRY(unit) - 1; index++)
                {
                    template->vlan_range[index].lower = template->vlan_range[index + 1].lower;
                    template->vlan_range[index].upper = template->vlan_range[index + 1].upper;
                }
                template->vlan_range[VLAN_RANGES_PER_ENTRY(unit) - 1].lower = VLAN_RANGE_DEFAULT;
                template->vlan_range[VLAN_RANGES_PER_ENTRY(unit) - 1].upper = VLAN_RANGE_DEFAULT;
                break;
            }
        }
    }
    if (vlan_range_index == VLAN_RANGES_PER_ENTRY(unit))
    {
        if (is_add)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "%s Vlan ranges Profile template is full",
                         vlan_range->is_outer ? "OUTER" : "INNER");
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "%s Vlan range [%d %d] not found",
                         vlan_range->is_outer ? "OUTER" : "INNER",
                         vlan_range->vlan_range_lower, vlan_range->vlan_range_upper);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add the vlan range info.
 *
 * \param [in] unit - unit ID
 * \param [in] vlan_domain - key to update
 * \param [in] vlan_range - vlan range profile info to add
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_range_update(
    int unit,
    int vlan_domain,
    dnx_vlan_range_info_t * vlan_range)
{
    dnx_vlan_domain_info_t vlan_domain_attr;
    vlan_range_profile_template_t template;
    int is_outer, is_add, ref_count;
    uint8 is_first, is_last;
    int old_vlan_range_profile, new_vlan_range_profile;
    uint32 vlan_range_mask;

    SHR_FUNC_INIT_VARS(unit);
    is_outer = vlan_range->is_outer;
    is_add = vlan_range->is_add;
    /** Get the vlan domain attribute profile info from HW */
    SHR_IF_ERR_EXIT(dnx_vlan_range_start_entry_hw_get(unit, vlan_domain, &vlan_domain_attr));

    old_vlan_range_profile =
        is_outer ? vlan_domain_attr.vlan_range_outer_index : vlan_domain_attr.vlan_range_inner_index;
    is_first = FALSE;
    is_last = FALSE;
    /*
     *1.Get the old profile
     *2.Check if vlan range overlap
     *3.Update template data
     *4.Exchange template
     */
    if (is_outer)
    {
        SHR_IF_ERR_EXIT(vlan_db.vlan_range_outer_profile.profile_data_get(unit, old_vlan_range_profile,
                                                                          &ref_count, &template));
        if (is_add)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_range_overlap_verify(unit, vlan_range, &template));
        }
        SHR_IF_ERR_EXIT(dnx_vlan_range_template_update(unit, vlan_range, &template));
        SHR_IF_ERR_EXIT(vlan_db.vlan_range_outer_profile.exchange(unit, 0, &template,
                                                                  old_vlan_range_profile, NULL, &new_vlan_range_profile,
                                                                  &is_first, &is_last));
    }
    else
    {
        SHR_IF_ERR_EXIT(vlan_db.vlan_range_inner_profile.profile_data_get(unit, old_vlan_range_profile,
                                                                          &ref_count, &template));
        if (is_add)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_range_overlap_verify(unit, vlan_range, &template));
        }
        SHR_IF_ERR_EXIT(dnx_vlan_range_template_update(unit, vlan_range, &template));
        SHR_IF_ERR_EXIT(vlan_db.vlan_range_inner_profile.exchange(unit, 0, &template,
                                                                  old_vlan_range_profile, NULL, &new_vlan_range_profile,
                                                                  &is_first, &is_last));
    }
    /*
     * Update vlan range table to HW
     */
    if (is_first == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_range_info_hw_set(unit, new_vlan_range_profile, &template, is_outer));
    }
    /*
     * Set new mask and vlan range profile to vlan domain attritube table
     */
    dnx_vlan_range_template_mask_get(unit, &template, &vlan_range_mask);
    if (is_outer)
    {
        vlan_domain_attr.vlan_range_outer_mask = vlan_range_mask;
        vlan_domain_attr.vlan_range_outer_index = new_vlan_range_profile;
    }
    else
    {
        vlan_domain_attr.vlan_range_inner_mask = vlan_range_mask;
        vlan_domain_attr.vlan_range_inner_index = new_vlan_range_profile;
    }
    SHR_IF_ERR_EXIT(dnx_vlan_range_start_entry_hw_set(unit, vlan_domain, &vlan_domain_attr));

    /*
     * When old vlan range profile is not referenced, clear related entries in HW table
     */
    if (is_last == TRUE && old_vlan_range_profile != new_vlan_range_profile)
    {
        sal_memset(&template, VLAN_RANGE_DEFAULT, sizeof(template));
        SHR_IF_ERR_EXIT(dnx_vlan_range_info_hw_set(unit, old_vlan_range_profile, &template, is_outer));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify Port, vlan ranges parameters for
 * BCM-API: bcm_dnx_vlan_translate_action_range_add()
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Ingress generic port
 * \param [in] outer_vlan_low - Packet outer VLAN ID low
 * \param [in] outer_vlan_high - Packet outer VLAN ID high
 * \param [in] inner_vlan_low - Packet inner VLAN ID low
 * \param [in] inner_vlan_high - Packet inner VLAN ID high
 * \param [in] action - Action for outer and inner tag
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_translate_action_range_add_verify(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_vlan_action_set_t * action)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((outer_vlan_low != BCM_VLAN_INVALID) && (inner_vlan_low != BCM_VLAN_INVALID))
    {
        if ((action->dt_outer != bcmVlanActionCompressed) || (action->dt_inner != bcmVlanActionCompressed))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Valid VLAN range for double tags, but action not correct.");
        }
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_high);
        if (inner_vlan_high < inner_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid inner VLAN range");
        }

        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_high);
        if (outer_vlan_high < outer_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid outer VLAN range");
        }
    }
    else if (outer_vlan_low != BCM_VLAN_INVALID)
    {
        if (action->ot_outer != bcmVlanActionCompressed)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Valid VLAN range for outer tag, but action not correct.");
        }
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_high);
        if (outer_vlan_high < outer_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid outer VLAN range");
        }
    }
    else if (inner_vlan_low != BCM_VLAN_INVALID)
    {
        if (action->it_inner != bcmVlanActionCompressed)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Valid VLAN range for inner tag, but action not correct.");
        }
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_high);
        if (inner_vlan_high < inner_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid inner VLAN range");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VLAN range not supported.");
    }

    /*
     * Verify action
     */
    if ((action->ot_outer != bcmVlanActionCompressed) &&
        (action->it_inner != bcmVlanActionCompressed) &&
        (action->dt_outer != bcmVlanActionCompressed) && (action->dt_inner != bcmVlanActionCompressed))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VLAN range action not correct.");
    }
    else if ((action->ot_outer == bcmVlanActionCompressed) && (action->new_outer_vlan != outer_vlan_low))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "new outer VLAN not correct.");
    }
    else if ((action->it_inner == bcmVlanActionCompressed) && (action->new_inner_vlan != inner_vlan_low))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "new inner VLAN not correct.");
    }
    else if ((action->dt_outer == bcmVlanActionCompressed) && (action->dt_inner == bcmVlanActionCompressed))
    {
        if ((action->new_outer_vlan != outer_vlan_low) || (action->new_inner_vlan != inner_vlan_low))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "new outer VLAN or new inner vlan not correct.");
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify Port, vlan ranges parameters for
 * BCM-API: bcm_dnx_vlan_translate_action_range_delete()
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Ingress generic port
 * \param [in] outer_vlan_low - Packet outer VLAN ID low
 * \param [in] outer_vlan_high - Packet outer VLAN ID high
 * \param [in] inner_vlan_low - Packet inner VLAN ID low
 * \param [in] inner_vlan_high - Packet inner VLAN ID high
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_translate_action_range_delete_verify(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((outer_vlan_low != BCM_VLAN_INVALID) && (inner_vlan_low != BCM_VLAN_INVALID))
    {
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_high);
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_high);
    }
    else if (outer_vlan_low != BCM_VLAN_INVALID)
    {
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_high);
    }
    else if (inner_vlan_low != BCM_VLAN_INVALID)
    {
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_high);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VLAN ID.");
    }
    if (((outer_vlan_low != BCM_VLAN_INVALID) && (outer_vlan_low > outer_vlan_high))
        || ((inner_vlan_low != BCM_VLAN_INVALID) && (inner_vlan_low > inner_vlan_high)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VLAN range");
    }
exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - Verify Port, vlan ranges parameters for
 * BCM-API: bcm_dnx_vlan_translate_action_range_get()
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Ingress generic port
 * \param [in] outer_vlan_low - Packet outer VLAN ID low
 * \param [in] outer_vlan_high - Packet outer VLAN ID high
 * \param [in] inner_vlan_low - Packet inner VLAN ID low
 * \param [in] inner_vlan_high - Packet inner VLAN ID high
 * \param [out] key_type - return the key type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_translate_action_range_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    int *key_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((outer_vlan_low != BCM_VLAN_INVALID) && (inner_vlan_low != BCM_VLAN_INVALID))
    {
        *key_type = bcmVlanTranslateKeyPortDouble;
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_high);
        if (inner_vlan_high < inner_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid inner VLAN range");
        }

        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_high);
        if (outer_vlan_high < outer_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid outer VLAN range");
        }
    }
    else if (outer_vlan_low != BCM_VLAN_INVALID)
    {
        *key_type = bcmVlanTranslateKeyPortOuter;
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, outer_vlan_high);
        if (outer_vlan_high < outer_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid outer VLAN range");
        }
    }
    else if (inner_vlan_low != BCM_VLAN_INVALID)
    {
        *key_type = bcmVlanTranslateKeyPortInner;
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DNX_VLAN_CHK_ID(unit, inner_vlan_high);
        if (inner_vlan_high < inner_vlan_low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid inner VLAN range");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VLAN range not supported.");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get vlan domain for a prot
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Ingress generic port
 * \param [out] vlan_domain - vlan domain of the port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_vlan_port_get_vlan_domain(
    int unit,
    bcm_gport_t port,
    uint32 *vlan_domain)
{
    uint8 is_phy_port;
    SHR_FUNC_INIT_VARS(unit);
    /** Check if the port is physical */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    /*
     * If the port is physical then get pp port vlan domain
     * If the port is not physical then get the lif vlan domain
     */
    if (is_phy_port != FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, vlan_domain));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_lif_ingress_vlan_domain_get(unit, port, vlan_domain));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Add a range of VLANs and an entry from ingress VLAN translation table
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Ingress generic port
 * \param [in] outer_vlan_low - Packet outer VLAN ID low
 * \param [in] outer_vlan_high - Packet outer VLAN ID high
 * \param [in] inner_vlan_low - Packet inner VLAN ID low
 * \param [in] inner_vlan_high - Packet inner VLAN ID high
 * \param [in] action - Action for outer and inner tag. Use bcmVlanActionCompressed to enable comprssion.
 *
 * \return
 *   shr_error_e return _SHR_E_PARAM if actions inconsistence whit input VID.
 *   ref to dnx_vlan_translate_action_range_add_verify for detail parameter checking logic.
 * \remark
 *  Support actions:
 *  Double tag compression:
 *     action.dt_outer = bcmVlanActionCompressed.
 *     action.dt_inner = bcmVlanActionCompressed.
 *     action.new_outer_vlan = outer_vlan_low.
 *     action.new_inner_vlan = inner_vlan_low.
 *  Single outer tag compression:
 *     action.ot_outer = bcmVlanActionCompressed.
 *     action.new_outer_vlan = outer_vlan_low.
 *  Single inner tag compression:
 *     action.it_outer = bcmVlanActionCompressed.
 *     action.new_inner_vlan = inner_vlan_low.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_vlan_action_set_t * action)
{
    dnx_vlan_range_info_t vlan_range_profile_info;
    uint32 vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_range_add_verify
                          (unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, action));
    SHR_IF_ERR_EXIT(dnx_vlan_port_get_vlan_domain(unit, port, &vlan_domain));
    if (outer_vlan_low != BCM_VLAN_INVALID)
    {
        vlan_range_profile_info.vlan_range_lower = outer_vlan_low;
        vlan_range_profile_info.vlan_range_upper = outer_vlan_high;
        vlan_range_profile_info.is_outer = TRUE;
        vlan_range_profile_info.is_add = TRUE;
        SHR_IF_ERR_EXIT(dnx_vlan_range_update(unit, vlan_domain, &vlan_range_profile_info));
    }
    if (inner_vlan_low != BCM_VLAN_INVALID)
    {
        vlan_range_profile_info.vlan_range_lower = inner_vlan_low;
        vlan_range_profile_info.vlan_range_upper = inner_vlan_high;
        vlan_range_profile_info.is_outer = FALSE;
        vlan_range_profile_info.is_add = TRUE;
        SHR_IF_ERR_EXIT(dnx_vlan_range_update(unit, vlan_domain, &vlan_range_profile_info));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Delete a range of VLANs and an entry from ingress VLAN translation table
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Ingress generic port
 * \param [in] outer_vlan_low - Packet outer VLAN ID low
 * \param [in] outer_vlan_high - Packet outer VLAN ID high
 * \param [in] inner_vlan_low - Packet inner VLAN ID low
 * \param [in] inner_vlan_high - Packet inner VLAN ID high
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  set outer_vlan_low to BCM_VLAN_INVALID if only update inner vlan
 *  set inner_vlan_low to BCM_VLAN_INVALID if only update outer vlan
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_translate_action_range_delete(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high)
{
    dnx_vlan_range_info_t vlan_range_profile_info;
    uint32 vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_range_delete_verify
                          (unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high));
    SHR_IF_ERR_EXIT(dnx_vlan_port_get_vlan_domain(unit, port, &vlan_domain));
    if (outer_vlan_low != BCM_VLAN_INVALID)
    {
        vlan_range_profile_info.vlan_range_lower = outer_vlan_low;
        vlan_range_profile_info.vlan_range_upper = outer_vlan_high;
        vlan_range_profile_info.is_outer = TRUE;
        vlan_range_profile_info.is_add = FALSE;
        SHR_IF_ERR_EXIT(dnx_vlan_range_update(unit, vlan_domain, &vlan_range_profile_info));
    }
    if (inner_vlan_low != BCM_VLAN_INVALID)
    {
        vlan_range_profile_info.vlan_range_lower = inner_vlan_low;
        vlan_range_profile_info.vlan_range_upper = inner_vlan_high;
        vlan_range_profile_info.is_outer = FALSE;
        vlan_range_profile_info.is_add = FALSE;
        SHR_IF_ERR_EXIT(dnx_vlan_range_update(unit, vlan_domain, &vlan_range_profile_info));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Delete all range of VLANs from ingress VLAN translation table
 *
 * \param [in] unit - Relevant unit
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_translate_action_range_delete_all(
    int unit)
{
    int index, is_outer;
    dnx_vlan_range_info_t vlan_range;
    dnx_vlan_domain_info_t vlan_domain_attr;
    vlan_range_profile_template_t vlan_range_template;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    sal_memset((void *) &vlan_range, 0, sizeof(vlan_range));
    sal_memset((void *) &vlan_domain_attr, 0, sizeof(vlan_domain_attr));
    /*
     *Clear vlan range start entry HW table
     */
    for (index = 0; index < dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit); index++)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_range_start_entry_hw_set(unit, index, &vlan_domain_attr));
    }
    /*
     *Clear template
     */
    SHR_IF_ERR_EXIT(vlan_db.vlan_range_outer_profile.clear(unit));
    SHR_IF_ERR_EXIT(vlan_db.vlan_range_inner_profile.clear(unit));
    /*
     *Clear vlan range HW table
     */
    sal_memset(&vlan_range_template, VLAN_RANGE_DEFAULT, sizeof(vlan_range_template));
    for (index = 0; index < VLAN_RANGE_ENTRY(unit); index++)
    {
        is_outer = TRUE;
        SHR_IF_ERR_EXIT(dnx_vlan_range_info_hw_set(unit, index, &vlan_range_template, is_outer));
        is_outer = FALSE;
        SHR_IF_ERR_EXIT(dnx_vlan_range_info_hw_set(unit, index, &vlan_range_template, is_outer));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get a range of VLANs and an entry from ingress VLAN translation table
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Ingress generic port
 * \param [in] outer_vlan_low - Packet outer VLAN ID low
 * \param [in] outer_vlan_high - Packet outer VLAN ID high
 * \param [in] inner_vlan_low - Packet inner VLAN ID low
 * \param [in] inner_vlan_high - Packet inner VLAN ID high
 * \param [out] action - Action for outer and inner tag
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  Update result in action:
 *  For double tag:
 *     action.new_outer_vlan = outer_vlan_low.
 *     action.new_inner_vlan = inner_vlan_low.
 *  For single outer tag:
 *     action.new_outer_vlan = outer_vlan_low.
 *  For single inner tag:
 *     action.new_inner_vlan = inner_vlan_low.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_vlan_action_set_t * action)
{
    uint32 vlan_domain;
    int vlan_range_offset, key_type = 0, ref_count;
    int outer_template_vlan_low, outer_template_vlan_high, inner_template_vlan_low, inner_template_vlan_high;
    dnx_vlan_domain_info_t vlan_domain_attr;
    vlan_range_profile_template_t outer_template, inner_template;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** init default*/
    SHR_NULL_CHECK(action, _SHR_E_PARAM, "action");
    bcm_vlan_action_set_t_init(action);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_range_get_verify
                          (unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, &key_type));

    SHR_IF_ERR_EXIT(dnx_vlan_port_get_vlan_domain(unit, port, &vlan_domain));
    SHR_IF_ERR_EXIT(dnx_vlan_range_start_entry_hw_get(unit, vlan_domain, &vlan_domain_attr));

    /*
     *Check if the entry is empty
     */
    if ((!vlan_domain_attr.vlan_range_outer_mask) && (!vlan_domain_attr.vlan_range_inner_mask))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Vlan range table for vlan domain:%d for is empty", vlan_domain);
    }
    SHR_IF_ERR_EXIT(vlan_db.vlan_range_outer_profile.profile_data_get(unit,
                                                                      vlan_domain_attr.vlan_range_outer_index,
                                                                      &ref_count, &outer_template));
    SHR_IF_ERR_EXIT(vlan_db.vlan_range_inner_profile.profile_data_get(unit,
                                                                      vlan_domain_attr.vlan_range_inner_index,
                                                                      &ref_count, &inner_template));

    for (vlan_range_offset = 0; vlan_range_offset < VLAN_RANGES_PER_ENTRY(unit); vlan_range_offset++)
    {
        outer_template_vlan_low = outer_template.vlan_range[vlan_range_offset].lower;
        outer_template_vlan_high = outer_template.vlan_range[vlan_range_offset].upper;
        inner_template_vlan_low = inner_template.vlan_range[vlan_range_offset].lower;
        inner_template_vlan_high = inner_template.vlan_range[vlan_range_offset].upper;
        if ((key_type == bcmVlanTranslateKeyPortDouble)
            && (outer_template_vlan_low == outer_vlan_low)
            && (outer_template_vlan_high == outer_vlan_high)
            && (inner_template_vlan_low == inner_vlan_low) && (inner_template_vlan_high == inner_vlan_high))
        {
            action->dt_outer = bcmVlanActionCompressed;
            action->new_outer_vlan = outer_vlan_low;
            action->dt_inner = bcmVlanActionCompressed;
            action->new_inner_vlan = inner_vlan_low;
            break;
        }
        else if ((key_type == bcmVlanTranslateKeyPortOuter)
                 && (outer_template_vlan_low == outer_vlan_low) && (outer_template_vlan_high == outer_vlan_high))
        {
            action->ot_outer = bcmVlanActionCompressed;
            action->new_outer_vlan = outer_vlan_low;
            break;
        }
        else if ((key_type == bcmVlanTranslateKeyPortInner)
                 && (inner_template_vlan_low == inner_vlan_low) && (inner_template_vlan_high == inner_vlan_high))
        {
            action->it_inner = bcmVlanActionCompressed;
            action->new_inner_vlan = inner_vlan_low;
            break;
        }
    }
    if (vlan_range_offset == VLAN_RANGES_PER_ENTRY(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Vlan ranges OUTER[%d,%d] INNER[%d,%d]for VLAN Domain %d not exist",
                     outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, vlan_domain);
    }
exit:
    SHR_FUNC_EXIT;
}
