/** \file vswitch_vpn.c
 *  General VSI functionality for VPN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm/vlan.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/vsi/vswitch_vpn.h>
/*
 * }
 */

/**
 * \brief - convert from VSI type to DBAL field.
 */
static shr_error_e
dnx_vsi_type_to_vsi_field(
    int unit,
    dnx_vsi_type_e type,
    dbal_fields_e * dbal_fld)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_fld, _SHR_E_PARAM, "dbal_fld");

    if (type == _bcmDnxVsiTypeMpls)
    {
        *dbal_fld = DBAL_FIELD_VSI_TYPE_MPLS;
    }
    else if (type == _bcmDnxVsiTypeVswitch)
    {
        *dbal_fld = DBAL_FIELD_VSI_TYPE_VSWITCH;
    }
    else if (type == _bcmDnxVsiTypeMim)
    {
        *dbal_fld = DBAL_FIELD_VSI_TYPE_MIM;
    }
    else if (type == _bcmDnxVsiTypeL2gre)
    {
        *dbal_fld = DBAL_FIELD_VSI_TYPE_L2GRE;
    }
    else if (type == _bcmDnxVsiTypeVxlan)
    {
        *dbal_fld = DBAL_FIELD_VSI_TYPE_VxLAN;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VSI type can't support to get DBAL field\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See vswitch_vpn.h
 */

shr_error_e
dnx_vswitch_vsi_usage_per_type_set(
    int unit,
    int vsi,
    dnx_vsi_type_e type,
    uint8 value)
{
    uint32 entry_handle_id;
    dbal_fields_e field;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    SHR_MAX_VERIFY(vsi, (nof_vsis - 1), _SHR_E_PARAM, "vsi out of range!\n");

    SHR_IF_ERR_EXIT(dnx_vsi_type_to_vsi_field(unit, type, &field));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_TYPE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_value_field8_set(unit, entry_handle_id, field, INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See vswitch_vpn.h
 */
shr_error_e
dnx_vswitch_vsi_usage_per_type_get(
    int unit,
    int vsi,
    dnx_vsi_type_e type,
    uint8 *is_used)
{
    uint32 entry_handle_id;
    dbal_fields_e field;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_used, _SHR_E_PARAM, "is_used");
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    SHR_MAX_VERIFY(vsi, (nof_vsis - 1), _SHR_E_PARAM, "vsi out of range!\n");

    SHR_IF_ERR_EXIT(dnx_vsi_type_to_vsi_field(unit, type, &field));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_TYPE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, field, INST_SINGLE, is_used));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See vswitch_vpn.h
 */
shr_error_e
dnx_vswitch_vsi_usage_check_except_type(
    int unit,
    int vsi,
    dnx_vsi_type_e type,
    uint8 *used)
{
    uint32 entry_handle_id;
    uint8 is_vsi_used, field_value;
    dbal_fields_e field;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(used, _SHR_E_PARAM, "used");
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    SHR_MAX_VERIFY(vsi, (nof_vsis - 1), _SHR_E_PARAM, "vsi out of range!\n");

    if (type != _bcmDnxVsiTypeNone)
    {
        SHR_IF_ERR_EXIT(dnx_vsi_type_to_vsi_field(unit, type, &field));
    }

    is_vsi_used = 0;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_TYPE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if ((type == _bcmDnxVsiTypeNone) || (field != DBAL_FIELD_VSI_TYPE_VSWITCH))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_TYPE_VSWITCH, INST_SINGLE, &field_value));
        is_vsi_used = field_value ? 1 : 0;
    }

    if (!is_vsi_used && ((type == _bcmDnxVsiTypeNone) || (field != DBAL_FIELD_VSI_TYPE_VxLAN)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_TYPE_VxLAN, INST_SINGLE, &field_value));
        is_vsi_used = field_value ? 1 : 0;
    }

    if (!is_vsi_used && ((type == _bcmDnxVsiTypeNone) || (field != DBAL_FIELD_VSI_TYPE_L2GRE)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_TYPE_L2GRE, INST_SINGLE, &field_value));
        is_vsi_used = field_value ? 1 : 0;
    }

    if (!is_vsi_used && ((type == _bcmDnxVsiTypeNone) || (field != DBAL_FIELD_VSI_TYPE_MIM)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_TYPE_MIM, INST_SINGLE, &field_value));
        is_vsi_used = field_value ? 1 : 0;
    }

    if (!is_vsi_used && ((type == _bcmDnxVsiTypeNone) || (field != DBAL_FIELD_VSI_TYPE_MPLS)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_TYPE_MPLS, INST_SINGLE, &field_value));
        is_vsi_used = field_value ? 1 : 0;
    }

    *used = is_vsi_used;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See vswitch_vpn.h
 */
shr_error_e
dnx_vswitch_vsi_usage_alloc(
    int unit,
    uint32 flags,
    dnx_vsi_type_e type,
    int *vsi,
    uint8 *was_replaced)
{

    uint8 is_vsi_allocated;
    uint8 used;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vsi, _SHR_E_PARAM, "vsi");
    SHR_NULL_CHECK(was_replaced, _SHR_E_PARAM, "was_replaced");

    /** allocate new VSI if DNX_ALGO_RES_ALLOCATE_WITH_ID isn't set */
    if (!(flags & DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        SHR_IF_ERR_EXIT(vlan_db.vsi.allocate_single(unit, flags, NULL, (int *) vsi));

        /** set the vsi usage type */
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_set(unit, *vsi, type, VSI_USAGE_VALUE_SET));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    SHR_MAX_VERIFY(*vsi, (nof_vsis - 1), _SHR_E_PARAM, "vsi out of range!\n");

    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, *vsi, &is_vsi_allocated));

    if (!is_vsi_allocated)
    {
        /** allocate VSI if not allocate with flag DNX_ALGO_RES_ALLOCATE_WITH_ID */
        SHR_IF_ERR_EXIT(vlan_db.vsi.allocate_single(unit, flags, NULL, (int *) vsi));

        /** set the vsi usage type */
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_set(unit, *vsi, type, VSI_USAGE_VALUE_SET));
        SHR_EXIT();
    }

    /** now VSI is alread allocated and with ID */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, *vsi, type, &used));
    if (used)
    {
        if (!(flags & DNX_ALGO_RES_ALLOCATE_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS,
                         "allocate VSI with-ID and without replace, where VSI already allocated for this usage\n");
        }
        else
        {
            /** replace when VSI is allocated and set flag DNX_ALGO_RES_ALLOCATE_WITH_ID */
            *was_replaced = 1;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_set(unit, *vsi, type, VSI_USAGE_VALUE_SET));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See vswitch_vpn.h
 */
shr_error_e
dnx_vswitch_vsi_usage_dealloc(
    int unit,
    dnx_vsi_type_e type,
    int vsi)
{
    uint8 is_vsi_allocated;
    uint8 used;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    SHR_MAX_VERIFY(vsi, (nof_vsis - 1), _SHR_E_PARAM, "vsi out of range!\n");

    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, vsi, type, &used));
    if (!used)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VSI is not allocated for this usage!\n");
    }

    /** Clear usage for this type */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_set(unit, vsi, type, VSI_USAGE_VALUE_CLEAR));

    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_vsi_allocated));

    if (is_vsi_allocated)
    {
        /** check if free VSI */
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_check_except_type(unit, vsi, type, &used));
        if (!used)
        {
            SHR_IF_ERR_EXIT(vlan_db.vsi.free_single(unit, vsi));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
