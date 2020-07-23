/**
 * \file egq_ofp_rates.h
 * 
 * Internal DNX procedures related to ofp rates 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef EGQ_OFP_RATES_H_INCLUDED
/** { */
#define EGQ_OFP_RATES_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * This file was ported from:
 *   arad_ofp_rates.h
 *   jer_ofp_rates.h
 */
#include <bcm/types.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/*
 * Defines
 * {
 */
/**
 * Number of calendar sets applied in this system. Essentially, each calender has
 * 'DNX_OFP_RATES_NOF_CALS_IN_DUAL_MODE' instances ('referred to, also, as 'CALENDAR SET's)
 * so that only one of them is used at a time. This way, we can modify one instance
 * while traffic is using the other (and, then, switch to the modified instance in
 * one instruction).
 */
#define DNX_OFP_RATES_NOF_CALS_IN_DUAL_MODE    2
/**
 * Number of clocks to traverse a single calendar slot in the EGQ
 */
#define DNX_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS    (1)
/**
 * EGQ conversion coefficient: Number of credits per one bit for channelized arbiters
 */
#define DNX_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB             (dnx_data_egr_queuing.params.cal_res_get(unit))
/**
 * EGQ conversion coefficient: Number of credits per one bit for QPAIR and TCG
 */
#define DNX_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG            (dnx_data_egr_queuing.params.cal_res_get(unit))
/**
 * EGQ conversion coefficient: Number of credits per one bit for Interface and for Recycling/Mirroring
 */
#define DNX_EGQ_UNITS_VAL_IN_BITS_INTF_RECYCLE         (dnx_data_egr_queuing.params.cal_res_get(unit))
/**
 * Maximal number of bytes per burst. This value is converted, by multiplying 
 * by 'dnx_data_egr_queuing.params.cal_burst_res', before loading into HW.
 * The latter is, then, a conversion coefficient between credits and bits. 
 */
#define DNX_EGQ_MAX_BURST_IN_BYTES                     0x4000
/*
 * Scheduler-related definition:
 */
#define DNX_OFP_RATES_CAL_LEN_SCH_MAX                  0x400
/**
 * Calender maximum length (number of slots) for 'channelized arbiters'
 */
#define DNX_OFP_RATES_CAL_SIZE_EGQ_CHAN_ARB_MAX        (DNX_DATA_MAX_EGR_QUEUING_PARAMS_CHAN_ARB_CALENDAR_SIZE)
/*
 * Maximal number of slots in calendar used for shaping 'qpairs'. Also referred to
 * as TC (or PRIORITY) calendar.
 */
#define DNX_OFP_RATES_CAL_SIZE_EGQ_PORT_PRIO_MAX       (DNX_DATA_MAX_EGR_QUEUING_PARAMS_PORT_PRIO_CALENDAR_SIZE)
/*
 * Maximal number of slots in calendar used for shaping TCG (Group of 'qpairs', TC Group)
 */
#define DNX_OFP_RATES_CAL_SIZE_EGQ_TCG_MAX             (DNX_DATA_MAX_EGR_QUEUING_PARAMS_TCG_CALENDAR_SIZE)
/*
 * Maximal number of slots in 'calcal' which is an 'indirect calendar', pointing to both 'interface'
 * calendar and to 'channelized arbiters' calendar. (The latter is pointed via an intermediary
 * table which contains, per interface, the index of the top slot in 'channelized arbiter' calendar.
 */
#define DNX_OFP_RATES_CALCAL_LEN_EGQ_MAX               (dnx_data_egr_queuing.params.cal_cal_len_get(unit))      /* 0x0100 
                                                                                                                 */
/**
 * This definition will probably not be needed after porting is done.
 */
#define DNX_EGQ_NIF_SCM_TBL_ENTRY_SIZE                 3
#define DNX_EGQ_PMC_TBL_ENTRY_SIZE                     3
#define DNX_EGQ_DWM_TBL_ENTRY_SIZE                     1
#define DNX_EGQ_ERPP_PER_PORT_TBL_ENTRY_SIZE           5
/**
 * Have a specific offset for TCG ID
 */

#define DNX_OFP_RATES_TCG_ID_GET(ps,tcg_ndx) \
  (ps*DNX_NOF_TCGS_IN_PS + tcg_ndx)
/**
 * Maximal value allowed for burst on any port, in bytes.
 * HW allows for a much larger value
 */
#define DNX_OFP_RATES_BURST_LIMIT_MAX                  (0xFFFF)
/**
 * Default value for burst on recycle/mirror traffic shaper, in credits.
 * To convert to bytes, divide by 'dnx_data_egr_queuing.params.cal_burst_res_get(unit)'
 */
#define DNX_OFP_RATES_BURST_DEFAULT                    0x4000
/**
 * Maximal value allowed for burst on any port, which is in 'flow control' state, in bytes.
 * HW allows for a much larger value
 */
#define DNX_OFP_RATES_BURST_FC_Q_LIMIT_MAX             (0x3FFF)
/**
 * Maximal value allowed for burst on any port, which is in 'empty' state, in bytes.
 * HW allows for a much larger value
 */
#define DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX          (0x3FFF)
/*
 *  The coefficient to convert 1Kilo-bit-per-second to bit-per-second (e.g.).
 */
#define DNX_RATE_1K                                    1000
/*
 * Maximal interface rate, in Mega-bits per second.
 * This is the upper boundary, it can be lower
 *  depending on the credit size
 */
#define DNX_IF_MAX_RATE_MBPS_DNX(unit)                (DNX_RATE_1K * dnx_data_egr_queuing.params.max_gbps_rate_egq_get(unit))
/**
 * Maximal interface rate, in Kilo-bits per second.
 * This is the upper boundary. In operation, it can be lower
 *  depending on the credit size
 */
#define DNX_IF_MAX_RATE_KBPS(unit)                    (DNX_RATE_1K * DNX_IF_MAX_RATE_MBPS_DNX(unit))
/*
 *  Setting  this value as maximal burst will result in no burst limitation
 */
#define DNX_INVALID_CALENDAR      0xff
/*
 * }
 */
/*
 * Typedefs
 * {
 */
/**
 * A single entry of a general calendar table. 
 * Consists of: 'port' to handle, number of credits to provide it with
 * and maximal burst size.
 * For now, credit table can be:
 *   DBAL_TABLE_EGQ_SHAPING_OTM_HP_CRDT_TABLE
 *   DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE
 *   DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE
 */
typedef struct
{
    uint32 port_cr_to_add;
    uint32 ofp_index;
    /**
     * Maximal number of credit for specified slot (port).
     * This is the 'burst size'
     */
    uint32 port_max_credit;
} dnx_egq_calendar_tbl_data_t;
/**
 * A single entry of OFP calendar - EGQ. 
 * Consists of: base_q_pair and credit. 
 */
typedef struct
{
    uint32 base_q_pair;
    uint32 credit;
} dnx_ofp_egq_rates_cal_entry_t;
typedef struct
{
    uint32 interface_select;
} dnx_egq_ccm_tbl_data_t;
typedef enum
{
  /**
   *  Calendar Set A - scheduler and EGQ.
   */
    DNX_OFP_RATES_CAL_SET_A = 0,
  /**
   *  Calendar Set B - scheduler and EGQ.
   */
    DNX_OFP_RATES_CAL_SET_B,
  /**
   *  Total number of calendar sets.
   */
    DNX_OFP_NOF_RATES_CAL_SETS
} dnx_ofp_rates_cal_set_e;

typedef enum
{
    DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE = 0,
    DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST,
    DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN,
    DNX_OFP_RATES_EGQ_CHAN_ARB_NOF_FIELD_TYPE
} dnx_ofp_rates_egq_chan_arb_field_type_e;

/**
 * A single entry of OFP calendar - SCH. 
 * Consists of: base_port_tc. 
 */
typedef uint32 dnx_ofp_sch_rates_cal_entry_t;

typedef enum
{
    FIRST_DNX_OFP_RATES_EGQ_CAL_TYPE = 0,
    /**
     * Represents Channelize arbiter calendar
     */
    DNX_OFP_RATES_EGQ_CAL_CHAN_ARB = FIRST_DNX_OFP_RATES_EGQ_CAL_TYPE,
    /**
     * Represents TCG calendar
     */
    DNX_OFP_RATES_EGQ_CAL_TCG,
    /**
     * Represents Port Priority calendar
     */
    DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY,
    /**
     * Number of calendar types
     */
    NUM_DNX_OFP_RATES_EGQ_CAL_TYPE
} dnx_ofp_rates_egq_cal_type_e;

typedef struct
{
    /**
     *  Calendar type.   
     */
    dnx_ofp_rates_egq_cal_type_e cal_type;
    /**
     *  Additional calendar information:
     *  Currently, indicates 'channelized arbiter id' in case of
     *  Channelized calendar (CHAN_ARB).
     */
    uint32 chan_arb_id;
    /**
     *  Additional calendar information:
     *  Relevant for CHAN_ARB and determines the slot_id in which
     *  the calendar is started.
     */
    uint32 arb_slot_id;
} dnx_ofp_rates_cal_info_t;
/**
 * EGQ calendar for all calendar types. For example, 'channelized arbiter' (Channelized interface,
 * which is composed of a collection of ports).
 * Note that the number of 'slots' is the maximal values over all calendar sizes.
 */
typedef struct
{
    dnx_ofp_egq_rates_cal_entry_t slots[DNX_DATA_MAX_EGR_QUEUING_COMMON_MAX_VAL_CALENDAR_SIZE];
} dnx_ofp_rates_cal_egq_t;
/**
 *  Scheduler calendar.
 *  Note: credit is always '1' for scheduler calendar.
 */
typedef struct
{
    dnx_ofp_sch_rates_cal_entry_t slots[DNX_OFP_RATES_CAL_LEN_SCH_MAX];
} dnx_ofp_rates_cal_sch_t;
typedef enum
{
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE = 0,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE = 1,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST = 2,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST = 3,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE = 4,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_RATE = 5,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST = 6,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_BURST = 7,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_EMPTY_Q_BURST = 8,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_FC_Q_BURST = 9
} dnx_ofp_rates_generic_func_state_e;
typedef enum
{
    FIRST_DNX_OFP_RATES_EGQ_CHAN_ARB_ID = 0,
    DNX_OFP_RATES_EGQ_CHAN_ARB_00 = FIRST_DNX_OFP_RATES_EGQ_CHAN_ARB_ID,
    DNX_OFP_RATES_EGQ_CHAN_ARB_01,
    DNX_OFP_RATES_EGQ_CHAN_ARB_02,
    DNX_OFP_RATES_EGQ_CHAN_ARB_03,
    DNX_OFP_RATES_EGQ_CHAN_ARB_04,
    DNX_OFP_RATES_EGQ_CHAN_ARB_05,
    DNX_OFP_RATES_EGQ_CHAN_ARB_06,
    DNX_OFP_RATES_EGQ_CHAN_ARB_07,
    DNX_OFP_RATES_EGQ_CHAN_ARB_CPU,
    DNX_OFP_RATES_EGQ_CHAN_ARB_08 = DNX_OFP_RATES_EGQ_CHAN_ARB_CPU,
    DNX_OFP_RATES_EGQ_CHAN_ARB_RCY,
    DNX_OFP_RATES_EGQ_CHAN_ARB_09 = DNX_OFP_RATES_EGQ_CHAN_ARB_RCY,
    DNX_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN,
    DNX_OFP_RATES_EGQ_NOF_CHAN_ARB,
    /**
     * Number of calendar types
     */
    NUM_DNX_OFP_RATES_EGQ_CHAN_ARB_ID,
    DNX_OFP_RATES_EGQ_CHAN_ARB_INVALID = DNX_INVALID_CALENDAR
} dnx_ofp_rates_egq_chan_arb_id_e;
/*
 * }
 */
/*
 * Procedure headers
 * {
 */
/*
 * JR2 calender algorithm {
 */

/**
 * \brief
 *   Given calendar info and number of credits per port, get,
 *   per implied port, the corresponding rate (in kilo bits per sec).
 * \param [in] unit -
 *   int. Identifier of HW platform.
 * \param [in] calcal_length -
 *   uint32. Number of 'slots' on 'calcal'. See 'remark' below.
 * \param [in] calcal_instances -
 *   uint32. Number of times indicated port appears on 'calcal'. See 'remark' below.
 * \param [in] calendar_len -
 *   uint32. Number of active slots on calendar where indicated port (for which we
 *   require this conversion) redides.
 * \param [in] packet_mode - indication if port_rate_p output is in units of PPS (packet_mode = 1) or in units of Kbps (packet_mode = 0)
 * \param [in] egq_resolution -
 *   uint32. 
 *   Conversion coefficient bits to credits. This is the number of credits required, for
 *   the system, so that one bit is transmitted.
 * \param [in,out] port_rate_p -
 *   Pointer to uint32. 
 *   As input,
 *     This is the number of credits that are made available every full cycle of
 *     the calendar and which are required to be converted to Kilo Bits Per Second
 *   As output,
 *     This is the resultant rate in Kilo Bits Per Second or packets per second (see packet_mode)
 *   Note:
 *     If input number of credits is '0' then output rate is also set to '0'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   The model for the calculation is:
 *     a. There is an external calendar cycle which is named 'calcal'.
 *     b. The number of entries (slots) on 'calcal' is 'calcal_length'. This is the
 *        size of one calcal cycle.
 *     c. A 'calcal entry' relates to a 'port' and points to the calendar of that port
 *        of size 'calendar_len'.
 *     d. A single port may appear a few times on 'calcal'. The number of times is
 *        referred to as 'calcal_instances'.
 *     e. In one cycle of 'calcal', each port gets a chance to activate one slot
 *        (out of 'calendar_len' slots).
 *   Indirect input:
 *     DNXCMN_CORE_CLOCK_KHZ_GET -
 *       The clock of the system (in kilo cycles per second)
 *     DNX_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS -
 *       The number of the clocks required, by the system, to cross one slot.
 * \see
 *   * dnx_ofp_rates_egq_shaper_rate_from_internal
 *   * dnx_ofp_rates_calculate_credits_from_rate
 */
shr_error_e dnx_ofp_rates_calculate_rate_from_credits(
    int unit,
    uint32 calcal_length,
    uint32 calcal_instances,
    uint32 calendar_len,
    int packet_mode,
    uint32 egq_resolution,
    uint32 *port_rate_p);
/**
 * \brief
 *   Given calendar info and rate (in KBPS) per port, get, per implied port,
 *   the corresponding numbner of credits it would get per one cycle of its calendar.
 * \param [in] unit -
 *   int. Identifier of HW platform.
 * \param [in] calcal_length -
 *   uint32. Number of 'slots' on 'calcal'. See 'remark' below.
 * \param [in] calcal_instances -
 *   uint32. Number of times indicated port appears on 'calcal'. See 'remark' below.
 * \param [in] calendar_len -
 *   uint32. Number of active slots on calendar where indicated port (for which we
 *   require this conversion) redides.
 * \param [in] egq_resolution -
 *   uint32. 
 *   Conversion coefficient bits to credits. This is the number of credits required, for
 *   the system, so that one bit is transmitted.
 * \param [in] packet_mode -
 *   uint32. Flag.
 *   If '0', rate is for DATA mode, in units of Kilo Bits Per Second.
 *   Otherwise, rate is for PACKET mode, in units of Packets Per Second.
 * \param [in] rate_kbps -
 *   uint32. 
 *   This is the rate in Kilo Bits Per Second which is required to be converted to
 *   internal units (credits per calendar cycle). If this input is '0' then output,
 *   (in *port_credits_p) is also '0'.
 * \param [out] port_credits_p -
 *   Pointer to uint32. 
 *   This is the resultant number of credits that are made available every full cycle of
 *   the calendar. 
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   The model for the calculation is:
 *     a. There is an external calendar cycle which is named 'calcal'.
 *     b. The number of entries (slots) on 'calcal' is 'calcal_length'. This is the
 *        size of one calcal cycle.
 *     c. A 'calcal entry' relates to a 'port' and points to the calendar of that port
 *        of size 'calendar_len'.
 *     d. A single port may appear a few times on 'calcal'. The number of times is
 *        referred to as 'calcal_instances'.
 *     e. In one cycle of 'calcal', each port gets a chance to activate one slot
 *        (out of 'calendar_len' slots).
 *   Indirect input:
 *     DNXCMN_CORE_CLOCK_KHZ_GET -
 *       The clock of the system (in kilo cycles per second)
 *     DNX_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS -
 *       The number of the clocks required, by the system, to cross one slot.
 * \see
 *   * dnx_ofp_rates_egq_shaper_rate_to_internal
 *   * dnx_ofp_rates_calculate_rate_from_credits
 */
shr_error_e dnx_ofp_rates_calculate_credits_from_rate(
    int unit,
    uint32 calcal_length,
    uint32 calcal_instances,
    uint32 calendar_len,
    uint32 egq_resolution,
    uint32 packet_mode,
    uint32 rate_kbps,
    uint32 *port_credits_p);
/*
 * }
 */

int dnx_ofp_rates_init(
    int unit);

int dnx_ofp_rates_deinit(
    int unit);

/* 
 * \brief -
 *   Update HW for specified interface with specified rate (in internal
 *   units).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to apply new setup for.
 * \param [in] calendar_id -
 *   Egress calendar id
 * \param [in] internal_rate -
 *   Rate, in internal units, to apply to specified interface on HW.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated HW:
 *         See DBAL_TABLE_EGQ_SHAPER_OTM_CALENDAR_CRDT_TABLE, 
 *         DBAL_FIELD_OTM_CAL_CRDT_TO_ADD
 * \see
 *   * dnx_ofp_rates_if_calendar_internal_rate_get()
 */
shr_error_e dnx_ofp_rates_if_calendar_internal_rate_set(
    int unit,
    int core,
    int calendar_id,
    uint32 internal_rate);

int dnx_ofp_rates_calcal_config(
    int unit,
    int core);
/* 
 * \brief -
 *   Function to re-calculate and set the CalCal and all OTM shapers.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to apply new setup for.
 * \param [in] if_recalc -
 *   If parameter 'if_recalc' is TRUE then recalculate calcal
 *   and interface shapers.
 *   Else copy calcal to the active calendar and recalculate only
 *   port shapers.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated sw state: dnx_ofp_rate_db
 *   * * Updated HW
 * \see
 *   * dnx_ofp_rates_egq_single_port_rate_sw_get()
 */
shr_error_e dnx_ofp_rates_egq_otm_shapers_set(
    int unit,
    int core,
    uint8 if_recalc);

shr_error_e dnx_ofp_rates_egq_interface_shaper_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *if_shaper_rate);

shr_error_e dnx_ofp_rates_if_calendar_internal_rate_get(
    int unit,
    int core,
    int calendar_id,
    uint32 *internal_rate);
/* 
 * \brief -
 *   Given logical port, get the rate that is assigned to it, from HW, in Kilo
 *   Bits Per Second. (Actually, HW is marked by the number of credits assigned
 *   to this port and that is converted to KBPS using calendar parameters).
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] core -
 *   int. Core to apply new setup for.
 * \param [in] logical_port -
 *   bcm_port_t. 'Logical port' to get 'rate' for.
 * \param [out] rate -
 *   Pointer to uint32. Rate assigned to this port, as retrieved from HW.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect input:
 *   * Name of credit table:
 *         See, for example, DBAL_TABLE_EGQ_SHAPING_OTM_CRDT_TABLE, 
 *         DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE, DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE
 * \see
 *   * dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate()
 *   * dnx_ofp_rates_calendar_entry_get()
 */
shr_error_e dnx_ofp_rates_egq_single_port_rate_hw_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 *rate);
/* 
 * \brief -
 *   Given logical port, get the rate that is assigned to it, from SW state, in Kilo
 *   Bits Per Second. 
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] core -
 *   int. Core to apply new setup for.
 * \param [in] logical_port -
 *   bcm_port_t. 'Logical port' to get 'rate' for.
 * \param [out] rate -
 *   Pointer to uint32. Rate assigned to this port, as retrieved from SW state.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect input:
 *   * Name of credit table:
 *         See, for example, DBAL_TABLE_EGQ_SHAPING_OTM_CRDT_TABLE, 
 *         DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE, DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE
 * \see
 *   * dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate()
 *   * dnx_ofp_rates_calendar_entry_get()
 */
shr_error_e dnx_ofp_rates_egq_single_port_rate_sw_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 *rate);
/**
 * \brief
 *   Given 'logical port' and priority ('priority_ndx') to identify a specific qpair,
 *   get count of all qpairs which are attached to any logical port and, if input
 *   logical port is not there, add it to the count.
 *   Used to verify that adding a qpair for input logical port (plus priority) is
 *   witin limits.
 * \param [in] unit -
 *   int. HW Unit-ID
 * \param [in] core -
 *   int. Core to use for identifying qpairs. Only qpairs related to this core
 *   are referenced.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port used to get 'base qpair'. The latter is used,
 *   together with 'priority_ndx', to identify the new qpair which is to be added 
 *   (In case iot is not already there).
 * \param [in] priority_ndx -
 *   uint32. See description in 'logical_port' above
 * \param [out] nof_valid_queues -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated 'total
 *   number of qpairs'
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_egq_port_priority_shaper_verify
 */
shr_error_e dnx_ofp_rates_sw_db_port_priority_nof_valid_queues_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 *nof_valid_queues);
/* 
 * \brief -
 *   A calendar is made out of slots, each corresponding to a port (base qpair).
 *   Given 'tm_port', from which to deduce 'base qpair', and 'priority_ndx'
 *   which indicates priority on that 'base qpair',
 *   set indicated maximal burst size (in Bytes) on all slots which
 *   correspond to input 'base qpair' (on calendar corresponding to 'port priority'
 *   (DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY)).
 *   See 'remark' below.
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] core -
 *   int. Core to apply new setup for.
 * \param [in] logical_port -
 *   bcm_port_t. 'Logical port' to apply new 'maximal burst size' for.
 * \param [in] priority_ndx -
 *   uint32. Priority to assign new 'max_burst' to. This input is used to identify the
 *   target qpair (as key). Target qpair is ('base qpair' plus 'priority_ndx')
 * \param [in] max_burst -
 *   uint32. Maximal burst size, in Bytes, to assign to to indicated priority
 *   on specified port.
 *   Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated HW:
 *         See, for example, DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE, 
 *         DBAL_FIELD_QPAIR_MAX_BURST
 *   * To convert KBPS to credits, multiply by dnx_data_egr_queuing.params.cal_burst_res_get(unit)
 * \see
 *   * dnx_cosq_control_egress_port_tc_set()
 *   * dnx_ofp_rates_max_burst_generic_set()
 */
shr_error_e dnx_ofp_rates_egq_port_priority_max_burst_set(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 max_burst);
/* 
 * \brief -
 *   A calendar is made out of slots, each corresponding to a port (base qpair).
 *   Given 'tm_port', from which to deduce 'base qpair', and 'priority_ndx'
 *   which indicates priority on that 'base qpair',
 *   get indicated maximal burst size (in Bytes) a slot which corresponds to
 *   input 'base qpair' (on calendar corresponding to 'port priority'
 *   (DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY)).
 *   See 'remark' below.
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] core -
 *   int. Core to retrieve setup for.
 * \param [in] logical_port -
 *   bcm_port_t. 'Logical port' to retrieve 'maximal burst size' for.
 * \param [in] priority_ndx -
 *   uint32. Priority to get 'max_burst' for. This input is used to identify the
 *   target qpair (as key). Target qpair is ('base qpair' plus 'priority_ndx')
 * \param [out] max_burst -
 *   Pointer to uint32. This procedure loads pointed memory by 'maximal burst size',
 *   in Bytes, assigned to to indicated priority on specified port.
 *   Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated HW:
 *         See, for example, DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE, 
 *         DBAL_FIELD_QPAIR_MAX_BURST
 *   * To convert KBPS to credits, multiply by dnx_data_egr_queuing.params.cal_burst_res_get(unit)
 * \see
 *   * dnx_cosq_control_egress_port_tc_set()
 *   * dnx_ofp_rates_max_burst_generic_set()
 */
shr_error_e dnx_ofp_rates_egq_port_priority_max_burst_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 *max_burst);
/* 
 * \brief -
 *   Given logical port (to identify base qpair and, consequently, the
 *   corresponding port scheduler 'ps'), and index of TCG within ps,
 *   Set the maximal burst size (in bytes) for that TCG in HW (on
 *   all slots in TCG calendar, DNX_OFP_RATES_EGQ_CAL_TCG, which correspond
 *   to this TCG).
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port to apply new setup for. 
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. TC group, on this port, for which to apply input 'max_burst'.
 * \param [in] max_burst -
 *   uint32. Maximal burst size, in bytes, to assign to TCG on port, in HW.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If dnx_ofp_rate_db.shaper_enable is NOT set then this procedure will only do
 *     validity check.
 * \see
 *   * dnx_ofp_rate_db
 */
shr_error_e dnx_ofp_rates_egq_tcg_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_ndx,
    uint32 max_burst);
/* 
 * \brief -
 *   Given logical port (to identify base qpair and, consequently, the
 *   corresponding port scheduler, 'ps'), and index of TCG within ps,
 *   Get the maximal burst size (in bytes) for that TCG from HW (on
 *   one of the slots in TCG calendar, DNX_OFP_RATES_EGQ_CAL_TCG, which
 *   correspond to this TCG).
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port to apply new setup for. 
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. TC group, on this port, for which to apply input 'max_burst'.
 * \param [in] max_burst -
 *   Pointer to uint32. This procedure loads pointed memory by 'maximal burst size',
 *   in bytes, assigned to TCG on port, in HW.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If dnx_ofp_rate_db.shaper_enable is NOT set then this procedure will do
 *     nothing.
 * \see
 *   * dnx_ofp_rate_db
 */
shr_error_e dnx_ofp_rates_egq_tcg_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_ndx,
    uint32 *max_burst);
/*
 * Single port burst setting
 */
/* 
 * \brief -
 *   A calendar is made out of slots, each corresponding to a port (qpair).
 *   Given 'tm_port', from which to deduce 'qpair',
 *   set indicated maximal burst size (in Bytes) on all slots which
 *   correspond to input 'qpair' (on channelized calendar corresponding to
 *   'TM port' (DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)).
 *   See 'remark' below.
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] core -
 *   int. Core to apply new setup for.
 * \param [in] logical_port -
 *   bcm_port_t. 'Logical port' to apply new 'maximal burst size' for.
 * \param [in] max_burst -
 *   uint32. Maximal burst size, in Bytes, to assign to to indicated priority
 *   on specified port.
 *   Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated HW:
 *         See, for example, DBAL_TABLE_EGQ_SHAPING_OTM_HP_LP_CRDT_TABLE, 
 *         DBAL_FIELD_OTM_HP_MAX_BURST, DBAL_FIELD_OTM_LP_MAX_BURST
 *   * To convert Bytes to credits, multiply by dnx_data_egr_queuing.params.cal_burst_res_get(unit)
 * \see
 *   * dnx_cosq_control_egress_port_tc_set()
 *   * dnx_ofp_rates_max_burst_generic_set()
 */
shr_error_e dnx_ofp_rates_single_port_max_burst_set(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 max_burst);
/* 
 * \brief -
 *   A calendar is made out of slots, each corresponding to a port (qpair).
 *   Given 'tm_port', from which to deduce 'qpair',
 *   get maximal burst size (in Bytes) on a slot which corresponds to input
 *   'qpair' (on channelized calendar corresponding to 'TM port'
 *   (DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)).
 *   See 'remark' below.
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] core -
 *   int. Core to retrieve setup for.
 * \param [in] logical_port -
 *   bcm_port_t. 'Logical port' to retrieve 'maximal burst size' for.
 * \param [in] max_burst_p -
 *   Pointer to uint32. This procedure loads pointed memory by 'maximal burst size',
 *   in Bytes, assigned to to indicated priority on specified port.
 *   Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated HW:
 *         See, for example, DBAL_TABLE_EGQ_SHAPING_OTM_HP_LP_CRDT_TABLE, 
 *         DBAL_FIELD_OTM_HP_MAX_BURST, DBAL_FIELD_OTM_LP_MAX_BURST
 *   * To convert Bytes to credits, multiply by dnx_data_egr_queuing.params.cal_burst_res_get(unit)
 * \see
 *   * dnx_cosq_control_egress_port_tc_set()
 *   * dnx_ofp_rates_max_burst_generic_set()
 */
shr_error_e dnx_ofp_rates_single_port_max_burst_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 *max_burst_p);
/* 
 * \brief -
 *   Given port scheduler (ps) corresponding to input tm_port,
 *   Set the rate for the indicated 'tcg_ndx' on that 'ps'
 *   (in sw-state db).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use when accessing SW dataBase.
 * \param [in] logical_port -
 *   Logical port to apply new setup for. 
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. TC group, on this port, for which to apply input rate.
 * \param [in] tcg_rate -
 *   uint32. Rate to assign to TCG on port, in sw-state, in Kilo
 *   bps.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If dnx_ofp_rate_db.shaper_enable is not set then this procedure will only do
 *     validity check.
 * \see
 *   * dnx_ofp_rate_db
 */
shr_error_e dnx_ofp_rates_egq_tcg_rate_sw_set(
    int unit,
    int core,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 tcg_rate);
/* 
 * \brief -
 *   Given tm_port and priority, convert to 'qpair' and assign it specified rate
 *   in sw-state db.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port to apply new setup for. 
 * \param [in] prio_ndx -
 *   dnx_egr_q_prio_e. Priority assigned to this port.
 * \param [in] ptc_rate -
 *   uint32. Rate to assign to port, in sw-state, in Kilo
 *   bps.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If dnx_ofp_rate_db.shaper_enable is not set then this procedure will only do
 *     validity check.
 * \see
 *   * dnx_ofp_rate_db
 */
shr_error_e dnx_ofp_rates_egq_port_priority_rate_sw_set(
    int unit,
    bcm_port_t logical_port,
    dnx_egr_q_prio_e prio_ndx,
    uint32 ptc_rate);
/* 
 * \brief -
 *   Given logical_port and 'tcg index', convert to 'qpair' and get,
 *   from HW, its rate.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] logical_port -
 *   Logical port to use to calculate corresponding base qpair and,
 *   from it, the corresponding port scheduler. 
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. Index of TCG on the port scheduler corresponding to
 *   this port. Added to first qpair, on corresponding posrt scheduler,
 *   to get to the qpair to retrieve info for.
 *   Must be between DNX_TCG_MIN and DNX_TCG_MAX
 * \param [out] tcg_rate -
 *   uint32. This procedure loads pointed memory by the rate assigned
 *   to specified tcg_ndx on specified port, in HW, in Kilo
 *   bps.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If dnx_ofp_rate_db.shaper_enable is not set then this procedure will do
 *     nothing.
 * \see
 *   * dnx_ofp_rate_db.shaper_enable.get
 *   * dnx_ofp_rate_db
 */
shr_error_e dnx_ofp_rates_egq_tcg_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 *tcg_rate);
shr_error_e dnx_ofp_rates_egq_port_priority_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t ptc_ndx,
    uint32 *ptc_rate);
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
shr_error_e dnx_ofp_rates_egq_tcg_rate_hw_set(
    int unit);
/* 
 * \brief -
 *   Go through all ports, in sw-state. get corresponding 'base qpair' and 'number of priorities'.
 *   For each priority,
 *     Store the rate in 'egq_rates'
 *     Store 'base_q_pair + ptc_ndx' in local calendar array (egq_calendar->slots)
 *   Use partial info in constructed calendar to update 'credits' in local calendar.
 *   Use constructed calendar ro update HW on all cores.
 * \param [in] unit -
 *   HW identifier of unit.
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
shr_error_e dnx_ofp_rates_egq_port_priority_rate_hw_set(
    int unit);

/* 
 * \brief -
 *   Given logical port (related to OTM port), set 'header compensation' (Header
 *   Compensation is used to adjust for differences in the packet header size to
 *   account for network headers added by the network interface).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] logical_port -
 *   Logical port to apply setup to. See 'remarks'
 * \param [in] comp -
 *   Value, in bytes, to add to each egress transmitted packet for the sake of
 *   shaping calculations. Value is signed (from 63 to -64).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing table DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, and field
 *     DBAL_FIELD_CR_ADJUST_VALUE
 * \see
 *   * dnx_cosq_control_egress_port_scheduler_set
 */
shr_error_e dnx_egr_port_compensation_set(
    int unit,
    bcm_port_t logical_port,
    int comp);
/* 
 * \brief -
 *   Given logical port (related to OTM port), get 'header compensation' (Header
 *   Compensation is used to adjust for differences in the packet header size to
 *   account for network headers added by the network interface).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] logical_port -
 *   Logical port to get info for. See 'remarks'
 * \param [out] comp_p -
 *   Pointer to int. This procedure loads pointed memory by value, in bytes, to
 *   add to each egress transmitted packet for the sake of shaping calculations.
 *   Value is signed (from 63 to -64).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing table DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, and field
 *     DBAL_FIELD_CR_ADJUST_VALUE
 * \see
 *   * dnx_cosq_control_egress_port_scheduler_get
 */
shr_error_e dnx_egr_port_compensation_get(
    int unit,
    bcm_port_t logical_port,
    int *comp_p);
/**
 * \brief
 *   Get WFQ settings, for 'scheduled' and 'unscheduled' traffic, for a specificed
 *   logical port (queue) in the OFP.
 *   Qpair is calculated using base-qpair (related to logical port) plus input
 *   priority (cosq).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] cosq -
 *   uint32. Priority, relative to base qpair (logical port), for which to get WFQ
 *   parameters.
 * \param [in] wfq_info -
 *   Pointer to dnx_egr_ofp_sch_wfq_t. This procedure loads info from HW using the following
 *   element:
 *     sched_weight
 *     unsched_weight
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_DWM, fields UC_OR_UC_LOW_QUEUE_WEIGHT, MC_OR_MC_LOW_QUEUE_WEIGHT
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, fields MC_OR_MC_LOW_QUEUE_WEIGHT,
 *     UC_OR_UC_LOW_QUEUE_WEIGHT using 'entry_offset' and 'core' as key.
 * \see
 *   * dnx_egq_weights_tbl_set
 *   * dnx_egr_ofp_scheduling_set
 */
shr_error_e dnx_ofp_rates_wfq_get(
    int unit,
    bcm_port_t logical_port,
    uint32 cosq,
    dnx_egr_ofp_sch_wfq_t * wfq_info);
/**
 * \brief
 *   Set WFQ settings, for 'scheduled' and 'unscheduled' traffic, for a specificed
 *   logical port (queue) in the OFP.
 *   Qpair is calculated using base-qpair (related to logical port) plus input
 *   priority (cosq).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] cosq -
 *   uint32. Priority, relative to base qpair (logical port), for which to set WFQ
 *   parameters.
 * \param [in] wfq_info -
 *   Pointer to dnx_egr_ofp_sch_wfq_t. This procedure extracts info using the following
 *   element:
 *     sched_weight
 *     unsched_weight
 *   This information is loaded into HW.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_DWM, fields UC_OR_UC_LOW_QUEUE_WEIGHT, MC_OR_MC_LOW_QUEUE_WEIGHT
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, fields MC_OR_MC_LOW_QUEUE_WEIGHT,
 *     UC_OR_UC_LOW_QUEUE_WEIGHT using 'entry_offset' and 'core' as key.
 * \see
 *   * dnx_egq_weights_tbl_set
 *   * dnx_egr_ofp_scheduling_set
 */
shr_error_e dnx_ofp_rates_wfq_set(
    int unit,
    dnx_port_t logical_port,
    uint32 cosq,
    dnx_egr_ofp_sch_wfq_t * wfq_info);

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
shr_error_e dnx_ofp_rates_port_tcg_shaper_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable);
/**
 * \brief
 *    Set a single mapping element which indicates, for specified port,  to which TCG,
 *    specified priority is mapped.
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] logical_port -
 *    uint32. Logical Port index to configure.
 * \param [in] priority -
 *    dnx_egr_q_prio_e. Priority (one of 8) to use in mapping TCG. Essentially,
 *    indicated TCG is mapped to 'priority' which is one of 8 entries
 *    assigned to 'logical_port'. (Basic range is 0 to 7 but it must
 *    also be smaller than the number of priorities assigned to this port).
 * \param [in] tcg_to_map
 *    dnx_tcg_ndx_t. The TCG to map to indicated priority and logical_port.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Function description, in other words: Associate the queue-pair (Port,Priority)
 *     to traffic class groups (TCG) attributes.
 * \see
 *   * dnx_ofp_rates_ofp_tcg_set
 *   * dnx_ofp_rates_ofp_tcg_get
 *   * dnx_cosq_egq_gport_egress_tc_to_tcg_map_set
 */
shr_error_e dnx_ofp_rates_tc_to_tcg_mapping_set(
    int unit,
    dnx_port_t logical_port,
    dnx_egr_q_prio_e priority,
    dnx_tcg_ndx_t tcg_to_map);
/**
 * \brief
 *    Get a single mapping element which indicates, for specified port,  to which TCG
 *    specified priority is mapped.
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] logical_port -
 *    bcm_port_t. Logical port index to configure.
 * \param [in] priority -
 *    uint32. Priority (one of 8) to use in mapping TCG. Essentially,
 *    indicated TCG is mapped to 'priority' which is one of 8 entries
 *    assigned to 'logical_port'. (Basic priority range is 0 to 7 but it must
 *    also be smaller than the number of priorities assigned to this port).
 * \param [out] tcg_to_map_p
 *    Pointer to dnx_tcg_ndx_t.
 *    This procedure loads pointed memory by the TCG that is mapped to
 *    indicated priority and logical_port.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * dnx_ofp_rates_ofp_tcg_set
 *   * dnx_ofp_rates_ofp_tcg_get
 */
shr_error_e dnx_ofp_rates_tc_to_tcg_mapping_get(
    int unit,
    bcm_port_t logical_port,
    uint32 priority,
    dnx_tcg_ndx_t * tcg_to_map_p);
/*********************************************************************
* NAME:
 *   dnx_ofp_rates_ofp_tcg_get 
 * TYPE:
 * PROC
 *    Associate the queue-pair (Port,Priority) to traffic class
 *    groups (TCG) attributes.
 * Input
 *   int                       unit -
 *     Identifier of the device to access.
 *  int                        core -
 *     Identifier of the core to access.
 *  uint32                     logical_port -
 *     Logical Port index to inquire.
 *   uint32                    tcg_info -
 *     TCG attributes to be filled/read from
 * REMARKS:
 *   The last four TCG are single-member groups. Scheduling within
 *   a TCG is done in a strict priority manner according to the
 *   priority level. (e.g. If P1,P2,P7 within the same TCG1.
 *   Then SP select is descending priority P7,P2,P1).
 * \see
 *   * dnx_ofp_rates_tcg_id_egq_verify
 *   * dnx_ofp_rates_tcg_id_egq
 */
shr_error_e dnx_ofp_rates_ofp_tcg_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    dnx_egr_queuing_tcg_info_t * tcg_info);
/**
 * \brief - Clear input structure, of type 'dnx_egr_tcg_sch_wfq_t'. This
 *   is used so loading of this structure by meaningful info can begin at a known
 *   starting point.
 *
 * \param [out] info -
 *   Pointer to dnx_egr_tcg_sch_wfq_t. This procedure loads pointed structure
 *   by known default values.
 *
 * \return
 *    No direct output: Void.
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_ofp_rates_tcg_sch_wfq_clear(
    dnx_egr_tcg_sch_wfq_t * info);
/* 
 * \brief -
 *   Set EIR Weight for Egress Port TCG.
 *   Sets, for a specified TCG within a certain OFP its excess rate.
 *   Excess traffic is scheduled between other TCGs according to
 *   weighted fair queueing or strict priority policy.
 *   Set to 'invalid', in case TCG does not take part of this policy.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] logical_port -
 *   Logical port to identify the corresponding base qpair and,
 *   consequently, the port scheduler to update.
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. Identifier of the index, on port scheduler, to update
 *   with new weight. Must be between DNX_TCG_MIN and DNX_TCG_MAX
 * \param [in] tcg_weight -
 *   Pointer to dnx_egr_tcg_sch_wfq_t. Weight for selected
 *   TCG index on specified port plus 'valid' indication.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * This function must only be called for eight priorities port.
 *   * Indirect output is into HW, tables:
 *   *  * DBAL_TABLE_EGQ_TCG_ATTRIBUTES (field DBAL_FIELD_WFQ_TCG_DIS)
 *   *  * DBAL_TABLE_EGQ_SHAPING_TCG_WEIGHTS (field DBAL_FIELD_WEIGHT_OF_TCG)
 * \see
 *   * dnx_ofp_rates_tcg_weight_get
 *   * dnx_cosq_gport_egress_port_tcg_sched_set
 */
int dnx_ofp_rates_tcg_weight_set(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight);
/* 
 * \brief -
 *   Get EIR Weight for Egress Port TCG.
*    Gets, for a specified TCG within a certain OFP its excess rate.
 *   Excess traffic is scheduled between other TCGs according to
 *   weighted fair queueing or strict priority policy.
 *   If invalid, get only this indication.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] logical_port -
 *   Logical port to identify the corresponding base qpair and,
 *   consequently, the port scheduler to update.
 * \param [in] tcg_ndx -
 *   dnx_tcg_ndx_t. Identifier of the index, on port scheduler, to get
 *   weight from. Must be between DNX_TCG_MIN and DNX_TCG_MAX
 * \param [out] tcg_weight -
 *   Pointer to dnx_egr_tcg_sch_wfq_t. This procedure loads pointed structure
 *   by weight of selected TCG index on specified port plus indication on whether
 *   it is 'valid', to start with.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * This function must only be called for eight priorities port.
 *   * Indirect input is in HW, tables:
 *   *  * DBAL_TABLE_EGQ_TCG_ATTRIBUTES (field DBAL_FIELD_WFQ_TCG_DIS)
 *   *  * DBAL_TABLE_EGQ_SHAPING_TCG_WEIGHTS (field DBAL_FIELD_WEIGHT_OF_TCG)
 * \see
 *   * dnx_ofp_rates_tcg_weight_set
 *   * dnx_cosq_gport_egress_port_tcg_sched_get
 */
int dnx_ofp_rates_tcg_weight_get(
    int unit,
    bcm_port_t logical_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight);

/* } */
/*
 * Procedures related to 'max_burst'
 * {
 */
/* 
 * \brief -
 *   Set global maximal burst size for all qpairs (on ALL cores) which are
 *   under FC (flow control).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] max_burst_fc_queues -
 *   uint32. Value of maximal burst in credits. Note that if a value of '0' is selected
 *   then no maximal limit is set on collection of credits for qpairs which are under FC
 *   (Flow Control)
 *   Value is limited to a maximal value of DNX_OFP_RATES_BURST_FC_Q_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Note that all cores are updated.
 * \see
 *   * dnx_cosq_control_egress_port_tc_set
 *   * bcm_dnx_cosq_control_set
 */
int dnx_egq_ofp_rates_fc_queues_max_burst_set(
    int unit,
    uint32 max_burst_fc_queues);
/* 
 * \brief -
 *   Get global maximal burst size for all qpairs (on ALL cores) which are
 *   under FC (flow control).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [out] max_burst_fc_queues -
 *   Pointer to uint32. This procedure loads pointed memory by the value of
 *   maximal burst in credits. (Note that if a value of '0' is selected
 *   then no maximal limit is set on collection of credits for qpairs which are under FC
 *   (Flow Control))
 *   Value is limited to a maximal value of DNX_OFP_RATES_BURST_FC_Q_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Note that all cores are updated.
 * \see
 *   * dnx_cosq_control_egress_port_tc_get
 *   * bcm_dnx_cosq_control_get
 */
int dnx_egq_ofp_rates_fc_queues_max_burst_get(
    int unit,
    uint32 *max_burst_fc_queues);
/* 
 * \brief -
 *   Set global maximal burst size for all 'priority' qpairs (on ALL cores) which are
 *   in 'empty' state.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] max_burst_empty_queues -
 *   uint32. Value of maximal burst in credits. Note that if a value of '0' is selected
 *   then no maximal limit is set on collection of credits for qpairs which are empty.
 *   Value is limited to a maximal value of DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Note that all cores are updated.
 * \see
 *   * dnx_cosq_control_egress_port_tc_set
 *   * bcm_dnx_cosq_control_set
 */
shr_error_e dnx_egq_ofp_rates_empty_queues_max_burst_set(
    int unit,
    uint32 max_burst_empty_queues);
/* 
 * \brief -
 *   Get global maximal burst size for all 'priority' qpairs (on ALL cores) which are
 *   in 'empty' state.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [out] max_burst_empty_queues -
 *   Pointer to uint32. This procedure loads pointed memory by value of maximal burst in
 *   credits. (Note that if a value of '0' is selected then no maximal limit is set on
 *   collection of credits for qpairs which are empty.
 *   Value is limited to a maximal value of DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Note that all cores are updated.
 * \see
 *   * dnx_cosq_control_egress_port_tc_get
 *   * bcm_dnx_cosq_control_get
 */
shr_error_e dnx_egq_ofp_rates_empty_queues_max_burst_get(
    int unit,
    uint32 *max_burst_empty_queues);
/* } */
/**
 * \brief -
 *   Configure OFP shaping rate, set single port rate.
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
 *     BCM_COSQ_BW_NOT_COMMIT (See documentation on definition. If
 *     set then HW is NOT updated and operation is logical only.)
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated sw state: dnx_ofp_rate_db.otm
 *   * * Updated sw state: dnx_ofp_rate_db.otm_cal
 *   * * Updated hw (if required)
 * \see
 *   * None
 */
shr_error_e dnx_egq_ofp_rates_port_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);
/**
 * \brief -
 *   Get calculated (shaping) rate for specified 'gport'.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Scheduler port in the format of gport.
 * \param [in] cosq -
 *   Not used. Must be set to '0'.
 * \param [out] kbits_sec_min -
 *   Pointer to uint32. Not used. Set to '0' by this procedure.
 * \param [out] kbits_sec_max -
 *   Pointer to uint32. This procedure loads pointed memory by
 *   maximal bandwidth in kbits/sec.
 * \param [out] flags -
 *   Pointer to uint32. Not used. Set to '0' by this procedure.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated sw state: dnx_ofp_rate_db.otm
 *   * * Updated sw state: dnx_ofp_rate_db.otm_cal
 * \see
 *   * None
 */
shr_error_e dnx_egq_ofp_rates_port_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/* } */
/* } */

#endif
/* EGQ_OFP_RATES_H_INCLUDED */
