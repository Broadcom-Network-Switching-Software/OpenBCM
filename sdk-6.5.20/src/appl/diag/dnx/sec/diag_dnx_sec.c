/** \file diag_dnx_sec.c
 * 
 * show diagnostic pack for SEC
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef INCLUDE_XFLOW_MACSEC

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MACSEC

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/parse.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/*bcm*/
#include <bcm/stat.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/property.h>
#include <soc/counter.h>
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/init/init.h>
#include <xflow_macsec_defs.h>
#include <bcm/xflow_macsec.h>
#include <dnx/dnx_sec.h>
#endif /* BCM_DNX_SUPPORT */

/*dcmn*/

/* #include <appl/diag/dcmn/counter.h> */

/* Put your new common defines in this file*/
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include "diag_dnx_sec.h"
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * LOCAL DEFs:
 * {
 */

#define DIAG_DNX_SEC_OPTION_DIRECTION       "direction"
#define DIAG_DNX_SEC_OPTION_TYPE            "type"
#define DIAG_DNX_SEC_OPTION_INSTANCE_ID     "instance"

#define DIAG_DNX_SEC_DIRECTION_ENCRYPT                      0
#define DIAG_DNX_SEC_DIRECTION_DECRYPT                      1

/*
 * LOCAL DEFs:
 * }
 */
/* 
 * Typpedef
 * {
 */
typedef struct diag_dnx_sec_stat_map_s
{
    bcm_xflow_macsec_stat_type_t stat_type;
    char *stat_str;
} diag_dnx_sec_stat_map_t;

static const diag_dnx_sec_stat_map_t stat_type_enum_to_str[] = {

    {bcmXflowMacsecUnctrlPortInOctets, "UnctrlPortInOctets"},
    {bcmXflowMacsecUnctrlPortInOctets, "UnctrlPortInOctets"},
    {bcmXflowMacsecUnctrlPortInUcastPkts, "UnctrlPortInUcastPkts"},
    {bcmXflowMacsecUnctrlPortInMulticastPkts, "UnctrlPortInMulticastPkts"},
    {bcmXflowMacsecUnctrlPortInBroadcastPkts, "UnctrlPortInBroadcastPkts"},
    {bcmXflowMacsecUnctrlPortInDiscards, "UnctrlPortInDiscards"},
    {bcmXflowMacsecUnctrlPortOutOctets, "UnctrlPortOutOctets"},
    {bcmXflowMacsecUnctrlPortOutUcastPkts, "UnctrlPortOutUcastPkts"},
    {bcmXflowMacsecUnctrlPortOutMulticastPkts, "UnctrlPortOutMulticastPkts"},
    {bcmXflowMacsecUnctrlPortOutBroadcastPkts, "UnctrlPortOutBroadcastPkts"},
    {bcmXflowMacsecUnctrlPortOutErrors, "UnctrlPortOutErrors"},
    {bcmXflowMacsecCtrlPortInOctets, "CtrlPortInOctets"},
    {bcmXflowMacsecCtrlPortInUcastPkts, "CtrlPortInUcastPkts"},
    {bcmXflowMacsecCtrlPortInMulticastPkts, "CtrlPortInMulticastPkts"},
    {bcmXflowMacsecCtrlPortInBroadcastPkts, "CtrlPortInBroadcastPkts"},
    {bcmXflowMacsecCtrlPortInDiscards, "CtrlPortInDiscards"},
    {bcmXflowMacsecCtrlPortInErrors, "CtrlPortInErrors"},
    {bcmXflowMacsecCtrlPortOutOctets, "CtrlPortOutOctets"},
    {bcmXflowMacsecCtrlPortOutUcastPkts, "CtrlPortOutUcastPkts"},
    {bcmXflowMacsecCtrlPortOutMulticastPkts, "CtrlPortOutMulticastPkts"},
    {bcmXflowMacsecCtrlPortOutBroadcastPkts, "CtrlPortOutBroadcastPkts"},
    {bcmXflowMacsecCtrlPortOutErrors, "CtrlPortOutErrors"},
    {bcmXflowMacsecSecyStatsTxUntaggedPkts, "SecyStatsTxUntaggedPkts"},
    {bcmXflowMacsecSecyStatsTxTooLongPkts, "SecyStatsTxTooLongPkts"},
    {bcmXflowMacsecSecyStatsRxUntaggedPkts, "SecyStatsRxUntaggedPkts"},
    {bcmXflowMacsecSecyStatsRxNoTagPkts, "SecyStatsRxNoTagPkts"},
    {bcmXflowMacsecSecyStatsRxBadTagPkts, "SecyStatsRxBadTagPkts"},
    {bcmXflowMacsecSecyStatsRxUnknownSCIPkts, "SecyStatsRxUnknownSCIPkts"},
    {bcmXflowMacsecSecyStatsRxNoSCIPkts, "SecyStatsRxNoSCIPkts"},
    {bcmXflowMacsecSecyStatsRxOverrunPkts, "SecyStatsRxOverrunPkts"},
    {bcmXflowMacsecSecyTxSCStatsProtectedPkts, "SecyTxSCStatsProtectedPkts"},
    {bcmXflowMacsecSecyTxSCStatsEncryptedPkts, "SecyTxSCStatsEncryptedPkts"},
    {bcmXflowMacsecSecyTxSCStatsOctetsProtected, "SecyTxSCStatsOctetsProtected"},
    {bcmXflowMacsecSecyTxSCStatsOctetsEncrypted, "SecyTxSCStatsOctetsEncrypted"},
    {bcmXflowMacsecSecyRxSCStatsUnusedSAPkts, "SecyRxSCStatsUnusedSAPkts"},
    {bcmXflowMacsecSecyRxSCStatsNotUsingSAPkts, "SecyRxSCStatsNotUsingSAPkts"},
    {bcmXflowMacsecSecyRxSCStatsLatePkts, "SecyRxSCStatsLatePkts"},
    {bcmXflowMacsecSecyRxSCStatsNotValidPkts, "SecyRxSCStatsNotValidPkts"},
    {bcmXflowMacsecSecyRxSCStatsInvalidPkts, "SecyRxSCStatsInvalidPkts"},
    {bcmXflowMacsecSecyRxSCStatsDelayedPkts, "SecyRxSCStatsDelayedPkts"},
    {bcmXflowMacsecSecyRxSCStatsUncheckedPkts, "SecyRxSCStatsUncheckedPkts"},
    {bcmXflowMacsecSecyRxSCStatsOKPkts, "SecyRxSCStatsOKPkts"},
    {bcmXflowMacsecSecyRxSCStatsOctetsValidated, "SecyRxSCStatsOctetsValidated"},
    {bcmXflowMacsecSecyRxSCStatsOctetsDecrypted, "SecyRxSCStatsOctetsDecrypted"},
    {bcmXflowMacsecSecyTxSAStatsProtectedPkts, "SecyTxSAStatsProtectedPkts"},
    {bcmXflowMacsecSecyTxSAStatsEncryptedPkts, "SecyTxSAStatsEncryptedPkts"},
    {bcmXflowMacsecSecyRxSAStatsUnusedSAPkts, "SecyRxSAStatsUnusedSAPkts"},
    {bcmXflowMacsecSecyRxSAStatsNotUsingSAPkts, "SecyRxSAStatsNotUsingSAPkts"},
    {bcmXflowMacsecSecyRxSAStatsNotValidPkts, "SecyRxSAStatsNotValidPkts"},
    {bcmXflowMacsecSecyRxSAStatsInvalidPkts, "SecyRxSAStatsInvalidPkts"},
    {bcmXflowMacsecSecyRxSAStatsOKPkts, "SecyRxSAStatsOKPkts"},
    {bcmXflowMacsecInMgmtPkts, "InMgmtPkts"},
    {bcmXflowMacsecFlowTcamHitCntr, "FlowTcamHitCntr"},
    {bcmXflowMacsecFlowTcamMissCntr, "FlowTcamMissCntr"},
    {bcmXflowMacsecScTcamHitCntr, "ScTcamHitCntr"},
    {bcmXflowMacsecScTcamMissCntr, "ScTcamMissCntr"},
    {bcmXflowMacsecOutMgmtPkts, "OutMgmtPkts"},
    {bcmXflowMacsecInPacketDropCntr, "InPacketDropCntr"},
    {bcmXflowMacsecOutPacketDropCntr, "OutPacketDropCntr"},
    {bcmXflowMacsecBadOlpHdrCntr, "BadOlpHdrCntr"},
    {bcmXflowMacsecBadSvtagHdrCntr, "BadSvtagHdrCntr"},
    {bcmXflowMacsecUnctrlPortInKayPkts, "UnctrlPortInKayPkts"},
    {bcmXflowMacsecIPsecCtrlPortDummyPkts, "IPsecCtrlPortDummyPkts"},
    {bcmXflowMacsecIPsecCtrlPortIPLengthMismatch, "IPsecCtrlPortIPLengthMismatch"},
    {bcmXflowMacsecIPsecTxOutErrors, "IPsecTxOutErrors"},
    {bcmXflowMacsecIPsecUnctrlPortInIkePkts, "IPsecUnctrlPortInIkePkts"},
    {bcmXflowMacsecIPsecSecyRxNoSPIPkts, "IPsecSecyRxNoSPIPkts"},
    {bcmXflowMacsecIPsecSecyRxIPFragmentsSetPkts, "IPsecSecyRxIPFragmentsSetPkts"},
    {bcmXflowMacsecIPSecSecyRxIllegalNxtHdrPkts, "IPSecSecyRxIllegalNxtHdrPkts"},
    {bcmXflowMacsecIPSecRxNoSAPkts, "IPSecRxNoSAPkts"},
    {bcmXflowMacsecIPSecRxSADummyPkts, "IPSecRxSADummyPkts"},
    {bcmXflowMacsecIPSecRxSAPadMismatchPkts, "IPSecRxSAPadMismatchPkts"},

};

typedef enum diag_dnx_sec_stat_type_e
{
    DIAG_DNX_SEC_STAT_TYPE_SC = 0,
    DIAG_DNX_SEC_STAT_TYPE_SA = 1,
    DIAG_DNX_SEC_STAT_TYPE_SUBPORT = 2,
    DIAG_DNX_SEC_STAT_TYPE_FLOW = 3,
    DIAG_DNX_SEC_TYPE_NOF = 4,
} diag_dnx_sec_stat_type_t;

/**
 * Enum for option requiring
 */
sh_sand_enum_t Sec_stat_type_enum_table[] = {
    {"sc", DIAG_DNX_SEC_STAT_TYPE_SC, "Secure Channel"},
    {"sa", DIAG_DNX_SEC_STAT_TYPE_SA, "Secure Association"},
    {"subport", DIAG_DNX_SEC_STAT_TYPE_SUBPORT, "Subport Number"},
    {"flow", DIAG_DNX_SEC_STAT_TYPE_FLOW, "Flow Id"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' field groups
 */
sh_sand_enum_t Sec_instance_enum_table[] = {
    {"MIN", 0, "Minimal value for Sec Instance id"},
    {"MAX", 0, "Maximal value for Sec Instance", "DNX_DATA.macsec.general.macsec_nof-1"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' field groups
 */
sh_sand_enum_t Sec_direction_enum_table[] = {
    {"Encrypt", DIAG_DNX_SEC_DIRECTION_ENCRYPT, "Encrypt (Egress) Direction"},
    {"Decrypt", DIAG_DNX_SEC_DIRECTION_DECRYPT, "Decrypt (Ingress) Direction"},
    {NULL}
};

bcm_xflow_macsec_stat_type_t stat_type_list_encrypt_array[DIAG_DNX_SEC_TYPE_NOF][bcmXflowMacsecStatTypeCount] = {
     /*SC*/ {xflowMacsecSecyTxSCStatsProtectedPkts, xflowMacsecSecyTxSCStatsEncryptedPkts,
             xflowMacsecSecyTxSCStatsOctetsProtected, xflowMacsecSecyTxSCStatsOctetsEncrypted},
     /*SA*/ {xflowMacsecSecyTxSAStatsEncryptedPkts, xflowMacsecSecyTxSAStatsProtectedPkts},
     /*SP*/ {xflowMacsecUnctrlPortOutOctets, xflowMacsecUnctrlPortOutUcastPkts, xflowMacsecUnctrlPortOutMulticastPkts,
             xflowMacsecUnctrlPortOutBroadcastPkts, xflowMacsecUnctrlPortOutErrors, xflowMacsecCtrlPortOutOctets,
             xflowMacsecCtrlPortOutUcastPkts, xflowMacsecCtrlPortOutMulticastPkts, xflowMacsecCtrlPortOutBroadcastPkts,
             xflowMacsecCtrlPortOutErrors, xflowMacsecSecyStatsTxUntaggedPkts, xflowMacsecSecyStatsTxTooLongPkts,
             xflowMacsecOutMgmtPkts, xflowMacsecIPsecCtrlPortDummyPkts, xflowMacsecIPsecCtrlPortIPLengthMismatch,
             xflowMacsecIPsecTxOutErrors},
    /*
     * Flow
     */
};

bcm_xflow_macsec_stat_type_t stat_type_list_decrypt_array[DIAG_DNX_SEC_TYPE_NOF][bcmXflowMacsecStatTypeCount] = {
     /*SC*/ {xflowMacsecSecyRxSCStatsUnusedSAPkts, xflowMacsecSecyRxSCStatsNotUsingSAPkts,
             xflowMacsecSecyRxSCStatsLatePkts, xflowMacsecSecyRxSCStatsNotValidPkts,
             xflowMacsecSecyRxSCStatsInvalidPkts, xflowMacsecSecyRxSCStatsDelayedPkts,
             xflowMacsecSecyRxSCStatsUncheckedPkts, xflowMacsecSecyRxSCStatsOKPkts,
             xflowMacsecSecyRxSCStatsOctetsValidated, xflowMacsecSecyRxSCStatsOctetsDecrypted,
             xflowMacsecScTcamHitCntr},
     /*SA*/ {xflowMacsecSecyRxSAStatsUnusedSAPkts, xflowMacsecSecyRxSAStatsNotUsingSAPkts,
             xflowMacsecSecyRxSAStatsNotValidPkts, xflowMacsecSecyRxSAStatsInvalidPkts, xflowMacsecSecyRxSAStatsOKPkts,
             xflowMacsecIPSecRxNoSAPkts},
     /*SP*/ {xflowMacsecUnctrlPortInOctets, xflowMacsecUnctrlPortInUcastPkts, xflowMacsecUnctrlPortInMulticastPkts,
             xflowMacsecUnctrlPortInBroadcastPkts, xflowMacsecUnctrlPortInKayPkts, xflowMacsecCtrlPortInOctets,
             xflowMacsecCtrlPortInUcastPkts, xflowMacsecCtrlPortInMulticastPkts, xflowMacsecCtrlPortInBroadcastPkts,
             xflowMacsecCtrlPortInDiscards, xflowMacsecCtrlPortInErrors, xflowMacsecSecyStatsRxUntaggedPkts,
             xflowMacsecSecyStatsRxNoTagPkts, xflowMacsecSecyStatsRxBadTagPkts, xflowMacsecSecyStatsRxUnknownSCIPkts,
             xflowMacsecSecyStatsRxNoSCIPkts, xflowMacsecInMgmtPkts, xflowMacsecIPsecUnctrlPortInIkePkts,
             xflowMacsecIPsecSecyRxNoSPIPkts, xflowMacsecIPsecSecyRxIPFragmentsSetPkts,
             xflowMacsecIPSecSecyRxIllegalNxtHdrPkts},
    /*
     * Flow
     */ {xflowMacsecFlowTcamHitCntr},

};

/* 
 * }
 */
/*
* LOCAL Structures:
* {
*/

/**
 * \brief
 *   Structure used as container for output of diag_dnxc_show_counters_controlled_counter_info_get()
 *   It has elements for the name of the counter, the name of the port, the value (64 bits),
 *   the rate (requiring two consequent invocations) and indication on whether counter has changed
 *   (requiring two consequent invocations)
 */
typedef struct diag_dnx_sec_stat_info_s
{
    char *cname;
    int macsec_id;
    uint64 value;
    int is_changed;
} diag_dnx_sec_stat_info_t;

/*
 * LOCAL Structures:
 * }
 */
/*
 * LOCAL FUNCTIONs:
 * {
 */

/**
 *  'Help' description for Context display (shell commands).
 */
static sh_sand_man_t dnx_sec_stat_show_man = {
    .brief = "Display all Sec (MACSec, IPSec) MIB statistics",
    .full = "'Display Sec (MACSec, IPSec) MIB statistics, according to direction and SEC entity type.\r\n",
    .synopsis = "[direction= <'encrypt'|'decrypt'>] [type=<'sc'|'sa'|'subport'|'flow'] [instance=<instance_id>]",
    .examples = "direction=encrypt type=sc instance=5",
};
static sh_sand_option_t dnx_sec_stat_show_options[] = {
    {DIAG_DNX_SEC_OPTION_DIRECTION, SAL_FIELD_TYPE_UINT32, "SEC direction", "encrypt-decrypt",
     (void *) Sec_direction_enum_table, "Encrypt-Decrypt"},
    {DIAG_DNX_SEC_OPTION_TYPE, SAL_FIELD_TYPE_UINT32, "SEC entity type", "sc-flow", (void *) Sec_stat_type_enum_table,
     "sc-flow"},
    {DIAG_DNX_SEC_OPTION_INSTANCE_ID, SAL_FIELD_TYPE_UINT32, "SEC Instance Id", "MIN-MAX",
     (void *) Sec_instance_enum_table, "MIN-MAX"},
    {NULL}
};

static sh_sand_man_t dnx_sec_stat_clear_man = {
    .brief = "Clear all Sec (MACSec, IPSec) MIB statistics",
    .full = "Clear all Sec (MACSec, IPSec) MIB statistics.\r\n",
    .synopsis = "",
    .examples = "",
};

/* Init diag_counter_info_t structure */
void
diag_dnx_sec_counter_info_t_init(
    diag_dnx_sec_stat_info_t * stat_info)
{

    COMPILER_64_SET(stat_info->value, 0, 0);
    stat_info->is_changed = 0;
    stat_info->cname = NULL;

}

/** see header file */
shr_error_e
sh_cmd_is_sec_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_is(unit, DNX_DEVICE))
    {
        /**
         * if init is not done, return that SEC is available.
         * this is done to prevent access to DNX-Data before it is init
         */
        if (!dnx_init_is_init_done_get(unit))
        {
            SHR_EXIT();
        }

        /** Not supported for devices without flexe support */
        if (!dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_macsec_supported))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
exit:
    SHR_FUNC_EXIT;
}

int
diag_dnx_sec_stat_list_get(
    int unit,
    int direction,
    diag_dnx_sec_stat_type_t type,
    bcm_xflow_macsec_stat_type_t * stat_type_list,
    int *num_of_instances)
{

    SHR_FUNC_INIT_VARS(unit);

    if (direction == DIAG_DNX_SEC_DIRECTION_ENCRYPT)
    {
        sal_memcpy(stat_type_list, stat_type_list_encrypt_array[type], sizeof(stat_type_list_encrypt_array[type]));
    }
    else
    {
        sal_memcpy(stat_type_list, stat_type_list_decrypt_array[type], sizeof(stat_type_list_decrypt_array[type]));
    }

    switch (type)
    {
        case DIAG_DNX_SEC_STAT_TYPE_SC:
            *num_of_instances =
                (direction ==
                 DIAG_DNX_SEC_DIRECTION_DECRYPT) ? dnx_data_macsec.
                ingress.secure_channel_nof_get(unit) : dnx_data_macsec.egress.secure_channel_nof_get(unit);
            break;
        case DIAG_DNX_SEC_STAT_TYPE_SA:
            *num_of_instances =
                (direction ==
                 DIAG_DNX_SEC_DIRECTION_DECRYPT) ? dnx_data_macsec.ingress.secure_assoc_nof_get(unit) : dnx_data_macsec.
                egress.secure_assoc_nof_get(unit);
            break;
        case DIAG_DNX_SEC_STAT_TYPE_SUBPORT:
            *num_of_instances =
                (direction ==
                 DIAG_DNX_SEC_DIRECTION_DECRYPT) ? dnx_data_macsec.ingress.policy_nof_get(unit) : dnx_data_macsec.
                egress.secure_channel_nof_get(unit);
            break;
        case DIAG_DNX_SEC_STAT_TYPE_FLOW:
            *num_of_instances =
                (direction == DIAG_DNX_SEC_DIRECTION_DECRYPT) ? dnx_data_macsec.ingress.flow_nof_get(unit) : 0;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_INTERNAL, "Internal Error - stat type");
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-ON* */
/**
 * \brief - display device counters
 */
static shr_error_e
cmd_dnx_sec_stat_show(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 flags = 0;
    uint32 direction, type, instance_id, instance_id_lower, instance_id_upper, handle_id, stat_handle_id;
    uint32 direction_lower, direction_upper, type_lower, type_upper;
    uint8 is_allocated = 0, add_row = FALSE, stat_for_macsec, stat_for_ipsec;
    int i, hw_index, num_of_instances, is_ipsec;
    diag_dnx_sec_stat_info_t stat_info;
    bcm_xflow_macsec_stat_type_t stat_type_list[bcmXflowMacsecStatTypeCount] = { bcmXflowMacsecStatTypeInvalid };
    bcm_xflow_macsec_handle_info_t handle_info;
    char value_str[50];
    VOL int *counter_interval_p;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init counter info structure
     */
    diag_dnx_sec_counter_info_t_init(&stat_info);

    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));

    if (*counter_interval_p != 0)
    {
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
    }
    else
    {
        dnx_xflow_macsec_counters_collect(unit);
        flags |= BCM_XFLOW_MACSEC_STAT_SYNC_DISABLE;
    }

    /*
     * Get parameters
     */
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_SEC_OPTION_DIRECTION, direction_lower, direction_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_SEC_OPTION_TYPE, type_lower, type_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_SEC_OPTION_INSTANCE_ID, instance_id_lower, instance_id_upper);

    PRT_TITLE_SET("MACSEC/IPSEC STAT for direction %s-%s type %s-%s SEC instance %d-%d",
                  Sec_direction_enum_table[direction_lower].string,
                  Sec_direction_enum_table[direction_upper].string,
                  Sec_stat_type_enum_table[type_lower].string,
                  Sec_stat_type_enum_table[type_upper].string, instance_id_lower, instance_id_upper);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stat Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MACSEC ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "HW Id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stat Value");

        /** Iterate over all MACSec Instance Ids */
    for (instance_id = instance_id_lower; instance_id <= instance_id_upper; instance_id++)
    {
        add_row = FALSE;
        for (direction = direction_lower; direction <= direction_upper; direction++)
        {
            for (type = type_lower; type <= type_upper; type++)
            {

                SHR_IF_ERR_EXIT(diag_dnx_sec_stat_list_get(unit, direction, type, stat_type_list, &num_of_instances));

                for (i = 0; stat_type_list[i] != bcmXflowMacsecStatTypeInvalid; i++)
                {
                    for (hw_index = 0; hw_index < num_of_instances; hw_index++)
                    {
                        handle_info.dir =
                            (direction == DIAG_DNX_SEC_DIRECTION_ENCRYPT) ? XFLOW_MACSEC_ENCRYPT : XFLOW_MACSEC_DECRYPT;
                        handle_info.type =
                            (type ==
                             DIAG_DNX_SEC_STAT_TYPE_SUBPORT) ? ((direction ==
                                                                 DIAG_DNX_SEC_DIRECTION_ENCRYPT) ?
                                                                xflowMacsecIdTypeSecureChan : xflowMacsecIdTypePolicy) :
                            type;
                        handle_info.instance_id = instance_id;
                        handle_info.hw_index = hw_index;

                        /*
                         * Create the handle
                         */
                        SHR_IF_ERR_EXIT(bcm_xflow_macsec_handle_create(unit, &handle_info, &handle_id));

                        /*
                         * Create the handle
                         */
                        SHR_IF_ERR_EXIT(dnx_xflow_macsec_handle_is_valid(unit, handle_id, instance_id,
                                                                         DNX_SEC_HANDLE_IGNORE_IS_IPSEC,
                                                                         &is_allocated));

                        if (!is_allocated)
                        {
                            continue;
                        }

                        SHR_IF_ERR_EXIT(dnx_xflow_macsec_handle_is_ipsec_get(unit, handle_id, &is_ipsec));
                        dnx_xflow_macsec_stat_supported_types_get(unit, instance_id, stat_type_list[i],
                                                                  &stat_for_macsec, &stat_for_ipsec);
                        if ((!is_ipsec && !stat_for_macsec) || (is_ipsec && !stat_for_ipsec))
                        {
                            continue;
                        }

                        stat_handle_id = handle_id;
                        if (type == DIAG_DNX_SEC_STAT_TYPE_SUBPORT)
                        {
                            SHR_IF_ERR_EXIT(bcm_xflow_macsec_subport_id_get(unit, handle_id, &stat_handle_id));
                        }

                            /** Get the stat value from the SW DB. */
                        SHR_IF_ERR_EXIT(bcm_xflow_macsec_stat_get
                                        (unit, flags, stat_handle_id, stat_type_list[i], &stat_info.value));

                        if (!COMPILER_64_IS_ZERO(stat_info.value))
                        {
                            add_row = TRUE;
                            format_uint64_decimal(value_str, stat_info.value, ',');
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", stat_type_enum_to_str[stat_type_list[i]].stat_str);
                            PRT_CELL_SET("%d", instance_id);
                            PRT_CELL_SET("%d", hw_index);
                            PRT_CELL_SET("%s", value_str);

                        }       /* if */
                    }   /* for (hw_index) */
                }       /* (stat_type_list) */
            }   /* for (type) */
        }       /* for (direction) */

        if (add_row)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }
    }   /* for (instance_id) */

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnx_sec_stat_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 direction, type, instance_id, handle_id, stat_handle_id;
    uint8 is_allocated = 0;
    int i, hw_index, num_of_instances;
    bcm_xflow_macsec_stat_type_t stat_type_list[bcmXflowMacsecStatTypeCount] = { bcmXflowMacsecStatTypeInvalid };
    bcm_xflow_macsec_handle_info_t handle_info;
    SHR_FUNC_INIT_VARS(unit);

    for (instance_id = 0; instance_id < dnx_data_macsec.general.macsec_nof_get(unit); instance_id++)
    {
        for (direction = DIAG_DNX_SEC_DIRECTION_ENCRYPT; direction <= DIAG_DNX_SEC_DIRECTION_DECRYPT; direction++)
        {
            for (type = 0; type < DIAG_DNX_SEC_TYPE_NOF; type++)
            {

                SHR_IF_ERR_EXIT(diag_dnx_sec_stat_list_get(unit, direction, type, stat_type_list, &num_of_instances));

                for (i = 0; stat_type_list[i] != bcmXflowMacsecStatTypeInvalid; i++)
                {
                    for (hw_index = 0; hw_index < num_of_instances; hw_index++)
                    {
                        handle_info.dir =
                            (direction == DIAG_DNX_SEC_DIRECTION_ENCRYPT) ? XFLOW_MACSEC_ENCRYPT : XFLOW_MACSEC_DECRYPT;
                        handle_info.type =
                            (type ==
                             DIAG_DNX_SEC_STAT_TYPE_SUBPORT) ? ((direction ==
                                                                 DIAG_DNX_SEC_DIRECTION_ENCRYPT) ?
                                                                xflowMacsecIdTypeSecureChan : xflowMacsecIdTypePolicy) :
                            type;
                        handle_info.instance_id = instance_id;
                        handle_info.hw_index = hw_index;

                        /*
                         * Create the handle
                         */
                        SHR_IF_ERR_EXIT(bcm_xflow_macsec_handle_create(unit, &handle_info, &handle_id));

                        /*
                         * Create the handle
                         */
                        SHR_IF_ERR_EXIT(dnx_xflow_macsec_handle_is_valid(unit, handle_id, instance_id,
                                                                         DNX_SEC_HANDLE_IGNORE_IS_IPSEC,
                                                                         &is_allocated));

                        if (!is_allocated)
                        {
                            continue;
                        }
                        stat_handle_id = handle_id;
                        if (type == DIAG_DNX_SEC_STAT_TYPE_SUBPORT)
                        {
                            SHR_IF_ERR_EXIT(bcm_xflow_macsec_subport_id_get(unit, handle_id, &stat_handle_id));
                        }

                            /** clear the stat value  */
                        SHR_IF_ERR_EXIT(dnx_xflow_macsec_stat_clear(unit, stat_handle_id));
                    }   /* (hw_index) */
                }       /* (stat_type_list) */
            }   /* Type */
        }       /* direction */
    }   /* instance_id */

exit:
    SHR_FUNC_EXIT;
}
static sh_sand_man_t sh_dnx_sec_stat_man = {
    .brief = "Presenting operations related to SEC MIB statatistics",
    .full = NULL
};

sh_sand_cmd_t sh_dnx_sec_stat_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"show", cmd_dnx_sec_stat_show, NULL, dnx_sec_stat_show_options, &dnx_sec_stat_show_man},
    {"clear", cmd_dnx_sec_stat_clear, NULL, NULL, &dnx_sec_stat_clear_man},
    {NULL}
};

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
sh_sand_man_t sh_dnx_sec_man = {
    .brief = "SEC commands",
    .full = "The dispatcher for the different SEC diagnostics commands\n"
};

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnx_sec_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"stat",  NULL,             sh_dnx_sec_stat_cmds,         NULL, &sh_dnx_sec_stat_man},
    {NULL}
};


/* *INDENT-ON* */
#else
typedef int diag_dnx_sec_not_empty;     /* Make ISO compilers happy. */
#endif /* INCLUDE_XFLOW_MACSEC */
