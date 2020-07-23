/**
 * \file egq_ofp_rates.c
 *
 * Egress queuing port management functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX
/*************
 * INCLUDES  *
 */
/* { */
/*
 * Includes related to DBAL
 * {
 */
/*
 * }
 */
/*
 * Includes related to DATA
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
/*
 * }
 */
/*
 * Includes related to SWSTATE
 * {
 */
#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_egr_db_access.h>
/*
 * }
 */
/*
 * Includes - General
 * {
 */
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/cosq_egq.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/cosq/egress/egq_dbal.h>
#include <bcm_int/dnx/cosq/egress/rate_measurement.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <shared/utilex/utilex_u64.h>
/*
 * }
 */
/*
 * Includes related to ALGO
 * {
 */
#include <bcm_int/dnx/algo/cal/algo_cal.h>
/*
 * }
 */
static shr_error_e dnx_ofp_rates_port_rate_set(
    int unit,
    bcm_core_t core);

/* } */

/*************
 * DEFINES   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/**
 * \brief
 *   This function controls the enable/disable for read access into TCG (shapers
 *   for the egress TC group) credit table.
 *   This function is used in order to not cause any traffic loss.
 *   Note that disabling shapers will effect all 'TCG' ports (TC group/Qpai group)
 *   in the system.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] core -
 *   Core to apply new setup for.
 * \param [in] read_en -
 *   This indicates the value to load into 'credit table read enable', TCG_CRDT_TABLE_READ_EN.
 *   If non-zero, then HW access is enabled by writing to that field.
 *     In that case, '*org_tcg_read_en_p' is input parameter and is used to indicate whether
 *     HW access to TCG ports is to be enabled or not (depending on the values pointed
 *     by that pointer). This is controlled by HW (TCG_SPR_ENA).
 *   If zero, then HW access is disabled by loading it into 'credit table read enable',
 *     org_tcg_read_en_p.
 *     In that case, '*org_tcg_read_en_p' is output value and is used to indicate the
 *     current values of 'enable' flag on TCG ports (before applying the requested 'diaable').
 * \param [in,out] org_tcg_read_en_p -
 *   Pointer to uint32. See 'read_en' above.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None.
 * \see
 *   * dnx_ofp_rates_egq_tcg_rate_hw_get
 */
static int
dnx_ofp_rates_tcg_read_enable_set(
    int unit,
    int core,
    uint32 read_en,
    uint32 *org_tcg_read_en_p)
{
    SHR_FUNC_INIT_VARS(unit);
    if (read_en)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE,
                                         (uint32) (*org_tcg_read_en_p), GEN_DBAL_FIELD_LAST_MARK));
    }
    else
    {
        /*
         * Get the value of DBAL_FIELD_TCG_SPR_ENA from HW (This is output of this procedure)
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE, org_tcg_read_en_p,
                                         GEN_DBAL_FIELD_LAST_MARK));
        /*
         * Set the value for DBAL_FIELD_TCG_SPR_ENA as '0'
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE, 0,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    /*
     * Set the value for DBAL_FIELD_TCG_CRDT_TABLE_READ_EN from input
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TCG_CRDT_TABLE_READ_EN, INST_SINGLE,
                                     (uint32) (read_en), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function controls the enable/disable for read access into port priority (shapers
 *   for the egress Q pairs) credit table.
 *   This function is used in order to not cause any traffic loss.
 *   Note that disabling shapers will effect all 'priority' ports (qpairs) in the system.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] core -
 *   Core to apply new setup for.
 * \param [in] read_en -
 *   This indicates the value to load into 'credit table read enable', QP_CRDT_TABLE_READ_EN.
 *   If non-zero, then HW access is enabled by writing to that field.
 *     In that case, '*org_tc_read_en_p' is input parameter and is used to indicate whether
 *     HW access to priority ports is to be enabled or not (depending on the values pointed
 *     by that pointer). This is controlled by HW (QPAIR_SPR_ENA).
 *   If zero, then HW access is disabled by loading it into 'credit table read enable',
 *     QP_CRDT_TABLE_READ_EN.
 *     In that case, '*org_tc_read_en_p' is output value and is used to indicate the
 *     current values of 'enable' flag on prioroty ports (before applying the requested 'diaable').
 * \param [in,out] org_tc_read_en_p -
 *   Pointer to uint32. See 'read_en' above.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None.
 * \see
 *   * dnx_ofp_rates_egq_port_priority_rate_hw_get
 */
static int
dnx_ofp_rates_tc_read_enable_set(
    int unit,
    int core,
    uint32 read_en,
    uint32 *org_tc_read_en_p)
{
    SHR_FUNC_INIT_VARS(unit);
    if (read_en)
    {
        /*
         * Set the value for DBAL_FIELD_QPAIR_SPR_ENA from input
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE,
                                         (uint32) (*org_tc_read_en_p), GEN_DBAL_FIELD_LAST_MARK));
    }
    else
    {
        /*
         * Get the value of DBAL_FIELD_QPAIR_SPR_ENA from HW (This is output of this procedure)
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE, org_tc_read_en_p,
                                         GEN_DBAL_FIELD_LAST_MARK));
        /*
         * Set the value for DBAL_FIELD_QPAIR_SPR_ENA as '0'
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE, 0,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    /*
     * Set the value for DBAL_FIELD_QP_CRDT_TABLE_READ_EN
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QP_CRDT_TABLE_READ_EN, INST_SINGLE,
                                     (uint32) (read_en), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function enables or disables HW access of otm and interface credit table.
 *   This function will enable/disable shapers accordingly in order to not cause
 *   any traffic lost.
 *   However, disabling shaper will effect all the ports in the system
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] core -
 *   Core to apply new setup for.
 * \param [in] read_en -
 *   This indicates the value to load into 'credit table read enable', OTM_CRDT_TABLE_READ_EN.
 *   If non-zero, then HW access is enabled by writing to that field.
 *     In that case, both  '*org_if_read_en' and '*org_port_read_en' are input parameters
 *     and are used to indicate whether HW access to interface and port are to be
 *     enabled or not (depending on the values pointed by these pointers).
 *   If zero, then HW access is disabled by loading it into 'credit table read enable',
 *     OTM_CRDT_TABLE_READ_EN.
 *     In that case, both '*org_if_read_en' and '*org_port_read_en' are output values
 *     and are used to indicate the current values of 'enable' flags on
 *     interface and port.
 * \param [in,out] org_if_read_en -
 *   Pointer to uint32. See 'read_en' above.
 * \param [in,out] org_port_read_en -
 *   Pointer to uint32. See 'read_en' above.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None.
 * \see
 *   * dnx_ofp_rates_if_calendar_internal_rate_set
 */
static shr_error_e
dnx_ofp_rates_if_port_read_enable_set(
    int unit,
    int core,
    uint32 read_en,
    uint32 *org_if_read_en,
    uint32 *org_port_read_en)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * By default Enable Interface, OTM, QPair, TCG shapers
     */
    if (read_en)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 3,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INTERFACE_SPR_ENA, INST_SINGLE,
                                         (uint32) (*org_if_read_en), GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA,
                                         INST_SINGLE, (uint32) (*org_port_read_en), GEN_DBAL_FIELD32,
                                         DBAL_FIELD_OTM_CRDT_TABLE_READ_EN, INST_SINGLE, (uint32) (read_en),
                                         GEN_DBAL_FIELD_LAST_MARK));

    }
    else
    {
        /*
         * Get the values: DBAL_FIELD_INTERFACE_SPR_ENA, DBAL_FIELD_OTM_SPR_ENA
         * They are output of this procedure.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 2,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INTERFACE_SPR_ENA, INST_SINGLE, org_if_read_en,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, org_port_read_en,
                                         GEN_DBAL_FIELD_LAST_MARK));
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 3,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INTERFACE_SPR_ENA, INST_SINGLE, (uint32) (0),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE,
                                         (uint32) (*org_port_read_en), GEN_DBAL_FIELD32,
                                         DBAL_FIELD_OTM_CRDT_TABLE_READ_EN, INST_SINGLE, (uint32) (read_en),
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_if_calendar_internal_rate_set(
    int unit,
    int core,
    int calendar_id,
    uint32 internal_rate)
{
    uint32 org_if_val, org_otm_val;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }

    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_OTM_CALENDAR_CRDT_TABLE, 3, 1,
                           /** keys */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_ID, calendar_id,
                                     GEN_DBAL_RANGE32, DBAL_FIELD_CALENDAR_SET, (uint32) DBAL_RANGE_ALL,
                                     (uint32) DBAL_RANGE_ALL, GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                           /** values */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_CAL_CRDT_TO_ADD, INST_SINGLE,
                                     (uint32) (internal_rate), GEN_DBAL_FIELD_LAST_MARK));

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_if_calendar_internal_rate_get(
    int unit,
    int core,
    int calendar_id,
    uint32 *internal_rate)
{
    uint32 org_if_val, org_otm_val;
    dbal_tables_e dbal_table_id;
    uint32 cal_set;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    /*
     * get internal interface rate
     */
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_CALENDAR_CRDT_TABLE;
    /*
     * Read only from calendar set '0' since vlue for calendar set '1' must be the same as
     * value for calendar set '0'.
     */
    cal_set = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table_id,
                                     3, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_ID,
                                     calendar_id,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_SET, (uint32) (cal_set),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_CAL_CRDT_TO_ADD, INST_SINGLE, internal_rate,
                                     GEN_DBAL_FIELD_LAST_MARK));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Implementation
 */
int
dnx_ofp_rates_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_init(
    int unit)
{
    int core, idx;
    uint32 init_max_burst;
    uint32 field32_value;

    SHR_FUNC_INIT_VARS(unit);
    init_max_burst = DNX_EGQ_MAX_BURST_IN_BYTES;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.init(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.alloc(unit, core));
    }

    {
        /*
         * By default Enable Interface, OTM, QPair, TCG shapers
         * Also, Enable credit table read
         */
        field32_value = (uint32) (1);
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 4,
                                         GEN_DBAL_RANGE32, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INTERFACE_SPR_ENA, INST_SINGLE, field32_value,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, field32_value,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE, field32_value,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE, field32_value,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    {
        /*
         * Iterate over all calendars, calsets, and cores
         */
        uint32 field32_value;
        field32_value = (uint32) (dnx_data_egr_queuing.params.max_credit_number_get(unit));
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_OTM_CALENDAR_CRDT_TABLE,
                                         3, 2,
                                         GEN_DBAL_RANGE32, DBAL_FIELD_CALENDAR_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL,
                                         GEN_DBAL_RANGE32, DBAL_FIELD_CALENDAR_SET, DBAL_RANGE_ALL, DBAL_RANGE_ALL,
                                         GEN_DBAL_RANGE32, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_CAL_CRDT_TO_ADD, INST_SINGLE,
                                         field32_value, GEN_DBAL_FIELD32, DBAL_FIELD_OTM_CAL_MAX_BURST,
                                         INST_SINGLE, field32_value, GEN_DBAL_FIELD_LAST_MARK));
    }
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * update sw db with default burst size
         */
        for (idx = 0; idx < dnx_data_egr_queuing.params.nof_q_pairs_get(unit); idx++)
        {
            /*
             * otm burst
             */
            /** consider set the default inside SW state XML */
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.burst.set(unit, core, idx, init_max_burst));
        }
        /**
         * For SWSTAE, init 'valid rate' indication to invalid
         * We assume that SWSTATE initializes all its memory to '0' at init.
         * Specifically:
         *   dnx_ofp_rate_db.otm.shaping.valid
         *   dnx_ofp_rate_db.qpair.valid
         *   dnx_ofp_rate_db.tcg.valid
         */
    }
    {
        /*
         * Enable credit table read
         * This MUST be the last step to protect access to the various credit tables.
         * See, for example, dnx_ofp_rates_if_calendar_internal_rate_set().
         */
        field32_value = (uint32) (1);
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 3,
                                         GEN_DBAL_RANGE32, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QP_CRDT_TABLE_READ_EN, INST_SINGLE, field32_value,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_CRDT_TABLE_READ_EN, INST_SINGLE,
                                         field32_value, GEN_DBAL_FIELD32, DBAL_FIELD_TCG_CRDT_TABLE_READ_EN,
                                         INST_SINGLE, field32_value, GEN_DBAL_FIELD_LAST_MARK));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Function to copy the CalCal length and entries from the active to inactive
 *   calendar before switching them.
 *   This is done in case of setting BW for a channalized local port, because in
 *   this case re-calculation of the CalCal and interface shapers is not needed.
 * \param [in] unit -
 *   Int. HW identifier of unit.
 * \param [in] core -
 *   Int. Core to apply new setup for.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * All operations are done on HW.
 * \see
 *   * DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION
 *   * * DBAL_FIELD_OTM_SPR_SET_SEL
 *   * * DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_A/B
 *   * DBAL_TABLE_EGQ_SHAPER_CAL_CAL_CALENDAR
 *   * * DBAL_FIELD_CAL_INDX
 */
static shr_error_e
dnx_ofp_rates_calcal_copy(
    int unit,
    int core)
{
    uint32 idx, calendar_len, interface_select;
    dnx_ofp_rates_cal_set_e cal2set, act_cal;
    dbal_fields_e dbal_field_id;
    uint32 org_if_val, org_otm_val;

    SHR_FUNC_INIT_VARS(unit);
    calendar_len = 0;
    interface_select = 0;
    /** Get the active calendar */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /** key construction. */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /** Get the value of OTM_SPR_SET_SELf */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_SET_SEL, INST_SINGLE, &act_cal,
                                     GEN_DBAL_FIELD_LAST_MARK));

    /** Load 'cal2set' by the inactive calendar set (Stand By) */
    cal2set = ((act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A);

    /*
     * Get the active calendar's length
     * Choose the appropriate DBAL field to read based on the current active calendar
     */
    dbal_field_id =
        ((act_cal ==
          DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_A : DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_B);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /** key construction. */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /** Get selected field val */
                                     GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, &calendar_len,
                                     GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Set the calendar's length to the inactive calendar
     * Choose the appropriate DBAL field to write based on the current inactive calendar
     */
    dbal_field_id =
        ((cal2set ==
          DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_A : DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_B);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /** key construction. */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /** Set selected field val */
                                     GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, calendar_len,
                                     GEN_DBAL_FIELD_LAST_MARK));

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }

    /** Get the CalCal entries of the active calendar and copy them to the calendar2set without recalculating */
    for (idx = 0; idx < calendar_len + 1; ++idx)
    {
        /** Key construction variable part. Get interface_select from active calendar */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_CAL_CAL_CALENDAR, 3, 1,
                                         /** key construction. */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_SET_SELECT, (uint32) act_cal,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CAL_SLOT_ID, (uint32) idx,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /** Get field val for DBAL_FIELD_CAL_INDX */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CAL_INDX, INST_SINGLE, &interface_select,
                                         GEN_DBAL_FIELD_LAST_MARK));

        /** Set interface_select to the  calendar2set */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_CAL_CAL_CALENDAR, 3, 1,
                                         /** key construction. */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_SET_SELECT, (uint32) cal2set,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CAL_SLOT_ID, (uint32) idx,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /** Set field val for DBAL_FIELD_CAL_INDX */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CAL_INDX, INST_SINGLE, interface_select,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_otm_shapers_set(
    int unit,
    int core,
    uint8 if_recalc)
{
    uint32 internal_rate, calcal_length, calcal_instances, cal_rate;
    int packet_mode;
    bcm_port_t logical_port;
    bcm_pbmp_t port_bm;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);

    /** If if_recalc is TRUE need to recalculate calcal and interface shapers */
    if (if_recalc == TRUE)
    {
        /** recalculate calcal */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calcal_config(unit, core));

        /** mark that calcal is modified */
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_is_modified.set(unit, core, TRUE));

        /** Recalculate all interface shapers */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                         &port_bm));

        BCM_PBMP_ITER(port_bm, logical_port)
        {
            /*
             * From the port get the associated shaper mode (0-DATA or 1-PACKET) needed for
             * the calculation of the internal rate
             */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_packet_mode_get(unit, logical_port, &packet_mode));
            SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.get(unit, core, calendar_id, &cal_rate));

            if (cal_rate)
            {
                uint32 egq_resolution;
                /** The calculation needs to consider the amount of calcal instances and the calcal size */
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                                nof_calcal_instances.get(unit, core, calendar_id, &calcal_instances));

                /** Translate Kbps rate to internal rate (credits/cycle) */
                if (packet_mode == 0)
                {
                    /** For DATA mode. Conversion coefficient is 'credits per bit' */
                    egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
                }
                else
                {
                    /** For PACKET mode. Conversion coefficient is 'credits per packet' */
                    egq_resolution = dnx_data_egr_queuing.params.cal_res_packet_mode_get(unit);
                }
                SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_credits_from_rate
                                (unit, calcal_length, calcal_instances, 1, egq_resolution, (uint32) packet_mode,
                                 cal_rate, &internal_rate));

                /** Set interface shaper rate */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_if_calendar_internal_rate_set(unit, core, calendar_id, internal_rate));
            }
        }
    }
    else
    {
        /** Copy existing calcal */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calcal_copy(unit, core));
    }

    /** Recalculate all port shapers */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port_rate_set(unit, core));

    /** mark that all shapers are recalculated with the new calcal */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_is_modified.set(unit, core, FALSE));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ofp_rates_egq_interface_shaper_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *if_shaper_rate_p)
{
    uint32 internal_rate, calcal_length, calcal_instances;
    bcm_core_t core;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * get egress interface shaper id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    /*
     * get interface shaper internal rate
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_if_calendar_internal_rate_get(unit, core, calendar_id, &internal_rate));
    /*
     * the calculation need to consider the amount of calcal instances and the calcal size
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.nof_calcal_instances.get(unit, core, calendar_id, &calcal_instances));
    /*
     * translate from internal rate (credits) to Kbps
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_rate_from_credits
                    (unit, calcal_length, calcal_instances, 1, 0, DNX_EGQ_UNITS_VAL_IN_BITS_INTF_RECYCLE,
                     &internal_rate));
    *if_shaper_rate_p = internal_rate;
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *   Depending on 'calendar type', indicate the number of slots corresponding to
 *   that 'calendar type'.
 *   This is referred to as 'calendar size'.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] cal_info_p -
 *   Pointer to dnx_ofp_rates_cal_info_t. Only cal_info->cal_type
 *   is used here, to indicate 'calendar type'. This may be:
 *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB -
 *       'Channelized arbiter' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY -
 *       'TC/priority/qpair' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_TCG -
 *       'TCG' calendar (group of qpairs.
 * \param [out] calendar_size_p -
 *   Pointer to uint32. This procedure loads pointed memory by the
 *   the number of slots available for specified 'calendar type'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 *   * dnx_data_egr_queuing.params.calendar_size_get
 */
static shr_error_e
dnx_ofp_rates_cal_size_get(
    int unit,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    uint32 *calendar_size_p)
{
    uint32 calendar_size;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cal_info_p, _SHR_E_PARAM, "cal_info_p");
    SHR_NULL_CHECK(calendar_size_p, _SHR_E_PARAM, "calendar_size_p");
    switch (cal_info_p->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            calendar_size = DNX_OFP_RATES_CAL_SIZE_EGQ_CHAN_ARB_MAX;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            calendar_size = DNX_OFP_RATES_CAL_SIZE_EGQ_PORT_PRIO_MAX;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            calendar_size = DNX_OFP_RATES_CAL_SIZE_EGQ_TCG_MAX;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unkonown type (%d) in cal_info", cal_info_p->cal_type);
            break;
        }
    }
    *calendar_size_p = calendar_size;
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Depending on 'calendar type' and the index of the port, which is shaped by
 *   that calendar, indicate the size of allowed burst, for that port, in
 *   internal units (credits).
 *   This is referred to as 'burst size'.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use for getting required info.
 * \param [in] cal_info_p -
 *   Pointer to dnx_ofp_rates_cal_info_t. Only cal_info->cal_type
 *   is used here, to indicate 'calendar type'. This may be:
 *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB -
 *       'Channelized arbiter' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY -
 *       'TC/priority/qpair' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_TCG -
 *       'TCG' calendar (group of qpairs.
 * \param [in] ofp_index -
 *   Index (identifier) of port for which burst size is required. A 'port'
 *   is specified on each slot of the calendar. See 'dnx_egq_calendar_tbl_data_t'
 * \param [out] burst_size_p -
 *   Pointer to uint32. This procedure loads pointed memory by the
 *   the number of credits on allowed busrt for specified 'calendar type'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * We currently assume that all calendar types have the same
 *     conversion coefficient from bits/second to credits.
 * \see
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 *   * DNX_EGR_NOF_Q_PAIRS_IN_PS
 */
static shr_error_e
dnx_ofp_rates_burst_size_get(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    uint32 ofp_index,
    uint32 *burst_size_p)
{
    uint32 burst_size;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cal_info_p, _SHR_E_PARAM, "cal_info_p");
    SHR_NULL_CHECK(burst_size_p, _SHR_E_PARAM, "burst_size_p");
    switch (cal_info_p->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.burst.get(unit, core, ofp_index, &burst_size));
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            /*
             * Burst size, for 'port priority' is constant and is not controlled by the user.
             */
            burst_size = DNX_EGQ_MAX_BURST_IN_BYTES;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            /*
             * Burst size, for 'TCG' is constant and is not controlled by the user.
             */
            burst_size = DNX_EGQ_MAX_BURST_IN_BYTES;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid calendar type %u", cal_info_p->cal_type);
        }
    }
    burst_size *= dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    *burst_size_p = burst_size;
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Read calendar length.
 *   Given calendar type plus calender set, retrieve the corresponding
 *   calendar field from HW. Retrieved info depends on input parameters
 *   as follows:
 *   If 'calendar type' is 'channelized arbiter', then
 *     if 'cal_info->chan_arb_id' (denoted 'offset') is '0' then
 *       '*egq_cal_len_p' is loaded by the value, corresponding to 'offset', in
 *       DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH.
 *     Else, '*egq_cal_len_p' is loaded by the difference between the value,
 *       corresponding to 'offset' and the value corresponding to 'offset - 1'.
 *   Else
 *     '*egq_cal_len_p' is loaded by the value of the field, containing the
 *     current number of active QPAIRs/TCGs.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use when accessing HW.
 * \param [in] cal_info -
 *   Pointer to dnx_ofp_rates_cal_info_t.
 *   Both 'cal_info->cal_type' and 'cal_info->chan_arb_id' are used here.
 * \param [in] cal2set -
 *   dnx_ofp_rates_cal_set_e. May only be equal to DNX_OFP_RATES_CAL_SET_A
 *   or not (In the latter case, it is assumed to be DNX_OFP_RATES_CAL_SET_B)
 * \param [out] egq_cal_len_p -
 *   Pointer to uint32. This procedure loads pointed memory by various values
 *   depending on input, as described above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_egress_shaper_cal_write
 *   * dnx_ofp_rates_retrieve_egress_shaper_reg_field_names
 *   * dnx_ofp_rates_egress_shaper_mem_field_read
 */
static int
dnx_ofp_rates_egress_shaper_cal_len_get(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    dnx_ofp_rates_cal_set_e cal2set,
    uint32 *egq_cal_len_p)
{
    dbal_fields_e dbal_field_id;
    uint32 calendar_size;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * retrieve memory and field names
     */
    {
        uint32 field32_value;
        uint32 offset;

        switch (cal_info_p->cal_type)
        {
            case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
            {
                dbal_field_id =
                    (cal2set ==
                     DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_PORT_OFFSET_HIGH_A : DBAL_FIELD_PORT_OFFSET_HIGH_B;
                offset = cal_info_p->chan_arb_id;
                SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH,
                                                 2, 1,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_ID, (uint32) (offset),
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                                 GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, egq_cal_len_p,
                                                 GEN_DBAL_FIELD_LAST_MARK));
                *egq_cal_len_p = (1 + *egq_cal_len_p);
                if (offset)
                {
                    /*
                     * New key construction.
                     */
                    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH,
                                                     2, 1,
                                                     GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_ID, (uint32) (offset - 1),
                                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                                     GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, &field32_value,
                                                     GEN_DBAL_FIELD_LAST_MARK));
                    *egq_cal_len_p -= (field32_value + 1);
                }
                break;
            }
            case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
            {
                dbal_field_id =
                    (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_NUM_OF_QP_SPR_A : DBAL_FIELD_NUM_OF_QP_SPR_B;
                SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                                 1, 1,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                                 GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, egq_cal_len_p,
                                                 GEN_DBAL_FIELD_LAST_MARK));
                (*egq_cal_len_p)++;
                break;
            }
            case DNX_OFP_RATES_EGQ_CAL_TCG:
            {
                dbal_field_id =
                    (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_NUM_OF_TCG_SPR_A : DBAL_FIELD_NUM_OF_TCG_SPR_B;
                SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                                 1, 1,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                                 GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, egq_cal_len_p,
                                                 GEN_DBAL_FIELD_LAST_MARK));
                (*egq_cal_len_p)++;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal enum for calendar type (%d). Should be between %d and %d",
                             cal_info_p->cal_type, FIRST_DNX_OFP_RATES_EGQ_CAL_TYPE,
                             NUM_DNX_OFP_RATES_EGQ_CAL_TYPE - 1);
            }
        }
    }
    /*
     * Read from HW
     */
    /*
     * Note that calendars, in this system, are not, necessarily, of the same maximal size ('calendar_size').
     * Verify that nothing has been broken here.
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_cal_size_get(unit, cal_info_p, &calendar_size));
    if (*egq_cal_len_p > calendar_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal cal length, %d (greater than mximum allowed, %d)\n",
                     *egq_cal_len_p, calendar_size);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Set calendar length.
 *   Given calendar type plus calender set, set the calendar field,
 *   corresponding to calendar length, into HW. Stored info depends on
 *   input parameters as follows:
 *   If 'calendar type' is 'channelized arbiter', then
 *     If 'cal_info->chan_arb_id' (denoted 'offset') is '0' then
 *       'cal_info_p->arb_slot_id' is considered as the first slot of all
 *       slots handled by the arbiter at 'cal_info_p->chan_arb_id' (= 'offset').
 *       Then, the last slot number (calculated by adding the first slot to
 *       'egq_cal_len') is loaded into the HW table (DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH)
 *       at 'offset'. Updated field is DBAL_FIELD_PORT_OFFSET_HIGH_A/B (depending
 *       on 'cal2set').
 *     Else, the first slot ('cal_info_p->arb_slot_id') is loaded into
 *       'offset - 1' and the last slot is loaded into 'offset - 1'.
 *   Else
 *     'egq_cal_len' is loaded into field 'DBAL_FIELD_NUM_OF_QP_SPR_A/B' or
 *     'DBAL_FIELD_NUM_OF_TCG_SPR_A' (depending on 'calendar type' and 'cal2set'),
 *     This is the field containing the current number of active QPAIRs/TCGs.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use when accessing HW.
 * \param [in] cal_info -
 *   Pointer to dnx_ofp_rates_cal_info_t.
 *   Both 'cal_type', 'chan_arb_id' and 'arb_slot_id' are used here. See description
 *   above.
 * \param [in] cal2set -
 *   dnx_ofp_rates_cal_set_e. This is the selected 'calendar set' (active or stand-by).
 *   May only be equal to DNX_OFP_RATES_CAL_SET_A or not (In the latter case, it is
 *   assumed to be DNX_OFP_RATES_CAL_SET_B).
 * \param [in] egq_cal_len -
 *   uint32. This is the length of the calendar used, as described above, to calculate
 *   the value to load into HW.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 *   * dnx_ofp_rates_egress_shaper_cal_len_get
 */
static int
dnx_ofp_rates_egress_shaper_cal_len_set(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    dnx_ofp_rates_cal_set_e cal2set,
    uint32 egq_cal_len)
{
    uint32 field32_value;
    uint32 offset;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Update calendar length for selected, non-active, set
     */
    /*
     *   Depending on calendar type (cal_info->cal_type) AND calendar set (cal2set),
     *   get the DBAL table and DBAL field corresponding to the length of the calendar
     *   and update them (in HW). Note that the actual HW update procedure depends,
     *   also, on 'calendar type: For 'channelized calendar', other elements of
     *   'cal_info' are used (see below). Otherwise, the length is set as per 'egq_cal_len'
     *   below (actually, 'egq_cal_len-1')
     *
     *   For 'channelized calendar', DNX_OFP_RATES_EGQ_CAL_CHAN_ARB, the following applies:
     *     cal_info->chan_arb_id (used to calculate 'offest', key into table).
     *     cal_info->arb_slot_id (used to calculate 'result', to load into table).
     */
    /*
     * Retrieve memory and field names and, then, write.
     */
    switch (cal_info_p->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            dbal_field_id =
                (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_PORT_OFFSET_HIGH_A : DBAL_FIELD_PORT_OFFSET_HIGH_B;
            offset = cal_info_p->chan_arb_id;
            if (offset)
            {
                field32_value = cal_info_p->arb_slot_id - 1;
                SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH,
                                                 2, 1,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_ID, (uint32) (offset - 1),
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                                 GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, field32_value,
                                                 GEN_DBAL_FIELD_LAST_MARK));
            }
            /*
             * New key construction and new value.
             */
            field32_value = egq_cal_len - 1 + cal_info_p->arb_slot_id;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH,
                                             2, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_ID, (uint32) (offset),
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, field32_value,
                                             GEN_DBAL_FIELD_LAST_MARK));
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            dbal_field_id =
                (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_NUM_OF_QP_SPR_A : DBAL_FIELD_NUM_OF_QP_SPR_B;
            field32_value = egq_cal_len - 1;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                             1, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, field32_value,
                                             GEN_DBAL_FIELD_LAST_MARK));
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            dbal_field_id =
                (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_NUM_OF_TCG_SPR_A : DBAL_FIELD_NUM_OF_TCG_SPR_B;
            field32_value = egq_cal_len - 1;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                             1, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, field32_value,
                                             GEN_DBAL_FIELD_LAST_MARK));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal enum for calendar type (%d). Should be between %d and %d",
                         cal_info_p->cal_type, FIRST_DNX_OFP_RATES_EGQ_CAL_TYPE, NUM_DNX_OFP_RATES_EGQ_CAL_TYPE - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Depending on calendar type, use corresponding DBAL field to get
 *   HW value of the flag that is used to select shapers set (A or B).
 *   This is referred to as the 'active' set.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use for HW access.
 * \param [in] cal_type -
 *   dnx_ofp_rates_egq_cal_type_e. Type of calendar to use.
 *   May be:
 *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB      (Field: DBAL_FIELD_OTM_SPR_SET_SEL)
 *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY (Field: DBAL_FIELD_QPAIR_SPR_SET_SEL)
 *     DNX_OFP_RATES_EGQ_CAL_TCG           (Field: DBAL_FIELD_TCG_SPR_SET_SEL)
 * \param [out] set_select_p -
 *   Pointer to enum dbal_fields_e. This procedure loads pointed
 *   memory by the value of the HW flag that is used to select
 *   shaper set.
 *   Options are:
 *     DNX_OFP_RATES_CAL_SET_A
 *     DNX_OFP_RATES_CAL_SET_B
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 */
static shr_error_e
dnx_ofp_rates_set_select_get(
    int unit,
    int core,
    dnx_ofp_rates_egq_cal_type_e cal_type,
    uint32 *set_select_p)
{
    dbal_fields_e field_cal_set;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(set_select_p, _SHR_E_PARAM, "set_select_p");
    /*
     * Check which DBAL field is used to select the calendar (EGQ & SCH - Calendars
     * get 'A' or 'B') that is active.
     */
    switch (cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            field_cal_set = DBAL_FIELD_OTM_SPR_SET_SEL;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            field_cal_set = DBAL_FIELD_QPAIR_SPR_SET_SEL;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            field_cal_set = DBAL_FIELD_TCG_SPR_SET_SEL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unkonown type (%d) in cal_info", cal_type);
            break;
        }
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, field_cal_set, INST_SINGLE, set_select_p,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *   This procedure relates to calendars which have a 'double' structure (two
 *   calendars, one active and one 'stand-by'):
 *   Given 'calendar type' (channelized/priority/TCG) and offset (slot) into
 *   corresponding calendar table, get the information related to that entry:
 *   (a) Offset into corresponding 'port' table (OTM/qpair/TCG).
 *   (b) Number of credits to assign to that port.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] cal_type -
 *   dnx_ofp_rates_egq_cal_type_e.
 *     Indicated 'calendar type'. This may be:
 *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB -
 *       'Channelized arbiter' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY -
 *       'TC/priority/qpair' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_TCG -
 *       'TCG' calendar (group of qpairs).
 * \param [in] cal_set -
 *   dnx_ofp_rates_cal_set_e. Calendar set to use when accessing HW table.
 *   May only be equal to DNX_OFP_RATES_CAL_SET_A or not (In the latter case,
 *   it is assumed to be DNX_OFP_RATES_CAL_SET_B)
 * \param [in] entry_offset -
 *   uint32. Slot. Index of entry to retrieve from specified 'double' calendar table
 *   This is the index within the selected 'cal_set'
 * \param [out] egq_calendar_tbl_data_p -
 *   Pointer to dnx_egq_calendar_tbl_data_t. This procedure loads pointed memory by the
 *   the information on indicated slot: Index of 'port' on selected slot and
 *   Number of credits to assign to it.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_full_calendar_get
 *   * dnx_ofp_rates_calendar_entry_set
 */
static int
dnx_ofp_rates_calendar_entry_get(
    int unit,
    int core,
    dnx_ofp_rates_egq_cal_type_e cal_type,
    dnx_ofp_rates_cal_set_e cal_set,
    uint32 entry_offset,
    dnx_egq_calendar_tbl_data_t * egq_calendar_tbl_data_p)
{
    dbal_tables_e egq_credit_table_name;
    dbal_fields_e egq_cr_field_name, egq_index_field_name, egq_burst_field_name;
    dbal_fields_e calendar_field_name;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(egq_calendar_tbl_data_p, _SHR_E_PARAM, "egq_calendar_tbl_data_p");
    egq_calendar_tbl_data_p->port_cr_to_add = 0;
    egq_calendar_tbl_data_p->ofp_index = 0;
    switch (cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            egq_credit_table_name = DBAL_TABLE_EGQ_SHAPING_OTM_CRDT_TABLE;
            egq_cr_field_name = DBAL_FIELD_OTM_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_OTM_INDEX;
            calendar_field_name = DBAL_FIELD_CHAN_ARB_CALENDAR;
            egq_burst_field_name = DBAL_FIELD_OTM_MAX_BURST;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            egq_credit_table_name = DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_QPAIR_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_QPAIR_INDEX;
            calendar_field_name = DBAL_FIELD_PORT_PRIO_CALENDAR;
            egq_burst_field_name = DBAL_FIELD_QPAIR_MAX_BURST;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            egq_credit_table_name = DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_TCG_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_TCG_INDX;
            calendar_field_name = DBAL_FIELD_TCG_CALENDAR;
            egq_burst_field_name = DBAL_FIELD_TCG_MAX_BURST;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unknown/illegal cal_type (%d)", cal_type);

        }
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, egq_credit_table_name,
                                     3, 3,
                                     GEN_DBAL_FIELD32, calendar_field_name, (uint32) (entry_offset),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_SET, cal_set,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, egq_cr_field_name, INST_SINGLE,
                                     &(egq_calendar_tbl_data_p->port_cr_to_add), GEN_DBAL_FIELD32, egq_index_field_name,
                                     INST_SINGLE, &(egq_calendar_tbl_data_p->ofp_index), GEN_DBAL_FIELD32,
                                     egq_burst_field_name, INST_SINGLE, &(egq_calendar_tbl_data_p->port_max_credit),
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   This procedure relates to all calendar types but is most meaningful for CHAN_ARB:
 *   Given 'calendar type' (channelized/priority/TCG) and offset (slot) into
 *   corresponding calendar table, get the information related to that entry:
 *   (a) Offset into corresponding 'port' table (OTM/qpair/TCG).
 *   (b) Number of credits to assign to that port.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] cal_info_p -
 *   Pointer to structure of type dnx_ofp_rates_cal_info_t.
 *     cal_type - (dnx_ofp_rates_egq_cal_type_e).
 *       Indicated 'calendar type'. This may be:
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB -
 *         'Channelized arbiter' calendar.
 *       DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY -
 *         'TC/priority/qpair' calendar.
 *       DNX_OFP_RATES_EGQ_CAL_TCG -
 *         'TCG' calendar (group of qpairs).
 *     The following elements are used for CHAN_ARB only:
 *     chan_arb_id -
 *       Index of the arbiter (interface) that is used for this 'channelized'
 *       calendar.
 *     arb_slot_id -
 *       Not used.
 * \param [in] cal_set -
 *   dnx_ofp_rates_cal_set_e. Calendar set to use when accessing HW table.
 *   May only be equal to either DNX_OFP_RATES_CAL_SET_A or DNX_OFP_RATES_CAL_SET_B.
 * \param [out] egq_calendar_tbl_data_p -
 *   Pointer to uint32. This procedure loads pointed memory by the offset of the
 *   indicated interface within the section of the calendar table which is assigned
 *   to specified calendar sedt.
 *   This value is '0' for all calendar types except for CHAN_ARB.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 */
static int
dnx_ofp_rates_calendar_offset_get(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    dnx_ofp_rates_cal_set_e cal_set,
    uint32 *offset_of_calendar_start_p)
{
    uint32 offset;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Just making sure some value is loaded even if error is encountered below.
     * Not required but good practice.
     */
    *offset_of_calendar_start_p = 0;
    if ((cal_info_p->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB) && (cal_info_p->chan_arb_id > 0))
    {
        uint32 dbal_field_val;

        dbal_field_id =
            ((cal_set == DNX_OFP_RATES_CAL_SET_B) ? DBAL_FIELD_PORT_OFFSET_HIGH_B : DBAL_FIELD_PORT_OFFSET_HIGH_A);
        dbal_field_val = 0;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH, 2, 1,
                                         /*
                                          * key construction.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_ID,
                                         (uint32) (cal_info_p->chan_arb_id - 1), GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID,
                                         core,
                                         /*
                                          * Get the offset value
                                          */
                                         GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, &dbal_field_val,
                                         GEN_DBAL_FIELD_LAST_MARK));
        offset = (1 + dbal_field_val);
    }
    else
    {
        offset = 0;
    }
    *offset_of_calendar_start_p = offset;
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   This procedure relates to calendars which have a 'double' structure (two
 *   calendars, one active and one 'stand-by'):
 *   Given 'calendar type' (channelized/priority/TCG) and offset (slot) into
 *   corresponding calendar table, get the information related to that entry:
 *   (a) Offset into corresponding 'port' table (OTM/qpair/TCG).
 *   (b) Number of credits to assign to that port.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] cal_type -
 *   dnx_ofp_rates_egq_cal_type_e.
 *     Indicated 'calendar type'. This may be:
 *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB -
 *       'Channelized arbiter' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY -
 *       'TC/priority/qpair' calendar.
 *     DNX_OFP_RATES_EGQ_CAL_TCG -
 *       'TCG' calendar (group of qpairs).
 * \param [in] cal_set -
 *   dnx_ofp_rates_cal_set_e. Calendar set to use when accessing HW table.
 *   May only be equal to DNX_OFP_RATES_CAL_SET_A or not (In the latter case,
 *   it is assumed to be DNX_OFP_RATES_CAL_SET_B)
 * \param [in] entry_offset -
 *   uint32. Slot. Index of entry to retrieve from specified 'double' calendar table.
 *   This is the index within the selected 'cal_set'
 * \param [in] egq_calendar_tbl_data_p -
 *   Pointer to dnx_egq_calendar_tbl_data_t. This procedure loads selected calendar
 *   at indicated slot (entry_offset) by: Index of 'port' on selected slot and
 *   Number of credits to assign to it.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If calndar table is 'channelized' then update both 'high priority' and 'low
 *     priotrity' tables. 'Low priority' gets twice the number of credits as 'high
 *     priority'
 * \see
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 *   * dnx_ofp_rates_calendar_entry_get
 */
static shr_error_e
dnx_ofp_rates_calendar_entry_set(
    int unit,
    int core,
    dnx_ofp_rates_egq_cal_type_e cal_type,
    dnx_ofp_rates_cal_set_e cal_set,
    uint32 entry_offset,
    dnx_egq_calendar_tbl_data_t * egq_calendar_tbl_data_p)
{
    dbal_tables_e egq_credit_table_name;
    dbal_fields_e egq_cr_field_name, egq_index_field_name, egq_burst_field_name;
    dbal_fields_e calendar_field_name;
    uint32 max_burst, max_burst_lp;

    SHR_FUNC_INIT_VARS(unit);
    switch (cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            /**  Go to the correct table, per chan arb id */
            egq_credit_table_name = DBAL_TABLE_EGQ_SHAPING_OTM_CRDT_TABLE;
            egq_cr_field_name = DBAL_FIELD_OTM_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_OTM_INDEX;
            calendar_field_name = DBAL_FIELD_CHAN_ARB_CALENDAR;
            egq_burst_field_name = DBAL_FIELD_OTM_MAX_BURST;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            egq_credit_table_name = DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_QPAIR_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_QPAIR_INDEX;
            calendar_field_name = DBAL_FIELD_PORT_PRIO_CALENDAR;
            egq_burst_field_name = DBAL_FIELD_QPAIR_MAX_BURST;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            egq_credit_table_name = DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_TCG_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_TCG_INDX;
            calendar_field_name = DBAL_FIELD_TCG_CALENDAR;
            egq_burst_field_name = DBAL_FIELD_TCG_MAX_BURST;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unknown/illegal cal_type (%d)", cal_type);
        }
    }

    /*
     * There are 2 memories for OTM shaper. burst size at LP memory need to by multipled by 2
     * On top of that devices which supporting small burst size need to
     * be configured with burst size + 1 for OTM calendar
     */
    max_burst = (uint32) (egq_calendar_tbl_data_p->port_max_credit);
    if (egq_credit_table_name == DBAL_TABLE_EGQ_SHAPING_OTM_CRDT_TABLE)
    {
        if (dnx_data_egr_queuing.params.burst_size_below_mtu_get(unit)
            && egq_calendar_tbl_data_p->port_max_credit <
            DNX_EGQ_MAX_BURST_IN_BYTES * dnx_data_egr_queuing.params.cal_burst_res_get(unit))
        {
            max_burst_lp = (uint32) (egq_calendar_tbl_data_p->port_max_credit) * 2 + 1;
            max_burst = (uint32) (egq_calendar_tbl_data_p->port_max_credit) + 1;
        }
        else
        {
            max_burst_lp = (uint32) (egq_calendar_tbl_data_p->port_max_credit) * 2;
        }
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, egq_credit_table_name, 3, 1,
                                         /** key construction. */
                                         GEN_DBAL_FIELD32, calendar_field_name, (uint32) (entry_offset),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_SET, cal_set,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /** Set the values for credits to add and corresponding index */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_LP_MAX_BURST, INST_SINGLE,
                                         max_burst_lp, GEN_DBAL_FIELD_LAST_MARK));
    }

    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, egq_credit_table_name, 3, 3,
                                     /** key construction. */
                                     GEN_DBAL_FIELD32, calendar_field_name, (uint32) (entry_offset),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CALENDAR_SET, cal_set,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /** Set the values for credits to add and corresponding index */
                                     GEN_DBAL_FIELD32, egq_cr_field_name, INST_SINGLE,
                                     (uint32) (egq_calendar_tbl_data_p->port_cr_to_add), GEN_DBAL_FIELD32,
                                     egq_index_field_name, INST_SINGLE, (uint32) (egq_calendar_tbl_data_p->ofp_index),
                                     GEN_DBAL_FIELD32, egq_burst_field_name, INST_SINGLE,
                                     max_burst, GEN_DBAL_FIELD_LAST_MARK));

    /*
     * Note that the LP table, EPS_OTM_LP_CRDT_TABLEm, gets twice as must on OTM_LP_CRDT_TO_ADD
     * as OTM_HP_CRDT_TO_ADD. This is done using DBAL encoding.
     * See hl_tm_egq_definition.xml under 'EGQ_SHAPING_OTM_CRDT_TABLE'
     * For OTM_LP_INDEX, the value is the same as OTM_HP_INDEX.
     */
exit:
    SHR_FUNC_EXIT;
}
/*
 *  Note: rates_table is only used to get the per-port shaper (max burst)
 */
/*
 * \brief -
 *   This procedure relates to 'active' calendar.
 *   Given 'calendar type' (channelized/priority/TCG) get all its entries.
 *   This functions reads all calendar values from the device (HW).
 *   For each entry, it gets
 *   (a) The corresponding 'base qpair'.
 *   (b) Number of credits to assign to that 'base qpair'.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use for HW access.
 * \param [in] cal_info_p -
 *   Pointer to structure of type dnx_ofp_rates_cal_info_t.
 *     cal_type - (dnx_ofp_rates_egq_cal_type_e).
 *       Indicated 'calendar type'. This may be:
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB -
 *         'Channelized arbiter' calendar.
 *       DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY -
 *         'TC/priority/qpair' calendar.
 *       DNX_OFP_RATES_EGQ_CAL_TCG -
 *         'TCG' calendar (group of qpairs).
 *     The following elements are used for CHAN_ARB only:
 *     chan_arb_id -
 *       Index of the arbiter that is used for this 'channelized'
 *       calendar.
 *     arb_slot_id -
 *       The first of the slots which are handled by 'chan_arb_id'.
 * \param [out] egq_cal_p -
 *   Pointer to an array of 'dnx_ofp_rates_cal_egq_t'. This procedure loads
 *   elements (slots) on this array by info related to that element:
 *   (a) Corresponding 'qpair'
 *   (b) Number of credits assigned to that 'qpair'
 *   The number of elements (slots) loaded by meaningful info is '*egq_cal_len_p'.
 * \param [out] egq_cal_len_p -
 *   Pointer to uint32. See 'egq_cal_p' above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_egq_single_port_rate_hw_get
 */
static int
dnx_ofp_rates_full_calendar_get(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    dnx_ofp_rates_cal_egq_t * egq_cal_p,
    uint32 *egq_cal_len_p)
{
    uint32 offset, slot;
    dnx_egq_calendar_tbl_data_t egq_data;
    uint32 cal2get;
    dnx_ofp_egq_rates_cal_entry_t *cal_slot;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cal_info_p, _SHR_E_PARAM, "cal_info_p");
    SHR_NULL_CHECK(egq_cal_p, _SHR_E_PARAM, "egq_cal_p");
    SHR_NULL_CHECK(egq_cal_len_p, _SHR_E_PARAM, "egq_cal_len_p");
    cal_slot = NULL;
    *egq_cal_len_p = 0;
    /*
     * Get the 'active' calendar set.
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_set_select_get(unit, core, cal_info_p->cal_type, &cal2get));
    /*
     * Read the Active EGQ calendar indirectly
     */
    /*
     * Read calendar length, corresponding to specified calendar id (cal_info_p->chan_arb_id).
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_len_get(unit, core, cal_info_p, cal2get, egq_cal_len_p));
    /*
     * EGQ tables are double in size -
     * The second half is for get 'B' calendars/shapers.
     */
    /*
     * Only meaningful for CHAN_ARB:
     *   Get 'offset' of calendar related to calendar id (cal_info_p->chan_arb_id)
     * For all others, just load 'offset' by '0'
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_offset_get(unit, core, cal_info_p, cal2get, &offset));
    /*
     * Copy info on all slots (ports) corresponding to specified calendar id (cal_info_p->chan_arb_id)
     * on the currently active 'calendar set' (cal2get).
     */
    for (slot = 0; slot < *egq_cal_len_p; ++slot)
    {
        cal_slot = &(egq_cal_p->slots[slot]);
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_entry_get
                        (unit, core, cal_info_p->cal_type, cal2get, offset + slot, &egq_data));
        cal_slot->credit = egq_data.port_cr_to_add;
        cal_slot->base_q_pair = egq_data.ofp_index;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *   This procedure has two modes of operation:
 *   If 'recalc'
 *     Given calendar type, rate per each active port, and length of calendar, get
 *     the corresponding calendar with 'calendar_len' slots (but see 'add_dummy_tail').
 *   else
 *     Given 'calendar type' (cal_info->cal_type) and 'interface' (Which is only relevant
 *     for 'channelized calendar', DNX_OFP_RATES_EGQ_CAL_CHAN_ARB and which is stored in
 *     'cal_info->chan_arb_id'), get the number of entries in the calendar (in '*calendar_len_p')
 *     and load info on all slots into '*calendar_p'. All other input parameters are not relevant,
 *     in this case.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use for getting required info.
 * \param [in] cal_info_p -
 *   Pointer to dnx_ofp_rates_cal_info_t. The following elements are input:
 *     cal_info->cal_type (dnx_ofp_rates_egq_cal_type_e) - May be:
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *       All other calendar types
 *   The following is also used,  but only for 'channelized calendar' (CAL_CHAN_ARB):
 *     cal_info->chan_arb_id (= 'calendar id'. Used to calculate 'offest', key into table).
 * \param [in] ports_rates_p -
 *   Only relevant when 'recalc == 0"
 *   Pointer to array of uint32. Dimension of array is dnx_data_egr_queuing.params.nof_q_pairs
 *   (currently 512). Each entry indicates the rate assigned to indicated port, in kilo
 *   bits per second. A port, whose rate is '0', is assumed to be inactive.
 * \param [in] nof_ports -
 *   Only relevant when 'recalc == 0"
 *   uint32. Number of slots in calendar. Each slot corresponds to one entry in calendar_len_p->slots[].
 *   This is, then, the number of output entries on calendar_len_p->slots[].
 * \param [in] recalc -
 *   uint32. Flag. Indicates whether requested calendar is to simply be read from HW or
 *   be recalculated. See description of usage above.
 * \param [in,out] calendar_p -
 *   Pointer to dnx_ofp_rates_cal_egq_t.
 *   If 'recalc == 0' then
 *     The array 'egq_cal_p->slots[]' is loaded by this procedure by 'credit' and 'base_q_pair'
 *     per each slot. Caller is responsoble to set 'calendar' with enough space to accommodate
 *     '*calendar_len_p' slots.
 *   Else
 *     Of these two parameters (of each 'egq_cal_p->slots[]'), 'base qpair' is INPUT and 'credit' is
 *     OUTPUT of this procedure.
 *     Caller is responsoble to set 'calendar' with enough space to accommodate 'nof_ports'
 *     slots.
 *     Each slot is represented, then, by the two parameters: 'base qpair' (= port index) and its
 *     corresponding number of 'credit's that the system is supposed to supply to that port when
 *     its turn comes.
 * \param [out] calendar_len_p -
 *   Pointer to uint32.
 *   This procedure loads pointed memory by the size of the calendar as output on *calendar_p
 *   The loaded value is always equal to the input parameter: 'nof_ports'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar
 */
static int
dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    uint32 *ports_rates_p,
    uint32 nof_ports,
    uint32 recalc,
    dnx_ofp_rates_cal_egq_t * calendar_p,
    uint32 *calendar_len_p)
{
    uint32 loc_calendar_len = nof_ports;

    SHR_FUNC_INIT_VARS(unit);

    if (!recalc)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_full_calendar_get(unit, core, cal_info_p, calendar_p, &loc_calendar_len));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_ofp_rates_fill_shaper_generic_calendar_credits(unit,
                                                                                core,
                                                                                cal_info_p,
                                                                                (uint32 *) ports_rates_p,
                                                                                loc_calendar_len, FALSE, calendar_p));
    }
    *calendar_len_p = nof_ports;
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_calculate_rate_from_credits(
    int unit,
    uint32 calcal_length,
    uint32 calcal_instances,
    uint32 calendar_len,
    int packet_mode,
    uint32 egq_resolution,
    uint32 *port_rate_p)
{
    uint32 calc;
    uint32 multiplier;
    uint64 u64_1, u64_2, u64_3, u64_4;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(port_rate_p, _SHR_E_PARAM, "port_rate_p");

    if (packet_mode == 0)
    {
        /*
         * For DATA mode the rate units are Kbps and therefore it does not need to be changed (multiplied by factor of 1).
         * This is because device_ticks_per_sec is in Kilo Hz and they need to be at the same units.
         */
        multiplier = 1;
    }
    else
    {
        /*
         * For PACKET mode the rate units are PPS (packets per sec) and needs to be multiplied by factor of 1000
         * This is because device_ticks_per_sec is in Kilo Hz and they (rate and ticks) need to be at the same units.
         */
        multiplier = 1000;
    }

    /** If input rate is '0' then set output to be '0' as well. */
    if (port_rate_p[0] != 0)
    {
        /*
         *  Calculate shaper rate according to:
         *
         *
         *  port_egq_rate [kbits/sec or packets/sec] =
         *
         *                 numInstances [slot/calCycle] * total_port_credits [credits/calCycle] * core_frequency [kilo-cycle/sec] * multiplier
         *   -----------------------------------------------------------------------------------------------------------------------------------
         *      calCalLen [slot/calCycle] * calendar_length [slot/calCycle] * egq_resolution [credits/(8*bit)] * slot_traverse_time [cycle/slot]
 */
        COMPILER_64_SET(u64_1, 0, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
        COMPILER_64_UMUL_32(u64_1, multiplier);
        COMPILER_64_UMUL_32(u64_1, (calcal_instances * port_rate_p[0]));
        COMPILER_64_SET(u64_2, 0,
                        (calcal_length * calendar_len * egq_resolution * DNX_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS));
        /**
         * The division to calculate the port rate must be rounded up. In order to do that we
         * do the following calculation: (divident + (divisor-1))/divident = rounded_up_value
         */
        COMPILER_64_COPY(u64_4, u64_2);
        COMPILER_64_SUB_32(u64_4, 1);
        COMPILER_64_ADD_64(u64_1, u64_4);
        COMPILER_64_UDIV_64(u64_1, u64_2);
        /*
         * The result of the division is in 'u64_1'. Put the smallest 'real' 64 bits value in 'u64_3'.
         * The result of the division is assumed to be expressible by 'unsigned long'. If not, return max possible value
         */
        COMPILER_64_SET(u64_3, 1, 0);
        if (COMPILER_64_GE(u64_1, u64_3))
        {
            calc = UTILEX_UINT_MAX;
        }
        else
        {
            COMPILER_64_TO_32_LO(calc, u64_1);
        }

        port_rate_p[0] = calc;
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_calculate_credits_from_rate(
    int unit,
    uint32 calcal_length,
    uint32 calcal_instances,
    uint32 calendar_len,
    uint32 egq_resolution,
    uint32 packet_mode,
    uint32 rate_kbps,
    uint32 *port_credits_p)
{
    uint32 calc, divisor;
    uint64 u64_1, u64_2, u64_3, u64_4;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(port_credits_p, _SHR_E_PARAM, "port_credits_p");

    /** If specified input rate is '0' then this is also the output 'credits' */
    if (rate_kbps == 0)
    {
        *port_credits_p = 0;
        SHR_EXIT();
    }

    if (packet_mode == 0)
    {
        /*
         * For DATA mode the rate is received at Kbps and therefore it does not need to be changed (divided by factor of 1).
         * This is because device_ticks_per_sec is in Kilo Hz and they need to be at the same units.
         */
        divisor = 1;
    }
    else
    {
        /*
         * For PACKET mode the rate is received at packets per sec and needs to be divided by factor of 1000
         * This is because device_ticks_per_sec is in Kilo Hz and they (rate and ticks) need to be at the same units.
         */
        divisor = 1000;
    }
    /*
     *  Calculate shaper credits according to:
     *
     *
     * total_port_credits [credits/calCycle] =
     *
     *   port_egq_rate [kbits/sec] * calcal_length [slot/calCycle] * calendar_length [slot/calCycle] * egq_resolution [credits/(8*bit)] * slot_traverse_time [cycle/slot]
     *  -------------------------------------------------------------------------------------------------------------------------------------------------------------------
     *                             divisor * calcal_instances [slot/calCycle] *  core_frequency [kilo-cycle/sec]
     */
    COMPILER_64_SET(u64_1, 0, (rate_kbps));
    COMPILER_64_UMUL_32(u64_1, (calcal_length * calendar_len * egq_resolution * DNX_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS));
    COMPILER_64_COPY(u64_4, u64_1);
    COMPILER_64_SET(u64_2, 0, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
    COMPILER_64_UMUL_32(u64_2, (divisor * calcal_instances));
    COMPILER_64_UDIV_64(u64_1, u64_2);
    /*
     * The result of the division is in 'u64_1'. Put the smallest 'real' 64 bits value in 'u64_3'.
     * The result of the division is assumed to be expressible by 'unsigned long'. If not, flag 'overflow'
     */
    COMPILER_64_SET(u64_3, 1, 0);
    if (COMPILER_64_GE(u64_1, u64_3))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected overflow. Result is larger than uint32: " COMPILER_64_PRINTF_FORMAT ".",
                     COMPILER_64_PRINTF_VALUE(u64_1));
    }
    COMPILER_64_TO_32_LO(calc, u64_1);
    /*
     * Now load 'u64_2' by 'rounded down' multiplication result.
     */
    COMPILER_64_UMUL_32(u64_2, calc);
    /*
     * Subtract the 'rounded down' value from the original multiplication result, to get the remainder.
     */
    COMPILER_64_SUB_64(u64_4, u64_2);
    if (!COMPILER_64_IS_ZERO(u64_4))
    {
        calc += 1;
    }
    /*
     * If the rate desired is smaller than the shaper resolution, 1 credit can be allocated.
     */
    if (calc == 0)
    {
        calc = 1;
    }
    *port_credits_p = calc;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given calendar type and number of credits, per port, assign,
 *   to each port, the corresponding rate (in kilo bits per sec or PPS).
 * \param [in] unit -   Identifier of HW platform.
 *
 * \param [in] core -   Core to use to get internal input parameters (calCal lenght, no. of instances
 *                      on calCal, core frequency).
 *
 * \param [in] cal_info_p - Pointer to dnx_ofp_rates_cal_info_t.
 *                          Only 'cal_info_p->cal_type' and 'cal_info_p->chan_arb_id' are used here.
 *                          'cal_type' may be:
 *                              DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *                              All others
 *                          'chan_arb_id' is only used for (cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB) and
 *                          it indicates the calendar id corresponding to inquired ports.
 *
 * \param [in] calendar_p - Pointer to dnx_ofp_rates_cal_egq_t which contains an array of type
 *                          dnx_ofp_egq_rates_cal_entry_t, of 'calendar_len' elements. Each element has:
 *                              Indicator of the port (base_q_pair)
 *                              Number of credits assigned to this port (credit)
 *
 * \param [in] calendar_len - uint32. Number of elements in 'slots[]'. See 'calendar_p' above.
 *
 * \param [in] nof_ports -  uint32. This is just the maximal number of ports and is only used to verify
 *                          'specific_port' is within range.
 *
 * \param [in] packet_mode -    int. indication that the calculation should take into account that this port is in packet
 *                              mode and not in data mode, this is relevant only for
 *                              DNX_OFP_RATES_EGQ_CAL_CHAN_ARB calendar type
 *
 * \param [in] specific_port - int32. Calculate 'rate' for 'specific_port' ONLY.
 *
 * \param [out] ports_rates -   Pointer tp uint32. this is an array of one element which is loaded by the rate,
 *                              in kbps or PPS, assigned to 'specific_port'.
 *
 *   Note that for (cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB), the calculation of the
 *   rate takes into account 'calCal' (both length and number of instances of corresponding
 *   interface).
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   This procedure is used for conversion on 'interface' level shaper and, also
 *   for recycling/mirroring.
 * \see
 *   * dnx_ofp_rates_egq_shaper_rate_from_internal
 */
static shr_error_e
dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    dnx_ofp_rates_cal_egq_t * calendar_p,
    uint32 calendar_len,
    uint32 nof_ports,
    int packet_mode,
    uint32 specific_port,
    uint32 *ports_rates)
{
    uint32 slot_id;
    uint32 egq_resolution;
    uint32 calcal_length;
    uint32 calcal_instances;
    const dnx_ofp_egq_rates_cal_entry_t *slot;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cal_info_p, _SHR_E_PARAM, "cal_info_p");
    SHR_NULL_CHECK(calendar_p, _SHR_E_PARAM, "calendar_p");

    if (0 == calendar_len)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal calendar len (%d)", calendar_len);
    }

    if (cal_info_p->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                        nof_calcal_instances.get(unit, core, cal_info_p->chan_arb_id, &calcal_instances));

        /** If calendar type is channelized arbiter EGQ resolution is 1/256 */
        if (packet_mode)
        {
            egq_resolution = dnx_data_egr_queuing.params.cal_res_packet_mode_get(unit);
        }
        else
        {
            egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
        }
    }
    else
    {
        /** There is no calcal. Assume only one entry */
        calcal_length = 1;
        calcal_instances = 1;

        /** If calendar type is QPAIR or TCG, EGQ resolution is 1/256 */
        egq_resolution = DNX_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG;

        /** packet mode is not relevant for calendar types other than DNX_OFP_RATES_EGQ_CAL_CHAN_ARB */
        packet_mode = 0;
    }

    if ((specific_port >= nof_ports) || ((int) specific_port < 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal 'specific_port' (%d). Must be in range 0 - %d", specific_port,
                     nof_ports);
    }

    ports_rates[0] = 0;
    for (slot_id = 0; slot_id < calendar_len; ++slot_id)
    {
        slot = &calendar_p->slots[slot_id];
        if (slot->base_q_pair >= DNX_EGR_NOF_Q_PAIRS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Illegal base_q_pair (%d) at slot %d of calendar type %d. Must be smaller than %d",
                         slot->base_q_pair, slot_id, cal_info_p->cal_type, DNX_EGR_NOF_Q_PAIRS);
        }

        if (slot->base_q_pair == (uint32) specific_port)
        {
            /** Increase the total sum of credits of this 'specific _port' by the val of this slot */
            ports_rates[0] += slot->credit;
        }
    }

    /**  Calculate shaper rate for this port_idx: */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_rate_from_credits
                    (unit, calcal_length, calcal_instances, calendar_len, packet_mode, egq_resolution,
                     &ports_rates[0]));

exit:
    SHR_FUNC_EXIT;
}
static int
dnx_ofp_rates_tcg_id_egq_verify(
    int unit,
    int core,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint8 *is_egq_valid)
{
    int nof_priorities;
    uint32 priority_i;
    dnx_egr_queuing_tcg_info_t tcg_port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_egq_valid, _SHR_E_PARAM, "is_egq_valid");
    *is_egq_valid = FALSE;
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    sal_memset(&tcg_port_info, 0, sizeof(tcg_port_info));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_ofp_tcg_get(unit, core, logical_port, &tcg_port_info));
    for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
    {
        if (tcg_port_info.tcg_ndx[priority_i] == tcg_ndx)
        {
            *is_egq_valid = TRUE;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*********************************************/
/***************STOP PORT AUX*****************/
/*********************************************/
/*
 * \brief -
 *   Check which calendar set (Calendars set 'A' or 'B') is currently active.
 *   Then build the non-active calendar in HW.
 *   Caller is assumed to swap between the active calendar and the non-active one
 *   after invoking this procedure.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use for the new setup.
 * \param [in] cal_info_p -
 *   Pointer to dnx_ofp_rates_cal_info_t. The following elements are input:
 *     cal_info->cal_type (dnx_ofp_rates_egq_cal_type_e) - May be:
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *       DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY
 *       DNX_OFP_RATES_EGQ_CAL_TCG
 *   The following is also used,  but only for 'channelized calendar' (CAL_CHAN_ARB):
 *     cal_info->chan_arb_id (= 'calendar id'. Used to calculate 'offest', key into table).
 * \param [in] egq_cal_p -
 *   Array of 'dnx_ofp_rates_cal_egq_t'. Caller is assumed to have loaded the first 'egq_cal_len'
 *   entries on slots[]. Info from this array is loaded into HW.
 * \param [in] egq_cal_len -
 *   Number of valid entries on input calendar.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   The following DBAL (HW) tables are updated:
 *   * DBAL_TABLE_EGQ_SHAPING_OTM_CRDT_TABLE
 *   * DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE;
 *   * DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE;
 * \see
 *   * dnx_ofp_rates_port_rate_set
 */
static shr_error_e
dnx_ofp_rates_active_egq_generic_calendars_config(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    dnx_ofp_rates_cal_egq_t * egq_cal_p,
    uint32 egq_cal_len)
{
    uint32 fld_val, offset, slot, calendar_size;
    dnx_ofp_rates_cal_set_e egq_to_set;
    dnx_egq_calendar_tbl_data_t egq_data;

    SHR_FUNC_INIT_VARS(unit);
    slot = 0;
    SHR_NULL_CHECK(egq_cal_p, _SHR_E_PARAM, "egq_cal_p");
    SHR_NULL_CHECK(cal_info_p, _SHR_E_PARAM, "cal_info_p");
    /*
     *  Verify calendar length validity.
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_cal_size_get(unit, cal_info_p, &calendar_size));
    if ((egq_cal_len > calendar_size) || (egq_cal_len < 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "egq_cal_len (%d) is out of range (%d,%d)", egq_cal_len, 1, calendar_size);
    }
    /*
     * 1. Check which calendars (Calendars set 'A' or 'B')
     *    are currently active.
     * 2. Then build the non-active calendar entries
     */
    /*
     * Step '1'
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_set_select_get(unit, core, cal_info_p->cal_type, &fld_val));
    /*
     * Select the non-active set.
     */
    egq_to_set = (fld_val == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
    /*
     * Step '2.1'
     */
    /*
     * Write to the non-Active EGQ calendar indirectly
     * Update calendar length for selected, non-active, set
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_len_set(unit, core, cal_info_p, egq_to_set, egq_cal_len));
    if (cal_info_p->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        offset = cal_info_p->arb_slot_id;
    }
    else
    {
        offset = 0;
    }
    /*
     * Step '2.2'
     */
    for (slot = 0; slot < egq_cal_len; ++slot)
    {
        sal_memset(&egq_data, 0, sizeof(egq_data));
        egq_data.port_cr_to_add = egq_cal_p->slots[slot].credit;
        egq_data.ofp_index = egq_cal_p->slots[slot].base_q_pair;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_burst_size_get
                        (unit, core, cal_info_p, egq_data.ofp_index, &egq_data.port_max_credit));
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_entry_set
                        (unit, core, cal_info_p->cal_type, egq_to_set, offset + slot, &egq_data));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * This procedure is invoked nowhere in this code. It is left here in case it
 * can be used by some future API.
 */
/**
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_sw_db_port_priority_nof_valid_queues_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 *nof_valid_queues)
{
    int base_q_pair, port_base_q_pair, nof_priorities;
    uint32 ptc_ndx, is_valid, is_ptc_found;
    bcm_pbmp_t pbmp;
    bcm_port_t port_i;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_valid_queues, _SHR_E_PARAM, "nof_valid_queues");
    is_ptc_found = FALSE;
    *nof_valid_queues = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &port_base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
    BCM_PBMP_ITER(pbmp, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &nof_priorities));
        for (ptc_ndx = 0; ptc_ndx < nof_priorities; ++ptc_ndx)
        {
            if ((port_base_q_pair == base_q_pair) && (ptc_ndx == priority_ndx))
            {
                is_ptc_found = TRUE;
            }
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.valid.get(unit, core, base_q_pair + ptc_ndx, &is_valid));
            if (is_valid)
            {
                *nof_valid_queues += 1;
            }
        }
    }
    if (!is_ptc_found)
    {
        *nof_valid_queues += 1;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   Verify helper procedure for dnx_ofp_rates_egq_tcg_shaper_verify().
 *   Get Number of valid TCGs specified in sw-state.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use when accessing SW dataBase or HW.
 * \param [in] logical_port -
 *   Logical port to add count for. See 'remark' below.
 * \param [in] tcg -
 *   uint32. TC group to add count for.  See 'remark' below.
 * \param [out] nof_valid_entries -
 *   Pointer to uint32. This procedure loads pointed memory by the total number of
 *   valid TCGs specified on sw state (plus the one related to 'logical_port' plus 'tcg'
 *   above).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Go through all logical potrs. Get PS for each and count the number of valid
 *     TCGs. Return the total sum. If the combination of 'input logical port plus
 *     input tcg' is not found. Add it to the total as well since it is candidate
 *     to be added.
 * \see
 *   * dnx_ofp_rate_db
 */
static shr_error_e
dnx_ofp_rates_sw_db_tcg_nof_valid_entries_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 tcg,
    uint32 *nof_valid_entries)
{
    int nof_priorities, base_q_pair;
    uint32 ps, tcg_ps, is_tcg_found, tcg_ndx, is_valid;
    bcm_port_t logical_port_i;
    bcm_pbmp_t pbmp;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_valid_entries, _SHR_E_PARAM, "nof_valid_entries");
    is_tcg_found = FALSE, *nof_valid_entries = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    tcg_ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
    BCM_PBMP_ITER(pbmp, logical_port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port_i, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port_i, &nof_priorities));
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        for (tcg_ndx = 0; tcg_ndx < nof_priorities; ++tcg_ndx)
        {
            if ((ps == tcg_ps) && (tcg_ndx == tcg))
            {
                is_tcg_found = TRUE;
            }
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.valid.get(unit, core, ps, tcg_ndx, &is_valid));
            if (is_valid)
            {
                *nof_valid_entries += 1;
            }
        }
    }
    if (!is_tcg_found)
    {
        *nof_valid_entries += 1;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify procedure for dnx_ofp_rates_egq_tcg_max_burst_set() and for dnx_ofp_rates_egq_tcg_rate_sw_set().
 *   Make sure:
 *     - Number of priorities (number of elements) for this TCG is not below minimum
 *       allowed for TCG.
 *     - TCG identifier (tcg_ndx) is withing allowed range (DNX_TCG_MIN,DNX_TCG_MAX)
 *     - Size of burst (or rate) is below maximal limit which is DNX_OFP_RATES_BURST_LIMIT_MAX bytes
 *       (DNX_IF_MAX_RATE_KBPS(unit) kilo BPS)
 *     - Number of TCGs specified in sw-state is valid.
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use when accessing SW dataBase or HW.
 * \param [in] logical_port -
 *   Logical port to verify setup for.
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. TC group, on this port, for which to verify input.
 * \param [in] value -
 *   uint32. General input. Either 'burst size' in bytes or 'rate' in KBPS to check range
 *   validity of.
 * \param [in] set_state -
 *   uint32. Indicator on what to test on 'value'
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None.
 * \see
 *   * dnx_ofp_rate_db
 */
static int
dnx_ofp_rates_egq_tcg_shaper_verify(
    int unit,
    int core,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 value,
    dnx_ofp_rates_generic_func_state_e set_state)
{
    uint32 nof_valid_entries, max_kbps;
    uint8 is_egq_valid;

    SHR_FUNC_INIT_VARS(unit);
    nof_valid_entries = 0;

    /*
     * In case TCG is not being mapped by any other port priority. Rate must be 0 (disabled).
     */
    switch (set_state)
    {
        case DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST:
        {
            break;
        }
        case DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE:
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_id_egq_verify(unit, core, logical_port, tcg_ndx, &is_egq_valid));
            max_kbps = (is_egq_valid) ? DNX_IF_MAX_RATE_KBPS(unit) : 0;
            if (value > max_kbps)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Rate, in kbps (%d), is above max %d)", value, max_kbps);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal 'set_state' enum (%d)", set_state);
        }
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_sw_db_tcg_nof_valid_entries_get
                    (unit, core, logical_port, tcg_ndx, &nof_valid_entries));
    if (nof_valid_entries > DNX_EGR_NOF_TCG_IDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_valid_entries (%d) is out of range (larger than max allowed %d)",
                     nof_valid_entries, DNX_EGR_NOF_TCG_IDS);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ofp_rates_egq_single_port_rate_hw_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 *rate)
{
    uint32 egq_cal_len, org_if_val, org_otm_val;
    uint32 egq_rates[1];
    int base_q_pair;
    dnx_ofp_rates_cal_egq_t egq_cal;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 nof_instances;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(rate, _SHR_E_PARAM, "rate");
    sal_memset(&cal_info, 0x0, sizeof(cal_info));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    cal_info.chan_arb_id = calendar_id;

    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.nof_calcal_instances.get(unit, core, cal_info.chan_arb_id, &nof_instances));

    if (nof_instances == 0)
    {
        /** all ports on the interface have rate=0, no calendar was allocated */
        *rate = 0;
    }
    else
    {
        int packet_mode;
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;
        egq_cal_len = 0;

        /** Retrieve calendars */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_full_calendar_get(unit, core, &cal_info, &egq_cal, &egq_cal_len));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_packet_mode_get(unit, logical_port, &packet_mode));

        /** Calculate egq rates */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              &egq_cal,
                                                                              egq_cal_len,
                                                                              DNX_EGR_NOF_BASE_Q_PAIRS,
                                                                              packet_mode, base_q_pair, egq_rates));
        /** Get the rate of the requested port */
        *rate = egq_rates[0];
    }

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * This procedure is invoked nowhere in this code. It is left here in case it
 * can be used by some future API.
 */
/**
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_single_port_rate_sw_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 *rate)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * in case of non-egq ports - return 0
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        *rate = 0;
    }
    else
    {
        int base_q_pair;
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.rate.get(unit, core, base_q_pair, rate));
    }

exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofp_rates_calcal_config(
    int unit,
    int core)
{
    uint32 idx;
    uint32 act_cal;
    uint32 chan_arb_i;
    uint32 egq_calcal_chan_arb_rate_requested;
    uint32 egq_calcal_calendar_len;
    uint32 egq_calcal_rates[DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS];
    uint32 egq_calcal_instances[DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS];
    dnx_ofp_rates_cal_sch_t *egq_calcal_calendar_p = NULL;
    dnx_ofp_rates_cal_set_e cal2set;
    uint32 cal_index_selected;
    uint32 org_if_val, org_otm_val;
    dbal_fields_e dbal_field_id;
    SHR_FUNC_INIT_VARS(unit);
    egq_calcal_chan_arb_rate_requested = 0;
    egq_calcal_calendar_len = 0;
    SHR_ALLOC(egq_calcal_calendar_p, sizeof(*egq_calcal_calendar_p), "egq_calcal_calendar_p", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    /** Clear */
    egq_calcal_chan_arb_rate_requested = 0;
    sal_memset(egq_calcal_instances, 0, sizeof(egq_calcal_instances));

    /** Calculate CalCal according to the calendar ofp rates */
    for (chan_arb_i = 0; chan_arb_i < dnx_data_egr_queuing.params.nof_calendars_get(unit); chan_arb_i++)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.get(unit, core, chan_arb_i, &egq_calcal_rates[chan_arb_i]));
        egq_calcal_chan_arb_rate_requested += egq_calcal_rates[chan_arb_i];
    }

    /** Convert rates to calendar */
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_from_rates_to_calendar
                    (unit, egq_calcal_rates, (dnx_data_egr_queuing.params.nof_calendars_get(unit)),
                     egq_calcal_chan_arb_rate_requested, DNX_OFP_RATES_CALCAL_LEN_EGQ_MAX,
                     egq_calcal_calendar_p->slots, &(egq_calcal_calendar_len)));

    /** Get the active calendar from HW */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /** key construction.*/
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /** Getting value of OTM_SPR_SET_SELf. */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_SET_SEL, INST_SINGLE, &act_cal,
                                     GEN_DBAL_FIELD_LAST_MARK));
    cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;

    /** Write CalCal length to the inactive calendar */
    if (cal2set == DNX_OFP_RATES_CAL_SET_A)
    {
        dbal_field_id = DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_A;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_B;
    }

    /** Set the value of CAL_CAL_LENGTH_FOR_OTM_SPR_ A/B */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /** key construction. */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /** Set the value of CAL_CAL_LENGTH_FOR_OTM_SPR_ A/B */
                                     GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE,
                                     (uint32) (egq_calcal_calendar_len - 1), GEN_DBAL_FIELD_LAST_MARK));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.set(unit, core, egq_calcal_calendar_len));

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /** added only because shaper configuration under traffic causing losts */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }

    /** Write CalCal entries, calculate nof_calal instances per chan_cal */
    for (idx = 0; idx < egq_calcal_calendar_len; ++idx)
    {
        egq_calcal_instances[egq_calcal_calendar_p->slots[idx]] += 1;
        cal_index_selected = egq_calcal_calendar_p->slots[idx];
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_CAL_CAL_CALENDAR, 3, 1,
                                         /** key construction. */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CAL_SLOT_ID, (uint32) (idx),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_SET_SELECT, (uint32) (cal2set),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /** Set the value of CAL_CAL_LENGTH_FOR_OTM_SPR_ A/B */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CAL_INDX, INST_SINGLE,
                                         (uint32) (cal_index_selected), GEN_DBAL_FIELD_LAST_MARK));
    }

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /** added only because shaper configuration under traffic causing losts */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }

    /** Set calal nof instances per chan_arb to sw_db */
    for (chan_arb_i = 0; chan_arb_i < dnx_data_egr_queuing.params.nof_calendars_get(unit); ++chan_arb_i)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                        nof_calcal_instances.set(unit, core, chan_arb_i, egq_calcal_instances[chan_arb_i]));
    }

exit:
    SHR_FREE(egq_calcal_calendar_p);
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Switch active calendar: Read current active calendar set from HW and switch
 *   (in HW) to the other calendar set.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to apply new setup for.
 * \param [in] cal_type -
 *   Type of calendar to apply this operation for. Cal_type may be:
 *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *     DNX_OFP_RATES_EGQ_CAL_TCG
 *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output: Change calendar set, in HW, to the one that
 *     is in 'stand by'
 * \see
 *   * dnx_ofp_rate_db
 *   * DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *   * dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 */
static shr_error_e
switch_active_calendar(
    int unit,
    int core,
    dnx_ofp_rates_egq_cal_type_e cal_type)
{
    /*
     * change active calendar
     */
    dnx_ofp_rates_cal_set_e cal2set;
    uint32 act_cal;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Cal_type may be:
     *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
     *     DNX_OFP_RATES_EGQ_CAL_TCG
     *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY
     */
    switch (cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Get field val for OTM_SPR_SET_SELf
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_SET_SEL, INST_SINGLE, &act_cal,
                                             GEN_DBAL_FIELD_LAST_MARK));
            cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
            /*
             * Set the value of the other 'set'
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Set field val for DBAL_FIELD_OTM_SPR_SET_SEL
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_SET_SEL, INST_SINGLE,
                                             (uint32) (cal2set), GEN_DBAL_FIELD_LAST_MARK));
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            /*
             * Get active calendar
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Get field val for DBAL_FIELD_QPAIR_SPR_SET_SEL
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_SET_SEL, INST_SINGLE, &act_cal,
                                             GEN_DBAL_FIELD_LAST_MARK));
            cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Get field val for DBAL_FIELD_QPAIR_SPR_SET_SEL
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_SET_SEL, INST_SINGLE,
                                             (uint32) (cal2set), GEN_DBAL_FIELD_LAST_MARK));
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            /*
             * Get active calendar
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Get field val for DBAL_FIELD_TCG_SPR_SET_SEL
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_SET_SEL, INST_SINGLE, &act_cal,
                                             GEN_DBAL_FIELD_LAST_MARK));
            cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Get field val for DBAL_FIELD_TCG_SPR_SET_SEL
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_SET_SEL, INST_SINGLE,
                                             (uint32) (cal2set), GEN_DBAL_FIELD_LAST_MARK));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unkonown type (%d)", cal_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Go through sw-state info related to all ports corresponding to 'channelized
 *   arbiter' calendar, recalculate all port shapers, update HW on inactive
 *   calendar and, then, switch to the updated calendar.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to apply new setup for.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rate_db
 *   * DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *   * dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar
 *   * dnx_ofp_rates_active_egq_generic_calendars_config
 */
static shr_error_e
dnx_ofp_rates_port_rate_set(
    int unit,
    bcm_core_t core)
{
    uint32 org_if_val, org_otm_val, cal_slots;
    dnx_ofp_rates_cal_info_t cal_info;
    int chan_arb_i, nof_calendars;
    /** Initialize all pointers which are released at 'exit' */
    /** Array capable of containing all logical port ids as specified on 'pbmp' below. */
    uint32 *logical_ports_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(logical_ports_p, sizeof(*logical_ports_p) * SOC_PBMP_PORT_MAX, "logical_ports_p", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    cal_info.arb_slot_id = 0;
    nof_calendars = dnx_data_egr_queuing.params.nof_calendars_get(unit);

    /** Cycle over all active calendars and update credits for all ports. */
    for (chan_arb_i = 0; chan_arb_i < nof_calendars; ++chan_arb_i)
    {
        uint32 nof_instances;
        cal_slots = 0;
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.nof_calcal_instances.get(unit, core, chan_arb_i, &nof_instances));
        if (nof_instances > 0)
        {
            /** Retrieve rates from the software database */
            bcm_port_t logical_port_i;
            bcm_pbmp_t pbmp;
            int base_q_pair;
            uint32 is_valid, tm_port_i;
            uint32 egq_calendar_len;
            int calendar_id;

            /** Clear array of logical ports. Not really required. Just good practice. */
            sal_memset(logical_ports_p, 0, sizeof(*logical_ports_p) * SOC_PBMP_PORT_MAX);
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));

            BCM_PBMP_ITER(pbmp, logical_port_i)
            {
                /** Get the interface corresponding to 'logical_port_i' */
                SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port_i, &calendar_id));
                if (calendar_id != chan_arb_i)
                {
                    /** handle current calendar */
                    continue;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port_i, &base_q_pair));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.valid.get(unit, core, base_q_pair, &is_valid));
                if (is_valid)
                {
                    logical_ports_p[cal_slots] = logical_port_i;
                    cal_slots++;
                }
            }

            /*
             * At this point, 'cal_slots' contains the number of the ports on specified interface ('chan_arb_i').
             * This is also the length of the calendar on this channelized interface.
             * Also, the array 'logical_ports_p[]' is loaded by the relevant logical port ids (first
             * 'cal_slots' elements).
             */
            if (cal_slots)
            {
                /** Enter if there is a non-empty calendar for this interface. */
                uint32 calcal_length, calcal_instances;
                dnx_ofp_rates_cal_set_e egq_to_set;
                uint32 fld_val;

                egq_calendar_len = cal_slots;
                /*
                 * HW limitation
                 * Only calendar zero (CPU) can point to slot zero. If calendar zero does not exist,
                 * calendars need to start from slot 1
                 */
                if (chan_arb_i && cal_info.arb_slot_id == 0)
                {
                    cal_info.arb_slot_id++;
                }

                /** Configure calendar */
                cal_info.chan_arb_id = chan_arb_i;
                cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;

                /*
                 * Note that calendar will always be recalculated since it is faster to do that
                 * then just copy from 'active' set to 'standby' set.
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.modified.set(unit, core, chan_arb_i, FALSE));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                                nof_calcal_instances.get(unit, core, chan_arb_i, &calcal_instances));
                SHR_IF_ERR_EXIT(dnx_ofp_rates_set_select_get(unit, core, cal_info.cal_type, &fld_val));

                /** Select the non-active set. */
                egq_to_set = (fld_val == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;

                /** Write to the non-Active EGQ calendar */
                /** Update calendar length for selected, non-active, set */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_len_set
                                (unit, core, &cal_info, egq_to_set, egq_calendar_len));

                for (cal_slots = 0; cal_slots < egq_calendar_len; cal_slots++)
                {
                    dnx_egq_calendar_tbl_data_t egq_data;
                    uint32 egq_rate;
                    int packet_mode;
                    uint32 egq_resolution;
                    uint32 offset;
                    dnx_ofp_egq_rates_cal_entry_t cal_entry;

                    logical_port_i = logical_ports_p[cal_slots];
                    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port_i, &core, &tm_port_i));
                    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port_i, &base_q_pair));

                    /** Get egq rate */
                    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.rate.get(unit, core, base_q_pair, &egq_rate));
                    cal_entry.base_q_pair = base_q_pair;

                    /** From the port get the associated shaper mode (DATA or PACKET) and number of priorities The base Q-pair is known. */
                    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_packet_mode_get(unit, logical_port_i, &packet_mode));
                    if (packet_mode == 0)
                    {
                        /** For DATA mode, conversion coefficient is 'credits per bit'. */
                        egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
                    }
                    else
                    {
                        /** For PACKET mode, conversion coefficient is 'credits per packet'. */
                        egq_resolution = dnx_data_egr_queuing.params.cal_res_packet_mode_get(unit);
                    }

                    /** Convert 'egq_rate' from KBPS to 'credits' (per calendar cycle). */
                    SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_credits_from_rate
                                    (unit, calcal_length, calcal_instances, egq_calendar_len, egq_resolution,
                                     (uint32) packet_mode, egq_rate, &(cal_entry.credit)));
                    offset = cal_info.arb_slot_id;
                    egq_data.port_cr_to_add = cal_entry.credit;
                    egq_data.ofp_index = cal_entry.base_q_pair;
                    SHR_IF_ERR_EXIT(dnx_ofp_rates_burst_size_get
                                    (unit, core, &cal_info, egq_data.ofp_index, &egq_data.port_max_credit));
                    SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_entry_set
                                    (unit, core, cal_info.cal_type, egq_to_set, (offset + cal_slots), &egq_data));
                }
            }
        }
        cal_info.arb_slot_id += cal_slots;
    }

    /** change active calendar for CAL_CHAN_ARB */
    SHR_IF_ERR_EXIT(switch_active_calendar(unit, core, DNX_OFP_RATES_EGQ_CAL_CHAN_ARB));

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FREE(logical_ports_p);
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *   A calendar is made out of slots, each corresponding to a port (base qpair).
 *   Given 'calendar' type and 'base qpair', set indicated maximal burst size (in credits)
 *   on all slots (of that calendar) which correspond to input 'base qpair'.
 *   Updated calendar is the currently active one.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to apply new setup for.
 * \param [in] base_q_pair -
 *   'Port' to apply new 'maximal burst size' for.
 * \param [in] cal_info_p -
 *   Pointer to dnx_ofp_rates_cal_info_t.
 *   Only 'cal_info->cal_type' and 'cal_info->chan_arb_id' are used here.
 *   'cal_type' is the type of the calendar and can be:
 *     DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *     DNX_OFP_RATES_EGQ_CAL_TCG
 *     DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY
 *   'chan_arb_id' is the calendar id corresponding to specified calendar and
 *   is only relevant for DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 * \param [in] max_burst -
 *   uint32. maximal burst size (in credits).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated HW:
 *         See, for example, DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE,
 *         DBAL_FIELD_QPAIR_MAX_BURST
 *   * To convert credits to KBPS, divide by dnx_data_egr_queuing.params.cal_burst_res_get(unit)
 *   * If selected calendar is empty, this procedure returns with error, indicating that
 *     shaping is not active.
 * \see
 *   * dnx_ofp_rates_egq_port_priority_max_burst_set()
 */
static int
dnx_ofp_rates_max_burst_generic_set(
    int unit,
    int core,
    uint32 base_q_pair,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    uint32 max_burst)
{
    uint32 fld_val, egq_to_set, cal_len, slot, offset;
    dnx_egq_calendar_tbl_data_t scm_tbl_data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cal_info_p, _SHR_E_PARAM, "cal_info_p");
    SHR_IF_ERR_EXIT(dnx_ofp_rates_set_select_get(unit, core, cal_info_p->cal_type, &fld_val));
    /*
     * Select the active calendar set.
     */
    egq_to_set = (fld_val == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_A : DNX_OFP_RATES_CAL_SET_B;
    /*
     * read calendar length
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_len_get(unit, core, cal_info_p, egq_to_set, &cal_len));
    if (cal_len == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No Shaping set! Set Shaping before setting Burst Max.");
    }
    /*
     * Only meaningful for CHAN_ARB:
     *   Get 'offset' of calendar related to calendar id (cal_info_p->chan_arb_id)
     * For all others, just load 'offset' by '0'
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_offset_get(unit, core, cal_info_p, egq_to_set, &offset));
    for (slot = 0; slot < cal_len; ++slot)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_entry_get
                        (unit, core, cal_info_p->cal_type, egq_to_set, slot + offset, &scm_tbl_data));
        if (scm_tbl_data.ofp_index == base_q_pair)
        {
            scm_tbl_data.port_max_credit = max_burst;
            SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_entry_set
                            (unit, core, cal_info_p->cal_type, egq_to_set, slot + offset, &scm_tbl_data));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_port_priority_max_burst_set(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 max_burst)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_qp_val;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&cal_info, 0, sizeof(cal_info));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_qp_val));
    }
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Setting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_set(unit,
                                                            core,
                                                            (base_q_pair + priority_ndx),
                                                            &cal_info,
                                                            max_burst *
                                                            dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_qp_val));
    }
exit:
    SHR_FUNC_EXIT;

}

/*
 * See egq_ofp_rates.h
 */
int
dnx_ofp_rates_egq_tcg_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_ndx,
    uint32 max_burst)
{
    uint32 org_tcg_val, tcg_id, ps;
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_ofp_rates_egq_tcg_shaper_verify(unit,
                                                              core,
                                                              logical_port,
                                                              tcg_ndx,
                                                              max_burst,
                                                              DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST));
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting tcg_id
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);
        /*
         * Setting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
        cal_info.chan_arb_id = 0;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_set(unit,
                                                            core,
                                                            tcg_id,
                                                            &cal_info,
                                                            max_burst *
                                                            dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FUNC_EXIT;

}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_single_port_max_burst_set(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 max_burst)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_if_val, org_otm_val;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    if (max_burst > DNX_OFP_RATES_BURST_LIMIT_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_burst, in bytes, (%d) is above max (%d)", max_burst,
                     DNX_OFP_RATES_BURST_LIMIT_MAX);
    }
    /*
     * Getting the fap port's base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Setting max burst
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.burst.set(unit, core, base_q_pair, max_burst));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    cal_info.chan_arb_id = calendar_id;

    cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;

    max_burst = max_burst * dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_set(unit, core, base_q_pair, &cal_info, max_burst));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_max_burst_generic_get(
    int unit,
    int core,
    uint32 base_q_pair,
    dnx_ofp_rates_cal_info_t * cal_info_p,
    uint32 *max_burst)
{
    dnx_ofp_rates_cal_set_e egq_to_get;
    uint32 fld_val, cal_len, slot, offset;
    dnx_egq_calendar_tbl_data_t scm_tbl_data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(max_burst, _SHR_E_PARAM, "max_burst");
    SHR_NULL_CHECK(cal_info_p, _SHR_E_PARAM, "cal_info_p");

    SHR_IF_ERR_EXIT(dnx_ofp_rates_set_select_get(unit, core, cal_info_p->cal_type, &fld_val));
    /*
     * Select the active calendar set.
     */
    egq_to_get = (fld_val == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_A : DNX_OFP_RATES_CAL_SET_B;
    /*
     * read calendar length
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_len_get(unit, core, cal_info_p, egq_to_get, &cal_len));
    /*
     * Only meaningful for CHAN_ARB:
     *   Get 'offset' of calendar related to calendar id (cal_info_p->chan_arb_id)
     * For all others, just load 'offset' by '0'
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_offset_get(unit, core, cal_info_p, egq_to_get, &offset));
    for (slot = 0; slot < cal_len; ++slot)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calendar_entry_get
                        (unit, core, cal_info_p->cal_type, egq_to_get, slot + offset, &scm_tbl_data));
        if (scm_tbl_data.ofp_index == base_q_pair)
        {
            *max_burst = scm_tbl_data.port_max_credit;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See ofp_egq_rates.h
 */
shr_error_e
dnx_ofp_rates_single_port_max_burst_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 *max_burst_p)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_if_val, org_otm_val;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    SHR_NULL_CHECK(max_burst_p, _SHR_E_PARAM, "max_burst_p");
    /*
     * Getting the fap port's base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Getting max burst
     */
    cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    cal_info.chan_arb_id = calendar_id;

    SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_get(unit, core, base_q_pair, &cal_info, max_burst_p));
    *max_burst_p /= dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_tcg_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_ndx,
    uint32 *max_burst)
{
    int core;
    uint32 org_tcg_val, tcg_id, ps;
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }
    SHR_NULL_CHECK(max_burst, _SHR_E_PARAM, "max_burst");
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting tcg_id
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);
        /*
         * Setting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
        cal_info.chan_arb_id = 0;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_get(unit, core, tcg_id, &cal_info, max_burst));
        *max_burst = *max_burst / dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_port_priority_max_burst_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 *max_burst_p)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_tc_val;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&cal_info, 0, sizeof(cal_info));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_tc_val));
    }
    SHR_NULL_CHECK(max_burst_p, _SHR_E_PARAM, "max_burst_p");
    {
        /*
         * Getting the logical port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_get(unit,
                                                            core, (base_q_pair + priority_ndx), &cal_info,
                                                            max_burst_p));
        /*
         * Convert to bytes
         */
        *max_burst_p /= dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_tc_val));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_tcg_rate_sw_set(
    int unit,
    int core,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 tcg_rate)
{
    int base_q_pair;
    uint32 ps;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_ofp_rates_egq_tcg_shaper_verify(unit,
                                                              core,
                                                              logical_port,
                                                              tcg_ndx,
                                                              tcg_rate, DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE));
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting ps
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        /*
         * Setting rate
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.valid.set(unit, core, ps, tcg_ndx, TRUE));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.rate.set(unit, core, ps, tcg_ndx, tcg_rate));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_port_priority_rate_sw_set(
    int unit,
    bcm_port_t logical_port,
    dnx_egr_q_prio_e prio_ndx,
    uint32 ptc_rate)
{
    int base_q_pair;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Setting rate
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.rate.set(unit, core, (base_q_pair + prio_ndx) /* q_pair */ ,
                                                       ptc_rate));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.valid.set(unit, core, (base_q_pair + prio_ndx) /* q_pair */ ,
                                                        TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_tcg_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 *tcg_rate)
{
    uint32 egq_cal_len, ps, org_tcg_val, tcg_id;
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    dnx_ofp_rates_cal_egq_t egq_cal;
    int core;
    uint32 egq_rates[1];

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /** added only because shaper configuration under traffic causing losts */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }
    SHR_NULL_CHECK(tcg_rate, _SHR_E_PARAM, "tcg_rate");
    sal_memset(&cal_info, 0x0, sizeof(cal_info));
    {
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
        egq_cal_len = 0;

        /** Retrieve calendars */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_full_calendar_get(unit, core, &cal_info, &egq_cal, &egq_cal_len));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);

        /** Calculate egq rates */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              &egq_cal,
                                                                              egq_cal_len,
                                                                              DNX_EGR_NOF_Q_PAIRS,
                                                                              0, tcg_id, egq_rates));
        /** Get the rate of the requested port */
        *tcg_rate = egq_rates[0];
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /** added only because shaper configuration under traffic causing losts */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ofp_rates_egq_port_priority_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t ptc_ndx,
    uint32 *ptc_rate)
{
    uint32 egq_cal_len, egq_rates[1];
    int base_q_pair;
    bcm_core_t core;

    dnx_ofp_rates_cal_egq_t egq_cal;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_tc_val;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ptc_rate, _SHR_E_PARAM, "ptc_rate");

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_tc_val));
    }
    {
        sal_memset(&cal_info, 0x0, sizeof(cal_info));
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
        egq_cal_len = 0;
        /*
         * Retrieve calendars
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_full_calendar_get(unit, core, &cal_info, &egq_cal, &egq_cal_len));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Calculate egq rates
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              &egq_cal,
                                                                              egq_cal_len,
                                                                              DNX_EGR_NOF_Q_PAIRS,
                                                                              0, (base_q_pair + ptc_ndx), egq_rates));
        /*
         * Get the rate of the requested port
         */
        *ptc_rate = egq_rates[0];
    }

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_tc_val));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   Go through all ports, in sw-state. Get corresponding 'base qpair' and 'number of priorities'.
 *   (which is also 'number of TCGs')
 *   For each TCG index,
 *     Store the rate in 'egq_rates'
 *     Store 'base_q_pair + tcg_ndx' in local calendar array (egq_calendar->slots)
 *   Use partial info in constructed calendar to update 'credits' in local calendar.
 *   Use constructed calendar ro update HW on all cores.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use when accessing HW and SW data base.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect input:
 *   * * dnx_ofp_rate_db.qpair
 * \see
 *   * dnx_rates_egq_port_priority_rate_hw_set
 *   * dnx_ofp_rate_db
 *   * dnx_algo_port_logicals_get
 */
static int
dnx_rates_egq_tcg_rate_hw_set(
    int unit,
    int core)
{
    int base_q_pair, nof_priorities;
    uint32
        egq_calendar_len,
        fap_port_ndx,
        ps,
        recalc,
        tcg_ndx, tcg_id, tcg_offset, act_cal, org_tcg_val, sum_of_tcg_requested, port_i, rate, is_valid, cal_slots;
    dnx_ofp_rates_cal_info_t cal_info;
    dnx_ofp_rates_cal_set_e cal2set;
    bcm_pbmp_t pbmp;
    int core_i;
    uint32 dbal_field_val;
    uint32 *egq_rates = NULL;
    dnx_ofp_rates_cal_egq_t *egq_calendar = NULL;

    SHR_FUNC_INIT_VARS(unit);
    egq_calendar_len = 0;
    recalc = TRUE;
    sum_of_tcg_requested = 0;
    cal_slots = 0;
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }
    sal_memset(&cal_info, 0, sizeof(cal_info));
    SHR_ALLOC(egq_rates, sizeof(uint32) * dnx_data_egr_queuing.params.nof_q_pairs_get(unit), "egq_rates", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egq_calendar, sizeof(dnx_ofp_rates_cal_egq_t), "egq_calendar", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    {
        sal_memset(egq_rates, 0, dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * sizeof(uint32));
        /*
         * Get active calendar
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                         /*
                                          * key construction.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /*
                                          * Get field val for DBAL_FIELD_TCG_SPR_SET_SEL
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_SET_SEL, INST_SINGLE, &act_cal,
                                         GEN_DBAL_FIELD_LAST_MARK));
        cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
        /*
         * Go through all ports, in sw-state. get corresponding 'base qpair' and 'number of priorities'.
         * Calculate 'port scheduler' by dividing 'base qpair' by DNX_EGR_NOF_Q_PAIRS_IN_PS.
         * Calculate 'tcg' by getting the remainder of that division.
         * For each TCG index,
         *   Store the rate in 'egq_rates'
         *   Store 'base_q_pair + tcg_ndx' in local calendar array (egq_calendar->slots)
         *   Sum all rates and slots.
         */
        BCM_PBMP_ITER(pbmp, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core_i, &fap_port_ndx));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &nof_priorities));
            ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
            tcg_offset = base_q_pair - (ps * DNX_EGR_NOF_Q_PAIRS_IN_PS);
            for (tcg_ndx = 0; tcg_ndx < nof_priorities; ++tcg_ndx)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.rate.get(unit, core, ps, tcg_ndx + tcg_offset, &rate));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.valid.get(unit, core, ps, tcg_ndx + tcg_offset, &is_valid));
                if (is_valid)
                {
                    tcg_id = base_q_pair + tcg_ndx;
                    egq_rates[tcg_id] = rate;
                    sum_of_tcg_requested += rate;
                    egq_calendar->slots[cal_slots].base_q_pair = tcg_id;
                    cal_slots++;
                }
            }
        }
        /*
         * Use info in newly constructed calendar to add 'internal rates' (credits) and,
         * then, update HW accordingly.
         */
        if (cal_slots)
        {
            cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
            SHR_IF_ERR_EXIT(dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar(unit,
                                                                                   core,
                                                                                   &cal_info,
                                                                                   egq_rates,
                                                                                   cal_slots,
                                                                                   recalc,
                                                                                   egq_calendar, &egq_calendar_len));
            /*
             * Write to device
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_active_egq_generic_calendars_config(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              egq_calendar, egq_calendar_len));
            /*
             * Set active calendar
             */
            /*
             * Set the value of 'TCG_SPR_SET_SEL' to 'cal2set'
             */
            dbal_field_val = cal2set;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Set field val for DBAL_FIELD_FC_QP_MAX_CREDIT
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_SET_SEL, INST_SINGLE,
                                             (uint32) (dbal_field_val), GEN_DBAL_FIELD_LAST_MARK));
        }
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FREE(egq_rates);
    SHR_FREE(egq_calendar);
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rate.h
 */
shr_error_e
dnx_ofp_rates_egq_tcg_rate_hw_set(
    int unit)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_rates_egq_tcg_rate_hw_set(unit, core));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *   Go through all ports, in sw-state. Get corresponding 'base qpair' and 'number of priorities'.
 *   For each priority,
 *     Store the rate in 'egq_rates'
 *     Store 'base_q_pair + ptc_ndx' in local calendar array (egq_calendar->slots)
 *   Use partial info in constructed calendar to update 'credits' in local calendar.
 *   Use constructed calendar ro update HW on all cores.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use when accessing HW and SW data base.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect input:
 *   * * dnx_ofp_rate_db.shaper_enable.
 *   * * dnx_ofp_rate_db.qpair
 * \see
 *   * dnx_rates_egq_port_priority_rate_hw_set
 *   * dnx_ofp_rate_db
 *   * dnx_algo_port_logicals_get
 */
static int
dnx_rates_egq_port_priority_rate_hw_set(
    int unit,
    int core)
{
    uint32
        egq_calendar_len,
        fap_port_ndx,
        ptc_ndx, recalc = TRUE, act_cal, org_qp_val, sum_of_ptc_requested, port_i, rate, is_valid, cal_slots;
    int base_q_pair, nof_priorities;
    dnx_ofp_rates_cal_info_t cal_info;
    dnx_ofp_rates_cal_set_e cal2set;
    bcm_pbmp_t pbmp;
    int core_i;
    uint32 dbal_field_val;
    dnx_ofp_rates_cal_egq_t *egq_calendar = NULL;
    uint32 *egq_rates = NULL;

    SHR_FUNC_INIT_VARS(unit);
    egq_calendar_len = 0;
    sum_of_ptc_requested = 0;
    cal_slots = 0;
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_qp_val));
    }
    sal_memset(&cal_info, 0, sizeof(cal_info));
    SHR_ALLOC(egq_rates, sizeof(uint32) * dnx_data_egr_queuing.params.nof_q_pairs_get(unit), "egq_rates", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egq_calendar, sizeof(dnx_ofp_rates_cal_egq_t), "egq_calendar", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    /*
     * Get active calendar
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /*
                                      * key construction.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /*
                                      * Get field val for DBAL_FIELD_QPAIR_SPR_SET_SEL
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_SET_SEL, INST_SINGLE, &act_cal,
                                     GEN_DBAL_FIELD_LAST_MARK));
    cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
    {
        sal_memset(egq_rates, 0, dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * sizeof(uint32));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
        /*
         * Go through all ports, in sw-state. get corresponding 'base qpair' and 'number of priorities'.
         * For each priority,
         *   Store the rate in 'egq_rates'
         *   Store 'base_q_pair + ptc_ndx' in local calendar array (egq_calendar->slots)
         *   Sum all rates and slots.
         */
        BCM_PBMP_ITER(pbmp, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core_i, &fap_port_ndx));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &nof_priorities));
            for (ptc_ndx = 0; ptc_ndx < nof_priorities; ++ptc_ndx)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.rate.get(unit, core, (base_q_pair + ptc_ndx), &rate));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.valid.get(unit, core, (base_q_pair + ptc_ndx), &is_valid));
                if (is_valid)
                {
                    egq_rates[base_q_pair + ptc_ndx] = rate;
                    sum_of_ptc_requested += rate;
                    egq_calendar->slots[cal_slots].base_q_pair = base_q_pair + ptc_ndx;
                    cal_slots++;
                }
            }
        }
        /*
         * Use info in newly constructed calendar to add 'internal rates' (credits) and,
         * then, update HW accordingly.
         */
        if (cal_slots)
        {
            cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
            SHR_IF_ERR_EXIT(dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar(unit,
                                                                                   core,
                                                                                   &cal_info,
                                                                                   egq_rates,
                                                                                   cal_slots,
                                                                                   recalc,
                                                                                   egq_calendar, &egq_calendar_len));
            /*
             * Write to device
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_active_egq_generic_calendars_config(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              egq_calendar, egq_calendar_len));
            /*
             * Set active calendar
             */
            /*
             * Set the value of 'QPAIR_SPR_SET_SEL' to 'cal2set'
             */
            dbal_field_val = cal2set;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             /*
                                              * Set field val for DBAL_FIELD_QPAIR_SPR_SET_SEL
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_SET_SEL, INST_SINGLE,
                                             (uint32) (dbal_field_val), GEN_DBAL_FIELD_LAST_MARK));
        }
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_qp_val));
    }
exit:
    SHR_FREE(egq_rates);
    SHR_FREE(egq_calendar);
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_egq_port_priority_rate_hw_set(
    int unit)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_rates_egq_port_priority_rate_hw_set(unit, core));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - set per-qpair compensation in HW
 */
static shr_error_e
dnx_egr_port_base_q_pair_compensation_set(
    int unit,
    int core,
    int base_q_pair,
    int comp)
{
    int field_size;
    uint32 field_val;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_val = (uint32) comp;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair);
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * write the value to HW at 1 complement
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES,
                                               DBAL_FIELD_CR_ADJUST_VALUE, FALSE, 0, 0, &field_size));

    field_val = UTILEX_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(comp, field_size);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CR_ADJUST_VALUE, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_port_compensation_set(
    int unit,
    bcm_port_t logical_port,
    int comp)
{
    int base_q_pair, egq_if, is_channelized, flags = 0;
    bcm_core_t core;
    bcm_pbmp_t logical_ports;
    bcm_port_t port_i;
    SHR_FUNC_INIT_VARS(unit);

    /** get interface related information */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &is_channelized));

    if (is_channelized)
    {
        /** get channels bitmap */
        flags = DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY;
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, logical_port, flags, &logical_ports));
    }
    else
    {
        /** set only the current port */
        BCM_PBMP_PORT_SET(logical_ports, logical_port);
    }

    /** iterate over channels bitmap and set compensation on every channel */
    BCM_PBMP_ITER(logical_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_egr_port_base_q_pair_compensation_set(unit, core, base_q_pair, comp));
    }

    /*
     * Egress Rate Measure compensation - set to the same value as per-q-pair compensation
     */
    if (dnx_data_egr_queuing.rate_measurement.feature_get(unit, dnx_data_egr_queuing_rate_measurement_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_rate_measurement_compensation_set(unit, core, egq_if, comp));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - get per-qpair compensation from HW
 */
static shr_error_e
dnx_egr_port_base_q_pair_compensation_get(
    int unit,
    int core,
    int base_q_pair,
    int *comp)
{
    int field_size = 0;
    uint32 field_val = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *comp = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair);
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * get compensation field size
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES,
                                               DBAL_FIELD_CR_ADJUST_VALUE, FALSE, 0, 0, &field_size));
    /*
     * Get the value: read 1 complement value from HW
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CR_ADJUST_VALUE, INST_SINGLE, &field_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *comp = UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(field_val, field_size);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_port_compensation_get(
    int unit,
    bcm_port_t logical_port,
    int *comp_p)
{
    int base_q_pair;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    SHR_IF_ERR_EXIT(dnx_egr_port_base_q_pair_compensation_get(unit, core, base_q_pair, comp_p));

exit:
    SHR_FUNC_EXIT;
}
/**
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_wfq_get(
    int unit,
    bcm_port_t logical_port,
    uint32 cosq,
    dnx_egr_ofp_sch_wfq_t * wfq_info)
{
    uint32 offset;
    dnx_egq_dwm_tbl_data_t dwm_tbl_data;
    int base_q_pair;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(wfq_info, _SHR_E_PARAM, "wfq_info");
    sal_memset(&dwm_tbl_data, 0, sizeof(dwm_tbl_data));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * offset set by base_q_pair + cosq
     */
    offset = base_q_pair + cosq;
    SHR_IF_ERR_EXIT(dnx_egq_dbal_weights_tbl_get(unit, core, offset, &dwm_tbl_data));
    wfq_info->sched_weight = dwm_tbl_data.uc_or_uc_low_queue_weight;
    wfq_info->unsched_weight = dwm_tbl_data.mc_or_mc_low_queue_weight;
exit:
    SHR_FUNC_EXIT;
}
/**
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_wfq_set(
    int unit,
    dnx_port_t logical_port,
    uint32 cosq,
    dnx_egr_ofp_sch_wfq_t * wfq_info)
{
    uint32 offset;
    dnx_egq_dwm_tbl_data_t dwm_tbl_data;
    int base_q_pair;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(wfq_info, _SHR_E_PARAM, "wfq_info");

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * offset set by base_q_pair + cosq
     */
    offset = base_q_pair + cosq;
    dwm_tbl_data.uc_or_uc_low_queue_weight = wfq_info->sched_weight;
    dwm_tbl_data.mc_or_mc_low_queue_weight = wfq_info->unsched_weight;
    SHR_IF_ERR_EXIT(dnx_egq_dbal_weights_tbl_set(unit, core, offset, &dwm_tbl_data));
exit:
    SHR_FUNC_EXIT;
}
/*
 * TCG-related procedures
 * {
 */
/**
 * \brief -
 *   Enable/disable TCG shapers, related to specified logical port.
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] enable -
 *   uint32. Flag. If TRUE then enable all TCG-qpairs shapers. Else, disable.
 *   (Affected qpairs are 'base_q_pair and corresponding 'priority' q_pairs).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Operation is carried out only if general flag, in HW, indicating
 *     whether TCG shapers are enabled, is asserted.
 *   * Specific qpairs are controlled via HW memory
 *     EPS_TCG_CFG_2,EPS_TCG_CFG (field TCG_SPR_DIS)
 * \see
 *   * dnx_egr_queuing_port_all_shapers_enable_set
 *   * dnx_algo_port_base_q_pair_get
 */
shr_error_e
dnx_ofp_rates_port_tcg_shaper_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable)
{
    int core, base_q_pair, nof_priorities, cosq;
    uint32 q_pair_shp_en_f;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE,
                                         (uint32 *) (&q_pair_shp_en_f), GEN_DBAL_FIELD_LAST_MARK));
        if (q_pair_shp_en_f)
        {
            for (cosq = 0; cosq < nof_priorities; cosq++)
            {
                SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_TCG_ATTRIBUTES,
                                                 2, 1,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_TCG_INDEX, (uint32) (base_q_pair + cosq),
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_DIS, INST_SINGLE,
                                                 (uint32) (!enable), GEN_DBAL_FIELD_LAST_MARK));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * Verify procedure for dnx_ofp_rates_tc_to_tcg_mapping_set().
 * See header of the latter for description of input/output parameters.
 */
static int
dnx_ofp_rates_tc_to_tcg_mapping_set_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_egr_q_prio_e priority,
    dnx_egr_queuing_tcg_info_t * tcg_info)
{
    uint32 ps, priority_offset;
    uint32 member_priority, priority_i, tcg_i;
    int base_q_pair;
    int nof_priorities;
    uint8 is_one_member;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "nof_priorities (%d) is smaller than minimal allowed (%d)",
                     nof_priorities, dnx_data_egr_queuing.params.tcg_min_priorities_get(unit));
    }
    ps = base_q_pair / DNX_NOF_TCGS;
    priority_offset = base_q_pair - (ps * DNX_NOF_TCGS);
    if ((priority_offset + nof_priorities) > dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_priorities (%d) plus priority_offset (%d) can not be larger than nof_egr_q_prio (%d)",
                     nof_priorities, priority_offset, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
    {
        /*
         * DNX_TCG_MIN may be changed to be bigger than 0.
         */
        if (((int) (tcg_info->tcg_ndx[priority_i]) < DNX_TCG_MIN) || ((tcg_info->tcg_ndx[priority_i]) > DNX_TCG_MAX))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Priority %d: Value of corresponding tcg (%d) is out of range (%d,%d).",
                         priority_i, tcg_info->tcg_ndx[priority_i], DNX_TCG_MIN, DNX_TCG_MAX);
        }
    }
    /*
     * If last four tcgs are only a single member TCG. In that case, verify first that
     * the required tcg_ndx is not mapped by other q-pair
     */
    /*
     * Verify each single member TCG has only one priority that is mapped to it
     */
    if (nof_priorities == DNX_NOF_TCGS_IN_PS)
    {
        for (tcg_i = DNX_EGR_SINGLE_MEMBER_TCG_START; tcg_i <= DNX_EGR_SINGLE_MEMBER_TCG_END; tcg_i++)
        {
            is_one_member = FALSE;
            member_priority = -1;
            for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
            {
                if (tcg_info->tcg_ndx[priority_i] == tcg_i)
                {
                    if (is_one_member)
                    {
                        /*
                         * More than one member set to this tcg
                         */
                        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                                     "TCG (%d) has more than one priority mapped to it (%d plus %d)",
                                     tcg_i, member_priority, priority_i);
                    }
                    else
                    {
                        is_one_member = TRUE;
                        member_priority = priority_i;
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
int
dnx_ofp_rates_tc_to_tcg_mapping_set(
    int unit,
    dnx_port_t logical_port,
    dnx_egr_q_prio_e priority,
    dnx_tcg_ndx_t tcg_to_map)
{
    uint32 ps, priority_i, priority_offset, allocation_bmp;
    int base_q_pair, nof_priorities;
    dnx_egr_queuing_tcg_info_t tcg_info;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_ofp_tcg_get(unit, core, logical_port, &tcg_info));
    if (priority >= dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "priority (%d) is larger than allowed (%d).",
                     priority, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit) - 1);
    }
    tcg_info.tcg_ndx[priority] = tcg_to_map;
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_to_tcg_mapping_set_verify(unit, logical_port, priority, &tcg_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (nof_priorities > dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Nof_priorities (%d) is larger than the maximum allowed (%d).",
                     nof_priorities, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    if (DNX_EGR_NOF_Q_PRIO != dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "DNX_EGR_NOF_Q_PRIO (%d) is is not equal to its setup in DATA (%d).",
                     DNX_EGR_NOF_Q_PRIO, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    ps = base_q_pair / DNX_NOF_TCGS;
    priority_offset = base_q_pair - (ps * DNX_NOF_TCGS);
    SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.get(unit, core, ps, &allocation_bmp));
    for (priority_i = 0; priority_i < DNX_NOF_TCGS_IN_PS; ++priority_i)
    {
        /*
         * Map the TCs which belong to port with given TCG mapping
         *
         * Set the variable: DBAL_FIELD_PRIO_MAP_ELEMENT: Mapping for all updated TCGs
         */
        if ((priority_i >= priority_offset) && (priority_i < (priority_offset + nof_priorities)))
        {
            /*
             * Only indicated 'priority' is updated in HW. All others, within this priority group'
             * are assumed to be consistent with it (See dnx_ofp_rates_ofp_tcg_set_verify()).
             */
            if (priority == (priority_i - priority_offset))
            {
                SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_QPAIR_TCG_MAP,
                                                 2, 1,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_PORT_SCHEDULER, ps,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_PRIO_MAP_ELEMENT, priority_i,
                                                 tcg_to_map, GEN_DBAL_FIELD_LAST_MARK));
            }
        }
        else if (allocation_bmp != 0xFF)
        {
            /*
             * If the TCs which do not belong to port are not mapped, they will need to be mapped
             * to default (which out of the range of the port TCG)
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_QPAIR_TCG_MAP, 2, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_PORT_SCHEDULER, ps,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_PRIO_MAP_ELEMENT, priority_i, priority_i,
                                             GEN_DBAL_FIELD_LAST_MARK));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * For details, see header file egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_tc_to_tcg_mapping_get(
    int unit,
    bcm_port_t logical_port,
    uint32 priority,
    dnx_tcg_ndx_t * tcg_to_map_p)
{
    uint32 priority_offset, ps;
    int base_q_pair;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    if (priority >= dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "priority (%d) is larger than allowed (%d).",
                     priority, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit) - 1);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    ps = base_q_pair / DNX_NOF_TCGS;
    priority_offset = base_q_pair - (ps * DNX_NOF_TCGS);
    if ((priority + priority_offset) >= dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "priority (%d) is larger than allowed (%d).",
                     priority, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit) - 1);
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPING_QPAIR_TCG_MAP,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_PORT_SCHEDULER, ps,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_PRIO_MAP_ELEMENT, (priority + priority_offset),
                                     tcg_to_map_p, GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
shr_error_e
dnx_ofp_rates_ofp_tcg_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    dnx_egr_queuing_tcg_info_t * tcg_info)
{
    uint32 priority_i;
    int base_q_pair, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(tcg_info, 0, sizeof(*tcg_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Nof_priorities (%d) is smaller than the minimum allowed (%d).",
                     nof_priorities, dnx_data_egr_queuing.params.tcg_min_priorities_get(unit));
    }
    if (nof_priorities > dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Nof_priorities (%d) is larger than the maximum allowed (%d).",
                     nof_priorities, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    if (DNX_EGR_NOF_Q_PRIO != dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "DNX_EGR_NOF_Q_PRIO (%d) is not equal to its setup in DATA (%d).",
                     DNX_EGR_NOF_Q_PRIO, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_to_tcg_mapping_get
                        (unit, logical_port, priority_i, &(tcg_info->tcg_ndx[priority_i])));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * Verify procedure for dnx_ofp_rates_tcg_weight_get() and for dnx_ofp_rates_tcg_weight_set().
 * See header of the latter for description of input parameters.
 */
static int
dnx_ofp_rates_tcg_weight_set_verify(
    int unit,
    int core,
    dnx_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight)
{
    int nof_priorities;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * API functionality only when port is with 8 priorities.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "nof_priorities (%d) is smaller than minimum allowed (%d)",
                     nof_priorities, dnx_data_egr_queuing.params.tcg_min_priorities_get(unit));
    }

    /*
     * Verify TCG
     */
    if (((int) (tcg_ndx) < (DNX_TCG_MIN)) || ((tcg_ndx) > (DNX_TCG_MAX)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tcg_ndx (%d) is out of range (%d,%d).", tcg_ndx, DNX_TCG_MIN, DNX_TCG_MAX);
    }
    /*
     * Verify TCG weight
     */
    if (tcg_weight->tcg_weight_valid)
    {
        if (((int) (tcg_weight->tcg_weight) < (DNX_EGQ_TCG_WEIGHT_MIN))
            || ((tcg_weight->tcg_weight) > (DNX_EGQ_TCG_WEIGHT_MAX)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tcg_weight (%d) is out of range (%d,%d).",
                         tcg_weight->tcg_weight, DNX_EGQ_TCG_WEIGHT_MIN, DNX_EGQ_TCG_WEIGHT_MAX);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
int
dnx_ofp_rates_tcg_weight_set(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight)
{
    int base_q_pair;
    uint32 field_val;
    int core;
    int ps, tcg_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_weight_set_verify(unit, core, logical_port, tcg_ndx, tcg_weight));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Set TCG weight valid
     */
    field_val = tcg_weight->tcg_weight_valid ? 0 : 1;
    ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_TCG_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TCG_INDEX, (uint32) tcg_id,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_WFQ_TCG_DIS, INST_SINGLE, (uint32) (field_val),
                                     GEN_DBAL_FIELD_LAST_MARK));
    if (tcg_weight->tcg_weight_valid)
    {
        /*
         * Set TCG weight only in case of valid
         */
        uint32 ps;

        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        field_val = tcg_weight->tcg_weight;
        /*
         * Update only the weight that corresponds to indicated TCG index (0 - 7).
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_TCG_WEIGHTS,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PORT_SCHEDULER, (uint32) (ps),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_WEIGHT_OF_TCG, tcg_ndx, (uint32) (field_val),
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
int
dnx_ofp_rates_tcg_weight_get(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight)
{
    uint32 field_val;
    int base_q_pair;
    int core;
    int ps, tcg_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_weight_set_verify(unit, core, logical_port, tcg_ndx, tcg_weight));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Get TCG weight valid
     */
    ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_TCG_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TCG_INDEX, (uint32) tcg_id,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_WFQ_TCG_DIS, INST_SINGLE, (&field_val),
                                     GEN_DBAL_FIELD_LAST_MARK));
    tcg_weight->tcg_weight_valid = field_val ? 0 : 1;
    if (tcg_weight->tcg_weight_valid)
    {
        /*
         * Get TCG weight only in case of valid
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        /*
         * Get the weight that corresponds to indicated TCG index (0 - 7).
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPING_TCG_WEIGHTS,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PORT_SCHEDULER, (uint32) (ps),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_WEIGHT_OF_TCG, tcg_ndx, (&field_val),
                                         GEN_DBAL_FIELD_LAST_MARK));
        tcg_weight->tcg_weight = field_val;
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/*
 * Procedures related to 'max_burst'
 * {
 */
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_ofp_rates_fc_queues_max_burst_get(
    int unit,
    uint32 *max_burst_fc_queues_p)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(max_burst_fc_queues_p, _SHR_E_PARAM, "max_burst_fc_queues_p");
    /*
     * Max burst for fc queues {
     */
    core = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /*
                                      * key construction.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /*
                                      * Get field val for DBAL_FIELD_FC_QP_MAX_CREDIT field
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_FC_QP_MAX_CREDIT, INST_SINGLE,
                                     max_burst_fc_queues_p, GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Max burst for fc queues }
     */
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_ofp_rates_empty_queues_max_burst_set(
    int unit,
    uint32 max_burst_empty_queues)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    if (max_burst_empty_queues >
        (DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX * dnx_data_egr_queuing.params.cal_burst_res_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_burst_empty_queues (%d) is above max (%d)",
                     max_burst_empty_queues,
                     (DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX * dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    }
    /*
     * Max burst for empty queues {
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * Enabling/Disabling shapers
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 2,
                                         /*
                                          * key construction.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /*
                                          * Set field val for DBAL_FIELD_EMPTY_QP_STOP_COLLECTING_EN
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EMPTY_QP_STOP_COLLECTING_EN, INST_SINGLE,
                                         (uint32) (max_burst_empty_queues ? 1 : 0),
                                         /*
                                          * Setting shapers
                                          *
                                          * Set field val for DBAL_FIELD_EMPTY_QP_MAX_CREDIT
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EMPTY_QP_MAX_CREDIT, INST_SINGLE,
                                         (uint32) (max_burst_empty_queues), GEN_DBAL_FIELD_LAST_MARK));
    }
    /*
     * Max burst for empty queues }
     */
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_ofp_rates_empty_queues_max_burst_get(
    int unit,
    uint32 *max_burst_empty_queues_p)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(max_burst_empty_queues_p, _SHR_E_PARAM, "max_burst_empty_queues_p");
    /*
     * Max burst for empty queues {
     */
    core = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /*
                                      * key construction.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /*
                                      * Get field val for DBAL_FIELD_EMPTY_QP_MAX_CREDIT field
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EMPTY_QP_MAX_CREDIT, INST_SINGLE,
                                     max_burst_empty_queues_p, GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Max burst for empty queues }
     */
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_ofp_rates_fc_queues_max_burst_set(
    int unit,
    uint32 max_burst_fc_queues)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    if (max_burst_fc_queues >
        (DNX_OFP_RATES_BURST_FC_Q_LIMIT_MAX * dnx_data_egr_queuing.params.cal_burst_res_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "max_burst_fc_queues (%d) is above max (%d)",
                     max_burst_fc_queues,
                     (DNX_OFP_RATES_BURST_FC_Q_LIMIT_MAX * dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    }
    /*
     * Max burst for fc queues {
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * Enabling/Disabling shapers
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 2,
                                         /*
                                          * key construction.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /*
                                          * Set field val for DBAL_FIELD_FC_QP_STOP_COLLECTING_EN
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FC_QP_STOP_COLLECTING_EN, INST_SINGLE,
                                         (uint32) (max_burst_fc_queues ? 1 : 0),
                                         /*
                                          * Setting shapers for FC (flow controlled)queues.
                                          * Set field val for DBAL_FIELD_FC_QP_MAX_CREDIT
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FC_QP_MAX_CREDIT, INST_SINGLE,
                                         (uint32) (max_burst_fc_queues), GEN_DBAL_FIELD_LAST_MARK));
    }
    /*
     * Max burst for fc queues }
     */
exit:
    SHR_FUNC_EXIT;
}
/*
 * See cosq_egq.h
 */
shr_error_e
dnx_egq_ofp_rates_port_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    int core;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    SHR_IF_ERR_EXIT(dnx_cosq_egq_user_port_get(unit, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d\nCosq must be set to 0 in case of Local gport", cosq);
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_single_port_rate_hw_get(unit, core, logical_port, kbits_sec_max));
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}
/**
 *  \brief
 *    Verify function for dnx_egq_ofp_rates_port_bandwidth_set().
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Scheduler port in the format of gport.
 * \param [in] cosq -
 *   Not used. Must be set to '0'.
 * \param [in] kbits_sec_min -
 *   Not used. Must be set to '0'.
 * \param [in] kbits_sec_max -
 *   Maximal bandwidth in kbits/sec
 * \param [in] flags -
 *   Bitmap. Allowed values:
 *     BCM_COSQ_BW_NOT_COMMIT (See documentation on definition)
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egq_ofp_rates_port_bandwidth_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid parameter: cosq: %d\n" "Cosq must be set to 0 in case of Local gport", cosq);
    }
    /*
     * Currently only BCM_COSQ_BW_NOT_COMMIT flag is supported
     */
    if ((flags & ~(BCM_COSQ_BW_NOT_COMMIT)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid parameter: flags: 0x%08X\n"
                     "The only supported flag is 0x%08X", flags, (uint32) BCM_COSQ_BW_NOT_COMMIT);
    }
    if (kbits_sec_max > DNX_IF_MAX_RATE_KBPS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid parameter: kbits_sec_max: %d. Maximal value allowed is %d",
                     kbits_sec_max, DNX_IF_MAX_RATE_KBPS(unit) - 1);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See cosq_egq.h
 */
shr_error_e
dnx_egq_ofp_rates_port_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    uint32 tm_port;
    bcm_port_t logical_port;
    int core;
    int channelized;
    dnx_algo_port_info_s port_info;
    int base_q_pair;
    int calendar_id;
    uint32 prev_kbits_sec_max, total_rate;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_egq_ofp_rates_port_bandwidth_set_verify
                          (unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));

    /** Get port information */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_user_port_get(unit, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));

    /** Mark calendar as modified */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.modified.set(unit, core, calendar_id, TRUE));

    /** Setting rate to sw state - In case of non-egq ports - do nothing */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.rate.get(unit, core, base_q_pair, &prev_kbits_sec_max));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.rate.set(unit, core, base_q_pair, kbits_sec_max));

        /** Mark port as valid */
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.valid.set(unit, core, base_q_pair, TRUE));

        /** For the non-channelized interface the rate will be used for calcal calculation */
        if (!channelized)
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.get(unit, core, calendar_id, &total_rate));
            if (total_rate < prev_kbits_sec_max)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "total_rate(=%d) <  prev_kbits_sec_max(=%d)", total_rate,
                             prev_kbits_sec_max);
            }

            /** total rate should hold all none channelized interfaces rate. */
            /** so we reduce the previous rate (rate=0 during init) of the current interface and add the new rate. */
            total_rate = total_rate - prev_kbits_sec_max + kbits_sec_max;
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.set(unit, core, calendar_id, total_rate));
        }
        if (!(flags & BCM_COSQ_BW_NOT_COMMIT))
        {
            /*
             * Commit to HW here and not when calling the API with the same GPort type.
             * Iterate over cores. !BCM_COSQ_BW_NOT_COMMIT affect all cores.
             */
            bcm_pbmp_t pbmp;
            int nof_cores;
            uint8 if_recalc;
            uint32 cal_is_modified;
            uint32 nof_instances;
            nof_instances = 0;
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            for (core = 0; core < nof_cores; core++)
            {
                /*
                 * Iterate over all ports to figure out whether calendar is changed on core and whether
                 * non-channelized interface rate is changed on core.
                 * 'if_recalc' is used to indicate whether we need to recalculate calcal and interface shapers.
                 * It will be set to TRUE in case of a non-channalized port, or in case we are setting rate
                 * for the first time, here (on init).
                 */
                if_recalc = FALSE;
                SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                                (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
                SOC_PBMP_ITER(pbmp, logical_port)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));
                    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
                    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.modified.get(unit, core, calendar_id, &cal_is_modified));
                    if (cal_is_modified)
                    {
                        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.nof_calcal_instances.get
                                        (unit, core, calendar_id, &nof_instances));
                        if (!channelized || (nof_instances <= 0))
                        {
                            /*
                             * For non-channalizeed port, or setting rate for the first time (on init)
                             * set if_recalc to TRUE
                             */
                            if_recalc = TRUE;
                            break;
                        }
                    }
                }
                /*
                 * Recalculate and set OTM shapers.
                 * If 'if_recalc' is TRUE then we need to recalculate calcal and interface shapers.
                 * Else copy calcal to the active calendar and recalculate only port shapers.
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_otm_shapers_set(unit, core, if_recalc));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
