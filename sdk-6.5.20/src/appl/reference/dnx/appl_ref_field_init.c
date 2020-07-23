/** \file appl_ref_field_init.c
 * 
 *
 * Stacking Programmable mode application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
/** soc */
#include <soc/schanmsg.h>
#include <shared/utilex/utilex_seq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dbal/dbal.h>

/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "appl_ref_field_init.h"
#include "appl_ref_l2_init.h"

/** bcm */
#include <bcm/field.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

/*
 * }
 */

/*
 * DEFINEs
 */
#define LEARN_RCY_PORT1 41
#define LEARN_RCY_PORT2 44

#define OLP_PORT_0 240
#define OLP_PORT_1 241
#define OLP_PORT_2 242
#define OLP_PORT_3 243
/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
* \brief
*  Configures FG type const
* \param [in] unit        -  Device id
* \param [in] stage -  PMF Stage
* \param [in] context_id  -  PMF context Id
* \param [in] action_type  -  PMF action_type
* \param [in] action_value  -  PMF action_value
* \param [in] action_value  -  Name of The Field group and action
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static int
appl_dnx_field_group_const_util(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_action_priority_t action_priorty,
    bcm_field_action_t action_type,
    int action_value,
    char *name)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    bcm_field_group_t fg_id = 0;
    bcm_field_action_t action_id = 0;
    void *dest_char;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, action_type, stage, &action_info_get_size));

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = action_type;
    action_info.prefix_size = action_info_get_size.size;
    action_info.prefix_value = action_value;
    action_info.size = 0;
    action_info.stage = stage;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_id));

    /*
     * Create and attach Const Field group to generate constant value for action
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = stage;

    /*
     * Constant FG : number of qualifiers in key must be 0 (empty key)
     */
    fg_info.nof_quals = 0;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_info[0].priority = action_priorty;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_fec_dest_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_field.init.fec_dest_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_j1_same_port_cb(
    int unit,
    int *dynamic_flags)
{

    if ((dnx_data_field.init.j1_same_port_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_learn_and_limit_cb(
    int unit,
    int *dynamic_flags)
{

    if (((dnx_data_field.init.j1_learning_get(unit)) &&
         (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
          dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
        || (dnx_data_field.init.learn_limit_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_learn_nullify_key_msbs_cb(
    int unit,
    int *dynamic_flags)
{

    if (((dnx_data_field.init.j1_learning_get(unit)) &&
         (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
          dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
        || (dnx_data_field.init.learn_limit_get(unit)))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;;
    }
    else if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_refresh_events_wrong_key_msbs)
             && (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
                 dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_j1_php_cb(
    int unit,
    int *dynamic_flags)
{

    if (((dnx_data_field.init.j1_php_get(unit)) &&
         (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
          dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_oam_layer_index_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oam_offset_supported))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    else
    {
        if (dnx_data_field.init.oam_layer_index_get(unit))
        {
            *dynamic_flags = 0;
        }
        else
        {
            *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
        }
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_trap_l4_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_field.init.l4_trap_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_oam_stat_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_field.init.oam_stat_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_flow_id_cb(
    int unit,
    int *dynamic_flags)
{

    if (dnx_data_field.init.flow_id_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }

    return _SHR_E_NONE;
}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_roo_cb(
    int unit,
    int *dynamic_flags)
{

    if ((dnx_data_field.init.roo_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;
}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_ipmc_inlif_cb(
    int unit,
    int *dynamic_flags)
{

    if ((dnx_data_field.init.jr1_ipmc_inlif_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;
}

/**
* \brief
*  Add entries per trap code and layer protocol+1 type
* \param [in] unit               - Device ID
* \param [in] fg_id              - FG ID
* \param [in] fg_info            - FG info
* \param [in] trap_id            - Trap id values key
* \param [in] pars_inc           -  Action of how much to increment the FWD layer
* \param [in] fwd_plus_one_type  - Forwarding+1 protocol type
* \param [in] fwd_plus_one_mask  -  Forwarding+1 mask
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
appl_ref_field_oam_layer_add_entries(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info,
    int trap_id,
    int pars_inc,
    int fwd_plus_one_type,
    int fwd_plus_one_mask)
{
    bcm_field_entry_info_t entry_info;
    int ii;
    bcm_field_entry_t dummy_ent;
    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < 8; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = fg_info->nof_actions;
        entry_info.nof_entry_quals = fg_info->nof_quals;

        entry_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
        entry_info.entry_qual[0].value[0] = fwd_plus_one_type;
        entry_info.entry_qual[0].mask[0] = fwd_plus_one_mask;
        entry_info.entry_qual[1].type = bcmFieldQualifyRxTrapCode;
        entry_info.entry_qual[1].value[0] = trap_id;
        entry_info.entry_qual[1].mask[0] = 0x1FF;
        entry_info.entry_qual[2].type = bcmFieldQualifyForwardingLayerIndex;
        entry_info.entry_qual[2].value[0] = ii;
        entry_info.entry_qual[2].mask[0] = 0x7;

        entry_info.entry_action[0].type = bcmFieldActionEgressForwardingIndex;
        entry_info.entry_action[0].value[0] = (ii + pars_inc) % 8;

        entry_info.entry_action[1].type = bcmFieldActionForwardingLayerIndex;
        entry_info.entry_action[1].value[0] = (ii + pars_inc) % 8;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals Trap LR_type %d Trap %d Index %d Action %d \n",
                     entry_info.entry_qual[0].value[0],
                     entry_info.entry_qual[1].value[0],
                     entry_info.entry_qual[2].value[0], entry_info.entry_action[0].value[0]);

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));

    }

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Will Add create a field group With relevant entries in TCAM for OAM and attach it to given as param context
* \param [in] unit        - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_oam_layer_index_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    int ii = 0;
    int nof_quals = 3;
    int nof_actions = 2;

    /*
     *
     *
     *   Trap Type                               |     Trap ID             |  Layer_FWD + 1      | Parsing_start_index
     * OAM_TRAP_ETH_OAM                   224                        X                          FWD + 1
     * OAM_TRAP_Y1731_O_MPLSTP        225                        X                          FWD
     * OAM_TRAP_Y1731_O_PWE            226                        X                          FWD
     * OAM_TRAP_BFD_O_IPV4               227                        X                          FWD + 2
     * OAM_TRAP_BFD_O_MPLS              228                        X                          FWD + 2
     * OAM_TRAP_BFD_O_PWE               229                     IPv4/IPv6                 FWD + 2
     * OAM_TRAP_BFDCC_O_MPLSTP       230                     IPv4/IPv6                 FWD + 2
     * OAM_TRAP_BFDCV_O_MPLSTP       231                     IPv4/IPv6                 FWD + 2
     * OAM_TRAP_BFD_O_PWE               229                        X                          FWD
     * OAM_TRAP_BFDCC_O_MPLSTP       230                        X                          FWD
     * OAM_TRAP_BFDCV_O_MPLSTP       231                        X                          FWD
     * OAM_TRAP_BFD_O_IPV6           232                        X                          FWD + 2
     *
     * Entrys are going to be add only for traps that need to increment the Parsing index of Forwarding
 */

    /**********************************/
    /*
     * Globals for oam_layer_index
     */
    /**********************************/
    int OAM_TRAP_ETH_OAM = 224;
    int OAM_TRAP_BFD_O_IPV4 = 227;
    int OAM_TRAP_BFD_O_MPLS = 228;
    int OAM_TRAP_BFD_O_PWE = 229;
    int OAM_TRAP_BFDCC_O_MPLSTP = 230;
    int OAM_TRAP_BFDCV_O_MPLSTP = 231;
    int OAM_TRAP_BFD_O_IPV6 = 232;

    int LayerTypeMask = 0x1F;

    bcm_field_group_t fg_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /*
     * / SHR_IF_ERR_EXIT(appl_ref_field_oam_add_entries(unit, OAM_TRAP_ETH_OAM, 1, 0, 0));
     *
     *
     * * Set quals
     */
    fg_info.nof_quals = nof_quals;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = bcmFieldQualifyRxTrapCode;
    fg_info.qual_types[2] = bcmFieldQualifyForwardingLayerIndex;

    /*
     * Set actions
     */
    fg_info.nof_actions = nof_actions;

    fg_info.action_types[0] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_FWD_IDX_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

   /**Entries for OAM_TRAP_ETH_OAM*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries(unit, fg_id, &fg_info, OAM_TRAP_ETH_OAM, 1, 0, 0));

   /**Entries for OAM_TRAP_BFD_O_IPV4*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries(unit, fg_id, &fg_info, OAM_TRAP_BFD_O_IPV4, 2, 0, 0));

   /**Entries for OAM_TRAP_BFD_O_IPV6*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries(unit, fg_id, &fg_info, OAM_TRAP_BFD_O_IPV6, 2, 0, 0));

   /**Entries for OAM_TRAP_BFD_O_MPLS*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries(unit, fg_id, &fg_info, OAM_TRAP_BFD_O_MPLS, 2, 0, 0));

   /**Entries for OAM_TRAP_BFD_O_PWE*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries
                    (unit, fg_id, &fg_info, OAM_TRAP_BFD_O_PWE, 2, bcmFieldLayerTypeIp4, LayerTypeMask));

   /**Entries for OAM_TRAP_BFD_O_PWE*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries
                    (unit, fg_id, &fg_info, OAM_TRAP_BFD_O_PWE, 2, bcmFieldLayerTypeIp6, LayerTypeMask));

   /**Entries for OAM_TRAP_BFDCC_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries
                    (unit, fg_id, &fg_info, OAM_TRAP_BFDCC_O_MPLSTP, 2, bcmFieldLayerTypeIp4, LayerTypeMask));

   /**Entries for OAM_TRAP_BFDCC_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries
                    (unit, fg_id, &fg_info, OAM_TRAP_BFDCC_O_MPLSTP, 2, bcmFieldLayerTypeIp6, LayerTypeMask));

   /**Entries for OAM_TRAP_BFDCV_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries
                    (unit, fg_id, &fg_info, OAM_TRAP_BFDCV_O_MPLSTP, 2, bcmFieldLayerTypeIp4, LayerTypeMask));

   /**Entries for OAM_TRAP_BFDCV_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_oam_layer_add_entries
                    (unit, fg_id, &fg_info, OAM_TRAP_BFDCV_O_MPLSTP, 2, bcmFieldLayerTypeIp6, LayerTypeMask));

exit:
    SHR_FUNC_EXIT;
}

/*
* When hitting TCP UDP traps in FLP they might be False Positive since the HW do not validate that the next protocol of IP is indeed TCP/UDP.
* Hence we add a validation in iPMF3 that in case the trap was hit but the next protocol is not TCP/UDP we disable the trap.

TCP SEQ and flags are zero                                   bcmRxTrapTcpSnFlagsZero
TCP SEQ is zero and either FIN/URG or PSH are one            bcmRxTrapTcpSnZeroFlagsSet
TCP SYN and FIN are set                                      bcmRxTrapTcpSynFin
TCP Source port equals destination port                      bcmRxTrapTcpEqualPorts
TCP fragment without full TCP header                         bcmRxTrapTcpFragmentIncompleteHeader
TCP fragment with offset less than 8                         bcmRxTrapTcpFragmentOffsetLt8
UDP Source port equals destination port                      bcmRxTrapUdpEqualPorts
*/

/**
* \brief
*  Add entries:
*  When +1 protocol match the hit trap - nothing should happen
*  When +1 protocol do no match the hit trap - should preform some defualt trap that has no actions configured
* \param [in] unit               - Device ID
* \param [in] fg_id              - FG ID
* \param [in] fg_info            - FG info
* \param [in] action_do_nothing  - The do nothing action
* \param [in] trap_do_nothing    - The do nothing trap
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
appl_ref_field_trap_l4_add_entries(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info,
    bcm_field_action_t action_do_nothing,
    bcm_gport_t trap_do_nothing)
{
    bcm_field_entry_info_t entry_info;
    int ii;
    int trap_code = 0;
    int tcp_udp_nof_traps = 7;
    bcm_field_entry_t dummy_ent;

    bcm_rx_trap_t *field_trap_tcp_udp_types_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(field_trap_tcp_udp_types_p, sizeof(*field_trap_tcp_udp_types_p) * tcp_udp_nof_traps,
              "field_trap_tcp_udp_types_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    field_trap_tcp_udp_types_p[0] = bcmRxTrapTcpSnFlagsZero;
    field_trap_tcp_udp_types_p[1] = bcmRxTrapTcpSnZeroFlagsSet;
    field_trap_tcp_udp_types_p[2] = bcmRxTrapTcpSynFin;
    field_trap_tcp_udp_types_p[3] = bcmRxTrapTcpEqualPorts;
    field_trap_tcp_udp_types_p[4] = bcmRxTrapTcpFragmentIncompleteHeader;
    field_trap_tcp_udp_types_p[5] = bcmRxTrapTcpFragmentOffsetLt8;
    field_trap_tcp_udp_types_p[6] = bcmRxTrapUdpEqualPorts;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Add entries that match +1 protocol and should not change the behavior %s%s%s%s\n",
                 EMPTY, EMPTY, EMPTY, EMPTY);

    for (ii = 0; ii < tcp_udp_nof_traps; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = 1;
        entry_info.nof_entry_quals = fg_info->nof_quals;

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, field_trap_tcp_udp_types_p[ii], &trap_code));

        entry_info.entry_qual[0].type = bcmFieldQualifyRxTrapCode;
        entry_info.entry_qual[0].value[0] = trap_code;
        entry_info.entry_qual[0].mask[0] = 0x1FF;
        entry_info.entry_qual[1].type = bcmFieldQualifyLayerRecordType;
        entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeTcp;
        entry_info.entry_qual[1].mask[0] = 0x1F;
        if (field_trap_tcp_udp_types_p[ii] == bcmRxTrapUdpEqualPorts)
        {
            entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeUdp;
        }

        entry_info.entry_action[0].type = action_do_nothing;
        entry_info.entry_action[0].value[0] = 0;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals Trap %d LR_type %d LR_Mask %d Action %d \n",
                     entry_info.entry_qual[0].value[0], entry_info.entry_qual[1].value[0],
                     entry_info.entry_qual[1].mask[0], entry_info.entry_action[0].value[0]);

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Add entries that DO NOT match +1 protocol should not change the behavior %s%s%s%s\n",
                 EMPTY, EMPTY, EMPTY, EMPTY);
    for (ii = 0; ii < tcp_udp_nof_traps; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = 1;
        entry_info.nof_entry_quals = fg_info->nof_quals;

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, field_trap_tcp_udp_types_p[ii], &trap_code));

        entry_info.entry_qual[0].type = bcmFieldQualifyRxTrapCode;
        entry_info.entry_qual[0].value[0] = trap_code;
        entry_info.entry_qual[0].mask[0] = 0x1FF;
        entry_info.entry_qual[1].type = bcmFieldQualifyLayerRecordType;
        entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeTcp;
        entry_info.entry_qual[1].mask[0] = 0x0;
        if (field_trap_tcp_udp_types_p[ii] == bcmRxTrapUdpEqualPorts)
        {
            entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeUdp;
        }

        entry_info.entry_action[0].type = bcmFieldActionTrap;
        entry_info.entry_action[0].value[0] = trap_do_nothing;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals Trap %d LR_type %d LR_Mask %d Action %d \n",
                     entry_info.entry_qual[0].value[0], entry_info.entry_qual[1].value[0],
                     entry_info.entry_qual[1].mask[0], entry_info.entry_action[0].value[0]);

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));
    }

exit:
    SHR_FREE(field_trap_tcp_udp_types_p);
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Will Add create a field group With relevant entries in TCAM for OAM and attach it to given as param context
* \param [in] unit        - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_trap_l4_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_attach_info_t attach_info;
    bcm_rx_trap_config_t trap_config;
    int trap_id;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int ii = 0;
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;

    bcm_field_action_t action_do_nothing;

    bcm_gport_t trap_do_nothing = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 31;
    action_info.size = 1;
    action_info.stage = bcmFieldStageIngressPMF1;

    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "L4_TRAP_Do_Nothing", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_do_nothing));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapCode;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordType;

    /*
     * Set actions
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.action_types[1] = action_do_nothing;

   /**For Void action there is no reason to put Valid bit, only waste bit (since not performed by FES)*/
    fg_info.action_with_valid_bit[1] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "L4_TRAP_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
   /**Would like to take */
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

   /**Max Strength*/
    BCM_GPORT_TRAP_SET(trap_do_nothing, trap_id, 15, 7);

   /**Entries */
    SHR_IF_ERR_EXIT(appl_ref_field_trap_l4_add_entries(unit, fg_id, &fg_info, action_do_nothing, trap_do_nothing));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add entries per stat_lm_index to +1
* \param [in] unit               - Device ID
* \param [in] fg_id              - FG ID
* \param [in] fg_info            - FG info
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
appl_ref_field_oam_stat_add_entries(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info)
{
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t dummy_ent;

    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Add entries%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_actions = fg_info->nof_actions;
    entry_info.nof_entry_quals = fg_info->nof_quals;
    entry_info.entry_qual[0].type = fg_info->qual_types[0];
    entry_info.entry_action[0].type = fg_info->action_types[0];
    entry_info.entry_qual[0].mask[0] = 0x3;

    entry_info.priority = 0;
    entry_info.entry_qual[0].value[0] = bcmFieldStatOamLmIndex0;
    entry_info.entry_action[0].value[0] = bcmFieldStatOamLmIndex1;
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));

    entry_info.priority = 1;
    entry_info.entry_qual[0].value[0] = bcmFieldStatOamLmIndex1;
    entry_info.entry_action[0].value[0] = bcmFieldStatOamLmIndex2;
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));

    entry_info.priority = 2;
    entry_info.entry_qual[0].value[0] = bcmFieldStatOamLmIndex2;
    entry_info.entry_action[0].value[0] = bcmFieldStatOamLmIndexInvalid;
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));

    entry_info.priority = 3;
    entry_info.entry_qual[0].value[0] = bcmFieldStatOamLmIndexInvalid;
    entry_info.entry_action[0].value[0] = bcmFieldStatOamLmIndex0;
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals stat_LM_id %d  Action stat_LM_id %d %s%s\n",
                 entry_info.entry_qual[0].value[0], entry_info.entry_action[0].value[0], EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Will Add create a field group With relevant entries in TCAM for OAM and attach it to given as param context
* \param [in] unit        - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_oam_stat_init(
    int unit)
{

    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_attach_info_t attach_info;
    int ii = 0;
    void *dest_char;
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /*
     * Set quals
     */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyStatOamLM;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatOamLM;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "OAM__stat_lm", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(appl_ref_field_oam_stat_add_entries(unit, fg_id, &fg_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Since FER is not  setting the dest type of trap correctly (i.e. setting dest instead of updating trap code)
* we need iPMF3  will do simple case of dest = dest and let hw handle the case dest type trap.
*
*  iPMF3 DE with dest = dest
* \param [in] unit        - Device id
* \param [in] Context_id        - Context to attach the FEC Field group to.
* \param [in] add_df        - If TRUE a FEC field group will be add otherwise only attached to context
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
appl_dnx_field_fec_dest_on_context(
    int unit,
    bcm_field_context_t context_id,
    int add_fg)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id = dnx_data_field.group.id_fec_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (add_fg == TRUE)
    {
        bcm_field_action_info_t action_info;
        bcm_field_action_t fec_dest_action = 0;
        void *dest_char;
        /** Create destination action to ignore destination qualifier which is 0*/
        bcm_field_action_info_t_init(&action_info);
        action_info.action_type = bcmFieldActionForward;
        action_info.prefix_size = 11;
        action_info.prefix_value = 0;
        action_info.size = 21;
        action_info.stage = bcmFieldStageIngressPMF3;
        dest_char = &(action_info.name[0]);
        sal_strncpy_s(dest_char, "FEC_Dst_w_o_qual", sizeof(action_info.name));

        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &fec_dest_action));

        bcm_field_group_info_t_init(&fg_info);
        fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
        fg_info.nof_quals = 1;
        fg_info.stage = bcmFieldStageIngressPMF3;

        fg_info.qual_types[0] = bcmFieldQualifyDstPort;

        fg_info.nof_actions = 1;
        fg_info.action_with_valid_bit[0] = FALSE;

        /** Order of actions should be the same as order of qualifiers*/
        fg_info.action_types[0] = fec_dest_action;

        dest_char = &(fg_info.name[0]);
        sal_strncpy_s(dest_char, "FEC_dest", sizeof(fg_info.name));

        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    }
    else
    {
        bcm_field_group_info_t_init(&fg_info);
        SHR_IF_ERR_EXIT(bcm_field_group_info_get(unit, fg_id, &fg_info));
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    /**Destination is taken from meta data*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Since FER is not  setting the dest type of trap  (i.e. setting dest instead of updating trap code)
* we need iPMF3 which will do simple case of dest = dest and let hw handle the case dest type trap.
*
*  iPMF3 DE with dest = dest
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_fec_dest_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_field_fec_dest_on_context(unit, context_id, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* ERPP Fwd-Context Selection
*   (FAI==0) -->  FWD-Context = ETH  =>  this is the problem
* ERPP Ethernet filters are executed, packet may be trapped
* Fix traps in PMF
*    PMF Context Selection by: PPH_Present(1b) == 0
*       No lookup
*       Actions:
*           Forward-Action-Profile = NORMAL (0) => revert ERPP filters
*           ACE_Context_Value[1:0] = J_Mode_FWD_Code_Override (via ACE-Pointer) => ensure correct context selection in ETPP
*    ETPP Fwd-Code Selection
*       (ACE_Context_Value[1:0] == J_Mode_FWD_Code_Override) --> FWD-Context = TM
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_j1_same_port_init(
    int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure the ACE format.
     */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "J1_SAME_P_ACE", sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id));

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextForwardingCodeOverride;

    SHR_IF_ERR_EXIT(bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_SAME_P_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id));

    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageEgress, context_id, BCM_FIELD_ACTION_DONT_CARE, bcmFieldActionAceEntryId,
                     ace_entry_handle, "J1_SAME_P_CNST_ACE"));

    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageEgress, context_id, BCM_FIELD_ACTION_DONT_CARE, bcmFieldActionTrap, 0,
                     "J1_SAME_P_CNST_TRAP"));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    /***
     * lowest priority presel
 */
    p_id.presel_id = dnx_data_field.preselector.default_j1_same_port_presel_id_epmf_get(unit);
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPphPresent;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = 0;
    p_data.qual_data[0].qual_mask = 0x1;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Egress parser always assume that FLOW extension present
* Hence need to create PMF to set the valid bit for flow ext to TRUE
* Since PMF is the one to generate the Flow id we set the action to be flow_id=0 flow_profile=0 and flow_valid=1
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_flow_id_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t flow_action = 0;
    bcm_field_action_info_t action_info_predefined;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_action_info_get
                    (unit, bcmFieldActionLatencyFlowId, bcmFieldStageIngressPMF1, &action_info_predefined));

    /** Create destination action to ignore destination qualifier which is 0*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionLatencyFlowId;
    action_info.prefix_size = action_info_predefined.size;
    /**Valid bit is the lsb*/
    action_info.prefix_value = 1;
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Flow_valid_bit", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &flow_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_actions = 1;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[0] = flow_action;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Flow_FG", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    /**Make the action to be lowest priority*/
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 0);

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_field_roo_init(
    int unit)
{

    /*
     *  * Context "Routing, ROO"
     *   Context_Selection:
     *      Fwd layerType = IPv4/IPv6
     *      out_lif0_range (!=0x0)
     *      out_lif1_range (!=0x0)
     *   Actions: Direct_Extraction
     *      fwd_domain_id = OutLIF[0] (18 lsb)
     *      OutLIF[0] = OutLIF[1]
     *Context " Routing, Not ROO with RIF"
     *   Context_Selection:
     *      Fwd layerType = IPv4/IPv6
     *      out_lif0_range (==0x0)
     *      out_lif1_range (==0x0)
     *   Actions: Direct_Extraction
     *      fwd_domain_id = OutLIF[0] (18 lsb)
     *Context " Routing, Not ROO without RIF"
     *   Context_Selection:
     *      Fwd layerType = IPv4/IPv6
     *      out_lif0_range (!=0x0)
     *      out_lif1_range (==0x0)
     *   Actions: Direct_Extraction
     *      fwd_domain_id = 0xFFFF (18 lsb)
     */

    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id_with_roo = 0;
    bcm_field_group_t fg_id_w_o_roo_with_rif = 0;
    bcm_field_group_t fg_id_w_o_roo_no_rif = 0;
    bcm_field_group_info_t fg_info;
    int ii = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;
    bcm_field_qualifier_info_get_t qual_info_get;
    void *dest_char;
    bcm_field_context_t context_id_with_roo;
    bcm_field_context_t context_id_w_o_roo_with_rif;
    bcm_field_context_t context_id_w_o_roo_no_rif;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_range_info_t range_info;
    int range_zero = 0, range_non_zero = 1;
    SHR_FUNC_INIT_VARS(unit);

    /**bcmFieldQualifyVrf = bcmFieldActionVSwitchNew = FWD_Domain*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcmFieldQualifyVrf, bcmFieldStageIngressPMF3, &qual_info_get));

    qual_info.size = qual_info_get.size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_G_OUT_LIF_LS", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyOutVPort1;

    /*
     * Set actions
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionVSwitchNew;
    fg_info.action_types[1] = bcmFieldActionOutVport0;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_w_ROO_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id_with_roo));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_w_ROO_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_with_roo));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of GLOB_OUT_LIF_0*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyOutVPort0, bcmFieldStageIngressPMF3, &qual_info_get));

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = qual_info_get.offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id_with_roo, context_id_with_roo, &attach_info));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    /***
     * Presel of WITH_OUT roo MUST BE Higher priority (i.e. lower number)
     * Since with_out roo check if the LIF range == 0
     * If the context was not hit there it means Lif Range != 0
 */
    p_id.presel_id = dnx_data_field.preselector.default_oam_roo_ipv4_presel_id_ipmf3_get(unit);
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id_with_roo;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    p_id.presel_id = dnx_data_field.preselector.default_oam_roo_ipv6_presel_id_ipmf3_get(unit);
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /*******************************************************
     * Without Roo with RIF
 */
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_w_o_ROO_w_RIF_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id_w_o_roo_with_rif));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_w_o_ROO_w_RIF_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_w_o_roo_with_rif));

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                    (unit, 0, fg_id_w_o_roo_with_rif, context_id_w_o_roo_with_rif, &attach_info));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    /***
     * Presel of without roo MUST BE Higher priority (i.e. lower number)
     * Since with_out roo check if the LIF range == 0
     * If the context was not hit there it means Lif Range != 0
 */
    p_id.presel_id = dnx_data_field.preselector.default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get(unit);
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id_w_o_roo_with_rif;
    p_data.nof_qualifiers = 3;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    p_data.qual_data[1].qual_type = bcmFieldQualifyVPortRangeCheck;
    /**The check the range of out_lif_1 we set arg to 1*/
    p_data.qual_data[1].qual_arg = 1;
    /**Assuming profile 0 is configured for no-range*/
    p_data.qual_data[1].qual_value = 0;
    p_data.qual_data[1].qual_mask = 0x1F;

    p_data.qual_data[2].qual_type = bcmFieldQualifyVPortRangeCheck;
    /** To check the range of out_lif_0 we set arg to 0 */
    p_data.qual_data[2].qual_arg = 0;
    /** Assuming profile 0 is configured for no-range*/
    p_data.qual_data[2].qual_value = 0;
    p_data.qual_data[2].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    p_id.presel_id = dnx_data_field.preselector.default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get(unit);
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /**Configure outlif_ranges*/
    bcm_field_range_info_t_init(&range_info);
    range_zero = 0;
    range_non_zero = 1;
    range_info.max_val = 0;
    range_info.min_val = 0;
    range_info.range_type = bcmFieldRangeTypeOutVport;
    SHR_IF_ERR_EXIT(bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF3, range_zero, &range_info));

    range_info.max_val = (1 << qual_info.size) - 1;
    SHR_IF_ERR_EXIT(bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF3, range_non_zero, &range_info));

    /*******************************************************
     * Without Roo without RIF
 */
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_w_o_ROO_w_o_RIF_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id_w_o_roo_no_rif));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_w_o_ROO_w_o_RIF_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_w_o_roo_no_rif));

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[0].input_arg = 0xFFFF;
    attach_info.key_info.qual_info[0].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                    (unit, 0, fg_id_w_o_roo_no_rif, context_id_w_o_roo_no_rif, &attach_info));

    /**
     * Presel of no roo without RIF MUST BE lowest priority (i.e. highest number)
     * In this case Outlif0 range is != 0, only thing to check is outlif1 range
     */
    p_data.context_id = context_id_w_o_roo_no_rif;
    p_data.nof_qualifiers = 2;
    p_id.presel_id = dnx_data_field.preselector.default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get(unit);
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    p_id.presel_id = dnx_data_field.preselector.default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get(unit);
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    {
        /**Add FEC on ROO context's*/
        bcm_field_group_t fg_fec_id = dnx_data_field.group.id_fec_get(unit);
        int add_fec_fg_needed = FALSE;
        int rv = BCM_E_NONE;

        rv = bcm_field_group_info_get(unit, fg_fec_id, &fg_info);
        if (rv != BCM_E_NONE)
        {
            add_fec_fg_needed = TRUE;
        }
        SHR_IF_ERR_EXIT(appl_dnx_field_fec_dest_on_context(unit, context_id_with_roo, add_fec_fg_needed));
        SHR_IF_ERR_EXIT(appl_dnx_field_fec_dest_on_context(unit, context_id_w_o_roo_with_rif, FALSE));
        SHR_IF_ERR_EXIT(appl_dnx_field_fec_dest_on_context(unit, context_id_w_o_roo_no_rif, FALSE));
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
appl_dnx_field_ipmc_inlif_init(
    int unit)
{

    /*
     *  * Context "IPMC IN-LIF"
     *   Context_Selection:
     *      Fwd layerType = IPv4/IPv6
     *      Fwd-layer-qualifier[0]=1
     *      (i.e. IPMC)
     *   Actions: Direct_Extraction
     *      fwd_domain_id = InLIF[0] (18 lsb)
     *      InLIF[0] = InLIF[1]
     */

    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id_ipmc_in_lif = 0;
    bcm_field_group_info_t fg_info;
    int ii = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;
    bcm_field_qualifier_info_get_t qual_info_get;
    void *dest_char;
    bcm_field_context_t context_id_ipmc_inlif;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    SHR_FUNC_INIT_VARS(unit);

    /**bcmFieldQualifyVrf = bcmFieldActionVSwitchNew = FWD_Domain*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcmFieldQualifyVrf, bcmFieldStageIngressPMF3, &qual_info_get));

    qual_info.size = qual_info_get.size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_IPMC_IN_LIF_LS", sizeof(qual_info.name));

    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id));
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyInVPort1;

    /*
     * Set actions
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionVSwitchNew;
    fg_info.action_types[1] = bcmFieldActionInVport0;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_IPMC_IN_LIF_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id_ipmc_in_lif));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_IPMC_IN_LIF_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_ipmc_inlif));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of IN_LIF_0*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyInVPort0, bcmFieldStageIngressPMF3, &qual_info_get));

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = qual_info_get.offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id_ipmc_in_lif, context_id_ipmc_inlif, &attach_info));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dnx_data_field.preselector.default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get(unit);;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id_ipmc_inlif;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = 1;
    p_data.qual_data[1].qual_mask = 0x1;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    p_id.presel_id = dnx_data_field.preselector.default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get(unit);;;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
appl_dnx_field_jr1_php_init(
    int unit)
{

    /*
     *  * Context "JR1 MPLS PHP from EEDB"
     *   Context_Selection:
     *      Fwd layerType = MPLS
     *      Out-LIF0 is in PHP-Range
     *
     *   Actions: Direct_Extraction
     *      InLIF[0] = 0
     *
     *  * Context "JR1 MPLS SWAP from EEDB"
     *   Context_Selection:
     *      Fwd layerType = MPLS
     *
     *
     *   Actions: TCAM
     *      If EEI.identifier = 0;
     *        EEI.identifier = SWAP
     *        EEI.MPLS_LAbel = 3;
     */

    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id_jr1_php = 0, fg_id_jr1_swap = 0;
    bcm_field_group_info_t fg_info;
    int ii = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;
    void *dest_char;
    bcm_field_context_t context_id_jr1_php = 0, context_id_jr1_swap = 0;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_range_info_t range_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Fwd_plus_2_offset_t", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordOffset;
    fg_info.qual_types[1] = qual_id;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionSystemHeaderSet;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_PHP_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id_jr1_php));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_PHP_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_jr1_php));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 2;
    attach_info.key_info.qual_info[1].offset = 32;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id_jr1_php, context_id_jr1_php, &attach_info));

    bcm_field_range_info_t_init(&range_info);
    range_info.max_val = dnx_data_lif.global_lif.nof_global_out_lifs_get(unit) - dnx_data_l3.rif.nof_rifs_get(unit) - 1;
    range_info.min_val = range_info.max_val - dnx_data_mpls.general.nof_php_gport_lif_get(unit);
    range_info.range_type = bcmFieldRangeTypeOutVport;
    SHR_IF_ERR_EXIT(bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF3, 1, &range_info));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dnx_data_field.preselector.default_j1_php_presel_id_ipmf3_get(unit);;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id_jr1_php;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeForwardingMPLS;
    p_data.qual_data[0].qual_mask = 0x1F;

    p_data.qual_data[1].qual_type = bcmFieldQualifyVPortRangeCheck;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = 1;
    p_data.qual_data[1].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /*
     * Without PTCH, PHP-Label.BoS=1
     */
    for (ii = 0; ii < 12; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_id = 0;
        entry_info.nof_entry_quals = 2;
        entry_info.nof_entry_actions = 1;
        entry_info.entry_qual[0].type = bcmFieldQualifyLayerRecordOffset;
        entry_info.entry_qual[0].value[0] = 14 + ii * 4;        /* Eth-header+Ethertype+Lables */
        entry_info.entry_qual[0].mask[0] = 0xFF;
        entry_info.entry_qual[1].type = qual_id;
        entry_info.entry_qual[1].value[0] = 14 + ii * 4 + 4;
        entry_info.entry_qual[1].mask[0] = 0xFF;

        entry_info.entry_action[0].type = bcmFieldActionSystemHeaderSet;
        entry_info.entry_action[0].value[0] = 0x800001; /* EEI.identifier = POP,upper-layer is IPvX */
        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_jr1_php, &entry_info, &entry_id));

    }

    /*
     * With PTCH, PHP-Label.BoS=1
     */
    for (ii = 0; ii < 12; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_id = 0;
        entry_info.nof_entry_quals = 2;
        entry_info.nof_entry_actions = 1;
        entry_info.entry_qual[0].type = bcmFieldQualifyLayerRecordOffset;
        entry_info.entry_qual[0].value[0] = 2 + 14 + ii * 4;    /* PTCH+Eth-header+Ethertype+Lables */
        entry_info.entry_qual[0].mask[0] = 0xFF;
        entry_info.entry_qual[1].type = qual_id;
        entry_info.entry_qual[1].value[0] = 2 + 14 + ii * 4 + 4;
        entry_info.entry_qual[1].mask[0] = 0xFF;

        entry_info.entry_action[0].type = bcmFieldActionSystemHeaderSet;
        entry_info.entry_action[0].value[0] = 0x800001; /* EEI.identifier = POP,upper-layer is IPvX */
        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_jr1_php, &entry_info, &entry_id));

    }

    bcm_field_entry_info_t_init(&entry_info);
    entry_id = 0;
    entry_info.nof_entry_quals = 0;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_action[0].type = bcmFieldActionSystemHeaderSet;
    entry_info.entry_action[0].value[0] = 0x800005;     /* EEI.identifier = POP,upper-layer is MPLS */
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_jr1_php, &entry_info, &entry_id));

    /**
     * MPLS SWAP from EEDB context
     */

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /*
     * Set quals
     */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyMplsForwardingLabelAction;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionSystemHeaderSet;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_SWAP_FROM_EEDB", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id_jr1_swap));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_SWAP_FROM_EEDB_CTX", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_jr1_swap));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id_jr1_swap, context_id_jr1_swap, &attach_info));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dnx_data_field.preselector.default_j1_swap_presel_id_ipmf3_get(unit);;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id_jr1_swap;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeForwardingMPLS;
    p_data.qual_data[0].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyMplsForwardingLabelAction;
    entry_info.entry_qual[0].value[0] = 0;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFF;
    entry_info.entry_action[0].type = bcmFieldActionSystemHeaderSet;
    entry_info.entry_action[0].value[0] = 0x900003;     /* Swap Label 3 */

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_jr1_swap, &entry_info, &entry_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_field_j1_learn_ingress(
    int unit)
{

    /*
     * j1_learning
     * 1. Context selection OLP port + TM forwarding type (ITMH) -> create a context with compare in iPMF1
     * 2. 2 user defined qualifiers :
     *     1. Learn-info format payload[39:40] - 2 bits
     *     2. Type of destination : payload[16:18] - 3 bits
     * 3. Build a compare key in iPMF1 with structure:
     *    Bits [0:8]: in-port
     *    Bits [9:10]: Learn-info format (user_qual1)
     * 4. Use Direct Extraction in iPMF2 with the following key:
     *     1. Compare result equal bit [bit 3]
     *     2. Type of destination : (user_qual2) [bits 0:2]
     * 5. Use FEM action with condition of the 4 bits of the key.
     * 6. In case 4 bits condition equal to
     *   1001, 1010, 1011, 1100,1101   [values of 9,10,11,12,13] perform action redirect with some destination port (const)
     * 7. Additional configuration for second-Pass (packets that are coming in 2nd pass from recycle port):
     *    context selection based on Recycle port.
     *    Constant FG : no key, Action performed: Redirect to destination port.
     */

    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_info_t fg_info;
    int ii = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t cmp_qual_id, const_qual_id, learn_foramt_qual_id, fwd_type_qual_id, const_trap_qual_id;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t redirect_action_id, trap_action_id;
    bcm_field_action_priority_t action_priority;
    void *dest_char;
    int op_offset;
    bcm_field_fem_action_info_t fem_action_info;
    bcm_field_context_t context_id, second_pass_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_gport_t olp_port, gport_rcy;
    uint32 format_a_recycle_port = LEARN_RCY_PORT1;
    uint32 format_b_recycle_port = LEARN_RCY_PORT2;
    uint32 olp_mc = OLP_MC_ID;
    uint32 olp_port_class_id = 1;
    uint32 rcycle_port_class_id = 2;
    uint32 recycle_port_destination, olp_mc_destination;
    bcm_field_context_param_info_t param_info;
    uint8 udh_size_in_bits = dnx_data_field.udh.field_class_id_size_0_get(unit) +
        dnx_data_field.udh.field_class_id_size_1_get(unit) +
        dnx_data_field.udh.field_class_id_size_2_get(unit) + dnx_data_field.udh.field_class_id_size_3_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Recycle port Configuration
     * there are 3 ports needed for DSP packet,
     * 2 RCY ports are needed to apply different combination
     * Mode 1 : for Out-LIF+Destination format(Apply Limit+Decode+Key-Msb)
     * Mode 2 : for EEI+FEC format  (Apply Limit+Key-Msb)
     */
    SHR_IF_ERR_EXIT(bcm_port_control_set(unit, format_a_recycle_port, bcmPortControlDistributeLearnMessageMode, 1));
    SHR_IF_ERR_EXIT(bcm_port_control_set(unit, format_b_recycle_port, bcmPortControlDistributeLearnMessageMode, 2));

    /**
     * Create user defined qual for 1bit result of the desired compare operand.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_1bitCmpRes", sizeof(qual_info.name));
    qual_info.size = 1;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cmp_qual_id));

    /**
     * Create user defined qual for zero constant qualifier (2 bits)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_zeroConst", sizeof(qual_info.name));
    qual_info.size = 2;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &const_qual_id));

    /**
     * Create user defined qual for  Learning format in OLP payload (bits 39:40)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_Format", sizeof(qual_info.name));
    qual_info.size = 2;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &learn_foramt_qual_id));

    /**
     * Create user defined qual for forwarding type in OLP payload (bits 18:16)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_Fwd_Type", sizeof(qual_info.name));
    qual_info.size = 3;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &fwd_type_qual_id));

    /**
     * Create user defined qual for const trap ID
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_zeros", sizeof(qual_info.name));
    qual_info.size = 13;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &const_trap_qual_id));

    /**
     * Create user defined action for  Learning format in OLP payload (bits 39:40)
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_redirect", sizeof(action_info.name));
    action_info.size = 4;
    action_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &redirect_action_id));

    /**
      * Create user defined action for trap overriding
      */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionTrap;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_trap", sizeof(action_info.name));
    action_info.size = 13;
    action_info.prefix_size = 19;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &trap_action_id));

    BCM_GPORT_LOCAL_SET(olp_port, OLP_PORT_0);
    BCM_GPORT_LOCAL_SET(gport_rcy, format_a_recycle_port);
    /*
     * create a context in iPMF1 stage with compare mode
     */
    bcm_field_context_info_t_init(&context_info);
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_CTX", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhTsh;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));

    /***
     * create the context selection based on OLP port profile
 */
    p_id.presel_id = dnx_data_field.preselector.default_j1_learn_presel_id_1st_pass_ipmf1_get(unit);;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = olp_port_class_id;
    p_data.qual_data[1].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, olp_port, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, olp_port_class_id));

        /**
    * Init the compare_info
    */
    bcm_field_context_compare_info_t_init(&compare_info);
    /**
    * Fill the compare info
    */
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = learn_foramt_qual_id;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    compare_info.first_key_info.qual_info[0].input_arg = 1;
    compare_info.first_key_info.qual_info[0].offset = 210 + udh_size_in_bits;   /* 112(ETH) + 144(DSP) + UDH -
                                                                                 * 47(payload location) + 2(encoding
                                                                                 * location) */

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = const_qual_id;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    compare_info.second_key_info.qual_info[0].input_arg = 0;
    compare_info.second_key_info.qual_info[0].offset = 0;

        /**
    * Create the context compare mode.
    */
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info));

    /*
     * create a direct Extraction Field Group in iPMF2
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /*
     * Set quals
     */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = fwd_type_qual_id;
    fg_info.qual_types[1] = cmp_qual_id;
    fg_info.qual_types[2] = const_trap_qual_id;

    /*
     * Set actions
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = redirect_action_id;
    fg_info.action_types[1] = trap_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

    /**FEM CONFIG*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;

    /*
     * '1001' - compare '1' + encoding for DSPA
     * '101x' - compare '1' + encoding for FEC
     * '110x' - compare '1' + encoding for MC_ID
     */
    for (ii = 9; ii < 14; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionRedirect;

    /*
     * '0xxx' - this is EEI+FEC payload
     */
    for (ii = 0; ii < 8; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 1;
    }
    fem_action_info.fem_extraction[1].action_type = bcmFieldActionRedirect;

     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                    (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_PORT_ID, &format_a_recycle_port,
                     &recycle_port_destination));

    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = ((recycle_port_destination >> ii) & 0x1);
    }

    /*
     * For EEI+FEC
     */
    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                    (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_PORT_ID, &format_b_recycle_port,
                     &recycle_port_destination));

    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        fem_action_info.fem_extraction[1].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[1].output_bit[ii].forced_value = ((recycle_port_destination >> ii) & 0x1);
    }

    action_priority = BCM_FIELD_ACTION_POSITION(1, 7);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info));

    /**
     * Retrieve the desired result offset of the qualifier.
     */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyCompareKeysResult0, bcmFieldStageIngressPMF2, &qual_info_get));

    /**
     * Get the compare operand offset.
     */
    SHR_IF_ERR_EXIT(bcm_field_compare_operand_offset_get(unit, 1, bcmFieldCompareOperandEqual, &op_offset));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 232 + udh_size_in_bits;  /* 112(ETH) + 144(DSP) + UDH- 47(payload
                                                                         * location) + 26(dest encoding location) */
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = op_offset + qual_info_get.offset;
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].offset = 0;
    attach_info.key_info.qual_info[2].input_arg = 0x1E00;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    /*
     * create a context in iPMF1 stage for 2nd pass
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_2Pass_CTX", sizeof(context_info.name));

    second_pass_context_id = dnx_data_field.context.default_j1_learning_2ndpass_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &second_pass_context_id));

    /***
     * create the context selection based on OLP port profile
 */
    p_id.presel_id = dnx_data_field.preselector.default_j1_learn_presel_id_2nd_pass_ipmf1_get(unit);;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = second_pass_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = rcycle_port_class_id;
    p_data.qual_data[0].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, gport_rcy, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, rcycle_port_class_id));
    BCM_GPORT_LOCAL_SET(gport_rcy, format_b_recycle_port);
    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, gport_rcy, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, rcycle_port_class_id));

    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                    (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_MC_ID, &olp_mc, &olp_mc_destination));

    /*
     * Create a constant Field Group to redirect all packets ingressing from recyle port to destination port
     */
    action_priority = BCM_FIELD_ACTION_PRIORITY(0, 0);
    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageIngressPMF1, second_pass_context_id, action_priority,
                     bcmFieldActionRedirect, olp_mc_destination, "J1_Learn_2Pass_Rd"));

    action_priority = BCM_FIELD_ACTION_DONT_CARE;
    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageIngressPMF1, second_pass_context_id, action_priority,
                     bcmFieldActionTrap, 0x1e00, "J1_Learn_2Pass_Trap"));

    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageIngressPMF1, second_pass_context_id, action_priority,
                     bcmFieldActionIngressDoNotLearn, 0, "J1_Learn_2Pass_DisL"));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
appl_dnx_field_j1_learn_nullify_key_msbs_ingress(
    int unit)
{

    /*
     * j1_learning_nullify_key_msbs
     * 1. Context selection OLP port + TM forwarding type (ITMH) -> create a context with compare in iPMF1
     * 2. 1 user defined qualifiers :
     *     1. Learn-info format cmd_type[2:0]
     * 3. Add one entry qualify the cmd_type[2:0]:
     * 4. In case cmd_type = refresh(2) or delete(0)
     *    perform action redirect with some destination port (const)
     * 7. Additional configuration for second-Pass (packets that are coming in 2nd pass from recycle port):
     *    context selection based on Recycle port.
     *    Constant FG : no key, Action performed: Redirect to destination port.
     */

    bcm_gport_t olp_gport, rcy_gport, olp_mc_gport;
    uint32 olp_mc = OLP_MC_ID, rcy_port = LEARN_RCY_PORT1;
    uint32 cmd_type_delete = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t cmd_type_qual_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_entry_t entry_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_t context_id, second_pass_context_id;
    uint32 olp_port_class_id = 1;
    uint32 rcycle_port_class_id = 2;
    uint32 olp_mc_destination;
    bcm_field_context_param_info_t param_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_priority_t action_priority;

    uint8 udh_size_in_bits = dnx_data_field.udh.field_class_id_size_0_get(unit) +
        dnx_data_field.udh.field_class_id_size_1_get(unit) +
        dnx_data_field.udh.field_class_id_size_2_get(unit) + dnx_data_field.udh.field_class_id_size_3_get(unit);

    bcm_field_entry_info_t entry_info;

    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Recycle port Configuration
     * Only Key-Msb needed;
     * 1 RCY needed;
     * Mode 1 :Apply Key-Msb
     */
    SHR_IF_ERR_EXIT(bcm_port_control_set(unit, rcy_port, bcmPortControlDistributeLearnMessageMode, 1));

    BCM_GPORT_LOCAL_SET(olp_gport, OLP_PORT_0);
    BCM_GPORT_LOCAL_SET(rcy_gport, rcy_port);

    /**
     * Create user defined qual for DSP CMD type in OLP payload (bits 177:175)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "DSP_CMD_Type", sizeof(qual_info.name));
    qual_info.size = 3;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cmd_type_qual_id));

    /*
     * create a context in iPMF1 stage
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_Nullify_Key_CTX", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhTsh;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));

    /***
     * create the context selection based on OLP port profile
 */
    p_id.presel_id = dnx_data_field.preselector.default_j1_learn_presel_id_1st_pass_ipmf1_get(unit);;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = olp_port_class_id;
    p_data.qual_data[1].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, olp_gport, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, olp_port_class_id));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /*
     * Set quals
     */

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cmd_type_qual_id;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionRedirect;
    fg_info.action_with_valid_bit[0] = TRUE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_Nullify_key_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 113 + udh_size_in_bits;  /* 112(ETH) + 6(CMD type location in DSP) */

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    /*
     * Add the entries documented in the beginning of the function
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 0;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    SHR_IF_ERR_EXIT(bcm_stk_gport_sysport_get(unit, rcy_gport, &rcy_gport));
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = cmd_type_delete;        /* Value of CMD type "refresh or delete" */
    entry_info.entry_qual[0].mask[0] = 5;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = rcy_gport;

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 0;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    BCM_GPORT_MCAST_SET(olp_mc_gport, olp_mc);
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0;      /* Value of CMD type "insert" */
    entry_info.entry_qual[0].mask[0] = 0;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = olp_mc_gport;

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

    /*
     * create a context in iPMF1 stage for 2nd pass
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_Nullify_Key_2Pass_CTX", sizeof(context_info.name));

    second_pass_context_id = dnx_data_field.context.default_j1_learning_2ndpass_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &second_pass_context_id));

    /***
     * create the context selection based on OLP port profile
 */
    p_id.presel_id = dnx_data_field.preselector.default_j1_learn_presel_id_2nd_pass_ipmf1_get(unit);;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = second_pass_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = rcycle_port_class_id;
    p_data.qual_data[0].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    BCM_GPORT_LOCAL_SET(rcy_gport, rcy_port);
    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, rcy_gport, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, rcycle_port_class_id));

    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                    (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_MC_ID, &olp_mc, &olp_mc_destination));
    /*
     * Create a constant Field Group to redirect all packets ingressing from recyle port to destination port
     */
    action_priority = BCM_FIELD_ACTION_PRIORITY(0, 0);
    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageIngressPMF1, second_pass_context_id, action_priority,
                     bcmFieldActionRedirect, olp_mc_destination, "J1_Learn_2Pass_Rd"));

    action_priority = BCM_FIELD_ACTION_DONT_CARE;
    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageIngressPMF1, second_pass_context_id, action_priority,
                     bcmFieldActionTrap, 0x1e00, "J1_Learn_2Pass_Trap"));

    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageIngressPMF1, second_pass_context_id, action_priority,
                     bcmFieldActionIngressDoNotLearn, 0, "J1_Learn_2Pass_DisL"));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
appl_dnx_field_j1_learn_and_limit_egress_fg_create(
    int unit,
    bcm_field_context_t context_id)
{
    /*
     *  * Context "Learn_Limit ePMF"
     *   Context_Selection:
     *      port class 1 (assigned to OLP port)
     *   Qualifiers:
     *      out_port (==OLP port)
     *      DSP_payload_format
     *      DSP_destination_encoding
     *      DSP_CMD_Type
     *   Actions:
     *      out_port = change OLP port
     */

    bcm_field_entry_info_t entry_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t learn_foramt_qual_id, fwd_type_qual_id, cmd_type_qual_id;
    bcm_field_group_attach_info_t attach_info;
    bcm_gport_t epmf_gport;
    uint32 epmf_dest_port = OLP_PORT_0;
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_entry_t entry_id;
    void *dest_char;

    /*
     * The TCAM entries which are added here.
     * They are planned to handle all possible combinations of J1 compatible learning  limit .
     *
     * out-port  dest encoding    payload format   CMD type    action B-J1 action B-J2 action A0-J1
     * --------------------------------------------------------------------------------------------
     * OLP_0     2 (Flow-ID/FEC)  A (dest+outlif)   insert     OLP_3       OLP_0       OLP_1
     * OLP_0     2 (Flow-ID/FEC)  A (dest+outlif)   XXX        OLP_1       OLP_1       OLP_1
     * OLP_0     XXX              A (dest+outlif)   insert     OLP_0       OLP_0       OLP_0
     * OLP_0     XXX              A (dest+outlif)   XXX        OLP_2       OLP_1       OLP_0
     * OLP_0     XXX              B (EEI+FEC)       insert     OLP_3       OLP_1       OLP_1
     * OLP_0     XXX              B (EEI+FEC)       XXX        OLP_1       OLP_1       OLP_1
     */
    uint32 learn_bug_rule_action[] = { OLP_PORT_1, OLP_PORT_1, OLP_PORT_0, OLP_PORT_0, OLP_PORT_1, OLP_PORT_1 };
    uint32 limit_bug_rule_action[] = { OLP_PORT_0, OLP_PORT_1, OLP_PORT_0, OLP_PORT_1, OLP_PORT_1, OLP_PORT_1 };
    uint32 limit_and_learn_bug_rule_action[] =
        { OLP_PORT_3, OLP_PORT_1, OLP_PORT_0, OLP_PORT_2, OLP_PORT_3, OLP_PORT_1 };
    uint32 *rule_action = limit_bug_rule_action;
    uint32 cmd_type_insert = 0;
    int is_j1_mode;

    SHR_FUNC_INIT_VARS(unit);

    is_j1_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

    /*
     * Set TCAM entries actions array
     */
    if ((dnx_data_field.init.j1_learning_get(unit)) && (is_j1_mode) && (dnx_data_field.init.learn_limit_get(unit)))
    {
        rule_action = limit_and_learn_bug_rule_action;
        cmd_type_insert = 1;
    }
    else if ((dnx_data_field.init.j1_learning_get(unit)) && (is_j1_mode))
    {
        rule_action = learn_bug_rule_action;
        cmd_type_insert = 1;
    }
    else if (dnx_data_field.init.learn_limit_get(unit))
    {
        rule_action = limit_bug_rule_action;
    }
    /**
     * Create user defined qual for forwarding type in OLP payload (bits 18:16)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_eg_Learn_Fwd_Type", sizeof(qual_info.name));
    qual_info.size = 3;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &fwd_type_qual_id));

    /**
     * Create user defined qual for  Learning format in OLP payload (bits 39:40)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_eg_Learn_Format", sizeof(qual_info.name));
    qual_info.size = 2;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &learn_foramt_qual_id));

    /**
     * Create user defined qual for DSP CMD type in OLP payload (bits 177:175)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "DSP_CMD_Type", sizeof(qual_info.name));
    qual_info.size = 3;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cmd_type_qual_id));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    /*
     * Set quals
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bcmFieldQualifyOutPort;
    fg_info.qual_types[1] = fwd_type_qual_id;
    fg_info.qual_types[2] = learn_foramt_qual_id;
    fg_info.qual_types[3] = cmd_type_qual_id;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionOutPortTrafficManagement;
    fg_info.action_with_valid_bit[0] = TRUE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_limit_ePMF_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 2;
    attach_info.key_info.qual_info[1].offset = 232;     /* 112(ETH) + 120(destination encoding location in DSP) */
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 2;
    attach_info.key_info.qual_info[2].offset = 210;     /* 112(ETH) + 98 (payload format encoding location in DSP) */
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[3].input_arg = 2;
    attach_info.key_info.qual_info[3].offset = is_j1_mode ? 113 : 118;  /* 112(ETH) + 6(CMD type location in DSP) */

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    BCM_GPORT_LOCAL_SET(epmf_gport, epmf_dest_port);

    /*
     * Add the entries documented in the beginning of the function
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 0;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = epmf_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = 2;      /* Value of flow-id/FEC */
    entry_info.entry_qual[1].mask[0] = 0x3;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 0;      /* Value of format A */
    entry_info.entry_qual[2].mask[0] = 0x3;
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    entry_info.entry_qual[3].value[0] = cmd_type_insert;        /* Value of CMD type "insert" */
    entry_info.entry_qual[3].mask[0] = is_j1_mode ? 0x5 : 0x3;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = rule_action[0];

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = epmf_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = 2;      /* Value of flow-id/FEC */
    entry_info.entry_qual[1].mask[0] = 0x3;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 0;      /* Value of format A */
    entry_info.entry_qual[2].mask[0] = 0x3;
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    entry_info.entry_qual[3].value[0] = cmd_type_insert;        /* Value of CMD type "insert" */
    entry_info.entry_qual[3].mask[0] = 0;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = rule_action[1];

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 2;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = epmf_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = 2;      /* Value of flow-id/FEC */
    entry_info.entry_qual[1].mask[0] = 0;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 0;      /* Value of format A */
    entry_info.entry_qual[2].mask[0] = 0x3;
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    entry_info.entry_qual[3].value[0] = cmd_type_insert;        /* Value of CMD type "insert" */
    entry_info.entry_qual[3].mask[0] = is_j1_mode ? 0x5 : 0x3;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = rule_action[2];

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 3;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = epmf_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = 2;      /* Value of flow-id/FEC */
    entry_info.entry_qual[1].mask[0] = 0;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 0;      /* Value of format A */
    entry_info.entry_qual[2].mask[0] = 0x3;
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    entry_info.entry_qual[3].value[0] = cmd_type_insert;        /* Value of CMD type "insert" */
    entry_info.entry_qual[3].mask[0] = 0;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = rule_action[3];

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 4;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = epmf_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = 2;      /* Value of flow-id/FEC */
    entry_info.entry_qual[1].mask[0] = 0;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 2;      /* Value of format B */
    entry_info.entry_qual[2].mask[0] = 2;
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    entry_info.entry_qual[3].value[0] = cmd_type_insert;        /* Value of CMD type "insert" */
    entry_info.entry_qual[3].mask[0] = is_j1_mode ? 0x5 : 0x3;;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = rule_action[4];

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 4;
    entry_info.nof_entry_actions = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = epmf_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = 2;      /* Value of flow-id/FEC */
    entry_info.entry_qual[1].mask[0] = 0;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 0;      /* Ignore Format */
    entry_info.entry_qual[2].mask[0] = 0;
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    entry_info.entry_qual[3].value[0] = cmd_type_insert;        /* Value of CMD type "insert" */
    entry_info.entry_qual[3].mask[0] = 0;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = rule_action[5];

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*
* ERPP Ethernet filters are executed, packet may be trapped
* Fix traps in PMF
*       Actions:
*           Forward-Action-Profile = NORMAL (0) => revert ERPP filters
*           ACE_Context_Value[1:0] = J_Mode_FWD_Code_Override (via ACE-Pointer) => ensure correct context selection in ETPP
*    ETPP Fwd-Code Selection
*       (ACE_Context_Value[1:0] == J_Mode_FWD_Code_Override) --> FWD-Context = TM
* \param [in] unit        - Device id
* \param [in] context_id    context to configure
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
appl_dnx_field_j1_same_intf_egress(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure the ACE format.
     */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "J1_SAME_P_ACE", sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id));

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextForwardingCodeOverride;

    SHR_IF_ERR_EXIT(bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle));

    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageEgress, context_id, BCM_FIELD_ACTION_DONT_CARE, bcmFieldActionAceEntryId,
                     ace_entry_handle, "J1_ePMF_CNST_ACE"));
    SHR_IF_ERR_EXIT(appl_dnx_field_group_const_util
                    (unit, bcmFieldStageEgress, context_id, BCM_FIELD_ACTION_DONT_CARE, bcmFieldActionTrap, 0,
                     "J1_ePMF_CNST_TRAP"));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Create egress PMF context and context selection.
* Call required functions to further configure the context
* \param [in] unit        - Device id
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
appl_dnx_field_j1_learn_and_limit_egress(
    int unit)
{
    void *dest_char;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    uint32 olp_port_class_id = 1;
    bcm_field_context_info_t context_info;
    bcm_gport_t olp_port;

    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_LOCAL_SET(olp_port, OLP_PORT_0);
    /*
     * create a direct Extraction Field Group in ePMF
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Learn_and_Limit_ePMF_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id));

    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, olp_port, bcmPortClassFieldEgressPacketProcessingPortCs, olp_port_class_id));

    /***
     * create the context selection based on OLP port profile
 */
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dnx_data_field.preselector.default_learn_limit_presel_id_epmf_get(unit);;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = olp_port_class_id;
    p_data.qual_data[0].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    SHR_IF_ERR_EXIT(appl_dnx_field_j1_learn_and_limit_egress_fg_create(unit, context_id));

    if ((dnx_data_field.init.j1_learning_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_j1_same_intf_egress(unit, context_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Create egress PMF context and context selection.
* Call required functions to further configure the context
* \param [in] unit        - Device id
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
appl_dnx_field_j1_learn_nullify_key_msbs_egress(
    int unit)
{
    void *dest_char;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    uint32 olp_port_class_id = 1;
    bcm_field_context_info_t context_info;
    bcm_gport_t olp_port;

    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_LOCAL_SET(olp_port, OLP_PORT_0);
    /*
     * create a direct Extraction Field Group in ePMF
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Learn_Nullify_key_ePMF_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id));

    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, olp_port, bcmPortClassFieldEgressPacketProcessingPortCs, olp_port_class_id));

    /***
     * create the context selection based on OLP port profile
 */
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dnx_data_field.preselector.default_learn_limit_presel_id_epmf_get(unit);;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = olp_port_class_id;
    p_data.qual_data[0].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    SHR_IF_ERR_EXIT(appl_dnx_field_j1_same_intf_egress(unit, context_id));

exit:
    SHR_FUNC_EXIT;
}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_learn_and_limit_init(
    int unit)
{
    bcm_port_config_t port_config;
    int num_of_ports;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the OLP port number */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /** Make sure that OLP port was configured. */
    BCM_PBMP_COUNT(port_config.olp, num_of_ports);
    if (num_of_ports == 0)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "OLP port should be configured in the device for the field %s%s%s.\n", EMPTY,
                          EMPTY, EMPTY);
    }

    if ((dnx_data_field.init.j1_learning_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        if (!BCM_PBMP_MEMBER(port_config.rcy, LEARN_RCY_PORT1))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_NONE,
                              "RCY port should be configured in the device for the field in J1 mode%s%s%s.\n", EMPTY,
                              EMPTY, EMPTY);
        }
        SHR_IF_ERR_EXIT(appl_dnx_field_j1_learn_ingress(unit));
    }
    SHR_IF_ERR_EXIT(appl_dnx_field_j1_learn_and_limit_egress(unit));

exit:
    SHR_FUNC_EXIT;
}

/** see appl_ref_field_init.h*/
shr_error_e
appl_dnx_field_learn_nullify_key_msbs_init(
    int unit)
{
    bcm_port_config_t port_config;
    int num_of_ports;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the OLP port number */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /** Make sure that OLP port was configured. */
    BCM_PBMP_COUNT(port_config.olp, num_of_ports);
    if (num_of_ports == 0)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "OLP port should be configured in the device for the field%s%s%s.\n", EMPTY,
                          EMPTY, EMPTY);
    }

    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        if (!BCM_PBMP_MEMBER(port_config.rcy, LEARN_RCY_PORT1))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_NONE,
                              "RCY port should be configured in the device for the field in J1 mode%s%s%s.\n", EMPTY,
                              EMPTY, EMPTY);
        }
        SHR_IF_ERR_EXIT(appl_dnx_field_j1_learn_nullify_key_msbs_ingress(unit));
    }
    SHR_IF_ERR_EXIT(appl_dnx_field_j1_learn_nullify_key_msbs_egress(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
