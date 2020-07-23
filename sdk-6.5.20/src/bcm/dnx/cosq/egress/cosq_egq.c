/**
 * \file cosq_egq.c
 * 
 *
 * cosq API functions, which are purely EGQ related, for DNX. \n
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/cosq/egress/egq_dbal.h>
#include <bcm_int/dnx/cosq/egress/cosq_egq.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/egq/egq_alloc_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>

/*
 * Port with the following number of priorities supports TCG
 */
#define DNX_TCG_NOF_PRIORITIES_SUPPORT           (8)
#define DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT      (DNX_TCG_NOF_PRIORITIES_SUPPORT)

#define DNX_COSQ_EGR_OFP_SCH_SP_LOW              (BCM_COSQ_SP1)
#define DNX_COSQ_EGR_OFP_SCH_SP_HIGH             (BCM_COSQ_SP0)

/*
 * credit egress dicsount limitations
 */
#define DNX_COSQ_EGRESS_CR_DISCOUNT_MIN_VAL      (-32)
#define DNX_COSQ_EGRESS_CR_DISCOUNT_MAX_VAL      (32)

/*
 * Maximal value of weight that can be assigned to a queue when WFQ (weighted
 * Fair Queuing) is applied.
 */
#define DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX      (255)

/*
 * COSQ max num for PB
 */
#define DNX_COSQ_PB_EGR_OFP_COSQ_NUM_MAX(unit)   (BCM_COS_MAX)
/*
 * Static (local) procedures.
 * {
 */
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_scheduler_queue_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    bcm_port_t logical_port;
    int core;
    int is_high_priority;
    dnx_egr_q_prio_e prio;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));

    SHR_IF_ERR_EXIT(dnx_egr_queuing_is_high_priority_queue_get(unit, core, logical_port, cosq, &is_high_priority));
    if (is_high_priority)
    {
        *mode = BCM_COSQ_SP0;
    }
    else
    {
        *mode = BCM_COSQ_SP1;
    }
    *weight = -1;
exit:
    SHR_FUNC_EXIT;
}
/* 
 * \brief -
 *   Map egress queue to a specific priority ('offset' from base qpair)
 *     Given a gport plus priority (TC) and Drop-precedence (DP) of unicast traffic
 *     entering egress, set the priority for outgoing traffic (by matching it to
 *     a specific port, related to egress queue).
 *     Note that DP of outgoing traffic is not touched.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] gport -
 *   bcm_gport_t. Identifier of traffic by its assigned gport.
 * \param [in] ingress_pri -
 *   bcm_cos_t. Priority (TC) of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_TC-1 (7).
 * \param [in] ingress_dp -
 *   bcm_color_t. Drop precedence of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_DP-1 (3).
 * \param [in] offset -
 *   bcm_cos_queue_t. Priority to assign to outgoing traffic. Must be between 0 and
 *   DNX_EGR_NOF_Q_PRIO-1 (7).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_cosq_gport_egress_map_set
 *   * dnx_egr_queuing_profile_mapping_set
 */
static shr_error_e
dnx_cosq_egq_gport_egress_queue_map_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t offset)
{
    int is_last, is_allocated;
    int old_profile, new_profile;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    dnx_egr_q_prio_mapping_type_e map_type;
    dnx_egr_q_prio_e tc;
    bcm_port_t logical_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
        gport = logical_port;
    }
    /** get the pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));
    /** Validate input params */
    if (ingress_pri < 0 || ingress_pri >= DNX_COSQ_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d. Legal range is %d,%d",
                     ingress_pri, 0, (DNX_COSQ_NOF_TC - 1));
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_COSQ_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d. Legal range is %d,%d",
                     ingress_dp, 0, (DNX_COSQ_NOF_DP - 1));
    }
    if (offset < 0 || offset >= DNX_EGR_NOF_Q_PRIO)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset parameter %d. Legal range is %d,%d",
                     offset, 0, (DNX_EGR_NOF_Q_PRIO - 1));
    }

    for (int port_idx = 0; port_idx < gport_info.internal_port_pp_info.nof_pp_ports; port_idx++)
    {
        /** Get Old data */
        SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get
                        (unit, gport_info.internal_port_pp_info.pp_port[port_idx],
                         gport_info.internal_port_pp_info.core_id[port_idx], &mapping_info));
        /** Change specific entry - preserve dp, change new tc (offset) */
        /** Unicast type to scheduled */
        map_type = DNX_EGR_UCAST_TO_SCHED;
        SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, offset, &tc));
        mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc = tc;

        /** Exchange data  */
        SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange
                        (unit, gport_info.internal_port_pp_info.pp_port[port_idx],
                         gport_info.internal_port_pp_info.core_id[port_idx],
                         &mapping_info, &old_profile, &is_last, &new_profile, &is_allocated));

        /** If this is a new profile, different from the old one, then load into HW. */
        if (is_allocated)
        {
            /** Create new profile in HW */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_profile_mapping_set
                            (unit, gport_info.internal_port_pp_info.core_id[port_idx], new_profile, &mapping_info));
        }

        /** Map port to new profile */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_profile_map_set
                        (unit, gport_info.internal_port_pp_info.core_id[port_idx],
                         gport_info.internal_port_pp_info.pp_port[port_idx], new_profile));
    }

exit:
    SHR_FUNC_EXIT;
}
/* 
 * \brief -
 *   Given 'logical port' and BCM priority ('cosq'), get the corresponding 'TC group'
 *   identifier.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] logical_port -
 *   bcm_port_t. Identifier of logical port to use to get 'base_q_pair'.
 * \param [in] cosq -
 *   int. 'Priority' to use, within 'base_q_pair' to identify the required TCG.
 *   Must be within DNX_TCG_MIN and DNX_TCG_MAX.
 * \param [in] tcg_p -
 *   dnx_tcg_ndx_t. This procedure loads pointed memory by the identifier of
 *   the TCG as deduced from 'logical port' and 'cosq'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_egq_gport_egress_port_tcg_sched_set
 */
static shr_error_e
dnx_cosq_egq_bcm_cosq_to_tcg(
    int unit,
    bcm_port_t logical_port,
    int cosq,
    dnx_tcg_ndx_t * tcg_p)
{
    int base_q_pair, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);
    if (cosq < DNX_TCG_MIN || cosq > DNX_TCG_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "unit %d, Invalid cosq parameter %d. Allowed range (incl. edges) is %d,%d",
                     unit, cosq, DNX_TCG_MIN, DNX_TCG_MAX);
    }
    /*
     * for the 4 priority port which takes the 4 upper qpairs of the PS, TCG index needs to be shifted by 4
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (cosq > nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cosq parameter (%d) is bigger than nof priorities (%u)", cosq, nof_priorities);
    }
    *tcg_p = cosq + (base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT);
exit:
    SHR_FUNC_EXIT;
}
/* 
 * \brief -
 *   Given logical port, verify that corresponding TCG index is valid.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] logical_port -
 *   Logical port to verify for. 
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. Index TC group, on this port, for which verify.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Verify by checking that 'tcg_ndx' is within range of number of
 *     priorities assigned to input 'logical port'
 * \see
 *   * dnx_cosq_egq_gport_egress_port_tcg_sched_set
 */
static shr_error_e
dnx_cosq_egq_port_egq_tcg_validate(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx)
{
    int nof_priorities, base_q_pair;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * retrieve port 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    tcg_ndx -= base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT;
    if (tcg_ndx >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "cosq is out of range. Should be in range of 0-%d. logical port: 0x%08X cosq %d failed",
                     nof_priorities, logical_port, tcg_ndx);
    }
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d TCG is not supported for given logical port 0x%08X. ", unit, logical_port);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * \brief -
 *   Given 'mode' and 'weight', get implied EIR weight for Egress
 *   Port TCG).
*    No HW is touched by this procedure.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] mode -
 *   int. 
 *     If set to '-1' then this procedure loads '*tcg_weight'
 *       by input 'weight' to indicate WFQ scheme scheme
 *     Else if set to 'BCM_COSQ_NONE' then this procedure loads '*tcg_weight'
 *       by '-1' to indicate weight is invalid
 * \param [in] weight -
 *   int. See 'mode' above. Used only when 'mode' is '-1'.
 * \param [out] tcg_weight -
 *   Pointer to dnx_egr_tcg_sch_wfq_t. SEe 'mode' above. This procedure loads
 *   pointed structure by input 'weight' or by '-1' (if actual weight is meaningless).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * This procedure does not access HW
 * \see
 *   * dnx_cosq_egq_gport_egress_port_tcg_sched_set
 */
static shr_error_e
dnx_cosq_egq_fill_tcg_weight_from_mode_weight(
    int unit,
    int mode,
    int weight,
    uint32 *tcg_weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (mode == -1)
    {
        /*
         * Take weight into consideration 
         */
        *tcg_weight = weight;
    }
    else if (mode == BCM_COSQ_NONE)
    {
        *tcg_weight = -1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - invalid mode given. mode: %d, weight: %d", mode, weight);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * \brief -
 *   Fill in the structure of wfq scheduler given
 *   bcm mode (SP) and weight
 *
 *   Given (a) a flag indicating 'unicast or multicast', (b) 'mode' which indicates whether
 *   to use SP (strict priority) or WFQ (weighted queuing) and (c) the weight to apply
 *   (if relevant) then
 *   load output structure by calculated weights, as required by HW
 *   See detailed logic in 'mode' below.
 * \param [in] is_ucast_egress_queue -
 *   int. Flag. If TRUE then required weights are for unicast traffic. Else,
 *   for multicast.
 * \param [in] mode -
 *   int. Flag.
 *   Can be:
 *     -1
 *     DNX_COSQ_EGR_OFP_SCH_SP_HIGH
 *     DNX_COSQ_EGR_OFP_SCH_SP_LOW
 *   If set to '-1' then input 'weight' is used. This procedure will select an
 *     element on 'sch_wfq_p' as per 'is_ucast_egress_queue' and will load it
 *     by input 'weight'. The other element is not touched.
 *   Else
 *     If unicast traffic (non-zero is_ucast_egress_queue)
 *       If 'mode' is SP_HIGH then,
 *         Set unicast weight to '0' and multicast to '1'. This
 *         setup gives strictly high priority to unicast.
 *       If 'mode' is SP_LOW then reverse unicast/multicast roles.
 *     Else (multicast traffic)
 *       If 'mode' is SP_HIGH then,
 *         Set unicast weight to '1' and multicast to '0'. This
 *         setup gives strictly high priority to multicast.
 *       If 'mode' is SP_LOW then reverse unicast/multicast roles.
 * \param [in] weight -
 *   This is the weight to assign as per 'mode'. (Only used when mode is -1)
 * \param [in] sch_wfq_p -
 *   Pointer to dnx_egr_ofp_sch_wfq_t. Caller is assumed to have cleared this structure.
 *   This procedure loads its two elements as follows:
 *     unsched_weight - Weight for multicast traffic
 *     sched_weight   - Weight for unicast traffic
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_egq_queue_wfq_set
 *   * dnx_cosq_egq_queue_wfq_get
 */
static shr_error_e
dnx_cosq_egq_fill_ofp_wfq_sch_from_mode_weight(
    int is_ucast_egress_queue,
    int mode,
    int weight,
    dnx_egr_ofp_sch_wfq_t * sch_wfq_p)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(sch_wfq_p, _SHR_E_PARAM, "sch_wfq_p");
    if (mode != -1)
    {
        /*
         * SP 
         */
        /*
         * 0 - indicate SP High 
         */
        /*
         * > 0 - indicate lower priority 
         */
        if (is_ucast_egress_queue == TRUE)
        {
            sch_wfq_p->sched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 0 : 1;
            sch_wfq_p->unsched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 1 : 0;
        }
        else
        {
            sch_wfq_p->sched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 1 : 0;
            sch_wfq_p->unsched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 0 : 1;
        }
    }
    else
    {
        /*
         * Take weight into consideration 
         */
        if (is_ucast_egress_queue == TRUE)
        {
            sch_wfq_p->sched_weight = weight;
        }
        else
        {
            sch_wfq_p->unsched_weight = weight;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/*
 * utility functions
 */
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_user_port_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_t * user_port_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(user_port_p, _SHR_E_PARAM, "user_port_p");
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, user_port_p));
exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_port_priority_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    int nof_priorities;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * retrieve port 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_user_port_get(unit, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (cosq >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "cosq is out of range. Should be in range of 0-%d. gport: 0x%08X cosq %d failed",
                     nof_priorities - 1, gport, cosq);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_cosq_priority_convert(
    int unit,
    int cosq,
    dnx_egr_q_prio_e * prio_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(prio_p, _SHR_E_PARAM, "prio_p");
    switch (cosq)
    {
        case BCM_COSQ_HIGH_PRIORITY:
        case 0:
        {
            *prio_p = DNX_EGR_Q_PRIO_0;
            break;
        }
        case BCM_COSQ_LOW_PRIORITY:
        case 1:
        {
            *prio_p = DNX_EGR_Q_PRIO_1;
            break;
        }
        case 2:
        {
            *prio_p = DNX_EGR_Q_PRIO_2;
            break;
        }
        case 3:
        {
            *prio_p = DNX_EGR_Q_PRIO_3;
            break;
        }
        case 4:
        {
            *prio_p = DNX_EGR_Q_PRIO_4;
            break;
        }
        case 5:
        {
            *prio_p = DNX_EGR_Q_PRIO_5;
            break;
        }
        case 6:
        {
            *prio_p = DNX_EGR_Q_PRIO_6;
            break;
        }
        case 7:
        {
            *prio_p = DNX_EGR_Q_PRIO_7;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d: invalid cosq %d", unit, cosq);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
#if (0)
/* { */
/**
 * \brief
 *   Schedule mode for Channelized NIF port and CPU interface.
 *   Get value of Strict priority which is used by this system. SP range: H/L
 *   Given 'gport' to identify a specific base qpair, get its priority
 *   in '*mode' (high or low).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'.
 * \param [in] cosq -
 *   bcm_cos_queue_t. Must be zero.
 * \param [out] mode -
 *   Pointer to int. This procedure loads pointed memory by the priority assigned to interface
 *   corresponding to indicated qpair. Can only be high (BCM_COSQ_SP0) or low (BCM_COSQ_SP1).
 *   See dnx_egq_dbal_gport_scheduler_queue_sched_set().
 * \param [out] weight -
 *   Pointer int. Ignored by this procedure. (Loaded by '-1')
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * This procedure is, currently, not called from anywhere in the code.
 *   * Accessing HW table EPS_QP_CFG, field QP_IS_HP
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, field DBAL_FIELD_QP_IS_HP
 * \see
 *   * dnx_egq_dbal_gport_scheduler_queue_sched_set
 */
shr_error_e
dnx_cosq_egq_gport_egress_scheduler_port_is_high_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d. Cosq must be set to 0 in case of Local gport", cosq);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_scheduler_queue_sched_get(unit, gport, cosq, mode, weight));
exit:
    SHR_FUNC_EXIT;
}
/* } */
#endif
/*
 * Validation procedure for dnx_cosq_egq_gport_egress_tc_to_tcg_map_set()
 * See description of IO parameters there.
 */
static shr_error_e
dnx_cosq_egq_gport_egress_tc_to_tcg_map_set_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    dnx_egr_q_prio_e prio;

    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only port tc gport. gport: 0x%x", gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    if ((!(mode >= BCM_COSQ_SP0 && mode <= BCM_COSQ_SP7)) && (!(mode >= DNX_TCG_MIN && mode <= DNX_TCG_MAX)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - mode range must be tcg_ndx (0-7) or SP0-SP7 (%d,%d)",
                     BCM_COSQ_SP0, BCM_COSQ_SP7);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See conq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_tc_to_tcg_map_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    bcm_port_t logical_port;
    int base_q_pair, nof_priorities;
    dnx_egr_q_prio_e prio;
    dnx_tcg_ndx_t tcg;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate 
     */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_gport_egress_tc_to_tcg_map_set_validate(unit, gport, cosq, mode, weight));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    /*
     * We assume (verified on egr_queuing.h) that BCM_COSQ_SP0 is larger than DNX_TCG_MAX
     */
    tcg = (mode >= BCM_COSQ_SP0) ? (mode - BCM_COSQ_SP0) : mode;
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (tcg > nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tcg parameter (%d) is bigger than nof priorities (%u)", tcg, nof_priorities);
    }
    /*
     * for the 4 priority port which takes the 4 upper qpairs of the PS, TCG index needs to be shifted by 4
     */
    tcg += base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT;
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_to_tcg_mapping_set(unit, logical_port, prio, tcg));
exit:
    SHR_FUNC_EXIT;
}
/*
 * Validate procedure for dnx_cosq_egq_gport_egress_tc_to_tcg_map_get()
 * See description of io parameters there.
 */
static shr_error_e
dnx_cosq_egq_gport_egress_tc_to_tcg_map_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode_p,
    int *weight_p)
{
    dnx_egr_q_prio_e prio;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mode_p, _SHR_E_PARAM, "mode_p");
    SHR_NULL_CHECK(weight_p, _SHR_E_PARAM, "weight_p");
    /*
     * Validate 
     */
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only port tc gport. gport: 0x%08X", gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));

exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_tc_to_tcg_map_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode_p,
    int *weight_p)
{
    int base_q_pair;
    dnx_tcg_ndx_t tcg_ndx;
    dnx_egr_q_prio_e prio;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_gport_egress_tc_to_tcg_map_get_validate(unit, gport, cosq, mode_p, weight_p));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    /*
     * Clear
     */
    tcg_ndx = 0;
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    /*
     * Get existing configuration.
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_to_tcg_mapping_get(unit, logical_port, prio, &tcg_ndx));
    /*
     * for the 4 priority port which takes the 4 upper qpairs of the PS, TCG index needs to be shifted by 4
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    *mode_p = tcg_ndx + BCM_COSQ_SP0 - (base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT);
exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_port_tcg_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    bcm_port_t logical_port;
    dnx_egr_tcg_sch_wfq_t info;
    dnx_tcg_ndx_t tcg_ndx;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate 
     */
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only TCG gport. gport: 0x%08X", gport);
    }
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg_ndx));
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_port_egq_tcg_validate(unit, logical_port, tcg_ndx));
    /*
     * Verify 
     */
    if (!((mode == -1) || (mode == BCM_COSQ_NONE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - mode (%d) can be -1 or BCM_COSQ_NONE (%d)", mode, BCM_COSQ_NONE);
    }
    if (mode == -1)
    {
        /*
         * Weight is taken into consideration. Must be between 1 and DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX
         */
        if (!((weight) >= 0 && ((weight) <= (DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Mode is %d but weight (%d) is invalid (Should be between %d and %d)",
                         mode, weight, 0, DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX);
        }
    }
    /*
     * Clear 
     */
    sal_memset(&info, 0, sizeof(info));
    /*
     * Get existing configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_weight_get(unit, logical_port, tcg_ndx, &info));
    info.tcg_weight_valid = (mode == BCM_COSQ_NONE) ? FALSE : TRUE;
    SHR_IF_ERR_EXIT(dnx_cosq_egq_fill_tcg_weight_from_mode_weight(unit, mode, weight, &info.tcg_weight));

    /*
     * Traffic for the port need to be disabled before setting the weights only when one of the ports moved to be SP
     * This is due to HW limitation that this can not be done under traffic
     * After the configuration port traffic should be enabled again
     */
    if (weight == 0)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, logical_port, FALSE, TRUE));
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_weight_set(unit, logical_port, tcg_ndx, &info));
    if (weight == 0)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, logical_port, TRUE, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_port_tcg_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    bcm_port_t logical_port;
    dnx_egr_tcg_sch_wfq_t info;
    dnx_tcg_ndx_t tcg_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");
    /*
     * Validate 
     */
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only TCG gport. gport: 0x%08X", gport);
    }

    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg_ndx));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_egq_tcg_validate(unit, logical_port, tcg_ndx));
    /*
     * Clear 
     */
    sal_memset(&info, 0, sizeof(info));
    /*
     * Get existed configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_weight_get(unit, logical_port, tcg_ndx, &info));
    if (info.tcg_weight_valid != FALSE)
    {
        *mode = -1;
        *weight = info.tcg_weight;
    }
    else
    {
        *mode = BCM_COSQ_NONE;
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * Validation procedure for dnx_cosq_egq_queue_wfq_set()
 * See description of io parameters there.
 */
static shr_error_e
dnx_cosq_egq_queue_wfq_set_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int max_cosq_num;
    dnx_egr_q_prio_e queue_id;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    max_cosq_num = DNX_COSQ_PB_EGR_OFP_COSQ_NUM_MAX(unit);
    if (!((mode == -1) || (mode >= BCM_COSQ_SP0 && mode <= BCM_COSQ_SP1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "parameter error - mode can be -1 or BCM_COSQ_SP0-1 (%d) or BCM_COSQ_SP0-0 (%d) ", BCM_COSQ_SP1,
                     BCM_COSQ_SP0);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &queue_id));
    if (!((queue_id >= 0 && queue_id <= max_cosq_num)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid queue_id parameter %d, max_cosq_num for this ofp: %d", queue_id, max_cosq_num);
    }
    if (mode == -1)
    {
        /*
         * Weight is taken into consideration 
         */
        if (!((weight >= 1) && (weight <= DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid weight given: %d. Must be between %d and %d (incl.)", weight, 1,
                         DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_queue_wfq_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    dnx_egr_ofp_sch_wfq_t sch_wfq;
    int is_ucast_egress_queue;
    dnx_egr_q_prio_e queue_id;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_queue_wfq_set_validate(unit, gport, cosq, mode, weight));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &queue_id));
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport))
    {
        /*
         * UC Egress queue fill 
         */
        is_ucast_egress_queue = TRUE;
    }
    else
    {
        /*
         * MC Egress queue fill 
         */
        is_ucast_egress_queue = FALSE;
    }
    /*
     * Since, in case 'mode = -1', procedure dnx_cosq_egq_fill_ofp_wfq_sch_from_mode_weight() only
     * loads one of the two weights (See 'mode' in the documentation of the corresponding H file), we
     * need to read both so that the other remains as is.
     *
     * Get current configuration.
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_wfq_get(unit, logical_port, queue_id, &sch_wfq));
    /*
     * Fill WFQ SCH structure 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_fill_ofp_wfq_sch_from_mode_weight(is_ucast_egress_queue, mode, weight, &sch_wfq));
    /*
     * Set new configuration 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_wfq_set(unit, logical_port, queue_id, &sch_wfq));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_cosq_egq_queue_wfq_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    int max_cosq_num;
    dnx_egr_q_prio_e queue_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");
    max_cosq_num = DNX_COSQ_PB_EGR_OFP_COSQ_NUM_MAX(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &queue_id));
    if (!((queue_id >= 0 && queue_id <= max_cosq_num)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid queue_id parameter %d, queue_id can be in range of 0-1 or High/Low priorities", queue_id);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h 
 */
shr_error_e
dnx_cosq_egq_queue_wfq_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    dnx_egr_ofp_sch_wfq_t sch_wfq;
    bcm_port_t logical_port;
    int is_ucast_egress_queue;
    int max_cosq_num;
    dnx_egr_q_prio_e queue_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_queue_wfq_get_validate(unit, gport, cosq, mode, weight));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &queue_id));
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    max_cosq_num = DNX_COSQ_PB_EGR_OFP_COSQ_NUM_MAX(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &queue_id));
    if (!((queue_id >= 0 && queue_id <= max_cosq_num)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid queue_id parameter %d, queue_id can be in range of 0-1 or High/Low priorities", queue_id);
    }
    /*
     * Get existing configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_wfq_get(unit, logical_port, queue_id, &sch_wfq));
    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport))
    {
        /*
         * UC Egress queue fill 
         */
        is_ucast_egress_queue = TRUE;
    }
    else
    {
        /*
         * MC Egress queue fill 
         */
        is_ucast_egress_queue = FALSE;
    }
    {
        /*
         * \brief -
         *   Fill in the bcm mode (SP) and weight from structure of wfq scheduler
         * 
         *   Given (a) a flag indicating 'unicast or multicast', (b) Structure of
         *     type 'dnx_egr_ofp_sch_wfq_t' which is loaded by 
         *       unsched_weight - Weight for multicast traffic
         *       sched_weight   - Weight for unicast traffic
         *   then apply reverse logic as 'dnx_cosq_egress_fill_ofp_wfq_sch_from_mode_weight()':
         *
         *     If flag indicates 'unicast'
         *       If 'unicast weight' is '0' (SP case)
         *         Set *mode to SP_HIGH (and *weight to -1)
         *       Else if 'multicast weight' is '0'  (SP case)
         *         Set *mode to SP_LOW (and *weight to -1)
         *       Else do take weight into consideration:
         *         Set '*mode' to '-1' (meaning 'weight is meaningful')
         *         Set *weight with input 'unicast weight'
         *     Else (flag indicates 'multicast')
         *       If 'multicast weight' is '0' (SP case)
         *         Set *mode to SP_HIGH (and *weight to -1)
         *       Else if 'unicast weight' is '0'  (SP case)
         *         Set *mode to SP_LOW (and *weight to -1)
         *       Else do take weight into consideration:
         *         Set '*mode' to '-1' (meaning 'weight is meaningful')
         *         Set *weight with input 'multicast weight'
         * \param [in] is_ucast_egress_queue -
         *   int. Flag. If TRUE then required weights are for unicast traffic. Else,
         *   for multicast.
         * \param [in] sch_wfq -
         *   dnx_egr_ofp_sch_wfq_t. Caller is assumed to have loaded this structure.
         *   This procedure uses only ONE of the two elements as follows:
         *     sched_weight   - Weight for unicast traffic
         * \param [out] mode -
         *   Pointer to int. This procedure loads pointed memory by a flag as described above.
         *   Can be:
         *     -1
         *     DNX_COSQ_EGR_OFP_SCH_SP_HIGH
         *     DNX_COSQ_EGR_OFP_SCH_SP_LOW
         * \param [out] weight -
         *   Pointer to int. This procedure loads pointed memory by a flag as described above.
         *   Only meaningful if '*mode' is '-1'.
         * \remark
         *   * None
         * \see
         *   * dnx_cosq_egq_queue_wfq_get
         *   * dnx_cosq_egq_queue_wfq_set
         *   * dnx_cosq_egress_fill_ofp_wfq_sch_from_mode_weight
         */
        if (is_ucast_egress_queue == TRUE)
        {
            if (sch_wfq.sched_weight == 0)
            {
                *mode = DNX_COSQ_EGR_OFP_SCH_SP_HIGH;
                *weight = -1;
            }
            else
            {
                if (sch_wfq.unsched_weight == 0)
                {
                    *mode = DNX_COSQ_EGR_OFP_SCH_SP_LOW;
                    *weight = -1;
                }
                else
                {
                    /*
                     * Take weight into consideration 
                     */
                    *mode = -1;
                    *weight = sch_wfq.sched_weight;
                }
            }
        }
        else
        {
            if (sch_wfq.unsched_weight == 0)
            {
                *mode = DNX_COSQ_EGR_OFP_SCH_SP_HIGH;
                *weight = -1;
            }
            else
            {
                if (sch_wfq.sched_weight == 0)
                {
                    *mode = DNX_COSQ_EGR_OFP_SCH_SP_LOW;
                    *weight = -1;
                }
                else
                {
                    /*
                     * Take weight into consideration 
                     */
                    *mode = -1;
                    *weight = sch_wfq.unsched_weight;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_egq_gport_egress_port_tc_bandwidth_set_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    dnx_egr_q_prio_e prio;

    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tc", unit, gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    /*
     * Validate gport 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: flags: %d\nFlags must be set to 0", flags);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_port_tc_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    dnx_egr_q_prio_e prio;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_gport_egress_port_tc_bandwidth_set_validate
                          (unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    if (kbits_sec_max > DNX_IF_MAX_RATE_KBPS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_max: %d", kbits_sec_max);
    }
    /*
     * Set configuration 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_rate_sw_set(unit, logical_port, prio, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_rate_hw_set(unit));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_cosq_egq_gport_egress_port_tc_bandwidth_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    dnx_egr_q_prio_e prio;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tc", unit, gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    /*
     * Validate gport 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_port_tc_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    dnx_egr_q_prio_e prio;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_gport_egress_port_tc_bandwidth_get_validate
                          (unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    /*
     * Get existing configuration 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_rate_hw_get(unit, port, prio, kbits_sec_max));
    /*
     * fill rates 
     */
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_port_tcg_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    dnx_tcg_ndx_t tcg;
    int core;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tcg", unit, gport);
    }
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_egq_tcg_validate(unit, logical_port, tcg));
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: flags: %d\nFlags must be set to 0", flags);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    if (kbits_sec_max > DNX_IF_MAX_RATE_KBPS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_max: %d. Larger than maximum allowed: %d",
                     kbits_sec_max, DNX_IF_MAX_RATE_KBPS(unit));
    }
    /*
     * Set configuration 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_rate_sw_set(unit, core, logical_port, tcg, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_rate_hw_set(unit));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_port_tcg_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    dnx_tcg_ndx_t tcg;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tcg", unit, gport);
    }
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_egq_tcg_validate(unit, logical_port, tcg));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_rate_hw_get(unit, logical_port, tcg, kbits_sec_max));
    /*
     * Fill unused outputs 
     */
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}
/*
 * Validation procedure for dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_set().
 * See description of input parameters there.
 */
static shr_error_e
dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_set_validate(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    int channelized;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit(%d) invalid cosq %d", unit, cosq);
    }
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: flags: %d\nFlags must be set to 0", flags);
    }
    if (!channelized)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bandwidth configuration for non-channnelized interface is not allowed");
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    int core;
    bcm_port_t logical_port;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_user_port_get(unit, gport, &logical_port));
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_set_validate
                          (unit, logical_port, cosq, kbits_sec_min, kbits_sec_max, flags));
    /*
     * get logical_port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    /*
     * update calendar shaper rate at SW database 
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.set(unit, core, calendar_id, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_otm_shapers_set(unit, core, TRUE));

exit:
    SHR_FUNC_EXIT;
}
/*
 * Validation procedure for dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_get()
 * See description of IO there
 */
static shr_error_e
dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    int channelized;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_user_port_get(unit, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d\nCosq must be set to 0 in case of interface gport",
                     cosq);
    }
    if (!channelized)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bandwidth configuration for non-channnelized interface is not allowed");
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_get_validate
                          (unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_interface_shaper_get(unit, logical_port, kbits_sec_max));
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}
/*
 * Validate procedure for dnx_cosq_egq_control_egress_port_scheduler_set()
 * See description of IO parameters there.
 */
static shr_error_e
dnx_cosq_egq_control_egress_port_scheduler_set_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    if (cosq != 0)
    {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "Invalid parameter: cosq: %d\n"), cosq));
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cosq must be set to 0 in case of Local gport");
    }
    switch (type)
    {
        case bcmCosqControlPacketLengthAdjust:
        {
            if (arg <= DNX_COSQ_EGRESS_CR_DISCOUNT_MIN_VAL || arg >= DNX_COSQ_EGRESS_CR_DISCOUNT_MAX_VAL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid argument discount %d parameter", arg);
            }
            break;
        }
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Set burst max for egq rate shaper 
             */
            /*
             * Verify 
             */
            if ((unsigned int) arg > DNX_OFP_RATES_BURST_LIMIT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid arg parameter (max burst in bytes): %d. More than max burst %d\n", arg,
                             DNX_OFP_RATES_BURST_LIMIT_MAX);
            }
            break;
        }
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        {
            /*
             * Verify 
             */
            if ((unsigned int) arg > DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid arg parameter (max burst in KBPS): %d. More than max burst %d\n", arg,
                             DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX);
            }
            break;
        }
        case bcmCosqControlEgressPriorityOverCast:
        {
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;

}
/*
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_control_egress_port_scheduler_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_control_egress_port_scheduler_set_validate(unit, gport, cosq, type, arg));
    switch (type)
    {
        case bcmCosqControlPacketLengthAdjust:
        {
            SHR_IF_ERR_EXIT(dnx_egr_port_compensation_set(unit, BCM_GPORT_LOCAL_GET(gport), arg));
            break;
        }
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Set burst max for egq rate shaper 
             */
            int core;
            bcm_port_t logical_port;

            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
            SHR_IF_ERR_EXIT(dnx_ofp_rates_single_port_max_burst_set(unit, core, logical_port, arg));
            /*
             * Devices which supporting small burst size need to disable relative interface shaper
             * This is the limitation for enabling this feature
             */
            if (dnx_data_egr_queuing.params.burst_size_below_mtu_get(unit) && arg < DNX_EGQ_MAX_BURST_IN_BYTES)
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_shaper_enable_set(unit, logical_port, FALSE));
            }
            break;
        }
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        {
            /*
             * Verify 
             */
            SHR_IF_ERR_EXIT(dnx_egq_dbal_empty_port_max_credit_set
                            (unit, arg * dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
            break;
        }
        case bcmCosqControlEgressPriorityOverCast:
        {
            /*
             * Set SP before WFQ mode (per OTM port) 
             */
            bcm_port_t logical_port;

            logical_port = BCM_GPORT_LOCAL_GET(gport);
            SHR_IF_ERR_EXIT(dnx_egr_queuing_prio_over_cast_set(unit, logical_port, arg));
            break;
        }
        default:
        {
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_control_egress_port_tc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid gport parameter 0x%08X, function supports only port tc", unit,
                     gport);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        case bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
        {
            /*
             * Set burst max for egq rate shaper 
             */
            int core;
            bcm_port_t logical_port;
            dnx_egr_q_prio_e prio;

            SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
            /*
             * Validate input priority is suitable for gport 
             */
            SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
            if (arg > DNX_OFP_RATES_BURST_LIMIT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg parameter (max burst in bytes): %d. more than max burst %d\n",
                             arg, DNX_OFP_RATES_BURST_LIMIT_MAX);
            }
            /*
             * Set Port-TC rate Info 
             */
            /*
             * At this point, 'type' can only be:
             *   bcmCosqControlBandwidthBurstMax:
             *   bcmCosqControlBandwidthBurstMaxEmptyQueue:
             *   bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
             * - Compiler wants to see all 'type's in a switch (or use 'default')
             * - If all types are specified then coverity complains that 'default' will never be used.
             * So, sadly, apply code as below (although it is not best practice).
             */
            if (type == bcmCosqControlBandwidthBurstMax)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_max_burst_set(unit, core, logical_port, prio, arg));
            }
            else if (type == bcmCosqControlBandwidthBurstMaxEmptyQueue)
            {
                SHR_IF_ERR_EXIT(dnx_egq_ofp_rates_empty_queues_max_burst_set
                                (unit, arg * dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
            }
            else
            {
                /*
                 * This is case: type == bcmCosqControlBandwidthBurstMaxFlowControlledQueue
                 */
                SHR_IF_ERR_EXIT(dnx_egq_ofp_rates_fc_queues_max_burst_set
                                (unit, arg * dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
            }
            break;
        }
        case bcmCosqControlPrioritySelect:
        {
            dnx_algo_port_info_s port_info;
            bcm_port_t logical_port;
            int is_egress_interleave;

            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

            if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, type %d not supported for RCY interface\n", unit, type);
            }

            SHR_IF_ERR_EXIT(dnx_algo_port_is_egress_interleave_get(unit, logical_port, &is_egress_interleave));
            if ((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1) && is_egress_interleave))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "unit %d, type %d not supported for ILKN interface if egress interleave is enabled\n",
                             unit, type);
            }

            SHR_IF_ERR_EXIT(dnx_egq_dbal_gport_scheduler_queue_sched_set(unit, gport, cosq, arg, -1));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_control_egress_port_tcg_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%08X, function support only port tcg", unit,
                     type);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Set burst max for egq rate shaper 
             */
            dnx_tcg_ndx_t tcg;
            bcm_port_t logical_port;
            /*
             * Validate gport 
             */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            SHR_IF_ERR_EXIT(dnx_cosq_egq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg));
            SHR_IF_ERR_EXIT(dnx_cosq_egq_port_egq_tcg_validate(unit, logical_port, tcg));
            if ((unsigned int) arg > DNX_OFP_RATES_BURST_LIMIT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg parameter: Max burst size (%d) is larger than max burst, %d\n",
                             arg, DNX_OFP_RATES_BURST_LIMIT_MAX);
            }
            /*
             * Set conifguration 
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_max_burst_set(unit, logical_port, tcg, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h 
 */
int
dnx_cosq_egq_control_egress_port_tc_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int dummy;
    uint32 tmp_reg;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
    /*
     * Just to keep coverity happy...
     */
    tmp_reg = 0;
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tc", unit, gport);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        case bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
        {
            /*
             * Get burst max for egq rate shaper 
             */
            int core;
            bcm_port_t logical_port;
            dnx_egr_q_prio_e prio;

            SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
            /*
             * Validate gport 
             */
            SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
            /*
             * Set arg
             */
            /*
             * At this point, 'type' can only be:
             *   bcmCosqControlBandwidthBurstMax:
             *   bcmCosqControlBandwidthBurstMaxEmptyQueue:
             *   bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
             * - Compiler wants to see all 'type's in a switch (or use 'default')
             * - If all types are specified then coverity complains that 'default' will never be used.
             * So, sadly, apply code as below (although it is better practice).
             */
            if (type == bcmCosqControlBandwidthBurstMax)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_max_burst_get
                                (unit, core, logical_port, prio, &tmp_reg));
            }
            else if (type == bcmCosqControlBandwidthBurstMaxEmptyQueue)
            {
                SHR_IF_ERR_EXIT(dnx_egq_ofp_rates_empty_queues_max_burst_get(unit, &tmp_reg));
                /*
                 * Convert credits to bytes.
                 */
                tmp_reg /= dnx_data_egr_queuing.params.cal_burst_res_get(unit);
            }
            else
            {
                /*
                 * This is: case bcmCosqControlBandwidthBurstMaxFlowControlledQueue
                 */
                SHR_IF_ERR_EXIT(dnx_egq_ofp_rates_fc_queues_max_burst_get(unit, &tmp_reg));
                /*
                 * Convert credits to bytes.
                 */
                tmp_reg /= dnx_data_egr_queuing.params.cal_burst_res_get(unit);
            }
            *arg = (int) tmp_reg;
            break;
        }
        case bcmCosqControlPrioritySelect:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_scheduler_queue_sched_get(unit, gport, cosq, arg, &dummy));
            break;
        }
        default:
        {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "unit %d, Invalid type %d\n"), unit, type));
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_control_egress_port_tcg_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%08X, function supports only port tcg", unit,
                     type);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Get burst max for egq rate shaper 
             */
            dnx_tcg_ndx_t tcg;
            bcm_port_t logical_port;
            /*
             * Validate gport 
             */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            SHR_IF_ERR_EXIT(dnx_cosq_egq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg));
            SHR_IF_ERR_EXIT(dnx_cosq_egq_port_egq_tcg_validate(unit, logical_port, tcg));
            /*
             * Get existing configuration 
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_max_burst_get(unit, logical_port, tcg, (uint32 *) arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See cosq_egq.h 
 */
shr_error_e
dnx_cosq_egq_control_egress_port_scheduler_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg_p, _SHR_E_PARAM, "arg_p");
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d\nCosq must be set to 0 in case of Local gport", cosq);
    }
    switch (type)
    {
        case bcmCosqControlPacketLengthAdjust:
        {
            SHR_IF_ERR_EXIT(dnx_egr_port_compensation_get(unit, BCM_GPORT_LOCAL_GET(gport), arg_p));
            break;
        }
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Get burst max for egq rate shaper 
             */
            int core;
            bcm_port_t logical_port;

            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
            SHR_IF_ERR_EXIT(dnx_ofp_rates_single_port_max_burst_get(unit, core, logical_port, (uint32 *) arg_p));
            break;
        }
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        {
            int tmp_reg;

            SHR_IF_ERR_EXIT(dnx_egq_dbal_empty_port_max_credit_get(unit, &tmp_reg));
            /*
             * Make sure to return bytes and not credits.
             */
            tmp_reg /= dnx_data_egr_queuing.params.cal_burst_res_get(unit);
            *arg_p = tmp_reg;
            break;
        }
        case bcmCosqControlEgressPriorityOverCast:
        {
            /*
             * Get SP before WFQ mode (per OTM port) 
             */
            bcm_port_t logical_port;

            logical_port = BCM_GPORT_LOCAL_GET(gport);
            SHR_IF_ERR_EXIT(dnx_egr_queuing_prio_over_cast_get(unit, logical_port, arg_p));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Validation procedure for dnx_cosq_egq_gport_egress_multicast_config_get()
 * See description of IO there
 */
static shr_error_e
dnx_cosq_egq_gport_egress_multicast_config_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate input params 
     */
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");
    if (ingress_pri < 0 || ingress_pri >= DNX_COSQ_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d", ingress_pri);
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_COSQ_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d", ingress_dp);
    }
    if (flags & ~BCM_COSQ_MULTICAST_SCHEDULED)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported flag parameter %08X", flags);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * See cosq_egq.h
 */
shr_error_e
dnx_cosq_egq_gport_egress_multicast_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    dnx_egr_q_prio_mapping_type_e map_type;
    dnx_egr_q_priority_t egr_prio;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate input params 
     */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_egq_gport_egress_multicast_config_get_validate
                          (unit, gport, ingress_pri, ingress_dp, flags, config));

    sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
    sal_memset(&egr_prio, 0, sizeof(egr_prio));
    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
        gport = logical_port;
    }
    /** get the pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));

    /** use first pp port from the list, even if list contain more */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get
                    (unit, gport_info.internal_port_pp_info.pp_port[0],
                     gport_info.internal_port_pp_info.core_id[0], &mapping_info));
    /*
     * Multicast type 
     */
    map_type = DNX_EGR_MCAST_TO_UNSCHED;
    config->scheduled_dp = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].dp;
    config->priority = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc;
exit:
    SHR_FUNC_EXIT;
}
/* 
 * This procedure is currently empty. Fill proper header
 * when it gets meaningful info.
 * Purpose:
 * Get various controls related to egress interface scheduler
 * (interface port)
 */
int
dnx_cosq_control_egress_interface_scheduler_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (type)
    {
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/* 
 * \brief -
 *   Map gport (corresponding to an egress queue) to a specific priority ('offset'
 *   from base qpair)
 *     Given a gport plus priority (TC) and Drop-precedence (DP) of unicast traffic
 *     entering egress, set the priority for outgoing traffic (by matching it to
 *     a specific port, related to an egress queue).
 *     Note that DP of outgoing traffic is not touched.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] gport -
 *   bcm_gport_t. Identifier of traffic by its assigned gport.
 * \param [in] ingress_pri -
 *   bcm_cos_t. Priority (TC) of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_TC-1 (7).
 * \param [in] ingress_dp -
 *   bcm_color_t. Drop precedence of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_DP-1 (3).
 * \param [in] offset -
 *   bcm_cos_queue_t. Priority to assign to outgoing traffic. Must be between 0 and
 *   DNX_EGR_NOF_Q_PRIO-1 (7).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * HW is updated on the core associated with 'gport'.
 * \see
 *   * bcm_dnx_cosq_gport_egress_map_set
 *   * dnx_egr_queuing_profile_mapping_set
 */
int
bcm_dnx_cosq_gport_egress_map_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t offset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport) ||
        BCM_GPORT_IS_TRUNK(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_queue_map_set(unit, gport, ingress_pri, ingress_dp, offset));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported gport type input parameter. Gport is 0x%X", gport);
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/*
 * \brief 
 *   This API is used to get multicast configuration.
 *          two configurtion options are accessed through this API,
 *          for more info see bcm_dnx_cosq_gport_egress_multicast_config_set.
 *
 * \param [in] unit -
 *   Unit-ID
 * \param [in] gport -
 *   Must be suitable to flags, see brief.
 *   valid values: CORE, LOCAL, MODPORT.
 *   NOTE: on get gport cannot be set to 0.
 * \param [in] ingress_pri -
 *   Priority (system TC)
 * \param [in] ingress_dp -
 *   Color (system DP)
 * \param [in] flags -
 *   BCM_COSQ_MULTICAST_UNSCHEDULED or BCM_COSQ_MULTICAST_SCHEDULED
 * \param [out] config -
 *   Will be set with the configured values. (see cosq.h)
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_egress_multicast_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (((BCM_COSQ_GPORT_IS_CORE(gport) && BCM_COSQ_GPORT_CORE_GET(gport) == BCM_CORE_ALL) || (gport == 0)) &&
        (flags == BCM_COSQ_MULTICAST_UNSCHEDULED))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_core_multicast_config_get(unit, ingress_pri, ingress_dp, config));
    }
    else if ((BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_MODPORT(gport) || BCM_GPORT_IS_TRUNK(gport)) &&
             (flags & BCM_COSQ_MULTICAST_SCHEDULED))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_multicast_config_get
                        (unit, gport, ingress_pri, ingress_dp, flags, config));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "invalid port parameter");
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * Validation procedure for part of bcm_dnx_cosq_gport_egress_multicast_config_set().
 * See description of IO parameters there.
 */
static shr_error_e
dnx_cosq_gport_egress_multicast_config_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config_p)
{
    dnx_egr_q_prio_e queue_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(config_p, _SHR_E_PARAM, "config_p");
    if (ingress_pri < 0 || ingress_pri >= DNX_COSQ_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d", ingress_pri);
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_COSQ_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d", ingress_dp);
    }
    if (!(flags == BCM_COSQ_MULTICAST_SCHEDULED))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported flag parameter 0x%08X", flags);
    }
    if (config_p->scheduled_dp < 0 || config_p->scheduled_dp > DNX_COSQ_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid scheduled_dp parameter %d", config_p->scheduled_dp);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, config_p->priority, &queue_id));
    if (queue_id < 0 || queue_id >= DNX_EGR_NOF_Q_PRIO)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid config_p->priority parameter %d", queue_id);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief
 *   This API is used to set multicast configuration.
 *          Two configurtion options are accessed through this API:
 *          1. flags = BCM_COSQ_MULTICAST_UNSCHEDULED and
 *             gport = 0 or BCM_CORE_ALL (using BCM_COSQ_GPORT_CORE_SET)
 *               this option will map the ingress_pri (system TC)
 *               and ingress_dp (system DP) to multicast TC,
 *               shared resources eligibility, and DB service pool.
 *
 *          2. flags = BCM_COSQ_MULTICAST_SCHEDULED and
 *             gport = LOCAL / MODPORT
 *               this option maps ingress_pri and ingress_dp to
 *               multicast TC and multicast DP.
 *
 * \param [in] unit -
 *   Unit-ID
 * \param [in] gport -
 *   must be suitable to flags, see brief.
 *   valid values: CORE, LOCAL, MODPORT, or 0.
 * \param [in] ingress_pri -
 *   Priority (system TC). 3 bits.
 * \param [in] ingress_dp -
 *   Color (system DP). 2 bits.
 * \param [in] flags -
 *   BCM_COSQ_MULTICAST_UNSCHEDULED or BCM_COSQ_MULTICAST_SCHEDULED
 * \param [in] config -
 *   configuration struct of type bcm_cosq_egress_multicast_config_t.
 *   For 'gport' set to 'LOCAL' or 'MODPORT', this procedure uses only
 *   two elements:
 *     'priority' -
 *       Priority to assign to outgoing multicast traffic. Must be between 0 and
 *       DNX_EGR_NOF_Q_PRIO-1 (7).
 *     'scheduled_dp' -
 *       Drop precedence to assign to outgoing multicast traffic.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   The configuration can be set only if traffic is not enabled,
 *   or if "allow modifications during traffic" soc property is enabled.
 *   If traffic is enabled and the property is disabled - the function will fail.
 */
shr_error_e
bcm_dnx_cosq_gport_egress_multicast_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (((BCM_COSQ_GPORT_IS_CORE(gport) && BCM_COSQ_GPORT_CORE_GET(gport) == BCM_CORE_ALL) || (gport == 0)) &&
        (flags == BCM_COSQ_MULTICAST_UNSCHEDULED))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_core_multicast_config_set(unit, ingress_pri, ingress_dp, config_p));
    }
    else if ((BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_MODPORT(gport) || BCM_GPORT_IS_TRUNK(gport)) &&
             (flags & BCM_COSQ_MULTICAST_SCHEDULED))
    {
        /*
         * map system TC and system DP to MC DP and offset from base queue pair
         */
        int is_last, is_allocated;
        int old_profile, new_profile;
        dnx_cosq_egress_queue_mapping_info_t mapping_info;
        dnx_egr_q_prio_mapping_type_e map_type;
        dnx_egr_q_prio_e queue_id;
        bcm_port_t logical_port;
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /** Validate input params */
        SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_egress_multicast_config_set_verify
                              (unit, gport, ingress_pri, ingress_dp, flags, config_p));
        SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, config_p->priority, &queue_id));
        if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
        {
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            gport = logical_port;
        }
        /** get the pp port */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                        &gport_info));

        sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
        for (int port_idx = 0; port_idx < gport_info.internal_port_pp_info.nof_pp_ports; port_idx++)
        {
                /** Get Old data */
            SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get
                            (unit, gport_info.internal_port_pp_info.pp_port[port_idx],
                             gport_info.internal_port_pp_info.core_id[port_idx], &mapping_info));

                /** Change specific entry - change new tc, new dp (scheduled_tc,scheduled_dp) */
                /** Multicast-to-unscheduled type */
            map_type = DNX_EGR_MCAST_TO_UNSCHED;
            mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].dp = config_p->scheduled_dp;
            mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc = queue_id;

                /** Exchange data */
            SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange
                            (unit, gport_info.internal_port_pp_info.pp_port[port_idx],
                             gport_info.internal_port_pp_info.core_id[port_idx],
                             &mapping_info, &old_profile, &is_last, &new_profile, &is_allocated));
            if (is_allocated)
            {
                /*
                 * Create new profile 
                 */
                SHR_IF_ERR_EXIT(dnx_egr_queuing_profile_mapping_set
                                (unit, gport_info.internal_port_pp_info.core_id[port_idx], new_profile, &mapping_info));
            }
            /*
             * Map port to new profile 
             */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_profile_map_set
                            (unit, gport_info.internal_port_pp_info.core_id[port_idx],
                             gport_info.internal_port_pp_info.pp_port[port_idx], new_profile));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Invalid type-of-port. Gport input parameter is 0x%X", gport);
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/*
 * Validation procedure for bcm_dnx_cosq_gport_egress_map_get()
 * See description of IO parameters there.
 */
static shr_error_e
dnx_cosq_gport_egress_map_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t * offset_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(offset_p, _SHR_E_PARAM, "offset_p");
    if (ingress_pri < 0 || ingress_pri >= DNX_COSQ_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d", ingress_pri);
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_COSQ_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d", ingress_dp);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * \brief -
 *   Get priority ('offset' from base qpair) assigned to a gport corresponding to
 *   an egress queue.
 *     Given a gport plus priority (TC) and Drop-precedence (DP) of unicast traffic
 *     entering egress, get the priority for outgoing traffic (which is matched to
 *     a specific port, related to an egress queue).
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] gport -
 *   bcm_gport_t. Identifier of traffic by its assigned gport.
 * \param [in] ingress_pri -
 *   bcm_cos_t. Priority (TC) of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_TC-1 (7).
 * \param [in] ingress_dp -
 *   bcm_color_t. Drop precedence of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_DP-1 (3).
 * \param [in] offset_p -
 *   bcm_cos_queue_t. This procedure loads pointed memory by priority assigned to
 *   outgoing traffic. Must be between 0 and DNX_EGR_NOF_Q_PRIO-1 (7).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * HW is updated on the core associated with 'gport'.
 * \see
 *   * bcm_dnx_cosq_gport_egress_map_set
 */
shr_error_e
bcm_dnx_cosq_gport_egress_map_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t * offset_p)
{
    bcm_port_t logical_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Validate input params 
     */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_egress_map_get_verify(unit, gport, ingress_pri, ingress_dp, offset_p));
    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport) ||
        BCM_GPORT_IS_TRUNK(gport))
    {

        dnx_cosq_egress_queue_mapping_info_t mapping_info;
        dnx_egr_q_prio_mapping_type_e map_type;
        dnx_egr_q_priority_t egr_prio;

        sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
        sal_memset(&egr_prio, 0, sizeof(egr_prio));

        if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
        {
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
            gport = logical_port;
        }
        /** get the pp port */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                        &gport_info));

        /** use the first pp port of the list, even if more than one exist */
        SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get
                        (unit, gport_info.internal_port_pp_info.pp_port[0],
                         gport_info.internal_port_pp_info.core_id[0], &mapping_info));
        /** Unicast type */
        map_type = DNX_EGR_UCAST_TO_SCHED;
        *offset_p = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "Unsupported gport type input parameter (neither UCAST_EGRESS nor MCAST_EGRESS). Gport is 0x%08X",
                     gport);
    }
exit:
    SHR_FUNC_EXIT;
}
