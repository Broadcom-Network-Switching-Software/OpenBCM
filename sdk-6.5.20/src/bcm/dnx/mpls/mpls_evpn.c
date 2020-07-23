/** \file mpls_evpn.c MPLS based EVPN functionality for DNX.
 * This file contains verification and translation between the MPLS
 * APIs and the mpls_l2vpn module. The mpls_l2vpn module contains
 * the implementation of the EVPN application architecture.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

#include <bcm/types.h>
#include <bcm/mpls.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <bcm_int/dnx/mpls/mpls_range.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>

/* ******************************************************************
 *
 *                              INGRESS
 *
 * ******************************************************************
 * {
 */

/**
 * \brief BCM_MPLS_SWITCH_* to BCM_MPLS_PORT_* mapping for L2VPN module usage (for EVPN)
 */
static const uint32 switch_to_port_flag_pairs[][2] = {
    {BCM_MPLS_SWITCH_WITH_ID, BCM_MPLS_PORT_WITH_ID},
    {BCM_MPLS_SWITCH_REPLACE, BCM_MPLS_PORT_REPLACE},
    {BCM_MPLS_SWITCH_ENTROPY_ENABLE, BCM_MPLS_PORT_ENTROPY_ENABLE}
};

/**
 * \brief BCM_MPLS_SWITCH2_* to BCM_MPLS_PORT_* mapping for L2VPN module usage (for EVPN)
 */
static const uint32 switch2_to_port_flag_pairs[][2] = {
    {BCM_MPLS_SWITCH2_CONTROL_WORD, BCM_MPLS_PORT_CONTROL_WORD}
};

/**
 * \brief BCM_MPLS_SWITCH2_* to BCM_MPLS_PORT2_* mapping for L2VPN module usage (for EVPN)
 */
static const uint32 switch2_to_port2_flag_pairs[][2] = {
    {BCM_MPLS_SWITCH2_STAT_ENABLE, BCM_MPLS_PORT2_STAT_ENABLE},
    {BCM_MPLS_SWITCH2_CROSS_CONNECT, BCM_MPLS_PORT2_CROSS_CONNECT}
};

/**
 * \brief Translate the BCM_MPLS_SWITCH_xxx flags to
 * BCM_MPLS_PORT_xx flags for L2VPN module usage (for EVPN)
 */
static shr_error_e
dnx_mpls_tunnel_term_flags_to_mpls_port_translate(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    int pair_i;
    SHR_FUNC_INIT_VARS(unit);

    for (pair_i = 0; pair_i < sizeof(switch_to_port_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        l2vpn->mpls_port->flags |=
            _SHR_IS_FLAG_SET(info->flags,
                             switch_to_port_flag_pairs[pair_i][0]) ? switch_to_port_flag_pairs[pair_i][1] : 0;
    }
    for (pair_i = 0; pair_i < sizeof(switch2_to_port_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        l2vpn->mpls_port->flags |=
            _SHR_IS_FLAG_SET(info->flags2,
                             switch2_to_port_flag_pairs[pair_i][0]) ? switch2_to_port_flag_pairs[pair_i][1] : 0;
    }
    for (pair_i = 0; pair_i < sizeof(switch2_to_port2_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        l2vpn->mpls_port->flags2 |=
            _SHR_IS_FLAG_SET(info->flags2,
                             switch2_to_port2_flag_pairs[pair_i][0]) ? switch2_to_port2_flag_pairs[pair_i][1] : 0;
    }

    /*
     * Must add
     */
    l2vpn->mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    l2vpn->mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Translate the BCM_MPLS_PORT_xxx flags to
 * BCM_MPLS_SWITCH_xx flags for L2VPN module usage (for EVPN)
 */
static shr_error_e
dnx_mpls_port_flags_to_tunnel_term_translate(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn,
    bcm_mpls_tunnel_switch_t * info)
{
    int pair_i;
    SHR_FUNC_INIT_VARS(unit);

    for (pair_i = 0; pair_i < sizeof(switch_to_port_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        info->flags |=
            _SHR_IS_FLAG_SET(l2vpn->mpls_port->flags,
                             switch_to_port_flag_pairs[pair_i][1]) ? switch_to_port_flag_pairs[pair_i][0] : 0;
    }
    for (pair_i = 0; pair_i < sizeof(switch2_to_port_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        info->flags2 |=
            _SHR_IS_FLAG_SET(l2vpn->mpls_port->flags,
                             switch2_to_port_flag_pairs[pair_i][1]) ? switch2_to_port_flag_pairs[pair_i][0] : 0;
    }
    for (pair_i = 0; pair_i < sizeof(switch2_to_port2_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        info->flags2 |=
            _SHR_IS_FLAG_SET(l2vpn->mpls_port->flags2,
                             switch2_to_port2_flag_pairs[pair_i][1]) ? switch2_to_port2_flag_pairs[pair_i][0] : 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Translates an MPLS tunnel termination object to an
 * MPLS L2VPN object (for EVPN).
 */
static shr_error_e
dnx_mpls_tunnel_switch_to_l2vpn(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    bcm_mpls_port_t *pmport;
    SHR_FUNC_INIT_VARS(unit);

    l2vpn->vpn = info->vpn;
    /*
     * Populate mpls_port
     */
    pmport = l2vpn->mpls_port;
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_term_flags_to_mpls_port_translate(unit, info, l2vpn));

    pmport->mpls_port_id = info->tunnel_id;
    pmport->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pmport->match_label = info->label;
    pmport->qos_map_id = info->qos_map_id;
    pmport->ingress_failover_id = info->failover_id;
    pmport->ingress_failover_port_id = info->failover_tunnel_id;
    /*
     * Check if IML+ESI is needed
     */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EVPN_IML))
    {
        if (dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_mpls_term_1_or_2_labels))
        {
            l2vpn->flags |= DNX_MPLS_L2VPN_FLAG_INGRESS_IML;
        }
        else if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EXPECT_BOS))
        {
            l2vpn->flags |= DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI;
        }
        else
        {
            l2vpn->flags |= DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI;
        }
    }
    if ((info->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid) ||
        (info->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid) ||
        (info->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid))
    {
        pmport->ingress_qos_model.ingress_phb = info->ingress_qos_model.ingress_phb;
        pmport->ingress_qos_model.ingress_remark = info->ingress_qos_model.ingress_remark;
        pmport->ingress_qos_model.ingress_ttl = info->ingress_qos_model.ingress_ttl;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Translate the output of an MPLS L2VPN call to MPLS tunnel
 * termination structure to return to MPLS API (for EVPN).
 */
static shr_error_e
dnx_mpls_l2vpn_to_tunnel_switch(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn,
    bcm_mpls_tunnel_switch_t * info)
{
    bcm_mpls_port_t *pmport;
    SHR_FUNC_INIT_VARS(unit);

    info->vpn = l2vpn->vpn;
    SHR_IF_ERR_EXIT(dnx_mpls_port_flags_to_tunnel_term_translate(unit, l2vpn, info));
    /*
     * Translate mpls_port back to l2vpn
     */
    pmport = l2vpn->mpls_port;
    info->tunnel_id = pmport->mpls_port_id;
    info->label = pmport->match_label;
    info->qos_map_id = pmport->qos_map_id;
    info->failover_id = pmport->ingress_failover_id;
    info->failover_tunnel_id = pmport->ingress_failover_port_id;
    /*
     * Restore EVPN/IML specific flags (For traverse)
     */
    if (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI))
    {
        info->flags |= (BCM_MPLS_SWITCH_EVPN_IML | BCM_MPLS_SWITCH_EXPECT_BOS);
    }
    else if (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI))
    {
        info->flags |= (BCM_MPLS_SWITCH_EVPN_IML);
    }
    else if (_SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_INGRESS_IML))
    {
        info->flags |= (BCM_MPLS_SWITCH_EVPN_IML);
    }
    else
    {
        info->flags |= (BCM_MPLS_SWITCH_NEXT_HEADER_L2);
    }
exit:
    SHR_FUNC_EXIT;
}

static uint32
dnx_mpls_evpn_term_supported_flags_get(
    int unit)
{
    int pair_i;
    uint32 allowed_flags;

    /** EVPN signifying flags */
    allowed_flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2 | BCM_MPLS_SWITCH_EVPN_IML;

    /** ESI indication */
    allowed_flags |= BCM_MPLS_SWITCH_EXPECT_BOS;

    /** Add all the flags that are translated to MPLS-Port flags */
    for (pair_i = 0; pair_i < sizeof(switch_to_port_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        allowed_flags |= switch_to_port_flag_pairs[pair_i][0];
    }

    return allowed_flags;
}

static uint32
dnx_mpls_evpn_term_supported_flags2_get(
    int unit)
{
    uint32 allowed_flags2 = 0;
    int pair_i;

    /** Translated flags */
    for (pair_i = 0; pair_i < sizeof(switch2_to_port_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        allowed_flags2 |= switch2_to_port_flag_pairs[pair_i][0];
    }
    for (pair_i = 0; pair_i < sizeof(switch2_to_port2_flag_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        allowed_flags2 |= switch2_to_port2_flag_pairs[pair_i][0];
    }

    return allowed_flags2;
}

/**
 * \brief Verify that only flags that are handled in EVPN or signify that the object IS evpn are set
 */
static shr_error_e
dnx_mpls_evpn_term_flags_verify(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    uint32 allowed_flags;
    uint32 allowed_flags2;

    SHR_FUNC_INIT_VARS(unit);

    allowed_flags = dnx_mpls_evpn_term_supported_flags_get(unit);
    if (info->flags & (~allowed_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flag(0x%X) was used for EVPN/IML creation, available flags are 0x%X.\n",
                     info->flags, allowed_flags);
    }

    allowed_flags2 = dnx_mpls_evpn_term_supported_flags2_get(unit);
    if (info->flags2 & (~allowed_flags2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flag2(0x%X) for EVPN/IML creation, available flags are 0x%X.\n",
                     info->flags2, allowed_flags2);
    }

    /** Some flags are not allowed in special cases*/
    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EVPN_IML))
    {
        if (dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_mpls_term_1_or_2_labels) &&
            _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EXPECT_BOS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_SWITCH_EXPECT_BOS is not needed for IML in current device.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Verify content of the tunnel_switch object for
 * EVPN/IML termination creation
 */
static shr_error_e
dnx_mpls_evpn_term_add_verify(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mpls_evpn_term_flags_verify(unit, info));

    if (info->vpn)
    {
        /*
         * P2P may have vpn==0. Allow that case.
         * Otherwise, VPN must be created.
         */
        uint8 is_vsi_alloc;
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, info->vpn, &is_vsi_alloc));
        if (!is_vsi_alloc)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VPN %d doesn't exist\n", info->vpn);
        }
    }
    if (info->tunnel_if != BCM_IF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tunnel_if parameter cannot be used. Please use bcm_l3_ingress_create instead with intf_id as mpls tunnel.");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_term_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    bcm_mpls_port_t mpls_port;
    dnx_mpls_l2vpn_info_t l2vpn_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_evpn_term_add_verify(unit, info));

    bcm_mpls_port_t_init(&mpls_port);
    dnx_mpls_l2vpn_info_t_init(unit, &l2vpn_info, MPLS_L2VPN_TYPE_EVPN, &mpls_port);
    /*
     * EVPN handling - Use MPLS L2VPN Module.
     */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_to_l2vpn(unit, info, &l2vpn_info));
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_add(unit, &l2vpn_info));
    /*
     * Copy output parameters like allocated resources and IDs back
     * to the MPLS tunnel termination object
     */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_to_tunnel_switch(unit, &l2vpn_info, info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_term_get(
    int unit,
    int local_in_lif,
    bcm_mpls_tunnel_switch_t * info)
{
    bcm_mpls_port_t mpls_port;
    dnx_mpls_l2vpn_info_t l2vpn_info;
    bcm_gport_t gport = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    dnx_mpls_l2vpn_info_t_init(unit, &l2vpn_info, MPLS_L2VPN_TYPE_EVPN, &mpls_port);
    l2vpn_info.vpn = info->vpn;
    /*
     * Get gport from gport mgmt's local_lif->gport
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS,
                                                _SHR_CORE_ALL, local_in_lif, &gport));

    l2vpn_info.mpls_port->mpls_port_id = gport;
    l2vpn_info.mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;

    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_get(unit, &l2vpn_info));
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_to_tunnel_switch(unit, &l2vpn_info, info));

    /*
     * this is done for the _traverse API as all other APIs expect correct 'action' to be provided
     */
    info->action = BCM_MPLS_SWITCH_ACTION_POP;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_term_delete(
    int unit,
    int local_in_lif)
{
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get gport from gport mgmt's local_lif->gport
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS,
                                                _SHR_CORE_ALL, local_in_lif, &gport));

    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_delete(unit, MPLS_L2VPN_TYPE_EVPN, TRUE, gport));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_local_in_lif_is_evpn(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    int *local_in_lif,
    uint8 *is_evpn)
{
    dbal_tables_e dbal_table_id;
    uint32 dbal_result_type;
    SHR_FUNC_INIT_VARS(unit);
    if (info != NULL)
    {
        dnx_mpls_term_match_t match;
        uint8 is_iml = _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EVPN_IML);
        /*
         *  Get entry from ISEM to get local lif id from label
         */
        if (is_iml && !dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_mpls_term_1_or_2_labels))
        {
            /*
             * Label is in range of IML labels. Get it from the dedicated DB.
             */
            match.match_criteria = TERM_MATCH_CRITERIA_LABEL_BOS;
            match.label = info->label;
            match.bos_val = _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EXPECT_BOS) ? 1 : 0;
        }
        else
        {
            /*
             * Non-IML label or feature mpls_term_1_or_2_labels is supported. Can still be EVPN.
             */
            SHR_IF_ERR_EXIT(dnx_mpls_term_match_criteria_from_tunnel_switch_get(unit, info, &match));
        }
        SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_GET, &match, local_in_lif));
    }
    /*
     * LSP, PWE and EVPN INLIF always use INLIF-2.
     */
    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                    (unit, *local_in_lif, dnx_drv_soc_core_default(), DBAL_PHYSICAL_TABLE_INLIF_2, &dbal_table_id,
                     &dbal_result_type, NULL));

    *is_evpn = (dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_EVPN);

exit:
    SHR_FUNC_EXIT;
}

/*
 * } ( INGRESS )
 * ******************************************************************
 *
 *                              EGRESS
 *
 * ******************************************************************
 * {
 */

static const uint32 egress_label_mpls_port_flags_pairs[][2] = {
    {BCM_MPLS_EGRESS_LABEL_WITH_ID, BCM_MPLS_PORT_WITH_ID},
    {BCM_MPLS_EGRESS_LABEL_REPLACE, BCM_MPLS_PORT_REPLACE},
    {BCM_MPLS_EGRESS_LABEL_CONTROL_WORD, BCM_MPLS_PORT_CONTROL_WORD}
};

static const uint32 egress_label_mpls_port_flags2_pairs[][2] = {
    {BCM_MPLS_EGRESS_LABEL_PROTECTION, BCM_MPLS_PORT2_EGRESS_PROTECTION}
};

/**
 * \brief Translate the BCM_MPLS_EGRESS_LABEL_xxx flags to
 * BCM_MPLS_PORT_xx flags for L2VPN module usage (for EVPN)
 */
static shr_error_e
dnx_mpls_egress_label_flags_to_mpls_port_translate(
    int unit,
    bcm_mpls_egress_label_t * eg_label,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    int pair_i;
    SHR_FUNC_INIT_VARS(unit);

    for (pair_i = 0; pair_i < sizeof(egress_label_mpls_port_flags_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        l2vpn->mpls_port->flags |=
            _SHR_IS_FLAG_SET(eg_label->flags, egress_label_mpls_port_flags_pairs[pair_i][0]) ?
            egress_label_mpls_port_flags_pairs[pair_i][1] : 0;
    }

    for (pair_i = 0; pair_i < sizeof(egress_label_mpls_port_flags2_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        l2vpn->mpls_port->flags2 |=
            _SHR_IS_FLAG_SET(eg_label->flags, egress_label_mpls_port_flags2_pairs[pair_i][0]) ?
            egress_label_mpls_port_flags2_pairs[pair_i][1] : 0;
    }
    /*
     * Copy the rest of the egress label flags as is
     */
    l2vpn->mpls_port->egress_label.flags = eg_label->flags;
    /*
     * Must add
     */
    l2vpn->mpls_port->flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Translate the BCM_MPLS_PORT_xx flags to
 * BCM_MPLS_EGRESS_LABEL_xxx flags for L2VPN module usage (for EVPN)
 */
static shr_error_e
dnx_mpls_port_flags_to_egress_label_translate(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn,
    bcm_mpls_egress_label_t * eg_label)
{
    int pair_i;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First copy the mpls_port->egress_label flags
     */
    eg_label->flags = l2vpn->mpls_port->egress_label.flags;
    /*
     * Now add translated flags if necessary.
     */
    for (pair_i = 0; pair_i < sizeof(egress_label_mpls_port_flags_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        eg_label->flags |=
            _SHR_IS_FLAG_SET(l2vpn->mpls_port->flags, egress_label_mpls_port_flags_pairs[pair_i][1]) ?
            egress_label_mpls_port_flags_pairs[pair_i][0] : 0;
    }
    for (pair_i = 0; pair_i < sizeof(egress_label_mpls_port_flags2_pairs) / (2 * sizeof(uint32)); ++pair_i)
    {
        eg_label->flags |=
            _SHR_IS_FLAG_SET(l2vpn->mpls_port->flags2, egress_label_mpls_port_flags2_pairs[pair_i][1]) ?
            egress_label_mpls_port_flags2_pairs[pair_i][0] : 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Translate tunnel initiator egress label information
 * to MPLS L2VPN object (for EVPN).
 */
static shr_error_e
dnx_mpls_tunnel_initiator_to_l2vpn(
    int unit,
    bcm_mpls_egress_label_t * eg_label,
    dnx_mpls_l2vpn_info_t * l2vpn)
{
    bcm_mpls_port_t *pmport;
    SHR_FUNC_INIT_VARS(unit);

    pmport = l2vpn->mpls_port;
    /*
     * Flags
     */
    SHR_IF_ERR_EXIT(dnx_mpls_egress_label_flags_to_mpls_port_translate(unit, eg_label, l2vpn));
    /*
     * ID
     */
    if (_SHR_IS_FLAG_SET(eg_label->flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
    {
        pmport->encap_id = BCM_L3_ITF_VAL_GET(eg_label->tunnel_id);
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(pmport->mpls_port_id, eg_label->tunnel_id);
    }
    /*
     * rest of the fields
     */
    pmport->egress_label.label = eg_label->label;
    pmport->egress_label.flags = eg_label->flags;
    pmport->egress_label.encap_access = eg_label->encap_access;
    pmport->egress_label.qos_map_id = eg_label->qos_map_id;
    pmport->egress_label.exp = eg_label->exp;
    pmport->egress_label.ttl = eg_label->ttl;
    pmport->egress_label.egress_qos_model.egress_ecn = eg_label->egress_qos_model.egress_ecn;
    pmport->egress_label.egress_qos_model.egress_ttl = eg_label->egress_qos_model.egress_ttl;
    pmport->egress_label.egress_qos_model.egress_qos = eg_label->egress_qos_model.egress_qos;
    pmport->egress_tunnel_if = eg_label->l3_intf_id;
    pmport->egress_failover_id = eg_label->egress_failover_id;
    pmport->egress_failover_port_id = eg_label->egress_failover_if_id;

    /*
     * Check if the egress EVPN label should be IML
     */
    if (_SHR_IS_FLAG_SET(eg_label->flags, BCM_MPLS_EGRESS_LABEL_IML))
    {
        l2vpn->flags |= DNX_MPLS_L2VPN_FLAG_EGRESS_IML;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Translate the output of an MPLS L2VPN call to MPLS
 * egress label structure to return to MPLS API (for EVPN).
 */
static shr_error_e
dnx_mpls_l2vpn_to_tunnel_initiator(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn,
    bcm_mpls_egress_label_t * eg_label)
{
    bcm_mpls_port_t *pmport;
    SHR_FUNC_INIT_VARS(unit);

    pmport = l2vpn->mpls_port;
    /*
     * Flags
     */
    SHR_IF_ERR_EXIT(dnx_mpls_port_flags_to_egress_label_translate(unit, l2vpn, eg_label));
    /*
     * ID
     */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(eg_label->tunnel_id, l2vpn->mpls_port->mpls_port_id);
    /*
     * Rest of the fields
     */
    eg_label->label = pmport->egress_label.label;
    eg_label->flags |= pmport->egress_label.flags;
    eg_label->encap_access = pmport->egress_label.encap_access;
    eg_label->qos_map_id = pmport->egress_label.qos_map_id;
    eg_label->exp = pmport->egress_label.exp;
    eg_label->ttl = pmport->egress_label.ttl;
    eg_label->egress_qos_model.egress_ecn = pmport->egress_label.egress_qos_model.egress_ecn;
    eg_label->egress_qos_model.egress_qos = pmport->egress_label.egress_qos_model.egress_qos;
    eg_label->egress_qos_model.egress_ttl = pmport->egress_label.egress_qos_model.egress_ttl;
    eg_label->l3_intf_id = pmport->egress_tunnel_if;
    eg_label->egress_failover_id = pmport->egress_failover_id;
    eg_label->egress_failover_if_id = pmport->egress_failover_port_id;

    /*
     * Check if this is EVPN label or IML
     */
    eg_label->flags |= _SHR_IS_FLAG_SET(l2vpn->flags, DNX_MPLS_L2VPN_FLAG_EGRESS_IML) ?
        BCM_MPLS_EGRESS_LABEL_IML : BCM_MPLS_EGRESS_LABEL_EVPN;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Verifies egress label object for EVPN/IML encapsulation
 * creation.
 */
static shr_error_e
dnx_mpls_evpn_encap_add_verify(
    int unit,
    bcm_mpls_egress_label_t * eg_label)
{
    uint32 allowed_flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check allowed flags
     */
    allowed_flags =
        BCM_MPLS_EGRESS_LABEL_IML |
        BCM_MPLS_EGRESS_LABEL_EVPN |
        BCM_MPLS_EGRESS_LABEL_WITH_ID |
        BCM_MPLS_EGRESS_LABEL_REPLACE |
        BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT |
        BCM_MPLS_EGRESS_LABEL_PROTECTION |
        BCM_MPLS_EGRESS_LABEL_STAT_ENABLE | BCM_MPLS_EGRESS_LABEL_CONTROL_WORD | BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    if (_SHR_IS_FLAG_SET(eg_label->flags, ~allowed_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flag set for EVPN/IML"
                     " encapsulation. Flags: 0x%08x\n, BCM_MPLS_EGRESS_LABEL_EXP/TTL_SET/COPY instead by egress_qos_model",
                     eg_label->flags);
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_encap_add(
    int unit,
    bcm_mpls_egress_label_t * eg_label)
{
    bcm_mpls_port_t mpls_port;
    dnx_mpls_l2vpn_info_t l2vpn_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_evpn_encap_add_verify(unit, eg_label));

    bcm_mpls_port_t_init(&mpls_port);
    dnx_mpls_l2vpn_info_t_init(unit, &l2vpn_info, MPLS_L2VPN_TYPE_EVPN, &mpls_port);
    /*
     * EVPN special handling - use MPLS-L2VPN module.
     */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_to_l2vpn(unit, eg_label, &l2vpn_info));
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_add(unit, &l2vpn_info));
    /*
     * Copy output parameters like allocated resources and IDs back
     * to the MPLS tunnel initiator object
     */
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_to_tunnel_initiator(unit, &l2vpn_info, eg_label));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_encap_get(
    int unit,
    bcm_gport_t gport,
    bcm_mpls_egress_label_t * eg_label,
    int *label_count)
{
    bcm_mpls_port_t mpls_port;
    dnx_mpls_l2vpn_info_t l2vpn;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    dnx_mpls_l2vpn_info_t_init(unit, &l2vpn, MPLS_L2VPN_TYPE_EVPN, &mpls_port);
    l2vpn.mpls_port->mpls_port_id = gport;
    l2vpn.mpls_port->flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_get(unit, &l2vpn));
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_to_tunnel_initiator(unit, &l2vpn, eg_label));
    /*
     * EVPN/IML is always a single egress label.
     */
    *label_count = 1;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_egress_label_is_evpn(
    int unit,
    bcm_if_t intf,
    bcm_gport_t * gport,
    uint8 *is_evpn)
{
    uint32 global_lif;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Initialize '*is_evpn' to FALSE and leave it this way unless encoding is such that
     * 'intf' contains a valid 'global_lif'
     */
    *is_evpn = FALSE;
    if (BCM_L3_ITF_TYPE_IS_LIF(intf))
    {
        if (_SHR_L3_ITF_SUB_TYPE_IS_NOT_ENCODED(intf))
        {
            /*
             * At this time, the only subtype allowed here is 'default' (no subtype encoding)
             */
            global_lif = BCM_L3_ITF_VAL_GET(intf);
            /*
             * Get GPort
             */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, global_lif,
                             gport));
            /*
             * get HW resources
             */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, *gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
            /*
             * Table unique to EVPN
             */
            *is_evpn = (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_EVPN);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See mpls_evpn.h
 */
shr_error_e
dnx_mpls_evpn_encap_delete(
    int unit,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_delete(unit, MPLS_L2VPN_TYPE_EVPN, FALSE, gport));

exit:
    SHR_FUNC_EXIT;
}

/*
 * } ( EGRESS )
 */
