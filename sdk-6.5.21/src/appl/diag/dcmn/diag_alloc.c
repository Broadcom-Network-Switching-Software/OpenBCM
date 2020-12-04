/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_alloc.c
 * Purpose: 
 */



#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag_alloc.h>
#include <appl/diag/diag.h>
#include <shared/swstate/sw_state_resmgr.h>

#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/gport_mgmt_sw_db.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>

#include <bcm/vlan.h>
#include <bcm/mpls.h>

#if defined(INCLUDE_DUNE_UI)
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#endif
/*************
 * DEFINES   *
 */
#define DIAG_ALLOC_MAX_DISPLAY_PER_LINE 10 /* max number of ids displayed per line */
#define DIAG_ALLOC_BLOCK_SIZE 40  
#define DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(arg) (arg) ? "allocated" : "free"
/*************
 * GLOBALS   *
 */
STATIC int diag_alloc_current_display_per_line  = 0; /* current number of ids displayed in current line */
STATIC int diag_alloc_print_flag = 0;

cmd_result_t cmd_diag_alloc_hw_lif_get_block(int unit, diag_alloc_system_table_parameters *parameters);
cmd_result_t cmd_diag_alloc_hw_fec_get_block(int unit, diag_alloc_system_table_parameters *parameters);
cmd_result_t cmd_diag_alloc_hw_eve_print    (int unit, diag_alloc_system_table_parameters *parameters);
cmd_result_t cmd_diag_alloc_hw_ive_print    (int unit, diag_alloc_system_table_parameters *parameters);
cmd_result_t diag_alloc_system_table_print_fec(int unit, int id);
cmd_result_t diag_alloc_system_table_print_ive(int unit, int id);
cmd_result_t diag_alloc_system_table_print_eve(int unit, int id);

static const DIAG_ALLOC_POOL_DEF pools[] =
{
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    |                Full                    |        Long     |   Short   |                 Pool                       |              Function for          |          Function for              
    |              Pool name                 |     Pool Name   | Pool Name |                  ID                        |              SW Info Print         |      direct(HW) Info Print         
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    {  "Ingress LIF bank allocation (A+ and below)"         ,"InLif", "IL", dpp_am_res_obs_inlif                       , NULL                              , cmd_diag_alloc_hw_lif_get_block   },
    {  "Egress encapsulation bank allocation (A+ and below)","OutLif" , "OL", dpp_am_res_obs_eg_encap                  , NULL                              , NULL                              },
    {  "FECs for global use"                  , "FEC"           , "FEC"    , dpp_am_res_fec_global                     , diag_alloc_system_table_print_fec , cmd_diag_alloc_hw_fec_get_block   },
    {  "VLAN translation ingress usage"       , "InVLAN"        , "IVE"    , dpp_am_res_vlan_edit_action_ingress       , diag_alloc_system_table_print_ive , cmd_diag_alloc_hw_ive_print       },
    {  "VLAN translation egress usage"        , "OutVLAN"       , "EVE"    , dpp_am_res_vlan_edit_action_egress        , diag_alloc_system_table_print_eve , cmd_diag_alloc_hw_eve_print       },
    {  "VSIs for TB VLANS"                    , "VLAN"          , "VLAN"   , dpp_am_res_vsi_vlan                       , NULL                              , NULL                              },
    {  "VSIs for MSTP"                        , "VSI"           , "VSI"    , dpp_am_res_vsi_mstp                       , NULL                              , NULL                              },
    {  "FEC Failover id (Jericho)"            , "FEC_Failover"  , "FECF"   , dpp_am_res_failover_fec_id                , NULL                              , NULL                              },
    {  "Ingress Failover id (Jericho)"        , "Ing_Failover"  , "INGF"   , dpp_am_res_failover_ingress_id            , NULL                              , NULL                              },
    {  "Egress Failover id (Jericho)"         , "Eg_Failover"   , "EGF"    , dpp_am_res_failover_egress_id             , NULL                              , NULL                              },
    {  "Failover id (Arad+ and below)"        , "Failover"      , "Fail"   , dpp_am_res_failover_common_id             , NULL                              , NULL                              },
    {  "QOS INGRESS LABEL MAP ID"             , "QOS_ELSP"      , "QOS_E"  , dpp_am_res_qos_ing_elsp                   , NULL                              , NULL                              },
    {  "QOS INGRESS LIF/COS IDs"              , "COS"           , "COS"    , dpp_am_res_qos_ing_lif_cos                , NULL                              , NULL                              },
    {  "QOS INGRESS PCP PROFILE IDs"          , "PCP_VLAN"      , "PCP_V"  , dpp_am_res_qos_ing_pcp_vlan               , NULL                              , NULL                              },
    {  "QOS INGRESS COS OPCODE IDs"           , "COS_Opcode"    , "COS_O"  , dpp_am_res_qos_ing_cos_opcode             , NULL                              , NULL                              },
    {  "QOS EGRESS REMARK QOS IDs"            , "QOS_Remark"    , "QOS_R"  , dpp_am_res_qos_egr_remark_id              , NULL                              , NULL                              },
    {  "QOS EGRESS MPLS PHP QOS IDs"          , "QOS_MPLS_PHP"  , "QOS_M"  , dpp_am_res_qos_egr_mpls_php_id            , NULL                              , NULL                              },
    {  "number of meters in processor A"      , "Meter_A"       , "M_A"    , dpp_am_res_meter_a                        , NULL                              , NULL                              },
    {  "number of meters in processor B"      , "Meter_B"       , "M_B"    , dpp_am_res_meter_b                        , NULL                              , NULL                              },
    {  "SW handles of policer"                , "Policer"       , "POL"    , dpp_am_res_ether_policer                  , NULL                              , NULL                              },
    {  "ECMP id"                              , "ECMP"          , "ECMP"   , dpp_am_res_ecmp_id                        , NULL                              , NULL                              },
    {  "QOS EGRESS L2 I TAG PROFILE IDs"      , "QOS_I_TAG"     , "QOS_I"  , dpp_am_res_qos_egr_l2_i_tag               , NULL                              , NULL                              },
    {  "QOS EGRESS DSCP/EXP MARKING PROFILE ID,s", "QOS_Mark"    , "QOS_M"  , dpp_am_res_qos_egr_dscp_exp_marking       , NULL                              , NULL                              },
    {  "RP id"                                 , "IPMC_RP"       , "IPMC_RP", dpp_am_res_rp_id                          , NULL                              , NULL                             },
    {  "OAM MA Index"                          , "OAM_MA"        , "OAM_M"  , dpp_am_res_oam_ma_index                   , NULL                              , NULL                             },
    {  "OAM MEP Index for short MA name format", "OAM_MEPShort"  , "OAM_S"  , dpp_am_res_oam_mep_id_short               , NULL                              , NULL                             },
    {  "OAM MEP Index for ccm MA name format"  , "OAM_MEPLong"   , "OAM_L"  , dpp_am_res_oam_mep_id_long                , NULL                              , NULL                             },
    {  "OAM MEP Index for ccm MA name format"  , "OAM_MEPLong"   , "OAM_L"  , dpp_am_res_oam_mep_id_long_non_48_maid    , NULL                              , NULL                             },
    {  "OAM MEP Index for 48B MA name format"  , "OAM_MEP_48"    , "OAM_48" , dpp_am_res_oam_mep_id_long_48_maid        , NULL                              , NULL                             },
    {  "OAM MEP Index for ccm MA name format"  , "OAM_MEP"       , "OAM_E"  , dpp_am_res_oam_mep_id                     , NULL                              , NULL                             },
    {  "OAM RMEP Index"                        , "OAM_RMEP"      , "OAM_R"  , dpp_am_res_oam_rmep_id                    , NULL                              , NULL                             },
    {  "OAM trap code upmep ftmh header"       , "OAM_TrapCode"  , "OAM_T"  , dpp_am_res_oam_trap_code_upmep_ftmh_header, NULL                              , NULL                             },
    {  "Profiles for PON use"                  , "PON_CHN_Prof"  , "PON"    , dpp_am_res_pon_channel_profile            , NULL                              , NULL                             },
    {  "Local Common InLif (Jericho)"          , "Common_Inlif"  , "LCLif"  , dpp_am_res_local_inlif_common             , NULL                              , NULL                             },
    {  "Local Wide InLif (Jericho)"            , "Wide_Inlif"    , "LWLif"  , dpp_am_res_local_inlif_wide               , NULL                              , NULL                             },
    {  "Local OutLif (Jericho)"                , "Local_Outlif"  , "LOLif"  , dpp_am_res_local_outlif                   , NULL                              , NULL                             },
    {  "Trill virtual nickname"                , "TrillNick"     , "TRILL"  , dpp_am_res_trill_virtual_nick_name        , NULL                              , NULL                             },
    {  "Field entry id"                        , "FieldEntryID"  , "FEID"   , dpp_am_res_field_entry_id                 , NULL                              , NULL                             },
    {  "Field direct extraction entry  id"     , "FieldDirExEntID", "FDEEID", dpp_am_res_field_direct_extraction_entry_id, NULL                             , NULL                             }
};

const char*  DIAG_ACTION_TPID_to_string( bcm_vlan_tpid_action_t enum_val)
{
    const char* str = NULL;
    switch(enum_val) 
    {
    case bcmVlanTpidActionNone:
        str = "none";
    break;
    case bcmVlanTpidActionModify:
        str = "set";
    break;
    default:
        str = "Unknown";
    }
    return str;
}

static const char*  DIAG_ACTION_VID_to_string( bcm_vlan_action_t enum_val)
{
    const char* str = NULL;
    switch(enum_val) 
    {
    case bcmVlanActionNone:
        str = "none";
    break;
    case bcmVlanActionAdd:
        str = "add";
    break;
    case bcmVlanActionReplace:
        str = "replace";
    break;
    case bcmVlanActionDelete:
        str = "delete";
    break;
    case bcmVlanActionCopy:
        str = "copy";
    break;
    case bcmVlanActionMappedAdd:
        str = "add vsi";
    break;
    case bcmVlanActionMappedReplace:
        str = "replace vsi";
    break;
    case bcmVlanActionOuterAdd:
        str = "add outer";
    break;
    case bcmVlanActionInnerAdd:
        str = "add inner";
    break;
    default:
        str = " Unknown";
    }
    return str;
}

static const char*  DIAG_ACTION_PCP_to_string( bcm_vlan_action_t enum_val)
{
    const char* str = NULL;
    switch(enum_val) 
    {
    case bcmVlanActionNone:
        str = "same_tag";
    break;
    case bcmVlanActionAdd:
        str = "mapped";
    break;
    case bcmVlanActionReplace:
        str = "mapped";
    break;
    case bcmVlanActionCopy:
        str = "opposite_tag";
    break;
    case bcmVlanActionOuterAdd:
        str = "same_tag";
    break;
    case bcmVlanActionInnerAdd:
        str = "opposite_tag";
    break;
    default:
        str = " Unknown";
    }
    return str;
}


/* *******************************************************************************************************
                                Utils function for diag pp gport:
 */

/* clear gport_param */
void diag_alloc_gport_param_init(DIAG_ALLOC_GPORT_PARAMS* gport_params) {
    sal_memset(gport_params, 0x0, sizeof(DIAG_ALLOC_GPORT_PARAMS));
}


/* clear gport_info */
void diag_alloc_gport_info_init(DIAG_ALLOC_GPORT_INFO* gport_info) {
    sal_memset(gport_info, 0, sizeof(DIAG_ALLOC_GPORT_INFO));
}

/* *******************************************************************************************************
                                main function for diag pp gport:
 */

/* get gport information according to parameters set by the user in diag pp gport */
cmd_result_t diag_alloc_gport_info_get(int unit, 
                                       DIAG_ALLOC_GPORT_PARAMS* gport_params, 
                                       DIAG_ALLOC_GPORT_INFO* gport_info) {
    uint32 encap_id;
    uint32 sub_type_id;
    int ret;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    bcm_vlan_port_t vlan_port; /* vlan_port struct if gport type is vlan */ 
    bcm_mpls_port_t mpls_port; /* mpls_port struct if gport type is mpls */

    

    /* gport_id */
    gport_info->gport_id = gport_params->gport_id;

    /* gport type */
    if (BCM_GPORT_IS_VLAN_PORT(gport_info->gport_id)) {
        gport_info->gport_type = diag_alloc_gport_info_gport_type_vlan;
    } else if (BCM_GPORT_IS_MPLS_PORT(gport_info->gport_id)) {
        gport_info->gport_type = diag_alloc_gport_info_gport_type_mpls;
    } else {
        cli_out("We support only vlan and mpls port \n");
        return CMD_FAIL; 
    }

    /* encap id of gport: gport id without gport type */

    /* encap id for vlan port id */
    if (gport_info->gport_type == diag_alloc_gport_info_gport_type_vlan) {
        encap_id = BCM_GPORT_VLAN_PORT_ID_GET(gport_info->gport_id);
    }  
    /* encap id for mpls port id */
    else if (gport_info->gport_type == diag_alloc_gport_info_gport_type_mpls){
        encap_id = BCM_GPORT_MPLS_PORT_ID_GET(gport_info->gport_id);
    } else {
        return CMD_FAIL;  
    }

    /* find if we have inLif, outLif, fec */
   ret = _bcm_dpp_gport_to_hw_resources(unit, gport_info->gport_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |_BCM_DPP_GPORT_HW_RESOURCES_FEC, &gport_hw_resources);
   if (ret != BCM_E_NONE) {
       cli_out("Diag failed, error in _bcm_dpp_gport_to_lif\n");
       return CMD_FAIL; 
   }

   gport_info->global_lif_id = gport_hw_resources.global_in_lif;
   gport_info->lif_id = gport_hw_resources.local_in_lif;
   gport_info->out_lif_id = gport_hw_resources.local_out_lif;
   gport_info->fec_id = gport_hw_resources.fec_id;

    /* if gport is egress only, inLif not exist */
    if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(encap_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)) {
        gport_info->lif_id = -1;
    } 
    /* if gport is ingress only, outlif not exist */
    else if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(encap_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY) {
         gport_info->out_lif_id = -1;
    }

    /* is_remote: if inLif and outlif */
    if (gport_info->lif_id>0 && gport_info->out_lif_id>0) {
        /* for vlan port id */
        if (gport_info->gport_type == diag_alloc_gport_info_gport_type_vlan) {
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.vlan_port_id = gport_info->gport_id;
            ret = bcm_vlan_port_find(unit, &vlan_port);
            if (ret != BCM_E_NONE) {
                cli_out("Diag failed, error in bcm_vlan_port_find\n");
                return CMD_FAIL;
            }
            gport_info->is_remote = BCM_ENCAP_REMOTE_GET(vlan_port.encap_id);
        } 
        /* for mpls port id */
        else if (gport_info->gport_type == diag_alloc_gport_info_gport_type_mpls) {
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.mpls_port_id = gport_info->gport_id;
            ret = bcm_mpls_port_get(unit, 0, &mpls_port);
            if (ret != BCM_E_NONE) {
                cli_out("Diag failed, error in bcm_petra_mpls_port_get\n");
                return CMD_FAIL; 
            } 
            gport_info->is_remote = BCM_ENCAP_REMOTE_GET(mpls_port.encap_id); 
        }
    }

    /* encoding (by gport type) */

    sub_type_id = BCM_GPORT_SUB_TYPE_GET(encap_id);

    switch (sub_type_id) {
    case BCM_GPORT_SUB_TYPE_MULTICAST : 
        gport_info->encoding = diag_alloc_gport_info_encoding_one_plus_one_protection;
        break;
    case BCM_GPORT_SUB_TYPE_LIF :
        gport_info->encoding = diag_alloc_gport_info_encoding_no_protection;
        break;
    case BCM_GPORT_SUB_TYPE_PROTECTION : 
        gport_info->encoding = diag_alloc_gport_info_encoding_one_one_protection;
        break;
    default :
        cli_out("Diag failed. Invalid sub type: %d\n", sub_type_id);
        return CMD_FAIL; 
    }

    /* if encoding is 1+1 protection, then get multicast info from gport id */
    if (gport_info->encoding == diag_alloc_gport_info_encoding_one_plus_one_protection) {
        /* multicast id */
        gport_info->multicast_id = BCM_GPORT_SUB_TYPE_MULTICAST_VAL_GET(encap_id);
        /* is primary */
        gport_info->multicast_is_primary = BCM_GPORT_SUB_TYPE_MULTICAST_PS_GET(encap_id);
    }

    /* get forwarding database info */

    if (gport_params->forwarding_database && !
		(BCM_GPORT_SUB_TYPE_LIF_EXC_GET(encap_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)) {
        /* for vlan port id */ 
        if (gport_info->gport_type == diag_alloc_gport_info_gport_type_vlan ||
            gport_info->gport_type == diag_alloc_gport_info_gport_type_mpls) {
            /* NOTE !! the data from the hash table is copied, not pointed*/
            ret = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                                (sw_state_htb_key_t) (&gport_info->gport_id), 
                                                (sw_state_htb_data_t) gport_info->forwarding_info_data,
                                                FALSE);
            if (ret != BCM_E_NONE) {
                cli_out("Diag failed, error in _bcm_dpp_sw_db_hash_vlan_find\n");
                return CMD_FAIL; 
            }
        } 
    } 
    return CMD_OK;
}

/* *******************************************************************************************************
                                print function for diag pp gport 
 */

void diag_alloc_gport_info_print(int unit, 
                                 DIAG_ALLOC_GPORT_PARAMS* gport_params,
                                 DIAG_ALLOC_GPORT_INFO* gport_info) {
    int ret;
    char* gport_type;
    char* encoding;
    char* mc_primary;

    /* gport type */
    if (gport_info->gport_type == diag_alloc_gport_info_gport_type_vlan) {
        gport_type = "VLAN-PORT";
    } else if (gport_info->gport_type == diag_alloc_gport_info_gport_type_mpls){
        gport_type = "MPLS-PORT";
    } else {
        return;
    }
    /* print encoding */
    switch (gport_info->encoding) {
    case diag_alloc_gport_info_encoding_no_protection : 
        encoding = "No-protection";
        break;
    case diag_alloc_gport_info_encoding_one_plus_one_protection :
        encoding = "1+1 protection";
        break;
    case diag_alloc_gport_info_encoding_one_one_protection :
        encoding = "1:1 protection";
        break;
    default :
        cli_out("Can't find encoding\n");
        encoding = "";
        return;
    }

    /* print gport id */
    cli_out("%s 0x%x Encoding: %s ", gport_type, gport_info->gport_id, encoding);

    /* print lif if exist */
    if (gport_info->global_lif_id > 0) {
        cli_out(",Global-LIF-ID: %d", gport_info->global_lif_id);
    }
    /* print fec if exist */
    if (gport_info->fec_id > 0) {
        cli_out(",FEC: %d", gport_info->fec_id);
    }
    /* print MC id if exist */
    if (gport_info->multicast_id > 0) {
        cli_out(",MC id: %d",gport_info->multicast_id);
    }
    cli_out("\n");

    /* print resource */
    if (gport_params->resource) {
        /* inLif */
        if (gport_info->lif_id <= 0) {
            cli_out("LocalInLif:  Not-Exist\n");
        } else if (gport_info->is_remote) {
            cli_out("LocalInLif: Remote %d\n", gport_info->lif_id);
        } else {
            cli_out("LocalInLif:  Exists %d\n", gport_info->lif_id);
        }

        /* outLif */
        if (gport_info->out_lif_id <= 0) {
            cli_out("LocalOutLif: Not-Exist\n");
        } else if (gport_info->is_remote) {
            cli_out("LocalOutLif: Remote %d\n", gport_info->out_lif_id);
        } else {
            cli_out("LocalOutLif: Exists %d\n", gport_info->out_lif_id);
        }

        /* fec */
        if (gport_info->fec_id <= 0) {
            cli_out("FEC:    Not-Exist\n");
        } else {
            cli_out("FEC:    Exist %d\n", gport_info->fec_id);
        }

        /* MC-ID */
        if (gport_info->multicast_id <= 0) {
            cli_out("MC-ID:  Not-Exist\n");
        } else {
            if (gport_info->multicast_is_primary) {
                mc_primary = "primary";
            }  else {
                mc_primary = "secondary";
            }
            cli_out("MC-ID:  Exist %d, %s\n", gport_info->multicast_id, mc_primary);
        }
    }

    /* print Forward database if asked by the user and if not ingress only */
    if (gport_params->forwarding_database 
        && gport_info->out_lif_id > 0) {
        ret = _bcm_dpp_sw_db_hash_vlan_print(unit, gport_info->forwarding_info_data); 
        if (ret != BCM_E_NONE) {
            cli_out("_bcm_dpp_sw_db_hash_vlan_print failed");
        }
    }

    /* get lif database info if asked by the user and is not egress only */
    if (gport_params->lif_database 
        && gport_info->lif_id > 0 
        && !gport_info->is_remote) {
        if (gport_info->gport_type == diag_alloc_gport_info_gport_type_vlan) {
            ret = _bcm_dpp_lif_ac_match_print(unit, gport_info->lif_id);
            if (ret != BCM_E_NONE) {
                cli_out("_bcm_dpp_lif_ac_match_print failed");
            }
        } else if (gport_info->gport_type == diag_alloc_gport_info_gport_type_mpls){
            ret = _bcm_dpp_lif_mpls_match_print(unit, gport_info->lif_id);
             if (ret != BCM_E_NONE) {
                 cli_out("_bcm_dpp_lif_ac_match_print failed");
             }
        }
    }
}



/* *******************************************************************************************************
                                Utils function for diag alloc 
 */

/* clear struct diag_alloc_system_table_parameters */
cmd_result_t diag_alloc_system_table_parameters_clear(diag_alloc_system_table_parameters* parameters) {
    sal_memset(parameters, 0x0, sizeof(diag_alloc_system_table_parameters));

    parameters->from = -1;
    parameters->to = -1;
    parameters->info = -1;

    return CMD_OK;
}

/* parse parameters from the command diag alloc */
cmd_result_t diag_alloc_system_table_parameters_parse(int unit, args_t *a, diag_alloc_system_table_parameters* parameters) {
    parse_table_t pt; /* parse table for optional user parameters */

    
    if (!parameters || !a) {
        cli_out("Diag failed. null parameters in diag_alloc_system_table_parameters_parse\n");
        return CMD_FAIL;
    }

    /* parse parameters and setup "parameters" function */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "from", PQ_DFL|PQ_INT, 0, &parameters->from, NULL);
    parse_table_add(&pt, "to", PQ_DFL|PQ_INT, 0, &parameters->to, NULL);
    parse_table_add(&pt, "info", PQ_DFL|PQ_INT, 0, &parameters->info, NULL);
    parse_table_add(&pt, "direct", PQ_DFL|PQ_INT, 0, &parameters->direct, NULL);

     if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    return CMD_OK; 
}

/* setup parameters format from dune ui_ppd_api (dune shell) using parameters from diag alloc(bcm shell) */
void 
cmd_diag_alloc_dune_shell_parameters_setup(SOC_SAND_TABLE_BLOCK_RANGE   *prm_block_range,
                                           diag_alloc_system_table_parameters* parameters) {
        /* range start */
    if (parameters->from > -1) {
        prm_block_range->iter = parameters->from;
    }
    /* range end */
    if (parameters->to > -1) {
        prm_block_range->entries_to_scan = parameters->to - prm_block_range->iter + 1;
    }
}


/* *******************************************************************************************************
                                Print functions for diag alloc 
 */


/* default print method, print only ids */
cmd_result_t diag_alloc_system_table_print_Ids(int unit, int id) {
    cli_out("%8d  ",id);

    diag_alloc_current_display_per_line++;

    if (diag_alloc_current_display_per_line == DIAG_ALLOC_MAX_DISPLAY_PER_LINE) {
        cli_out("\n");
        diag_alloc_current_display_per_line = 0;
    }
    return CMD_OK;
}

/* print method for fec infos */
cmd_result_t diag_alloc_system_table_print_fec(int unit, int id) {
    bcm_l3_egress_t fec_info; /* optional info: FEC info */
    int ret;
    char access;

    /* get the FEC info from the id */
    bcm_l3_egress_t_init(&fec_info);
    ret = bcm_l3_egress_get(unit ,id, &fec_info);
    if (ret != BCM_E_NONE) {
        cli_out("Diag failed. Can't get fec infos for id: %d \n", id);
        return CMD_FAIL;
    }

    /* set the acccess character */
    access = ((fec_info.flags & BCM_L3_HIT) != 0) ? 'V' : 'X';

    /* print FEC info */
    cli_out("FEC-id: 0x%-8x encap id: 0x%-8x port: 0x%-8x intf 0x%-8x access: %c \n",
            id, fec_info.encap_id, fec_info.port, fec_info.intf, access);
    return CMD_OK;
}

/* print method for VLAN editing infos*/
cmd_result_t diag_alloc_system_table_print_eve(int unit, int id) {

    SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY command_key;
    _bcm_dpp_vlan_translate_action_t bcm_dpp_action;
        
    int is_first_lookup = TRUE;
    int ret; 

    ret = _bcm_dpp_vlan_edit_action_get(unit,id,0,&bcm_dpp_action) ;
    if (ret != BCM_E_NONE) {
        cli_out("Diag failed. Can't get vlan editing infos for id: %d \n", id);
        return CMD_FAIL;
    }

    if (diag_alloc_print_flag==0) {

        if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
            cli_out("The API is only available when VLAN translation is set to Advanced mode");
            diag_alloc_print_flag=1;
            return CMD_OK;
        }     

        cli_out(" -----------------------------------------------------------------------------------------------------------------------------------\n\r"
                "|                                                   EGRESS VLAN Editing Table(SW)                                                   |\n\r"
                " -----------------------------------------------------------------------------------------------------------------------------------\n\r");
        cli_out("|        ||        |  VLAN   ||                     OUTER                        ||                      INNER                      |\n\r"
                "| Action ||  TAG   |  Edit   || value  |                     action              || value  |                    action              |\n\r"
                "|   ID   || format | Profile || TPID   |      VID      |      PCP      |  TPID   || TPID   |      VID      |      PCP      |  TPID  |\n\r");
        cli_out(" -----------------------------------------------------------------------------------------------------------------------------------\n\r");

        diag_alloc_print_flag=1;
    }

    SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear(&command_key);
    command_key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
           

    while(_bcm_petra_vlan_edit_eg_command_id_find(unit, id, is_first_lookup, &command_key)==BCM_E_EXISTS)
    {
        is_first_lookup=0;

        cli_out("| %-7d|| %-7d| %-8d|| 0x%-5x| %-14s| %-14s| %-7s || 0x%-5x| %-14s| %-14s| %-7s|\n\r",
                id, command_key.tag_format, command_key.edit_profile, 
                bcm_dpp_action.outer.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.outer.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.outer.pcp_action),  DIAG_ACTION_TPID_to_string(bcm_dpp_action.outer.tpid_action),
                bcm_dpp_action.inner.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.inner.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.inner.pcp_action), DIAG_ACTION_TPID_to_string(bcm_dpp_action.inner.tpid_action));

    }

    if(is_first_lookup)
    {
        cli_out("| %-7d||        |         || 0x%-5x| %-14s| %-7s| %-7s|| 0x%-5x| %-14s| %-7s| %-7s|\n\r",
                id, bcm_dpp_action.outer.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.outer.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.outer.pcp_action), DIAG_ACTION_TPID_to_string(bcm_dpp_action.outer.tpid_action),
                bcm_dpp_action.inner.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.inner.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.inner.pcp_action), DIAG_ACTION_TPID_to_string(bcm_dpp_action.inner.tpid_action));

    }

    cli_out(" -----------------------------------------------------------------------------------------------------------------------------------\n\r");

    return CMD_OK;

}

/* print method for Ingress VLAN editing infos*/
cmd_result_t diag_alloc_system_table_print_ive(int unit, int id) {
      
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY command_key;
    _bcm_dpp_vlan_translate_action_t bcm_dpp_action;
    uint32 command_id=0;

    int is_first_action_print = 1;
    int ret;      

    ret = _bcm_dpp_vlan_edit_action_get(unit,id, 1, &bcm_dpp_action) ;
    if (ret != BCM_E_NONE) {
        cli_out("Diag failed. Can't get vlan editing infos for id: %d \n", id);
        return CMD_FAIL;
    } 

    if (diag_alloc_print_flag==0) {

        if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
            cli_out("The API is only available when VLAN translation is set to Advanced mode");
            diag_alloc_print_flag=1;
            return CMD_OK;
        }     

        cli_out(" -----------------------------------------------------------------------------------------------------------------------------------\n\r"
                "|                                                   Ingress VLAN Editing Table(SW)                                                  |\n\r"
                " -----------------------------------------------------------------------------------------------------------------------------------\n\r");
        cli_out("|        ||        |  VLAN   ||                     OUTER                        ||                      INNER                      |\n\r"
                "| Action ||  TAG   |  Edit   || value  |                     action              || value  |                    action              |\n\r"
                "|   ID   || format | Profile || TPID   |      VID      |      PCP      |  TPID   || TPID   |      VID      |      PCP      |  TPID  |\n\r");
        cli_out(" -----------------------------------------------------------------------------------------------------------------------------------\n\r");

        diag_alloc_print_flag=1;
    }

    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(&command_key);
    
    for (command_key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE; command_key.tag_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; command_key.tag_format++) 
    {

        for(command_key.edit_profile = 0; command_key.edit_profile < DPP_NOF_INGRESS_VLAN_EDIT_PROFILES; command_key.edit_profile++)
        {
            ret = soc_ppd_lif_ing_vlan_edit_command_id_get(unit, &command_key, &command_id);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
                 cli_out("Diag failed. Can't get vlan action for tag_format: %d and edit_profile: %d error: %d\n", command_key.tag_format, command_key.edit_profile, ret);
                 return CMD_FAIL;
             }

            if (command_id == id) {
                   cli_out("|  %-4d  ||  %-4d  |  %-4d   || 0x%-4x | %-14s| %-14s| %-7s || 0x%-4x | %-14s| %-14s| %-7s|\n\r",
                           id, command_key.tag_format, command_key.edit_profile, 
                           bcm_dpp_action.outer.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.outer.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.outer.pcp_action), DIAG_ACTION_TPID_to_string(bcm_dpp_action.outer.tpid_action),
                           bcm_dpp_action.inner.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.inner.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.inner.pcp_action), DIAG_ACTION_TPID_to_string(bcm_dpp_action.inner.tpid_action));
                   is_first_action_print=0;
            }
        }
    }

    if (is_first_action_print) {
        cli_out("|  %-4d  ||        |         || 0x%-4x | %-14s| %-14s| %-7s || 0x%-4x | %-14s| %-14s| %-7s|\n\r",
                id, bcm_dpp_action.outer.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.outer.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.outer.pcp_action), DIAG_ACTION_TPID_to_string(bcm_dpp_action.outer.tpid_action),
                bcm_dpp_action.inner.tpid_val, DIAG_ACTION_VID_to_string(bcm_dpp_action.inner.vid_action), DIAG_ACTION_PCP_to_string(bcm_dpp_action.inner.pcp_action), DIAG_ACTION_TPID_to_string(bcm_dpp_action.inner.tpid_action));
    }
    
    cli_out(" -----------------------------------------------------------------------------------------------------------------------------------\n\r");

    return CMD_OK;
}



/* *******************************************************************************************************
                                Main function for diag alloc 
 */

/* display used ids of pools.
   optional: filter by a range of ids. 
   */
cmd_result_t
cmd_diag_alloc_sw_common_eval_object(int unit, 
                                        diag_alloc_system_table_parameters* parameters, 
                                        cmd_result_t (*eval_object_cb)(int,int),
                                        int pool_idx) { 
    int pool_id; /* current pool id, after calculating by core. */
    int low_id; /* base id for the current pool */
    int nbr_entries; /* number of entries in the pool */ 
    int res_id;
    int core_id;
    uint32 ret;   
    uint8 nof_cores_per_res;
    sw_state_res_pool_info_t pool_info; /* get number of used id for current pool */
    int id; /* current id in in the current pool */
    int number_entries_used_found;  /* number of entries used displayed */
    uint32 range_start= -1; /* optional parameter: start range, if unused -1, */
    uint32 range_end= -1; /* optional parameter: end range, if unused -1 */
    int rc = BCM_E_NONE;

    /* Get optional users parameters */ 
    range_start = parameters->from;
    range_end = parameters->to;

    if ((range_end > -1) && (range_start > range_end)) {
        cli_out("Diag failed. \"from\":%d must be superior to \"to\" %d \n", range_start, range_end);
        return CMD_FAIL;
    }

    res_id = pools[pool_idx].pool_id;

    /* for each core/ instance id, display ids */

    /* Get the number of cores for this resource id */
    rc = bcm_dpp_am_resource_to_nof_pools( unit, res_id, &nof_cores_per_res);
    if (rc != BCM_E_NONE) {
        return CMD_FAIL;
    }

    for (core_id=0; core_id<nof_cores_per_res; ++core_id) 
    {
        rc = bcm_dpp_am_resource_id_to_pool_id_get (unit, core_id, res_id, &pool_id);
        if (rc != BCM_E_NONE) {
          return CMD_FAIL;
        }
        /* get the lowest id for the current pool and the number of entries */
        ret = sw_state_res_pool_get(unit,pool_id,0,&low_id,&nbr_entries,0,0);
        if (ret != BCM_E_NONE) {
            cli_out("Diag failed. Can't get configuration for pool: %d \n", pool_id);
            return CMD_FAIL;
        }

        /* init number of entries used already displayed for the current pool */
        number_entries_used_found = 0;

        /* init lowest id for current pool */
        id = low_id;

        /* get the number of used entries in the current pool */
        ret = sw_state_res_pool_info_get(unit, pool_id, &pool_info);
        if (ret != BCM_E_NONE) {
            cli_out("Diag failed. Can't get status for pool: %d \n", pool_id);
            return CMD_FAIL;
        }

        cli_out("\nPool %s(%d) Total number of entries: %d   Used entries: %d   Low entry ID is: %d(0x%x) \n\r",
                pools[pool_idx].poolDescription, pool_id, 
                nbr_entries, pool_info.used, low_id, low_id);

        if(pool_info.used)
            cli_out("List of used entries in this pool:\n\r");

        /* display ids for each used entries of the current pool */
        while (number_entries_used_found != pool_info.used && id<low_id+nbr_entries) {

            /* found an id in use to display */
            if (sw_state_res_check(unit,pool_id,1,id) == BCM_E_EXISTS) {

                /* the user has selected range, check the current id is within the range */
                if ((range_start == -1 || range_start <= id) && (range_end == -1 || range_end >= id)) {

                    /* call the print method */
                    ret = eval_object_cb(unit,id);
                    if (ret != CMD_OK) {
                        cli_out("Diag failed. Can't display info\n");
                        return CMD_FAIL;
                    }
                }
                number_entries_used_found++;
            }
            id++;
        }
    }
    cli_out("\n");
    cli_out("\n");
    return CMD_OK;  
}

cmd_result_t 
cmd_diag_alloc_hw_ive_print(int unit, diag_alloc_system_table_parameters *parameters)
{
    uint32 ret;
    uint32 command_id;
    uint32 tmp_command_id;
    int is_first_action_print=1;
    int is_advanced_mode;
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY command_key;
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO command_info;

    is_advanced_mode = SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit);

    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(&command_key);
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&command_info);

    for (command_id = parameters->from; command_id <= parameters->to; command_id++) {
        is_first_action_print=1;
        if (sw_state_res_check(unit,dpp_am_res_vlan_edit_action_ingress,1,command_id) == BCM_E_EXISTS){

            if(diag_alloc_print_flag==0){
                if (is_advanced_mode) {
                    cli_out(" -----------------------------------------------------------------------------------------------------------------------------\n\r"
                            "|                                                Ingress VLAN Editing Table(HW)                                               |\n\r"
                            " -----------------------------------------------------------------------------------------------------------------------------\n\r");
                   cli_out("|        ||        |  VALN   ||        |         |          OUTER                      ||          INNER                      |\n\r"
                           "| Action ||  TAG   |  Edit   || Tags   | TPID    | PCP+DEI   | TPID   |      VID       || PCP+DEI   | TPID   |      VID       |\n\r"
                           "|   ID   || format | Profile || Remove | Profile | Source    | index  |     Source     || Source    | index  |     Source     |\n\r");
                   cli_out(" -----------------------------------------------------------------------------------------------------------------------------\n\r");
                }
                else{

                    cli_out(" -----------------------------------------------------------------------------------------------------------------------------------------\n\r"
                            "|                                                     Ingress VLAN Editing Table(HW)                                                      |\n\r"
                            " -----------------------------------------------------------------------------------------------------------------------------------------\n\r");
                    cli_out("|        ||                    |  VALN   ||        |         |          OUTER                      ||          INNER                      |\n\r"
                            "| Action ||        TAG         |  Edit   || Tags   | TPID    | PCP+DEI   | TPID   |      VID       || PCP+DEI   | TPID   |      VID       |\n\r"
                            "|   ID   ||      format        | Profile || Remove | Profile | Source    | index  |     Source     || Source    | index  |     Source     |\n\r");
                    cli_out(" -----------------------------------------------------------------------------------------------------------------------------------------\n\r");
                }
                diag_alloc_print_flag=1;
            }

            ret = soc_ppd_lif_ing_vlan_edit_command_info_get(unit, command_id, &command_info);
            if(soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK){
                cli_out("Diag failed, soc_ppd_lif_ing_vlan_edit_command_info_get failed \n");
                return CMD_FAIL;
            }

            for (command_key.tag_format=0; command_key.tag_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; command_key.tag_format++){
               for (command_key.edit_profile=0;command_key.edit_profile < DPP_NOF_INGRESS_VLAN_EDIT_PROFILES; command_key.edit_profile++) {

                   ret = soc_ppd_lif_ing_vlan_edit_command_id_get(unit, &command_key, &tmp_command_id);
                   if(soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK){
                       cli_out("Diag failed, soc_ppd_lif_ing_vlan_edit_command_id_get failed \n");
                       return CMD_FAIL;
                   }
                    if (command_id==tmp_command_id) {
                        is_first_action_print=0;
                        if (is_advanced_mode) {
                            cli_out("| %-7d|| %-7d| %-8d|| %-7d| %-8d| %-9s | %-6d | %-14s || %-9s | %-6d | %-14s |\n\r",
                                    command_id, command_key.tag_format, command_key.edit_profile,command_info.tags_to_remove,command_info.tpid_profile,
                                    SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.outer_tag.pcp_dei_source), command_info.outer_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.outer_tag.vid_source),
                                    SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.inner_tag.pcp_dei_source), command_info.inner_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.inner_tag.vid_source));
                        }
                        else{
                            cli_out("| %-7d|| %-2d(%-14s)| %-8d|| %-7d| %-8d| %-9s | %-6d | %-14s || %-9s | %-6d | %-14s |\n\r",
                                    command_id, command_key.tag_format, soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(command_key.tag_format), command_key.edit_profile, command_info.tags_to_remove,command_info.tpid_profile,
                                    SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.outer_tag.pcp_dei_source), command_info.outer_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.outer_tag.vid_source),
                                    SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.inner_tag.pcp_dei_source), command_info.inner_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.inner_tag.vid_source));
                        }
                    }
                }
            }
            if (is_first_action_print==1) {
                if (is_advanced_mode) {
                    cli_out("| %-7d||        |         || %-7d| %-8d| %-9s | %-6d | %-14s || %-9s | %-6d | %-14s |\n\r",
                            command_id, command_info.tags_to_remove, command_info.tpid_profile,
                            SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.outer_tag.pcp_dei_source), command_info.outer_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.outer_tag.vid_source),
                            SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.inner_tag.pcp_dei_source), command_info.inner_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.inner_tag.vid_source));
                }
                else{
                    cli_out("| %-7d||                    |         || %-7d| %-8d| %-9s | %-6d | %-14s || %-9s | %-6d | %-14s |\n\r",
                            command_id, command_info.tags_to_remove, command_info.tpid_profile,
                            SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.outer_tag.pcp_dei_source), command_info.outer_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.outer_tag.vid_source),
                            SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.inner_tag.pcp_dei_source), command_info.inner_tag.tpid_index, SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.inner_tag.vid_source));

                }
            }
            if (is_advanced_mode) {
                cli_out(" -----------------------------------------------------------------------------------------------------------------------------\n\r"); 
            } else {
                cli_out(" -----------------------------------------------------------------------------------------------------------------------------------------\n\r");
                }
            }
    }

    return CMD_OK;
}


cmd_result_t 
cmd_diag_alloc_hw_eve_print(int unit, diag_alloc_system_table_parameters *parameters)
{

    uint32 ret;
    uint32 entry_id;
    int is_advanced_mode;
    SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY command_key;
    SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO command_info;

    SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear(&command_key);
    SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_clear(&command_info);

    is_advanced_mode = SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit);

    if (is_advanced_mode) {
      cli_out(" -------------------------------------------------------------------------------------------------------------------\n\r"
              "|                                           Egress VLAN Editing Table(HW)                                           |\n\r"
              " -------------------------------------------------------------------------------------------------------------------\n\r");
      cli_out("|        ||        |  VALN   ||        |          OUTER                      ||          INNER                      |\n\r"
              "|  Entry ||  TAG   |  Edit   || Tags   | PCP+DEI   | TPID   |      VID       || PCP+DEI   | TPID   |      VID       |\n\r"
              "|   ID   || format | Profile || Remove | Source    | index  |     Source     || Source    | index  |     Source     |\n\r");
      cli_out(" -------------------------------------------------------------------------------------------------------------------\n\r");
    }
    else{

        cli_out(" -------------------------------------------------- ---------------------------------------------------------------------------\n\r"
                "|                                                Egress VLAN Editing Table(HW)                                                  |\n\r"
                " -------------------------------------------------------------------------------------------------------------------------------\n\r");
       cli_out("|        ||                    |  VALN   ||        |          OUTER                      ||          INNER                      |\n\r"
               "|  Entry ||        TAG         |  Edit   || Tags   | PCP+DEI   | TPID   |      VID       || PCP+DEI   | TPID   |      VID       |\n\r"
               "|   ID   ||      format        | Profile || Remove | Source    | index  |     Source     || Source    | index  |     Source     |\n\r");
       cli_out(" -------------------------------------------------------------------------------------------------------------------------------\n\r");
    }

    for (entry_id = parameters->from; entry_id <= parameters->to; entry_id++) {
        command_key.tag_format = entry_id >> 4;
        command_key.edit_profile = entry_id & 0xf;

        ret = soc_ppd_eg_vlan_edit_command_info_get(unit, &command_key, &command_info); 
        if(soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK){
           cli_out("Diag failed, soc_ppd_eg_vlan_edit_command_info_get failed \n");
           return CMD_FAIL;
        }

        if (is_advanced_mode) {
                cli_out("| %-7d|| %-7d| %-8d|| %-7d| %-9s | %-6d | %-14s || %-9s | %-6d | %-14s |\n\r",
                        entry_id, command_key.tag_format, command_key.edit_profile, command_info.tags_to_remove,
                        SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.outer_tag.pcp_dei_source), command_info.outer_tag.tpid_index, SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.outer_tag.vid_source),
                        SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.inner_tag.pcp_dei_source), command_info.inner_tag.tpid_index, SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.inner_tag.vid_source));
            }
            else{
                cli_out("| %-7d|| %-2d-%-14s| %-8d|| %-7d| %-9s | %-6d | %-14s || %-9s | %-6d | %-14s |\n\r",
                        entry_id, command_key.tag_format, soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(command_key.tag_format), command_key.edit_profile, command_info.tags_to_remove,
                        SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.outer_tag.pcp_dei_source), command_info.outer_tag.tpid_index, SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.outer_tag.vid_source),
                        SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(command_info.inner_tag.pcp_dei_source), command_info.inner_tag.tpid_index, SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(command_info.inner_tag.vid_source));
            }
    }

    if (is_advanced_mode)
        cli_out(" -------------------------------------------------------------------------------------------------------------------\n\r");
    else
        cli_out(" -------------------------------------------------------------------------------------------------------------------------------\n\r");

    return CMD_OK;
}

/* get fec block and print them */
int 
cmd_diag_alloc_hw_fec_get_block_and_print(int unit, 
                                          SOC_PPC_FRWRD_FEC_MATCH_RULE *prm_rule, 
                                          SOC_SAND_TABLE_BLOCK_RANGE   *prm_block_range, 
                                          int                          block_size) {

    uint32 cur_indx,
           nof_entries_to_print,
           total_to_print;
    uint32 *prm_fec_array = NULL;
    uint32 prm_nof_entries, 
           total_nof_entries=0;
    uint32 ret;
    uint32 total_nof_entries_to_scan;

#if defined(INCLUDE_DUNE_UI)
    char user_msg[5] = "";
    uint32 con;
#endif

    total_nof_entries_to_scan = prm_block_range->entries_to_scan;
    /* init */

    prm_fec_array = (uint32*)sal_alloc(sizeof(uint32) * prm_block_range->entries_to_act, "prm_fec_array");
    if(prm_fec_array == NULL) {
        cli_out("Memory allocatoin failure.\n");
        return CMD_FAIL;
    }

    /* Call function */

    ret = soc_ppd_frwrd_fec_get_block(
          unit,
          prm_rule,
          prm_block_range,
          prm_fec_array,
          &prm_nof_entries
        );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
        cli_out("Diag failed, soc_ppd_frwrd_fec_get_block - FAIL \n");
        sal_free(prm_fec_array);
        return CMD_FAIL;
    }


    /* print results */

    total_nof_entries +=  prm_nof_entries;

    cur_indx = 0;
    total_to_print = 32000;
    while (cur_indx <= total_to_print) {

        if (prm_nof_entries == 0) {
            break;
        }

        nof_entries_to_print = SOC_SAND_MIN(block_size, total_to_print - cur_indx );
        ret = soc_ppd_frwrd_fec_print_block(
                  unit,
                  prm_rule,
                  prm_block_range, 
                  prm_fec_array,
                  prm_nof_entries
                );

        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
                cli_out("Diag failed, soc_ppd_frwrd_fec_print_block - FAIL \n");
                sal_free(prm_fec_array);
                return CMD_FAIL;
        }

        cur_indx+= nof_entries_to_print;

        if (cur_indx < total_to_print) {

#if defined(INCLUDE_DUNE_UI)

            soc_sand_os_printf("Hit Space to continue/Any Key to Abort.\n\r");
            con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
            if (!con) {
                break;
            } 

#endif
            /* Call function */
            ret = soc_ppd_frwrd_fec_get_block(
                    unit,
                    prm_rule,
                    prm_block_range,
                    prm_fec_array,
                    &prm_nof_entries
                  );

            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
                cli_out("Diag failed, soc_ppd_frwrd_fec_get_block - FAIL \n");
                sal_free(prm_fec_array);
                return CMD_FAIL;
            }

            if (total_nof_entries_to_scan <= total_nof_entries){
                break;
            }

            if (total_nof_entries_to_scan - total_nof_entries < prm_nof_entries)
            {
                prm_nof_entries = total_nof_entries_to_scan - total_nof_entries;
            }

            total_nof_entries +=  prm_nof_entries;
        } else {
            goto exit;
        }
    }

    soc_sand_os_printf("nof_entries: %u\n",total_nof_entries);

    goto exit;
exit:
    if (prm_fec_array != NULL)
    {
      sal_free(prm_fec_array);
    }

    return CMD_OK;
}


cmd_result_t 
cmd_diag_alloc_hw_fec_get_block(int unit, 
                                diag_alloc_system_table_parameters *parameters) {


    uint32 ret;
    SOC_PPC_FRWRD_FEC_MATCH_RULE prm_rule;
    SOC_SAND_TABLE_BLOCK_RANGE prm_block_range;

    /* setup parameters */

    unit = (unit); 

    SOC_PPC_FRWRD_FEC_MATCH_RULE_clear(&prm_rule);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);

    prm_rule.type = SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ALL;
    prm_block_range.entries_to_act = DIAG_ALLOC_BLOCK_SIZE;
    prm_block_range.entries_to_scan = 10000;
    SOC_SAND_TBL_ITER_SET_BEGIN(&prm_block_range.iter);

    cmd_diag_alloc_dune_shell_parameters_setup(&prm_block_range, parameters); 



    /* call function */
    ret = cmd_diag_alloc_hw_fec_get_block_and_print(unit, 
                                                    &prm_rule, 
                                                    &prm_block_range, 
                                                    DIAG_ALLOC_BLOCK_SIZE);

    if (ret != 0) {
        cli_out("Diag failed, cmd_diag_alloc_hw_fec_get_block_and_print failed \n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

/* get inLif block and print them */
int 
cmd_diag_alloc_hw_lif_table_get_block_and_print(int unit, 
                                          SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE *prm_rule, 
                                          SOC_SAND_TABLE_BLOCK_RANGE   *prm_block_range, 
                                          int                          block_size) {

    uint32 cur_indx,
           nof_entries_to_print,
           total_to_print;
    SOC_PPC_LIF_ENTRY_INFO *prm_entries_array=NULL;
    uint32 prm_nof_entries, 
           total_nof_entries=0;
    uint32 ret;   

#if defined(INCLUDE_DUNE_UI)
    char user_msg[5] = "";
    uint32 con;
#endif

    /* init */

    prm_entries_array = (SOC_PPC_LIF_ENTRY_INFO*)sal_alloc(sizeof(SOC_PPC_LIF_ENTRY_INFO) * prm_block_range->entries_to_act, "prm_entries_array");
    if(prm_entries_array == NULL) {
        cli_out("Memory allocatoin failure.\n");
        return CMD_FAIL;
    }

    /* Call function */

    ret = soc_ppd_lif_table_get_block(
          unit,
          prm_rule,
          prm_block_range,
          prm_entries_array,
          &prm_nof_entries
        );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
        cli_out("Diag failed, soc_ppd_lif_table_get_block - FAIL \n");
        sal_free(prm_entries_array);
        return CMD_FAIL;
    }


    /* print results */

    total_nof_entries +=  prm_nof_entries;

    cur_indx = 0;
    total_to_print = 20000;
    while (cur_indx <= total_to_print) {

        if (prm_nof_entries == 0) {
            break;
        }

        nof_entries_to_print = SOC_SAND_MIN(block_size, total_to_print - cur_indx );
        ret = soc_ppd_lif_table_print_block(
                  unit,
                  prm_rule,
                  prm_block_range,
                  prm_entries_array,
                  prm_nof_entries
                );

        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
                cli_out("Diag failed, soc_ppd_lif_table_print_block - FAIL \n");
                sal_free(prm_entries_array);
                return CMD_FAIL;
        }

        if (SOC_SAND_TBL_ITER_IS_END(&prm_block_range->iter)) {
            cli_out("nof_entries: %u\n",total_nof_entries);
            sal_free(prm_entries_array);
            return CMD_OK;
        }

        cur_indx+= nof_entries_to_print;

        if (cur_indx < total_to_print) {

#if defined(INCLUDE_DUNE_UI)

            soc_sand_os_printf("Hit Space to continue/Any Key to Abort.\n\r");
            con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
            if (!con) {
                break;
            } 

#endif

            /* Call function */
            ret = soc_ppd_lif_table_get_block(
                    unit,
                    prm_rule,
                    prm_block_range,
                    prm_entries_array,
                    &prm_nof_entries
                  );

            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
                cli_out("Diag failed, soc_ppd_frwrd_fec_get_block - FAIL \n");
                sal_free(prm_entries_array);
                return CMD_FAIL;
            }
            total_nof_entries +=  prm_nof_entries;
        } else {
            goto exit;
        }
    }

    soc_sand_os_printf("nof_entries: %u\n",total_nof_entries);

    goto exit;
exit:
    if (prm_entries_array != NULL)
    {
      sal_free(prm_entries_array);
    }

    return CMD_OK;
}

/* get inLif from the hardware and print them
   convert line to parameters, setup parameters and call get_block_and_print */
cmd_result_t 
cmd_diag_alloc_hw_lif_get_block(int unit, 
                                diag_alloc_system_table_parameters *parameters) {


    uint32 ret;
    SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE prm_rule;
    SOC_SAND_TABLE_BLOCK_RANGE prm_block_range;

    /* setup parameters */

    unit = (unit); 

    SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear(&prm_rule);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);

    prm_rule.entries_type_bm = SOC_PPC_LIF_ENTRY_TYPE_ALL;
    prm_block_range.entries_to_act = DIAG_ALLOC_BLOCK_SIZE;
    prm_block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    SOC_SAND_TBL_ITER_SET_BEGIN(&prm_block_range.iter);

    cmd_diag_alloc_dune_shell_parameters_setup(&prm_block_range, parameters);

    /* Call function */
    ret = cmd_diag_alloc_hw_lif_table_get_block_and_print(unit,
                                                          &prm_rule,
                                                          &prm_block_range,
                                                          DIAG_ALLOC_BLOCK_SIZE);

    if (ret != 0) {
        cli_out("Diag failed, cmd_diag_alloc_hw_lif_table_get_block_and_print failed ");
        return CMD_FAIL;
    }
    return CMD_OK;
}

cmd_result_t
cmd_diag_alloc_init(int unit, args_t *a, int pool_idx) {

    cmd_result_t result;
    bcm_error_t rv;
    int pool_id;
    int low_id;
    int lowest_id;
    int tot_nbr_entries;
    int nbr_entries;
    int res_id;
    uint8 nof_cores_per_res;
    int core_id;
    int last_start_id = -1;
    diag_alloc_system_table_parameters parameters;  /* diag alloc parameters passed by user in bcm shell */
    cmd_result_t (*eval_object_cb) (int,int); /* define callback for print */

    /* init */
    diag_alloc_current_display_per_line = 0;
    diag_alloc_print_flag = 0;

    /* set parameters*/
    diag_alloc_system_table_parameters_clear(&parameters);
    result = diag_alloc_system_table_parameters_parse(unit, a, &parameters);
    if (result != CMD_OK) {
        cli_out("Diag failed, can't parse diag alloc's parameters \n");
        return CMD_FAIL;
    }

    tot_nbr_entries=0;
    lowest_id=0;
    res_id = pools[pool_idx].pool_id; 

    rv = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_cores_per_res);
    if (rv != BCM_E_NONE) {
      return CMD_FAIL;
    }


    for (core_id=0 ; core_id < nof_cores_per_res ; core_id++) {

        rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
        if (rv != BCM_E_NONE) {
          return CMD_FAIL;
        }

        /* get the lowest id for the current pool and the number of entries */
        rv = sw_state_res_pool_get(unit,pool_id,0,&low_id,&nbr_entries,0,0);
        if (rv != BCM_E_NONE) {
            cli_out("Pool %s is unavalible.\n\r", pools[pool_idx].poolDescription);
            return CMD_OK;
        }
        else{
            if (core_id==0){
                lowest_id=low_id;
            } else if (low_id <= last_start_id) {
                /* The resource is per core rather than per range. The range is overlapping, so break. */
                break;
            }
            last_start_id = low_id;
            tot_nbr_entries += nbr_entries; 
        }
    }

    if (parameters.from == -1)
        parameters.from=lowest_id;
    if (parameters.to == -1)
        parameters.to = lowest_id + tot_nbr_entries - 1;

    if (parameters.from < lowest_id) {
        cli_out("Diag failed. Minimal number of action is %d \n", lowest_id);
        return CMD_FAIL;
    }

    if (parameters.to > (tot_nbr_entries + lowest_id)) {
        cli_out("Diag failed. Maximum number of action is %d \n", tot_nbr_entries);
        return CMD_FAIL;
    }

    if (parameters.from > parameters.to) {
        cli_out("Diag failed. \"from\":%d must be superior to \"to\" %d \n", parameters.from, parameters.to);
        return CMD_FAIL;
    }

    /* set print function */
    if (parameters.info > 0) {
        eval_object_cb = pools[pool_idx].info_print_cb;
    } else {
        eval_object_cb = diag_alloc_system_table_print_Ids;
    }

    /* decide how to get data (hw or sw) */
    /* get data from hardware */
    if (parameters.direct) {
         if (pools[pool_idx].cmd_diag_alloc_hw_block == NULL)
            return CMD_NOTIMPL;
         else
            return (pools[pool_idx].cmd_diag_alloc_hw_block)(unit, &parameters); 
    } 
    /* get data from software */
    else {
        if (eval_object_cb == NULL)
            return CMD_NOTIMPL;
        else
            return cmd_diag_alloc_sw_common_eval_object(unit, &parameters, eval_object_cb, pool_idx);
    }
}

cmd_result_t
cmd_diag_alloc_all_print(int unit)
{

    int pool_id; /* current pool id, after calculating by core. */
    int low_id; /* base id for the current pool */
    int lowest_id;
    int nbr_entries; /* number of entries in the pool */ 
    int tot_nbr_entries;
    int tot_used_entries;
    int i;
    int res_id;
    uint8 core_id, nof_cores_per_res;
    uint32 ret = CMD_OK;

    sw_state_res_pool_info_t pool_info; /* get number of used id for current pool */

    for (i = 0; i < sizeof(pools) / sizeof(DIAG_ALLOC_POOL_DEF); i++) {
        lowest_id=0;
        tot_used_entries=0;
        tot_nbr_entries=0;
        res_id = pools[i].pool_id; 


        cli_out(" Pool %-41s ", pools[i].poolDescription);
        ret = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_cores_per_res);
        if (ret != BCM_E_NONE) {
          return CMD_FAIL;
        }

        /* for each pool id, display ids */
        for (core_id=0 ; core_id < nof_cores_per_res ; core_id++) {

            ret = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
            if (ret != BCM_E_NONE) {
              return CMD_FAIL;
            }

            /* get the lowest id for the current pool and the number of entries */
            ret = sw_state_res_pool_get(unit,pool_id,0,&low_id,&nbr_entries,0,0);
            if (ret != BCM_E_NONE) {
                cli_out(" is unavalible.\n\r");
            }
            else{
                tot_nbr_entries += nbr_entries;
                /* get the number of used entries in the current pool */
                ret = sw_state_res_pool_info_get(unit, pool_id, &pool_info);
                if (ret != BCM_E_NONE) {
                    cli_out("Diag failed. Can't get status for pool: %s \n", pools[i].poolName);
                    return CMD_FAIL;
                }
                else{
                    if(core_id == 0) 
                        lowest_id=low_id;
                    if (low_id<lowest_id) 
                        lowest_id=low_id;
                    tot_used_entries += pool_info.used; 
                }
            }
        }

        if (ret == BCM_E_NONE){
            cli_out("Total number of entries: %-6d  Used entries %-5d  Lowest entry ID is: %d(0x%x) \n\r", 
                    tot_nbr_entries, tot_used_entries, lowest_id, lowest_id);
        }
    }

    return CMD_OK;
}


cmd_result_t cmd_diag_alloc_my_in_test(int unit)
{
       int rv; 
       bcm_dpp_am_local_inlif_info_t inlif_info;
       uint32 flags = 0; /* BCM_DPP_AM_LIF_FLAG_WITH_ID*/
       /*int lif_id[2];*/

    inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_isid;
    inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE;
    inlif_info.glif = 2; /* pre-alloced Global LIF ID*/ 
        inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
    rv = _bcm_dpp_am_local_inlif_alloc(unit, flags, &inlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Inlif alloced . base id= %d , ext_id=%d \n\r", inlif_info.base_lif_id, inlif_info.ext_lif_id);

    /*lif_id[0]= inlif_info.base_lif_id;*/

    inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
    inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON;
    inlif_info.glif = 4; /* pre-alloced Global LIF ID*/ 
            inlif_info.counting_profile_id = 0;
    rv = _bcm_dpp_am_local_inlif_alloc(unit, flags, &inlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Inlif alloced . alloc id= %d , \n\r", inlif_info.base_lif_id);
    /*lif_id[1]= inlif_info.base_lif_id;*/

    inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
    inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE;
    inlif_info.glif = 6; /* pre-alloced Global LIF ID*/ 
    rv = _bcm_dpp_am_local_inlif_alloc(unit, flags, &inlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Inlif alloced . base id= %d , ext_id=%d \n\r", inlif_info.base_lif_id, inlif_info.ext_lif_id);

    /*lif_id[2]= inlif_info.base_lif_id;*/

    inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
    inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE;
    inlif_info.glif = 8; /* pre-alloced Global LIF ID*/ 
    rv = _bcm_dpp_am_local_inlif_alloc(unit, flags, &inlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Inlif alloced . base id= %d , ext_id=%d \n\r", inlif_info.base_lif_id, inlif_info.ext_lif_id);

    inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_isid;
    inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE;
    inlif_info.glif = 22; /* pre-alloced Global LIF ID*/ 
        inlif_info.counting_profile_id = 0;
    rv = _bcm_dpp_am_local_inlif_alloc(unit, flags, &inlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Inlif alloced . base id= %d , ext_id=%d \n\r", inlif_info.base_lif_id, inlif_info.ext_lif_id);

        inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_isid;
    inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE;
    inlif_info.glif = 25; /* pre-alloced Global LIF ID*/ 
        inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
    rv = _bcm_dpp_am_local_inlif_alloc(unit, flags, &inlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Inlif alloced . base id= %d , ext_id=%d \n\r", inlif_info.base_lif_id, inlif_info.ext_lif_id);

   inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
    inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE;
    inlif_info.glif = 11; /* pre-alloced Global LIF ID*/ 
    inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
    rv = _bcm_dpp_am_local_inlif_alloc(unit, flags, &inlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Inlif alloced . base id= %d , ext_id=%d \n\r", inlif_info.base_lif_id, inlif_info.ext_lif_id);
#if 0
    lif_id[3]= inlif_info.base_lif_id;

    rv = _bcm_dpp_am_local_inlif_is_alloc(unit,lif_id[0]);
    if (rv == BCM_E_EXISTS) {
        cli_out(" Inlif 0 exists  \n\r");
    }
    else if (rv == BCM_E_NOT_FOUND) 
    {
        cli_out(" Inlif %d not found \n\r", lif_id[0]);
    }
    else
    {
        return CMD_FAIL;
    }
  
    rv = _bcm_dpp_am_local_inlif_dealloc(unit, lif_id[0]);
    cli_out(" Inlif %d dealloced  \n\r", lif_id[0]);

    rv = _bcm_dpp_am_local_inlif_is_alloc(unit,lif_id[0]);
    if (rv == BCM_E_EXISTS) {
        cli_out(" Inlif %d exists  \n\r", lif_id[0]);
    }
    else if (rv == BCM_E_NOT_FOUND) 
    {
        cli_out(" Inlif %d not found \n\r",lif_id[0]);
    }
    else
    {
        return CMD_FAIL;
    }

    rv = _bcm_dpp_am_local_inlif_dealloc(unit, lif_id[3]);
        cli_out(" Inlif %d dealloced  \n\r", lif_id[3]);
#endif
    cli_out(" Test done.\n\r");

    return CMD_OK;
}

cmd_result_t cmd_diag_alloc_my_in_counter_profile_test(int unit)
{

       int rv; 
       int counter_profile=0;

      rv = _bcm_dpp_am_local_inlif_counting_profile_set(unit, counter_profile,31000, 11000);
      if (rv != BCM_E_NONE) {
        return CMD_FAIL;
     }

      rv = _bcm_dpp_am_local_outlif_counting_profile_set(unit, counter_profile,19000, 10000, FALSE);
      if (rv != BCM_E_NONE) {
        return CMD_FAIL;
     }

      
     return CMD_OK;
}

cmd_result_t cmd_diag_alloc_my_out_test(int unit)
{
       int rv;
       int lif_id[6] = {0};
       int global_lif_id;

       bcm_dpp_am_local_out_lif_info_t outlif_info;
       uint32 flags = 0; /* BCM_DPP_AM_LIF_FLAG_WITH_ID*/
/*
       prop_config.key=1;
       prop_config.value=BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT;

      rv = bcm_petra_switch_lif_property_set(unit,bcmLifPropertyOutGlobalLifRangeType,&prop_config);
if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
       prop_config.key=1;
       prop_config.value=BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_1;
      rv = bcm_petra_switch_lif_property_set(unit,bcmLifPropertyEEDBBankPhase,&prop_config);
if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }

       sal_memset(&outlif_info,0,sizeof(bcm_dpp_am_local_out_lif_info_t));
        outlif_info.app_alloc_info.pool_id =  dpp_am_res_eg_out_rif;
        outlif_info.app_alloc_info.application_type = 0;
        outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_DIRECT;
        outlif_info.glif = 5001;  
        outlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
        rv = _bcm_dpp_am_local_outlif_alloc(unit, flags, &outlif_info);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }

        cli_out(" Outlif alloced . alloc id= %d , \n\r", outlif_info.base_lif_id);
        lif_id[0] = outlif_info.base_lif_id;
*/
              sal_memset(&outlif_info,0,sizeof(bcm_dpp_am_local_out_lif_info_t));
        outlif_info.app_alloc_info.pool_id =  dpp_am_res_eep_ip_tnl;
        outlif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_ip_tunnel_roo;
        outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_COUPLED;
        outlif_info.glif = 5002; /* pre-alloced Global LIF ID*/ 
        outlif_info.counting_profile_id = 0;
        rv = _bcm_dpp_am_local_outlif_alloc(unit, flags, &outlif_info);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        cli_out(" Outlif alloced . alloc id= %d , \n\r", outlif_info.base_lif_id);
        lif_id[1] = outlif_info.base_lif_id;
        rv = bcm_dpp_am_global_lif_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, &global_lif_id);
                if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        /* 
        outlif_info.app_alloc_info.pool_id =  dpp_am_res_eep_global;
        outlif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_linker_layer;
        outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_COUPLED | BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
        outlif_info.glif = 5003;  pre-alloced Global LIF ID                                         ;
        */
        outlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
        rv = _bcm_dpp_am_local_outlif_alloc(unit, flags, &outlif_info);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        cli_out(" Outlif alloced . Global id=%d, local lif id= %d , ext_id=%d \n\r",global_lif_id,  outlif_info.base_lif_id, outlif_info.ext_lif_id );
        lif_id[2] = outlif_info.base_lif_id;

                rv = bcm_dpp_am_global_lif_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, &global_lif_id);
                if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }

        outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_COUPLED | BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
        outlif_info.glif = 5004; /* pre-alloced Global LIF ID*/ 
        rv = _bcm_dpp_am_local_outlif_alloc(unit, flags, &outlif_info);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        cli_out(" Outlif alloced . Global id=%d, local lif id= %d , ext_id=%d \n\r",global_lif_id,  outlif_info.base_lif_id, outlif_info.ext_lif_id );
        lif_id[3] = outlif_info.base_lif_id;

    outlif_info.app_alloc_info.pool_id =  dpp_am_res_eg_out_rif;
    outlif_info.app_alloc_info.application_type = 0;
    outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_COUPLED;
    outlif_info.glif = 5005; /* pre-alloced Global LIF ID*/ 
    rv = _bcm_dpp_am_local_outlif_alloc(unit, flags, &outlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Outlif alloced . alloc id= %d , \n\r", outlif_info.base_lif_id);

        lif_id[4] = outlif_info.base_lif_id;

    outlif_info.app_alloc_info.pool_id =  dpp_am_res_eg_data_erspan;
    outlif_info.app_alloc_info.application_type = 0;
    outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_COUPLED;
    outlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
    outlif_info.glif = 5006; /* pre-alloced Global LIF ID*/ 
    rv = _bcm_dpp_am_local_outlif_alloc(unit, flags, &outlif_info);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
    cli_out(" Outlif alloced . alloc id= %d , \n\r", outlif_info.base_lif_id);

    lif_id[5] = outlif_info.base_lif_id;


    rv = _bcm_dpp_am_local_outlif_dealloc(unit, lif_id[1]);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
        cli_out(" Outlif dealloced . lif id= %d , \n\r", lif_id[0]);

    rv = _bcm_dpp_am_local_outlif_dealloc(unit, lif_id[4]);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }
        cli_out(" Outlif dealloced . lif id= %d , \n\r", lif_id[4]);

    cli_out(" Test done.\n\r");

    return CMD_OK;
}

cmd_result_t cmd_diag_alloc_eedb_banks(int unit)
{
    int rv , bank_id_iter; 
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;

    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit) ; bank_id_iter++) 
    {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.eedb_info.banks.get(unit, bank_id_iter, &eedb_bank_info);
        if (rv != BCM_E_NONE) {
                return CMD_FAIL;
        }    
        if (bank_id_iter % 2 == 0)
        {
            cli_out(" \n");
            cli_out(" EEDB bank id %d configuration:\n\r", bank_id_iter/2);
            
        }
        cli_out(" Half id= %d :", bank_id_iter);
        cli_out(" Phase= %d , is extended = %d, ext_type=%d Type=%d Free/total=%d/%d \n\r",
                eedb_bank_info.phase, eedb_bank_info.is_extended , eedb_bank_info.ext_type, eedb_bank_info.type,
                eedb_bank_info.nof_free_entries, _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit));
        if (eedb_bank_info.is_extended )
        {
            cli_out(" Extension bank = %d \n\r", eedb_bank_info.extension_bank);
            
        }
    }

    return CMD_OK;

}


cmd_result_t cmd_diag_alloc_failover_link_list(int unit)
{
    int rv , failover_id_iter;
    uint32 size ;

    for (failover_id_iter = 0; failover_id_iter < _BCM_DPP_AM_LOCAL_LIF_NOF_FAILOVER_IDS ;failover_id_iter ++ ) 
    {
        rv = LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, failover_id_iter, &size) ;
        if (rv != BCM_E_NONE) {
                return CMD_FAIL ;
        }
        if (size != 0 )
        {
            rv = failover_linked_list_print(unit,failover_id_iter);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }   
        }
    }

    return CMD_OK;

}

cmd_result_t cmd_diag_alloc_eth_type_index_tabel(int unit)
{
    int rv , index; 
    _l2_ether_type_index_entry_ref_t entry_ref_cnt;
    SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO          eth_type_index_entry ;

    cli_out("\n");
    cli_out(" entry index            ethertype(ref)            outertpid(ref)            innertpid(ref)\n");

    for (index = 0;index < _BCM_L2_NUM_OF_ETH_TYPE_INDEX_ENTRY;index++)  {

        sal_memset(&eth_type_index_entry,0,sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO));
        rv = sw_state_access[unit].dpp.bcm.l2.ether_type_index_table_ref_cnt.get(unit,index,&entry_ref_cnt);
        if (rv != BCM_E_NONE) {
                return CMD_FAIL ;
        }
       /* coverity[returned_value : FALSE] */   
       rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_get(
           unit, index,&eth_type_index_entry);
        cli_out("\n");
        cli_out("%-24d",index);
        cli_out("0x%-4x(%4d)                 ",eth_type_index_entry.ether_type,entry_ref_cnt.eth_type_ref);
        cli_out("0x%-4x(%4d)                 ",eth_type_index_entry.tpid_0,entry_ref_cnt.outer_tpid_ref);
        cli_out("0x%-4x(%4d)                 ",eth_type_index_entry.tpid_1,entry_ref_cnt.inner_tpid_ref);

    }
    cli_out("\n");

    return CMD_OK;

}

cmd_result_t
cmd_dpp_diag_alloc(int unit, args_t* a)
{
    char      *function;
    int i;

    function = ARG_GET(a);

    if (!function)
        return CMD_USAGE;
    
    if (sal_strcasecmp(function, "all")==0) {
        return cmd_diag_alloc_all_print(unit);
    }
    if (sal_strcasecmp(function, "in_test")==0) {
        return cmd_diag_alloc_my_in_test(unit);
    }
    if (sal_strcasecmp(function, "cp_test")==0) {
        return cmd_diag_alloc_my_in_counter_profile_test(unit);
    }
    if (sal_strcasecmp(function, "out_test")==0) {
        return cmd_diag_alloc_my_out_test(unit);
    }

    if (sal_strcasecmp(function, "eedb")==0) {
        return cmd_diag_alloc_eedb_banks(unit);
    }
    if (SOC_IS_JERICHO(unit)) {
        if (sal_strcasecmp(function, "failover")==0) {
            return cmd_diag_alloc_failover_link_list(unit);
        }
        if (sal_strcasecmp(function, "eth_index")==0) {
            return cmd_diag_alloc_eth_type_index_tabel(unit);
        }
    }

    for (i = 0; i < sizeof(pools) / sizeof(DIAG_ALLOC_POOL_DEF); i++) {
        if(DIAG_FUNC_STR_MATCH(function, pools[i].poolName, pools[i].shortPoolName))
        {
            return cmd_diag_alloc_init(unit,a,i);
        }
    }

    return CMD_USAGE;
}


void
print_alloc_usage(int unit)
{
    int i;
    char cmd_dpp_diag_alloc_usage[] =
    "Usage (DIAG alloc):"
    "\n\tDIAGnotsics allocation commands\n\t"
    "Usages:\n\t"
    "DIAG alloc [OPTION] <parameters> ..."
#ifdef __PEDANTIC__
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
    "OPTION can be:"
    "\nall - display info for all pools";
#endif   /*COMPILER_STRING_CONST_LIMIT*/

    cli_out(cmd_dpp_diag_alloc_usage);

    for (i = 0; i < sizeof(pools) / sizeof(DIAG_ALLOC_POOL_DEF); i++){
        cli_out("\n%-12s or %-7s - display info for %s pool",pools[i].poolName, pools[i].shortPoolName, pools[i].poolDescription);
    };

    cli_out("\n\nparameters can be(available for all options except all)\n");
    cli_out("info=1          = display entry information\n");
    cli_out("direct=1        = get entry information directly from hardware\n");
    cli_out("from=<number>   = display from this entry id\n");
    cli_out("to=<number>     = display till this entry id\n");

}

cmd_result_t 
dpp_diag_alloc_cosq_sched_print(int unit,
                                int core,
                                int flow_id,
                                int flow_count,
                                char* allocated_resource)
{
    int rc = BCM_E_NONE;
    int region_type;
    int quad_start_flow_id;
    int i = -1; 
    int allocated_ref_len;
    int flow_region_size = 1024;
    bcm_dpp_am_cosq_pool_ref_t allocated_ref;
    bcm_dpp_am_cosq_quad_allocation_t is_allocated;
    SOC_TMC_AM_SCH_FLOW_TYPE flow_type;

    allocated_ref.entries = (int*)sal_alloc(sizeof(int) * DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit) * flow_region_size, "allocated refernce");
    if (allocated_ref.entries == NULL) {
        return CMD_FAIL;
    }

    cli_out("\n\t\tCosq scheduler allocation manager. Core %d\n\n", core);
    cli_out("\t\t============================================\n\n");

    if (-1 != flow_id) {
        quad_start_flow_id = (flow_id/4)*4;

        if (SOC_IS_QAX(unit)) {
            flow_id = BCM_COSQ_FLOW_ID_QAX_ADD_OFFSET(unit, flow_id);
        }

        while(flow_count-- > 0) {
            {
                rc = bcm_dpp_am_cosq_get_region_type(unit,core,flow_id,&region_type);
                if (rc != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                rc = bcm_dpp_am_cosq_fetch_quad(unit,core,flow_id,region_type,&is_allocated);
                if (rc != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                cli_out("\t\tDisplay allocation status of flow_ids: %d - %d\n", quad_start_flow_id, quad_start_flow_id+3);
                switch (region_type) {
                case DPP_DEVICE_COSQ_REGION_TYPE0:
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow1));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow1));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow1));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow1));
                    break;
                case DPP_DEVICE_COSQ_REGION_TYPE1:
                case DPP_DEVICE_COSQ_REGION_TYPE5:
                    cli_out("\t\tCL (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow1));
                    cli_out("\t\tFQ (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow2));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow3));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow4));
                    break;
                case DPP_DEVICE_COSQ_REGION_TYPE2:
                case DPP_DEVICE_COSQ_REGION_TYPE6:
                    cli_out("\t\tCL (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow1));
                    cli_out("\t\tFQ (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow2));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow3));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow4));
                    cli_out("\t\tCL CONNECTOR (%d,%d): %s\n",quad_start_flow_id -4,quad_start_flow_id -2,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow3||is_allocated.flow1));
                    cli_out("\t\tFQ CONNECTOR (%d,%d): %s\n",quad_start_flow_id -3,quad_start_flow_id -1,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow4||is_allocated.flow2));
                    break;
                case DPP_DEVICE_COSQ_REGION_TYPE3:
                    cli_out("\t\tCL (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow1));
                    cli_out("\t\tHR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow2));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow3));
                    cli_out("\t\tCONNECTOR (%d): %s\n",quad_start_flow_id++,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow4));
                    cli_out("\t\tCL CONNECTOR (%d,%d): %s\n",quad_start_flow_id -4,quad_start_flow_id -2,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow3||is_allocated.flow1));
                    cli_out("\t\tHR CONNECTOR (%d,%d): %s\n",quad_start_flow_id -3,quad_start_flow_id -1,DIAG_ALLOC_COSQ_SCHED_IS_FREE_GET_STRING(is_allocated.flow4||is_allocated.flow2));
                    break;
                default:
                    return CMD_FAIL;
                }
            }
        }
    }

    if (allocated_resource) {
        if (!sal_strncmp(allocated_resource,"connector",strlen(allocated_resource))) {
            flow_type = SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR;
        } else if (!sal_strncmp(allocated_resource,"cl",strlen(allocated_resource))) {
            flow_type = SOC_TMC_AM_SCH_FLOW_TYPE_CL;
        } else if (!sal_strncmp(allocated_resource,"fq",strlen(allocated_resource))) {
            flow_type = SOC_TMC_AM_SCH_FLOW_TYPE_FQ;
        } else if (!sal_strncmp(allocated_resource,"hr",strlen(allocated_resource))) {
            flow_type = SOC_TMC_AM_SCH_FLOW_TYPE_HR;
        } else if (!sal_strncmp(allocated_resource,"cl_composite",strlen(allocated_resource))) {
            flow_type = SOC_TMC_AM_SCH_FLOW_TYPE_CL_COMPOSITE;
        } else if (!sal_strncmp(allocated_resource,"fq_composite",strlen(allocated_resource))) {
            flow_type = SOC_TMC_AM_SCH_FLOW_TYPE_FQ_COMPOSITE;
        } else if (!sal_strncmp(allocated_resource,"hr_composite",strlen(allocated_resource))) {
            flow_type = SOC_TMC_AM_SCH_FLOW_TYPE_HR_COMPOSITE;
        } else {
            return CMD_FAIL;
        }

        cli_out("\n\n\t\tAllocated %s flow_ids:\n", allocated_resource);

        rc = bcm_dpp_am_cosq_fetch_allocated_resources(unit, core, flow_type, &allocated_ref);
        if (rc != BCM_E_NONE) {
            return CMD_FAIL;
        }

        allocated_ref_len = (allocated_ref.max_entries > allocated_ref.valid_entries) ? allocated_ref.valid_entries - 1 : allocated_ref.max_entries - 1;
        while (++i < allocated_ref_len) {
            cli_out("%d, ",allocated_ref.entries[i]);
        }
        cli_out("%d.\n",allocated_ref.entries[i]);
        cli_out("\t\tTotal: %d\n",allocated_ref_len + 1);
    }

    sal_free(allocated_ref.entries);

    return CMD_OK;
}


void
print_tmplt_usage(int unit)
{
    char cmd_dpp_diag_tmplt_usage[] =
    "Usage (DIAG template):"
    "\n\tDIAGnotsics Template manager commands\n\t"
    "Usages:\n\t"
    "DIAG template [OPTION] "
#ifdef __PEDANTIC__
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
    "OPTION can be:"
    "\n\tall    display all resourse types and basic information about them"
    "\n\ttype # display extended information about specific template type"    
    "\n\n";
#endif   /*COMPILER_STRING_CONST_LIMIT*/    
    cli_out(cmd_dpp_diag_tmplt_usage);
}

cmd_result_t
cmd_dpp_diag_tmplt(int unit, args_t* a)
{
    char *function;
    int  val;
    function = ARG_GET(a);

    if (function) {
        int strnlen_function;
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if(!sal_strncasecmp(function, "all",strnlen_function)) {
            val = -1;
        } else {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strncasecmp(function, "type", strnlen_function)) {
            function = ARG_GET(a);
            if (function) {
                val = sal_ctoi(function,0);
            } else
                return CMD_USAGE;
        } else {
            return CMD_USAGE;
          }
        }
    }
    else {
        return CMD_USAGE;
    }

    if(shr_template_dump(unit, val)){
        return CMD_FAIL;
    }
    return CMD_OK;
}
