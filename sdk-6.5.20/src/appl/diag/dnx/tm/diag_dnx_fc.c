/** \file diag_dnx_fc.c
 * 
 * DNX TM Flow Control diagnostics 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/fabric/fabric.h>
/** soc */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
/** allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>
/** local */
#include "diag_dnx_fc.h"

/**
 * \brief
 * This function encodes a enum field. encode enum means
 * translate its SW value to HW value
 */
shr_error_e dbal_field_types_encode_enum(
    int unit,
    dbal_field_types_defs_e field_type,
    uint32 sw_field_val,
    uint32 *hw_field_val);

/* 
 * --------------------------------------------------------------------------
 * dnx tm flow control
 * --------------------------------------------------------------------------
 */

/*
 * Define the short strings for each FC Type.
 */
/* *INDENT-OFF* }*/
static sh_sand_enum_t sand_diag_fc_type_enum[] = {
   /**String          Value                        Description */
    {"GLB_RSC",       DNX_FC_TYPE_GLB_RSC,         "Generation: Present signals from Global Resources' thresholds to CFC indicating to generate FC.In this case index is not used. Instead the parameter 'resource' is used to represent the type of Global Resource to be presented."},
    {"AF_NIF",        DNX_FC_TYPE_AF_NIF,          "Generation: Present signals from  Almost full NIF FIFO's thresholds to CFC indicating to generate FC. In this case index is used to represent the NIF port, possible indexes 0-(Number of NIF ports)"},
    {"AF_ILKN",       DNX_FC_TYPE_ILKN_AF_NIF,     "Generation: Present signals from ILKN Almost Full NIF , possible indexes 0-(Number of ILKN interfaces)"},
    {"ILKN_LLFC",     DNX_FC_TYPE_LLFC_ILKN,       "Generation: Present signals from CFC to an ILKN interface in order to transmit LLFC , possible indexes 0-(Number of ILKN interfaces)"},
    {"LLFC_NIF",      DNX_FC_TYPE_LLFC_NIF,        "Generation: Present signals from CFC to NIF indicating to generate LLFC. In this case index is used to represent the NIF port, possible indexes 0-(Number of NIF ports)"},
    {"PFC_NIF",       DNX_FC_TYPE_PFC_NIF,         "Generation: Present signals from CFC to NIF indicating to generate PFC. In this case index is used to represent the NIF port, possible indexes 0-(Number of NIF ports x Number of NIF priorities)"},
    {"VSQ_A",         DNX_FC_TYPE_VSQ_A,           "Generation: Present signals from VSQ A's thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs A)"},
    {"VSQ_B",         DNX_FC_TYPE_VSQ_B,           "Generation: Present signals from VSQ B's thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs B)"},
    {"VSQ_C",         DNX_FC_TYPE_VSQ_C,           "Generation: Present signals from VSQ C's thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs C)"},
    {"VSQ_D",         DNX_FC_TYPE_VSQ_D,           "Generation: Present signals from VSQ D's thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs D)"},
    {"VSQ_LLFC",      DNX_FC_TYPE_VSQ_LLFC,        "Generation: Present signals from VSQ LLFC(VSQ E)'s thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs E)"},
    {"VSQ_PFC",       DNX_FC_TYPE_VSQ_PFC,         "Generation: Present signals from VSQ PFC's thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs F)"},
    {"PFC_EGQ",       DNX_FC_TYPE_FC_EGQ,          "Reception: Present PFC indications from CFC received by the EGQ. In this case index is used to represent the Q-pair of the EGQ interface, possible indexes 0-(Number of EGQ Q-pairs)"},
    {"NIF_PFC",       DNX_FC_TYPE_NIF_PFC,         "Reception: Present PFC indications from NIF received by the CFC. In this case index is used to represent the priority of the NIF port, possible indexes 0-(Number of NIF ports x Number of NIF priorities)"},
    {"OOB_SINGLE",    DNX_FC_TYPE_OOB_SINGLE,      "Reception: Present Q-pair based FC (to stop a single Q-pair) indications from SPI OOB calendar received by the EGQ. In this case index is used to represent the Q-pair of the EGQ interface, possible indexes 0-(Number of EGQ Q-pairs)"},
    {"OOB_PORT_BASED",DNX_FC_TYPE_OOB_PORT_BASED,  "Reception: Present Q-pair based FC (to stop all Q-pairs of a port) indications from SPI OOB calendar received by the EGQ. In this case index is used to represent the Q-pair of the EGQ interface, possible indexes 0-(Number of EGQ Q-pairs)"},
    {"OOB_LLFC",      DNX_FC_TYPE_OOB_LLFC,        "Reception: Present Link level FC (to stop a NIF port)  indications from SPI OOB calendar received by the NIF. In this case index is used to represent the NIF port, possible indexes 0-(Number of NIF ports + CMIC ports)"},
    {"GEN_BMP_OOB",   DNX_FC_TYPE_OOB_GEN_BMP,     "Reception: Present which of the Generic PFC bitmaps are used by the SPI OOB calendar to send FC to the EGQ. In this case index is used to represent the Generic Bitmap index, possible indexes 0-(Number of Generic Bitmaps)"},
    {"ILKN_OOB",      DNX_FC_TYPE_ILKN_OOB,        "Reception: Present FC received by ILKN OOB calendar. In this case index is used to represent the calendar entry, possible indexes 0-(OOB calendar length)"},
    {"PP_COE",        DNX_FC_TYPE_PP_COE,          "Reception: Present FC received by the PP COE calendar. In this case index is used to represent the calendar entry, possible indexes indexes 0-(COE calendar length)"},
    {"STOP_NIF",      DNX_FC_TYPE_NIF_STOP_PM,     "Reception: Present FC received by NIF port in order to stop the PM, possible indexes 0-(Number of NIF ports)"},
    {"STOP_ILKN",     DNX_FC_TYPE_ILKN_STOP_PM,    "Reception: Present FC received by ILKN interface in order to stop the PM, possible indexes 0-(Number of ILKN interfaces)"},
    {"INB_ILKN",      DNX_FC_TYPE_INB_ILKN,        "Reception: Present FC received by Inband ILKN calendar. In this case index is used to represent the calendar entry, possible indexes 0-(Inband ILKN calendar length)"},
    {"ALL",           DNX_FC_TYPE_COUNT,           "All types"},
    {NULL}
};

static sh_sand_enum_t sand_diag_fc_nif_type_enum[] = {
   /**String            Value                               Description */
    {"LLFC_TO_PM",      DNX_FC_NIF_TYPE_LLFC_TO_PM,         "LLFC generation: Present indications that LLFC is generated to Port Macro. Port must he ETHU type."},
    {"LLFC_FROM_CFC",   DNX_FC_NIF_TYPE_LLFC_FROM_CFC,      "LLFC generation: Present signals from CFC indicating to generate LLFC. Port must he ETHU type."},
    {"PFC_TO_PM",       DNX_FC_NIF_TYPE_PFC_TO_PM,          "PFC generation: Present indications that PFC is generated to Port Macro. Port must he ETHU type."},
    {"PFC_FROM_CFC",    DNX_FC_NIF_TYPE_PFC_FROM_CFC,       "PFC generation: Present signals from CFC indicating to generate PFC. Port must he ETHU type."},
    {"PFC_FROM_QMLF",   DNX_FC_NIF_TYPE_PFC_FROM_QMLF,      "PFC generation: Present signals from QMLF(Almost full NIF) indicating to generate PFC. Port must he ETHU type."},
    {"LLFC_FROM_QMLF",  DNX_FC_NIF_TYPE_LLFC_FROM_QMLF,     "LLFC generation: Present signals from QMLF(Almost full NIF) indicating to generate LLFC. Port must he ETHU type."},
    {"STOP_TO_PM",      DNX_FC_NIF_TYPE_STOP_TO_PM,         "LLFC reception: Present indications that LLFC signal to stop transmitting was sent to Port Macro. Port must he ETHU type."},
    {"STOP_FROM_CFC",   DNX_FC_NIF_TYPE_STOP_FROM_CFC,      "LLFC reception: Present received LLFC signals from CFC indicating to stop transmitting. Port must he ETHU type."},
    {"PFC_FROM_PM",     DNX_FC_NIF_TYPE_PFC_FROM_PM,        "PFC reception: Present received PFC signals from Port Macro. Port must he ETHU type."},
    {"PFC_TO_CFC",      DNX_FC_NIF_TYPE_PFC_TO_CFC,         "PFC reception: Present PFC signals sent to CFC. Port must he ETHU type."},
    {"LLFC_TO_ILKN",    DNX_FC_NIF_TYPE_LLFC_TO_ILKN,       "LLFC reception: Present an LLFC signal to stop transmitting that was received from CFC. Port must he ILKN type."},
    {"ALL",             DNX_FC_NIF_TYPE_COUNT,              "All types"},
    {NULL}
};

static sh_sand_enum_t sand_diag_fc_direction_enum[] = {
   /**String      Value                                Description */
    {"GEN",       DNX_FC_DIRECTION_GENERATION,         "Generation of FC"},
    {"REC",       DNX_FC_DIRECTION_RECEPTION,          "Reception of FC"},
    {NULL}
};

static sh_sand_enum_t     sand_diag_fc_index_to_glb_rsc_enum[] = {
   /**String                      Value                            Description */
    {"DRAM_LOW",                  DNX_FC_DRAM_TOTAL_LOW_PRIO,      "DRAM Total Low priority"},
    {"DRAM_HIGH",                 DNX_FC_DRAM_TOTAL_HIGH_PRIO,     "DRAM Total High priority"},
    {"SRAM_LOW",                  DNX_FC_SRAM_TOTAL_LOW_PRIO,      "SRAM Total Low priority"},
    {"SRAM_HIGH",                 DNX_FC_SRAM_TOTAL_HIGH_PRIO,     "SRAM Total High priority"},
    {"DRAM_P0_LOW",               DNX_FC_DRAM_POOL_0_LOW_PRIO,     "DRAM Pool 0 Low priority"},
    {"DRAM_P0_HIGH",              DNX_FC_DRAM_POOL_0_HIGH_PRIO,    "DRAM Pool 0 High priority"},
    {"DRAM_P1_LOW",               DNX_FC_DRAM_POOL_1_LOW_PRIO,     "DRAM Pool 1 Low priority"},
    {"DRAM_P1_HIGH",              DNX_FC_DRAM_POOL_1_HIGH_PRIO,    "DRAM Pool 1 High priority"},
    {"SRAM_P0_LOW",               DNX_FC_SRAM_POOL_0_LOW_PRIO,     "SRAM Pool 0 Low priority"},
    {"SRAM_P0_HIGH",              DNX_FC_SRAM_POOL_0_HIGH_PRIO,    "SRAM Pool 0 High priority"},
    {"SRAM_P1_LOW",               DNX_FC_SRAM_POOL_1_LOW_PRIO,     "SRAM Pool 1 Low priority"},
    {"SRAM_P1_HIGH",              DNX_FC_SRAM_POOL_1_HIGH_PRIO,    "SRAM Pool 1 High priority"},
    {"HDRM",                      DNX_FC_HDRM_TOTAL_HIGH_PRIO,     "HDRM Total High priority (only this priority supported)"},
    {"ALL",                       DNX_FC_GLB_RSC_INVALID,          "All resources"},
    {NULL}
};

static sh_sand_enum_t sand_diag_fc_ips_type_enum[] = {
   /**String      Value                         Description */
    {"DELS",      DNX_FC_IPS_TYPE_DELS,         "SRAM Delete Dequeue Command Queue FC"},
    {"DELD",      DNX_FC_IPS_TYPE_DELD,         "DRAM Delete Dequeue Command Queue FC"},
    {"S2D",       DNX_FC_IPS_TYPE_S2D,          "SRAM-To-DRAM Dequeue Command Queue FC"},
    {"OCB",       DNX_FC_IPS_TYPE_OCBO,         "OCB-Only Dequeue Command Queues FC"},
    {"SRAM",      DNX_FC_IPS_TYPE_SRAM,         "SRAM to fabric that is not mixed with DRAM Dequeue Command Queues FC"},
    {"MIXS",      DNX_FC_IPS_TYPE_MIXS,         "SRAM to fabric that is mixed with DRAM Dequeue Command Queues FC"},
    {"MIXD",      DNX_FC_IPS_TYPE_MIXD,         "DRAM to fabric that is mixed with SRAM Dequeue Command Queues FC"},
    {"ALL",       DNX_FC_IPS_TYPE_COUNT,        "All types"},
    {NULL}
};

static sh_sand_enum_t sand_diag_fc_ips_contexts_enum[] = {
   /**String      Value                                       Description */
    {"LOCAL0",    DBAL_ENUM_FVAL_DQCQ_CONTEXT_LOCAL_0,       "Local device core 0 DQCQ context"},
    {"LOCAL1",    DBAL_ENUM_FVAL_DQCQ_CONTEXT_LOCAL_1,       "Local device core 1 DQCQ context"},
    {"FUC_DEV1",  DBAL_ENUM_FVAL_DQCQ_CONTEXT_FUC_DEV1,      "Fabric Unicast DQCQ context (valid only for Clos) or Device 1 DQCQ context (valid only for Mesh)"},
    {"Dev2",      DBAL_ENUM_FVAL_DQCQ_CONTEXT_DEV2,          "Device 2 DQCQ context, valid only for Mesh"},
    {"FMC_DEV3",  DBAL_ENUM_FVAL_DQCQ_CONTEXT_FMC_DEV3,      "Fabric Multicast DQCQ context (valid only for Clos) or Device 2 DQCQ context (valid only for Mesh)"},
    {"ALL",       DBAL_NOF_ENUM_DQCQ_CONTEXT_VALUES,         "All contexts"},
    {NULL}
};

sh_sand_enum_t dqcq_priority_table[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of DQCQ priorities supported",
     .value = 0,
     .plugin_str = "DNX_DATA.iqs.dqcq.nof_priorities-1"}
    ,
    {NULL}
};

static sh_sand_option_t sh_dnx_fc_cfc_options[] = {

    /*Name              Type                  Description                            Default      Extension*/
    {"Direction",       SAL_FIELD_TYPE_ENUM,  "Direction of FC",                                "GEN",       (void *) sand_diag_fc_direction_enum},
    {"Type",            SAL_FIELD_TYPE_ENUM,  "Flow Control type",                              "ALL",       (void *) sand_diag_fc_type_enum},
    {"Index",           SAL_FIELD_TYPE_INT32, "Which index to show, not relevant for GLB_RSC",  "-1",         NULL},
    {"Resource",        SAL_FIELD_TYPE_ENUM,  "Resource type relevant for GLB_RSC only",        "ALL",       (void *) sand_diag_fc_index_to_glb_rsc_enum},
    {"Count",           SAL_FIELD_TYPE_INT32, "How many IDs to show",                           "1",          NULL},
    {"ON",              SAL_FIELD_TYPE_BOOL,  "Show only FC that are currently ON",             "0",          NULL},
    {"OOB_InterFace",   SAL_FIELD_TYPE_INT32, "Select which OOB interface to show",             "-1",         NULL},
    {"ILKN_ID",         SAL_FIELD_TYPE_INT32, "Select which ILKN interface to show",            "-1",         NULL},
    {NULL}
};

static sh_sand_option_t sh_dnx_fc_nif_options[] = {

    /*Name              Type                  Description                            Default      Extension*/
    {"Direction",       SAL_FIELD_TYPE_ENUM,  "Direction of FC",                     "GEN",       (void *) sand_diag_fc_direction_enum},
    {"Type",            SAL_FIELD_TYPE_ENUM,  "Flow Control type",                   "ALL",       (void *) sand_diag_fc_nif_type_enum},
    {"Port",            SAL_FIELD_TYPE_PORT,  "Logical port",                        "all",        NULL},
    {"ON",              SAL_FIELD_TYPE_BOOL,  "Show only FC that are currently ON",  "0",          NULL},
    {NULL}
};

static sh_sand_option_t sh_dnx_fc_ips_options[] = {

    /*Name              Type                  Description                            Default       Extension*/
    {"Type",            SAL_FIELD_TYPE_ENUM,  "Flow Control type",                   "ALL",        (void *) sand_diag_fc_ips_type_enum},
    {"ConTeXt",         SAL_FIELD_TYPE_ENUM,  "DQCQ Context to show",                "ALL",        (void *) sand_diag_fc_ips_contexts_enum},
    {"PRIOrity",        SAL_FIELD_TYPE_INT32, "DQCQ Priority to show, can be range", "0-max",      (void *) dqcq_priority_table, "0-max"},
    {"ON",              SAL_FIELD_TYPE_BOOL,  "Show only FC that are currently ON",  "0",          NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_fc_man = {
    "Present Flow Control status from either CFC, NIF or IPS blocks",
};

static sh_sand_man_t sh_dnx_fc_cfc_man = {
    .brief = "Present Flow Control status from CFC block",
    .full =
        "Present Flow Control status from CFC block for a specified Flow Control type and direction. Directions can be GENeration and RECeption.\n\n"
        "When Generation is chosen this refers to indications from other blocks (CGM, NIF) that will result in generating one of three types of Flow control (LLFC, PFC, OOB).\n"
        "When Reception is chosen this refers to indications from one of three types of Flow control (LLFC, PFC, OOB, ILKN) to other blocks (EGQ, NIF) that will result in stopping the traffic on a specific target(Interface, Port, Port+COSQ).\n"
        "If no OOB_IF option is specified, all will be shown.\n"
        "If no Index is specified all FC for the specified type will be shown. \n"
        "A range of FC for the specified type will can shown, starting with the one specified by Index with count specified in Count. \n"
        "If ON=1 is set only the indications that are ON in the specified range fill be shown \n",
    .synopsis =
        "Direction=<FC direction> Type=<FC Type> Index=<FC ID> Count=<Range of IDs to be shown> ON=<Show only FC indications that are currently On> Core=<Specify core for the OOB FC> OOB_InterFace=<In case of OOB, choose the OOB interface to be shown> ILKN_ID=<In case of ILKN, choose the ILKN interface to be shown>",
    .examples =
        "Direction=REC Type=OOB_SINGLE Index=5 Count=10 ON=1 OOB_InterFace=0\n"
        "Direction=GEN Type=GLB_RSC Resource=DRAM_P1_HIGH\n" "Direction=GEN Type=ILKN_LLFC ILKN_ID=0"
};

static sh_sand_man_t sh_dnx_fc_nif_man = {
    .brief = "Present Flow Control status from NIF block",
    .full =
        "Present Flow Control status for a specified Flow Control type and direction. Directions can be GENeration and RECeption.\n\n"
        "When Generation is chosen this refers to indications from CFC or QMLF that will result in generating one of two types of Flow control - LLFC or PFC.\n"
        "When Reception is chosen this refers to indications of one of the two types of Flow control LLFC or PFC that will result in stopping the traffic on a specific target(Interface, Port, Port+COSQ).\n"
        "If no Port is specified all will be shown.\n" "If no Type is specified all will be shown. \n"
        "If ON=1 is set only the indications that are ON in the specified range fill be shown \n",
    .synopsis =
        "Direction=<FC direction> Type=<FC Type> Port=<Logical port> ON=<Show only FC indications that are currently On>",
    .examples = "Direction=REC Type=PFC_FROM_PM  Port=13 ON=1"
};

static sh_sand_man_t sh_dnx_fc_ips_man = {
    .brief = "Present Flow Control status received by IPS block's DQCQs",
    .full =
        "Present received Flow Control status for a specified Dequeue Command Queue type.\n\n"
        "If a credit is received, causing the Credit Balance to become positive, a dequeue command needs to be issued.\n"
        "However, if the relevant DQCQ is not ready (FC), the IPS will discard the credit and will not update the Q descriptor.\n"
        "The DQCQ Flow control is generated from each IPT-PDQ context, when the context level reaches a configurable threshold.\n"
        "There are two additional debug indications that are available:\n" ""
        "\t- EIR Flow control that is forwarded to the IPS to stop Excess credits generation.\n"
        "\t- Fabric MC Flow control that is forwarded to the IPS to stop FMC credits generation.\n",
    .synopsis =
        "Type=<FC Type> ConTeXt=<DQCQ Context to show> PRIOrity=<DQCQ Priority to show> ON=<Show only FC indications that are currently On>",
    .examples = "Type=SRAM ON=1 ConTeXt=LOCAL0 PRIOrity=0\n" "Type=DELD ON=0 Core=0"
};

static sh_sand_man_t sh_dnx_fc_ips_8_prio_man = {
    .brief = "Present Flow Control status received by IPS block's DQCQs",
    .full =
        "Present received Flow Control status for a specified Dequeue Command Queue type.\n\n"
        "If a credit is received, causing the Credit Balance to become positive, a dequeue command needs to be issued.\n"
        "However, if the relevant DQCQ is not ready (FC), the IPS will discard the credit and will not update the Q descriptor.\n"
        "The DQCQ Flow control is generated from each IPT-PDQ context, when the context level reaches a configurable threshold.\n",
    .synopsis =
        "Type=<FC Type> ConTeXt=<DQCQ Context to show> PRIOrity=<DQCQ Priority to show> ON=<Show only FC indications that are currently On>",
    .examples = "Type=SRAM ON=1 ConTeXt=LOCAL0 PRIOrity=0\n" "Type=DELD ON=0 Core=0"
};

/**
 * \brief
 *   Check if DQCQs with 8 priorities are supported
 */
shr_error_e
sh_fc_ips_is_8_prio(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        /**
         * This is done to prevent access to DNX-Data before it is initialized
         */
        if (!dnx_init_is_init_done_get(unit))
        {
            SHR_EXIT();
        }

        if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
        {
            /**
             * Not supported, exit with error
             */
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check if DQCQs with 8 priorities are NOT supported
 */
shr_error_e
sh_fc_ips_is_not_8_prio(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        /**
         * This is done to prevent access to DNX-Data before it is initialized
         */
        if (!dnx_init_is_init_done_get(unit))
        {
            SHR_EXIT();
        }

        if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == TRUE)
        {
            /**
             * Not supported, exit with error
             */
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_fc_ips_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    uint32 hw_context;
    uint8 only_on_indications = FALSE;
    uint8 enabled = FALSE;
    uint8 fmc_enabled = FALSE;
    uint8 eir_enabled = FALSE;
    uint8 is_mesh = FALSE;
    int skip_field;
    uint8 priority_is_present = FALSE;
    int priority = 0;
    int priority_start = 0;
    int priority_end = 0;
    bcm_core_t user_core = _SHR_CORE_ALL;
    bcm_core_t core_id = _SHR_CORE_ALL;
    dnx_fc_ips_type_e type = DNX_FC_IPS_TYPE_INVALID;
    dnx_fc_ips_type_e type_start = DNX_FC_IPS_TYPE_INVALID;
    dnx_fc_ips_type_e type_end = DNX_FC_IPS_TYPE_INVALID;
    dbal_enum_value_field_dqcq_context_e context = 0;
    dbal_enum_value_field_dqcq_context_e context_start = 0;
    dbal_enum_value_field_dqcq_context_e context_end = 0;
    dbal_enum_value_field_dqcq_subcontext_e subcontext = 0;

    /**
     * Array that holds the correct DBAL tables for each of the DQCQ FC statuses.
     */
    dbal_tables_e table[DNX_FC_IPS_TYPE_COUNT] =
        { DBAL_TABLE_IQS_DELETE_DQCQ_FLOW_CONTROL_STATUS, DBAL_TABLE_IQS_DELETE_DQCQ_FLOW_CONTROL_STATUS,
        DBAL_TABLE_IQS_SRAM_TO_DRAM_DQCQ_FLOW_CONTROL_STATUS, DBAL_TABLE_IQS_OCBO_DQCQ_FLOW_CONTROL_STATUS,
        DBAL_TABLE_IQS_DQCQ_SUBCONTEXTS_DQCQ_FLOW_CONTROL_STATUS,
        DBAL_TABLE_IQS_DQCQ_SUBCONTEXTS_DQCQ_FLOW_CONTROL_STATUS,
        DBAL_TABLE_IQS_DQCQ_SUBCONTEXTS_DQCQ_FLOW_CONTROL_STATUS
    };

    /**
     * Array that holds a sting name for each of the DQCQ FC types.
     * Will be used for better displaying of the information.
     */
    char *dqcq_name[DNX_FC_IPS_TYPE_COUNT] = { "SRAM Delete", "DRAM Delete", "SRAM to DRAM",
        "OCB-Only", "SRAM", "Mixed SRAM", "Mixed DRAM"
    };

    /**
     * Array that holds a sting name for each of the DQCQ contexts.
     * It is a two-dimensional array, the second dimension indicates the connects mode of the device -
     * If it is Fabric use index 0 and if it is Mesh use index 1 (For single FAP mode - like Mesh mode)
     * Will be used for better displaying of the information.
     */
    char *context_name[DBAL_NOF_ENUM_DQCQ_CONTEXT_VALUES][2] = { {"Local device 0", "Local device 0"},
    {"Local device 1", "Local device 1"},
    {"Fabric UC", "Device 1"},
    {"INVALID", "Device 2"},
    {"Fabric MC", "Device 3"}
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Get input parameters
     */
    SH_SAND_GET_ENUM("Type", type);
    SH_SAND_GET_ENUM("ConTeXt", context);
    SH_SAND_GET_INT32("PRIOrity", priority);
    SH_SAND_GET_INT32("core", user_core);
    SH_SAND_GET_BOOL("ON", only_on_indications);
    SH_SAND_IS_PRESENT("PRIOrity", priority_is_present);

    /**
     * When displaying all Flow Control types, specifying context and priority is not supported,
     * because not all of the types are per context and priority
     */
    if ((type == DNX_FC_IPS_TYPE_COUNT)
        && ((priority_is_present == TRUE) || (context != DBAL_NOF_ENUM_DQCQ_CONTEXT_VALUES)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When displaying all Flow Control types, specifying context and priority is not supported!\n");
    }

    /**
     * Delete-SRAM, Delete-DRAM and SRAM-to-DRAM's FC indication are not per context
     */
    if ((type <= DNX_FC_IPS_TYPE_S2D) && (context != DBAL_NOF_ENUM_DQCQ_CONTEXT_VALUES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Specified Flow Control type is not per context!\n");
    }

    /**
     * OCB-Only, Delete-SRAM and Delete-DRAM's FC indication are not per priority
     */
    if (((type <= DNX_FC_IPS_TYPE_DELD) || (type == DNX_FC_IPS_TYPE_OCBO)) && (priority_is_present == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Specified Flow Control type is not per priority!\n");
    }

     /**
      * Check for unsupported contexts
      */
    if ((context == DBAL_ENUM_FVAL_DQCQ_CONTEXT_LOCAL_1) && (dnx_data_device.general.nof_cores_get(unit) > 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Local 1 context supported only in multi-core devices!\n");
    }
    if ((context > DBAL_NOF_ENUM_DQCQ_CONTEXT_VALUES) && (context > DBAL_ENUM_FVAL_DQCQ_CONTEXT_LOCAL_0)
        && (dnx_data_iqs.dqcq.max_nof_contexts_get(unit) == 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Device supports only Local 0 context!\n");
    }

    /**
     * If specified type is ALL, we need to display all types
     */
    if (type == DNX_FC_IPS_TYPE_COUNT)
    {
        type_start = DNX_FC_IPS_TYPE_FIRST;
        type_end = DNX_FC_IPS_TYPE_COUNT - 1;
    }
    else
    {
        type_start = type;
        type_end = type;
    }

    /**
     * If specified context is ALL, we need to display all contexts
     */
    if (context == DBAL_NOF_ENUM_DQCQ_CONTEXT_VALUES)
    {
        context_start = 0;
        context_end = DBAL_NOF_ENUM_DQCQ_CONTEXT_VALUES - 1;
    }
    else
    {
        context_start = context;
        context_end = context;
    }

    /**
     * If specified priority is -1, we need to display all priorities
     */
    if (priority_is_present == FALSE)
    {
        priority_start = 0;
        priority_end = dnx_data_iqs.dqcq.nof_priorities_get(unit) - 1;
    }
    else
    {
        priority_start = priority;
        priority_end = priority;
    }

    /**
     * Print table header and columns
     */
    PRT_TITLE_SET("IPS Dequeue Command Queues FC");
    PRT_COLUMN_ADD("DQCQ");
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Context");
    PRT_COLUMN_ADD("Priority");
    PRT_COLUMN_ADD("IPT FC");
    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
    {
        /**
         * Only supported for devices with 2 priorities
 */
        PRT_COLUMN_ADD("FMC FC");
        PRT_COLUMN_ADD("EIR FC");
    }

     /**
      * Allocate the DBAL table handle
      * Needs to be allocated before the loop
      */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table[0], &entry_handle_id));

    /**
     * Iterate over the cores
     */
    DNXCMN_CORES_ITER(unit, user_core, core_id)
    {
        /**
         * Iterate over the types
         */
        for (type = type_start; type <= type_end; type++)
        {
             /**
              * Allocate the correct DBAL table handle based on the type
              * and set the current core
              */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table[type], entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

             /**
              * DRAM and SRAN Delete DQCQs are not per context and priority
              * Get the FC indication for these types and print
              */
            if (type <= DNX_FC_IPS_TYPE_DELD)
            {
                 /**
                  * Get the FC status
                  */
                dbal_value_field8_request(unit, entry_handle_id,
                                          ((type ==
                                            DNX_FC_IPS_TYPE_DELS) ? DBAL_FIELD_IPT_DRAM_FC : DBAL_FIELD_IPT_SRAM_FC),
                                          INST_SINGLE, &enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                 /**
                  * Print information only if the user has requested to print all
                  * indications regardless of the status (only_on_indications ==0)
                  * or if the current indication has ON status
                  */
                if (!(only_on_indications) || (enabled))
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", dqcq_name[type]);
                    PRT_CELL_SET("%u", core_id);
                    PRT_CELL_SET("-");
                    PRT_CELL_SET("-");
                    PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
                    {
                        PRT_CELL_SET("N/A");
                        PRT_CELL_SET("N/A");
                    }
                }
                continue;
            }

             /**
              * SRAM to DRAM DQCQ is not per context, only priority
              * Get the FC indication for this type and print
              */
            if (type == DNX_FC_IPS_TYPE_S2D)
            {
                  /**
                   * Iterate over the priorities
                   */
                for (priority = priority_start; priority <= priority_end; priority++)
                {
                      /**
                       * Set the Priority key to the DBAL table and get the status
                       */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
                    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IPT_DQCQ_FC, INST_SINGLE, &enabled);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                      /**
                       * Print information only if the user has requested to print all
                       * indications regardless of the status (only_on_indications ==0)
                       * or if the current indication has ON status
                       */
                    if (!(only_on_indications) || (enabled))
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", dqcq_name[type]);
                        PRT_CELL_SET("%u", core_id);
                        PRT_CELL_SET("-");
                        PRT_CELL_SET("%d", priority);
                        PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                        if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
                        {
                            PRT_CELL_SET("N/A");
                            PRT_CELL_SET("N/A");
                        }
                    }
                }
                continue;
            }

             /**
              * Iterate over the contexts
              */
            for (context = context_start; context <= context_end; context++)
            {
                  /**
                   * Skip contexts are not supported in the current device and get the HW
                   * value corresponding to the SW enumerator
                   */
                skip_field = dbal_field_types_encode_enum(unit, DBAL_FIELD_TYPE_DEF_DQCQ_CONTEXT, context, &hw_context);
                if (skip_field == _SHR_E_UNAVAIL)
                {
                    continue;
                }

                 /**
                  * Check in what connect mode the device is
                  * in order to print the correct Context string
                  */
                is_mesh = (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE) ? TRUE : FALSE;

                 /**
                  * Set the Context key to the DBAL table
                  */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DQCQ_CONTEXT, context);

                 /**
                  * OCB-Only DQCQ is not per priority
                  * Get the FC indication for these types and print
                  */
                if (type == DNX_FC_IPS_TYPE_OCBO)
                {
                    /**
                     * Get the FC status
                     */
                    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IPT_DQCQ_FC, INST_SINGLE, &enabled);

                    /**
                     * If EIR and Fabric MC FC status is supported get these values as well
                     */
                    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
                    {
                        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EIR_DQCQ_FC, INST_SINGLE,
                                                  &eir_enabled);
                        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FMC_DQCQ_FC, INST_SINGLE,
                                                  &fmc_enabled);
                    }
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                    /**
                     * Print information only if the user has requested to print all
                     * indications regardless of the status (only_on_indications ==0)
                     * or if any of the indications has ON status
                     */
                    if (!(only_on_indications) || (enabled) || (eir_enabled) || (fmc_enabled))
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", dqcq_name[type]);
                        PRT_CELL_SET("%u", core_id);
                        PRT_CELL_SET("%s", context_name[context][is_mesh]);
                        PRT_CELL_SET("-");
                        PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                        if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
                        {
                            PRT_CELL_SET("%s", (fmc_enabled ? "ON" : "OFF"));
                            PRT_CELL_SET("%s", (eir_enabled ? "ON" : "OFF"));
                        }
                    }
                    continue;
                }

                 /**
                  * At this stage the only Typer remaining are the 3 sub-context types
                  * SRAM, MIXS and MIXD. Check the type to determine the subcontext.
                  */
                if (type == DNX_FC_IPS_TYPE_SRAM)
                {
                    subcontext = DBAL_ENUM_FVAL_DQCQ_SUBCONTEXT_SRAM;
                }
                else if (type == DNX_FC_IPS_TYPE_MIXS)
                {
                    subcontext = DBAL_ENUM_FVAL_DQCQ_SUBCONTEXT_MIXS;
                }
                else
                {
                    subcontext = DBAL_ENUM_FVAL_DQCQ_SUBCONTEXT_MIXD;
                }

                 /**
                  * Set the Sub-context key to the DBAL table
                  */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DQCQ_SUBCONTEXT, subcontext);

                 /**
                  *  All Sub-context DQCQs are per context and priority
                  *  so iterate over the priorities
                  */
                for (priority = priority_start; priority <= priority_end; priority++)
                {
                      /**
                       * Set the Priority key to the DBAL table and get the status
                       */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
                    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IPT_DQCQ_FC, INST_SINGLE, &enabled);
                      /**
                       * If EIR and Fabric MC FC status is supported get these values as well
                       */
                    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
                    {
                        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EIR_DQCQ_FC, INST_SINGLE,
                                                  &eir_enabled);
                        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_FMC_DQCQ_FC, INST_SINGLE,
                                                  &fmc_enabled);

                    }
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                      /**
                       * Print information only if the user has requested to print all
                       * indications regardless of the status (only_on_indications ==0)
                       * or if any of the indications has ON status
                       */
                    if (!(only_on_indications) || (enabled) || (eir_enabled) || (fmc_enabled))
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", dqcq_name[type]);
                        PRT_CELL_SET("%u", core_id);
                        PRT_CELL_SET("%s", context_name[context][is_mesh]);
                        PRT_CELL_SET("%d", priority);
                        PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                        if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == FALSE)
                        {
                            PRT_CELL_SET("%s", (fmc_enabled ? "ON" : "OFF"));
                            PRT_CELL_SET("%s", (eir_enabled ? "ON" : "OFF"));
                        }
                    }
                }
            }
        }
    }

    /**
     * If there is content, print table,
     * else give a message
     */
    if (PRT_ROW_NUM() > 0)
    {
        PRT_COMMITX;
    }
    else
    {
        cli_out("No content for table IPS Dequeue Command Queues FC!\n");
        PRT_FREE;
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function presents the information from the NIF block for different types of Flow control
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
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
sh_dnx_fc_nif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    bcm_port_t logical_port_i;
    int lane, priority, lane_in_core = 0;
    bcm_pbmp_t lanes, logical_ports, logical_ports_nif;
    dnx_algo_port_ethu_access_info_t ethu_access_info;
    uint8 enabled = 0, only_on_indications, direction, is_pfc = 0;
    int type_start, type_end, prio_start, prio_end;
    dnx_fc_nif_type_e type;
    dbal_fields_e field = DBAL_FIELD_EMPTY;
    dbal_tables_e table = DBAL_TABLE_EMPTY;
    uint8 content_exists = FALSE;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get input parameters
     */
    SH_SAND_GET_ENUM("Direction", direction);
    SH_SAND_GET_ENUM("Type", type);
    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_GET_BOOL("ON", only_on_indications);

    if ((type != DNX_FC_NIF_TYPE_COUNT) && (type <= DNX_FC_NIF_TYPE_LLFC_FROM_QMLF)
        && (direction == DNX_FC_DIRECTION_RECEPTION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Reception for the specified Flow Control type is not supported.\n")}

    if ((type != DNX_FC_NIF_TYPE_COUNT) && (type > DNX_FC_NIF_TYPE_LLFC_FROM_QMLF)
        && (direction == DNX_FC_DIRECTION_GENERATION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Generation for the specified Flow Control type is not supported.\n")}

    /*
     * If specified type is ALL, we need to display all types of FC
     */
    if (type == DNX_FC_NIF_TYPE_COUNT)
    {
        type_start = DNX_FC_NIF_TYPE_FIRST;
        type_end = DNX_FC_NIF_TYPE_COUNT - 1;
    }
    else
    {
        type_start = type;
        type_end = type;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_PFC_DEBUG, &entry_handle_id));
    for (type = type_start; type <= type_end; type++)
    {
        /*
         * Determine if the type is one of the PFC
         */
        if ((type == DNX_FC_NIF_TYPE_PFC_TO_CFC) || (type == DNX_FC_NIF_TYPE_PFC_FROM_PM)
            || (type == DNX_FC_NIF_TYPE_PFC_TO_PM) || (type == DNX_FC_NIF_TYPE_PFC_FROM_CFC)
            || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
        {
            is_pfc = 1;
        }
        else
        {
            is_pfc = 0;
        }

        /*
         * Print table header
         */
        PRT_TITLE_SET("NIF FLOW CONTROL DEBUG");
        /*
         * Print Flow Control diagnostic
         */
        PRT_COLUMN_ADD("Type");
        PRT_COLUMN_ADD("Logical Port");
        if (type == DNX_FC_NIF_TYPE_LLFC_TO_ILKN)
        {
            PRT_COLUMN_ADD("ILKN Core ID");
            PRT_COLUMN_ADD("ILKN Port ID");
        }
        else
        {
            if (is_pfc)
            {
                PRT_COLUMN_ADD("Priority");
            }
            PRT_COLUMN_ADD("ETHU");
            if ((!is_pfc) || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
            {
                PRT_COLUMN_ADD("Lane in core");
            }
        }
        PRT_COLUMN_ADD("FC Indication");

        /*
         * Switch over the different types to determine the field and table
         */
        switch (type)
        {
            case DNX_FC_NIF_TYPE_PFC_FROM_QMLF:
                table = DBAL_TABLE_NIF_FC_ETHU_PFC_QMLF_DEBUG;
                field = DBAL_FIELD_SIGNAL_STATUS;
                break;
            case DNX_FC_NIF_TYPE_LLFC_FROM_QMLF:
                table = DBAL_TABLE_NIF_FC_ETHU_LLFC_QMLF_DEBUG;
                field = DBAL_FIELD_SIGNAL_STATUS;
                break;
            case DNX_FC_NIF_TYPE_LLFC_TO_PM:
                field = DBAL_FIELD_GEN_LLFC_TO_PM;
                table = DBAL_TABLE_NIF_FC_ETHU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_LLFC_FROM_CFC:
                field = DBAL_FIELD_GEN_LLFC_FROM_CFC;
                table = DBAL_TABLE_NIF_FC_ETHU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_STOP_TO_PM:
                field = DBAL_FIELD_LLFC_STOP_TX_TO_PM;
                table = DBAL_TABLE_NIF_FC_ETHU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_STOP_FROM_CFC:
                field = DBAL_FIELD_LLFC_STOP_TX_FROM_CFC;
                table = DBAL_TABLE_NIF_FC_ETHU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_FROM_CFC:
                field = DBAL_FIELD_GEN_PFC_FROM_CFC;
                table = DBAL_TABLE_NIF_FC_ETHU_PFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_TO_PM:
                field = DBAL_FIELD_GEN_PFC_TO_PM;
                table = DBAL_TABLE_NIF_FC_ETHU_PFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_FROM_PM:
                field = DBAL_FIELD_RX_PFC_FROM_PM;
                table = DBAL_TABLE_NIF_FC_ETHU_PFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_TO_CFC:
                field = DBAL_FIELD_RX_PFC_TO_CFC;
                table = DBAL_TABLE_NIF_FC_ETHU_PFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_LLFC_TO_ILKN:
                field = DBAL_FIELD_LLFC_STOP_TX_FROM_CFC;
                table = DBAL_TABLE_NIF_FC_ILU_LLFC_DEBUG;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid Flow Control Type");
        }

        /*
         * Iterate over valid logical ports bitmap
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &logical_ports_nif));
        BCM_PBMP_AND(logical_ports, logical_ports_nif);
        BCM_PBMP_ITER(logical_ports, logical_port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port_i, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 1, 1) && (type != DNX_FC_NIF_TYPE_LLFC_TO_ILKN))
            {
                LOG_CLI((BSL_META("Skipping port %d for type %s, not of ETHU type... \n"), logical_port_i,
                         sand_diag_fc_nif_type_enum[type].string));
                continue;
            }

            if ((!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0)) && (type == DNX_FC_NIF_TYPE_LLFC_TO_ILKN))
            {
                LOG_CLI((BSL_META("Skipping port %d for type %s, not of ILKN type... \n"), logical_port_i,
                         sand_diag_fc_nif_type_enum[type].string));
                continue;
            }

            if (type == DNX_FC_NIF_TYPE_LLFC_TO_ILKN)
            {
                /** Get the ILKN access info */
                SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, logical_port_i, &ilkn_access_info));
                /** Get the information from the DBAL table */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table, entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID,
                                           ilkn_access_info.port_in_core);
                dbal_value_field8_request(unit, entry_handle_id, field, INST_SINGLE, &enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                if (!(only_on_indications) || (enabled))
                {
                    /*
                     * Present the information for FC
                     */
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", sand_diag_fc_nif_type_enum[type].string);
                    PRT_CELL_SET("%u", logical_port_i);
                    PRT_CELL_SET("%u", ilkn_access_info.ilkn_core);
                    PRT_CELL_SET("%u", ilkn_access_info.port_in_core);
                    PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                }
            }
            else
            {
                /** If type is PFC we also have to iterate over priority */
                if (is_pfc)
                {
                    prio_start = 0;
                    prio_end = BCM_COS_COUNT - 1;
                }
                else
                {
                    prio_start = prio_end = 0;
                }

                /** Determine the CDU access info */
                SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port_i, &ethu_access_info));
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port_i, 0, &lanes));

                /** Get the information from the DBAL table */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table, entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_access_info.core);
                for (priority = prio_start; priority <= prio_end; priority++)
                {
                    if (is_pfc)
                    {
                        /** If type is PFC we also have priority as a key */
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
                        if (type != DNX_FC_NIF_TYPE_PFC_FROM_QMLF)
                        {
                            /** When generated from QMLF, PFC status table is accessed by lane in core and not ETHU ID */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID,
                                                       ethu_access_info.ethu_id_in_core);
                        }
                    }
                    _SHR_PBMP_ITER(lanes, lane)
                    {
                        /** If type is PFC we don't have lane as a key, except when generated from QMLF*/
                        if ((!is_pfc) || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
                        {
                            lane_in_core = lane % dnx_data_nif.phys.nof_phys_per_core_get(unit);
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
                        }
                        dbal_value_field8_request(unit, entry_handle_id, field, INST_SINGLE, &enabled);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                        if (!(only_on_indications) || (enabled))
                        {
                            /*
                             * Present the information for FC
                             */
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", sand_diag_fc_nif_type_enum[type].string);
                            PRT_CELL_SET("%u", logical_port_i);
                            if (is_pfc)
                            {
                                PRT_CELL_SET("%u", priority);
                            }
                            PRT_CELL_SET("%u", ethu_access_info.ethu_id_in_core);
                            /** If type is PFC we don't have lane, except when generated from QMLF*/
                            if ((!is_pfc) || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
                            {
                                PRT_CELL_SET("%u", lane_in_core);
                            }
                            PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                        }

                        /** If type is PFC we don't have lane as a key, except when generated from QMLF.
                          * Because of this we ned to break the loop over the lanes and continue with the next priority. */
                        if (is_pfc && (type != DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
                        {
                            break;
                        }
                    }
                }
            }
        }

        if (PRT_ROW_NUM() > 0)
        {
            PRT_COMMITX;
            content_exists = TRUE;
        }
        else
        {
            PRT_FREE;
        }
    }

    /** If no content was found, give a message */
    if (!content_exists)
    {
        cli_out("No content for table NIF FLOW CONTROL DEBUG\n");
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function presents the information from the CFC block for different types of Flow control
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
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
sh_dnx_fc_cfc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    int id, id_min = 0, id_max = 0, index, id_current;
    uint32 select, select_prev, bit_idx, alloc_size;
    uint32 *status = NULL;
    uint8 enabled = 0, only_on_indications, direction;
    int oob_if, if_current;
    int core, core_id;
    int ilkn_if;
    uint32 range, max_range, max_id;
    int nof_presented, type_start, type_end;
    int if_start, if_end, is_oob = 0, is_ilkn = 0;
    dnx_fc_type_e type;
    dnx_fc_glb_rsc_e resource;
    dbal_fields_e field_select = DBAL_FIELD_EMPTY;
    dbal_tables_e table_select = DBAL_TABLE_EMPTY;
    dbal_fields_e field_status = DBAL_FIELD_EMPTY;
    dbal_tables_e table_status = DBAL_TABLE_EMPTY;
    uint8 content_exists = FALSE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get input parameters
     */
    SH_SAND_GET_ENUM("Direction", direction);
    SH_SAND_GET_ENUM("Type", type);
    SH_SAND_GET_INT32("OOB_InterFace", oob_if);
    SH_SAND_GET_ENUM("Resource", resource);
    SH_SAND_GET_INT32("Index", index);
    SH_SAND_GET_INT32("Count", range);
    SH_SAND_GET_BOOL("ON", only_on_indications);
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_INT32("ILKN_ID", ilkn_if);

    if ((type != DNX_FC_TYPE_COUNT) && (type <= DNX_FC_TYPE_VSQ_PFC) && (direction == DNX_FC_DIRECTION_RECEPTION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Reception for the specified Flow Control type is not supported.\n")}

    if ((type != DNX_FC_TYPE_COUNT) && (type > DNX_FC_TYPE_VSQ_PFC) && (direction == DNX_FC_DIRECTION_GENERATION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Generation for the specified Flow Control type is not supported.\n")}

    if ((type == DNX_FC_TYPE_GLB_RSC) && (index != -1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Index for type Global resource is not supported. Please use Resource. \n");
    }
    else if ((type != DNX_FC_TYPE_GLB_RSC) && (resource != -1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource for the specified type is not supported. Please use Index. \n");
    }

    if ((type == DNX_FC_TYPE_INB_ILKN) || (type == DNX_FC_TYPE_ILKN_STOP_PM) || (type == DNX_FC_TYPE_ILKN_AF_NIF)
        || (type == DNX_FC_TYPE_LLFC_ILKN))
    {
        if (((type != DNX_FC_TYPE_INB_ILKN)
             && !(dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilu_fc_status)))
            || ((type == DNX_FC_TYPE_INB_ILKN)
                && !(dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_inb_ilkn_rx_status))))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_NONE, "This Diagnostic command is not supported on the current Device %s%s%s\n",
                              EMPTY, EMPTY, EMPTY);
        }

        if ((ilkn_if != -1)
            && ((ilkn_if < 0) || (ilkn_if >= dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN Interface is not valid for device. \n");
        }
    }

    /*
     * If specified type is ALL, we need to display all types of FC
     */
    if (type == DNX_FC_TYPE_COUNT)
    {
        type_start = DNX_FC_TYPE_FIRST;
        type_end = DNX_FC_TYPE_COUNT - 1;
    }
    else
    {
        type_start = type;
        type_end = type;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_STATUS_SELECT, &entry_handle_id));
    for (type = type_start; type <= type_end; type++)
    {
        /*
         * Skip types that are not valid for device
         */
        if ((((type == DNX_FC_TYPE_ILKN_STOP_PM) || (type == DNX_FC_TYPE_ILKN_AF_NIF)
              || (type == DNX_FC_TYPE_LLFC_ILKN))
             && !(dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilu_fc_status)))
            || ((type == DNX_FC_TYPE_INB_ILKN)
                && !(dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_inb_ilkn_rx_status))))
        {
            continue;
        }

        if (((type == DNX_FC_TYPE_LLFC_NIF) || (type == DNX_FC_TYPE_NIF_STOP_PM)
             || (type == DNX_FC_TYPE_PFC_NIF))
            && !(dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_fc_to_nif_status)))
        {
            continue;
        }

        /*
         * Determine if the type is one of the OOB
         */
        if ((type >= DNX_FC_TYPE_OOB_SINGLE) && (type <= DNX_FC_TYPE_PP_COE) && (type != DNX_FC_TYPE_COUNT))
        {
            is_oob = 1;
        }
        else
        {
            is_oob = 0;
        }

        /*
         * Determine if the type is one of the ILKN
         */
        if ((type == DNX_FC_TYPE_INB_ILKN) || (type == DNX_FC_TYPE_ILKN_STOP_PM) || (type == DNX_FC_TYPE_ILKN_AF_NIF)
            || (type == DNX_FC_TYPE_LLFC_ILKN))
        {
            is_ilkn = 1;
        }
        else
        {
            is_ilkn = 0;
        }

        /*
         * For Calendar FC if specified interface is ALL, we need to display results for all interfaces
         */
        if (is_oob)
        {
            if (oob_if == -1)
            {
                if_start = 0;
                if_end = dnx_data_fc.oob.nof_oob_ids_get(unit) - 1 - 1;
            }
            else
            {
                if_start = oob_if;
                if_end = oob_if;
            }
        }
        else if (type == DNX_FC_TYPE_INB_ILKN)
        {
            if (ilkn_if == -1)
            {
                if_start = 0;
                if_end = dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) - 1;
            }
            else
            {
                if_start = ilkn_if;
                if_end = ilkn_if;
            }
        }
        else if (type == DNX_FC_TYPE_PP_COE)
        {
            if (ilkn_if == -1)
            {
                if_start = 0;
                if_end = dnx_data_fc.coe.nof_coe_cal_instances_get(unit) - 1;
            }
            else
            {
                if_start = ilkn_if;
                if_end = ilkn_if;
            }
        }
        else
        {
            /*
             * So that we would only pass through the loop once
             */
            if_start = if_end = 0;
        }

        /*
         * Iterate over calendar interfaces
         */
        for (if_current = if_start; if_current <= if_end; if_current++)
        {
            /*
             * Set the relevant tables for most of the types. For types with different tables the value will be overwritten in the switch
             */
            table_status = DBAL_TABLE_FC_STATUS;
            table_select = DBAL_TABLE_FC_STATUS_SELECT;

            /*
             * Switch over the different threshold types
             */
            switch (type)
            {
                case DNX_FC_TYPE_GLB_RSC:
                    /*
                     * Set maximum ID
                     */
                    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                                    (unit, DBAL_TABLE_FC_GLB_RSC_STATUS, DBAL_FIELD_FC_INDEX, TRUE, 0, 0,
                                     DBAL_PREDEF_VAL_MAX_VALUE, &max_id));
                    max_range = max_id + 1;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_STATUS;
                    table_status = DBAL_TABLE_FC_GLB_RSC_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_AF_NIF:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_nif.phys.nof_phys_per_core_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_AF_NIF_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_FC_EGQ:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_egr_queuing.params.nof_q_pairs_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_PFC_STATUS_SEL;
                    field_status = DBAL_FIELD_PFC_STATUS;
                    break;
                case DNX_FC_TYPE_NIF_PFC:
                    /*
                     * Set maximum ID
                     */
                    max_range = (dnx_data_nif.phys.nof_phys_per_core_get(unit) * BCM_COS_COUNT);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_NIF_PFC_STATUS_SEL;
                    field_status = DBAL_FIELD_NIF_PFC_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_LLFC:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_LLFC_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_LLFC_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_A:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_a_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_ABCD_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_ABCD_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_B:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_b_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_ABCD_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_ABCD_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_C:
                    /*
                     * Set maximum ID (Only categories 2 and 3 can generate FC, so half the indexes are used)
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_c_nof_get(unit) / 2;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_ABCD_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_ABCD_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_D:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_ABCD_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_ABCD_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_PFC:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_PFC_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_PFC_STATUS;
                    break;
                case DNX_FC_TYPE_OOB_SINGLE:
                case DNX_FC_TYPE_OOB_PORT_BASED:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_egr_queuing.params.nof_q_pairs_get(unit);
                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_OOB_RX_PFC_SEL;
                    field_status = DBAL_FIELD_OOB_RX_PFC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_FC_OOB_STATUS_SELECT;
                    break;
                case DNX_FC_TYPE_OOB_LLFC:
                    /*
                     * Set maximum ID
                     * (NIF ports + 4 CMIC)
                     */
                    max_range = dnx_data_fc.general.nof_cal_llfcs_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_OOB_RX_LLFC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_OOB_GEN_BMP:
                    /*
                     * Set maximum ID
                     */
                    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                                    (unit, DBAL_TABLE_FC_GENERIC_BITMAP, DBAL_FIELD_BMP_IDX, TRUE, 0, 0,
                                     DBAL_PREDEF_VAL_MAX_VALUE, &max_id));
                    max_range = max_id + 1;
                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_OOB_RX_GEN_PFC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_ILKN_OOB:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_fc.oob.calender_length_get(unit, if_current)->rx;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_ILKN_OOB_RX_FC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_PP_COE:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_fc.coe.calender_length_get(unit, if_current)->len;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_ILKN_PP_RX_FC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_INB_ILKN:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_fc.inband.calender_length_get(unit, if_current)->rx;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_RX_STATUS;
                    table_status = DBAL_TABLE_FC_ILKN_INB_RX_FC_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_ILKN_STOP_PM:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_nif.ilkn.ilkn_if_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_TX_LLFC_STOP;
                    table_status = DBAL_TABLE_FC_NIF_ILU_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_ILKN_AF_NIF:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_nif.ilkn.ilkn_if_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_RX_LLFC;
                    table_status = DBAL_TABLE_FC_NIF_ILU_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_LLFC_ILKN:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_nif.ilkn.ilkn_if_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_TX_LLFC_GEN;
                    table_status = DBAL_TABLE_FC_NIF_ILU_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_LLFC_NIF:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_nif.phys.nof_phys_per_core_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_NIF_TX_LLFC_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_NIF_STOP_PM:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_nif.phys.nof_phys_per_core_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_NIF_STOP_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_PFC_NIF:
                    /*
                     * Set maximum ID
                     */
                    max_range = (dnx_data_nif.phys.nof_phys_per_core_get(unit) * BCM_COS_COUNT);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_PFC_GEN_STATUS;
                    field_select = DBAL_FIELD_PFC_GEN_STATUS_SEL;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid Flow Control Type");
            }

            if (type == DNX_FC_TYPE_OOB_SINGLE || type == DNX_FC_TYPE_OOB_PORT_BASED)
            {
                /*
                 * Select the appropriate target for the Q-pair based FC
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_OOB_STATUS_SELECT, entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, if_current);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_PFC_SRC_SEL, INST_SINGLE,
                                             (type == DNX_FC_TYPE_OOB_SINGLE ? 0 : 1));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }

            /*
             * Get number of bits presented at once from the DBAL table
             */
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_status, field_status, FALSE, 0, 0, &nof_presented));

            /*
             * Get the appropriate size and allocate the needed memory for the status
             */
            if ((nof_presented % SHR_BITWID) == 0)
            {
                alloc_size = nof_presented / SHR_BITWID;
            }
            else
            {
                alloc_size = (nof_presented / SHR_BITWID) + 1;
            }

            /*
             * The max_range is per core, but for some OOB types the status register is with size 2*max_range (each calendar instance can have targets on both cores)
             */
            if ((type == DNX_FC_TYPE_OOB_LLFC) || (type == DNX_FC_TYPE_OOB_PORT_BASED)
                || (type == DNX_FC_TYPE_OOB_SINGLE))
            {
                alloc_size = alloc_size * 2;
            }
            SHR_ALLOC_SET_ZERO(status, sizeof(uint32) * (alloc_size), "status", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            /*
             * Determine ID max and min for the case where user wants to display more than one FC
             */
            if (type == DNX_FC_TYPE_GLB_RSC)
            {
                /** For Global resource the index is determined by the resource type enumerator */
                index = resource;
            }

            /*
             * Calculate the minimum and maximum IDs to be presented.
             */
            if ((index == -1) && (range == 1))
            {
                id_min = 0;
                id_max = max_range - 1;
            }
            else if ((index != -1) && (range == 1))
            {
                id_min = id_max = index;
            }
            else if ((index != -1) && (range != 1))
            {
                id_min = index;
                id_max = index + range;
            }

            /*
             * In case the specified range is outside the max IDs allowed, display up to max_range
             */
            if (id_max > max_range)
            {
                id_max = max_range;
            }

            /*
             * Iterate over the cores
             */
            DNXCMN_CORES_ITER(unit, core_id, core)
            {

                /*
                 * Set the select_prev to initial value to ensure first iteration will read the HW
                 */
                select_prev = 0xFFFF;

                /*
                 * Print table header
                 */
                PRT_TITLE_SET("CFC FLOW CONTROL DEBUG");
                PRT_INFO_ADD("Flow Control Type %s", sand_diag_fc_type_enum[type].string);
                if (is_oob || (type == DNX_FC_TYPE_INB_ILKN))
                {
                    PRT_INFO_ADD("Calendar Interface %d", if_current);
                }
                PRT_INFO_ADD("Showing results for core=%d", core);

                /*
                 * Print Flow Control diagnostic
                 */
                if (type == DNX_FC_TYPE_GLB_RSC)
                {
                    PRT_COLUMN_ADD("Resource");
                }
                else
                {
                    PRT_COLUMN_ADD("Index");
                }
                PRT_COLUMN_ADD("FC Indication");

                /*
                 * Iterate over IDs and get the information from the status tables
                 */
                for (id_current = id_min; id_current <= id_max; id_current++)
                {
                    /*
                     * For some OOB types each calendar instance can have targets on both cores
                     */
                    if ((type == DNX_FC_TYPE_OOB_SINGLE) || (type == DNX_FC_TYPE_OOB_PORT_BASED)
                        || (type == DNX_FC_TYPE_OOB_LLFC))
                    {
                        id = id_current + max_range * core;
                    }
                    else
                    {
                        id = id_current;
                    }

                    /*
                     * Determine the value of the select
                     */
                    switch (type)
                    {
                        case DNX_FC_TYPE_VSQ_A:
                            select = 0;
                            break;
                        case DNX_FC_TYPE_VSQ_B:
                            select = 1;
                            break;
                        case DNX_FC_TYPE_VSQ_C:
                            select = 2;
                            break;
                        case DNX_FC_TYPE_VSQ_D:
                            select = (id / nof_presented) + 3;
                            break;
                        default:
                            select = id / nof_presented;
                    }

                    /*
                     * Read the status from HW again only is the select is changed.
                     * This means that the current ID is not in the range of the surrent select
                     */
                    if (select != select_prev)
                    {
                        if (table_select != DBAL_TABLE_EMPTY)
                        {
                            /*
                             * Set the appropriate select for the bits to be presented
                             */
                            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_select, entry_handle_id));
                            if (is_oob)
                            {
                                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, if_current);
                            }
                            else
                            {
                                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                            }
                            dbal_entry_value_field32_set(unit, entry_handle_id, field_select, INST_SINGLE, select);
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                        }

                        /*
                         * Clear the variable and get the status
                         */
                        sal_memset(status, 0x0, sizeof(uint32) * (alloc_size));
                        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_status, entry_handle_id));
                        if ((is_oob) || (type == DNX_FC_TYPE_INB_ILKN))
                        {
                            /*
                             * For calendars the key to the table is the interface
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, if_current);
                            if (type == DNX_FC_TYPE_INB_ILKN)
                            {
                                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, id_current);
                            }
                        }
                        else if (type == DNX_FC_TYPE_GLB_RSC)
                        {
                            /*
                             * For Global resources the keys to the table are core and resource index
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, id);
                        }
                        else if ((is_ilkn == 1) && (type != DNX_FC_TYPE_INB_ILKN))
                        {
                            /*
                             * ILKN the keys are ILKN port ID and ILKN Core ID
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID,
                                                       (id_current / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)));
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID,
                                                       (id_current % dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)));
                        }
                        else
                        {
                            /*
                             * For all other types the key to the table is only core
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                        }
                        dbal_value_field_arr32_request(unit, entry_handle_id, field_status, INST_SINGLE, status);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                        /** Registers for EGQ PFC status and NIF PFC status are both 'sticky' and will clear on read only.
                          * Changing the STATUS_SEL for each of them will result with an OR between two different ranges of the PFC vector.
                          * This is wrong behavior and can be avoided by reading the registers twice for the same select value in order to clear them. */
                        if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pfc_status_regs_double_read))
                        {
                            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                        }

                        /*
                         * Set the select_prev
                         */
                        select_prev = select;
                    }

                    /*
                     * Get the bit corresponding to the requested index
                     */
                    bit_idx = ((type == DNX_FC_TYPE_GLB_RSC) || (is_ilkn)) ? 0 : (id % nof_presented);
                    enabled = SHR_BITGET(status, bit_idx) ? 1 : 0;

                    if (!(only_on_indications) || (enabled))
                    {
                        /*
                         * Present the information for both enabled and disabled FC
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        if (type == DNX_FC_TYPE_GLB_RSC)
                        {
                            PRT_CELL_SET("%s", sand_diag_fc_index_to_glb_rsc_enum[id_current].string);
                        }
                        else
                        {
                            PRT_CELL_SET("%u", id_current);
                        }
                        PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                    }
                }
                if (PRT_ROW_NUM() > 0)
                {
                    PRT_COMMITX;
                    content_exists = TRUE;
                }
                else
                {
                    PRT_FREE;
                }
            }
            SHR_FREE(status);
        }
    }

    /** If no content was found, give a message */
    if (!content_exists)
    {
        cli_out("No content for table CFC FLOW CONTROL DEBUG\n");
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnx_fc_cmds[] = {
    /*keyword,      action,                command,                     options,                 man                    */
    {"CFC",        sh_dnx_fc_cfc_cmd,      NULL,                        sh_dnx_fc_cfc_options,   &sh_dnx_fc_cfc_man},
    {"NIF",        sh_dnx_fc_nif_cmd,      NULL,                        sh_dnx_fc_nif_options,   &sh_dnx_fc_nif_man},
    {"IPS",        sh_dnx_fc_ips_cmd,      NULL,                        sh_dnx_fc_ips_options,   &sh_dnx_fc_ips_8_prio_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_fc_ips_is_8_prio},
    {"IPS",        sh_dnx_fc_ips_cmd,      NULL,                        sh_dnx_fc_ips_options,   &sh_dnx_fc_ips_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_fc_ips_is_not_8_prio},
    {NULL}
};
/* *INDENT-ON* */
