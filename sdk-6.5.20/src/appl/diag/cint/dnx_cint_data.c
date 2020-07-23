/** \file dnx_cint_data.c 
 * 
 * Expose DNX internal functions, structures, constans, ...  to cint.
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

typedef int dnx_cint_data_not_empty; /* Make ISO compilers happy. */

#if defined(INCLUDE_LIB_CINT) && defined(BCM_DNX_SUPPORT)

/*
 * Include files.
 * {
 */

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <appl/reference/dnx/appl_ref_dynamic_port.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <src/appl/reference/dnx/appl_ref_dram_init.h>
#include <appl/diag/dnx/diag_dnx_sch.h>

#include <bcm/types.h>
#include <bcm/cosq.h>
#include <include/bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <include/bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <src/bcm/dnx/tune/egr_queuing_tune.h>
#include <src/bcm/dnx/tune/scheduler_tune.h>
#include <src/appl/reference/dnx/appl_ref_sys_device.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
/*
 * }
 */


CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         appl_dnx_dynamic_port_create,
                         int,int,unit,0,0,
                         int,int,port,0,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         appl_dnx_dynamic_port_destroy,
                         int,int,unit,0,0,
                         int,int,port,0,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         appl_dnx_e2e_scheme_logical_port_to_base_voq_get,
                         int, int, unit, 0, 0,
                         bcm_module_t, bcm_module_t, module_id, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         int*, int, base_voq, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         appl_dnx_logical_port_to_sysport_get,
                         int, int, unit, 0, 0,
                         bcm_module_t, bcm_module_t, module_id, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         int*, int, system_port, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         appl_dnx_logical_port_to_base_voq_connector_id_get,
                         int, int, unit, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         bcm_module_t, bcm_module_t, remote_modid, 0, 0,
                         int*, int, base_voq_connector, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         sh_dnx_scheduler_port_actual_credit_rate_get,
                         int, int, unit, 0, 0,
                         int, int, core, 0, 0,
                         int, int, port_hr, 0, 0,
                         uint32*, uint32, credit_rate_kbps, 1, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         sh_dnx_scheduler_port_current_fc_info_get,
                         int, int, unit, 0, 0,
                         int, int, core, 0, 0,
                         int, int, port_hr, 0, 0,
                         uint32*, uint32, fc_count, 1, 0,
                         uint32*, uint32, fc_percent, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dnx_dram_buffers_quarantine_nof_free_buffers_get,
                         int, int, unit, 0, 0,
                         uint32*, uint32, nof_free_buffers, 1, 0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dnx_dram_buffers_quarantine_buffer_to_quarantine_schedule,
                         int, int, unit, 0, 0,
                         bcm_core_t, bcm_core_t, core, 0, 0,
                         uint32, uint32, nof_free_buffers, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dnx_dram_buffers_quarantine_buffer_to_quarantine_done,
                         int, int, unit, 0, 0,
                         bcm_core_t, bcm_core_t, core, 0, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_port_phy_measure_get,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,port,0,0,
                         int,int,is_rx,0,0,
                         soc_dnxc_port_phy_measure_t*,soc_dnxc_port_phy_measure_t,phy_measure,1,0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0, appl_dnx_dram_power_up_handle, 
                         int, int, unit, 0, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         dnx_tune_egr_ofp_rate_set,
                         int, int, unit, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         int, int, if_speed, 0, 0,
                         int, int, commit_changes, 0 ,0,
                         int, int, is_remove, 0 ,0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         dnx_tune_scheduler_port_rates_set,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,port,0,0,
                         uint32,uint32,flags,0,0,
                         int,int,if_speed,0,0,
                         int,int,commit_changes,0,0,
                         int, int, is_remove, 0 ,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         appl_dnx_e2e_voq_profiles_get,
                         int, int, unit, 0, 0,
                         int, int, speed, 0, 0,
                         int*, int, delay_tolerance_level, 1, 0,
                         int*, int, rate_class, 1, 0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0, appl_dnx_e2e_profiles_create,
                         int, int, unit, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         appl_dnx_sys_device_ports_get,
                         int, int, unit, 0, 0,
                         bcm_pbmp_t*, bcm_pbmp_t, pbmp, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_algo_port_interface_rate_get,
                         int, int, unit, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         uint32, uint32, flags, 0, 0,
                         int*, int, rate, 1, 0);


CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_calc,
                         int, int, unit, 0, 0,
                         uint32, uint32, rate, 0, 0,
                         uint32, uint32, max_nof_timer_cycles, 0, 0,
                         uint32, uint32, max_grant_size_val, 0, 0,
                         uint32*, uint32, best_grant_size, 1, 0,
                         uint32*, uint32, best_timer_cycles, 1, 0);

static cint_function_t __cint_dnx_functions[] = 
    {
        CINT_FWRAPPER_NENTRY("appl_dnx_dynamic_port_create", appl_dnx_dynamic_port_create),
        CINT_FWRAPPER_NENTRY("appl_dnx_dynamic_port_destroy", appl_dnx_dynamic_port_destroy),
        CINT_FWRAPPER_NENTRY("appl_dnx_e2e_scheme_logical_port_to_base_voq_get",
                              appl_dnx_e2e_scheme_logical_port_to_base_voq_get),
        CINT_FWRAPPER_NENTRY("appl_dnx_logical_port_to_sysport_get",
                              appl_dnx_logical_port_to_sysport_get),
        CINT_FWRAPPER_NENTRY("appl_dnx_logical_port_to_base_voq_connector_id_get",
                              appl_dnx_logical_port_to_base_voq_connector_id_get),
        CINT_FWRAPPER_NENTRY("sh_dnx_scheduler_port_actual_credit_rate_get",
                              sh_dnx_scheduler_port_actual_credit_rate_get),
        CINT_FWRAPPER_NENTRY("sh_dnx_scheduler_port_current_fc_info_get",
                              sh_dnx_scheduler_port_current_fc_info_get),
        CINT_FWRAPPER_NENTRY("dnx_dram_buffers_quarantine_nof_free_buffers_get",
                              dnx_dram_buffers_quarantine_nof_free_buffers_get),
        CINT_FWRAPPER_NENTRY("dnx_dram_buffers_quarantine_buffer_to_quarantine_schedule",
                              dnx_dram_buffers_quarantine_buffer_to_quarantine_schedule),
        CINT_FWRAPPER_NENTRY("dnx_dram_buffers_quarantine_buffer_to_quarantine_done",
                              dnx_dram_buffers_quarantine_buffer_to_quarantine_done),
        CINT_FWRAPPER_NENTRY("dnx_port_phy_measure_get", dnx_port_phy_measure_get),
        CINT_FWRAPPER_NENTRY("appl_dnx_dram_power_up_handle", appl_dnx_dram_power_up_handle),        
        CINT_FWRAPPER_NENTRY("dnx_tune_egr_ofp_rate_set", dnx_tune_egr_ofp_rate_set),
        CINT_FWRAPPER_NENTRY("dnx_tune_scheduler_port_rates_set", dnx_tune_scheduler_port_rates_set),
        CINT_FWRAPPER_NENTRY("appl_dnx_e2e_voq_profiles_get", appl_dnx_e2e_voq_profiles_get),
        CINT_FWRAPPER_NENTRY("appl_dnx_e2e_profiles_create", appl_dnx_e2e_profiles_create),
        CINT_FWRAPPER_NENTRY("appl_dnx_sys_device_ports_get", appl_dnx_sys_device_ports_get),
        CINT_FWRAPPER_NENTRY("dnx_algo_port_interface_rate_get", dnx_algo_port_interface_rate_get),
        CINT_FWRAPPER_NENTRY("dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_calc",
                             dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_calc),
        CINT_ENTRY_LAST
    };  

static cint_parameter_desc_t __cint_struct_members__soc_dnxc_port_phy_measure_t[] =
{
    {
        "int",
        "one_clk_time_measured_int",
        0,
        0
    },
    {
        "int",
        "one_clk_time_measured_remainder",
        0,
        0
    },
    {
        "int",
        "serdes_freq_int",
        0,
        0
    },
    {
        "int",
        "serdes_freq_remainder",
        0,
        0
    },
    {
        "uint32",
        "ref_clk_int",
        0,
        0
    },
    {
        "uint32",
        "ref_clk_remainder",
        0,
        0
    },
    {
        "int",
        "lane",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__soc_dnxc_port_phy_measure_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    soc_dnxc_port_phy_measure_t* s = (soc_dnxc_port_phy_measure_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->one_clk_time_measured_int); break;
        case 1: rv = &(s->one_clk_time_measured_remainder); break;
        case 2: rv = &(s->serdes_freq_int); break;
        case 3: rv = &(s->serdes_freq_remainder); break;
        case 4: rv = &(s->ref_clk_int); break;
        case 5: rv = &(s->ref_clk_remainder); break;
        case 6: rv = &(s->lane); break;
        default: rv = NULL; break;
    }

    return rv;
}

static cint_struct_type_t __cint_dnx_structures[] =
{
    {
    "soc_dnxc_port_phy_measure_t",
    sizeof(soc_dnxc_port_phy_measure_t),
    __cint_struct_members__soc_dnxc_port_phy_measure_t,
    __cint_maddr__soc_dnxc_port_phy_measure_t
    },
    { NULL }
};

/* Defines section */
static cint_constants_t __cint_dnx_constants[] =
{
    { "DNX_ALGO_PORT_SPEED_F_MBPS", DNX_ALGO_PORT_SPEED_F_MBPS },
    { "DNX_ALGO_PORT_SPEED_F_KBPS", DNX_ALGO_PORT_SPEED_F_KBPS },
    { "DNX_ALGO_PORT_SPEED_F_RX", DNX_ALGO_PORT_SPEED_F_RX },
    { "DNX_ALGO_PORT_SPEED_F_TX", DNX_ALGO_PORT_SPEED_F_TX }, 
    { NULL }
};

cint_data_t dnx_cint_data = 
{
    NULL,
    __cint_dnx_functions,
    __cint_dnx_structures,
    NULL, 
    NULL, 
    __cint_dnx_constants, 
    NULL
}; 

#endif /* INCLUDE_LIB_CINT && (BCM_DNX_SUPPORT)*/

