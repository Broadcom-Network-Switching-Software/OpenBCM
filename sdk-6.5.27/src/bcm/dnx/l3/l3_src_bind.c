/** \file l3_src_bind.c
 *
 *  l3 source bind for anti-spoofing on DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3
/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/vswitch/vswitch.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/l3/l3_src_bind.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */
static shr_error_e
dnx_l3_src_bind_ipv4_entry_init(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(info, 0x0, sizeof(L3_SRC_BIND_IPV4_ENTRY));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_bind_ipv6_entry_init(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(info, 0x0, sizeof(L3_SRC_BIND_IPV6_ENTRY));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_bind_ipv4_get(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * src_bind_info,
    uint32 mac_compress_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_bind_info, _SHR_E_PARAM, "src_bind_info");

    if (src_bind_info->smac_valid)
    {
        table_id = DBAL_TABLE_IPV4_DHCP_ANTI_IP_ADDRESS_SPOOFING;
    }
    else
    {
        table_id = DBAL_TABLE_IPV4_STATIC_ANTI_IP_ADDRESS_SPOOFING;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_SAV_ID, src_bind_info->in_lif_sav);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4, src_bind_info->sip,
                                      src_bind_info->sip_mask);

    if (src_bind_info->smac_valid)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, mac_compress_id);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_bind_ipv4_add(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * src_bind_info,
    uint32 mac_compress_id)
{
    uint32 rv = BCM_E_NONE;
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_bind_info, _SHR_E_PARAM, "src_bind_info");

    rv = dnx_l3_src_bind_ipv4_get(unit, src_bind_info, mac_compress_id);
    if (rv == BCM_E_NOT_FOUND)
    {
        if (src_bind_info->smac_valid)
        {
            table_id = DBAL_TABLE_IPV4_DHCP_ANTI_IP_ADDRESS_SPOOFING;
        }
        else
        {
            table_id = DBAL_TABLE_IPV4_STATIC_ANTI_IP_ADDRESS_SPOOFING;
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_SAV_ID, src_bind_info->in_lif_sav);

        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4, src_bind_info->sip,
                                          src_bind_info->sip_mask);

        if (src_bind_info->smac_valid)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, mac_compress_id);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, 0);
        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    else if (rv == BCM_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "the entry has been exists!\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_bind_ipv4_delete(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * src_bind_info,
    uint32 mac_compress_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_bind_info, _SHR_E_PARAM, "src_bind_info");
    SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_get(unit, src_bind_info, mac_compress_id));
    if (src_bind_info->smac_valid)
    {
        table_id = DBAL_TABLE_IPV4_DHCP_ANTI_IP_ADDRESS_SPOOFING;
    }
    else
    {
        table_id = DBAL_TABLE_IPV4_STATIC_ANTI_IP_ADDRESS_SPOOFING;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_SAV_ID, src_bind_info->in_lif_sav);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4, src_bind_info->sip,
                                      src_bind_info->sip_mask);

    if (src_bind_info->smac_valid)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, mac_compress_id);
    }
    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_bind_ipv4_delete_all(
    int unit,
    int is_dhcp)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_dhcp)
    {
        table_id = DBAL_TABLE_IPV4_DHCP_ANTI_IP_ADDRESS_SPOOFING;
    }
    else
    {
        table_id = DBAL_TABLE_IPV4_STATIC_ANTI_IP_ADDRESS_SPOOFING;
    }
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_bind_ipv6_get(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * src_bind_info,
    uint32 mac_compress_id,
    uint32 ip6_msb32_compress_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (src_bind_info->smac_valid)
    {
        table_id = DBAL_TABLE_IPV6_DHCP_ANTI_IP_ADDRESS_SPOOFING;
    }
    else
    {
        table_id = DBAL_TABLE_IPV6_STATIC_ANTI_IP_ADDRESS_SPOOFING;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_SAV_ID, src_bind_info->in_lif_sav);

    if (src_bind_info->smac_valid)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, mac_compress_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID, ip6_msb32_compress_id);
    }

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, src_bind_info->sip6,
                                         src_bind_info->sip6_mask);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_bind_ipv6_add(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * src_bind_info,
    uint32 mac_compress_id,
    uint32 ip6_msb32_compress_id)
{
    uint32 rv = BCM_E_NONE;
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_bind_info, _SHR_E_PARAM, "src_bind_info");
    rv = dnx_l3_src_bind_ipv6_get(unit, src_bind_info, mac_compress_id, ip6_msb32_compress_id);
    if (rv == BCM_E_NOT_FOUND)
    {
        if (src_bind_info->smac_valid)
        {
            table_id = DBAL_TABLE_IPV6_DHCP_ANTI_IP_ADDRESS_SPOOFING;
        }
        else
        {
            table_id = DBAL_TABLE_IPV6_STATIC_ANTI_IP_ADDRESS_SPOOFING;
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_SAV_ID, src_bind_info->in_lif_sav);

        if (src_bind_info->smac_valid)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, mac_compress_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID,
                                       ip6_msb32_compress_id);
        }

        dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, src_bind_info->sip6,
                                             src_bind_info->sip6_mask);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, 0);
        /** Write to HW */
        rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    }
    else if (rv == BCM_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "the entry has been exists!\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_bind_ipv6_delete(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * src_bind_info,
    uint32 mac_compress_id,
    uint32 ip6_msb32_compress_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_bind_info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv6_get(unit, src_bind_info, mac_compress_id, ip6_msb32_compress_id));

    if (src_bind_info->smac_valid)
    {
        table_id = DBAL_TABLE_IPV6_DHCP_ANTI_IP_ADDRESS_SPOOFING;
    }
    else
    {
        table_id = DBAL_TABLE_IPV6_STATIC_ANTI_IP_ADDRESS_SPOOFING;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_SAV_ID, src_bind_info->in_lif_sav);

    if (src_bind_info->smac_valid)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, mac_compress_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID, ip6_msb32_compress_id);
    }
    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, src_bind_info->sip6,
                                         src_bind_info->sip6_mask);
    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_bind_ipv6_delete_all(
    int unit,
    int is_dhcp)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_dhcp)
    {
        table_id = DBAL_TABLE_IPV6_DHCP_ANTI_IP_ADDRESS_SPOOFING;
    }
    else
    {
        table_id = DBAL_TABLE_IPV6_STATIC_ANTI_IP_ADDRESS_SPOOFING;
    }
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_mac_compression_get(
    int unit,
    bcm_mac_t mac_addr,
    uint32 *mac_compress_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_id = DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_addr);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, INST_SINGLE, mac_compress_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_mac_compression_add(
    int unit,
    bcm_mac_t mac_addr,
    uint32 *mac_compress_id)
{
    uint32 rv = BCM_E_NONE;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    uint32 new_mac_compress_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mac_compress_id, _SHR_E_PARAM, "mac_compress_id");

    rv = dnx_l3_src_mac_compression_get(unit, mac_addr, &new_mac_compress_id);
    if (rv == BCM_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_id_allocate(unit, &new_mac_compress_id));

        table_id = DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_addr);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, INST_SINGLE,
                                     new_mac_compress_id);

        /** Write to HW */
        rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
        if (rv != BCM_E_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_id_free(unit, new_mac_compress_id));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    *mac_compress_id = new_mac_compress_id;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_mac_compression_delete(
    int unit,
    bcm_mac_t mac_addr)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    uint32 mac_compress_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_src_mac_compression_get(unit, mac_addr, &mac_compress_id));
    table_id = DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_addr);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_mac_compression_clear(
    int unit)
{
    uint32 entry_handle_id;
    bcm_mac_t mac_addr = { 0 };
    uint32 mac_compress_id = 0;
    uint32 core_id = DBAL_CORE_DEFAULT;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Traverse Ingress non-native.
     *  Note: symmetric LIFs will be skipped based on "is_symmetric" indication
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CONDITION_EQUAL_TO, &core_id, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {

        dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_addr);
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_COMPRESSED_MAC_ID, INST_SINGLE,
                                            &mac_compress_id);

        /*
         * for DHCP, decrease the reference count of the mac_compress_id * set reference count to ZERO, delete the
         * entry 
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_ref_clear(unit, mac_compress_id));
        SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_id_free(unit, mac_compress_id));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_MAC_ADDRESS_COMPRESSION_DB));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_ip6_msb32_compression_get(
    int unit,
    bcm_ip6_t ip6_addr,
    uint32 *ip6_msb32_compress_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    uint8 sip6_MSB32[4];
    int i = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ip6_msb32_compress_id, _SHR_E_PARAM, "ip6_msb32_compress_id");
    for (i = 0; i < 4; i++)
    {
        sip6_MSB32[i] = ip6_addr[3 - i];
    }

    table_id = DBAL_TABLE_IPV6_32MSBS_COMPRESSION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_127_TO_96, sip6_MSB32);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID, INST_SINGLE, ip6_msb32_compress_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_ip6_msb32_compression_add(
    int unit,
    bcm_ip6_t ip6_addr,
    uint32 *ip6_msb32_compress_id)
{
    uint32 rv = BCM_E_NONE;
    uint32 entry_handle_id;
    int i = 0;
    dbal_tables_e table_id;
    uint8 sip6_MSB32[4];
    uint32 new_ip6_compress_id;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ip6_msb32_compress_id, _SHR_E_PARAM, "ip6_msb32_compress_id");
    rv = dnx_l3_src_ip6_msb32_compression_get(unit, ip6_addr, &new_ip6_compress_id);
    if (rv == BCM_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_id_allocate(unit, &new_ip6_compress_id));

        for (i = 0; i < 4; i++)
        {
            sip6_MSB32[i] = ip6_addr[3 - i];
        }

        table_id = DBAL_TABLE_IPV6_32MSBS_COMPRESSION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_127_TO_96, sip6_MSB32);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID, INST_SINGLE,
                                     new_ip6_compress_id);

        /** Write to HW */
        rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
        if (rv != BCM_E_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_id_free(unit, new_ip6_compress_id));
        }

    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    *ip6_msb32_compress_id = new_ip6_compress_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_ip6_msb32_compression_delete(
    int unit,
    bcm_ip6_t ip6_addr)
{
    uint32 entry_handle_id;
    int i = 0;
    dbal_tables_e table_id;
    uint8 sip6_MSB32[4];
    uint32 ip6_msb32_compress_id;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_src_ip6_msb32_compression_get(unit, ip6_addr, &ip6_msb32_compress_id));

    for (i = 0; i < 4; i++)
    {
        sip6_MSB32[i] = ip6_addr[3 - i];
    }

    table_id = DBAL_TABLE_IPV6_32MSBS_COMPRESSION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_127_TO_96, sip6_MSB32);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_src_ip6_msb32_compression_clear(
    int unit)
{
    uint32 entry_handle_id;
    uint32 core_id = DBAL_CORE_DEFAULT;
    int is_end;
    uint32 ip6_msb32_compress_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Traverse Ingress non-native.
     *  Note: symmetric LIFs will be skipped based on "is_symmetric" indication
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CONDITION_EQUAL_TO, &core_id, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV6_32MSBS_COMPRESSION_ID, INST_SINGLE,
                                            &ip6_msb32_compress_id);
        /*
         * for DHCP, decrease the reference count of the ip6_msb32_compress_id * set reference count to ZERO, delete
         * the entry 
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_ref_clear(unit, ip6_msb32_compress_id));
        SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_id_free(unit, ip6_msb32_compress_id));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_IPV6_32MSBS_COMPRESSION));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_in_lif_profile_get(
    int unit,
    bcm_gport_t port,
    dbal_tables_e * inlif_dbal_table_id,
    uint32 *in_lif_sav,
    int *in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info)
{
    uint32 entry_handle_id;
    uint32 my_in_lif_profile;
    in_lif_profile_info_t old_in_lif_profile_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    lif_table_mngr_inlif_info_t lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Fetch the in_lif_profile by gport:
     */
    /*
     * Handle Ingress Virtual Native Vlan port
     */
    /*
     * get Local In-LIF:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, &gport_hw_resources));

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    if ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB) ||
        (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_PON_DTC_CLASSIFICATION))
    {
        /*
         * Get value:
         * - ingress_network_group_id - via in_lif_profile
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &lif_info));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &my_in_lif_profile));
        SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &old_in_lif_profile_info));
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                        (unit, my_in_lif_profile, &old_in_lif_profile_info, gport_hw_resources.inlif_dbal_table_id,
                         LIF));
        *in_lif_sav = gport_hw_resources.global_in_lif;
        *in_lif_profile = my_in_lif_profile;
        sal_memcpy(in_lif_profile_info, &old_in_lif_profile_info, sizeof(in_lif_profile_info_t));
    }
    else
    {
        /** we only support Virtual Ingress native / Non-Native gport*/
        SHR_ERR_EXIT(BCM_E_PARAM, "Invalid gport = %d !\n", port);
    }

    *inlif_dbal_table_id = gport_hw_resources.inlif_dbal_table_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_src_in_lif_profile_set(
    int unit,
    bcm_gport_t port,
    dbal_tables_e inlif_dbal_table_id,
    int in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info)
{
    uint32 entry_handle_id;
    in_lif_profile_info_t new_in_lif_profile_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    lif_table_mngr_inlif_info_t lif_info;
    int is_p2p = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    sal_memcpy(&new_in_lif_profile_info, in_lif_profile_info, sizeof(new_in_lif_profile_info));

    /*
     * Handle Ingress Virtual Native Vlan port
     */
    /*
     * get Local In-LIF:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, &gport_hw_resources));

    if (_SHR_GPORT_IS_VLAN_PORT(port) || _SHR_GPORT_IS_EXTENDER_PORT(port))
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_is_p2p(unit, port, &is_p2p));
    }

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, inlif_dbal_table_id, &entry_handle_id));

    if ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB) ||
        (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_PON_DTC_CLASSIFICATION))
    {
        /*
         * Get value:
         * - ingress_network_group_id - via in_lif_profile
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &lif_info));

        if (is_p2p)
        {
            lif_info.table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P;
        }

        lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
        lif_info.global_lif = gport_hw_resources.global_in_lif;

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile);
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                        (unit, _SHR_CORE_ALL, entry_handle_id, &gport_hw_resources.local_in_lif, &lif_info));
    }
    else
    {
        /** we only support Virtual Ingress native / Non-Native gport*/
        SHR_ERR_EXIT(BCM_E_PARAM, "Invalid gport = %d !\n", port);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_source_bind_verify(
    int unit,
    bcm_l3_source_bind_t * info)
{

    bcm_ip6_t ip6_ZERO = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    if (((unit) < 0) || ((unit) >= BCM_LOCAL_UNITS_MAX))
    {
        SHR_ERR_EXIT(BCM_E_UNIT, "Invalid unit %d\n", unit);
    }

    if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 1)
    {
        if (BCM_IP6_ADDR_EQ(info->ip6, ip6_ZERO))
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "IPv6 address should not be all ZERO for IPV6 anti-spoofing check!\n");
        }
    }
    else
    {
        if (info->ip == 0)
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "IPv4 address should not be ZERO for IPV4 anti-spoofing check!\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_l3_source_bind_add
 * Purpose:
 *      Add or replace an L3 source binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_dnx_l3_source_bind_add(
    int unit,
    bcm_l3_source_bind_t * info)
{
    int rv = BCM_E_NONE;
    L3_SRC_BIND_IPV4_ENTRY ipv4_bind_info;
    L3_SRC_BIND_IPV6_ENTRY ipv6_bind_info;
    uint32 in_lif_sav;
    int in_lif_profile;
    in_lif_profile_info_t in_lif_profile_info;
    dbal_tables_e inlif_dbal_table_id;
    uint32 mac_compress_id = 0;
    uint32 ip6_msb32_compress_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_l3_source_bind_verify(unit, info));

    SHR_IF_ERR_EXIT(dnx_l3_src_in_lif_profile_get
                    (unit, info->port, &inlif_dbal_table_id, &in_lif_sav, &in_lif_profile, &in_lif_profile_info));

    if (!BCM_MAC_IS_ZERO(info->mac))
    {
        SHR_IF_ERR_EXIT(dnx_l3_src_mac_compression_add(unit, info->mac, &mac_compress_id));
    }

    /*
     * if info->flags & BCM_L3_SOURCE_BIND_IP6, set ipv6 sav entry; * or set ipv4 sav entry 
     */
    if (info->flags & BCM_L3_SOURCE_BIND_IP6)
    {
        rv = dnx_l3_src_bind_ipv6_entry_init(unit, &ipv6_bind_info);
        sal_memcpy(ipv6_bind_info.sip6, info->ip6, sizeof(bcm_ip6_t));
        sal_memcpy(ipv6_bind_info.sip6_mask, info->ip6_mask, sizeof(bcm_ip6_t));
        if (BCM_MAC_IS_ZERO(info->mac))
        {
            ipv6_bind_info.smac_valid = 0;
        }
        else
        {
            ipv6_bind_info.smac_valid = 1;
            sal_memcpy(ipv6_bind_info.smac, info->mac, sizeof(bcm_mac_t));
            SHR_IF_ERR_EXIT(dnx_l3_src_ip6_msb32_compression_add(unit, ipv6_bind_info.sip6, &ip6_msb32_compress_id));
        }

        ipv6_bind_info.in_lif_sav = in_lif_sav;
        rv = dnx_l3_src_bind_ipv6_add(unit, &ipv6_bind_info, mac_compress_id, ip6_msb32_compress_id);

        if (ipv6_bind_info.smac_valid)
        {
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
            {
                dnx_l3_src_ip6_msb32_compression_delete(unit, ipv6_bind_info.sip6);
                dnx_l3_src_mac_compression_delete(unit, ipv6_bind_info.smac);
            }
            else if (rv == BCM_E_NONE)
            {
                /*
                 * reference count ++
                 */
                SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_update_ref_count(unit, mac_compress_id, 1));
                SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_update_ref_count(unit, ip6_msb32_compress_id, 1));
            }
        }
        SHR_IF_ERR_EXIT(rv);

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_entry_init(unit, &ipv4_bind_info));

        if (BCM_MAC_IS_ZERO(info->mac))
        {
            ipv4_bind_info.smac_valid = 0;
        }
        else
        {
            ipv4_bind_info.smac_valid = 1;
            sal_memcpy(ipv4_bind_info.smac, info->mac, sizeof(bcm_mac_t));
        }
        /*
         * in_lif_sav = global_in_lif
         */
        ipv4_bind_info.in_lif_sav = in_lif_sav;
        ipv4_bind_info.sip = info->ip;
        ipv4_bind_info.sip_mask = info->ip_mask;
        rv = dnx_l3_src_bind_ipv4_add(unit, &ipv4_bind_info, mac_compress_id);
        if (ipv4_bind_info.smac_valid)
        {
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
            {
                dnx_l3_src_mac_compression_delete(unit, ipv4_bind_info.smac);
            }
            else if (rv == BCM_E_NONE)
            {
                /*
                 * reference count ++
                 */
                SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_update_ref_count(unit, mac_compress_id, 1));
            }
        }
        SHR_IF_ERR_EXIT(rv);

    }
exit:
    SHR_FUNC_EXIT;

}
/*
 * Function:
 *      bcm_dnx_l3_source_bind_delete
 * Purpose:
 *      Add or replace an L3 source binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_dnx_l3_source_bind_delete(
    int unit,
    bcm_l3_source_bind_t * info)
{
    L3_SRC_BIND_IPV4_ENTRY ipv4_bind_info;
    L3_SRC_BIND_IPV6_ENTRY ipv6_bind_info;
    uint32 in_lif_sav;
    int in_lif_profile;
    in_lif_profile_info_t in_lif_profile_info;
    dbal_tables_e inlif_dbal_table_id;
    uint32 mac_compress_id = 0;
    uint32 ip6_msb32_compress_id = 0;
    uint32 nof_ref_ip6_sip = 0;
    uint32 nof_ref_mac = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_IF_ERR_EXIT(dnx_l3_source_bind_verify(unit, info));

    SHR_IF_ERR_EXIT(dnx_l3_src_in_lif_profile_get
                    (unit, info->port, &inlif_dbal_table_id, &in_lif_sav, &in_lif_profile, &in_lif_profile_info));

    if (!BCM_MAC_IS_ZERO(info->mac))
    {
        SHR_IF_ERR_EXIT(dnx_l3_src_mac_compression_get(unit, info->mac, &mac_compress_id));
    }

    /*
     * if info->flags & BCM_L3_SOURCE_BIND_IP6, set ipv6 sav entry; * or set ipv4 sav entry 
     */
    if (info->flags & BCM_L3_SOURCE_BIND_IP6)
    {
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv6_entry_init(unit, &ipv6_bind_info));
        sal_memcpy(ipv6_bind_info.sip6, info->ip6, sizeof(bcm_ip6_t));
        sal_memcpy(ipv6_bind_info.sip6_mask, info->ip6_mask, sizeof(bcm_ip6_t));
        if (BCM_MAC_IS_ZERO(info->mac))
        {
            ipv6_bind_info.smac_valid = 0;
        }
        else
        {
            ipv6_bind_info.smac_valid = 1;
            sal_memcpy(ipv6_bind_info.smac, info->mac, sizeof(bcm_mac_t));
            SHR_IF_ERR_EXIT(dnx_l3_src_ip6_msb32_compression_get(unit, ipv6_bind_info.sip6, &ip6_msb32_compress_id));
        }
        ipv6_bind_info.in_lif_sav = in_lif_sav;
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv6_delete(unit, &ipv6_bind_info, mac_compress_id, ip6_msb32_compress_id));

        if (ipv6_bind_info.smac_valid == 1)
        {
            /*
             * for DHCP, decrease the reference count of the ipv6 sip msb32 compress ID * if reference count==1, delete 
             * the entry 
             */
            SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_ref_get(unit, ip6_msb32_compress_id, &nof_ref_ip6_sip));
            if (nof_ref_ip6_sip >= 1)
            {
                SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_update_ref_count(unit, ip6_msb32_compress_id, 0));
            }
            if (nof_ref_ip6_sip == 1)
            {
                SHR_IF_ERR_EXIT(dnx_l3_src_ip6_msb32_compression_delete(unit, ipv6_bind_info.sip6));
                SHR_IF_ERR_EXIT(dnx_algo_l3_ip6_msb32_compress_id_free(unit, ip6_msb32_compress_id));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_entry_init(unit, &ipv4_bind_info));
        if (BCM_MAC_IS_ZERO(info->mac))
        {
            ipv4_bind_info.smac_valid = 0;
        }
        else
        {
            ipv4_bind_info.smac_valid = 1;
            sal_memcpy(ipv4_bind_info.smac, info->mac, sizeof(bcm_mac_t));
        }
        ipv4_bind_info.in_lif_sav = in_lif_sav;
        ipv4_bind_info.sip = info->ip;
        ipv4_bind_info.sip_mask = info->ip_mask;
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_delete(unit, &ipv4_bind_info, mac_compress_id));
    }

    /*
     * for DHCP, decrease the reference count of the mac compress ID * if reference count==1, delete the entry 
     */

    if (!BCM_MAC_IS_ZERO(info->mac))
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_ref_get(unit, mac_compress_id, &nof_ref_mac));
        if (nof_ref_mac >= 1)
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_update_ref_count(unit, mac_compress_id, 0));
        }
        if (nof_ref_mac == 1)
        {
            SHR_IF_ERR_EXIT(dnx_l3_src_mac_compression_delete(unit, info->mac));
            SHR_IF_ERR_EXIT(dnx_algo_l3_mac_compress_id_free(unit, mac_compress_id));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * Function:
 *      bcm_dnx_l3_source_bind_enable_set
 * Purpose:
 *      Get the l3 SAV check mode(enable or disable) on a gport.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) L3 source binding information
 *      enable - (IN) 1:enable L3 SAV; 0:disable L3 SAV
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

int
bcm_dnx_l3_source_bind_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    uint32 in_lif_sav;
    int in_lif_profile;
    int new_in_lif_profile;
    dbal_tables_e inlif_dbal_table_id;
    in_lif_profile_info_t in_lif_profile_info;
    dnx_in_lif_profile_last_info_t last_profile;
    int old_enable = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    SHR_IF_ERR_EXIT(dnx_l3_src_in_lif_profile_get
                    (unit, port, &inlif_dbal_table_id, &in_lif_sav, &in_lif_profile, &in_lif_profile_info));

    old_enable = in_lif_profile_info.ingress_fields.ip_anti_spoofing_check_mode;
    if (old_enable == enable)
    {
        SHR_ERR_EXIT(BCM_E_NONE, "Not enable/disable ip_anti_spoofing for port %d again!\n", port);
    }
    else
    {
        in_lif_profile_info.ingress_fields.ip_anti_spoofing_check_mode = enable;
    }

    /*
     * Exchange in_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, in_lif_profile, &new_in_lif_profile, LIF, inlif_dbal_table_id,
                     &last_profile));

    SHR_IF_ERR_EXIT(dnx_l3_src_in_lif_profile_set
                    (unit, port, inlif_dbal_table_id, new_in_lif_profile, &in_lif_profile_info));
exit:
    SHR_FUNC_EXIT;

}

/*
 * Function:
 *      bcm_dnx_l3_source_bind_enable_get
 * Purpose:
 *      Get the l3 SAV check mode(enable or disable) on a gport.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) L3 source binding information
 *      enable - (OUT) 1:enable L3 SAV; 0:disable L3 SAV
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

int
bcm_dnx_l3_source_bind_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    uint32 in_lif_sav;
    int in_lif_profile;
    dbal_tables_e inlif_dbal_table_id;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_l3_src_in_lif_profile_get
                    (unit, port, &inlif_dbal_table_id, &in_lif_sav, &in_lif_profile, &in_lif_profile_info));

    *enable = in_lif_profile_info.ingress_fields.ip_anti_spoofing_check_mode;

exit:
    SHR_FUNC_EXIT;

}
/*
 * Function:
 *      bcm_dnx_l3_source_bind_delete_all
 * Purpose:
 *      delete all L3 source binding entries
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

int
bcm_dnx_l3_source_bind_delete_all(
    int unit)
{
    int is_dhcp = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_delete_all(unit, is_dhcp));
    SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv6_delete_all(unit, is_dhcp));
    is_dhcp = 1;
    SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_delete_all(unit, is_dhcp));
    SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv6_delete_all(unit, is_dhcp));

    SHR_IF_ERR_EXIT(dnx_l3_src_ip6_msb32_compression_clear(unit));
    SHR_IF_ERR_EXIT(dnx_l3_src_mac_compression_clear(unit));
exit:
    SHR_FUNC_EXIT;
}
/*
 * Function:
 *      bcm_dnx_l3_source_bind_get
 * Purpose:
 *      Retrieve the details of an existing L3 source binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN/OUT) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */

int
bcm_dnx_l3_source_bind_get(
    int unit,
    bcm_l3_source_bind_t * info)
{
    L3_SRC_BIND_IPV4_ENTRY ipv4_bind_info;
    L3_SRC_BIND_IPV6_ENTRY ipv6_bind_info;
    uint32 in_lif_sav;
    int in_lif_profile;
    in_lif_profile_info_t in_lif_profile_info;
    dbal_tables_e inlif_dbal_table_id;
    uint32 mac_compress_id;
    uint32 ip6_msb32_compress_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_l3_source_bind_verify(unit, info));
    SHR_IF_ERR_EXIT(dnx_l3_src_in_lif_profile_get
                    (unit, info->port, &inlif_dbal_table_id, &in_lif_sav, &in_lif_profile, &in_lif_profile_info));

    if (info->flags & BCM_L3_SOURCE_BIND_IP6)
    {
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv6_entry_init(unit, &ipv6_bind_info));
        sal_memcpy(ipv6_bind_info.sip6, info->ip6, sizeof(bcm_ip6_t));
        sal_memcpy(ipv6_bind_info.sip6_mask, info->ip6_mask, sizeof(bcm_ip6_t));

        if (BCM_MAC_IS_ZERO(info->mac))
        {
            ipv6_bind_info.smac_valid = 0;
        }
        else
        {
            ipv6_bind_info.smac_valid = 1;
            sal_memcpy(ipv6_bind_info.smac, info->mac, sizeof(bcm_mac_t));
            SHR_IF_ERR_EXIT(dnx_l3_src_mac_compression_get(unit, ipv6_bind_info.smac, &mac_compress_id));
            SHR_IF_ERR_EXIT(dnx_l3_src_ip6_msb32_compression_get(unit, ipv6_bind_info.sip6, &ip6_msb32_compress_id));
        }
        ipv6_bind_info.in_lif_sav = in_lif_sav;

        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv6_get(unit, &ipv6_bind_info, mac_compress_id, ip6_msb32_compress_id));

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_entry_init(unit, &ipv4_bind_info));

        if (BCM_MAC_IS_ZERO(info->mac))
        {
            ipv4_bind_info.smac_valid = 0;
        }
        else
        {
            ipv4_bind_info.smac_valid = 1;
            sal_memcpy(ipv4_bind_info.smac, info->mac, sizeof(bcm_mac_t));
            SHR_IF_ERR_EXIT(dnx_l3_src_mac_compression_get(unit, ipv4_bind_info.smac, &mac_compress_id));
        }
        /*
         * in_lif_sav = global_in_lif
         */
        ipv4_bind_info.in_lif_sav = in_lif_sav;
        ipv4_bind_info.sip = info->ip;
        ipv4_bind_info.sip_mask = info->ip_mask;
        SHR_IF_ERR_EXIT(dnx_l3_src_bind_ipv4_get(unit, &ipv4_bind_info, mac_compress_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_l3_source_bind_traverse
 * Purpose:
 *      Traverse through the L3 source bindings and run callback at
 *      each defined binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) Callback function
 *      user_data - (IN) User data to be passed to callback function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */

int
bcm_dnx_l3_source_bind_traverse(
    int unit,
    bcm_l3_source_bind_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_l3_source_bind_traverse is not supported.\n");

exit:
    SHR_FUNC_EXIT;
}
