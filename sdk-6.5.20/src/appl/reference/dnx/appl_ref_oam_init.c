/** \file appl_ref_oam_appl_init.c
 * 
 *
 * OAM application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include "appl_ref_oam_init.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
/* #include <soc/drv.h> */
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/**
 * Defult channel type value for Y1731 over PWE
 */
#define DNX_OAM_PWE_CHANNEL_TYPE_VALUE 0x27
/**
 * Defult channel type value for Y1731 over MPLS TP
 */
#define DNX_OAM_Y1731_MPLS_TP_CHANNEL_TYPE_VALUE 0x8902
/**
 * Defult channel type value for BFD over PWE
 */
#define DNX_PWE_BFD_CHANNEL_TYPE_VALUE 0x7

/** Reserved value for punt "trap ID" */
#define DNX_OAM_PUNT_MSB 0x400

/**
 *  Definitions for ingress and egress sides
 */
#define EGRESS_SIDE 0
#define INGRESS_SIDE 1

#define NOT_ACCELERATED 0
#define ACCELERATED 1
/**
 *  Initial value for flags
 */
#define BCM_OAM_NO_FLAG 0

/** Snoop command */
#define SNOOP_MIRROR_ID 1

/**
 * \brief - used to apply correct oam_offset value in PPH header
 */
#define APPL_DNX_OAM_FORWARDING_INDEX_VALUE 7

/**
 * Table for all supported opcodes.
 */
bcm_oam_opcode_t supported_opcodes[] = {
    bcmOamOpcodeCCM,
    bcmOamOpcodeLBR,
    bcmOamOpcodeLBM,
    bcmOamOpcodeLTR,
    bcmOamOpcodeLTM,
    bcmOamOpcodeAIS,
    bcmOamOpcodeLCK,
    bcmOamOpcodeLinearAPS,
    bcmOamOpcodeLMR,
    bcmOamOpcodeLMM,
    bcmOamOpcode1DM,
    bcmOamOpcodeDMR,
    bcmOamOpcodeDMM,
    bcmOamOpcodeSLR,
    bcmOamOpcodeSLM
};

typedef struct
{
/** ids for traps */
    int downmep_trap_id;
    int upmep_trap_id;
    int level_trap_id;
    int recycle_trap_id;
    int snoop_trap_id;
    int passive_trap_id;
    int egr_level_trap_id;
    int egr_snoop_trap_id;
    int egr_passive_trap_id;
    int oamp_eth_trap_id;
    int oamp_eth_performance_trap_id;
    int oamp_mpls_trap_id;
    int oamp_mpls_performance_trap_id;
    int oamp_pwe_trap_id;
    int oamp_pwe_performance_trap_id;
    int oamp_up_mep_trap_id;
    int downmep_reflector_trap_id;
    int upmep_reflector_trap_id;
}
oam_traps_t;

typedef struct
{
    /** ids for traps */
    int ipv4_trap_id;
    int ipv6_trap_id;
    int mpls_trap_id;
    int pwe_trap_id;
}
bfd_traps_t;
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

/**
 * \brief
 *   This function allocates a trap_id and configures the trap
 *
 * \param [in] unit - The unit number.
 * \param [in] trap_type - The trap of type that we want to configure (related to bcm_rx_trap_t enum)
 * \param [out] trap_id - The id of the trap that was created
 * \param [in] trap_config - Pointer to trap configuration (related to bcm_rx_trap_config_t struct)
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_rx_trap_t
 *   bcm_rx_trap_config_t
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_oam_trap_config(
    int unit,
    bcm_rx_trap_t trap_type,
    int *trap_id,
    bcm_rx_trap_config_t * trap_config)
{
    bcm_error_t rv;
    int flags = 0;

    rv = bcm_rx_trap_type_create(unit, flags, trap_type, trap_id);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    rv = bcm_rx_trap_set(unit, *trap_id, trap_config);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    return rv;
}

/**
 * \brief
 *   This function allocates a snoop and a snoop-trap
 *
 * \param [in] unit - The unit number.
 * \param [in] snoop_destination - The port used for snooping
 * \param [out] oam_traps - Structure of configured trap ids
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_rx_trap_t
 *   bcm_rx_trap_config_t
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_oam_snoop_config(
    int unit,
    int snoop_destination,
    oam_traps_t * oam_traps)
{
    bcm_mirror_destination_t mirror_dest;
    bcm_rx_trap_config_t trap_config_snoop;
    int trap_code;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = snoop_destination;
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP | BCM_MIRROR_DEST_WITH_ID;
    BCM_GPORT_MIRROR_SNOOP_SET(mirror_dest.mirror_dest_id, SNOOP_MIRROR_ID);
    SHR_IF_ERR_EXIT(bcm_mirror_destination_create(unit, &mirror_dest));

    if (!BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error!  Could not create snoop.\n");
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapSnoopOamPacket, &trap_code));
    oam_traps->snoop_trap_id = trap_code;

    bcm_rx_trap_config_t_init(&trap_config_snoop);

    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_code, &trap_config_snoop));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function creates a new PMF rule that didn't add system header.
 *   PMF rule selection is done according trap.
 *
 * \param [in] unit - The unit number.
 * \param [in] presel_id - preselector id
 * \param [in] trap_code - trap code
 * \param [in] trap_mask - trap mask
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 */
static shr_error_e
appl_dnx_field_trap_no_system_header(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_field_context_t context_id,
    uint32 trap_code,
    uint32 trap_mask)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    bcm_field_presel_entry_id_t presel_entry_id;
    void *dest_char;

    /** Creating new context */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_No_Header", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_field_context_create failed. Error:%d \n"), rv));
        return rv;
    }

    /** Changing new context SYS HDR profile to None (No SYS HDRs are added) */
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    context_param.param_arg = 0;
    context_param.param_val = bcmFieldSystemHeaderProfileNone;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &context_param);
    if (rv != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_field_context_param_set failed. Error:%d \n"), rv));
    }

    context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    context_param.param_arg = 0;
    context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset0, 0);
    /**Context that was created for iPMF1 is also created for iPMF2*/
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &context_param);
    if (rv != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_field_context_param_set failed. Error:%d \n"), rv));
    }

    /** Adding trap_code preselector for the context */
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
    presel_data.qual_data[0].qual_value = trap_code;
    presel_data.qual_data[0].qual_mask = trap_mask;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_data);
    if (rv != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_field_presel_set failed. Error:%d \n"), rv));
        return rv;
    }

    return rv;
}
/**
 * \brief
 * This function configures the downmep field processor program.
 * Program Context Selection: Trap-Code == OAM-REFLECTOR
 * Action: Override destination with source system port
 *
 * \param [in] unit - The unit number.
 * \param [in] trap_id - OAM-REFLECTOR reflector trap-id.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 */

shr_error_e
appl_dnx_oam_downmep_reflector_fp_config(
    int unit,
    int trap_id)
{
    bcm_field_action_info_t action_info;
    bcm_field_action_t action;
    bcm_field_group_t downmep_reflector_fg_id;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_info_t fg_info;
    int qual_index;
    bcm_field_context_info_t context_info;
    bcm_field_context_t context_id;
    char *dest_char;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF2;
    SHR_FUNC_INIT_VARS(unit);

    /** Action - override destination to be source system port*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionForward;
    /** Set the prefix which mark ActionForward as system-port*/
    action_info.prefix_size = 16;
    action_info.prefix_value = 0xc;
    action_info.size = 16;
    action_info.stage = stage;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action));

    /** define a field group with source system port will be used as a key - direct extraction*/
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 1;
    fg_info.stage = stage;
    fg_info.qual_types[0] = bcmFieldQualifySrcPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action;
    fg_info.action_with_valid_bit[0] = FALSE;
    dest_char = (char *) &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "DEST<-SSP", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &downmep_reflector_fg_id));

    /** Creating new context */
    bcm_field_context_info_t_init(&context_info);
    dest_char = (char *) &(context_info.name[0]);
    sal_strncpy_s(dest_char, "OAM DOWNMEP REFLECTOR", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dnx_data_field.context.default_oam_reflector_context_get(unit);
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = trap_id;
    p_data.qual_data[0].qual_mask = 0x1FF;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /** attach to context */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        attach_info.key_info.qual_types[qual_index] = fg_info.qual_types[qual_index];
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    for (qual_index = 0; qual_index < fg_info.nof_actions; qual_index++)
    {
        attach_info.payload_info.action_types[qual_index] = fg_info.action_types[qual_index];
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, downmep_reflector_fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * This function configures downmep reflector application.
 * Downmap reflector walkthrough:
 * 1. OAM Ingress classifier - classify the packet as loopback message and set a OAM reflector trap code
 * 2. PMF2 - In a case OAM reflector trap code is set, overrides the destination to source system port
 * 3. System headers Traps - In a case OAM reflector trap code is set, override OUTLIF to be OutLIF of type ETPS_OAM_REFLECTOR
 * 4. Egress - In a case of a  OutLIF of type ETPS_OAM_REFLECTOR - SA <--> DA will be swapped and opcode will be changed to LBR
 *
 * Step #1 is configured in TBD
 * Steps #2 and #3 implemented this function
 * Step #4 does not required SW configuration
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   Error code (as per 'bcm_error_e').
 */

static shr_error_e
appl_dnx_oam_downmep_reflector_init(
    int unit,
    oam_traps_t * oam_traps)
{
    bcm_if_t downmep_oam_reflector_encap_id;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t lif_gport;
    SHR_FUNC_INIT_VARS(unit);

    /** allocate OAM reflector OUTLIF */
    SHR_IF_ERR_EXIT(bcm_oam_reflector_encap_create(unit, 0, &downmep_oam_reflector_encap_id));

    /** allocate and set trap */
    bcm_rx_trap_config_t_init(&trap_config);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(lif_gport, downmep_oam_reflector_encap_id);
    trap_config.encap_id = lif_gport;
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config
                    (unit, bcmRxTrapOamReflector, &oam_traps->downmep_reflector_trap_id, &trap_config));

    /** create field processor program */
    SHR_IF_ERR_EXIT(appl_dnx_oam_downmep_reflector_fp_config(unit, oam_traps->downmep_reflector_trap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function configures upmep reflector application.
 * Upmap reflector walkthrough:
 * 1. OAM egress classifier - classify the packet as loopback message and set a egress OAM reflector trap code and subtype=Loopback
 * 2. Egress Trap - set recycle priority
 * 3. if
 * 4. Egress - In a case of a  OutLIF of type ETPS_OAM_REFLECTOR - SA <--> DA will be swapped and opcode will be changed to LBR
 *
 * Step #1 is configured in TBD
 * Steps #2 and #3 implemented this function
 * Step #4 does not required SW configuration
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   Error code (as per 'bcm_error_e').
 */

static shr_error_e
appl_dnx_oam_upmep_reflector_init(
    int unit,
    oam_traps_t * oam_traps)
{
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    /** allocate and set trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.is_recycle_high_priority = 0;
    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config
                    (unit, bcmRxTrapEgTxOamReflector, &oam_traps->upmep_reflector_trap_id, &trap_config));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * This function configures the UP Mep traps(egress side):
 * Trap for accelerated endpoints(recycled and after this trapped to OAMP):
 *                bcmRxTrapEgTxOAMUPMEPOAMP - ETH-OAM

 * Trap for non-accelerated endpoints point(recycled and after this trapped to CPU):
 *                bcmRxTrapEgTxOamUpMEPDest1
 * Traps for errors(trapped to CPU):
 *                bcmRxTrapEgTxOamPassive        - Passive Filtering
 *                bcmRxTrapEgTxOamLevel          - Level Filtering
 * Snoop configuration for MIP
 *                This is done via mirror mechanism
 *
 * \param [in] unit -
 *   The unit number.
 * \param[in] cpu_port
 * \param[in] oamp_port
 * \param [out] oam_traps -
 *   Structure of configured trap ids
 * \return
 *   Error code (as per 'bcm_error_e').
 * \ remark
 *   See bcm_rx_trap_t enum
 *   See bcm_port_config_t struct
 */

static shr_error_e
appl_dnx_oam_up_mep_trap_init(
    int unit,
    bcm_gport_t cpu_port,
    bcm_gport_t oamp_gport,
    oam_traps_t * oam_traps)
{
    bcm_rx_trap_config_t trap_config;
    int up_mep_ingress_trap_id;
    int up_mep_ingress_cpu_trap_id;
    bcm_mirror_destination_t mirror_dest;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_field_context_t context_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Define User defined traps(on ingress side) in order not to add system headers on 2'nd pass */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.dest_port = oamp_gport;
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config(unit, bcmRxTrapUserDefine, &up_mep_ingress_trap_id, &trap_config));

    bcm_rx_trap_config_t_init(&trap_config);

    /** Ingress trap strength range 0-15 */
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, up_mep_ingress_trap_id, 15, 0);
    /** Trap stamped on FHEI header */
    trap_config.stamped_trap_code = oam_traps->oamp_eth_trap_id;

    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config
                    (unit, bcmRxTrapEgTxOamUpMEPOamp, &oam_traps->oamp_up_mep_trap_id, &trap_config));

    /** Set PMF rule */
    context_id = dnx_data_field.context.default_oam_context_get(unit);
    SHR_IF_ERR_EXIT(appl_dnx_field_trap_no_system_header(unit, 1, context_id, up_mep_ingress_trap_id, 0x1ff));

    /**********************************************************************************/
    /** Define UP MEP traps for CPU(Dest1) */
    /**********************************************************************************/

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.dest_port = cpu_port;
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config(unit, bcmRxTrapUserDefine, &up_mep_ingress_cpu_trap_id, &trap_config));

    bcm_rx_trap_config_t_init(&trap_config);

    /** Ingress trap strength range 0-15  */
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, up_mep_ingress_cpu_trap_id, 15, 0);
    /** For backward compatibility same trap id used for OAMP and CPU in FHEI */
    trap_config.stamped_trap_code = oam_traps->oamp_eth_trap_id;
    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config
                    (unit, bcmRxTrapEgTxOamUpMEPDest1, &oam_traps->upmep_trap_id, &trap_config));

    /** Set PMF rule */
    context_id = dnx_data_field.context.default_oam_upmep_context_get(unit);
    SHR_IF_ERR_EXIT(appl_dnx_field_trap_no_system_header(unit, 2, context_id, up_mep_ingress_cpu_trap_id, 0x1ff));

    /**********************************************************************************/
    /** Define UP MEP traps for Level                                                 */
    /**********************************************************************************/
    bcm_rx_trap_config_t_init(&trap_config);

    /** Ingress trap strength range 0-15  */
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, oam_traps->level_trap_id, 15, 0);

    /** Trap id stamped on FHEI header */
    trap_config.stamped_trap_code = oam_traps->level_trap_id;
    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config(unit, bcmRxTrapEgTxOamLevel, &oam_traps->egr_level_trap_id, &trap_config));

    /**********************************************************************************/
    /** Define UP MEP traps for Passive                                               */
    /**********************************************************************************/
    bcm_rx_trap_config_t_init(&trap_config);

    /** Ingress trap strength range 0-15  */
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, oam_traps->passive_trap_id, 15, 0);

    /** Trap id stamped on FHEI system header */
    trap_config.stamped_trap_code = oam_traps->passive_trap_id;
    SHR_IF_ERR_EXIT(appl_dnx_oam_trap_config
                    (unit, bcmRxTrapEgTxOamPassive, &oam_traps->egr_passive_trap_id, &trap_config));

    /**********************************************************************************/
    /** Define UP MEP traps/snoop for MIP                                             */
    /**********************************************************************************/

    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = cpu_port;
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP | BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;
    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    SHR_IF_ERR_EXIT(bcm_mirror_destination_create(unit, &mirror_dest));

    /*
     * Call mirror_header_info_set() in addition to ensure that snooped packet will come with an FHEI
     */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = cpu_port;       /* This field may optionally be set to a unique port and Up MEP
                                                         * snoop packets may be recognized according to that. In this
                                                         * example use a common CPU port. */
    SHR_IF_ERR_EXIT(bcm_mirror_header_info_set
                    (unit, BCM_MIRROR_DEST_IS_SNOOP, mirror_dest.mirror_dest_id, &mirror_header_info));
    /** Used for bcm_oam_profile_action_set */
    oam_traps->egr_snoop_trap_id = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);
    /**
      * Up MEP snoop packets will come with a sniff/trap FHEI and
      * the trap code being that associated with
      * (bcmRxTrapEgTxFieldSnoop0+ mirror_dest.mirror_dest_id).
      *
      * in other words the trap_code returned from
      * bcm_rx_trap_type_get(unit,0,bcmRxTrapEgTxFieldSnoop0+mirror_dest.mirror_dest_id,&trap_code);
      *
      * After the first set of system headers, egress snooped
      * packets will have exactly 64 bytes of the original packets,
      * including system headers from the egress.
      * This is due to the flag
      * BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER.
      * After those 64 bytes the snooped packet will include the
      * full original network header.
      */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function initializes the protection trap,
 * or a single punt trap.  First the trap type
 * is added to the trap type table, then the
 * trap is added to the punt destinations table.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] trap_type -
 *   Punt trap type.
 * \param [in] cpu_port -
 *   CPU port (destination for punt packets).
 * \return
 *   Error code (as per 'bcm_error_e').
* \remark
*   * None
* \see
*   * None
 */
static shr_error_e
appl_dnx_oam_single_punt_config(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_gport_t cpu_port)
{
    bcm_rx_trap_config_t trap_config;
    int trap_id = 0, rv;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&trap_config);

    /** CPU port */
    trap_config.dest_port = cpu_port;

    rv = bcm_rx_trap_type_create(unit, trap_config.flags, trap_type, &trap_id);
    if (BCM_FAILURE(rv))
    {
        SHR_ERR_EXIT(rv, "punt: bcm_rx_trap_type_create failed.");
    }

    trap_config.flags |= BCM_RX_TRAP_TRAP;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (BCM_FAILURE(rv))
    {
        SHR_ERR_EXIT(rv, "punt: bcm_rx_trap_set failed.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function initializes the OAMP punt traps.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] cpu_port -
 *   CPU port (destination for punt packets).
 * \return
 *   Error code (as per 'bcm_error_e').
* \remark
*   * None
* \see
*   * None
 */
static shr_error_e
appl_dnx_oam_punt_config(
    int unit,
    bcm_gport_t cpu_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** punt config */
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampChanTypeMissErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampTypeErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampRmepErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampMaidErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampFlexCrcMissErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampMdlErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampCcmIntrvErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampMyDiscErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampSrcIpErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampYourDiscErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampSrcPortErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampRmepStateChange, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampParityErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampPuntNextGoodPacket, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampTimestampErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampTrapErr, cpu_port));
    SHR_IF_ERR_EXIT(appl_dnx_oam_single_punt_config(unit, bcmRxTrapOampMplsLmDmErr, cpu_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function configures the Mep traps:
 * Traps for accelerated endpoints(trapped to OAMP):
 *                bcmRxTrapOamEthAccelerated - ETH-OAM
 *                bcmRxTrapOamY1731MplsTp    - Y.1731 over MPLS-TP
 *                bcmRxTrapOamY1731Pwe       - Y.1731 over PWE
 * Trap for non-accelerated endpoints point(trapped to CPU):
 *                bcmRxTrapBfdOamDownMEP     - Y.1731 for non-accelerated endpoints and BFD
 *                bcmRxTrapOamUpMEP          - Y.1731 for
 *                non-accelerated endpoints and BFD
 *                bcmRxTrapRecycleOamPacket  - Recycle
 *
 * Traps for errors(trapped to CPU):
 *                bcmRxTrapOamPassive        - Passive Filtering
 *                bcmRxTrapOamLevel          - Level Filtering
 *                bcmRxTrapRedirectToCpuOamPacket - Trap to CPU
 *                bcmRxTrapSnoopOamPacket    - Snoop to CPU
 *
 * \param [in] unit -
 *   The unit number.
 * \param [out] oam_traps -
 *   Structure of configured trap ids
 * \return
 *   Error code (as per 'bcm_error_e').
 * \ remark
 *   See bcm_rx_trap_t enum
 *   See bcm_port_config_t struct
 */
static shr_error_e
appl_dnx_oam_trap_init(
    int unit,
    oam_traps_t * oam_traps)
{
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t oamp_gport[2];
    bcm_gport_t cpu_port;
    bcm_port_config_t port_config;
    int count;
    int trap_id;
    int system_headers_mode;
    bcm_rx_trap_config_t_init(&trap_config);

    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_port_config_get failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Finding first CPU port
     */
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        trap_config.dest_port = cpu_port;
        break;
    }

    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oam_offset_supported))
    {
        if (system_headers_mode == 0)
        {
            /** DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE */
            trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
        }
        else
        {
            trap_config.flags |= BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX;
            trap_config.egress_forwarding_index = 7;
        }
    }
    /*
     * Configuring destination(OAMP) for accelerated endpoints
     */
    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gport, &count);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_port_internal_get failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Check if at least one OAMP port configured.
     */
    if (count < 1)
    {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit, "Error: Oamp port disabled, accelerated OAM/BFD unavailable.\n")));
    }
    else
    {
        trap_config.dest_port = oamp_gport[0];

        /*
         * Set OAM OAMP trap
         * Trap is then used in default profile.
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamEthAccelerated, &oam_traps->oamp_eth_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set OAM performance OAMP trap
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamPerformanceEthAccelerated,
                                      &oam_traps->oamp_eth_performance_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set  Y.1731 over MPLS-TP trap.
         * Trap is required for MPLS accelerated MEPs (not used in this file)
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamY1731MplsTp, &trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set  Y.1731 over MPLS-TP performance trap.
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamPerformanceY1731MplsTp,
                                      &oam_traps->oamp_mpls_performance_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set Y.1731 over PWE trap
         * Trap is required for PWE accelerated MEPs (not used in this file)
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamY1731Pwe, &trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set Y.1731 over PWE performance trap
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamPerformanceY1731Pwe,
                                      &oam_traps->oamp_pwe_performance_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    /*
     * Check if at least one CPU port is configured.
     */
    if (cpu_port >= BCM_PBMP_PORT_MAX)
    {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit, "Error: No CPU ports, redirecting packets to CPU is unavailable.\n")));
    }
    else
    {
        /*
         * Configuring destination(CPU) for non-accelerated endpoints and errors)
         */
        trap_config.dest_port = cpu_port;

        /*
         * Set OAM non-accelerated endpoints trap
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapBfdOamDownMEP, &oam_traps->downmep_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set OAM Level trap
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamLevel, &oam_traps->level_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set OAM passive trap
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamPassive, &oam_traps->passive_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         *  Set OAM snoop trap
         */

        rv = appl_dnx_oam_snoop_config(unit, cpu_port, oam_traps);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "Failed to configure snoop. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /** Part of OAMP functionality - is it enabled? */
        if (count >= 1)
        {
            rv = appl_dnx_oam_punt_config(unit, cpu_port);
            if (BCM_FAILURE(rv))
            {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit, "Failed to configure protection and punt.  Error:%d (%s)\n"), rv,
                           bcm_errmsg(rv)));
                return rv;
            }

            /** UP-MEP configuration(egress side) */
            rv = appl_dnx_oam_up_mep_trap_init(unit, cpu_port, oamp_gport[0], oam_traps);
            if (BCM_FAILURE(rv))
            {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit, "appl_dnx_oam_up_mep_trap_init. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
                return rv;
            }

        }
    }

    return rv;
}

/**
 * \brief
 * This function configures the bfd over IPv6 - UDP invalid checksum trap:
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   Error code (as per 'bcm_error_e').
 * \ remark
 *   See bcm_rx_trap_t enum
 *   See bcm_port_config_t struct
 */
static shr_error_e
appl_dnx_bfd_ipv6_invalid_udp_checksum(
    int unit)
{
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t cpu_port;
    bcm_port_config_t port_config;
    int udp_checksum_trap_id = 0;

    bcm_rx_trap_config_t_init(&trap_config);

    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_port_config_get failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;

    /*
     * Finding first CPU port
     */
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        trap_config.dest_port = cpu_port;
        break;
    }

    /*
     * max trap strength
     */
    trap_config.trap_strength = 0xf;
    trap_config.snoop_strength = 0x0;
    /*
     * Check if at least one CPU port is configured.
     */
    if (cpu_port >= BCM_PBMP_PORT_MAX)
    {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit, "Error: No CPU ports, redirecting packets to CPU is unavailable.\n")));
    }
    else
    {
        /*
         * BFD IPv6 trap for invalid UDP checksum
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapEgBfdIpv6InvalidUdpChecksum, &udp_checksum_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        }
    }

    return rv;
}

/**
 * \brief
 * This function configures the Mep traps:
 *                bcmRxTrapOamBfdIpv4   - Single-hop, micro bfd
  *               bcmRxTrapOamBfdIpv6   - Single-hop, micro bfd
 *                and  multi-hop IPv4
 *                bcmRxTrapOamBfdMpls - BFD on MPLS
 *                bcmRxTrapOamY1731Pwe  - BFD on PWE
 *
 * \param [in] unit -
 *   The unit number.
 * \param [out] bfd_traps -
 *   Structure of configured trap ids
 * \return
 *   Error code (as per 'bcm_error_e').
 * \ remark
 *   See bcm_rx_trap_t enum
 *   See bcm_port_config_t struct
 */
static shr_error_e
appl_dnx_bfd_trap_init(
    int unit,
    bfd_traps_t * bfd_traps)
{
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_config_t trap_config_bfd_ipv6;
    bcm_gport_t oamp_gport[2];
    int count;
    int system_headers_mode;

    bcm_rx_trap_config_t_init(&trap_config);
    bcm_rx_trap_config_t_init(&trap_config_bfd_ipv6);

    /*
     * Configuring destination(OAMP) for accelerated endpoints
     */
    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gport, &count);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_port_internal_get failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oam_offset_supported))
    {
        if (system_headers_mode == 0)
        {
            /** DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE */
            trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
        }
        else
        {
            trap_config.flags |= BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX;
            trap_config.egress_forwarding_index = 7;
        }
    }
    /*
     * Check if at least one OAMP port configured.
     */
    if (count < 1)
    {
        LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Error: Oamp port disabled, can't init OAM.\n")));
    }
    else
    {
        trap_config.dest_port = oamp_gport[0];

        /*
         * Set BFD over IPv4 trap
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamBfdIpv4, &bfd_traps->ipv4_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_bfd_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set BFD over IPv6 trap
         */
        sal_memcpy(&trap_config_bfd_ipv6, &trap_config, sizeof(bcm_rx_trap_config_t));
        trap_config_bfd_ipv6.flags |= (BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER);
        /*
         * For BFD over IPv6, PEMLA micro code copies SIP to the end of the packet for CRC verification in the OAMP.
         * This requires the EGRESS-FWD-INDEX and the FORWARD-HEADER both to be set to 3 (BFD PDU)
         */
        trap_config_bfd_ipv6.egress_forwarding_index = 3;       /* bfd index */
        trap_config_bfd_ipv6.forwarding_header = 3;     /* bfd index */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamBfdIpv6, &bfd_traps->ipv6_trap_id, &trap_config_bfd_ipv6);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_bfd_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set  BFD over MPLS trap.
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamBfdMpls, &bfd_traps->mpls_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_bfd_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /*
         * Set BFD over PWE trap
         */
        rv = appl_dnx_oam_trap_config(unit, bcmRxTrapOamBfdPwe, &bfd_traps->pwe_trap_id, &trap_config);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_bfd_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    return rv;
}

/**
 * \brief
 *   Sets the default OAM profile for a given port for trap.
 *   This profile may be used for Ethernet OAM MEPs
 * \param
 *   [in] unit  -  Number of hardware unit used..
 *   [in] is_ingress - 1 to set ingress profile, 0 to set egress
 *        profile
 *   [in] is_acc - 1 to set accelerated profile, 0 to set
 *        non-accelerated profile
 *   [in] profile_id - Profile identifier.
 *   [in] active_dest_gport - destination port for active
 *        matching packets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected
 * \remark
 *   * None
 * \see
 */
static shr_error_e
appl_dnx_oam_default_profile_set(
    int unit,
    uint8 is_ingress,
    const uint8 is_acc,
    bcm_oam_profile_t profile_id,
    bcm_gport_t active_dest_gport,
    const oam_traps_t * oam_traps)
{
    bcm_error_t rv;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    bcm_oam_match_type_t endpoint_type;
    bcm_oam_dest_mac_type_t dest_mac_type;
    bcm_gport_t level_dest_gport;
    bcm_gport_t cpu_dest_gport;
    bcm_gport_t snoop_dest_gport;
    bcm_gport_t oamp_dest_gport;
    bcm_gport_t passive_dest_gport;
    bcm_gport_t downmep_reflector_gport = 0;
    bcm_gport_t upmep_reflector_gport = 0;
    uint32 flags;
    int skip_config;
    int opcode_index;
    int nof_supported_opcodes;
    int endpoint_type_start;
    int endpoint_type_end;

    rv = BCM_E_NONE;
    flags = 0;
    oamp_dest_gport = 0;

    /*
     * Get ports for trap types
     */
    if (is_ingress)
    {
        BCM_GPORT_TRAP_SET(level_dest_gport, oam_traps->level_trap_id, 10, 0);
        BCM_GPORT_TRAP_SET(cpu_dest_gport, oam_traps->downmep_trap_id, 10, 0);
        BCM_GPORT_TRAP_SET(snoop_dest_gport, oam_traps->snoop_trap_id, 0, 3);
        BCM_GPORT_TRAP_SET(passive_dest_gport, oam_traps->passive_trap_id, 10, 0);
        BCM_GPORT_TRAP_SET(downmep_reflector_gport, oam_traps->downmep_reflector_trap_id, 15, 0);

        if (is_acc)
        {
            BCM_GPORT_TRAP_SET(oamp_dest_gport, oam_traps->oamp_eth_trap_id, 10, 0);
        }
    }
    else
    {
        BCM_GPORT_TRAP_SET(level_dest_gport, oam_traps->egr_level_trap_id, 7, 0);
        BCM_GPORT_TRAP_SET(cpu_dest_gport, oam_traps->upmep_trap_id, 7, 0);
        BCM_GPORT_TRAP_SET(snoop_dest_gport, oam_traps->egr_snoop_trap_id, 0, 7);
        BCM_GPORT_TRAP_SET(passive_dest_gport, oam_traps->egr_passive_trap_id, 7, 0);
        BCM_GPORT_TRAP_SET(upmep_reflector_gport, oam_traps->upmep_reflector_trap_id, 15, 0);

        if (is_acc)
        {
            BCM_GPORT_TRAP_SET(oamp_dest_gport, oam_traps->oamp_up_mep_trap_id, 7, 0);
        }

    }

    /*
     * Prepare action_key and action_result
     */
    bcm_oam_action_key_t_init(&action_key);
    bcm_oam_action_result_t_init(&action_result);
    action_key.flags = 0;
    action_key.inject = 0;

    action_result.flags = 0;
    action_result.meter_enable = 0;
    action_key.endpoint_type = 0;
    action_key.dest_mac_type = 0;
    action_result.counter_increment = 0;
    action_result.destination = BCM_GPORT_INVALID;

    nof_supported_opcodes = sizeof(supported_opcodes) / sizeof(bcm_oam_opcode_t);

    if (is_acc)
    {
        /*
         * enpoint_type is not a key for OAMB. So endpoint_type_start = endpoint_type_end = bcmOAMMatchTypeMEP
         */
        endpoint_type_start = bcmOAMMatchTypeMEP;
        endpoint_type_end = bcmOAMMatchTypeMEP;
    }
    else
    {
        /*
         *  For non-acc profile configure all endpoint types
         */
        endpoint_type_start = bcmOAMMatchTypeMIP;
        endpoint_type_end = bcmOAMMatchTypeLowMDL;
    }

    for (dest_mac_type = bcmOAMDestMacTypeMcast; dest_mac_type < bcmOAMDestMacTypeCount; dest_mac_type++)
    {
        action_key.dest_mac_type = dest_mac_type;
        for (opcode_index = 0; opcode_index < nof_supported_opcodes; opcode_index++)
        {
            action_key.opcode = supported_opcodes[opcode_index];

            for (endpoint_type = endpoint_type_start; endpoint_type <= endpoint_type_end; endpoint_type++)
            {
                skip_config = 0;
                /*
                 * Don't count OAM packets (other than CCM)
                 */
                action_result.counter_increment = 0;
                action_key.endpoint_type = endpoint_type;
                /*
                 *  Configure endpoint_type dependent variables
                 *  Specific cases that are configured separately,
                 *  are skipped.
 */
                switch (endpoint_type)
                {
                    case bcmOAMMatchTypeMEP:
                    {
                        if (action_key.opcode == bcmOamOpcodeCCM)
                        {
                            /*
                             * Count OAM-CCM packets
                             */
                            action_result.counter_increment = 1;
                        }
                        /*
                         * Send matching packets to active_dest_port
                         *
                         *  Destination port table for NON-ACC-MEP:
                         *
                         *  Opcode |   UniCast   Multicast   UnknownUniCast
                         *  -----------------------------------------------
                         *  XXM    |    active     active      error
                         *  LBM    |    reflector  active      error
                         *  CCM    |    active     active      error
                         *  Other  |    active     error       error
                         *
                         *  Destination port table for ACC-MEP:
                         *
                         *  Opcode |   UniCast   Multicast   UnknownUniCast
                         *  -----------------------------------------------
                         *  XXM    |    active     active      error
                         *  LBM    |    reflector  active      error
                         *  CCM    |    active     oamp        error
                         *  Other  |    active     error       error
                         */
                        if (dest_mac_type == bcmOAMDestMacTypeUknownUcast)
                        {
                            action_result.destination = level_dest_gport;
                        }
                        else if (dest_mac_type == bcmOAMDestMacTypeMcast)
                        {
                            {
                                if ((action_key.opcode == bcmOamOpcodeLinearAPS)
                                    || (action_key.opcode == bcmOamOpcodeLTM) || (action_key.opcode == bcmOamOpcodeLBM)
                                    || (action_key.opcode == bcmOamOpcodeLMM) || (action_key.opcode == bcmOamOpcodeAIS)
                                    || (action_key.opcode == bcmOamOpcodeDMM) || (action_key.opcode == bcmOamOpcodeSLM))
                                {
                                    action_result.destination = active_dest_gport;
                                }
                                else if ((action_key.opcode == bcmOamOpcodeCCM))
                                {
                                    if (is_acc)
                                    {
                                        action_result.destination = oamp_dest_gport;

                                    }
                                    else
                                    {
                                        action_result.destination = active_dest_gport;
                                    }
                                }
                                else
                                {
                                    action_result.destination = level_dest_gport;
                                }
                            }
                        }
                        else
                        {
                            /*
                             * Unicast
                             */
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac && action_key.opcode == bcmOamOpcodeLBM)
                            {
                                action_result.destination =
                                    is_ingress ? downmep_reflector_gport : upmep_reflector_gport;
                            }
                            else
                            {
                                action_result.destination = active_dest_gport;
                            }
                        }
                        break;
                    }
                    case bcmOAMMatchTypeMIP:
                    {
                        /*
                         * Configure MIP: Only CCM, LBM and LTM are handled in MIP
                         *
                         *  Destination port table for MIP:
                         *
                         *  Opcode |   UniCast   Multicast   UnknownUniCast
                         *  -----------------------------------------------
                         *  LBM    |    reflector  skip        skip
                         *  LTM    |    cpu        cpu         cpu
                         *  CCM    |    error      snoop       skip
                         *  Other  |    error      skip        skip
                         */
                        action_result.counter_increment = 0;
                        if (action_key.opcode == bcmOamOpcodeLTM)
                        {
                            /*
                             *  LTMs should be forwarded without updating the TTL,
                             *  where the CPU will be responsible for forwarding.
                             */
                            action_result.destination = cpu_dest_gport;
                        }
                        else if (action_key.opcode == bcmOamOpcodeLBM)
                        {
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                action_result.destination =
                                    is_ingress ? downmep_reflector_gport : upmep_reflector_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        else if (action_key.opcode == bcmOamOpcodeCCM)
                        {
                            /*
                             * CCM packets should be forwarded
                             */
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                /*
                                 * level_dest_port is used for error cases
                                 */
                                action_result.destination = level_dest_gport;
                            }
                            else if (dest_mac_type == bcmOAMDestMacTypeMcast)
                            {
                                action_result.destination = snoop_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        else
                        {
                            /*
                             * MIPs are transparent to all but CCM, LTM, LBM.
                             */
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                /*
                                 * level_dest_port is used for error cases
                                 */
                                action_result.destination = level_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        break;
                    }
                    case bcmOAMMatchTypePassive:
                    {
                        action_result.destination = passive_dest_gport;
                        break;

                    }
                    case bcmOAMMatchTypeLowMDL:
                    {
                        action_result.destination = level_dest_gport;
                        break;
                    }
                    default:
                    {
                        skip_config = 1;
                        break;
                    }
                }
                if (skip_config == 0)
                {
                    /*
                     * Set Action for profile
                     */
                    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
                    if (BCM_FAILURE(rv))
                    {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit, "bcm_dnx_oam_profile_action_set failed. Error:%d (%s)\n"), rv,
                                   bcm_errmsg(rv)));
                        return rv;
                    }
                }
            }
        }
    }

    return rv;
}

/**
 * \brief
 *   Sets a defult OAM profile for ingress or egress for
 *   non-accelarated endpoint:
 *    - Gets profile_id
 *    - Gets ports for trap types
 *    - Sets default actions per endpoint type and opcode
 *
 * \param
 *   [in] unit  -  Number of hardware unit used..
 *   [in] is_ingress - 1 to set ingress profile, 0 to set egress
 *        profile
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected
 * \remark
 *   * None
 * \see
 */
static shr_error_e
appl_dnx_oam_configure_default_profile(
    int unit,
    uint8 is_ingress,
    oam_traps_t oam_traps)
{
    bcm_error_t rv;
    bcm_gport_t active_dest_gport;
    bcm_oam_profile_type_t oam_profile_type;
    bcm_oam_profile_t profile_id;
    int active_trap;
    uint32 flags;

    rv = BCM_E_NONE;

    oam_profile_type = is_ingress ? bcmOAMProfileIngressLIF : bcmOAMProfileEgressLIF;
    flags = BCM_OAM_NO_FLAG;    /* Setting no flags. Default LM type will be SINGLE ENDED LM */
    rv = bcm_oam_profile_create(unit, flags, oam_profile_type, &profile_id);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_oam_profile_create failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Get downmep trap code
     */
    if (is_ingress)
    {
        active_trap = oam_traps.downmep_trap_id;
    }
    else
    {
        active_trap = oam_traps.upmep_trap_id;
    }
    BCM_GPORT_TRAP_SET(active_dest_gport, active_trap, 7, 0);

    /*
     * Set Default Profile - OAMA
     */
    rv = appl_dnx_oam_default_profile_set(unit, is_ingress, NOT_ACCELERATED, profile_id, active_dest_gport, &oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_oam_default_profile_set failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    return rv;
}

/**
 * \brief
 *   Sets a defult OAM profile for ingress or egress for
 *   accelarated endpoint:
 *    - Gets profile_id
 *    - Gets ports for trap types
 *    - Sets default actions per endpoint type and opcode
 *
 * \param
 *   [in] unit  -  Number of hardware unit used..
 *   [in] is_ingress - 1 to set ingress profile, 0 to set egress
 *        profile
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected
 * \remark
 *   * None
 * \see
 */
static shr_error_e
appl_dnx_oam_configure_default_acc_profile(
    int unit,
    uint8 is_ingress,
    oam_traps_t oam_traps)
{
    bcm_error_t rv;
    bcm_gport_t active_dest_gport;
    bcm_oam_profile_type_t oam_acc_profile_type;
    bcm_oam_profile_t acc_profile_id;
    int active_trap;
    uint32 flags;

    rv = BCM_E_NONE;

    oam_acc_profile_type =
        is_ingress ? bcmOAMProfileIngressAcceleratedEndpoint : bcmOAMProfileEgressAcceleratedEndpoint;
    flags = BCM_OAM_NO_FLAG;    /* Setting no flags. Default LM type will be SINGLE ENDED LM */
    rv = bcm_oam_profile_create(unit, flags, oam_acc_profile_type, &acc_profile_id);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_oam_profile_create failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Get downmep trap code
     */
    if (is_ingress)
    {
        active_trap = oam_traps.downmep_trap_id;
    }
    else
    {
        active_trap = oam_traps.upmep_trap_id;
    }
    BCM_GPORT_TRAP_SET(active_dest_gport, active_trap, 7, 0);

    /*
     * Set Accelerated Default Profile - OAMB
     */
    rv = appl_dnx_oam_default_profile_set(unit, is_ingress, ACCELERATED, acc_profile_id, active_dest_gport, &oam_traps);

    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_oam_default_profile_set failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }
    return rv;
}

/**
 * \brief - Enable ignoring protection state for OAMP inject packets
 *
 * \param [in] unit - unit id
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
appl_bcm_dnx_oam_failover_state_ignore_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * For devices which have ignoring protection state HW capability and JR2 mode.
     */
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_protection_state_ignore) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit)))
    {
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchControlOamBfdFailoverStateIgnore, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * In order to support HLM by LIF in egress, egress acc profile for injection packet for oam over mpls will be set to a reserved profile.
 * In non acc profile, counter should always be enabled. 
 * Users can control whether the packet should be ccounted, by setting reserved egress acc profile.
 */
int
appl_dnx_oam_hlm_egress_with_update_counter_in_egr_acc_profile(
    int unit,
    int opcode,
    int counter_enable)
{
    bcm_error_t rv;
    uint32 flags = 0;
    bcm_oam_action_key_t oam_action_key;
    bcm_oam_action_result_t oam_action_result;
    int reserved_acc_egr_profile_id = dnx_data_oam.general.oam_egr_acc_action_profile_id_for_inject_mpls_get(unit);
    int profile_id = 0;

    bcm_oam_action_key_t_init(&oam_action_key);
    bcm_oam_action_result_t_init(&oam_action_result);

    profile_id = reserved_acc_egr_profile_id | bcmOAMProfileEgressAcceleratedEndpoint << 24 | 1 << 31;;

    oam_action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    oam_action_key.endpoint_type = bcmOAMMatchTypeMEP;
    oam_action_key.inject = 1;
    oam_action_key.opcode = opcode;

    /*
     * Get original profile configuration
     */
    rv = bcm_oam_profile_action_get(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv, in bcm_oam_profile_action_get\n");
        return rv;
    }

    oam_action_result.counter_increment = counter_enable;
    oam_action_result.destination = BCM_GPORT_INVALID;

    /*
     * Set new profile configuration
     */
    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv, in bcm_oam_profile_action_set\n");
        return rv;
    }

    return rv;
}

/*
 * See prototype definition for function description
 */
shr_error_e
appl_dnx_oam_init(
    int unit)
{
    bcm_error_t rv;
    oam_traps_t oam_traps = { 0 };
    bfd_traps_t bfd_traps = { 0 };
    uint64 arg;
    int indexed_mode_enable;
    int i = 0;
    bcm_oam_opcode_t opcodes_dis_cnt[] = {
        bcmOamOpcodeCCM, bcmOamOpcodeLMM, bcmOamOpcodeLMR, bcmOamOpcodeDMM, bcmOamOpcodeDMR
    };

    rv = appl_dnx_oam_trap_init(unit, &oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_oam_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    rv = appl_dnx_bfd_trap_init(unit, &bfd_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_bfd_trap_config failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    
    if (!(soc_is(unit, J2P_DEVICE)))
    {
        rv = appl_dnx_bfd_ipv6_invalid_udp_checksum(unit);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_dnx_bfd_ipv6_invalid_udp_checksum failed. Error:%d (%s)\n"), rv,
                       bcm_errmsg(rv)));
            return rv;
        }
    }

    /*
     * Set OAM reflectors
     */
    rv = appl_dnx_oam_downmep_reflector_init(unit, &oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_oam_downmep_reflector_init failed. Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    rv = appl_dnx_oam_upmep_reflector_init(unit, &oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_oam_upmep_reflector_init failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Configure the default profile on ingress side
     */
    rv = appl_dnx_oam_configure_default_profile(unit, INGRESS_SIDE, oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_oam_configure_default_profile for ingress failed. Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Configure the default accelerated profile on ingress side
     */
    rv = appl_dnx_oam_configure_default_acc_profile(unit, INGRESS_SIDE, oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U
                   (unit, "appl_dnx_oam_configure_default_acc_profile  for ingress failed. Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Configure the default profile on egress side
     */
    rv = appl_dnx_oam_configure_default_profile(unit, EGRESS_SIDE, oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "appl_dnx_oam_configure_default_profile for egress failed. Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Configure the default accelerated profile  on egress side
     */
    rv = appl_dnx_oam_configure_default_acc_profile(unit, EGRESS_SIDE, oam_traps);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U
                   (unit, "appl_dnx_oam_configure_default_acc_profile for egress failed. Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    /*
     * Configure : OAM packets over native Ethernet over PWE is recognized as data packet on PWE LIF
     * This configuration only affect pwe unindexed tagged mode
     */
    indexed_mode_enable = dnx_data_lif.in_lif.in_lif_profile_allocate_indexed_mode_get(unit);
    if (indexed_mode_enable == IN_LIF_PROFILE_PWE_INDEXED_MODE_NOF_VALUES)
    {
        COMPILER_64_SET(arg, 0, 1);
        rv = bcm_oam_control_set(unit, bcmOamControlNativeEthernetOverPweClassification, arg);
        if (BCM_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcmOamControlNativeEthernetOverPweClassification failed. Error:%d (%s)\n"), rv,
                       bcm_errmsg(rv)));
            return rv;
        }
    }

    /*
     * Enable ignoring protection state for OAMP inject packets.
     */
    rv = appl_bcm_dnx_oam_failover_state_ignore_set(unit);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U
                   (unit, "appl_bcm_dnx_oam_failover_state_ignore_set failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
     * In order to support HLM by LIF in egress, egress acc profile for injection packet for oam over mpls will be set to a reserved profile.
     * Here, configure whether the packet with specific opcode should be counted, by setting reserved egress acc profile.
     */
    for (i = 0; i < sizeof(opcodes_dis_cnt) / sizeof(bcm_oam_opcode_t); i++)
    {
        rv = appl_dnx_oam_hlm_egress_with_update_counter_in_egr_acc_profile(unit, opcodes_dis_cnt[i], 0);
        if (rv != BCM_E_NONE)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit,
                        "Error:%d, in appl_dnx_oam_hlm_egress_with_update_counter_in_egr_acc_profile with opcode(%d)\n"),
                       rv, opcodes_dis_cnt[i]));
            return rv;
        }
    }

    return rv;
}
