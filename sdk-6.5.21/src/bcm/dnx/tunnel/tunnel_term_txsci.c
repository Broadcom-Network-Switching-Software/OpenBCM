/** \file tunnel_term_txsci.c
 *  TXSCI APIs used for DNX TUNNEL termination functionality.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include "tunnel_term_txsci.h"
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */
/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * Global and Static
 */

/*
 * }
 */
/*
 * Functions
 * {
 */

/*
 * For more  details see .h file
 */
shr_error_e
dnx_tunnel_terminator_create_txsci_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The bcmTunnelTypeEsp is available for SVTAG supported devices only");
    }

    if (info->sci >= (1 << dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The TXSCI value (%d) must be between 0-%d.", info->sci,
                     (1 << dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit)) - 1);
    }

    if (info->priority != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel terminator entry configured for EM table, but priority is defined\n");
    }

    if (info->udp_dst_port != 0 || info->udp_src_port != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "UDP ports must be set to zero when using the bcmTunnelTypeEsp tunnel type \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
dnx_tunnel_terminator_lookup_add_txsci(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPVX_TT_IPSEC, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_TXSCI, info->sci);

    /** Set DATA field - {in LIF} */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
dnx_tunnel_terminator_lookup_get_txsci(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPVX_TT_IPSEC, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_TXSCI, info->sci);

    /** Set pointer to receive field - {in LIF} */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

    /** Read from DBAL - check the returned value in calling function */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
dnx_tunnel_terminator_lookup_delete_txsci(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** take handle to delete from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPVX_TT_IPSEC, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_TXSCI, info->sci);
    /** Delete entry from DBAL table*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
