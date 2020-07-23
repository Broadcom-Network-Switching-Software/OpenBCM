/** \file diag_dnx_trap_last_pkt.c
 * 
 *
 * file for last packet trap diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <soc/sand/sand_signals.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/rx/rx_trap_map.h>
#include "diag_dnx_trap.h"

/*
 * }
 */

/**
 * \brief
 *    Init callback function for "trap last info" diag commands
 *    1) Create and set two competeing ingress predefined trap
 *    2) Send packet that trigger the traps
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
diag_dnx_trap_last_pkt_init_cb(
    int unit)
{
    bcm_mirror_destination_t snoop_dest;
    bcm_gport_t snoop_dest_port;
    bcm_rx_trap_config_t config = { 0 };
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Configure snoop command */
    bcm_mirror_destination_t_init(&snoop_dest);
    BCM_GPORT_SYSTEM_PORT_ID_SET(snoop_dest_port, 14);
    BCM_GPORT_LOCAL_SET(snoop_dest.gport, snoop_dest_port);
    snoop_dest.flags |= BCM_MIRROR_DEST_IS_SNOOP;
    SHR_IF_ERR_EXIT(bcm_mirror_destination_create(unit, &snoop_dest));

    /** Configure trap action */
    bcm_rx_trap_config_t_init(&config);
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_LOCAL_CPU;
    config.trap_strength = 15;
    config.snoop_cmnd = BCM_GPORT_MIRROR_GET(snoop_dest.mirror_dest_id);
    config.snoop_strength = 7;
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapLinkLayerSaEqualsDa, &trap_id));
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

    SHR_IF_ERR_EXIT(diag_sand_packet_send_by_case(unit, "sa_equals_da", NULL, SAND_PACKET_RESUME));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_trap_last_pkt_info_man = {
    .brief = "Show last packet trap info\n",
    .full = "Shows all considered and triggered traps in the device.\n",
    .synopsis = "[block=<IRPP | ERPP | ETPP> core=0/1/all]",
    .examples = "block=IRPP core=0",
    .init_cb = diag_dnx_trap_last_pkt_init_cb
};

static sh_sand_option_t dnx_trap_last_pkt_info_options[] = {
    {"BLock", SAL_FIELD_TYPE_ENUM, "Trap block", "ALL", (void *) Trap_block_enum_table},
    {NULL}
};

#define SH_DNX_TRAP_INGRESS_NOF_BLOCKS     (11)
#define SH_DNX_TRAP_ERPP_MAX_NOF_HIT_SIGNALS (13)

typedef struct
{
    char *block;
    char *trap_code;
    char *fwd_strength;
    char *snoop_code;
    char *snoop_strength;
} sh_dnx_trap_ingress_sig_t;

const sh_dnx_trap_ingress_sig_t ingress_sig_list[SH_DNX_TRAP_INGRESS_NOF_BLOCKS] = {
    {"LLR", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"VTT1", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"VTT2", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"VTT3", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"VTT4", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"VTT5", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"FWD1", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"FWD2", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"IPMF1", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"FER", "Fwd_Action_CPU_Trap_Code", "Fwd_Action_strength", "Snoop_Code", "Snoop_Strength"},
    {"IPMF3", "Fwd_Action_CPU_Trap_Code", "Fwd_strength", "Snoop_Code", NULL},
};

typedef struct
{
    bcm_rx_trap_t trap_type;
    int res_priority;
    uint32 nof_hit_signals;
    char *hit_signals_names[SH_DNX_TRAP_ERPP_MAX_NOF_HIT_SIGNALS];
} sh_dnx_trap_erpp_hit_indication_t;

/* *INDENT-OFF* */

const sh_dnx_trap_erpp_hit_indication_t erpp_hit_sig_list[DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES] = {
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TDM_DISCARD] =             {bcmRxTrapEgTdmDiscard, 1, 1, {"TDM_Wrong_Port_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_INVALID_OTM] =             {bcmRxTrapEgInvalidDestPort, 2, 1, {"Invalid_OTM_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_EXCLUDE_SRC] =             {bcmRxTrapEgExcludeSrc, 5, 1, {"Exclude_Src_Int"}},
        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_DSS_STACKING] =            {bcmRxTrapDssStacking, 3, 1, {"Dss_Stacking_Int"}},
        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_LAG_MULTICAST] =           {bcmRxTrapLagMulticast, 4, 1, {"LAG_MC_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_UNACCEPTABLE_FRAME_TYPE] = {bcmRxTrapEgDiscardFrameTypeFilter, 7, 1, {"Unacceptable_Frame_Type_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_SRC_EQUAL_DEST] =          {bcmRxTrapEgHairPinFilter, 6, 1, {"Src_Eq_Dst_Int"}},
        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_UNKNOWN_DA] =              {bcmRxTrapEgUnknownDa, 17, 1, {"Unknown_DA_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_SPLIT_HORIZON] =           {bcmRxTrapEgSplitHorizonFilter, 8, 1, {"Split_Horizon_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_SCOPE] =               {bcmRxTrapEgIpmcTtlErr, 11, 1, {"TTL_Scope_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_GLEM_PP_TRAP] =            {bcmRxTrapEgGlemPpTrap, 9, 1, {"GLEM_PP_Trap_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_GLEM_NON_PP_TRAP] =        {bcmRxTrapEgGlemNonePpTrap, 10, 1, {"GLEM_NON_PP_Trap_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_FILTERS_ERROR] =      {bcmRxTrapEgIpv4Error, 14, 8, {"IPv4_Version_Err_Int", 
                                                                                       "IPv4_Header_Length_Err_Int", 
                                                                                       "IPv4_Total_Length_Err_Int", 
                                                                                       "IPv4_Options_Int", 
                                                                                       "IPv4_SIP_Eq_DIP_Int", 
                                                                                       "IPv4_DIP_Eq_Zero_Int", 
                                                                                       "IPv4_SIP_is_MC_Int",
                                                                                       "IPv4_Checksum_Err_Int"}},
                                                                                       
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_FILTERS_ERROR] =      {bcmRxTrapEgIpv6Error, 15, 13, {"IPv6_Version_Err_Int", 
                                                                                        "IPv6_Unspecified_Src_Int", 
                                                                                        "IPv6_Unspecified_Dst_Int", 
                                                                                        "IPv6_SIP_is_MC_Int", 
                                                                                        "IPv6_Loopback_Int", 
                                                                                        "IPv6_Hop_by_Hop_Int", 
                                                                                        "IPv6_DIP_is_MC_Int",
                                                                                        "IPv6_Link_Local_Dst_Int", 
                                                                                        "IPv6_Link_Local_Src_Int", 
                                                                                        "IPv6_Site_Local_Dst_Int",
                                                                                        "IPv6_Site_Local_Src_Int", 
                                                                                        "IPv6_IPv4_Compatible_Dst_Int", 
                                                                                        "IPv6_IPv4_Mapped_Dst_Int"}},
                                                                                        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_LAYER_4_FILTERS_ERROR] =   {bcmRxTrapEgL4Error, 16, 7, {"TCP_Seq_Num_and_Flags_are_Zero_Int", 
                                                                                     "TCP_Fragment_with_Incomplete_TCP_Header_Int",
                                                                                     "TCP_Seq_Num_is_Zero_and_FIN_or_URG_or_PSH_is_Set_Int", 
                                                                                     "TCP_SYN_and_FIN_are_Set_Int",
                                                                                     "TCP_Src_Port_Eq_Dst_Port_Int", 
                                                                                     "TCP_Fragment_with_Offset_Less_than_8_Int",
                                                                                     "UDP_Src_Port_Eq_Dst_Port_Int"}},
                                                                                     
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ZERO] =         {bcmRxTrapEgIpv4Ttl0, 12, 1, {"TTL_Eq_Zero_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ONE] =          {bcmRxTrapEgIpv4Ttl1, 13, 1, {"TTL_Eq_One_Int"}},
};

typedef struct
{
    bcm_rx_trap_t trap_type;
    int strength;
    int res_priority;
} sh_dnx_trap_erpp_info_t;

/* *INDENT-ON* */

/**
 * \brief
 *    Retrives info of ingress resolved trap from signals
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [out] is_trap_resolved - is trap triggered in IRPP 
 * \param [out] trap_id_p - trap id (HW code)
 * \param [out] snp_code_p - snoop code
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_ingress_resolved_trap_info_get(
    int unit,
    int core,
    uint8 *is_trap_resolved,
    uint32 *trap_id_p,
    uint32 *snp_code_p)
{
    uint32 fwd_strength = 0;
    int rv = BCM_E_NONE, rv_invalid_dest = BCM_E_NONE;
    uint32 invalid_dest_trap_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    rv_invalid_dest =
        dpp_dsig_read(unit, core, "IRPP", "LBP", "", "Header_to_Add.PPH_FHEI_5B_Ext.Code", &invalid_dest_trap_id, 1);

    rv = dpp_dsig_read(unit, core, "IRPP", "IPMF3", "", "Fwd_Action_CPU_Trap_Code", trap_id_p, 1);
    if ((rv == BCM_E_NOT_FOUND) && (rv_invalid_dest == BCM_E_NOT_FOUND))
    {
        *is_trap_resolved = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "IRPP", "", "LBP", "snoop_code", snp_code_p, 1));
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "IRPP", "", "LBP", "fwd_strength", &fwd_strength, 1));

    if (((*trap_id_p == 0) && (fwd_strength == 0)) ||
        ((*trap_id_p == *snp_code_p) && (*trap_id_p == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE)
         && (rv_invalid_dest == BCM_E_NOT_FOUND)))
    {
        *is_trap_resolved = FALSE;
    }
    else
    {
        *is_trap_resolved = TRUE;
    }

    if (rv_invalid_dest == BCM_E_NONE)
    {
        if (invalid_dest_trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_INVALID_DESTINATION)
        {
            *is_trap_resolved = TRUE;
            *trap_id_p = invalid_dest_trap_id;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints info of resolved trap for last packet.
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] trap_id - trap id (HW code)
 * \param [in] snp_code - snoop code (HW code) 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_ingress_resolved_trap_info_print(
    int unit,
    int core,
    uint32 trap_id,
    uint32 snp_code,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_config_t trap_fwd_config, trap_snp_config;
    bcm_rx_trap_t trap_type = bcmRxTrapCount, snoop_type = bcmRxTrapCount;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    uint8 is_alloc = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&trap_fwd_config);
    bcm_rx_trap_config_t_init(&trap_snp_config);

    if (trap_id != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_INVALID_DESTINATION)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, trap_id, &trap_type));
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, trap_id, &is_alloc));
    }
    else
    {
        is_alloc = TRUE;
    }

    if (is_alloc == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
        sal_printf("Resolved IRPP Forward Action Trap Type: %s\n", trap_name);
        sal_printf("Forward Action configuration can not be presented, since trap is no longer allocated\n");
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_fwd_config));
        SHR_IF_ERR_EXIT(dnx_trap_action_info_irpp_print(unit, "Resolved IRPP Forward Action", trap_type, trap_id,
                                                        &trap_fwd_config, sand_control));
    }

    if (snp_code < DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_ITMH_SNIF_CODE)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, snp_code, &snoop_type));

        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, snp_code, &is_alloc));
        if (is_alloc == FALSE)
        {
            trap_snp_config.snoop_cmnd = -1;
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, snp_code, &trap_snp_config));
        }
    }

    SHR_IF_ERR_EXIT(dnx_trap_snp_action_info_irpp_print(unit, "Resolved IRPP Snoop Action", snoop_type, snp_code,
                                                        &trap_snp_config, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of considered traps for last packet fwd action.
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */

static shr_error_e
sh_dnx_trap_last_pkt_ingress_fwd_considered_traps_info_print(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint32 cur_trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE;
    uint32 prev_trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE;
    uint32 block_i, strength;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    char *code_name;
    char *strength_name;
    char *block_name;
    bcm_rx_trap_t trap_type;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("IRPP Considered traps for packet - Forward");
    PRT_COLUMN_ADD("Trap type");
    PRT_COLUMN_ADD("Trap id (HW code)");
    PRT_COLUMN_ADD("Forward strength");

    for (block_i = 0; block_i < SH_DNX_TRAP_INGRESS_NOF_BLOCKS; block_i++)
    {
        block_name = (char *) ingress_sig_list[block_i].block;
        code_name = (char *) ingress_sig_list[block_i].trap_code;
        strength_name = (char *) ingress_sig_list[block_i].fwd_strength;
        ;
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "IRPP", block_name, "", code_name, &cur_trap_code, 1));

        if ((cur_trap_code != prev_trap_code) &&
            (cur_trap_code < DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_ITMH_SNIF_CODE))
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, cur_trap_code, &trap_type));
            SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("bcmRxTrap%s", trap_name);
            PRT_CELL_SET("0x%x", cur_trap_code);

            SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "IRPP", block_name, "", strength_name, &strength, 1));
            PRT_CELL_SET("%d", strength);
        }

        prev_trap_code = cur_trap_code;
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of considered traps for last packet snoop action.
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */

static shr_error_e
sh_dnx_trap_last_pkt_ingress_snoop_considered_traps_info_print(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint32 cur_trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE;
    uint32 prev_trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE;
    uint32 block_i, strength;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    char *code_name;
    char *strength_name;
    char *block_name;
    bcm_rx_trap_t trap_type;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("IRPP Considered traps for packet - Snoop");
    PRT_COLUMN_ADD("Trap type");
    PRT_COLUMN_ADD("Snoop Code");
    PRT_COLUMN_ADD("Snoop strength");

    for (block_i = 0; block_i < SH_DNX_TRAP_INGRESS_NOF_BLOCKS; block_i++)
    {
        block_name = (char *) ingress_sig_list[block_i].block;
        code_name = (char *) ingress_sig_list[block_i].snoop_code;
        strength_name = (char *) ingress_sig_list[block_i].snoop_strength;

        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "IRPP", block_name, "", code_name, &cur_trap_code, 1));

        if ((cur_trap_code != prev_trap_code) &&
            (cur_trap_code < DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_ITMH_SNIF_CODE))
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, cur_trap_code, &trap_type));
            SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("bcmRxTrap%s", trap_name);
            PRT_CELL_SET("0x%x", cur_trap_code);

            if (strength_name == NULL)
            {
                PRT_CELL_SET("--");
            }
            else
            {
                SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "IRPP", block_name, "", strength_name, &strength, 1));
                PRT_CELL_SET("%d", strength);
            }
        }

        prev_trap_code = cur_trap_code;
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that shows last packet trap info.
 * \param [in] unit - unit ID
 * \param [in] core - core id 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_ingress_info_cmd(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint32 trap_id = 0, snp_code = 0;
    uint8 is_trap_resolved = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_resolved_trap_info_get(unit, core, &is_trap_resolved, &trap_id,
                                                                        &snp_code));

    if (is_trap_resolved)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_resolved_trap_info_print(unit, core, trap_id, snp_code,
                                                                              sand_control));

        if (trap_id != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE)
        {
            SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_fwd_considered_traps_info_print(unit, core, sand_control));
        }

        if (snp_code != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE)
        {
            SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_snoop_considered_traps_info_print(unit, core, sand_control));
        }
    }
    else
    {
        sal_printf("No IRPP trap was resolved\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Retrives info of ERPP resolved trap from signals
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [out] is_fwd_resolved - is forward action resolved in ERPP 
 * \param [out] is_snp_resolved - is snoop action resolved in ERPP 
 * \param [out] fwd_act_profile_p - Forward Action Profile
 * \param [out] snp_act_profile_p - Snoop Action Profile
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_resolved_trap_info_get(
    int unit,
    int core,
    uint8 *is_fwd_resolved,
    uint8 *is_snp_resolved,
    uint32 *fwd_act_profile_p,
    uint32 *snp_act_profile_p)
{
    uint32 fwd_strength = 0, snp_strength = 0;
    int rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rv = dpp_dsig_read(unit, core, "ERPP", "", "ETMR", "Fwd_Action_Profile_Index", fwd_act_profile_p, 1);
    if (rv == BCM_E_NOT_FOUND)
    {
        *is_fwd_resolved = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ERPP", "", "ETMR", "Fwd_Strength", &fwd_strength, 1));
    if ((*fwd_act_profile_p == 0) && (fwd_strength == 0))
    {
        *is_fwd_resolved = FALSE;
    }
    else
    {
        *is_fwd_resolved = TRUE;
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ERPP", "", "EPMF", "Snoop_Action_Profile_Index", snp_act_profile_p, 1));
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ERPP", "", "EPMF", "Snoop_Strength", &snp_strength, 1));
    if ((*snp_act_profile_p == 0) && (snp_strength == 0))
    {
        *is_snp_resolved = FALSE;
    }
    else
    {
        *is_snp_resolved = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ERPP last info diag
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [out] fwd_considered_traps_list - traps considered for fwd action resolution
 * \param [out] snp_considered_traps_list - traps considered for snoop action resolution
 * \param [out] resolved_fwd_info - fwd action resolved info
 * \param [out] resolved_snp_info - snp action resolve info
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_considered_traps_info_get(
    int unit,
    int core,
    sh_sand_control_t * sand_control,
    bcm_rx_trap_t * fwd_considered_traps_list,
    bcm_rx_trap_t * snp_considered_traps_list,
    sh_dnx_trap_erpp_info_t * resolved_fwd_info,
    sh_dnx_trap_erpp_info_t * resolved_snp_info)
{
    bcm_gport_t gport_get = BCM_GPORT_INVALID;
    int nof_fwd_considered_traps = 0, nof_snp_considered_traps = 0;
    bcm_rx_trap_t cur_trap_type = bcmRxTrapCount;
    int cur_fwd_strength = 0, cur_snp_strength = 0, cur_trap_res_priority = 0;
    int trap_ind, trap_sig_ind, rv;
    uint32 hit_sig_val;
    SHR_FUNC_INIT_VARS(unit);

    resolved_fwd_info->trap_type = bcmRxTrapCount;
    resolved_snp_info->trap_type = bcmRxTrapCount;

    /** Run on all traps interrupt signals and look for hit indications */
    for (trap_ind = 0; trap_ind < DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES; trap_ind++)
    {
        for (trap_sig_ind = 0; trap_sig_ind < erpp_hit_sig_list[trap_ind].nof_hit_signals; trap_sig_ind++)
        {
            rv = dpp_dsig_read(unit, core, "ERPP", "", "", erpp_hit_sig_list[trap_ind].hit_signals_names[trap_sig_ind],
                               &hit_sig_val, 1);
            if ((rv == BCM_E_NOT_FOUND))
            {
                /** In case of adapter, no interrupt signals */
                SHR_EXIT();
            }

            if (hit_sig_val)
            {
                cur_trap_type = erpp_hit_sig_list[trap_ind].trap_type;
                cur_trap_res_priority = erpp_hit_sig_list[trap_ind].res_priority;

                SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(0, cur_trap_type, &gport_get));
                cur_fwd_strength = BCM_GPORT_TRAP_GET_STRENGTH(gport_get);
                cur_snp_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(gport_get);

                if (cur_fwd_strength != 0)
                {
                    fwd_considered_traps_list[nof_fwd_considered_traps] = cur_trap_type;
                    nof_fwd_considered_traps++;

                    if ((cur_fwd_strength > resolved_fwd_info->strength) ||
                        ((cur_fwd_strength == resolved_fwd_info->strength)
                         && (cur_trap_res_priority > resolved_fwd_info->res_priority)))
                    {
                        resolved_fwd_info->trap_type = cur_trap_type;
                        resolved_fwd_info->strength = cur_fwd_strength;
                        resolved_fwd_info->res_priority = cur_trap_res_priority;
                    }
                }

                if (cur_snp_strength != 0)
                {
                    snp_considered_traps_list[nof_snp_considered_traps] = cur_trap_type;
                    nof_snp_considered_traps++;

                    if ((cur_snp_strength > resolved_snp_info->strength) ||
                        ((cur_snp_strength == resolved_snp_info->strength)
                         && (cur_trap_res_priority > resolved_snp_info->res_priority)))
                    {
                        resolved_snp_info->trap_type = cur_trap_type;
                        resolved_snp_info->strength = cur_snp_strength;
                        resolved_snp_info->res_priority = cur_trap_res_priority;
                    }
                }

                /** If one of signals was hit skip to next trap */
                break;
            }
        }
    }

    fwd_considered_traps_list[nof_fwd_considered_traps] = bcmRxTrapCount;
    snp_considered_traps_list[nof_snp_considered_traps] = bcmRxTrapCount;

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    A diagnostics function that prints ERPP resolved trap (forward) for last packet.
 * \param [in] unit - unit ID
 * \param [in] core - core ID 
 * \param [in] resolved_fwd_trap_type - resolved trap for forward action 
 * \param [in] fwd_profile_sig_val - forward action profile signal value
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_resolved_fwd_trap_info_print(
    int unit,
    int core,
    bcm_rx_trap_t resolved_fwd_trap_type,
    uint32 fwd_profile_sig_val,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t trap_gport_get;
    int trap_id = -1;
    uint8 is_alloc = FALSE;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    if (resolved_fwd_trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, resolved_fwd_trap_type, &trap_gport_get));
        trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport_get);
    }

    if ((resolved_fwd_trap_type == bcmRxTrapCount) || (fwd_profile_sig_val != DNX_RX_TRAP_ID_TYPE_GET(trap_id)))
    {
        resolved_fwd_trap_type = bcmRxTrapEgUserDefine;
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ERPP, DNX_RX_TRAP_CLASS_USER_DEFINED, fwd_profile_sig_val);
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, trap_id, &is_alloc));
        if (is_alloc == FALSE)
        {
            sal_printf("Resolved ERPP Forward Action Profile: %d\n", fwd_profile_sig_val);
            sal_printf("Forward Action configuration can not be presented, since profile is no longer allocated\n");
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));

    SHR_IF_ERR_EXIT(dnx_trap_action_info_erpp_print(unit, "Resolved ", resolved_fwd_trap_type, trap_id,
                                                    &trap_config, sand_control));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ERPP resolved trap (snoop) for last packet.
 * \param [in] unit - unit ID
 * \param [in] core - core ID 
 * \param [in] resolved_snp_trap_type - resolved trap for snoop action
 * \param [in] snp_profile_sig_val - snoop action profile signal value 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_resolved_snp_trap_info_print(
    int unit,
    int core,
    bcm_rx_trap_t resolved_snp_trap_type,
    uint32 snp_profile_sig_val,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t trap_gport_get;
    int snp_act_profile = -1;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Resolved ERPP Trap Snoop Action");
    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Value");

    if (resolved_snp_trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, resolved_snp_trap_type, &trap_gport_get));
        snp_act_profile = BCM_GPORT_TRAP_GET_ID(trap_gport_get);
    }

    if ((resolved_snp_trap_type == bcmRxTrapCount) || (snp_profile_sig_val != snp_act_profile))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Trap type");
        PRT_CELL_SET("bcmRxTrapEgUserDefine");

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop action profile");
        PRT_CELL_SET("%d", snp_profile_sig_val);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, resolved_snp_trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Trap type");
        PRT_CELL_SET("bcmRxTrap%s", trap_name);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop action profile");
        PRT_CELL_SET("%d", snp_act_profile);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop strength");
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport_get));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of considered traps for last packet.
 * \param [in] unit - unit ID
 * \param [in] fwd_considered_traps_list - list of considered traps for forward action 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_considered_fwd_traps_info_print(
    int unit,
    bcm_rx_trap_t * fwd_considered_traps_list,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_t cur_trap_type = fwd_considered_traps_list[0];
    int trap_ind = 0, trap_id;
    bcm_gport_t trap_gport_get;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ERPP Considered traps for packet - Forward");
    PRT_COLUMN_ADD("Trap type");
    PRT_COLUMN_ADD("Trap id");
    PRT_COLUMN_ADD("Forward action profile");
    PRT_COLUMN_ADD("Forward strength");

    while (cur_trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, cur_trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("bcmRxTrap%s", trap_name);

        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, cur_trap_type, &trap_gport_get));
        trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport_get);

        PRT_CELL_SET("0x%x", trap_id);
        PRT_CELL_SET("%d", DNX_RX_TRAP_ID_TYPE_GET(trap_id));
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(trap_gport_get));

        cur_trap_type = fwd_considered_traps_list[++trap_ind];
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of considered traps for last packet.
 * \param [in] unit - unit ID
 * \param [in] snp_considered_traps_list - list of considered traps for snoop action 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_considered_snp_traps_info_print(
    int unit,
    bcm_rx_trap_t * snp_considered_traps_list,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_t cur_trap_type = snp_considered_traps_list[0];
    int trap_ind = 0;
    bcm_gport_t trap_gport_get;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ERPP Considered traps for packet - Snoop");
    PRT_COLUMN_ADD("Trap type");
    PRT_COLUMN_ADD("Snoop action profile");
    PRT_COLUMN_ADD("Snoop strength");

    while (cur_trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, cur_trap_type, &trap_gport_get));

        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, cur_trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("bcmRxTrap%s", trap_name);
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_ID(trap_gport_get));
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport_get));

        cur_trap_type = snp_considered_traps_list[++trap_ind];
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ERPP last info diag
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_info_cmd(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint8 is_fwd_resolved = FALSE, is_snp_resolved = FALSE;
    uint32 fwd_act_profile = 0, snp_act_profile = 0;
    bcm_rx_trap_t fwd_considered_traps_list[DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES] = { 0 };
    bcm_rx_trap_t snp_considered_traps_list[DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES] = { 0 };
    sh_dnx_trap_erpp_info_t resolved_fwd_info = { 0 };
    sh_dnx_trap_erpp_info_t resolved_snp_info = { 0 };
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_resolved_trap_info_get
                    (unit, core, &is_fwd_resolved, &is_snp_resolved, &fwd_act_profile, &snp_act_profile));
    if (is_fwd_resolved == FALSE && is_snp_resolved == FALSE)
    {
        sal_printf("No ERPP trap was resolved\n");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_considered_traps_info_get(unit, core, sand_control,
                                                                        fwd_considered_traps_list,
                                                                        snp_considered_traps_list,
                                                                        &resolved_fwd_info, &resolved_snp_info));

    if (is_fwd_resolved)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_resolved_fwd_trap_info_print(unit, core, resolved_fwd_info.trap_type,
                                                                               fwd_act_profile, sand_control));
    }
    else
    {
        sal_printf("No ERPP trap forward action was resolved\n");
    }

    if (is_snp_resolved)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_resolved_snp_trap_info_print(unit, core, resolved_snp_info.trap_type,
                                                                               snp_act_profile, sand_control));
    }
    else
    {
        sal_printf("No ERPP trap snoop action was resolved\n");
    }

    if (resolved_fwd_info.trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_considered_fwd_traps_info_print
                        (unit, fwd_considered_traps_list, sand_control));
    }

    if (resolved_snp_info.trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_considered_snp_traps_info_print
                        (unit, snp_considered_traps_list, sand_control));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Map oam trap type and id from forward action profile and oam sub-type.
 * \param [in] unit - unit ID
 * \param [in] fwd_act_profile - forward action profile 
 * \param [in] oam_sub_type - OAM sub type  
 * \param [out] oam_trap_id_p - OAM trap id encoded
 * \param [out] trap_type_p - trap type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static void
sh_dnx_trap_last_pkt_etpp_oam_trap_info_get(
    int unit,
    uint32 fwd_act_profile,
    uint8 oam_sub_type,
    int *oam_trap_id_p,
    bcm_rx_trap_t * trap_type_p)
{
    dnx_rx_trap_etpp_oam_types_e oam_trap_type;

    switch (oam_sub_type)
    {
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT:
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588:
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP:
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM:
            switch (fwd_act_profile)
            {
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_2:
                    oam_trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_OAMP;
                    *trap_type_p = bcmRxTrapEgTxOamUpMEPOamp;
                    break;
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3:
                    oam_trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1;
                    *trap_type_p = bcmRxTrapEgTxOamUpMEPDest1;
                    break;
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4:
                    oam_trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2;
                    *trap_type_p = bcmRxTrapEgTxOamUpMEPDest2;
                    break;
                default:
                    return;
            }
            break;
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOOPBACK:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_REFLECTOR;
            *trap_type_p = bcmRxTrapEgTxOamReflector;
            break;
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_LEVEL_ERR;
            *trap_type_p = bcmRxTrapEgTxOamLevel;
            break;
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_PASSIVE_ERR;
            *trap_type_p = bcmRxTrapEgTxOamPassive;
            break;
        default:
            return;
    }

    *oam_trap_id_p = DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(fwd_act_profile, oam_trap_type);
}

/**
 * \brief
 *    A diagnostics function that prints ETPP resolved trap (forward) info for last packet.
 * \param [in] unit - unit ID
 * \param [in] fwd_act_profile - forward action profile 
 * \param [in] fwd_strength - forward strength
 * \param [in] oam_sub_type - OAM sub type  
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_resolved_fwd_trap_info_print(
    int unit,
    uint32 fwd_act_profile,
    uint32 fwd_strength,
    uint8 oam_sub_type,
    sh_sand_control_t * sand_control)
{
    int oam_trap_id = -1, trap_id = -1;
    bcm_rx_trap_t trap_type = bcmRxTrapCount;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    if (fwd_act_profile == 0)
    {
        sal_printf("No ETPP trap forward action was resolved\n");
        SHR_EXIT();
    }

    if (oam_sub_type != DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL)
    {
        sh_dnx_trap_last_pkt_etpp_oam_trap_info_get(unit, fwd_act_profile, oam_sub_type, &oam_trap_id, &trap_type);
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM, oam_trap_id);
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED, fwd_act_profile);
    }

    bcm_rx_trap_config_t_init(&trap_config);
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));
    trap_config.trap_strength = fwd_strength;

    SHR_IF_ERR_EXIT(dnx_trap_action_info_etpp_print(unit, "Resolved ", trap_type, trap_id, &trap_config, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ETPP resolved trap (snoop) info for last packet.
 * \param [in] unit - unit ID
 * \param [in] snp_act_profile - snoop action profile 
 * \param [in] snp_strength - snoop strength
 * \param [in] oam_sub_type - OAM sub type  
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_resolved_snp_trap_info_print(
    int unit,
    uint32 snp_act_profile,
    uint32 snp_strength,
    uint8 oam_sub_type,
    sh_sand_control_t * sand_control)
{
    char trap_name[DBAL_MAX_STRING_LENGTH];
    int oam_trap_id = -1, trap_id = -1;
    bcm_rx_trap_t trap_type = bcmRxTrapCount;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (snp_act_profile == 0)
    {
        sal_printf("No ETPP trap snoop action was resolved\n");
        SHR_EXIT();
    }

    if (oam_sub_type != DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL)
    {
        sh_dnx_trap_last_pkt_etpp_oam_trap_info_get(unit, snp_act_profile, oam_sub_type, &oam_trap_id, &trap_type);
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM, oam_trap_id);
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED, snp_act_profile);
    }

    PRT_TITLE_SET("Resolved ETPP Trap Snoop Action");
    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Value");

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap type");
    PRT_CELL_SET("bcmRxTrap%s", trap_name);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap id");
    PRT_CELL_SET("0x%x", trap_id);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Snoop strength");
    PRT_CELL_SET("%d", snp_act_profile);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Snoop action profile");
    PRT_CELL_SET("%d", snp_strength);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Retrives info of ETPP resolved trap from signals
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [out] fwd_act_profile_p - forward action profile
 * \param [out] fwd_strength_p - forward strength 
 * \param [out] snp_act_profile_p - snoop action profile
 * \param [out] snp_strength_p - snoop strength
 * \param [out] oam_sub_type_p - OAM sub type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_resolved_trap_info_get(
    int unit,
    int core,
    uint32 *fwd_act_profile_p,
    uint32 *fwd_strength_p,
    uint32 *snp_act_profile_p,
    uint32 *snp_strength_p,
    uint32 *oam_sub_type_p)
{
    int rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rv = dpp_dsig_read(unit, core, "ETPP", "", "Trap", "Fwd_Action_Profile", fwd_act_profile_p, 1);
    if ((rv == BCM_E_NOT_FOUND))
    {
        *fwd_act_profile_p = 0;
        *fwd_strength_p = 0;
        *snp_act_profile_p = 0;
        *snp_strength_p = 0;
        *oam_sub_type_p = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ETPP", "", "Trap", "Fwd_Action_Strength", fwd_strength_p, 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ETPP", "", "Trap", "Snoop_Action_Profile", snp_act_profile_p, 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ETPP", "", "Trap", "Snoop_Action_Strength", snp_strength_p, 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ETPP", "", "FWD", "OAM_Sub_Type", oam_sub_type_p, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ETPP last info diag
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_info_cmd(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint32 fwd_act_profile = 0, fwd_strength = 0, snp_act_profile = 0, snp_strength = 0, oam_sub_type = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_resolved_trap_info_get(unit, core, &fwd_act_profile, &fwd_strength,
                                                                     &snp_act_profile, &snp_strength, &oam_sub_type));

    if ((fwd_act_profile == 0) && (snp_act_profile == 0))
    {
        sal_printf("No ETPP trap was resolved\n");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_resolved_fwd_trap_info_print(unit, fwd_act_profile, fwd_strength,
                                                                           oam_sub_type, sand_control));

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_resolved_snp_trap_info_print(unit, snp_act_profile, snp_strength,
                                                                           oam_sub_type, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that shows user-defined trap info.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_block_e trap_block;
    int core_in, core_id;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("block", trap_block);
    SH_SAND_GET_INT32("core", core_in);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        sal_printf("\nCore %d:", core_id);
        sal_printf("\n-------\n");

        switch (trap_block)
        {
            case DNX_RX_TRAP_BLOCK_INGRESS:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_info_cmd(unit, core_id, sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_ERPP:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_info_cmd(unit, core_id, sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_ETPP:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_info_cmd(unit, core_id, sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_NUM_OF:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_info_cmd(unit, core_id, sand_control));
                sal_printf("\n");
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_info_cmd(unit, core_id, sand_control));
                sal_printf("\n");
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_info_cmd(unit, core_id, sand_control));
                break;
            default:
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }

        sal_printf("\n");
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_trap_last_pkt_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"INFo", sh_dnx_trap_last_pkt_info_cmd, NULL, dnx_trap_last_pkt_info_options, &dnx_trap_last_pkt_info_man}
    ,
    {NULL}
};
