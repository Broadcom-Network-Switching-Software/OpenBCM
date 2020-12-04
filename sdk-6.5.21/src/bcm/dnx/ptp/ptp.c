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
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
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

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/uc.h>
#include <soc/uc_msg.h>

#include <bcm/types.h>
#include <shared/bslenum.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ptp_access.h>
#include <soc/portmod/portmod.h>
#include <phymod/phymod.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <bcm/ptp.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/ptp_common.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/dnx/ptp/ptp.h>

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

#if defined(INCLUDE_PTP)
static int bcm_dnx_ptp_dnx_system_header_config_send(
    int unit);
static const bcm_ptp_port_identity_t portid_all =
    { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, PTP_IEEE1588_ALL_PORTS };
#endif /* defined(INCLUDE_PTP) */

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
    SHR_IF_ERR_EXIT(bcm_dnx_ptp_init(unit));
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
 *   set ieee 1588 port properties - RX timestamp operation
 * \param [in] unit - The unit number.
 * \param [in] core_id - The core number.
 * \param [in] ptc - ptc.
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
dnx_ptp_rx_timestamp_config_set(
    int unit,
    int core_id,
    uint32 ptc,
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
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, ptc);

    if (use_external_mac == TRUE && config_count == TRUE)
    {
        /** when external MAC is enabled and count=TRUE on the port: */
        /** Incoming packets - setting timestamp to 0 for systemHeader JR-1 mode or don't use ingress_ts in JR-2 mode,*/
        /** the external MAC already reduced the RX time from the correction field */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_TIME_STAMP, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTERNAL_BRCM_MAC, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_INGRESS_TIME_STAMP, INST_SINGLE, FALSE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_TIME_STAMP, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTERNAL_BRCM_MAC, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_INGRESS_TIME_STAMP, INST_SINGLE,
                                     (config_count == TRUE));
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   set ieee 1588 port properties - enanble/disable 1588 MAC TX timestamping
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
dnx_ptp_tx_timestamp_config_set(
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
        /** disablling 1588 MAC Tx stamping */
        /** if external mac enable, the internal mac disable */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IEEE_1588_MAC_ENABLE, INST_SINGLE, FALSE);
    }
    else
    {
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
 *   get from HW if using external mac for a specific ptc
 * \param [in] unit - The unit number.
 * \param [in] core_id - The core number.
 * \param [in] ptc - ptc.
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
dnx_ptp_external_mac_config_get(
    int unit,
    int core_id,
    int ptc,
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
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT, ptc);

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
        /** Configure TX timestamping */
        SHR_IF_ERR_EXIT(dnx_ptp_tx_timestamp_config_set(unit, core,
                                                 allocation_info.pp_port_per_core_array[core], config_count, use_external_mac));

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
            /** Configure TX timestamping */
            SHR_IF_ERR_EXIT(dnx_ptp_tx_timestamp_config_set(unit, gport_info.internal_port_pp_info.core_id[port_idx],
                                                     gport_info.internal_port_pp_info.pp_port[port_idx], config_count, use_external_mac));
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
    SHR_IF_ERR_EXIT(dnx_ptp_rx_timestamp_config_set(unit, core, ptc, config_count, use_external_mac));

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
    bcm_gport_t gport;
    uint32 ptc;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port,
                     DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT |
                     DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port = gport_info.local_port;

    /** Get the relevant sysport Gport */
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, gport_info.sys_port);

    /** Get PTC and core */
    SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core));

    /** Check if port is member of LAG */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));

    if (port_in_lag)
    {
        bcm_trunk_t tid_find;
        bcm_trunk_pp_port_allocation_info_t allocation_info;

        /** Get trunk id from sysport Gport */
        SHR_IF_ERR_EXIT(bcm_trunk_find(unit, -1, gport, &tid_find));

        /** Get the pp port allocation info of a given trunk */
        SHR_IF_ERR_EXIT(bcm_trunk_pp_port_allocation_get(unit, tid_find, 0, &allocation_info));

        /** Get ieee 1588 profile per pp port */
        SHR_IF_ERR_EXIT(dnx_ptp_port_profile_get(unit, core,
                                                 allocation_info.pp_port_per_core_array[core], &profile_id));

        /** Get info from SW state - allocation manager */
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
            SHR_IF_ERR_EXIT(dnx_ptp_external_mac_config_get(unit, core, ptc, TRUE, &use_external_mac));
            if (use_external_mac == TRUE)
            {
                config_array->flags |= BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE;
            }
            config_array->pkt_drop = profile_info.pkt_drop;
            config_array->pkt_tocpu = profile_info.pkt_tocpu;
            config_array->user_trap_id = profile_info.user_trap_id;
            if (profile_info.flags & BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP)
            {
                config_array->flags |= BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
            }
            if (profile_info.flags & BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP)
            {
                config_array->flags |= BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP;
            }
        }
    }
    else
    {
        /** Get pp-port and core-id first. */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** Get the profile of the port */
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
                SHR_IF_ERR_EXIT(dnx_ptp_external_mac_config_get(unit, core, ptc, TRUE, &use_external_mac));
                if (use_external_mac == TRUE)
                {
                    config_array->flags |= BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE;
                }
                config_array->pkt_drop = profile_info.pkt_drop;
                config_array->pkt_tocpu = profile_info.pkt_tocpu;
                config_array->user_trap_id = profile_info.user_trap_id;
                if (profile_info.flags & BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP)
                {
                    config_array->flags |= BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
                }
                if (profile_info.flags & BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP)
                {
                    config_array->flags |= BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP;
                }
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

#if defined(INCLUDE_PTP)

/**
 * \brief - PTP stack intialization
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    if (dnx_drv_soc_feature(unit, soc_feature_ptp))
    {
        SHR_IF_ERR_EXIT(bcm_common_ptp_init(unit));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an entry to the acceptable master table of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [out] clock_num - PTP clock number
 * \param [out] port_num - PTP clock port number
 * \param [out] priority1_alt_value - Alternate priority1
 * \param [out] *master_info - Acceptable master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_acceptable_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int priority1_alt_value,
    bcm_ptp_clock_peer_address_t * master_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_acceptable_master_add(unit, ptp_id, clock_num, port_num,
                                                         priority1_alt_value, master_info));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get maximum number of acceptable master table entries of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [out] *enabled - Acceptable master table enabled Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_acceptable_master_enabled_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint8 *enabled)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_acceptable_master_enabled_get(unit, ptp_id, clock_num, port_num, enabled));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - bcm_dnx_ptp_acceptable_master_enabled_set.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] enabled - Acceptable master table enabled Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_acceptable_master_enabled_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint8 enabled)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_acceptable_master_enabled_set(unit, ptp_id, clock_num, port_num, enabled));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - List the acceptable master table of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [out] *num_masters - Number of acceptable master table entries
 * \param [out] *master_addr - Acceptable master table
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_acceptable_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_masters,
    int *num_masters,
    bcm_ptp_clock_peer_address_t * master_addr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_acceptable_master_list(unit, ptp_id, clock_num, port_num,
                                                          max_num_masters, num_masters, master_addr));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove an entry from the acceptable master table of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [out] *master_info - Acceptable master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_acceptable_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t * master_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_acceptable_master_remove(unit, ptp_id, clock_num, port_num, master_info));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear (i.e. remove all entries from) the acceptable master table of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_acceptable_master_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_acceptable_master_table_clear(unit, ptp_id, clock_num, port_num));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear (i.e. remove all entries from) the acceptable master table of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *max_table_entries - Maximum number of acceptable master table entries
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_acceptable_master_table_size_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int *max_table_entries)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_acceptable_master_table_size_get(unit, ptp_id, clock_num, port_num,
                                                                    max_table_entries));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register a callback for handling PTP events.
 *
 * \param [in] unit - Unit ID
 * \param [in] cb_types - The set of PTP callbacks types for which the specified callback should be called
 * \param [in] cb - A pointer to the callback function to call for the specified PTP events
 * \param [out] *user_data - Pointer to user data to supply in the callback
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_cb_register(
    int unit,
    bcm_ptp_cb_types_t cb_types,
    bcm_ptp_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_cb_register(unit, cb_types, cb, user_data));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a callback for handling PTP events.
 *
 * \param [in] unit - Unit ID
 * \param [in] cb_types - The set of PTP callbacks types for which the specified callback should be called
 * \param [in] cb - A pointer to the callback function to call for the specified PTP events
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_cb_unregister(
    int unit,
    bcm_ptp_cb_types_t cb_types,
    bcm_ptp_cb cb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_cb_unregister(unit, cb_types, cb));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get clock quality, clock accuracy member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *accuracy - PTP clock accuracy
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_accuracy_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_accuracy_t * accuracy)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_accuracy_get(unit, ptp_id, clock_num, accuracy));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set clock quality, clock accuracy member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *accuracy - PTP clock accuracy
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_accuracy_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_accuracy_t * accuracy)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_accuracy_set(unit, ptp_id, clock_num, accuracy));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get maxStepsRemoved configured for the PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *max_steps_removed - max steps removed for the PTP clock
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_max_steps_removed_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *max_steps_removed)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_max_steps_removed_get(unit, ptp_id, clock_num, max_steps_removed));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set maxStepsRemoved configured for the PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] max_steps_removed - max steps removed for the PTP clock
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_max_steps_removed_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 max_steps_removed)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_max_steps_removed_set(unit, ptp_id, clock_num, max_steps_removed));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] *clock_info - PTP clock information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_create(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_clock_info_t * clock_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_create(unit, ptp_id, clock_info));

    SHR_IF_ERR_EXIT(bcm_dnx_ptp_dnx_system_header_config_send(unit));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP clock current dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *dataset - Current dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_current_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_current_dataset_t * dataset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_current_dataset_get(unit, ptp_id, clock_num, dataset));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP clock current dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *dataset - Current dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_default_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_default_dataset_t * dataset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_default_dataset_get(unit, ptp_id, clock_num, dataset));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP domain member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *domain - PTP clock domain
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_domain_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *domain)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_domain_get(unit, ptp_id, clock_num, domain));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP domain member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] domain - PTP clock domain
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_domain_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 domain)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_domain_set(unit, ptp_id, clock_num, domain));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get configuration information (data and metadata) of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *clock_info - PTP clock information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_info_t * clock_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_get(unit, ptp_id, clock_num, clock_info));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get local priority of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *local_priority - PTP clock local priority
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_local_priority_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *local_priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_local_priority_get(unit, ptp_id, clock_num, local_priority));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set local priority of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] local_priority - PTP clock local priority
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_local_priority_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 local_priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_local_priority_set(unit, ptp_id, clock_num, local_priority));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get slave peer aging time for PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *aging_time - PTP clock peer aging time
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_peer_age_timer_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *aging_time)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_peer_age_timer_get(unit, ptp_id, clock_num, aging_time));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set slave peer aging time for PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] aging_time - PTP clock peer aging time
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_peer_age_timer_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 aging_time)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_peer_age_timer_set(unit, ptp_id, clock_num, aging_time));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get slave peer aging time for PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *override_flag - Override Mask for flags to set
 * \param [out] *override_value - Override Value for flags to set
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_pkt_flags_override_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *override_flag,
    uint32 *override_value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_pkt_flags_override_get(unit, ptp_id, clock_num, override_flag, override_value));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set slave peer aging time for PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] override_flag - Override Mask for flags to set
 * \param [in] override_value - Override Value for flags to set
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_pkt_flags_override_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 override_flag,
    uint32 override_value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_pkt_flags_override_set(unit, ptp_id, clock_num, override_flag, override_value));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP clock parent dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *dataset - Current dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_parent_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_parent_dataset_t * dataset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_parent_dataset_get(unit, ptp_id, clock_num, dataset));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get announce receipt timeout member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *timeout - PTP clock port announce receipt timeout
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_announce_receipt_timeout_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *timeout)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_announce_receipt_timeout_get(unit, ptp_id, clock_num,
                                                                           clock_port, timeout));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set announce receipt timeout member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] timeout - PTP clock port announce receipt timeout
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_announce_receipt_timeout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 timeout)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_announce_receipt_timeout_set(unit, ptp_id, clock_num,
                                                                           clock_port, timeout));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *info - PTP clock port configuration information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_configure(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_configure(unit, ptp_id, clock_num, clock_port, info));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *dataset - Current dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_dataset_t * dataset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_dataset_get(unit, ptp_id, clock_num, clock_port, dataset));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get delay mechanism member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *delay_mechanism - PTP clock port delay mechanism
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_delay_mechanism_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *delay_mechanism)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_delay_mechanism_get(unit, ptp_id, clock_num, clock_port,
                                                                  delay_mechanism));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set delay mechanism member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] delay_mechanism - PTP clock port delay mechanism
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_delay_mechanism_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 delay_mechanism)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_delay_mechanism_set(unit, ptp_id, clock_num, clock_port,
                                                                  delay_mechanism));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Disable a PTP clock port of an ordinary clock (OC) or a boundary clock (BC).
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_disable(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_disable(unit, ptp_id, clock_num, clock_port));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable a PTP clock port of an ordinary clock (OC) or a boundary clock (BC).
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_enable(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_enable(unit, ptp_id, clock_num, clock_port));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port identity of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *identity - PTP port identity
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_identity_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_identity_t * identity)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_identity_get(unit, ptp_id, clock_num, clock_port, identity));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get configuration information (data and metadata) of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *info - PTP clock port configuration information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_info_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_info_get(unit, ptp_id, clock_num, clock_port, info));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP clock port expected asymmetric latencies.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *latency_in - PTP clock port inbound latency (ns)
 * \param [out] *latency_out - PTP clock port outbound latency (ns)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_latency_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *latency_in,
    uint32 *latency_out)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_latency_get(unit, ptp_id, clock_num, clock_port, latency_in,
                                                          latency_out));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP clock port expected asymmetric latencies.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] latency_in - PTP clock port inbound latency (ns)
 * \param [in] latency_out - PTP clock port outbound latency (ns)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_latency_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 latency_in,
    uint32 latency_out)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_latency_set(unit, ptp_id, clock_num, clock_port, latency_in,
                                                          latency_out));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP clock port expected delay asymmetry
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *delay_asymmetry_ns - PTP clock port delay asymmetry (ns)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_delay_asymmetry_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *delay_asymmetry_ns)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_delay_asymmetry_get
                    (unit, ptp_id, clock_num, clock_port, delay_asymmetry_ns));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP clock port expected delay asymmetry
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] delay_asymmetry_ns - PTP clock port delay asymmetry (ns)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_delay_asymmetry_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int delay_asymmetry_ns)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_delay_asymmetry_set
                    (unit, ptp_id, clock_num, clock_port, delay_asymmetry_ns));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get log announce interval of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *interval - PTP clock port log announce interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_announce_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_announce_interval_get(unit, ptp_id, clock_num, clock_port, interval));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set log announce interval of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] interval - PTP clock port log announce interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_announce_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_announce_interval_set(unit, ptp_id, clock_num, clock_port, interval));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get log minimum delay request interval member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *interval - PTP clock port log minimum delay request interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_min_delay_req_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_min_delay_req_interval_get(unit, ptp_id, clock_num,
                                                                             clock_port, interval));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set log minimum delay request interval member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] interval - PTP clock port log minimum delay request interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_min_delay_req_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_min_delay_req_interval_set(unit, ptp_id, clock_num,
                                                                             clock_port, interval));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get log minimum peer delay (PDelay) request interval member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *interval - PTP clock port log minimum peer delay (PDelay) request interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_min_pdelay_req_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_min_pdelay_req_interval_get(unit, ptp_id, clock_num,
                                                                              clock_port, interval));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set log minimum peer delay (PDelay) request interval member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] interval - PTP clock port log minimum peer delay (PDelay) request interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_min_pdelay_req_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_min_pdelay_req_interval_set(unit, ptp_id, clock_num,
                                                                              clock_port, interval));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get log sync interval member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] *interval - PTP clock port log sync interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_sync_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_sync_interval_get(unit, ptp_id, clock_num, clock_port, interval));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set log sync interval member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [in] interval - PTP clock port log sync interval
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_log_sync_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_log_sync_interval_set(unit, ptp_id, clock_num, clock_port, interval));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get MAC address of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *mac - PTP clock port MAC address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_mac_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_mac_t * mac)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_mac_get(unit, ptp_id, clock_num, clock_port, mac));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get network protocol of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *protocol - PTP clock port network protocol
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_protocol_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_protocol_t * protocol)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_protocol_get(unit, ptp_id, clock_num, clock_port, protocol));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port type of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *type - PTP clock port type
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_type_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_type_t * type)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_type_get(unit, ptp_id, clock_num, clock_port, type));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP version number member of a PTP clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *version - PTP clock port PTP version number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_version_number_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *version)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_version_number_get(unit, ptp_id, clock_num, clock_port, version));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get priority1 member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *priority1 - PTP clock priority1
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_priority1_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *priority1)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_priority1_get(unit, ptp_id, clock_num, priority1));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set priority1 member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] priority1 - PTP clock priority1
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_priority1_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 priority1)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_priority1_set(unit, ptp_id, clock_num, priority1));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get priority2 member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *priority2 - PTP clock priority2
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_priority2_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *priority2)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_priority2_get(unit, ptp_id, clock_num, priority2));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set priority2 member of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] priority2 - PTP clock priority2
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_priority2_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 priority2)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_priority2_set(unit, ptp_id, clock_num, priority2));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get slave-only (SO) flag of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *slaveonly - PTP clock slave-only (SO) flag
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_slaveonly_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *slaveonly)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_slaveonly_get(unit, ptp_id, clock_num, slaveonly));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set slave-only (SO) flag of a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] slaveonly - PTP clock slave-only (SO) flag
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_slaveonly_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 slaveonly)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_slaveonly_set(unit, ptp_id, clock_num, slaveonly));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get local time of a PTP node.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *time - PTP timestamp
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_time_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timestamp_t * time)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_time_get(unit, ptp_id, clock_num, time));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP clock time properties dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *data - Time properties dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_time_properties_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_time_properties_t * data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_time_properties_get(unit, ptp_id, clock_num, data));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set local time of a PTP node.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *time - PTP timestamp
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_time_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timestamp_t * time)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_time_set(unit, ptp_id, clock_num, time));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get timescale members of a PTP clock time properties dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *timescale - PTP clock timescale properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_timescale_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timescale_t * timescale)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_timescale_get(unit, ptp_id, clock_num, timescale));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set timescale members of a PTP clock time properties dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *timescale - PTP clock timescale properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_timescale_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timescale_t * timescale)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_timescale_set(unit, ptp_id, clock_num, timescale));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get traceability members of a PTP clock time properties dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *trace - PTP clock traceability properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_traceability_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_trace_t * trace)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_traceability_get(unit, ptp_id, clock_num, trace));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set traceability members of a PTP clock time properties dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *trace - PTP clock traceability properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_traceability_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_trace_t * trace)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_traceability_set(unit, ptp_id, clock_num, trace));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set user description member of the default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *desc - User description
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_user_description_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *desc)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_user_description_set(unit, ptp_id, clock_num, desc));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get UTC members of a PTP clock time properties dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *utc - PTP clock coordinated universal time (UTC) properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_utc_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_utc_t * utc)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_utc_get(unit, ptp_id, clock_num, utc));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set UTC members of a PTP clock time properties dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *utc - PTP clock coordinated universal time (UTC) properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_utc_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_utc_t * utc)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_utc_set(unit, ptp_id, clock_num, utc));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register a C-TDEV alarm callback function.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] alarm_cb - pointer to C-TDEV alarm callback function
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_alarm_callback_register(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_ctdev_alarm_cb alarm_cb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_alarm_callback_register(unit, ptp_id, clock_num, alarm_cb));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a C-TDEV alarm callback function.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_alarm_callback_unregister(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_alarm_callback_unregister(unit, ptp_id, clock_num));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get C-TDEV recursive algorithm forgetting factor (alpha).
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *alpha_numerator - Forgetting factor numerator
 * \param [out] *alpha_denominator - Forgetting factor denominator
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_alpha_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 *alpha_numerator,
    uint16 *alpha_denominator)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_alpha_get(unit, ptp_id, clock_num, alpha_numerator, alpha_denominator));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get C-TDEV recursive algorithm forgetting factor (alpha).
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] alpha_numerator - Forgetting factor numerator
 * \param [in] alpha_denominator - Forgetting factor denominator
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_alpha_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 alpha_numerator,
    uint16 alpha_denominator)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_alpha_set(unit, ptp_id, clock_num, alpha_numerator, alpha_denominator));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get enable/disable state of C-TDEV processing.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *enable - Enable Boolean
 * \param [out] *flags - C-TDEV control flags (UNUSED)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_enable_get(unit, ptp_id, clock_num, enable, flags));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set enable/disable state of C-TDEV processing.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] enable - Enable Boolean
 * \param [in] flags - C-TDEV control flags (UNUSED)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_enable_set(unit, ptp_id, clock_num, enable, flags));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get verbose program control option of C-TDEV processing.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *verbose - Verbose Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_verbose_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *verbose)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_verbose_get(unit, ptp_id, clock_num, verbose));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set verbose program control option of C-TDEV processing.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] verbose - Verbose Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_ctdev_verbose_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int verbose)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_ctdev_verbose_set(unit, ptp_id, clock_num, verbose));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Shut down the PTP subsystem.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_detach(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_detach(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the foreign master dataset of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [out] *data_set - Foreign master dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_foreign_master_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_foreign_master_dataset_t * data_set)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_foreign_master_dataset_get(unit, ptp_id, clock_num, port_num, data_set));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Locks the unit mutex.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_lock(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_lock(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  UnLocks the unit mutex.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_unlock(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_unlock(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP input channels precedence mode.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] channel_select_mode - Input channel precedence mode selector
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_input_channel_precedence_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_select_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_input_channel_precedence_mode_set(unit, ptp_id, clock_num, channel_select_mode));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP input channels switching mode
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] channel_switching_mode - Channel switching mode selector
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_input_channel_switching_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_switching_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_input_channel_switching_mode_set(unit, ptp_id, clock_num, channel_switching_mode));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP input channels
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *num_channels - (IN) Max number of channels (time sources)
 *                             (OUT) Number of returned channels (time sources)
 * \param [out] *channels - Channels (time sources)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_input_channels_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_channels,
    bcm_ptp_channel_t * channels)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_input_channels_get(unit, ptp_id, clock_num, num_channels, channels));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP input channels
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *num_channels - (IN) Max number of channels (time sources)
 *                             (OUT) Number of returned channels (time sources)
 * \param [in] *channels - Channels (time sources)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_input_channels_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_channels,
    bcm_ptp_channel_t * channels)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_input_channels_set(unit, ptp_id, clock_num, num_channels, channels));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets SyncE output control state
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *state - Non-zero to indicate that SyncE output should be controlled by TOP
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_synce_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *state)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_synce_output_get(unit, ptp_id, clock_num, state));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set SyncE output control on/off
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] synce_port - PTP SyncE clock port
 * \param [in] state - Non-zero to indicate that SyncE output should be controlled by TOP
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_synce_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int synce_port,
    int state)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_synce_output_set(unit, ptp_id, clock_num, synce_port, state));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get enable/disable state of modular system functionality
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *enable - Enable Boolean
 * \param [out] *flags - Modular system control flags (UNUSED)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_enable_get(unit, ptp_id, clock_num, enable, flags));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set enable/disable state of modular system functionality
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] enable - Enable Boolean
 * \param [in] flags - Modular system control flags (UNUSED)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_enable_set(unit, ptp_id, clock_num, enable, flags));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PHY timestamping configuration state and data
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *phyts - PHY timestamping Boolean
 * \param [out] *framesync_pin - Framesync GPIO pin
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_phyts_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *phyts,
    int *framesync_pin)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_phyts_get(unit, ptp_id, clock_num, phyts, framesync_pin));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PHY timestamping configuration state and data
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] phyts - PHY timestamping Boolean
 * \param [in] framesync_pin - Framesync GPIO pin
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_phyts_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int phyts,
    int framesync_pin)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_phyts_set(unit, ptp_id, clock_num, phyts, framesync_pin));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PHY port bitmap
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *pbmp - Port bitmap
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_portbitmap_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t * pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_portbitmap_get(unit, ptp_id, clock_num, pbmp));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PHY port bitmap
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] pbmp - Port bitmap
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_portbitmap_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_portbitmap_set(unit, ptp_id, clock_num, pbmp));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get verbose program control option of modular system functionality
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *verbose - Verbose Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_verbose_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *verbose)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_verbose_get(unit, ptp_id, clock_num, verbose));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set verbose program control option of modular system functionality
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] verbose - Verbose Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_modular_verbose_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int verbose)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_modular_verbose_set(unit, ptp_id, clock_num, verbose));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get packet counters
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *counters - Packet counts/statistics
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_packet_counters_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_packet_counters_t * counters)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_packet_counters_get(unit, ptp_id, clock_num, counters));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Clock Port Drop Packet count statistics
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *counters - Drop Packet counts/statistics
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_drop_counters_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_packet_drop_counters_t * counters)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_drop_counters_get(unit, ptp_id, clock_num, clock_port, counters));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Clock Port Drop Packet count statistics
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port  - PTP clock port
 * \param [in] enable - Eanble Drop Packet counts/statistics
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_drop_counters_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_drop_counters_enable_set(unit, ptp_id, clock_num, clock_port, enable));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset packet counters
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num       - PTP clock port number (to clear port-specific stats)
 * \param [in] counter_bitmap - counter bitmap
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_packet_counters_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 counter_bitmap)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_packet_counters_clear(unit, ptp_id, clock_num, port_num, counter_bitmap));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Peer Dataset
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] max_num_peers - max number of peers
 * \param [in] peers - list of peer entries
 * \param [in] *num_peers - # of peer entries returned in peers
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_peer_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_peers,
    bcm_ptp_peer_entry_t * peers,
    int *num_peers)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_peer_dataset_get(unit, ptp_id, clock_num,
                                                    port_num, max_num_peers, peers, num_peers));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set phase offsets for path asymmetry compensation
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *offset - PTP servo phase holdover state offsets
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_phase_offset_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    const bcm_ptp_phase_offset_t * offset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_phase_offset_set(unit, ptp_id, clock_num, offset));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get phase offsets for path asymmetry compensation
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *offset - PTP servo phase holdover state offsets
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_phase_offset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_phase_offset_t * offset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_phase_offset_get(unit, ptp_id, clock_num, offset));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get primary domain member of a PTP transparent clock default dataset
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *primary_domain - PTP transparent clock primary domain
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_primary_domain_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *primary_domain)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_primary_domain_get(unit, ptp_id, clock_num, primary_domain));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set primary domain member of a PTP transparent clock default dataset
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] primary_domain - PTP transparent clock primary domain
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_primary_domain_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int primary_domain)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_primary_domain_set(unit, ptp_id, clock_num, primary_domain));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP servo configuration properties
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *config - PTP servo configuration properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_servo_configuration_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_servo_configuration_get(unit, ptp_id, clock_num, config));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP servo configuration properties
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *config - PTP servo configuration properties
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_servo_configuration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_servo_configuration_set(unit, ptp_id, clock_num, config));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP servo state and status information
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *status - PTP servo state and status information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_servo_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_servo_status_get(unit, ptp_id, clock_num, status));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP servo threshold information
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *threshold - PTP servo threshold
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_servo_threshold_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_servo_threshold_get(unit, ptp_id, clock_num, threshold));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP servo threshold information
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *threshold - PTP servo threshold
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_servo_threshold_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_servo_threshold_set(unit, ptp_id, clock_num, threshold));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PTP signal output
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *threshold - PTP servo threshold
 * \param [out] *signal_output_count - (IN)  Max number of signal outputs
 *                                     (OUT) Number signal outputs returned
 * \param [out] *signal_output_id - Array of signal outputs
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signal_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_count,
    bcm_ptp_signal_output_t * signal_output_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_signal_output_get(unit, ptp_id, clock_num, signal_output_count, signal_output_id));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove PTP signal output
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] signal_output_id - Signal to remove/invalidate
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signal_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int signal_output_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_signal_output_remove(unit, ptp_id, clock_num, signal_output_id));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PTP signal output
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *signal_output_id - ID of signal
 * \param [in] *output_info - Signal information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signal_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_id,
    bcm_ptp_signal_output_t * output_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_signal_output_set(unit, ptp_id, clock_num, signal_output_id, output_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an entry to the signaled unicast master table
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *master_info - Acceptable master address
 * \param [in] mask - Signaled unicast master configuration flags
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signaled_unicast_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_unicast_master_t * master_info,
    uint32 mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_signaled_unicast_master_add(unit, ptp_id, clock_num, port_num, master_info, mask));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove an entry from the signaled unicast master table
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *master_info - Acceptable master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signaled_unicast_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t * master_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_signaled_unicast_master_remove(unit, ptp_id, clock_num, port_num, master_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - List the signaled unicast slaves
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] max_num_slaves - Maximum number of signaled unicast slaves
 * \param [in] *num_slaves - Number of signaled unicast slaves
 * \param [in] *slave_info - Signaled unicast slave table
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signaled_unicast_slave_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_slaves,
    int *num_slaves,
    bcm_ptp_clock_peer_t * slave_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_signaled_unicast_slave_list(unit, ptp_id, clock_num, port_num,
                                                               max_num_slaves, num_slaves, slave_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Cancel active unicast services for slaves and clear signaled unicast
 *          slave table entries that correspond to caller-specified master port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] callstack - Boolean to unsubscribe/unregister unicast services.
 *              TRUE : Cancellation message to slave and PTP stack.
 *              FALSE: Cancellation message tunneled to slave only.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signaled_unicast_slave_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int callstack)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_signaled_unicast_slave_table_clear(unit, ptp_id, clock_num, port_num, callstack));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - bcm_dnx_ptp_cosq_port_send, it's a internal funtion
 *          Send ptp cosq port to ukernel.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_cosq_port_send(
    int unit)
{
    mos_msg_data_t start_msg;
    int timeout_usec = 1900000;
    int ptp_cosq_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /*
     * get ptp_cosq_port from configuration file
     */
    ptp_cosq_port = dnx_data_ptp.ptp_firmware.ptp_cosq_port_get(unit);
    if (!ptp_cosq_port)
    {
        SHR_ERR_EXIT(BCM_E_RESOURCE, "Error: ptp get cosq port failed. \n");
    }

    start_msg.s.mclass = MOS_MSG_CLASS_1588;
    start_msg.s.subclass = MOS_MSG_SUBCLASS_PTP_COSQ_PORT;
    start_msg.s.len = 0;
    start_msg.s.data = bcm_htonl(ptp_cosq_port);

    SHR_IF_ERR_EXIT(soc_cmic_uc_msg_send(unit, 1, &start_msg, timeout_usec));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register a signaling arbiter function that contains the accept/reject logic
 *          for incoming signaling messages.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] arb - PTP signaling message arbiter function pointer
 * \param [in] user_data - User data.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signaling_arbiter_register(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_signaling_arbiter_t arb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_ptp_register_signaling_arbiter(unit, arb, user_data));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a signaling arbiter function that contains the accept/reject logic
 *          for incoming signaling messages.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] arb - PTP signaling message arbiter function pointer
 * \param [in] user_data - User data.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_signaling_arbiter_unregister(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_ptp_unregister_signaling_arbiter(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create a PTP stack instance
 *
 * \param [in] unit - Unit ID
 * \param [in] *ptp_info - Pointer to an PTP Stack Info structure
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_stack_create(
    int unit,
    bcm_ptp_stack_info_t * ptp_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_stack_create(unit, ptp_info));

    SHR_IF_ERR_EXIT(bcm_dnx_ptp_cosq_port_send(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create a PTP stack instance
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - ID of stack to destroy
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_stack_destroy(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_stack_destroy(unit, ptp_id));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get a PTP stack instance
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - ID of stack to get
 * \param [in] ptp_info - info structs for stack
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_stack_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_stack_info_t * ptp_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_stack_get(unit, ptp_id, ptp_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets all PTP stack instances
 *
 * \param [in] unit - Unit ID
 * \param [in] max_size - maximum number of stacks to return
 * \param [in] ptp_info - info structs for stack
 * \param [in] no_of_stacks - actual number of stacks that can be returned
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_stack_get_all(
    int unit,
    int max_size,
    bcm_ptp_stack_info_t * ptp_info,
    int *no_of_stacks)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_stack_get_all(unit, max_size, ptp_info, no_of_stacks));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an entry to the unicast master table of a PTP clock
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *master_info - Acceptable master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_unicast_master_t * master_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_master_add(unit, ptp_id, clock_num, port_num, master_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - List the unicast master table of a PTP clock
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] max_num_masters - Maximum number of unicast master table entries
 * \param [in] *num_masters - Number of unicast master table entries
 * \param [in] *master_addr - Unicast master table
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_masters,
    int *num_masters,
    bcm_ptp_clock_peer_address_t * master_addr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_master_list(unit, ptp_id, clock_num, port_num,
                                                              max_num_masters, num_masters, master_addr));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove an entry from the static unicast master table
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *master_addr - Unicast master table
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t * master_addr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_master_remove(unit, ptp_id, clock_num, port_num, master_addr));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear (i.e. remove all entries from) the unicast master table
 *          of a PTP clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_master_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_master_table_clear(unit, ptp_id, clock_num, port_num));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get maximum number of unicast master table entries of a PTP clock
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *max_table_entries - Maximum number of unicast master table entries
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_master_table_size_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int *max_table_entries)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_master_table_size_get(unit, ptp_id, clock_num, port_num,
                                                                        max_table_entries));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add a unicast slave to a PTP clock port
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *slave_info - Unicast slave information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_slave_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_t * slave_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_slave_add(unit, ptp_id, clock_num, port_num, slave_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  List the unicast slaves of a PTP clock port
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] max_num_slaves - Maximum number of unicast slave table entries
 * \param [in] *num_slaves - Number of unicast slave table entriesunicast slave table entries
 * \param [in] *slave_info - Unicast slave table
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_slave_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_slaves,
    int *num_slaves,
    bcm_ptp_clock_peer_t * slave_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_slave_list(unit, ptp_id, clock_num, port_num,
                                                             max_num_slaves, num_slaves, slave_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Remove a unicast slave of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *slave_info - Unicast slave information
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_slave_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_t * slave_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_slave_remove(unit, ptp_id, clock_num, port_num, slave_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Clear (i.e. remove all) unicast slaves of a PTP clock port.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_static_unicast_slave_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_static_unicast_slave_table_clear(unit, ptp_id, clock_num, port_num));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Instruct TOP to send a PHY Sync pulse, and mark the time that the PHYs are now synced to.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] sync_input - PTP PHY SYNC inputs
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_sync_phy(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_sync_phy_input_t sync_input)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_sync_phy(unit, ptp_id, clock_num, sync_input));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Instruct TOP to send a PHY Sync pulse, and mark the time that the PHYs are now synced to.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *time - BSHB time output
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_bs_time_info_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_bs_time_info_t * time)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_bs_time_info_get(unit, ptp_id, clock_num, time));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Initialize the telecom profile.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_init(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_init(unit, ptp_id, clock_num));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get telecom profile ITU-T G.781 network option.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *network_option - Telecom profile ITU-T G.781 network option
 *              | Disable | Option I | Option II | Option III
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_network_option_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_network_option_t * network_option)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_network_option_get(unit, ptp_id, clock_num, network_option));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set telecom profile ITU-T G.781 network option.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *network_option - Telecom profile ITU-T G.781 network option
 *              | Disable | Option I | Option II | Option III
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_network_option_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_network_option_t network_option)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_network_option_set(unit, ptp_id, clock_num, network_option));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Add a packet master.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *address - Packet master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_port_address_t * address)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_add(unit, ptp_id, clock_num, port_num, address));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get current best packet master clock as defined by telecom profile
 *           master selection logic.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *pktmaster - Best packet master clock
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_best_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktmaster_t * pktmaster)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_best_get(unit, ptp_id, clock_num, pktmaster));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get packet master by address
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *address - Packet master address
 * \param [out] *pktmaster - Best packet master clock
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_port_address_t * address,
    bcm_ptp_telecom_g8265_pktmaster_t * pktmaster)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_get(unit, ptp_id, clock_num, address, pktmaster));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get list of in-use packet masters
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] max_master_count - Maximum number of packet masters
 * \param [out] *num_masters - Number of in-use packet masters
 * \param [out] *best_master - Best packet master list index
 * \param [out] *pktmaster - Packet masters
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int max_master_count,
    int *num_masters,
    int *best_master,
    bcm_ptp_telecom_g8265_pktmaster_t * pktmaster)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_list(unit, ptp_id, clock_num,
                                                                    max_master_count, num_masters, best_master,
                                                                    pktmaster));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set packet master lockout to exclude from master selection process
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] lockout - Packet master lockout Boolean
 * \param [in] *address - Packet master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_lockout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 lockout,
    bcm_ptp_clock_port_address_t * address)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_lockout_set(unit, ptp_id, clock_num, lockout, address));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set packet master non-reversion to control master selection process
 *          if master fails or is unavailable.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] nonres - Packet master non-reversion Boolean
 * \param [in] *address - Packet master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_non_reversion_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 nonres,
    bcm_ptp_clock_port_address_t * address)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_non_reversion_set(unit, ptp_id, clock_num,
                                                                                 nonres, address));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set priority override of packet master
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] override - Packet master priority override Boolean
 * \param [in] *address - Packet master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Priority override controls whether a packet master's priority value
 *   is set automatically using grandmaster priority1 / priority2 in PTP
 *   announce message (i.e., override equals TRUE) or via host API calls
 *   (i.e., override equals FALSE).
 *
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_priority_override(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 override,
    bcm_ptp_clock_port_address_t * address)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_priority_override(unit, ptp_id, clock_num,
                                                                                 override, address));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set priority of packet master
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] priority - Packet master priority
 * \param [in] *address - Packet master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_priority_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 priority,
    bcm_ptp_clock_port_address_t * address)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_priority_set(unit, ptp_id, clock_num,
                                                                            priority, address));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Remove a packet master
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *address - Packet master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_port_address_t * address)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_remove(unit, ptp_id, clock_num, port_num, address));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set packet master wait-to-restore duration to control master selection
 *            process if master fails or is unavailable.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] wait_sec - Packet master wait-to-restore duration
 * \param [in] *address - Packet master address
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_packet_master_wait_duration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint64 wait_sec,
    bcm_ptp_clock_port_address_t * address)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_packet_master_wait_duration_set(unit, ptp_id, clock_num,
                                                                                 wait_sec, address));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get packet timing and PDV statistics thresholds req'd for packet timing
 *           signal fail (PTSF) analysis.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *thresholds - Packet timing and PDV statistics thresholds
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_pktstats_thresholds_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktstats_t * thresholds)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_pktstats_thresholds_get(unit, ptp_id, clock_num, thresholds));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set packet timing and PDV statistics thresholds req'd for packet timing
 *           signal fail (PTSF) analysis.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] thresholds - Packet timing and PDV statistics thresholds
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_pktstats_thresholds_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktstats_t thresholds)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_pktstats_thresholds_set(unit, ptp_id, clock_num, thresholds));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set the ITU-T G.781 quality level (QL) of local clock.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] ql - Quality level
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Ref. ITU-T G.781 and ITU-T G.8265.1.
 *   Quality level is mapped to PTP clockClass.
 *   Quality level is used to infer ITU-T G.781 synchronization network
 *   option. ITU-T G.781 synchronization network option must be uniform
 *   among slave and its packet masters.
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_quality_level_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_quality_level_t ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_quality_level_set(unit, ptp_id, clock_num, ql));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get PTP |Announce|Sync|Delay_Resp| message receipt timeout req'd for
 *           packet timing signal fail (PTSF) analysis.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] message_type - PTP message type
 * \param [out] *receipt_timeout - PTP |Announce|Sync|Delay_Resp| message receipt timeout
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_receipt_timeout_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_message_type_t message_type,
    uint32 *receipt_timeout)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_receipt_timeout_get(unit, ptp_id, clock_num, message_type,
                                                                     receipt_timeout));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set PTP |Announce|Sync|Delay_Resp| message receipt timeout req'd for
 *           packet timing signal fail (PTSF) analysis.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] message_type - PTP message type
 * \param [out] receipt_timeout - PTP |Announce|Sync|Delay_Resp| message receipt timeout
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_receipt_timeout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_message_type_t message_type,
    uint32 receipt_timeout)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_receipt_timeout_set(unit, ptp_id, clock_num, message_type,
                                                                     receipt_timeout));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Shut down the telecom profile.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_telecom_g8265_shutdown(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_telecom_g8265_shutdown(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set Time Of Day format for PTP stack instance.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] type - Time of Day format
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_time_format_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_time_type_t type)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_time_format_set(unit, ptp_id, type));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get time source status.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *status - Time source status
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_timesource_input_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timesource_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_timesource_input_status_get(unit, ptp_id, clock_num, status));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get PTP ToD input source(s).
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *num_tod_sources - (IN) Maximum Number of ToD inputs
 *             (OUT) Number of ToD inputs
 * \param [out] *tod_sources - Array of ToD inputs
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_tod_input_sources_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_tod_sources,
    bcm_ptp_tod_input_t * tod_sources)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_tod_input_sources_get(unit, ptp_id, clock_num, num_tod_sources, tod_sources));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set PTP TOD input(s).
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] num_tod_sources - Number of ToD inputs
 * \param [in] *tod_sources - Array of ToD inputs
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_tod_input_sources_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_tod_sources,
    bcm_ptp_tod_input_t * tod_sources)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_tod_input_sources_set(unit, ptp_id, clock_num, num_tod_sources, tod_sources));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get PTP TOD output(s).
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *tod_output_count - (IN)  Max number of ToD outputs
 *                                 (OUT) Number ToD outputs returned
 * \param [out] *tod_output - Array of ToD outputs
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_tod_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_count,
    bcm_ptp_tod_output_t * tod_output)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_tod_output_get(unit, ptp_id, clock_num, tod_output_count, tod_output));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Remove a ToD output.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] tod_output_id - ToD output ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_tod_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_output_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_tod_output_remove(unit, ptp_id, clock_num, tod_output_id));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set PTP ToD output.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *tod_output_id - ToD output ID
 * \param [in] *output_info - ToD output configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_tod_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_id,
    bcm_ptp_tod_output_t * output_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_tod_output_set(unit, ptp_id, clock_num, tod_output_id, output_info));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get PTP Transparent clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *data_set - PTP transparent clock default dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_transparent_clock_default_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_transparent_clock_default_dataset_t * data_set)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_transparent_clock_default_dataset_get(unit, ptp_id, clock_num, data_set));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get PTP transparent clock port dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_port - PTP clock port number
 * \param [out] *data_set - PTP transparent clock default dataset
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_transparent_clock_port_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 clock_port,
    bcm_ptp_transparent_clock_port_dataset_t * data_set)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_transparent_clock_port_dataset_get(unit, ptp_id, clock_num, clock_port, data_set));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [out] *duration - durationField (sec)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_unicast_request_duration_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_unicast_request_duration_get(unit, ptp_id, clock_num, port_num, duration));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get maximum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [out] *duration_max - maximum durationField (sec)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_unicast_request_duration_max_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration_max)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_unicast_request_duration_max_get(unit, ptp_id, clock_num, port_num, duration_max));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set maximum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] duration_max - maximum durationField (sec)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_unicast_request_duration_max_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration_max)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_unicast_request_duration_max_set(unit, ptp_id, clock_num, port_num, duration_max));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set minimum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] *duration_min - minimum durationField (sec)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_unicast_request_duration_min_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration_min)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_unicast_request_duration_min_get(unit, ptp_id, clock_num, port_num, duration_min));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set minimum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] duration_min - minimum durationField (sec)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_unicast_request_duration_min_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration_min)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_unicast_request_duration_min_set(unit, ptp_id, clock_num, port_num, duration_min));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] port_num - PTP clock port number
 * \param [in] duration - durationField (sec)
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_unicast_request_duration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_unicast_request_duration_set(unit, ptp_id, clock_num, port_num, duration));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get clock quality information a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *clock_quality - PTP clock quality
 * \param [in] flags - identify fields of bcm_ptp_clock_quality_t to get
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_quality_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_quality_t * clock_quality,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_quality_get(unit, ptp_id, clock_num, clock_quality, flags));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set clock quality information a PTP clock default dataset.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] clock_quality - PTP clock quality
 * \param [in] flags - identify fields of bcm_ptp_clock_quality_t to configure
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_quality_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_quality_t clock_quality,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_quality_set(unit, ptp_id, clock_num, clock_quality, flags));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Retrieve applicable clock class value for given clock traceability information.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] traceability_info - clock traceability information
 * \param [in] clock_class - clock class applicable to the provided traceability information as per G8275.1
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_g8275p1_traceability_info_clock_class_map(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_g8275p1_clock_traceability_info_t traceability_info,
    uint8 *clock_class)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_g8275p1_traceability_info_clock_class_map(unit,
                                                                             ptp_id, clock_num, traceability_info,
                                                                             clock_class));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set flags to be used for overriding BMCA
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] flags -  Set flags to be used for overriding BMCA
 * \param [in] enable_mask -  Set flags to be used for overriding BMCA
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_bmca_override_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 flags,
    uint32 enable_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_clock_bmca_override_set(unit, ptp_id, clock_num, flags, enable_mask));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get flags to be used for overriding BMCA.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *flags -  Get flags to be used for overriding BMCA
 * \param [in] *enable_mask -  Get flags to be used for overriding BMCA
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_bmca_override_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *flags,
    uint32 *enable_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_clock_bmca_override_get(unit, ptp_id, clock_num, flags, enable_mask));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get status of whether APTS automatic switching feature is enabled.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *enable -  status  of APTS automatic switching feature
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_enable_get(unit, ptp_id, clock_num, enable));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  APTS - Enable/disable automatic switching feature.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] enable -  enable/disable APTS automatic switching feature
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_enable_set(unit, ptp_id, clock_num, enable));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  APTS - get priority order of time and frequency sources.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *num_sources - number of sources
 * \param [out] *priority_list - priority list of sources
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_source_prio_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_sources,
    bcm_ptp_clock_apts_source_t * priority_list)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_source_prio_get(unit, ptp_id, clock_num, num_sources, priority_list));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  APTS - Set priority order of time and frequency sources.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] num_sources - number of sources
 * \param [in] *priority_list - priority list of sources
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_source_prio_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_sources,
    bcm_ptp_clock_apts_source_t * priority_list)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_source_prio_set(unit, ptp_id, clock_num, num_sources, priority_list));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  APTS - retrieve current operating mode of APTS system.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] *current_mode - current operating mode of APTS system
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_mode_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_apts_mode_t * current_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_mode_get(unit, ptp_id, clock_num, current_mode));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  APTS - indicate whether time or frequency source is available.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] source - time or frequency source
 * \param [in] *enable - get whether time or frequency source is avaialble
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_source_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_apts_source_t source,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_source_enable_get(unit, ptp_id, clock_num, source, enable));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  APTS - set whether time or frequency source is available.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] source - time or frequency source
 * \param [in] enable - Set whether time or frequency source is avaialble
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_source_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_apts_source_t source,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_source_enable_set(unit, ptp_id, clock_num, source, enable));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Ext. PHY - Run state machine to synchronize CMIC time with external PHY time.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] extphy_config - external PHY config
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_external_phy_synchronize(
    int unit,
    int stack_id,
    int clock_num,
    bcm_ptp_external_phy_config_t * extphy_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_external_phy_synchronize(unit, stack_id, clock_num, extphy_config));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  configure a fixed offset to compensate for delays in the GPS receiver path.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [in] offset_nsec - offset to compensate for delays
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_gps_offset_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int offset_nsec)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_gps_offset_set(unit, ptp_id, clock_num, offset_nsec));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  get configured offset to compensate for delays in the GPS receiver path.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 * \param [out] *offset_nsec - configured offset to compensate for delays
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_apts_gps_offset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *offset_nsec)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_apts_gps_offset_get(unit, ptp_id, clock_num, offset_nsec));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  _bcm_dnx_ptp_dnx_syshdr_infomation_send, it's a internal funtion
 *          Purpose of this function is to update firmware about
 *          dnx system header information to facilitate ptp packet porocessing
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
bcm_dnx_ptp_dnx_system_header_config_send(
    int unit)
{

    bcm_ptp_port_identity_t portid;
    uint8 payload[PTP_MGMTMSG_PAYLOAD_DNX_JR2_HEADERS_CONFIGURATION_SIZE_OCTETS] = { 0 };
    uint16 payload_len = 0;
    int tmp = 0;
    uint8 *curs = &payload[0];
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;
    uint32 entry_handle_id;
    bcm_ptp_stack_id_t ptp_id = PTP_STACK_ID_DEFAULT;
    int clock_num = PTP_CLOCK_NUMBER_DEFAULT;
    dnx_jr2_system_headers_info_t jr2_system_headers_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t in_tm_port_pp;
    bcm_port_t in_logical_port;
    int cpu_channel;
    int ptp_cosq_port;
    uint32 flags;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /*
     * read DNX Headers information 
     */
    jr2_system_headers_info.system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE,
                               &(jr2_system_headers_info.ftmh_lb_key_ext_size));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                               &jr2_system_headers_info.ftmh_stacking_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE,
                               &jr2_system_headers_info.pph_base_size);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_SINGLE,
                              &jr2_system_headers_info.udh_enable);

    jr2_system_headers_info.pph_lif_ext_size[0] = 0;
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 0,
                               &jr2_system_headers_info.pph_lif_ext_size[1]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 1,
                               &jr2_system_headers_info.pph_lif_ext_size[2]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 2,
                               &jr2_system_headers_info.pph_lif_ext_size[3]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF_EXT_SIZE, INST_SINGLE,
                               &jr2_system_headers_info.pph_lif_ext_size[4]);
    jr2_system_headers_info.pph_lif_ext_size[5] = 0;
    jr2_system_headers_info.pph_lif_ext_size[6] = 0;
    jr2_system_headers_info.pph_lif_ext_size[7] = 0;

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Suppose JR2 always enable UDH since at least UDH_BASE exists.*/
    jr2_system_headers_info.udh_data_lenght_per_type[0] = dnx_data_field.udh.type_0_length_get(unit);
    jr2_system_headers_info.udh_data_lenght_per_type[1] = dnx_data_field.udh.type_1_length_get(unit);
    jr2_system_headers_info.udh_data_lenght_per_type[2] = dnx_data_field.udh.type_2_length_get(unit);
    jr2_system_headers_info.udh_data_lenght_per_type[3] = dnx_data_field.udh.type_3_length_get(unit);

    /*
     * COSQ Port (204) port details 
     */
    ptp_cosq_port = dnx_data_ptp.ptp_firmware.ptp_cosq_port_get(unit);
    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, ptp_cosq_port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    in_logical_port = gport_info.local_port;

    SHR_IF_ERR_EXIT(bcm_dnx_port_get(unit, ptp_cosq_port, &flags, &interface_info, &mapping_info));
    in_tm_port_pp = mapping_info.pp_port;

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, in_logical_port, &cpu_channel));

    jr2_system_headers_info.cosq_port_pp_port = in_tm_port_pp;
    jr2_system_headers_info.cosq_port_cpu_channel = cpu_channel;

    SHR_IF_ERR_EXIT(_bcm_ptp_function_precheck(unit, ptp_id, clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT));

    if (SHR_FAILURE(bcm_common_ptp_clock_port_identity_get(unit, ptp_id, clock_num, PTP_IEEE1588_ALL_PORTS, &portid)))
    {
        /*
         * On error, target message to (all clocks, all ports) portIdentity. 
         */
        portid = portid_all;
    }

    /*
     * Make payload.
     *    Octet 0...2     : Custom management message key/identifier.
     *                      Octet 0= 'B'; Octet 1= 'C'; Octet 2= 'M'.
     *    Octet 3...5     : Reserved.
     *    Octet 6...23    : DNX Headers configurable parameters.
     */

    curs = &payload[0];
    sal_memcpy(curs, "BCM\0\0\0", 6);
    curs += 6;

    _bcm_ptp_uint32_write(curs, (uint32) jr2_system_headers_info.ftmh_lb_key_ext_size);
    curs += sizeof(uint32);

    _bcm_ptp_uint32_write(curs, (uint32) jr2_system_headers_info.ftmh_stacking_ext_size);
    curs += sizeof(uint32);

    _bcm_ptp_uint32_write(curs, (uint32) jr2_system_headers_info.pph_base_size);
    curs += sizeof(uint32);

    for (tmp = 0; tmp < BCM_PTP_DNX_JER2_PPH_LIF_EXT_TYPE_MAX; tmp++)
    {
        _bcm_ptp_uint32_write(curs, (uint32) jr2_system_headers_info.pph_lif_ext_size[tmp]);
        curs += sizeof(uint32);
    }

    *curs++ = jr2_system_headers_info.system_headers_mode;
    *curs++ = jr2_system_headers_info.udh_enable;

    for (tmp = 0; tmp < BCM_PTP_DNX_JER2_UDH_DATA_TYPE_MAX; tmp++)
    {
        *curs++ = jr2_system_headers_info.udh_data_lenght_per_type[tmp];
    }

    _bcm_ptp_uint32_write(curs, (uint32) jr2_system_headers_info.cosq_port_cpu_channel);
    curs += sizeof(uint32);

    _bcm_ptp_uint32_write(curs, (uint32) jr2_system_headers_info.cosq_port_pp_port);
    curs += sizeof(uint32);

    payload_len = curs - payload;

    SHR_IF_ERR_EXIT(_bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid,
                                                     PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_DNX_JR2_HEADERS_CONFIGURATION,
                                                     payload, payload_len, resp, &resp_len));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Add a new TLV config.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_tlv_config_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    bcm_ptp_tlv_type_t tlv_type,
    uint16 data_len,
    uint8 *data,
    bcm_ptp_tlv_action_t action,
    int config_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_tlv_config_add
                    (unit, ptp_id, clock_index, clock_port, tlv_type, data_len, data, action, config_id));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  delete existing TLV config.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_tlv_config_delete(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    int config_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_tlv_config_delete(unit, ptp_id, clock_index, clock_port, config_id));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  delete all TLV config.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_port_tlv_config_delete_all(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_port_tlv_config_delete_all(unit, ptp_id, clock_index, clock_port));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Enable or disable a config.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_tlv_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    int config_id,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_tlv_enable_set(unit, ptp_id, clock_index, clock_port, config_id, enable));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get the status of a config.
 *
 * \param [in] unit - Unit ID
 * \param [in] ptp_id - PTP stack ID
 * \param [in] clock_num - PTP clock number
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_ptp_clock_tlv_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    int config_id,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_ptp_clock_tlv_enable_get(unit, ptp_id, clock_index, clock_port, config_id, enable));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register input clock esmc quality change callback.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] ql_change_cb - Input clock esmc QL change callback function pointer
 *
 * \return
 *   See shr_error_r
 * \remark
 *   QL change callback generates an event to notify user if a
 *   QL level change happens to take necessary action if required.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_ql_change_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_ql_change_cb ql_change_cb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_ql_change_callback_register(unit, stack_id, ql_change_cb));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Un-register input clock esmc quality change callback.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   QL change callback generates an event to notify user if a
 *   QL level change happens to take necessary action if required.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_ql_change_callback_unregister(
    int unit,
    int stack_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_ql_change_callback_unregister(unit, stack_id));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ESMC PDU tunneling-enabled Boolean.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] enable - Enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Boolean controls whether ToP tunnels slow-protocol Ethertype packets
 *   - which include ESMC PDUs - to host for processing.
 * \see
 *   * None
 */
int
bcm_dnx_esmc_tunnel_get(
    int unit,
    int stack_id,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_tunnel_get(unit, stack_id, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ESMC PDU tunneling-enabled Boolean.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] enable - Enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Boolean controls whether ToP tunnels slow-protocol Ethertype packets
 *   - which include ESMC PDUs - to host for processing.
 * \see
 *   * None
 */
int
bcm_dnx_esmc_tunnel_set(
    int unit,
    int stack_id,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_tunnel_set(unit, stack_id, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ITU-T G.781 networking option for SyncE.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [out] g781_option - (OUT) ITU-T G.781 networking option.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_esmc_g781_option_get(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t * g781_option)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_g781_option_get(unit, stack_id, g781_option));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ITU-T G.781 networking option for SyncE.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [out] g781_option - (OUT) ITU-T G.781 networking option.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_esmc_g781_option_set(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t g781_option)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_g781_option_set(unit, stack_id, g781_option));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ITU-T G.781 networking option for SyncE.
 *
 * \param [in] unit - Unit ID
 * \param [in] opt - ITU-T G.781 networking option.
 * \param [in] ql - ITU-T G.781 quality level.
 * \param [out] ssm_code -  SSM code.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_esmc_QL_SSM_map(
    int unit,
    bcm_esmc_network_option_t opt,
    bcm_esmc_quality_level_t ql,
    uint8 *ssm_code)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_QL_SSM_map(unit, opt, ql, ssm_code));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ITU-T G.781 quality level (QL) corresponding to synchronization
 *   status message (SSM) code.
 *
 * \param [in] unit - Unit ID
 * \param [in] opt - ITU-T G.781 networking option.
 * \param [in] ssm_code -  SSM code.
 * \param [out] ql - ITU-T G.781 quality level.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_esmc_SSM_QL_map(
    int unit,
    bcm_esmc_network_option_t opt,
    uint8 ssm_code,
    bcm_esmc_quality_level_t * ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_SSM_QL_map(unit, opt, ssm_code, ql));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set port bitmap for ESMC Tx
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] essm code - synce clock type.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_essm_code_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_essm_code_t essm_code)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_essm_code_set(unit, stack_id, dpll_index, essm_code));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port bitmap for ESMC Tx
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] essm code - synce clock type.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_essm_code_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_essm_code_t * essm_code)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_essm_code_get(unit, stack_id, dpll_index, essm_code));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get logical DPLL instance input/output bindings.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] bindings  - DPLL instance bindings.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_dpll_bindings_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t * bindings)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_dpll_bindings_get(unit, stack_id, dpll_index, bindings));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set logical DPLL instance input/output bindings.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] bindings  - DPLL instance bindings.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_dpll_bindings_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t * bindings)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_dpll_bindings_set(unit, stack_id, dpll_index, bindings));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get reference clocks for DPLL instances.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] max_num_dpll - Maximum number of DPLL instances.
 * \param [in] dpll_ref - DPLL instance reference clocks.
 * \param [in] num_dpll - Number of DPLL instances.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Reference switching. Gets current reference clocks of DPLL instances
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_dpll_reference_get(
    int unit,
    int stack_id,
    int max_num_dpll,
    int *dpll_ref,
    int *num_dpll)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_dpll_reference_get(unit, stack_id, max_num_dpll, dpll_ref, num_dpll));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get DPLL bandwidth.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] bandwidth  - DPLL bandwidth.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_dpll_bandwidth_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t * bandwidth)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_dpll_bandwidth_get(unit, stack_id, dpll_index, bandwidth));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set DPLL bandwidth.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] bandwidth  - DPLL bandwidth.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_dpll_bandwidth_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t * bandwidth)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_dpll_bandwidth_set(unit, stack_id, dpll_index, bandwidth));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get DPLL instance's phase control configuration.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] phase_control - Phase control configuration.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_dpll_phase_control_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t * phase_control)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_dpll_phase_control_get(unit, stack_id, dpll_index, phase_control));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set DPLL instance's phase control configuration.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] phase_control - Phase control configuration.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_dpll_phase_control_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t * phase_control)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_dpll_phase_control_set(unit, stack_id, dpll_index, phase_control));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Start/stop T-DPLL input clock monitoring, reference selection, and
 *    switching state machine
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] enable - Enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_control(
    int unit,
    int stack_id,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_control(unit, stack_id, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get MAC address of input clock
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] mac - Input clock MAC address.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_mac_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t * mac)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_mac_get(unit, stack_id, clock_index, mac));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set MAC address of input clock
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] mac - Input clock MAC address.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_mac_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t * mac)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_mac_set(unit, stack_id, clock_index, mac));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get fractional frequency error of an input clock from input-clock
 *   monitoring process.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] mac - Input clock MAC address.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_frequency_error_get(
    int unit,
    int stack_id,
    int clock_index,
    int *freq_error_ppb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_frequency_error_get(unit, stack_id, clock_index, freq_error_ppb));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get monitor threshold state of an input clock from input-clock
 *   monitoring process.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] threshold_type  - Input-clock monitoring threshold type.
 * \param [in] threshold_state - Input-clock monitoring threshold state Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_threshold_state_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    int *threshold_state)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_threshold_state_get
                    (unit, stack_id, clock_index, threshold_type, threshold_state));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get input clock enable Boolean
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [out] enable - Input clock enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_enable_get(unit, stack_id, clock_index, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set input clock enable Boolean
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] enable - Input clock enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_enable_set(unit, stack_id, clock_index, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get L1 mux mapping (mux index and port number) of input clock 
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index. 
 * \param [out] l1mux - L1 mux mapping 
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_l1mux_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t * l1mux)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_l1mux_get(unit, stack_id, clock_index, l1mux));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set L1 mux mapping (mux index and port number) of input clock
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] l1mux - L1 mux mapping
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_l1mux_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t * l1mux)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_l1mux_set(unit, stack_id, clock_index, l1mux));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get valid Boolean of an input clock from input-clock monitoring
 *   process
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [out] valid - Input clock valid Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_valid_get(
    int unit,
    int stack_id,
    int clock_index,
    int *valid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_valid_get(unit, stack_id, clock_index, valid));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set input-clock valid Boolean from monitoring process
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] valid - Input clock valid Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_valid_set(
    int unit,
    int stack_id,
    int clock_index,
    int valid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_valid_set(unit, stack_id, clock_index, valid));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get input clock frequency.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [out] clock_frequency   - Frequency (Hz).
 * \param [out] tsevent_frequency - TS event frequency (Hz).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *clock_frequency,
    uint32 *tsevent_frequency)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_frequency_get
                    (unit, stack_id, clock_index, clock_frequency, tsevent_frequency));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set input clock frequency.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] clock_frequency   - Frequency (Hz).
 * \param [in] tsevent_frequency - TS event frequency (Hz).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 clock_frequency,
    uint32 tsevent_frequency)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_frequency_set
                    (unit, stack_id, clock_index, clock_frequency, tsevent_frequency));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get input clock quality level (QL).
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [out] ql - QL.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_ql_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t * ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_ql_get(unit, stack_id, clock_index, ql));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set input clock quality level (QL).
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] ql - QL.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_ql_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_ql_set(unit, stack_id, clock_index, ql));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get input clock priority for reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [out] priority - Input clock priority.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_priority_get(
    int unit,
    int stack_id,
    int clock_index,
    int *priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_priority_get(unit, stack_id, clock_index, priority));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set input clock priority for reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] priority - Input clock priority.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_priority_set(
    int unit,
    int stack_id,
    int clock_index,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_priority_set(unit, stack_id, clock_index, priority));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get input clock lockout Boolean for reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [out] lockout - Input clock lockout Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_lockout_get(
    int unit,
    int stack_id,
    int clock_index,
    int *lockout)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_lockout_get(unit, stack_id, clock_index, lockout));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set input clock lockout Boolean for reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Input clock index.
 * \param [in] lockout - Input clock lockout Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_lockout_set(
    int unit,
    int stack_id,
    int clock_index,
    int lockout)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_lockout_set(unit, stack_id, clock_index, lockout));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get input clock monitoring interval
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [out] monitor_interval - Input clock monitoring interval (sec).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Monitoring interval defines the period over which fractional frequency
 *   error of an input clock is calculated for purposes of threshold-based
 *   comparison / validation.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_monitor_interval_get(
    int unit,
    int stack_id,
    uint32 *monitor_interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_monitor_interval_get(unit, stack_id, monitor_interval));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set input clock monitoring interval
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] monitor_interval - Input clock monitoring interval (sec).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Monitoring interval defines the period over which fractional frequency
 *   error of an input clock is calculated for purposes of threshold-based
 *   comparison / validation.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_monitor_interval_set(
    int unit,
    int stack_id,
    uint32 monitor_interval)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_monitor_interval_set(unit, stack_id, monitor_interval));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get monitor threshold for input-clock valid classification required
 *   in reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] threshold_type - Input clock monitoring threshold type.
 * \param [out] threshold - Input clock monitoring threshold (ppb)..
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_monitor_threshold_get(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_monitor_threshold_get(unit, stack_id, threshold_type, threshold));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set monitor threshold for input-clock valid classification required
 *   in reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] threshold_type - Input clock monitoring threshold type.
 * \param [in] threshold - Input clock monitoring threshold (ppb)..
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_monitor_threshold_set(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_monitor_threshold_set(unit, stack_id, threshold_type, threshold));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get QL-enabled Boolean for reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] ql_enabled - QL-enabled Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_ql_enabled_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *ql_enabled)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_ql_enabled_get(unit, stack_id, dpll_index, ql_enabled));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set QL-enabled Boolean for reference selection
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] ql_enabled - QL-enabled Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_ql_enabled_set(
    int unit,
    int stack_id,
    int dpll_index,
    int ql_enabled)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_ql_enabled_set(unit, stack_id, dpll_index, ql_enabled));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get revertive mode Boolean for reference selection and switching
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] revertive  - Revertive mode Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_revertive_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *revertive)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_revertive_get(unit, stack_id, dpll_index, revertive));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set revertive mode Boolean for reference selection and switching
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] revertive  - Revertive mode Boolean
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_revertive_set(
    int unit,
    int stack_id,
    int dpll_index,
    int revertive)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_revertive_set(unit, stack_id, dpll_index, revertive));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get best (i.e. selected) reference for a DPLL instance
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] best_clock - Best / preferred input clock index
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_best_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *best_clock)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_best_get(unit, stack_id, dpll_index, best_clock));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register input clock monitoring callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] monitor_cb - Input clock monitoring callback function pointer.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Input clock monitoring callback generates an event to notify user if state
 *   has changed w.r.t. a threshold criterion.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_monitor_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_cb monitor_cb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_monitor_callback_register(unit, stack_id, monitor_cb));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Un-register input clock monitoring callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Input clock monitoring callback generates an event to notify user if state
 *   has changed w.r.t. a threshold criterion.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_monitor_callback_unregister(
    int unit,
    int stack_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_monitor_callback_unregister(unit, stack_id));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register input clock reference selection callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] selector_cb - Input clock reference selection callback function pointer..
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Reference selection callback generates an event to notify user if a new
 *   reference is selected but automatic switching to it is deferred, because
 *   revertive option is not enabled.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_selector_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_selector_cb selector_cb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_selector_callback_register(unit, stack_id, selector_cb));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Un-register input clock reference selection callback 
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Reference selection callback generates an event to notify user if a new
 *   reference is selected but automatic switching to it is deferred, because
 *   revertive option is not enabled. 
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_selector_callback_unregister(
    int unit,
    int stack_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_selector_callback_unregister(unit, stack_id));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register input clock callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] type - callback type.
 * \param [in] callback - Input clock callback function pointer.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Reference selection callback generates an event to notify user if a new
 *   reference is selected but automatic switching to it is deferred, because
 *   revertive option is not enabled.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type,
    bcm_tdpll_input_clock_cb callback)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_callback_register(unit, stack_id, type, callback));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Un-register input clock callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] type - callback type.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_input_clock_callback_unregister(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_input_clock_callback_unregister(unit, stack_id, type));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create the output clock
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] pbmp - Port bit-map.
 * \param [in] bandwidth - Bandwidth for the DPLL.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_create(
    int unit,
    int stack_id,
    int clock_index,
    bcm_pbmp_t pbmp,
    bcm_tdpll_dpll_bandwidth_t * bandwidth)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_create(unit, stack_id, clock_index, pbmp, bandwidth));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get output clock enable Boolean
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [out] enable - Output clock enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_enable_get(unit, stack_id, clock_index, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get output clock enable Boolean
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] enable - Output clock enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_enable_set(unit, stack_id, clock_index, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get output-clock (synthesizer) frequency.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] synth_frequency - Synthesizer frequency (Hz).
 * \param [in] tsevent_frequency - TS event frequency (Hz).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_synth_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *synth_frequency,
    uint32 *tsevent_frequency)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_synth_frequency_get
                    (unit, stack_id, clock_index, synth_frequency, tsevent_frequency));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set output-clock (synthesizer) frequency.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] synth_frequency - Synthesizer frequency (Hz).
 * \param [in] tsevent_frequency - TS event frequency (Hz).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_synth_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 synth_frequency,
    uint32 tsevent_frequency)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_synth_frequency_set
                    (unit, stack_id, clock_index, synth_frequency, tsevent_frequency));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get synthesizer derivative-clock frequency.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [out] deriv_frequency - Derivative clock frequency (Hz).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_deriv_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *deriv_frequency)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_deriv_frequency_get(unit, stack_id, clock_index, deriv_frequency));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set synthesizer derivative-clock frequency.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] deriv_frequency - Derivative clock frequency (Hz).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_deriv_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 deriv_frequency)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_deriv_frequency_set(unit, stack_id, clock_index, deriv_frequency));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get holdover configuration data.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] hdata - Holdover configuration data.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_holdover_data_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_data_t * hdata)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_holdover_data_get(unit, stack_id, clock_index, hdata));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set manual holdover frequency correction.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] hfreq - Holdover frequency correction (ppt).
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Manual frequency correction is used if holdover mode is manual
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_holdover_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_frequency_correction_t hfreq)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_holdover_frequency_set(unit, stack_id, clock_index, hfreq));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get holdover mode..
 *   |Manual|Instantaneous|One-Second|Fast Average|Slow Average|
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] hmode - Holdover mode.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Manual frequency correction is used if holdover mode is manual
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_holdover_mode_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t * hmode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_holdover_mode_get(unit, stack_id, clock_index, hmode));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set holdover mode..
 *   |Manual|Instantaneous|One-Second|Fast Average|Slow Average|
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 * \param [in] hmode - Holdover mode.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Manual frequency correction is used if holdover mode is manual
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_holdover_mode_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t hmode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_holdover_mode_set(unit, stack_id, clock_index, hmode));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset holdover frequency calculations.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] clock_index - Output clock index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_output_clock_holdover_reset(
    int unit,
    int stack_id,
    int clock_index)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_output_clock_holdover_reset(unit, stack_id, clock_index));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Execute state machine for Rx ESMC PDU.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] ingress_port  - Ingress port.
 * \param [in] esmc_pdu_data - ESMC PDU.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_rx_state_machine(
    int unit,
    int stack_id,
    int ingress_port,
    bcm_esmc_pdu_data_t * esmc_pdu_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_rx_state_machine(unit, stack_id, ingress_port, esmc_pdu_data));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Execute state machine for Rx ESMC PDU.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] ql - QL.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t * ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_ql_get(unit, stack_id, dpll_index, ql));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set quality level (QL) for ESMC.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] ql - QL.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   Assignment is transient. QL shall be overwritten by subsequent ESMC PDUs
 *   received from a T-DPLL selected reference clock.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_ql_set(unit, stack_id, dpll_index, ql));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get quality level (QL) for ESMC during holdover.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] ql - QL.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_holdover_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t * ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_holdover_ql_get(unit, stack_id, dpll_index, ql));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set quality level (QL) for ESMC during holdover.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] ql - QL.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_holdover_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_holdover_ql_set(unit, stack_id, dpll_index, ql));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get MAC address for ESMC.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] mac - Local port MAC address.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   MAC address is used as source MAC in Tx'd ESMC info and event PDUs. MAC
 *   address is used in discard-from-self logic for reflected PDUs.
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_mac_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t * mac)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_mac_get(unit, stack_id, dpll_index, mac));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set MAC address for ESMC.
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] mac - Local port MAC address.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   MAC address is used as source MAC in Tx'd ESMC info and event PDUs. MAC
 *   address is used in discard-from-self logic for reflected PDUs.
 * \see
 *   * None
 */

int
bcm_dnx_tdpll_esmc_mac_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t * mac)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_mac_set(unit, stack_id, dpll_index, mac));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ESMC PDU receive (Rx) enable state
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] enable - ESMC PDU receive enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_rx_enable_get(
    int unit,
    int stack_id,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_rx_enable_get(unit, stack_id, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ESMC PDU receive (Rx) enable state
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] enable - ESMC PDU receive enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_rx_enable_set(
    int unit,
    int stack_id,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_rx_enable_set(unit, stack_id, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ESMC PDU transmit (Tx) enable state
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] enable - ESMC PDU transit enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_tx_enable_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_tx_enable_get(unit, stack_id, dpll_index, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ESMC PDU transmit (Tx) enable state
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] enable - ESMC PDU transit enable Boolean.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_tx_enable_set(
    int unit,
    int stack_id,
    int dpll_index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_tx_enable_set(unit, stack_id, dpll_index, enable));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port bitmap for ESMC Rx
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [out] pbmp - Rx port bitmap.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_rx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t * pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_rx_portbitmap_get(unit, stack_id, dpll_index, pbmp));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set port bitmap for ESMC Rx
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] pbmp - Rx port bitmap.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_rx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_rx_portbitmap_set(unit, stack_id, dpll_index, pbmp));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port bitmap for ESMC Tx
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] pbmp - Rx port bitmap.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_tx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t * pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_tx_portbitmap_get(unit, stack_id, dpll_index, pbmp));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set port bitmap for ESMC Tx
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] dpll_index - DPLL instance number.
 * \param [in] pbmp - Rx port bitmap.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_esmc_tx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_esmc_tx_portbitmap_set(unit, stack_id, dpll_index, pbmp));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register ESMC PDU Rx callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 * \param [in] rx_cb - ESMC PDU Rx callback function pointer.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_esmc_rx_callback_register(
    int unit,
    int stack_id,
    bcm_esmc_rx_cb rx_cb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_rx_callback_register(unit, stack_id, rx_cb));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Un-register ESMC PDU Rx callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_esmc_rx_callback_unregister(
    int unit,
    int stack_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_rx_callback_unregister(unit, stack_id));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register tdpll event Rx callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_event_callback_register(
    int unit,
    int ptp_id,
    bcm_tdpll_event_cb_f callback)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_event_callback_register(unit, ptp_id, callback));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Un-register tdpll event callback
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_tdpll_event_callback_unregister(
    int unit,
    int ptp_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tdpll_event_callback_unregister(unit, ptp_id));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - ESMC PDU transmit (Tx)
 *
 * \param [in] unit - Unit ID
 * \param [in] stack_id - Stack identifier index.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   *
 * \see
 *   * None
 */
int
bcm_dnx_esmc_tx(
    int unit,
    int stack_id,
    bcm_pbmp_t pbmp,
    bcm_esmc_pdu_data_t * esmc_pdu_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_esmc_tx(unit, stack_id, pbmp, esmc_pdu_data));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

#endif /* defined(INCLUDE_PTP) */
