/**
 * \file diag_dnx_egr_queuing.c
 * 
 * diagnostics for egress queuing
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_PORT

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/cosq/egress/egq_dbal.h>
#include <bcm_int/dnx/cosq/egress/rate_measurement.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/port/port_flexe.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
/** sal */
#include <sal/appl/sal.h>
#include "diag_dnx_egr_queuing.h"
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>


/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egq_compensation_options[] = {
    /*name        type                  desc                                                       default  ext  */
    {"port",      SAL_FIELD_TYPE_PORT,  "port # / logical port type / port name",                  "all",   NULL},
    {"value",     SAL_FIELD_TYPE_INT32, "Compensation value to set. Only 'get' if not specified",  "0",     NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egq_compensation_man = {
    .brief = "Set/Print egress compensation information per logical port",
    .full = "Set/Print egress compensation information per logical port\r\n"
        "  port - \r\n"
        "    The supported options might be:\r\n"
        "      Simple integer (i.e. '5')    \r\n"
        "      Bitmap of ports as logical port type: \r\n"
        "        'tm_ing', 'tm_egr', 'tm_e2e', 'fabric', 'nif', 'eth', 'all'        \r\n"
        "        (See 'dnx_algo_port_logicals_type_e')                              \r\n"
        "      Bitmaps of ports using port name (i.e. 'il', 'sfi256', 'xe2-xe4')    \r\n"
        "    If not specified then select ALL EGQ ports\r\n"
        "  value - \r\n"
        "    If not specified then just display compensation values for specified ports.\r\n"
        "    Otherwise, set specified value as 'compensation' for specified ports.\r\n",
    .examples = "port=13\n" "port=13 value=0"
};


/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egq_shaping_options[] = {
    /*name        type                  desc                                          default     ext*/
    {"port",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name",      "all",      NULL},
    {"GRaphical", SAL_FIELD_TYPE_BOOL, "print graphical illustration of the PS",      "FALSE",    NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egq_shaping_man = {
    .brief = "Print egress shaping information per port",
    .full =
        "Print egress shaping information per port\r\n"
        "  port - \r\n"
        "    The supported options might be:\r\n"
        "      Simple integer (i.e. '5')    \r\n"
        "      Bitmap of ports as logical port type: \r\n"
        "        'tm_ing', 'tm_egr', 'tm_e2e', 'fabric', 'nif', 'eth', 'all'        \r\n"
        "        (See 'dnx_algo_port_logicals_type_e')                              \r\n"
        "      Bitmaps of ports using port name (i.e. 'il', 'sfi256', 'xe2-xe4')    \r\n"
        "    If no port is provided then all ports shaping info is printed          \r\n"
        "  GRaphical - \r\n"
        "    If specified (port must be single!) then display all qpairs (priorities)\r\n"
        "    and correspondong priority setup (SP/WFQ) plus bandwidth setup.         \r\n",
    .examples = "\n" "port=13\n" "port=13 graphical"
};

sh_sand_man_t sh_dnx_egr_rqp_rate_man = {
    .brief = "calculate and display egress rqp rate",
    .examples = "core=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_pqp_rate_options[] = {
    /*name              type                     desc                                         default */
    {"core",            SAL_FIELD_TYPE_INT32,    NULL,                                        "0"},
    {"port",            SAL_FIELD_TYPE_INT32,    "port # / logical port type / port name",    _STRINGIFY(EGQ_NOT_INCLUDED_VAL),      NULL},
    {"TrafficClasS",    SAL_FIELD_TYPE_INT32,    "parameter: traffic class index",            _STRINGIFY(EGQ_NOT_INCLUDED_VAL),      NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_pqp_rate_man = {
    .brief = "calculate and display egress pqp rate",
    .examples = "port=13 tc=0 \n" "core=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_epep_rate_options[] = {
    /*name              type                     desc                                         default */
    {"core",            SAL_FIELD_TYPE_INT32,    NULL,                                        "0"},
    {"port",            SAL_FIELD_TYPE_INT32,    "port # / logical port type / port name",    _STRINGIFY(EGQ_NOT_INCLUDED_VAL),       NULL},
    {"TrafficClasS",    SAL_FIELD_TYPE_INT32,    "parameter: traffic class index",            _STRINGIFY(EGQ_NOT_INCLUDED_VAL),       NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_epep_rate_man = {
    .brief = "calculate and display egress epep rate",
    .examples = "port=13 tc=0 \n" "core=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_epni_rate_options[] = {
    /*name              type                     desc                                         default    ext*/
    {"SCheMe",          SAL_FIELD_TYPE_INT32,    "Indicate scheme",                          "0",       NULL},
    {"BandWidth",       SAL_FIELD_TYPE_INT32,    "Indicate the objective to be measured",    "0",       NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_epni_rate_man = {
    .brief = "calculate and display egress epni rate according to the scheme.\n",
    .full = "when scheme = 0(measure total device), bw can be ignored,\n"
        "when scheme = 1(measure EGQ IF), bw indicates EGQ IF to be measured,\n"
        "when scheme = 2(measure port), bw indicates port to be measured,\n"
        "when scheme = 3(measure QPair), bw indicates QPair to be measured,\n"
        "when scheme = 4(measure Mirror Priority), bw indicates Mirror Priority to be measured.",
    .synopsis = "[scheme=<integer>] [bw=<integer>]",
    .examples = "scheme=2 bw=1"
};

/* *INDENT-OFF* */
/*
 * For 'core', use 'system' documentation from sh_sand_sys_arguments[]
 */
sh_sand_option_t sh_dnx_egr_queuing_rate_options[] = {
    /*name              type                     desc                                     default                                ext*/
    {"core",            SAL_FIELD_TYPE_INT32,    NULL,                                    "0",                                   NULL},
    {"port",            SAL_FIELD_TYPE_INT32,    "port # / logical port type / port name",_STRINGIFY(EGQ_NOT_INCLUDED_VAL),      NULL},
    {"TrafficClass",    SAL_FIELD_TYPE_INT32,    "parameter: traffic class index",        _STRINGIFY(EGQ_NOT_INCLUDED_VAL),      NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_queuing_rate_man = {
    .brief = "calculate and display egress queuing rate",
    .full = "Display counters as per specified core, logical port, and priority ('tc')\r\n"
        "  port - \r\n"
        "    The supported options might be:\r\n"
        "      Simple integer (i.e. '5')    \r\n"
        "      Bitmap of ports as logical port type: \r\n"
        "        'tm_ing', 'tm_egr', 'tm_e2e', 'fabric', 'nif', 'eth', 'all'        \r\n"
        "        (See 'dnx_algo_port_logicals_type_e')                              \r\n"
        "      Bitmaps of ports using port name (i.e. 'il', 'sfi256', 'xe2-xe4')    \r\n"
        "  If both 'tc' and 'port' are specified, and NEITHER is '-1', then 'port\r\n"
        "    ('logical port') is converted to 'qpair', 'tc' is added to it (as   \r\n"
        "    priority) and the counters for this 'priority' port are displayed.  \r\n"
        "If 'tc' is '-1' (or not specified) then 'port ('logical port') is converted to 'TM port'\r\n"
        "  and the latter's counters are displayed"
        "If 'port' is '-1' as well (or was not specified, as well) then all counters are displayed",
    .examples = "port=13 tc=0 \n" "core=0"
};

/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_egr_interface_rate_options[] = {
    /*name              type                     desc                                         default    ext*/
    {"port",            SAL_FIELD_TYPE_PORT,     "port # / logical port type / port name",    "all",      NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_egr_interface_rate_man = {
    .brief = "display egress utilization rate per interface",
    .full = NULL,
    .synopsis = NULL,
    .examples = "port=13 \n"
};

/**
 * \brief
 *   Print input string a given number of times, on the same line.
 *   Following that, depending on selected combination, add a few
 *   of characters, one of which is '\n'.
 * \param [in] nof_iters -
 *   int. Number of repetitions of input 'str'.
 * \param [in] str -
 *   char pointer. Points to string to print a few times
 * \param [in] new_line -
 *   uint8. Indication for characters to print after repetition of 'str':
 *     1 - "|\n| "
 *     2 - " \n| "
 *     3 - "\n  "
 *     4 - "\n _"
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   * shr_error_e
 *   * sh_dnx_egq_shaping_man
 *   * sh_dnx_egq_shaping_options
 */
static void
sh_dnx_print_string_in_loop(
    int nof_iters,
    const char *str,
    uint8 new_line)
{
    int ii;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    for (ii = 0; ii < nof_iters; ii++)
    {
        cli_out("%s", str);
    }
    if (new_line == 1)
    {
        cli_out("|\n| ");
    }
    else if (new_line == 2)
    {
        cli_out(" \n| ");
    }
    else if (new_line == 3)
    {
        cli_out("\n  ");
    }
    else if (new_line == 4)
    {
        cli_out("\n _");
    }
    SHR_EXIT();
exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * \brief
 *   Calculate rate on the base of counter information.
 *     1, Convert byte counter to Kbps
 *     2, Convert packet counter to Kpps
 * \param [in] unit - Unit #
 * \param [in] counter_info - on which to calculate rate
 * \param [out] rate - rate to be calculated.
 * \return 
 *      Standard error handling
 */
static shr_error_e
sh_dnx_egq_counter_rate_calc(
    int unit,
    dnx_egq_counter_info_t * counter_info,
    uint64 *rate)
{
    uint64 count_val;
    uint64 max_count_bytes;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(counter_info, _SHR_E_PARAM, "counter_info");
    SHR_NULL_CHECK(rate, _SHR_E_PARAM, "rate");

    /*
     * In order to avoid overflow, calculations are done in 64 bits.
     */
    COMPILER_64_ALLONES(max_count_bytes);
    /*
     * Set 'max_count_bytes' to the maximal value for a 64 bits counter so
     * that it does not overflow when converting to bits.
     * If larger than that, divide (bits-to-kbits) first.
     */
    COMPILER_64_UDIV_32(max_count_bytes, UTILEX_NOF_BITS_IN_BYTE);
    COMPILER_64_COPY(count_val, counter_info->count_val);
    if (COMPILER_64_GT(counter_info->count_val, max_count_bytes))
    {
        /*
         * 64 bits counter is large and will overflow when converted to bits. So
         * First, convert 'bits' to 'kilo bits' by dividing by 1000
         */
        COMPILER_64_UDIV_32(count_val, 1000);
        if (DNX_EGQ_COUTNER_TYPE_BYTE == counter_info->type)
        {
            /*
             * If cunter is for bytes, then convert it to 'per bit' by multiplying by 8
             */
            COMPILER_64_UMUL_32(count_val, UTILEX_NOF_BITS_IN_BYTE);
        }
    }
    else
    {
        if (DNX_EGQ_COUTNER_TYPE_BYTE == counter_info->type)
        {
            /*
             * If cunter is for bytes, then convert it to 'per bit' by multiplying by 8
             */
            COMPILER_64_UMUL_32(count_val, UTILEX_NOF_BITS_IN_BYTE);
        }
        /*
         * Convert 'bits' to 'kilo bits' by dividing by 1000
         */
        COMPILER_64_UDIV_32(count_val, 1000);
    }
    COMPILER_64_COPY(*rate, count_val);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the actual counter rate of rqp
 */
static shr_error_e
sh_dnx_egq_rqp_counter_rate_get(
    int unit,
    int core,
    dnx_egq_counter_rate_info_t * rate_info,
    int *nof_counter)
{
    int counter_idx;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[DNX_EGQ_RQP_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, DNX_EGQ_RQP_COUNTER_NUM * sizeof(dnx_egq_counter_info_t));

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_RQP, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_RQP, core));

    /** Get actual Credit count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_rqp_counter_info_get(unit, core, counter_info, nof_counter));

    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        sal_strncpy_s(rate_info[counter_idx].name, counter_info[counter_idx].name, sizeof(rate_info[counter_idx].name));
        if (!counter_info[counter_idx].count_ovf)
        {
            SHR_IF_ERR_EXIT(sh_dnx_egq_counter_rate_calc
                            (unit, &counter_info[counter_idx], &rate_info[counter_idx].rate));
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_RQP, core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the actual counter rate of pqp
 */
static shr_error_e
sh_dnx_egq_pqp_counter_rate_get(
    int unit,
    int core,
    int port,
    int tc,
    dnx_egq_counter_rate_info_t * rate_info)
{
    bcm_core_t core_used;
    int counter_idx;
    int qpair, base_qp;
    int num_priorities;
    uint32 otm;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[DNX_EGQ_PQP_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, sizeof(counter_info));

    if ((EGQ_NOT_INCLUDED_VAL != port) && (EGQ_NOT_INCLUDED_VAL != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_used));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_qp));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

        SHR_RANGE_VERIFY(tc, 0, num_priorities - 1, _SHR_E_PARAM,
                         "tc (%d) is out of range. (Number of priorities is %d)\n", tc, num_priorities);
        qpair = base_qp + tc;

        /** Select port HR to be tracked */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_configuration_set
                        (unit, core_used, DNX_EGQ_COUTNER_FILTER_BY_QP, qpair));
    }
    else if (EGQ_NOT_INCLUDED_VAL != port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core_used, &otm));

        /** Select OTM port to be tracked */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_configuration_set
                        (unit, core_used, DNX_EGQ_COUTNER_FILTER_BY_OTM, otm));
    }
    else
    {
        core_used = core;
        /** Select the device to be tracked */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_pqp_counter_configuration_set
                        (unit, core_used, DNX_EGQ_COUTNER_FILTER_BY_NONE, 0));
    }

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_PQP, core_used));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_PQP, core_used));

    /** Get actual Credit count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_pqp_counter_info_get(unit, core_used, counter_info));

    for (counter_idx = 0; counter_idx < DNX_EGQ_PQP_COUNTER_NUM; counter_idx++)
    {
        sal_strncpy_s(rate_info[counter_idx].name, counter_info[counter_idx].name, sizeof(rate_info[counter_idx].name));
        if (!counter_info[counter_idx].count_ovf)
        {
            SHR_IF_ERR_EXIT(sh_dnx_egq_counter_rate_calc
                            (unit, &counter_info[counter_idx], &rate_info[counter_idx].rate));
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_PQP, core_used));

    /** Disable the credit counter filters */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_pqp_counter_configuration_reset(unit, core_used));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the actual counter rate of rqp
 */
static shr_error_e
sh_dnx_egq_epep_counter_rate_get(
    int unit,
    int core,
    int port,
    int tc,
    dnx_egq_counter_rate_info_t * rate_info,
    int *nof_counter)
{
    bcm_core_t core_used;
    int flag = 0;
    int counter_idx;
    int qpair, base_qp;
    int num_priorities;
    int nof_counter_tmp;
    int count_by, count_place;
    uint32 otm;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, sizeof(counter_info));

    if ((EGQ_NOT_INCLUDED_VAL != port) && (EGQ_NOT_INCLUDED_VAL != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_used));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_qp));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

        SHR_RANGE_VERIFY(tc, 0, num_priorities - 1, _SHR_E_PARAM,
                         "tc (%d) is out of range. (Number of priorities is %d)\n", tc, num_priorities);
        qpair = base_qp + tc;

        /** Select port HR to be tracked */
        count_by = DNX_EGQ_COUTNER_FILTER_BY_QP;
        count_place = qpair;
    }
    else if (EGQ_NOT_INCLUDED_VAL != port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core_used, &otm));

        /** Select OTM port to be tracked */
        count_by = DNX_EGQ_COUTNER_FILTER_BY_OTM;
        count_place = otm;
    }
    else
    {
        core_used = core;

        /** Select the device to be tracked */
        count_by = DNX_EGQ_COUTNER_FILTER_BY_NONE;
        count_place = 0;
    }

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    /** Count packet.*/
    /** Set EPNI counter to trace the selected objective.*/
    flag = DNX_EGQ_CONFIGURATION_COUNT_PACKETS;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core_used, flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core_used));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core_used));

    /** Get actual packet count for 1 sec */
    flag = DNX_EGQ_EPNI_QUEUE_COUNTER | DNX_EGQ_NON_PROGRAMMABLE_COUNTER;
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_info_get(unit, core_used, flag, counter_info, &nof_counter_tmp));
    *nof_counter = nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core_used));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_configuration_reset(unit, core_used));

    /** Count byte.*/
    /** Set EPNI counter to trace the selected objective.*/
    flag = DNX_EGQ_CONFIGURATION_COUNT_BYTES;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core_used, flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core_used));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core_used));

    /** Get actual packet count for 1 sec */
    flag = DNX_EGQ_EPNI_QUEUE_COUNTER | DNX_EGQ_NON_PROGRAMMABLE_COUNTER;
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_info_get
                    (unit, core_used, flag, &counter_info[*nof_counter], &nof_counter_tmp));
    *nof_counter += nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core_used));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_configuration_reset(unit, core_used));

    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        sal_strncpy_s(rate_info[counter_idx].name, counter_info[counter_idx].name, sizeof(rate_info[counter_idx].name));
        if (!counter_info[counter_idx].count_ovf)
        {
            SHR_IF_ERR_EXIT(sh_dnx_egq_counter_rate_calc
                            (unit, &counter_info[counter_idx], &rate_info[counter_idx].rate));
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the actual counter rate of rqp
 */
static shr_error_e
sh_dnx_egq_epni_counter_rate_get(
    int unit,
    int core,
    dnx_egr_epni_rate_scheme_e scheme,
    int bw,
    dnx_egq_counter_rate_info_t * rate_info,
    int *nof_counter)
{
    int set_flag, get_flag;
    int counter_idx;
    int nof_counter_tmp;
    int count_by, count_place;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_egq_counter_info_t counter_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(counter_info, 0, 2 * DNX_EGQ_EPNI_COUNTER_NUM * sizeof(dnx_egq_counter_info_t));

    switch (scheme)
    {
        case DNX_EGR_EPNI_RATE_SCHEME_TOTAL:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_NONE;
            get_flag =
                DNX_EGQ_EPNI_QUEUE_COUNTER | DNX_EGQ_EPNI_IF_COUNTER | DNX_EGQ_EPNI_MIRROR_COUNTER |
                DNX_EGQ_NON_PROGRAMMABLE_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_QP:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_QP;
            get_flag = DNX_EGQ_EPNI_QUEUE_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_PORT:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_OTM;
            get_flag = DNX_EGQ_EPNI_QUEUE_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_IF:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_IF;
            get_flag = DNX_EGQ_EPNI_IF_COUNTER;
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_MIRROR:
            count_by = DNX_EGQ_COUTNER_FILTER_BY_MIRROR;
            get_flag = DNX_EGQ_EPNI_MIRROR_COUNTER;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected EPNI rate scheme %d\n", scheme);
            break;
    }

    count_place = bw;

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    /** Count packet.*/
    /** Set EPNI counter to trace the selected objective.*/
    set_flag = DNX_EGQ_CONFIGURATION_COUNT_PACKETS;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core, set_flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core));

    /** Get actual packet count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_info_get(unit, core, get_flag, counter_info, &nof_counter_tmp));
    *nof_counter = nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_configuration_reset(unit, core));

    /** Count byte.*/
    /** Set EPNI counter to trace the selected objective.*/
    set_flag = DNX_EGQ_CONFIGURATION_COUNT_BYTES;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_epni_counter_configuration_set(unit, core, set_flag, count_by, count_place));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_EPNI, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_EPNI, core));

    /** Get actual packet count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_info_get
                    (unit, core, get_flag, &counter_info[*nof_counter], &nof_counter_tmp));
    *nof_counter += nof_counter_tmp;

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_EPNI, core));

    /** Disable the counter filters */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_configuration_reset(unit, core));

    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        sal_strncpy_s(rate_info[counter_idx].name, counter_info[counter_idx].name, sizeof(rate_info[counter_idx].name));
        if (!counter_info[counter_idx].count_ovf)
        {
            SHR_IF_ERR_EXIT(sh_dnx_egq_counter_rate_calc
                            (unit, &counter_info[counter_idx], &rate_info[counter_idx].rate));
        }
        rate_info[counter_idx].type = counter_info[counter_idx].type;
        rate_info[counter_idx].count_ovf = counter_info[counter_idx].count_ovf;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given a logical port, display, graphically, all corresponsing qpairs and their
 *   priority setup (SP/WFQ) plus bandwidth setup.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port for which to build the graphucal display.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   * sh_dnx_egq_shaping_cmd
 */
static shr_error_e
sh_dnx_egq_shaping_graphic(
    int unit,
    bcm_port_t logical_port)
{
    int ii, jj;
    uint8 is_sp_bmp;
    /*
     * tcg_to_tcs_table[ii] = if the x-th bit is on, port priority-x is attached to TCG ii
     */
    uint32 *tcg_to_tcs_p = NULL;
    uint32 tm_port;
    int port_nof_q_pairs, pps_base_q_pair;
    int ps_num;
    bcm_gport_t local_port_gport;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(tcg_to_tcs_p, sizeof(*tcg_to_tcs_p) * DNX_NOF_TCGS,
                       "tcg_to_tcs_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    is_sp_bmp = 0;
    ii = 0;
    jj = 0;

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &port_nof_q_pairs));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &pps_base_q_pair));
    ps_num = pps_base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    BCM_GPORT_LOCAL_SET(local_port_gport, logical_port);
    cli_out("\n Graphic representation of port %d through port scheduler number %d:\n"
            "**********************************************************************\n  ", logical_port, ps_num);
    /*
     * Iterates over q_pairs, and when nof_q_pairs is reached finds next port index 
     */
    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        cli_out("  Port Index %03d   ", logical_port);
    }
    cli_out(" \n  ");
    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        cli_out("  Q-Pair %03d(P%d)   ", pps_base_q_pair + ii, ii);
    }
    cli_out("\n  ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "     | U | M |     ", 3);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "     |___|___|     ", 3);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "     |___|___|     ", 4);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "_______|___|_______", 2);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "       |   |       ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  ---------------  ", 1);

    /*
     * iterates over q_pairs and prints the weights or the strict priority
     */
    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        int mode_uc, mode_mc, weight_uc, weight_mc;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        info.cosq = port_nof_q_pairs;

        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeUnicastEgress, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, ii, &mode_uc, &weight_uc));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeMulticastEgress, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, ii, &mode_mc, &weight_mc));

        if (mode_uc != -1 || mode_mc != -1)
        {
            /*
             * Strict priority queue (and not weighted queue)
             */
            if (mode_uc > mode_mc)
            {
                cli_out("  \\    L   H    /  ");
            }
            else
            {
                cli_out("  \\    H   L    /  ");
            }
            /*
             * 'is_sp_bmp' is a bitmap. One bit per priority (qpair).
             * Bit(ii) is set if qpair 'ii' is programmer as 'strict priority'.
             * If not set, then 'qpair' is programmed as 'WFQ'.
             */
            is_sp_bmp = is_sp_bmp | SAL_BIT(ii);
        }
        else
        {
            cli_out("  \\ %03d     %03d /  ", weight_uc, weight_mc);
        }
    }
    cli_out("|\n| ");

    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        /*
         * Check 'is_sp_bmp', bit per bit (each bit represents 'qpair').
         * If a bit is set then qpair is 'Strict Priority'. Otherwise, it
         * is WFQ.
         */
        if (is_sp_bmp & SAL_BIT(0))
        {
            cli_out("   \\   -SP-    /   ");
        }
        else
        {
            cli_out("   \\   -WFQ-   /   ");
        }
        is_sp_bmp >>= 1;
    }
    cli_out("|\n| ");

    sh_dnx_print_string_in_loop(port_nof_q_pairs, "    -----------    ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "       |\\|/|       ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "       |/|\\|       ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

    /*
     * iterates over q_pairs and prints the shapers bandwidth
     */
    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        uint32 kbits_sec_max, flags, kbits_sec_min;
        int tcg_index, temp;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        info.cosq = ii;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTC, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, info.out_gport, ii, &kbits_sec_min, &kbits_sec_max, &flags));
        if (kbits_sec_max == 0)
        {
            cli_out("    (unlimited)    ");
        }
        else
        {
            cli_out("  (%010uKbs)  ", kbits_sec_max);
        }
        /*
         * fill the tcg_to_tcs_p with values
         */
        if (port_nof_q_pairs == 1)
        {
            cli_out("|\n|__________|_________|\n");
            cli_out("           |          \n");
            goto exit;
        }
        else if (port_nof_q_pairs == 2)
        {
            continue;
        }
        else
        {
            /*
             * get multicast queue
             * number ii
             */
            SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, ii, &tcg_index, &temp));
            tcg_index -= BCM_COSQ_SP0;
            tcg_to_tcs_p[tcg_index] |= SAL_BIT(ii);
        }
    }
    cli_out("|\n| ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

    if (port_nof_q_pairs == 2)
    {
        cli_out("     ----------------------------     |\n| ");
        cli_out("     \\   H                  L   /     |\n| ");
        cli_out("      \\           SP           /      |\n| ");
        cli_out("       ------------------------       |\n|_");
        cli_out("__________________|___________________|\n  ");
        cli_out("                  |                    \n");
        goto exit;
    }
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "                   ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, "   |   |  ...  |   ", 0);
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, "         |         ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, " ----------------- ", 1);

    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        uint8 space_left = 15;
        uint32 temp_TFG_to_TCs_i = tcg_to_tcs_p[ii];
        if (ii < 4)
        {
            cli_out(" \\");
        }
        else
        {
            cli_out(" |");
        }
        for (jj = 0; jj < 8 && space_left > 0; jj++)
        {
            if (tcg_to_tcs_p[ii] & SAL_BIT(0))
            {
                cli_out(" P%d", jj);
                space_left -= 3;
            }
            tcg_to_tcs_p[ii] >>= 1;
        }
        tcg_to_tcs_p[ii] = temp_TFG_to_TCs_i;
        if (space_left == 15)
        {
            cli_out("(none attached)");
            space_left = 0;
        }
        sh_dnx_print_string_in_loop(space_left, " ", 0);
        if (ii < 4)
        {
            cli_out("/ ");
        }
        else
        {
            cli_out("| ");
        }
    }
    cli_out("|\n| ");

    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        uint8 space_left = 10;
        int8 skip_first = 5;
        if (ii < 4)
        {
            cli_out("  \\   ");
        }
        else
        {
            cli_out(" |    ");
        }
        for (jj = 0; jj < 8 && space_left > 0; jj++)
        {
            if ((tcg_to_tcs_p[ii] & SAL_BIT(0)) && (skip_first--) <= 0)
            {
                cli_out("P%d ", jj);
                space_left -= 3;
            }
            tcg_to_tcs_p[ii] >>= 1;
        }
        sh_dnx_print_string_in_loop(space_left, " ", 0);
        if (ii < 4)
        {
            cli_out("/  ");
        }
        else
        {
            cli_out(" | ");
        }

    }
    cli_out("|\n| ");
    for (ii = 0; ii < 8; ii++)
    {
        if (ii < 4)
        {
            cli_out("   \\-SP%d Group-/   ", ii);
        }
        else
        {
            cli_out(" |     -SP%d-     | ", ii);
        }
    }
    cli_out("|\n| ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, "    -----------    ", 0);
    sh_dnx_print_string_in_loop(port_nof_q_pairs / 2, " ----------------- ", 1);
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

    /*
     * iterates over q_pairs and prints the TCG's CIR rates
     */
    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        uint32 flags, kbits_sec_max, kbits_sec_min;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        info.cosq = ii; /* TCG number */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTCG, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, info.out_gport, ii, &kbits_sec_min, &kbits_sec_max, &flags));
        cli_out(" CIR:%010uKbs ", kbits_sec_max);
    }
    cli_out("|\n| ");

    /*
     * iterates over q_pairs and prints the TCG's EIR rates
     */
    for (ii = 0; ii < port_nof_q_pairs; ii++)
    {
        int weight, mode;
        bcm_cosq_gport_info_t info;

        info.in_gport = local_port_gport;
        /*
         * TCG number
         */
        info.cosq = ii;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTCG, &info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, info.out_gport, ii, &mode, &weight));
        if (mode == -1)
        {
            cli_out(" EIR:%010u    ", weight);
        }
        else
        {
            cli_out(" EIR:   none       ");
        }
        pps_base_q_pair++;
    }
    cli_out("|\n| ");
    sh_dnx_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
    cli_out
        ("         --------------------------------------------------------------------------------------------------------------------------------------         |\n| ");
    cli_out
        ("                                                      |                                          |                                                      |\n| ");
    cli_out
        ("                                                   All EIRs                                   All CIRs                                                  |\n| ");
    cli_out
        ("                                                      |                                          |                                                      |\n| ");
    cli_out
        ("                                               ---------------                            ---------------                                               |\n| ");
    cli_out
        ("                                               \\    -WFQ-    /                            |     -FQ-    |                                               |\n| ");
    cli_out
        ("                                                -------------                             ---------------                                               |\n| ");
    cli_out
        ("                                                      |                                          |                                                      |\n| ");
    cli_out
        ("                                                  ----------------------------------------------------                                                  |\n| ");
    cli_out
        ("                                                  \\   L                                          H   /                                                  |\n| ");
    cli_out
        ("                                                   \\                      -SP-                      /                                                   |\n|");
    cli_out
        ("                                                     ------------------------------------------------                                                    |\n|");
    cli_out
        ("_____________________________________________________________________________|___________________________________________________________________________|\n ");
    cli_out("                                                                             |\n\n");

exit:
    SHR_FREE(tcg_to_tcs_p);
    SHR_FUNC_EXIT;
}

/**
 * See diag_dnx_egr_queuing.h
 */
shr_error_e
sh_dnx_egq_shaping_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t egq_ports;
    bcm_port_t logical_port;
    bcm_gport_t gport;
    uint32 if_speed, port_speed, min_speed, flags;
    bcm_core_t core;
    int channelized, egr_if;
    bcm_pbmp_t logical_ports_bitmap;
    int port_count;
    int is_graphical, is_port_present;
    dnx_algo_port_info_s port_info;
    int port_nof_q_pairs;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", logical_ports_bitmap);
    SH_SAND_IS_PRESENT("port", is_port_present);
    SH_SAND_GET_BOOL("GRaphical", is_graphical);
    _SHR_PBMP_FIRST(logical_ports_bitmap, logical_port);
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    BCM_PBMP_COUNT(logical_ports_bitmap, port_count);

    if (is_graphical)
    {
        if ((port_count == 1) && (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info)))
        {
            SHR_IF_ERR_EXIT(sh_dnx_egq_shaping_graphic(unit, logical_port));
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Graphical option requires a single valid egress TM port %s%s%s",
                              EMPTY, EMPTY, EMPTY);
        }
    }
    else
    {
        /**
         * Print table header
         */
        PRT_TITLE_SET("EGQ shaping");
        PRT_COLUMN_ADD("Port");
        PRT_COLUMN_ADD("EGQ Port rate [Kbps]");
        PRT_COLUMN_ADD("Core");
        PRT_COLUMN_ADD("EGQ IF");
        PRT_COLUMN_ADD("Nof Qpairs");
        PRT_COLUMN_ADD("Channelized");
        PRT_COLUMN_ADD("EGQ IF rate [Kbps]");
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &egq_ports));

        /** if ports were provided then make sure to only print the egress TM ports, otherwise print all */
        if (is_port_present)
        {
            BCM_PBMP_AND(egq_ports, logical_ports_bitmap);
        }

        /** iterate all logical egq_ports */
        BCM_PBMP_ITER(egq_ports, logical_port)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            BCM_GPORT_LOCAL_SET(gport, logical_port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &port_speed, &flags));
            SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &egr_if));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &port_nof_q_pairs));
            PRT_CELL_SET("%d", logical_port);
            PRT_CELL_SET("%d", port_speed);
            PRT_CELL_SET("%d", core);
            PRT_CELL_SET("%d", egr_if);
            PRT_CELL_SET("%d", port_nof_q_pairs);
            /**
             * Gettin interface speed for a non-channelized interface is not allowed.
             * It is configured to the maximum allowed at HW ('unlimited)
             */
            if (channelized)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_fabric_port_get(unit, gport, 0, &gport));
                SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &if_speed, &flags));
                PRT_CELL_SET("%s", "yes");
                PRT_CELL_SET("%d", if_speed);
            }
            else
            {
                PRT_CELL_SET("%s", "no");
                PRT_CELL_SET("unlimited");
            }
        }
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * See diag_dnx_egr_queuing.h
 */
shr_error_e
sh_dnx_egq_compensation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t egq_ports;
    bcm_port_t port;
    bcm_gport_t gport;
    int comp_for_get, comp_for_set;
    uint32 is_set;
    bcm_pbmp_t logical_ports;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Just to silence compiler's complaint
     */
    comp_for_set = 0;
    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_IS_PRESENT("value", is_set);
    /*
     * AND ports with egq ports. if port is not an egq port it will not be displayed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &egq_ports));
    BCM_PBMP_AND(egq_ports, logical_ports);

    if (is_set)
    {
        SH_SAND_GET_INT32("value", comp_for_set);
    }
    /**
     * Print table header
     */
    PRT_TITLE_SET("EGQ Compensation");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Compensation [Bytes]");
    /*
     * Iterate all logical egq_ports
     */
    BCM_PBMP_ITER(egq_ports, port)
    {
        BCM_GPORT_LOCAL_SET(gport, port);
        if (is_set)
        {
            /*
             * If this is a 'set' operation then store that requested value first and, then, retrieve
             * and display the new value.
             */
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, comp_for_set));
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_get(unit, gport, 0, bcmCosqControlPacketLengthAdjust, &comp_for_get));
        PRT_CELL_SET("%d", port);
        PRT_CELL_SET("%d", comp_for_get);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}


/**
 * \brief - calculate and display egress rqp couter rate
 */
shr_error_e
sh_dnx_egr_rqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, core_id;
    int counter_idx, nof_counter;
    dnx_egq_counter_rate_info_t rate_info[DNX_EGQ_RQP_COUNTER_NUM];
    char *prefix_string[3];
    uint32 num_prefix_string;
    uint32 kilo;
    uint64 maximal_for_disply;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    num_prefix_string = sizeof(prefix_string) / sizeof(prefix_string[0]);
    {
        /*
         * Initializations for display of counters which are, possibly, 64-bits
         */
        uint32 ii;

        ii = 0;
        prefix_string[ii++] = "M";
        prefix_string[ii++] = "G";
        prefix_string[ii++] = "KG";
        kilo = 1000;
        COMPILER_64_SET(maximal_for_disply, 0, 1000000000);
    }
    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        PRT_TITLE_SET("Egress RQP Rate on core (%d)", core);
        PRT_COLUMN_ADD("No.");
        PRT_COLUMN_ADD("Coutner");
        PRT_COLUMN_ADD("Rate");
        PRT_COLUMN_ADD("Unit");
        sal_memset(rate_info, 0, sizeof(rate_info));
        SHR_IF_ERR_EXIT(sh_dnx_egq_rqp_counter_rate_get(unit, core, rate_info, &nof_counter));

        for (counter_idx = 0; counter_idx < nof_counter; counter_idx++)
        {
            char *unit_prefix;

            unit_prefix = "K";
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", counter_idx);
            PRT_CELL_SET("%s", rate_info[counter_idx].name);
            if (!rate_info[counter_idx].count_ovf)
            {
                uint64 local_rate;
                uint32 ii;
                uint32 local_rate_u32;
                /*
                 * We assume that the maximal value here is
                 * (2^64 / 1000) * 8
                 * So that dividing it by 1,000,000,000 will
                 * always result in a value which is smaller than 2^32 (uint32)
                 */
                COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
                ii = 0;
                while (COMPILER_64_GT(local_rate, maximal_for_disply))
                {
                    COMPILER_64_UDIV_32(local_rate, kilo);
                    if (ii >= num_prefix_string)
                    {
                        /*
                         * If all prefixes were expired then something is wrong in the
                         * estimate of maximal counter value.
                         */
                        uint64 max_expected;

                        COMPILER_64_ALLONES(max_expected);
                        COMPILER_64_UDIV_32(max_expected, kilo);
                        COMPILER_64_UMUL_32(max_expected, 8);
                        COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Indicated rate (high,low = " COMPILER_64_PRINTF_FORMAT
                                     ") is higher than expected (high,low = " COMPILER_64_PRINTF_FORMAT ")",
                                     COMPILER_64_PRINTF_VALUE(local_rate), COMPILER_64_PRINTF_VALUE(max_expected));
                    }
                    unit_prefix = prefix_string[ii];
                    ii++;
                }
                COMPILER_64_TO_32_LO(local_rate_u32, local_rate);
                PRT_CELL_SET("%u", local_rate_u32);
            }
            else
            {
                PRT_CELL_SET("%s", "overflow!!!");
            }
            if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
            {
            PRT_CELL_SET("%s%s", unit_prefix, "pps")}
            else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
            {
                PRT_CELL_SET("%s%s", unit_prefix, "bps");
            }
            else
            {
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type,
                                  EMPTY, EMPTY);
            }
        }

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress pqp couter rate
 */
shr_error_e
sh_dnx_egr_pqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    uint32 otm;
    int logical_port, tc;
    int qpair, base_qp;
    int counter_idx;
    dnx_egq_counter_rate_info_t rate_info[DNX_EGQ_PQP_COUNTER_NUM];
    char *prefix_string[3];
    uint32 num_prefix_string;
    uint32 kilo;
    uint64 maximal_for_disply;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    num_prefix_string = sizeof(prefix_string) / sizeof(prefix_string[0]);
    {
        /*
         * Initializations for display of counters which are, possibly, 64-bits
         */
        uint32 ii;

        ii = 0;
        prefix_string[ii++] = "M";
        prefix_string[ii++] = "G";
        prefix_string[ii++] = "KG";
        kilo = 1000;
        COMPILER_64_SET(maximal_for_disply, 0, 1000000000);
    }
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("port", logical_port);
    SH_SAND_GET_INT32("TrafficClasS", tc);

    SHR_IF_ERR_EXIT(sh_dnx_egq_pqp_counter_rate_get(unit, core, logical_port, tc, rate_info));

    if ((EGQ_NOT_INCLUDED_VAL != logical_port) && (EGQ_NOT_INCLUDED_VAL != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_qp));
        qpair = base_qp + tc;
        PRT_TITLE_SET("Egress PQP Rate on QPair: port=%d, tc=%d, qpair=%d", logical_port, tc, qpair);
    }
    else if (EGQ_NOT_INCLUDED_VAL != logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &otm));
        PRT_TITLE_SET("Egress PQP Rate on OTM: port=%d, otm=%d", logical_port, otm);
    }
    else
    {
        PRT_TITLE_SET("Egress PQP Rate on core (%d)", core);
    }

    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    for (counter_idx = 0; counter_idx < DNX_EGQ_PQP_COUNTER_NUM; counter_idx++)
    {
        char *unit_prefix;

        unit_prefix = "K";
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", counter_idx);
        PRT_CELL_SET("%s", rate_info[counter_idx].name);
        if (!rate_info[counter_idx].count_ovf)
        {
            uint64 local_rate;
            uint32 ii;
            uint32 local_rate_u32;

            /*
             * We assume that the maximal value here is
             * (2^64 / 1000) * 8
             * So that dividing it by 1,000,000,000 will
             * always result in a value which is smaller than 2^32 (uint32)
             */
            COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
            ii = 0;
            while (COMPILER_64_GT(local_rate, maximal_for_disply))
            {
                COMPILER_64_UDIV_32(local_rate, kilo);
                if (ii >= num_prefix_string)
                {
                    /*
                     * If all prefixes were expired then something is wrong in the
                     * estimate of maximal counter value.
                     */
                    uint64 max_expected;

                    COMPILER_64_ALLONES(max_expected);
                    COMPILER_64_UDIV_32(max_expected, kilo);
                    COMPILER_64_UMUL_32(max_expected, 8);
                    COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Indicated rate (high,low = " COMPILER_64_PRINTF_FORMAT
                                 ") is higher than expected (high,low = " COMPILER_64_PRINTF_FORMAT ")",
                                 COMPILER_64_PRINTF_VALUE(local_rate), COMPILER_64_PRINTF_VALUE(max_expected));
                }
                unit_prefix = prefix_string[ii];
                ii++;
            }
            COMPILER_64_TO_32_LO(local_rate_u32, local_rate);
            PRT_CELL_SET("%u", local_rate_u32);
        }
        else
        {
            PRT_CELL_SET("%s", "overflow!!!");
        }
        if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
        {
        PRT_CELL_SET("%s%s", unit_prefix, "pps")}
        else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
        {
            PRT_CELL_SET("%s%s", unit_prefix, "bps");
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type, EMPTY,
                              EMPTY);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress epep couter rate
 */
shr_error_e
sh_dnx_egr_epep_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    uint32 otm;
    int logical_port, tc;
    int qpair, base_qp;
    int counter_idx, nof_counter;
    dnx_egq_counter_rate_info_t rate_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];
    char *prefix_string[3];
    uint32 num_prefix_string;
    uint32 kilo;
    uint64 maximal_for_disply;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    num_prefix_string = sizeof(prefix_string) / sizeof(prefix_string[0]);
    {
        /*
         * Initializations for display of counters which are, possibly, 64-bits
         */
        uint32 ii;

        ii = 0;
        prefix_string[ii++] = "M";
        prefix_string[ii++] = "G";
        prefix_string[ii++] = "KG";
        kilo = 1000;
        COMPILER_64_SET(maximal_for_disply, 0, 1000000000);
    }
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("port", logical_port);
    SH_SAND_GET_INT32("TrafficClasS", tc);

    SHR_IF_ERR_EXIT(sh_dnx_egq_epep_counter_rate_get(unit, core, logical_port, tc, rate_info, &nof_counter));

    if ((EGQ_NOT_INCLUDED_VAL != logical_port) && (EGQ_NOT_INCLUDED_VAL != tc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_qp));
        qpair = base_qp + tc;
        PRT_TITLE_SET("Egress EPEP Rate on QPair: port=%d, tc=%d, hr_id=%d", logical_port, tc, qpair);
    }
    else if (EGQ_NOT_INCLUDED_VAL != logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &otm));
        PRT_TITLE_SET("Egress EPEP Rate on OTM: port=%d, otm=%d", logical_port, otm);
    }
    else
    {
        PRT_TITLE_SET("Egress EPEP Rate on core(%d)", core);
    }

    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    for (counter_idx = 0; counter_idx < nof_counter; counter_idx++)
    {
        char *unit_prefix;

        unit_prefix = "K";
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", counter_idx);
        PRT_CELL_SET("%s", rate_info[counter_idx].name);
        if (!rate_info[counter_idx].count_ovf)
        {
            uint64 local_rate;
            uint32 ii;
            uint32 local_rate_u32;
            /*
             * We assume that the maximal value here is
             * (2^64 / 1000) * 8
             * So that dividing it by 1,000,000,000 will
             * always result in a value which is smaller than 2^32 (uint32)
             */
            COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
            ii = 0;
            while (COMPILER_64_GT(local_rate, maximal_for_disply))
            {
                COMPILER_64_UDIV_32(local_rate, kilo);
                if (ii >= num_prefix_string)
                {
                    /*
                     * If all prefixes were expired then something is wrong in the
                     * estimate of maximal counter value.
                     */
                    uint64 max_expected;

                    COMPILER_64_ALLONES(max_expected);
                    COMPILER_64_UDIV_32(max_expected, kilo);
                    COMPILER_64_UMUL_32(max_expected, 8);
                    COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Indicated rate (high,low = " COMPILER_64_PRINTF_FORMAT
                                 ") is higher than expected (high,low = " COMPILER_64_PRINTF_FORMAT ")",
                                 COMPILER_64_PRINTF_VALUE(local_rate), COMPILER_64_PRINTF_VALUE(max_expected));
                }
                unit_prefix = prefix_string[ii];
                ii++;
            }
            COMPILER_64_TO_32_LO(local_rate_u32, local_rate);
            PRT_CELL_SET("%u", local_rate_u32);
        }
        else
        {
            PRT_CELL_SET("%s", "overflow!!!");
        }
        if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
        {
        PRT_CELL_SET("%s%s", unit_prefix, "pps")}
        else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
        {
            PRT_CELL_SET("%s%s", unit_prefix, "bps");
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type, EMPTY,
                              EMPTY);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate and display egress epni according to the scheme
 */
shr_error_e
sh_dnx_egr_epni_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    int bw;
    dnx_egr_epni_rate_scheme_e scheme;
    int counter_idx, nof_counter = 0;
    dnx_egq_counter_rate_info_t rate_info[2 * DNX_EGQ_EPNI_COUNTER_NUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("SCheMe", scheme);
    SH_SAND_GET_INT32("BandWidth", bw);

    switch (scheme)
    {
        case DNX_EGR_EPNI_RATE_SCHEME_TOTAL:
            PRT_TITLE_SET("Egress EPNI Rate on the device");
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_QP:
            PRT_TITLE_SET("Egress EPNI Rate on QPair (%d)", bw);
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_PORT:
            PRT_TITLE_SET("Egress EPNI Rate on Port (%d)", bw);
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_IF:
            PRT_TITLE_SET("Egress EPNI Rate on EGQ Interface (%d)", bw);
            break;
        case DNX_EGR_EPNI_RATE_SCHEME_MIRROR:
            PRT_TITLE_SET("Egress EPNI Rate on Mirror (%d)", bw);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected EPNI rate scheme %d\n", scheme);
            break;
    }

    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(sh_dnx_egq_epni_counter_rate_get(unit, core, scheme, bw, rate_info, &nof_counter));
        if (0 != core)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        }
        PRT_CELL_SET("%d", core);
        for (counter_idx = 0; counter_idx < nof_counter; counter_idx++)
        {
            char *unit_prefix;

            unit_prefix = "K";
            if (0 != counter_idx)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%d", counter_idx);
            PRT_CELL_SET("%s", rate_info[counter_idx].name);
            if (!rate_info[counter_idx].count_ovf)
            {
                uint64 local_rate;
                uint64 maximal_for_disply;
                uint32 kilo, ii;
                char *prefix_string[3];
                uint32 num_prefix_string;
                uint32 local_rate_u32;
                /*
                 * We assume that the maximal value here is
                 * (2^64 / 1000) * 8
                 * So that dividing it by 1,000,000,000 will
                 * always result in a value which is smaller than 2^32 (uint32)
                 */
                ii = 0;
                prefix_string[ii++] = "M";
                prefix_string[ii++] = "G";
                prefix_string[ii++] = "KG";
                num_prefix_string = sizeof(prefix_string) / sizeof(prefix_string[0]);
                kilo = 1000;
                COMPILER_64_SET(maximal_for_disply, 0, 1000000000);
                COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
                ii = 0;
                while (COMPILER_64_GT(local_rate, maximal_for_disply))
                {
                    COMPILER_64_UDIV_32(local_rate, kilo);
                    if (ii >= num_prefix_string)
                    {
                        /*
                         * If all prefixes were expired then something is wrong in the
                         * estimate of maximal counter value.
                         */
                        uint64 max_expected;

                        COMPILER_64_ALLONES(max_expected);
                        COMPILER_64_UDIV_32(max_expected, kilo);
                        COMPILER_64_UMUL_32(max_expected, 8);
                        COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Indicated rate (high,low = " COMPILER_64_PRINTF_FORMAT
                                     ") is higher than expected (high,low = " COMPILER_64_PRINTF_FORMAT ")",
                                     COMPILER_64_PRINTF_VALUE(local_rate), COMPILER_64_PRINTF_VALUE(max_expected));
                    }
                    unit_prefix = prefix_string[ii];
                    ii++;
                }
                COMPILER_64_TO_32_LO(local_rate_u32, local_rate);
                PRT_CELL_SET("%u", local_rate_u32);
            }
            else
            {
                PRT_CELL_SET("%s", "overflow!!!");
            }
            if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
            {
            PRT_CELL_SET("%s%s", unit_prefix, "pps")}
            else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
            {
                PRT_CELL_SET("%s%s", unit_prefix, "bps");
            }
            else
            {
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type,
                                  EMPTY, EMPTY);
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * See diag_dnx_egr_queuing.h
 */
shr_error_e
sh_dnx_egr_queuing_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    uint32 otm;
    int logical_port, tc;
    int qpair, base_qp;
    int counter_idx, nof_counter, nof_counter_total;
    dnx_egq_counter_rate_info_t rate_info[DNX_EGQ_COUNTER_NUM_SUM];
    char *prefix_string[3];
    uint32 num_prefix_string;
    uint32 kilo;
    uint64 maximal_for_disply;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    num_prefix_string = sizeof(prefix_string) / sizeof(prefix_string[0]);
    {
        /*
         * Initializations for display of counters which are, possibly, 64-bits
         */
        uint32 ii;

        ii = 0;
        prefix_string[ii++] = "M";
        prefix_string[ii++] = "G";
        prefix_string[ii++] = "KG";
        kilo = 1000;
        COMPILER_64_SET(maximal_for_disply, 0, 1000000000);
    }
    nof_counter_total = 0;
    sal_memset(rate_info, 0, sizeof(rate_info));
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("port", logical_port);
    SH_SAND_GET_INT32("TrafficClass", tc);

    if ((EGQ_NOT_INCLUDED_VAL != logical_port) && (EGQ_NOT_INCLUDED_VAL != tc))
    {
        /*
         * Enter if both 'logical_port' and 'tc' ARE NOT set to '-1'.
         * Display counters for logical port (qpair) at priority 'tc' (TC port).
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_qp));
        qpair = base_qp + tc;
        PRT_TITLE_SET("Egress Queuing Rate on HR: port=%d, tc=%d, qpair=%d", logical_port, tc, qpair);
    }
    else if (EGQ_NOT_INCLUDED_VAL != logical_port)
    {
        /*
         * Enter if 'tc' IS set to '-1' but 'logical_port' IS NOT set to '-1'.
         * Display counters for logical port (TM port).
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &otm));
        PRT_TITLE_SET("Egress Queuing Rate on OTM: port=%d, otm=%d", logical_port, otm);
    }
    else
    {
        /*
         * Enter if both 'logical_port' and 'tc' ARE set to '-1'. This is the default.
         * Display all counters.
         */
        SHR_IF_ERR_EXIT(sh_dnx_egq_rqp_counter_rate_get(unit, core, rate_info, &nof_counter));
        nof_counter_total += nof_counter;

        PRT_TITLE_SET("Egress Queuing Rate on core (%d)", core);
    }

    SHR_IF_ERR_EXIT(sh_dnx_egq_pqp_counter_rate_get(unit, core, logical_port, tc, &rate_info[nof_counter_total]));
    nof_counter_total += DNX_EGQ_PQP_COUNTER_NUM;
    SHR_IF_ERR_EXIT(sh_dnx_egq_epep_counter_rate_get
                    (unit, core, logical_port, tc, &rate_info[nof_counter_total], &nof_counter));
    nof_counter_total += nof_counter;

    PRT_COLUMN_ADD("No.");
    PRT_COLUMN_ADD("Coutner");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Unit");

    for (counter_idx = 0; counter_idx < nof_counter_total; counter_idx++)
    {
        char *unit_prefix;

        unit_prefix = "K";
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", counter_idx);
        PRT_CELL_SET("%s", rate_info[counter_idx].name);
        if (!rate_info[counter_idx].count_ovf)
        {
            uint64 local_rate;
            uint32 ii;
            uint32 local_rate_u32;
            /*
             * We assume that the maximal value here is
             * (2^64 / 1000) * 8
             * So that dividing it by 1,000,000,000 will
             * always result in a value which is smaller than 2^32 (uint32)
             */
            COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
            ii = 0;
            while (COMPILER_64_GT(local_rate, maximal_for_disply))
            {
                COMPILER_64_UDIV_32(local_rate, kilo);
                if (ii >= num_prefix_string)
                {
                    /*
                     * If all prefixes were expired then something is wrong in the
                     * estimate of maximal counter value.
                     */
                    uint64 max_expected;

                    COMPILER_64_ALLONES(max_expected);
                    COMPILER_64_UDIV_32(max_expected, kilo);
                    COMPILER_64_UMUL_32(max_expected, 8);
                    COMPILER_64_COPY(local_rate, rate_info[counter_idx].rate);
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Indicated rate (high,low = " COMPILER_64_PRINTF_FORMAT
                                 ") is higher than expected (high,low = " COMPILER_64_PRINTF_FORMAT ")",
                                 COMPILER_64_PRINTF_VALUE(local_rate), COMPILER_64_PRINTF_VALUE(max_expected));
                }
                unit_prefix = prefix_string[ii];
                ii++;
            }
            COMPILER_64_TO_32_LO(local_rate_u32, local_rate);
            PRT_CELL_SET("%u", local_rate_u32);
        }
        else
        {
            PRT_CELL_SET("%s", "overflow!!!");
        }
        if (DNX_EGQ_COUTNER_TYPE_PACKET == rate_info[counter_idx].type)
        {
        PRT_CELL_SET("%s%s", unit_prefix, "pps")}
        else if (DNX_EGQ_COUTNER_TYPE_BYTE == rate_info[counter_idx].type)
        {
            PRT_CELL_SET("%s%s", unit_prefix, "bps");
        }
        else
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid EGQ Counter Type %d %s%s\n", rate_info[counter_idx].type, EMPTY,
                              EMPTY);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * brief - get utilization rate per EGQ IF. the rate is calculated per user-defined interval
 */
shr_error_e
sh_dnx_egr_interface_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    bcm_port_t port;
    bcm_pbmp_t logical_ports, egq_ports;
    bcm_cos_queue_t cosq = -1;
    int egr_if, interval;
    uint32 promile_rate, promile, promile_bytes, promile_mbps, granularity;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_egr_queuing.rate_measurement.feature_get(unit, dnx_data_egr_queuing_rate_measurement_is_supported))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "This Diagnostic command is not supported on the current Device %s%s%s\n", EMPTY,
                          EMPTY, EMPTY);
    }
    SH_SAND_GET_PORT("port", logical_ports);

    /*
     * AND ports with egq ports. if port is not an egq port it will not be displayed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &egq_ports));
    BCM_PBMP_AND(logical_ports, egq_ports);

    /*
     * Get Interval
     */
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_get(unit, 0, cosq, bcmCosqControlEgrRateMeasurementInterval, &interval));

    /**
     * Print table header
     */
    PRT_TITLE_SET("Rate Measure per Interface");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("EGQ IF");
    PRT_COLUMN_ADD("Utilization Rate [Percent]");
    PRT_COLUMN_ADD("Utilization Rate [Mbps]");

    /** Iterate all logical egq_ports */
    BCM_PBMP_ITER(logical_ports, port)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
        SHR_IF_ERR_EXIT(dnx_rate_measurement_counter_get(unit, core, egr_if, &promile_rate));

        PRT_CELL_SET("%d", port);
        PRT_CELL_SET("%d", core);
        PRT_CELL_SET("%d", egr_if);
        PRT_CELL_SET("%3d.%1d%%", promile_rate / 10, promile_rate % 10);

        SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_rate_promile_get(unit, core, egr_if, &promile, &granularity));
        promile_bytes = promile_rate * promile;
        /*
         * promile[Mb/sec] = (promile[bytes] * 1,000,000 [usec/sec] / (1,000,000 [bytes/Mb] * interval [usec]))
         */
        promile_mbps = promile_bytes / interval;
        PRT_CELL_SET("%u", promile_mbps);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
