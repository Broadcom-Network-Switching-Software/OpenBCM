/** \file diag_dnx_ingress_rate.c
 * 
 * diagnostics for ingress rate
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
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
/** sal */
#include <sal/appl/sal.h>
#include "diag_dnx_ingress_rate.h"

/** Print ingress counter rate row */
#define DNX_ING_COUNTER_RATE_PRINT_64(_index, _name, _type, _val, _ovf)                        \
{                                                                                              \
    uint32 _rate;                                                                              \
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);                                                             \
    PRT_CELL_SET("%d", _index++);                                                              \
    PRT_CELL_SET("%s", _name);                                                                 \
    if (_type == DNX_INGRESS_COUTNER_TYPE_PACKET)                                              \
    {                                                                                          \
        COMPILER_64_UDIV_32(_val,1000);                                                        \
        _rate = COMPILER_64_LO(_val);                                                          \
        if (_ovf)                                                                              \
        {                                                                                      \
            PRT_CELL_SET("overflow!!!");                                                       \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            PRT_CELL_SET("%u", _rate);                                                         \
        }                                                                                      \
        PRT_CELL_SET("kpps");                                                                  \
    }                                                                                          \
    else if (_type == DNX_INGRESS_COUTNER_TYPE_BYTE)                                           \
    {                                                                                          \
        COMPILER_64_UMUL_32(_val,UTILEX_NOF_BITS_IN_BYTE);                                     \
        COMPILER_64_UDIV_32(_val,1000);                                                        \
        _rate = COMPILER_64_LO(_val);                                                          \
        if (_ovf)                                                                              \
        {                                                                                      \
            PRT_CELL_SET("overflow!!!");                                                       \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            PRT_CELL_SET("%u", _rate);                                                         \
        }                                                                                      \
        PRT_CELL_SET("kbps");                                                                  \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress counter type (%d).\n", _type);             \
    }                                                                                          \
}

#define DNX_ING_COUNTER_RATE_PRINT_32(_index, _name, _type, _val, _ovf)                        \
{                                                                                              \
    uint32 _rate;                                                                              \
    uint64 _tmp_64;                                                                            \
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);                                                             \
    PRT_CELL_SET("%d", _index++);                                                              \
    PRT_CELL_SET("%s", _name);                                                                 \
    if (_type == DNX_INGRESS_COUTNER_TYPE_PACKET)                                              \
    {                                                                                          \
        _val /= 1000;                                                                          \
        _rate = _val;                                                                          \
        if (_ovf)                                                                              \
        {                                                                                      \
            PRT_CELL_SET("overflow!!!");                                                       \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            PRT_CELL_SET("%u", _rate);                                                         \
        }                                                                                      \
        PRT_CELL_SET("kpps");                                                                  \
    }                                                                                          \
    else if (_type == DNX_INGRESS_COUTNER_TYPE_BYTE)                                           \
    {                                                                                          \
        COMPILER_64_SET(_tmp_64,0,_val);                                                       \
        COMPILER_64_UMUL_32(_tmp_64,UTILEX_NOF_BITS_IN_BYTE);                                  \
        COMPILER_64_UDIV_32(_tmp_64,1000);                                                     \
        _rate = COMPILER_64_LO(_tmp_64);                                                       \
        if (_ovf)                                                                              \
        {                                                                                      \
            PRT_CELL_SET("overflow!!!");                                                       \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            PRT_CELL_SET("%u", _rate);                                                         \
        }                                                                                      \
        PRT_CELL_SET("kbps");                                                                  \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress counter type (%d).\n", _type);             \
    }                                                                                          \
}

/** Specify the max size for the title of rate table */
#define DNX_ING_RATE_TABLE_TITLE_MAX_SIZE    (64)

/**
 * \brief - calculate and display ingress ire couter rate
 */
shr_error_e
sh_dnx_ingress_ire_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, core_id;
    int counter_idx;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_ingress_ire_counter_info_t counter_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** get input parameters */
    SH_SAND_GET_INT32("core", core_id);

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        PRT_TITLE_SET("Ingress IRE Rate on core (%d)", core);
        PRT_COLUMN_ADD("No.");
        PRT_COLUMN_ADD("Counter");
        PRT_COLUMN_ADD("Rate");
        PRT_COLUMN_ADD("Unit");

        SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_IRE, core));
        SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_IRE, core));

        SHR_IF_ERR_EXIT(dnx_ingress_ire_counter_info_get(unit, core, &counter_info));

        counter_idx = 0;
        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "NIF Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.nif_pkt_cnt, counter_info.nif_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "RCYH Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.rcyh_pkt_cnt, counter_info.rcyh_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "RCYL Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.rcyl_pkt_cnt, counter_info.rcyl_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "CPU Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.cpu_pkt_cnt, counter_info.cpu_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "OLP Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.olp_pkt_cnt, counter_info.olp_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "OAMP Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.oamp_pkt_cnt, counter_info.oamp_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "Register Interface Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.reg_if_pkt_cnt, counter_info.reg_if_pkt_ovf);

        PRT_COMMITX;

        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_CGM, core));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief Options for DNX TM Ingress IRE rate diag
 */

/**
 * \brief MAN for DNX TM Ingress IRE rate diag
 */
sh_sand_man_t sh_dnx_ingress_ire_rate_man = {
/* *INDENT-OFF* */
    .brief    = "IRE counter rate calculation and display",
    .full     = "Calculate and display IRE counter rate per core \n"
                "If Core is provided, the rate will be returned only for the specified core."
                "The counters provided are: \n"
                "   - NIF Packet Count\n"
                "   - RCYH Packet Count\n"
                "   - RCYL Packet Count\n"
                "   - CPU Packet Count\n"
                "   - OLP Packet Count\n"
                "   - OAMP Packet Count\n"
                "   - Register Interface Packet Count\n",
    .synopsis = "[core=<id>]",
    .examples = "core=0 \n"
  
/* *INDENT-ON* */
};

/**
 * \brief - calculate and display ingress sqm couter rate
 */
shr_error_e
sh_dnx_ingress_sqm_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, core_id;
    int counter_idx;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_ingress_sqm_counter_info_t counter_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** get input parameters */
    SH_SAND_GET_INT32("core", core_id);

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        PRT_TITLE_SET("Ingress SQM Rate on core (%d)", core);
        PRT_COLUMN_ADD("No.");
        PRT_COLUMN_ADD("Counter");
        PRT_COLUMN_ADD("Rate");
        PRT_COLUMN_ADD("Unit");

        SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_SQM, core));
        SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_SQM, core));

        SHR_IF_ERR_EXIT(dnx_ingress_sqm_counter_info_get(unit, core, &counter_info));

        counter_idx = 0;
        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "EN-QUEUE Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.pkt_enq_cnt, counter_info.enq_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "DE-QUEUE Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.pkt_deq_cnt, counter_info.deq_pkt_ovf);

        PRT_COMMITX;

        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_SQM, core));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief Options for DNX TM Ingress SQM rate diag
 */

/**
 * \brief MAN for DNX TM Ingress SQM rate diag
 */
sh_sand_man_t sh_dnx_ingress_sqm_rate_man = {
/* *INDENT-OFF* */
    .brief    = "SQM counter rate calculation and display",
    .full     = "Calculate and display SQM counter rate per core \n"
                "If Core is provided, the rate will be returned only for the specified core."
                "The counters provided are: \n"
                "   - En-queue Packet Count\n"
                "   - De-queue Packet Count\n",
    .synopsis = "[core=<id>]",
    .examples = "core=0 \n"
  
/* *INDENT-ON* */
};

/**
 * \brief - calculate and display ingress ipt couter rate
 */
shr_error_e
sh_dnx_ingress_ipt_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, core_id;
    int counter_idx;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    dnx_ingress_ipt_counter_info_t counter_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** get input parameters */
    SH_SAND_GET_INT32("core", core_id);

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        PRT_TITLE_SET("Ingress IPT Rate on core (%d)", core);
        PRT_COLUMN_ADD("No.");
        PRT_COLUMN_ADD("Counter");
        PRT_COLUMN_ADD("Rate");
        PRT_COLUMN_ADD("Unit");

        SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_IPT, core));
        SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_IPT, core));

        SHR_IF_ERR_EXIT(dnx_ingress_ipt_counter_info_get(unit, core, &counter_info));

        counter_idx = 0;
        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "Packet from CGM Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.pkt_cgm_cnt, counter_info.cgm_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "Packet from SPB Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.pkt_spb_cnt, counter_info.spb_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "ITPP Discard Packet Count", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.pkt_itpp_disc_cnt, counter_info.itpp_disc_pkt_ovf);
        PRT_COMMITX;

        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_IPT, core));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief Options for DNX TM Ingress IPT rate diag
 */

/**
 * \brief MAN for DNX TM Ingress IPT rate diag
 */
sh_sand_man_t sh_dnx_ingress_ipt_rate_man = {
/* *INDENT-OFF* */
    .brief    = "IPT counter rate calculation and display",
    .full     = "Calculate and display IPT counter rate per core \n"
                "If Core is provided, the rate will be returned only for the specified core."
                "The counters provided are: \n"
                "   - Packet from CGM Count\n"
                "   - Packet from SPB Count\n"
                "   - Packet ITPP Discard Count\n",
    .synopsis = "[core=<id>]",
    .examples = "core=0 \n"
  
/* *INDENT-ON* */
};

/**
 * \brief - calculate and display ingress cgm couter rate
 */
shr_error_e
sh_dnx_ingress_cgm_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char title[DNX_ING_RATE_TABLE_TITLE_MAX_SIZE];
    int core, core_id;
    int counter_idx, nof_cos = 1;
    int queue_id_provided, base_queue_id_provided;
    uint32 cosq_flags;
    uint32 nof_clock_cycles;
    uint32 queue_id, base_queue_id;
    dnxcmn_time_t gtimer_time;
    dnx_ingress_congestion_counter_info_t counter_info;
    bcm_gport_t base_gport = BCM_GPORT_INVALID, logical_port = BCM_GPORT_INVALID;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("queue", queue_id_provided);
    SH_SAND_IS_PRESENT("basequeue", base_queue_id_provided);

    /** get input parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_UINT32("queue", queue_id);
    SH_SAND_GET_UINT32("basequeue", base_queue_id);

    if (queue_id_provided)
    {
        base_queue_id = queue_id;
        nof_cos = 1;
        sal_snprintf(title, DNX_ING_RATE_TABLE_TITLE_MAX_SIZE, "Ingress CGM Rate: queue=%d", base_queue_id);
    }
    else if (base_queue_id_provided)
    {
        /*
         * Find if the base_queue_id provided is pointing to a UC or MC group.
         *  The purpose of this is to see how many queues are there in the bundle.
         */
        SHR_IF_ERR_EXIT(dnx_ipq_queue_gport_get(unit, BCM_CORE_ALL, base_queue_id, &base_gport));

        
        SHR_IF_ERR_EXIT(bcm_cosq_gport_get(unit, base_gport, &logical_port, &nof_cos, &cosq_flags));

        sal_snprintf(title, DNX_ING_RATE_TABLE_TITLE_MAX_SIZE, "Ingress CGM Rate: base_queue=%d nof_cos=%d",
                     base_queue_id, nof_cos);
    }
    else
    {
        base_queue_id = 0;
        nof_cos = dnx_data_ipq.queues.nof_queues_get(unit);
        sal_snprintf(title, DNX_ING_RATE_TABLE_TITLE_MAX_SIZE, "Ingress CGM Rate on All Queues");
    }

    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        PRT_TITLE_SET("%s on core(%d)", title, core);
        PRT_COLUMN_ADD("No.");
        PRT_COLUMN_ADD("Counter");
        PRT_COLUMN_ADD("Rate");
        PRT_COLUMN_ADD("Unit");

        /** Configure the programmable counter to track */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_configuration_set
                        (unit, core, base_queue_id, nof_cos));

        SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_CGM, core));
        SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_CGM, core));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_info_get(unit, core, &counter_info));

        counter_idx = 0;
        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "SRAM Enqueued packets", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.sram_enq_pkt_cnt, counter_info.sram_enq_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "SRAM Rejected packets", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.sram_disc_pkt_cnt, counter_info.sram_disc_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "SRAM to DRAM Dequeued packets", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.sram_to_dram_pkt_cnt, counter_info.sram_to_dram_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "SRAM to Fabric Dequeued packets", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.sram_to_fabric_pkt_cnt, counter_info.sram_to_fabric_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_32(counter_idx, "SRAM Deleted packets", DNX_INGRESS_COUTNER_TYPE_PACKET,
                                      counter_info.sram_del_pkt_cnt, counter_info.sram_del_pkt_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "SRAM Enqueued bytes", DNX_INGRESS_COUTNER_TYPE_BYTE,
                                      counter_info.sram_enq_byte_cnt, counter_info.sram_enq_byte_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "SRAM Discarded bytes", DNX_INGRESS_COUTNER_TYPE_BYTE,
                                      counter_info.sram_disc_byte_cnt, counter_info.sram_disc_byte_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "SRAM Deleted bytes", DNX_INGRESS_COUTNER_TYPE_BYTE,
                                      counter_info.sram_del_byte_cnt, counter_info.sram_del_byte_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "SRAM to DRAM Dequeued bytes", DNX_INGRESS_COUTNER_TYPE_BYTE,
                                      counter_info.sram_to_dram_byte_cnt, counter_info.sram_to_dram_byte_ovf);

        DNX_ING_COUNTER_RATE_PRINT_64(counter_idx, "SSRAM to fabric Dequeued bytes", DNX_INGRESS_COUTNER_TYPE_BYTE,
                                      counter_info.sram_to_fabric_byte_cnt, counter_info.sram_to_fabric_byte_ovf);

        PRT_COMMITX;

        /** Disable the counter
         * Last argument configures the VOQ_MASK which is (num_cos - 1).
         * That is why counter is disabled with num_cos == 1 */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_configuration_set(unit, core, 0, 1));

        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_CGM, core));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief Options for DNX TM Ingress CGM rate diag
 */
sh_sand_option_t sh_dnx_ingress_cgm_rate_options[] = {
/* *INDENT-OFF* */
    /*name        type                   desc                                            default*/
    {"queue",     SAL_FIELD_TYPE_UINT32, "voq id - used to count a single voq",          "0", tm_queue_id_table, "0-max"},
    {"basequeue", SAL_FIELD_TYPE_UINT32, "base voq id - used to count the whole bundle", "0", tm_queue_id_table, "0-max"},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief MAN for DNX TM Ingress CGM rate diag
 */
sh_sand_man_t sh_dnx_ingress_cgm_rate_man = {
/* *INDENT-OFF* */
    .brief    = "CGM counter rate calculation and display",
    .full     = "Calculate and display CGM counter rate per Queue or per BaseQueue \n"
                "If Queue is provided, the rate will be returned only for the specified queue."
                "Else if BaseQueue is provided the rate will be returned for the whole queue bundle."
                "The counters provided are: \n"
                "   - SRAM enqueued packets\n"
                "   - SRAM discarded packets\n"
                "   - SRAM to Fabric dequeued packets\n"
                "   - SRAM to DRAM dequeued packets\n"
                "   - SRAM deleted packets\n"
                "   - SRAM Enqueued bytes\n"
                "   - SRAM Discarded bytess\n"
                "   - SRAM Deleted bytes\n"
                "   - SRAM to DRAM Dequeued bytes\n"
                "   - SRAM to fabric Dequeued bytes",
    .synopsis = "[Queue=<id>] [BaseQueue=<id>] [core=<id>]",
    .examples = "q=34 \n"
                "bq=32 \n"
  
/* *INDENT-ON* */
};
