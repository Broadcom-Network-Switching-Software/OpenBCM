/** \file ptp.c
 *
 * Handles the PTP (1588 protocol) functionalities for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PTP

/*
 * Include files currently used for DNX.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/port.h>
#include <bcm/trunk.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/feature.h>

#include <bcm/types.h>
#include <shared/bslenum.h>
#include <bcm_int/ptp_common.h>
#include <bcm_int/dnx/ptp/ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ptp_access.h>
#include <soc/portmod/portmod.h>
#include <phymod/phymod.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

/*
 * }
 */

/*
 * DEFINEs/
 * {
 */
/* action field defines holds the action profile configuration.
 * This is static configuration set by the rx_trap module as follow:
 * trap DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_0 - used by profile 0 for trap
 * trap DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_1 - used by profile 1 for drop the packet
 * trap DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_2 - used by profile 2 for forwaring the packet 
*/
#define DNX_PTP_ACTION_FIELD_ACTION_INDEX_TRAP  (0)
#define DNX_PTP_ACTION_FIELD_ACTION_INDEX_DROP  (1)
#define DNX_PTP_ACTION_FIELD_ACTION_INDEX_FWD   (2)

/* TCAM IDENTIFICATION definitions */
#define DNX_PTP_1588_IDENTIFICATION_CAM_TABLE_ENTRIES_NUM (29)
#define DNX_PTP_1588_IDENTIFICATION_CAM_TABLE_COLUMNS_NUM (10)

#define DNX_PTP_IPV4         DBAL_ENUM_FVAL_LAYER_TYPES_IPV4
#define DNX_PTP_IPV6         DBAL_ENUM_FVAL_LAYER_TYPES_IPV6
#define DNX_PTP_ETH          DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET
#define DNX_PTP_UDP          DBAL_ENUM_FVAL_LAYER_TYPES_UDP
#define DNX_PTP_1588         DBAL_ENUM_FVAL_LAYER_TYPES_PTP
#define DNX_PTP_MPLS_TERM        DBAL_ENUM_FVAL_LAYER_TYPES_MPLS
#define DNX_PTP_MPLS_UNTERM      DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM
#define DNX_PTP_ANY          (0)

/** Get code_index from trap id for Q2A/J2C/J2P, code_index together with 1588 trap code prefix represent trap code */
#define DNX_PTP_TRAP_CODE_OFFSET_GET(trap_code_id) (trap_code_id - DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_0)

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

/**
 * \brief
 *   calculate the mask based on the type, as configured in identification cam table
 *
 * \param [in] unit - The unit number.
 * \param [in] type - layer_record type
 * \param [out] ret_mask - return mask value
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_identification_mask_calc(
    int unit,
    int type,
    uint32 *ret_mask)
{
    uint32 mask;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Note: we take the "type" field instead of the mask, because dbal TCAm do not contain mask field. */
    /** but the size is the same */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_PTP_IEEE_1588_IDENTIFICATION,
                                               DBAL_FIELD_FORWARDING_TYPE_0, TRUE, 0, 0, &field_info));

    switch (type)
    {
        case DNX_PTP_ANY:
            mask = (1 << field_info.field_nof_bits) - 1;
            break;
        case DNX_PTP_MPLS_TERM:
            mask = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS;
            /** allowed only one bit different between the types. otherwise, other type will be included by the mask */
            if (utilex_bitstream_get_nof_on_bits(&mask, 1) > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, " illegal mask. type MPLS mask=0x%X. \n", mask);
            }
            break;
        default:
            mask = 0;
    }

    *ret_mask = mask;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize PTP HW registers/memories
 *
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_hw_init(
    int unit)
{
    int tp_cmd, entry_idx;
    uint32 entry_handle_id;
    uint32 mask;
    /* *INDENT-OFF* */

    /** 1588 supported encapsulation */
    int ieee_1588_identification_cam[DNX_PTP_1588_IDENTIFICATION_CAM_TABLE_ENTRIES_NUM]
                                    [DNX_PTP_1588_IDENTIFICATION_CAM_TABLE_COLUMNS_NUM] =

   /*   input         input         input         input         input         input         input              input                output     output    */ /* example/description */    
   /*   FWD-0         FWD-1         FWD-2         FWD-3         FWD-4         FWD-5         FWD-6              FWD-7        encup      header_idx*/
/*0*/ {{DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, TRUE ,     3        },  /* 1588oEoMPLSoE (MPLS terminated)       */
/*1*/  {DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,      DNX_PTP_ANY ,        TRUE ,     0        },  /* 1588oE (switched)                     */
/*2*/  {DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,      DNX_PTP_ANY ,        TRUE ,     3        },  /* 1588oEoMPLSoE (MPLS routed)           */
/*3*/  {DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,      DNX_PTP_ANY ,        TRUE ,     2        },  /* "my mac"                              */
/*4*/  {DNX_PTP_ANY , DNX_PTP_IPV4, DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,      DNX_PTP_ANY ,        TRUE ,     2        },  /* 1588oEoIPv4oAny                       */
/*5*/  {DNX_PTP_ANY , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , TRUE ,     3        },  /* 1588oEoMPLSoMPLSoE (switched)         */
/*6*/  {DNX_PTP_ANY , DNX_PTP_IPV4, DNX_PTP_IPV4, DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             TRUE ,     3        },  /* 1588oEoIPv4oIPv4oAny                  */
/*7*/  {DNX_PTP_ANY , DNX_PTP_IPV4, DNX_PTP_ETH , DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             TRUE ,     3        },  /* "my mac"                              */
/*8*/  {DNX_PTP_ANY , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_ETH , DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , TRUE ,     4        },  /* "my mac"                              */
/*9*/  {DNX_PTP_ANY , DNX_PTP_IPV4, DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , TRUE ,     4        },  /* 1588oEoMPLSoMPLSoIPv4oAny             */
/*10*/ {DNX_PTP_ANY , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_IPV4, DNX_PTP_ETH , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , TRUE ,     4        },  /* 1588oEoIPv4oMPLSoMPLSoAny             */
/*11*/ {DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, FALSE,     4        },  /* 1588oUDPoIPv4oMPLSoE (MPLS terminated) */
/*12*/ {DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     2        },  /* 1588oUDPoIPv4oE (routed) or 1588oUDPoIPv4oIPoE (IP terminated) */
/*13*/ {DNX_PTP_IPV6, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, FALSE,     4        },  /* 1588oUDPoIPv6oMPLSoE (MPLS terminated) */
/*14*/ {DNX_PTP_IPV6, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     2        },  /* 1588oUDPoIPv6oE (routed) or 1588oUDPoIPv6oIPoE (IP terminated) */
/*15*/ {DNX_PTP_ETH , DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, FALSE,     5        },  /* 1588oUDPoIPv4oEoMPLSoE (MPLS terminated) */
/*16*/ {DNX_PTP_ETH , DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     2        },  /* 1588oUDPoIPv4oE (switched) or 1588oUDPoIPv4oIPoE (IP routed) */
/*17*/ {DNX_PTP_MPLS_UNTERM, DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,      FALSE,     4        },  /* 1588oUDPoIPv4oMPLSoE (MPLS routed) */
/*18*/ {DNX_PTP_ETH , DNX_PTP_IPV6, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, FALSE,     5        },  /* 1588oUDPoIPv6oEoMPLSoE (MPLS terminated) */
/*19*/ {DNX_PTP_ETH , DNX_PTP_IPV6, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     2        },  /* 1588oUDPoIPv6oE (switched) or 1588oUDPoIPv6oIPoE (IP routed) */
/*20*/ {DNX_PTP_MPLS_UNTERM, DNX_PTP_IPV6, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,      FALSE,     4        },  /* 1588oUDPoIPv6oMPLSoE (MPLS routed) */
/*21*/ {DNX_PTP_ANY , DNX_PTP_IPV4, DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     3        },  /* 1588oUDPoIPv4oIPv4oE (switched)   */
/*22*/ {DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_IPV6, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     5        },  /* 1588oUDPoIPv6oEoMPLSoE (MPLS routed)   */
/*23*/ {DNX_PTP_ANY , DNX_PTP_ETH , DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     5        },  /* 1588oUDPoIPv4oEoMPLSoE (MPLS routed)   */
/*24*/ {DNX_PTP_ANY , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , FALSE,     4        },  /* 1588oUDPoIPv4oMPLSoMPLSoE (switched)   */
/*25*/ {DNX_PTP_ANY , DNX_PTP_IPV4, DNX_PTP_ETH , DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY ,             FALSE,     4        },  /* 1588oUDPoIPv4oEoIPv4oAny   */
/*26*/ {DNX_PTP_ANY , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_IPV6, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , DNX_PTP_ANY , FALSE,     4        },  /* 1588oUDPoIPv6oMPLSoMPLSoE (switched)   */
/*27*/ {DNX_PTP_ANY , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_ETH , DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , FALSE,     4        },  /* 1588oUDPoIPv4oEoMPLSoE (switched)   */
/*28*/ {DNX_PTP_ANY , DNX_PTP_MPLS_TERM, DNX_PTP_MPLS_UNTERM, DNX_PTP_IPV4, DNX_PTP_IPV4, DNX_PTP_UDP , DNX_PTP_1588, DNX_PTP_ANY , FALSE,     4        }}; /* 1588oUDPoIPv4oIPv4oMPLSoE (switched)  */

    /* *INDENT-ON* */

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * init TP-COMMNAD table 
     */
    /** according to the command result from IHB_IEEE_1588_ACTION table, */
    /** an action is taken at the command bit place. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_EGRESS_TP_COMMAND, &entry_handle_id));
    /** configure tp_cmd RX_STAMPING_AND_TX_CF_STAMPING, which reflect one step */
    tp_cmd = DBAL_ENUM_FVAL_PTP_TP_COMMAND_RX_STAMPING_AND_TX_CF_STAMPING;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_COMMAND, tp_cmd);
    /** when bit is 1 correction field is updated when packet is transmitted */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_STAMP_CF, INST_SINGLE, TRUE);
    /** when bit is 1 the timestamp is taken when packet is received */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_STAMP, INST_SINGLE, TRUE);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** configure tp_cmd TX_RECORDING, which reflect two step */
    tp_cmd = DBAL_ENUM_FVAL_PTP_TP_COMMAND_TX_RECORDING;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_COMMAND, tp_cmd);
    /** when bit is 1 tx time is recorded when packet is transmitted */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECORD_TS, INST_SINGLE, TRUE);
    /** when bit is 1 correction field is updated when packet is transmitted */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_STAMP_CF, INST_SINGLE, FALSE);
    /** when bit is 1 the timestamp is taken when packet is received */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_STAMP, INST_SINGLE, FALSE);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);
    /*
     * init IDENTIFICATION TCAM table 
     */
    for (entry_idx = 0; entry_idx < DNX_PTP_1588_IDENTIFICATION_CAM_TABLE_ENTRIES_NUM; entry_idx++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_IEEE_1588_IDENTIFICATION, &entry_handle_id));
        /** set access ID (index) */
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_idx));

        /** set key */
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][0], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_0,
                                          ieee_1588_identification_cam[entry_idx][0], mask);
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][1], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_1,
                                          ieee_1588_identification_cam[entry_idx][1], mask);
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][2], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_2,
                                          ieee_1588_identification_cam[entry_idx][2], mask);
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][3], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_3,
                                          ieee_1588_identification_cam[entry_idx][3], mask);
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][4], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_4,
                                          ieee_1588_identification_cam[entry_idx][4], mask);
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][5], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_5,
                                          ieee_1588_identification_cam[entry_idx][5], mask);
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][6], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_6,
                                          ieee_1588_identification_cam[entry_idx][6], mask);
        SHR_IF_ERR_EXIT(dnx_ptp_identification_mask_calc(unit, ieee_1588_identification_cam[entry_idx][7], &mask));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_TYPE_7,
                                          ieee_1588_identification_cam[entry_idx][7], mask);

        /** set the result as written to "program field */
        /** set result - da profile and valid bit */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_IEEE_1588, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NONE_UDP_ENCAPSULATION, INST_SINGLE,
                                     ieee_1588_identification_cam[entry_idx][8]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_INDEX, INST_SINGLE,
                                     ieee_1588_identification_cam[entry_idx][9]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

    /** Init GLOBAL CONFIG */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_GLOBAL_CONFIG, &entry_handle_id));
    /*
     * ZERO means enable. See "TS_UDP_Encapsulation = (Cfg_TS_UDP_Encap == application_specific_ext.TS_Encapsulation)" 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDP_CHECKSUM_ENABLE, INST_SINGLE, 0);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Initialize the ptp module
 */
shr_error_e
dnx_ptp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        /** HW initialization */
        SHR_IF_ERR_EXIT(dnx_ptp_hw_init(unit));

        /** Note: init TM DB per port is made in Algo module */
        /** Note: adding internally used 1588 drop trap is made from tune module */
    }
#if defined(INCLUDE_PTP)
    SHR_IF_ERR_EXIT(bcm_common_ptp_init(unit));
#endif /* defined(INCLUDE_PTP) */

    /*
     * TBD JR2: UDH Header parsing information needs to be sent to firmware 
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   set the ieee 1588 profile id of the pp port
 *
 * \param [in] unit - The unit number.
 * \param [in] core_id - The core number.
 * \param [in] pp_port - The pp port
 * \param [in] profile_idx - the profile id of 1588 per port, as used in action table
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_port_profile_set(
    int unit,
    int core_id,
    int pp_port,
    int profile_idx)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, pp_port);
    /** set the table values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IEEE_1588_PROFILE, INST_SINGLE, profile_idx);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get the ieee 1588 profile id of the pp port
 *
 * \param [in] unit - The unit number.
 * \param [in] core_id - The core number.
 * \param [in] pp_port - The pp port
 * \param [out] profile_idx - the profile id of 1588 per port, as used in action table
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_port_profile_get(
    int unit,
    int core_id,
    int pp_port,
    int *profile_idx)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, pp_port);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IEEE_1588_PROFILE, INST_SINGLE,
                               (uint32 *) profile_idx);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * convert dbal message type to the bcm message type
 */
static int
dnx_ptp_message_type_dbal_to_bcm_convert(
    int unit,
    int dbal_message_type)
{
    int bcm_message_type = 0;
    switch (dbal_message_type)
    {
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_SYNC:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_SYNC;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_DELAY_REQ:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_DELAY_REQ;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_REQ:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_RESP:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_FOLLOWUP:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_FOLLOWUP;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_DELAY_RESP:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_DELAY_RESP;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_RESP_FOLLOWUP:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_ANNOUNCE:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_ANNOUNCE;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_SIGNALLING:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_SIGNALLING;
            break;
        case DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_MANAGMENT:
            bcm_message_type = BCM_PORT_TIMESYNC_PKT_MANAGMENT;
            break;
        default:
            /** unknown message types will get bcm_type=0 and will be treathed by the action table as action trap */
            bcm_message_type = 0;
            break;
    }

    return bcm_message_type;
}

/**
 * \brief
 *   set the ieee 1588 action table 
 *
 * \param [in] unit - The unit number.
 * \param [in] profile_id - The profile number.
 * \param [in] profile_info - pointer to the configuration strucrture, as saved in the template manager
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_action_table_set(
    int unit,
    int profile_id,
    dnx_ptp_port_profile_info_t * profile_info)
{
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info_msg_type;
    dbal_table_field_info_t field_info_address;
    int msg_type, address, none_udp_encap, is_two_step, bcm_msg_type;
    dbal_enum_value_field_ptp_tp_command_e tp_command;
    int action_index, code_index, update_ts_compensation = FALSE;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_PTP_1588_ACTION, DBAL_FIELD_MESSAGE_TYPE,
                                               TRUE, 0, 0, &field_info_msg_type));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_PTP_1588_ACTION, DBAL_FIELD_ADDRESS,
                                               TRUE, 0, 0, &field_info_address));
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    for (msg_type = 0; msg_type <= field_info_msg_type.max_value; msg_type++)
    {
        for (address = 0; address <= field_info_address.max_value; address++)
        {
            for (none_udp_encap = FALSE; none_udp_encap <= TRUE; none_udp_encap++)
            {
                for (is_two_step = FALSE; is_two_step <= TRUE; is_two_step++)
                {
                    /** default profile */
                    if (profile_info->ptp_enabled == FALSE)
                    {
                        /** if 1588 packet detected on port which is not ptp_enable - trap the packet */
                        tp_command = DBAL_ENUM_FVAL_PTP_TP_COMMAND_NO_STAMPING_NO_RECORDING;
                        action_index = DNX_PTP_ACTION_FIELD_ACTION_INDEX_TRAP;
                        code_index = DNX_PTP_TRAP_CODE_OFFSET_GET(DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_0);
                        update_ts_compensation = FALSE;
                    }
                    else
                    {
                        /** always doing ts compensation (ptp delay). if not required, the delay will be set to zero */
                        update_ts_compensation = TRUE;
                        /** by default setting command - no RX time record & no TX time stamping */
                        tp_command = DBAL_ENUM_FVAL_PTP_TP_COMMAND_NO_STAMPING_NO_RECORDING;
                        /** when one step TC is enabled setting command - RX record & TX stamping */
                        if (_SHR_IS_FLAG_SET(profile_info->flags, BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP))
                        {
                            /** 1588 protocol TC implementation, need to updated CF in following 4 messages */
                            if (DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_SYNC == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_DELAY_REQ == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_REQ == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_RESP == msg_type)
                            {
                                /** when one step TC is enabled setting command 1, RX time record & TX time stamping */
                                tp_command = DBAL_ENUM_FVAL_PTP_TP_COMMAND_RX_STAMPING_AND_TX_CF_STAMPING;
                            }
                        }
                        else if (_SHR_IS_FLAG_SET(profile_info->flags, BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP))
                        {
                            /** 1588 protocol TC implementation, need to updated CF in following 4 messages */
                            if (DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_SYNC == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_DELAY_REQ == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_REQ == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_RESP == msg_type)
                            {
                                /** when two step TC is enabled setting command 2, TX time recording */
                                tp_command = DBAL_ENUM_FVAL_PTP_TP_COMMAND_TX_RECORDING;
                            }
                        }
                        /*
                         * Jericho mode beginning
                         * In Jericho, default mode means no stamp
                         * {
                         */
                        else if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
                        {
                            /** no flag s set. configure the tp command based on the "is_two_step" (flag-field-9) indication */
                            if (DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_SYNC == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_DELAY_REQ == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_REQ == msg_type ||
                                DBAL_ENUM_FVAL_PTP_1588_MSG_TYPE_PKT_PDELAY_RESP == msg_type)
                            {
                                /** when two step TC is enabled setting command 2, TX time recording */
                                tp_command = (is_two_step == TRUE) ? DBAL_ENUM_FVAL_PTP_TP_COMMAND_TX_RECORDING :
                                    DBAL_ENUM_FVAL_PTP_TP_COMMAND_RX_STAMPING_AND_TX_CF_STAMPING;
                            }
                        }
                        /*
                         * Jericho mode end
                         * {
                         */

                        /*
                         * pkt_drop, pkt_tocpu bitmaps of 1588 event and general packet types indicating whether to :
                         *   1. forward    (drop-0,tocpu-0) 
                         *   2. trap/snoop (drop-0,tocpu-1)
                         *   3. drop       (drop-1,tocpu-0) 
                         * the packet.
                         */
                        bcm_msg_type = dnx_ptp_message_type_dbal_to_bcm_convert(unit, msg_type);
                        /** unknown message types will get bcm_type=0 and will be treathed by the action table as action trap */
                        if (bcm_msg_type == 0)
                        {
                            action_index = DNX_PTP_ACTION_FIELD_ACTION_INDEX_TRAP;
                            code_index = DNX_PTP_TRAP_CODE_OFFSET_GET(DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_0);
                        }
                        else if (((profile_info->pkt_drop & bcm_msg_type) == 0) &&
                                 ((profile_info->pkt_tocpu & bcm_msg_type) == 0))
                        {
                            action_index = DNX_PTP_ACTION_FIELD_ACTION_INDEX_FWD;
                            code_index = DNX_PTP_TRAP_CODE_OFFSET_GET(DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_2);
                        }
                        else if (((profile_info->pkt_drop & bcm_msg_type) != 0))
                        {
                            action_index = DNX_PTP_ACTION_FIELD_ACTION_INDEX_DROP;
                            code_index = DNX_PTP_TRAP_CODE_OFFSET_GET(DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_1);
                        }
                        else if (((profile_info->pkt_tocpu & bcm_msg_type) != 0))
                        {
                            if ((profile_info->user_trap_id >= DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_3) &&
                                (profile_info->user_trap_id <= DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_7))
                            {
                                action_index =
                                    profile_info->user_trap_id - DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_0;
                                code_index = DNX_PTP_TRAP_CODE_OFFSET_GET(profile_info->user_trap_id);
                            }
                            else
                            {
                                action_index = DNX_PTP_ACTION_FIELD_ACTION_INDEX_TRAP;
                                code_index =
                                    DNX_PTP_TRAP_CODE_OFFSET_GET(DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_0);
                            }
                        }
                    }

                    /** Taking a handle */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_ACTION, &entry_handle_id));
                    /** Setting key fields */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TWO_STEP, is_two_step);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MESSAGE_TYPE, msg_type);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDRESS, address);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NONE_UDP_ENCAPSULATION,
                                               none_udp_encap);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_PROFILE, profile_id);
                    /** set the table values */
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UPDATE_TIME_STAMP_COMPENSATION,
                                                 INST_SINGLE, update_ts_compensation);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TP_COMMAND, INST_SINGLE, tp_command);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_INDEX, INST_SINGLE,
                                                 action_index);
                    if (dnx_data_ptp.general.feature_get(unit, dnx_data_ptp_general_has_trap_code_offset) == TRUE)
                    {
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CODE_INDEX, INST_SINGLE,
                                                     code_index);
                    }

                    /** Preforming the action */
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                    DBAL_HANDLE_FREE(unit, entry_handle_id);
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   set ieee 1588 port properties - mac enable and how mac operate (internal/external)
 * \param [in] unit - The unit number.
 * \param [in] core_id - The core number.
 * \param [in] pp_port - pp port. 
 * \param [in] config_count - enable/disable 1588 functionality
 * \param [in] use_external_mac - TRUE if using external mac
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_mac_config_set(
    int unit,
    int core_id,
    int pp_port,
    int config_count,
    int use_external_mac)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, pp_port);

    if (use_external_mac == TRUE && config_count == TRUE)
    {
        /** when external MAC is enabled and count=TRUE on the port: */
        /** Incoming packets - setting timestamp to 0 for systemHeader JR-1 mode or don't use ingress_ts in JR-2 mode,*/
        /** the external MAC already reduced the RX time from the correction field */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_TIME_STAMP, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTERNAL_BRCM_MAC, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_INGRESS_TIME_STAMP, INST_SINGLE, FALSE);
        /** disablling 1588 MAC Tx stamping */
        /** if external mac enable, the internal mac disable */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IEEE_1588_MAC_ENABLE, INST_SINGLE, FALSE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_TIME_STAMP, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTERNAL_BRCM_MAC, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_INGRESS_TIME_STAMP, INST_SINGLE,
                                     (config_count == TRUE));
        /** enabling 1588 MAC Tx stamping */
        /** if external mac disable and count=TRUE, the internal mac enable. otherwise - disable */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IEEE_1588_MAC_ENABLE, INST_SINGLE,
                                     (config_count == TRUE && use_external_mac == FALSE));
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get from HW if using external mac for a specific pp port
 * \param [in] unit - The unit number.
 * \param [in] core_id - The core number.
 * \param [in] pp_port - pp port. 
 * \param [in] config_count - enable/disable 1588 functionality
 * \param [out] use_external_mac - TRUE if using external mac
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_mac_config_get(
    int unit,
    int core_id,
    int pp_port,
    int config_count,
    int *use_external_mac)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, pp_port);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXTERNAL_BRCM_MAC, INST_SINGLE,
                               (uint32 *) use_external_mac);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_ptp_port_timesync_config_set(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t * config_array)
{
    dnx_ptp_port_profile_info_t profile_info;
    uint8 first_reference, last_reference;
    int new_profile, old_profile;
    int port_idx;
    int use_external_mac;
    int port_in_lag = FALSE;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t local_port;
    int core;
    uint32 ptc;

    SHR_FUNC_INIT_VARS(unit);

    /** clear profile info */
    sal_memset(&profile_info, 0x0, sizeof(dnx_ptp_port_profile_info_t));

    /** set the profile info */
    profile_info.ptp_enabled = config_count;

    if (config_count == TRUE)
    {
        if (config_array->flags & BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP)
        {
            profile_info.flags |= BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
        }
        if (config_array->flags & BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP)
        {
            profile_info.flags |= BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP;
        }

        profile_info.pkt_drop = config_array->pkt_drop;
        profile_info.pkt_tocpu = config_array->pkt_tocpu;
        profile_info.user_trap_id = config_array->user_trap_id;
        use_external_mac = _SHR_IS_FLAG_SET(config_array->flags, BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE);

    }
    else
    {
        use_external_mac = 0;
    }

    /** Check if port is member of LAG */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));

    if (port_in_lag)
    {
        bcm_gport_t gport;
        bcm_trunk_t tid_find;
        bcm_trunk_pp_port_allocation_info_t allocation_info;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port,
                         DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT |
                         DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        local_port = gport_info.local_port;
        /** Get the relevant core ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, local_port, &core));

        /** Get the relevant sysport Gport */
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, gport_info.sys_port);

        /** Get trunk id from sysport Gport */
        SHR_IF_ERR_EXIT(bcm_trunk_find(unit, -1, gport, &tid_find));

        /** Get the pp port allocation info of a given trunk */
        SHR_IF_ERR_EXIT(bcm_trunk_pp_port_allocation_get(unit, tid_find, 0, &allocation_info));

        /** get from HW the old_profile used for that port */
        SHR_IF_ERR_EXIT(dnx_ptp_port_profile_get(unit, core,
                                                 allocation_info.pp_port_per_core_array[core], &old_profile));

        /** set SW state - allocation manager */
        SHR_IF_ERR_EXIT(algo_ptp_db.ptp_port_profile.exchange(unit, 0, &profile_info, old_profile, NULL,
                                                              &new_profile, &first_reference, &last_reference));

        /** and update HW */
        if (first_reference)
        {
            SHR_IF_ERR_EXIT(dnx_ptp_action_table_set(unit, new_profile, &profile_info));
        }
        SHR_IF_ERR_EXIT(dnx_ptp_port_profile_set(unit, core,
                                                 allocation_info.pp_port_per_core_array[core], new_profile));

        /** configure other attributes corresponding to the port */
        SHR_IF_ERR_EXIT(dnx_ptp_mac_config_set(unit, core,
                                               allocation_info.pp_port_per_core_array[core], config_count,
                                               use_external_mac));
    }
    else
    {
        /** Get pp-port and core-id first. */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** Update the profile of the port */
        for (port_idx = 0; port_idx < gport_info.internal_port_pp_info.nof_pp_ports; port_idx++)
        {
            /** get from HW the old_profile used for that port */
            SHR_IF_ERR_EXIT(dnx_ptp_port_profile_get(unit, gport_info.internal_port_pp_info.core_id[port_idx],
                                                     gport_info.internal_port_pp_info.pp_port[port_idx], &old_profile));

            /** set SW state - allocation manager */
            SHR_IF_ERR_EXIT(algo_ptp_db.ptp_port_profile.exchange(unit, 0, &profile_info, old_profile, NULL,
                                                                  &new_profile, &first_reference, &last_reference));

            /** and update HW */
            if (first_reference)
            {
                SHR_IF_ERR_EXIT(dnx_ptp_action_table_set(unit, new_profile, &profile_info));
            }
            SHR_IF_ERR_EXIT(dnx_ptp_port_profile_set(unit, gport_info.internal_port_pp_info.core_id[port_idx],
                                                     gport_info.internal_port_pp_info.pp_port[port_idx], new_profile));

            /** configure other attributes corresponding to the port */
            SHR_IF_ERR_EXIT(dnx_ptp_mac_config_set(unit, gport_info.internal_port_pp_info.core_id[port_idx],
                                                   gport_info.internal_port_pp_info.pp_port[port_idx], config_count,
                                                   use_external_mac));
        }
    }

    /** Configure ingress ptp attributes corresponding to PTC */

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port = gport_info.local_port;
    /** Get PTC and core */
    SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core));

    /** Configure ingress ptp attributes corresponding to PTC */
    SHR_IF_ERR_EXIT(dnx_ptp_mac_config_set(unit, core, ptc, config_count, use_external_mac));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_ptp_port_timesync_config_get(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t * config_array,
    int *array_count)
{
    dnx_ptp_port_profile_info_t profile_info;
    int profile_id, ref_count = 0;
    int port_idx, use_external_mac = 0;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int port_in_lag = FALSE;
    bcm_port_t local_port;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if port is member of LAG */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));

    if (port_in_lag)
    {
        bcm_gport_t gport;
        bcm_trunk_t tid_find;
        bcm_trunk_pp_port_allocation_info_t allocation_info;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port,
                         DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT |
                         DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        local_port = gport_info.local_port;
        /** Get the relevant core ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, local_port, &core));

        /** Get the relevant sysport Gport */
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, gport_info.sys_port);

        /** Get trunk id from sysport Gport */
        SHR_IF_ERR_EXIT(bcm_trunk_find(unit, -1, gport, &tid_find));

        /** Get the pp port allocation info of a given trunk */
        SHR_IF_ERR_EXIT(bcm_trunk_pp_port_allocation_get(unit, tid_find, 0, &allocation_info));

        /** get from HW the old_profile used for that port */
        SHR_IF_ERR_EXIT(dnx_ptp_port_profile_get(unit, core,
                                                 allocation_info.pp_port_per_core_array[core], &profile_id));

        /** get info from SW state - allocation manager */
        SHR_IF_ERR_EXIT(algo_ptp_db.ptp_port_profile.profile_data_get(unit, profile_id, &ref_count, &profile_info));

        if (profile_info.ptp_enabled == FALSE)
        {
            *array_count = 0;
        }
        else
        {
            bcm_port_timesync_config_t_init(config_array);
            *array_count = 1;
            config_array->flags = profile_info.flags;
            config_array->flags |= BCM_PORT_TIMESYNC_DEFAULT;
            /** get from HW if using external mac */
            SHR_IF_ERR_EXIT(dnx_ptp_mac_config_get(unit, core,
                                                   allocation_info.pp_port_per_core_array[core], TRUE,
                                                   &use_external_mac));
            if (use_external_mac == TRUE)
            {
                config_array->flags |= BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE;
            }
            config_array->pkt_drop = profile_info.pkt_drop;
            config_array->pkt_tocpu = profile_info.pkt_tocpu;
            config_array->user_trap_id = profile_info.user_trap_id;
        }
    }
    else
    {
        /** Get pp-port and core-id first. */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** get the profile of the port */
        for (port_idx = 0; port_idx < gport_info.internal_port_pp_info.nof_pp_ports; port_idx++)
        {

            /** get from HW the old_profile used for that port */
            SHR_IF_ERR_EXIT(dnx_ptp_port_profile_get(unit, gport_info.internal_port_pp_info.core_id[port_idx],
                                                     gport_info.internal_port_pp_info.pp_port[port_idx], &profile_id));

            /** get info from SW state - allocation manager */
            SHR_IF_ERR_EXIT(algo_ptp_db.ptp_port_profile.profile_data_get(unit, profile_id, &ref_count, &profile_info));

            if (profile_info.ptp_enabled == FALSE)
            {
                *array_count = 0;
            }
            else
            {
                bcm_port_timesync_config_t_init(config_array);
                *array_count = 1;
                config_array->flags = profile_info.flags;
                config_array->flags |= BCM_PORT_TIMESYNC_DEFAULT;
                /** get from HW if using external mac */
                SHR_IF_ERR_EXIT(dnx_ptp_mac_config_get(unit, gport_info.internal_port_pp_info.core_id[port_idx],
                                                       gport_info.internal_port_pp_info.pp_port[port_idx], TRUE,
                                                       &use_external_mac));
                if (use_external_mac == TRUE)
                {
                    config_array->flags |= BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE;
                }
                config_array->pkt_drop = profile_info.pkt_drop;
                config_array->pkt_tocpu = profile_info.pkt_tocpu;
                config_array->user_trap_id = profile_info.user_trap_id;
                /** break loop, once one port is updated */
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_ptp_timesync_config_set_verify(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t * config_array)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_drv_soc_feature(unit, soc_feature_timesync_support))
    {
        if (0 != config_count && 1 != config_count)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "config_count != 0 && config_count != 1");
        }
        else if (1 == config_count && NULL == config_array)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "config_array == NULL");
        }

        if (config_count)
        {
            /*
             * Verify valid values 
             */
            if (!(config_array->flags & BCM_PORT_TIMESYNC_DEFAULT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "parameters not supported, BCM_PORT_TIMESYNC_DEFAULT must be on");
            }

            if (config_array->flags & BCM_PORT_TIMESYNC_MPLS)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_TIMESYNC_MPLS flag is not supported");
            }

            if (config_array->pkt_drop & config_array->pkt_tocpu)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "same bit should not turned on in pkt_drop & pkt_tocpu");
            }

            if (config_array->flags & BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP &&
                config_array->flags & BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "one step & two step flags should not be enabled simultaniesly");
            }

            if (((config_array->pkt_tocpu != 0) && (config_array->user_trap_id != 0)) &&
                ((config_array->user_trap_id < DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_3) ||
                 (config_array->user_trap_id > DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_IEEE_1588_7)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid user_trap_id = %d. user_trap_id should be of bcmRxTrap1588UserX"
                             "trap only! Use bcm_rx_trap_type_create to allocate a valid trap_id",
                             config_array->user_trap_id);
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "1588 is not supported for this device");
    }

    if (BCM_GPORT_IS_TRUNK(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Trunk group needs to be set via iterating port members of trunk.");
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file  */
shr_error_e
dnx_ptp_timesync_config_get_verify(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t * config_array,
    int *array_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_drv_soc_feature(unit, soc_feature_timesync_support))
    {
        SHR_NULL_CHECK(array_count, _SHR_E_PARAM, "array_count");
        if (array_size != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "array_size must be set to 1");
        }
        SHR_NULL_CHECK(config_array, _SHR_E_PARAM, "config_array");
    }
    else
    {
        SHR_ERR_EXIT(BCM_E_UNAVAIL, "1588 is not supported for this device");
    }

    if (BCM_GPORT_IS_TRUNK(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Trunk group needs to be gotten via iterating port members of trunk.");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ptp_port_delay_set_verify(
    int unit,
    bcm_port_t port,
    int value)
{
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, DBAL_FIELD_INGRESS_P2P_DELAY,
                                               FALSE, 0, 0, &field_info));
    if (value > field_info.max_value)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "value=%d. It is above max value (%d)", value, field_info.max_value);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_ptp_port_delay_set(
    int unit,
    bcm_port_t port,
    int value)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 entry_handle_id;
    bcm_port_t local_port;
    int core;
    uint32 ptc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ptp_port_delay_set_verify(unit, port, value));

    /** Configure ingress_p2p_delay corresponding to PTC */

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port = gport_info.local_port;
    /** Get PTC and core */
    SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, ptc);
    /** set the table values */
    /** Note: INGRESS_P2P_DELAY is in 1/4 nano-seconds, API is in nano-seconds. DBAL handle the conversion */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_P2P_DELAY, INST_SINGLE, value);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ptp_port_delay_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t local_port;
    int core;
    uint32 ptc;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get ingress_p2p_delay corresponding to PTC */

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port = gport_info.local_port;
    /** Get PTC and core */
    SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core));

    /** get the profile of the port */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, ptc);
    /** get the delay value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_P2P_DELAY, INST_SINGLE, (uint32 *) value);
    /** get value */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_control_phy_timesync_set().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [in] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 value)
{
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Gport is not a legal port for this API.\r\n");
    }

    if (dnx_drv_soc_feature(unit, soc_feature_timesync_support))
    {
        switch (type)
        {
            case bcmPortControlPhyTimesyncTimestampAdjust:
            {
                COMPILER_64_SET(work_reg_64, 0, phymodTimesyncCompensationModeCount);
                if (COMPILER_64_GE(value, work_reg_64))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Value %d is not supported for the type bcmPortControlPhyTimesyncTimestampAdjust.\r\n",
                                 COMPILER_64_LO(value));
                }
                break;
            }
            default:
                break;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "1588 is not supported for this device.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set various PHY timesync controls for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [in] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortControlPhyTimesyncTimestampAdjust:
        {
            SHR_IF_ERR_EXIT(portmod_port_control_phy_timesync_set
                            (unit, port, _SHR_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_ADJUST, value));
            break;
        }
        case bcmPortControlPhyTimesyncOneStepEnable:
        {
            SHR_IF_ERR_EXIT(portmod_port_control_phy_timesync_set
                            (unit, port, _SHR_PORT_CONTROL_PHY_TIMESYNC_ONE_STEP_ENABLE, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "dnx_port_phy_timesync_set type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_control_phy_timesync_get().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [out] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Gport is not a legal port for this API.\r\n");
    }

    if (dnx_drv_soc_feature(unit, soc_feature_timesync_support))
    {
        SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "1588 is not supported for this device.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get various PHY timesync controls for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [out] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortControlPhyTimesyncOneStepEnable:
        {
            SHR_IF_ERR_EXIT(portmod_port_control_phy_timesync_get
                            (unit, port, _SHR_PORT_CONTROL_PHY_TIMESYNC_ONE_STEP_ENABLE, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "dnx_port_phy_timesync_get type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_phy_timesync_config_set().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_config_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    int enable;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Gport is not a legal port for this API.\r\n");
    }

    if (dnx_drv_soc_feature(unit, soc_feature_timesync_support))
    {
        SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf");
        SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, port, &enable));
        if (enable)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Users must disable the port before enabling or configuring 1588.\r\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "1588 is not supported for this device.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set various PHY timesync configuaion for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_timesync_config_set(unit, port, (portmod_phy_timesync_config_t *) conf));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_phy_timesync_config_get().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_config_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Gport is not a legal port for this API.\r\n");
    }

    if (dnx_drv_soc_feature(unit, soc_feature_timesync_support))
    {
        SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "1588 is not supported for this device.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get various PHY timesync configuration for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_phy_timesync_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_timesync_config_get(unit, port, (portmod_phy_timesync_config_t *) conf));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_timesync_tx_info_get().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] tx_info - timesync tx info
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_timesync_tx_info_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_timesync_tx_info_t * tx_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Gport is not a legal port for this API.\r\n");
    }

    if (dnx_drv_soc_feature(unit, soc_feature_timesync_support))
    {
        SHR_NULL_CHECK(tx_info, _SHR_E_PARAM, "tx_info");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "1588 is not supported for this device.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get two-step PTP PHY timesync tx info for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] tx_info - timesync tx info
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_ptp_port_timesync_tx_info_set(
    int unit,
    bcm_port_t port,
    bcm_port_timesync_tx_info_t * tx_info)
{
    portmod_fifo_status_t info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_timesync_tx_info_get(unit, port, &info));
    COMPILER_64_SET(tx_info->timestamp, info.timestamps_in_fifo_hi, info.timestamps_in_fifo);
    tx_info->sequence_id = info.sequence_id;

exit:
    SHR_FUNC_EXIT;
}
