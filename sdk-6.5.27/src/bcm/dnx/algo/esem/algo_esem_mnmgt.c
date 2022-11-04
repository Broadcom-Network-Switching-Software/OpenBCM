/**
 * \file algo_esem_mnmgt.c
 *
 * This file contains APIs required ESEM management
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/esem/algo_esem_mnmgt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/esem_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/** indications that kept in the SWSTATE, NOTE for the SWSTATE size once adding more FLAGS */
#define ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_VPN           SAL_BIT(0)
#define ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_NWK_DOMAIN    SAL_BIT(1)
#define ESEM_FLAGS_ENTRY_CREATED_BY_AC                  SAL_BIT(2)
#define ESEM_FLAGS_ENTRY_ADDED_BY_VXLAN_VPN             SAL_BIT(3)
#define ESEM_FLAGS_ENTRY_ADDED_BY_VXLAN_NWK_DOMAIN      SAL_BIT(4)
#define ESEM_FLAGS_ENTRY_ADDED_BY_AC                    SAL_BIT(5)

extern shr_error_e dbal_entry_handle_get_internal(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_handle_t ** entry_handle);

extern shr_error_e dbal_tables_sizes_get(
    int unit,
    dbal_tables_e table_id,
    int *key_size,
    int *max_pld_size);
/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */
/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**********************************************
 * FUNCTIONS related to ESEM objects   *
 **********************************************/
/*
 * {
 */

/*
 * See header in algo_l3_egr_pointed.h
 */
shr_error_e
dnx_algo_esem_mnmgt_init(
    int unit)
{
    uint8 is_init;
    int esem_max_capacity = dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_esem_sw_managment);
    sw_state_htb_create_info_t htb_sw_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_esem_sw_managment))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "esem_sw_managment should be set to 1 when performning init to related module.");
    }

    sal_memset(&htb_sw_info, 0, sizeof(htb_sw_info));

    if (!sw_state_is_warm_boot(unit))
    {
        /** Initialize flow SW state */
        SHR_IF_ERR_EXIT(esem_db.is_init(unit, &is_init));

        if (!is_init)
        {
            SHR_IF_ERR_EXIT(esem_db.init(unit));
        }
    }

    if (esem_max_capacity)
    {
        htb_sw_info.max_nof_elements = esem_max_capacity;
        SHR_IF_ERR_EXIT(esem_db.esem_key_to_sw_info_htb.create(unit, &htb_sw_info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_esem_entry_bits_by_source(
    int unit,
    algo_esem_entry_source_e source,
    int8 *created_by_source,
    int8 *added_by_source)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (source)
    {
        case DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_VPN:
            (*created_by_source) = ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_VPN;
            (*added_by_source) = ESEM_FLAGS_ENTRY_ADDED_BY_VXLAN_VPN;
            break;

        case DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_NWK_DOMAIN:
            (*created_by_source) = ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_NWK_DOMAIN;
            (*added_by_source) = ESEM_FLAGS_ENTRY_ADDED_BY_VXLAN_NWK_DOMAIN;
            break;

        case DNX_ALGO_ESEM_ENTRY_SOURCE_AC:
            (*created_by_source) = ESEM_FLAGS_ENTRY_CREATED_BY_AC;
            (*added_by_source) = ESEM_FLAGS_ENTRY_ADDED_BY_AC;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "source %d undefined", source);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_esem_entry_sw_source_status_get(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source,
    algo_esem_entry_status_e * entry_status)
{
    dbal_entry_handle_t *hw_entry_handle;
    int8 esem_result = 0;
    esem_key_t esem_key;
    int rv;
    int8 created_by_source = 0, added_by_source = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &hw_entry_handle));
    sal_memcpy(esem_key.match_key, hw_entry_handle->phy_entry.key, sizeof(esem_key.match_key));

    rv = esem_db.esem_key_to_sw_info_htb.find(unit, &esem_key, &esem_result);

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        (*entry_status) = ALGO_ESEM_ENTRY_STATUS_NOT_EXISTS;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_esem_entry_bits_by_source(unit, source, &created_by_source, &added_by_source));

    if (esem_result & created_by_source)
    {
        (*entry_status) = ALGO_ESEM_ENTRY_STATUS_CREATED_BY_SOURCE;
        SHR_EXIT();
    }

    if (esem_result & added_by_source)
    {
        (*entry_status) = ALGO_ESEM_ENTRY_STATUS_ADDED_BY_SOURCE;
        SHR_EXIT();
    }

    (*entry_status) = ALGO_ESEM_ENTRY_STATUS_EXISTS_SOURCE_NOT_MATCH;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_esem_entry_sw_source_create(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source)
{
    dbal_entry_handle_t *hw_entry_handle;
    int8 esem_result = 0;
    esem_key_t esem_key;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &hw_entry_handle));
    sal_memcpy(esem_key.match_key, hw_entry_handle->phy_entry.key, sizeof(esem_key.match_key));

    switch (source)
    {
        case DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_VPN:
            esem_result |= ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_VPN;
            break;

        case DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_NWK_DOMAIN:
            esem_result |= ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_NWK_DOMAIN;
            break;

        case DNX_ALGO_ESEM_ENTRY_SOURCE_AC:
            esem_result |= ESEM_FLAGS_ENTRY_CREATED_BY_AC;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "source %d undefined", source);
    }

    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(esem_db.esem_key_to_sw_info_htb.insert(unit, &esem_key, &esem_result),
                                       _SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_esem_entry_sw_source_add(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source)
{
    dbal_entry_handle_t *hw_entry_handle;
    int8 created_by_source = 0, existing_esem_result = 0;
    esem_key_t esem_key;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &hw_entry_handle));
    sal_memcpy(esem_key.match_key, hw_entry_handle->phy_entry.key, sizeof(esem_key.match_key));

    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(esem_db.esem_key_to_sw_info_htb.find(unit, &esem_key, &existing_esem_result),
                                       _SHR_E_NOT_FOUND);

    switch (source)
    {
        case DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_VPN:
            existing_esem_result |= ESEM_FLAGS_ENTRY_ADDED_BY_VXLAN_VPN;
            created_by_source |= ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_VPN;
            break;

        case DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_NWK_DOMAIN:
            existing_esem_result |= ESEM_FLAGS_ENTRY_ADDED_BY_VXLAN_NWK_DOMAIN;
            created_by_source |= ESEM_FLAGS_ENTRY_CREATED_BY_VXLAN_NWK_DOMAIN;
            break;

        case DNX_ALGO_ESEM_ENTRY_SOURCE_AC:
            existing_esem_result |= ESEM_FLAGS_ENTRY_ADDED_BY_AC;
            created_by_source |= ESEM_FLAGS_ENTRY_CREATED_BY_AC;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "source %d undefined", source);
    }

    if (existing_esem_result & created_by_source)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "cannot add source %d to entry that already created by it", source);
    }

    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(esem_db.esem_key_to_sw_info_htb.replace(unit, &esem_key, &existing_esem_result),
                                       _SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_esem_entry_sw_source_delete(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source)
{
    dbal_entry_handle_t *hw_entry_handle;
    int8 esem_result = 0;
    esem_key_t esem_key;
    int8 created_by_source = 0, added_by_source = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &hw_entry_handle));
    sal_memcpy(esem_key.match_key, hw_entry_handle->phy_entry.key, sizeof(esem_key.match_key));

    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(esem_db.esem_key_to_sw_info_htb.find(unit, &esem_key, &esem_result),
                                       _SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(dnx_esem_entry_bits_by_source(unit, source, &created_by_source, &added_by_source));

    if (esem_result & created_by_source)
    {
        if ((esem_result & ~created_by_source) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "cannot delete entry since other sources %d still related to it", esem_result);
        }
        SHR_IF_ERR_EXIT(esem_db.esem_key_to_sw_info_htb.delete(unit, &esem_key));
    }

    if (esem_result & added_by_source)
    {
        int8 new_esem_result = esem_result & ~added_by_source;
        SHR_IF_ERR_EXIT(esem_db.esem_key_to_sw_info_htb.replace(unit, &esem_key, &new_esem_result));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * }
 */
