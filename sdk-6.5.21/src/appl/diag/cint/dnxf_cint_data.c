/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hand-coded support for soc_dnxf routines. 
 */
typedef int dnxf_cint_data_not_empty; /* Make ISO compilers happy. */

#if defined(INCLUDE_LIB_CINT) && defined(BCM_DNXF_SUPPORT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/dnxf_drv.h>

#include <bcm_int/dnxf/fabric.h>
#include <include/bcm_int/dnxf/port.h>

#include <bcm/error.h>
    

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_dnxf_cell_filter_set,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         uint32,uint32,array_size,0,0,
                         soc_dnxc_filter_type_t*,soc_dnxc_filter_type_t,filter_type_arr,1,0,
                         uint32*,uint32,filter_type_val,1,0);    

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_dnxf_control_cell_filter_clear,
                         int,int,unit,0,0);    

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dnxf_cell_filter_receive,
                         int,int,unit,0,0,
                         dnxc_captured_cell_t*,dnxc_captured_cell_t,data_out,1,0);

/* soc dnxf fabric control cell */
CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         soc_dnxf_control_cell_filter_set,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         soc_dnxc_control_cell_types_t,soc_dnxc_control_cell_types_t,cell_type,0,0, 
                         uint32,uint32,array_size,0,0,
                         soc_dnxc_control_cell_filter_type_t*,soc_dnxc_control_cell_filter_type_t,filter_type_arr,1,0,
                         uint32*,uint32,filter_type_val,1,0);  

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_dnxf_cell_filter_clear,
                         int,int,unit,0,0);    

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dnxf_control_cell_filter_receive,
                         int,int,unit,0,0,
                         soc_dnxc_captured_control_cell_t*,soc_dnxc_captured_control_cell_t,data_out,1,0);

/*Flow status cell format - should be used just for lab validation {*/        
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         _bcm_dnxf_fabric_link_flow_status_cell_format_set,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,flags,0,0,
                         int,int,cell_format,0,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         _bcm_dnxf_fabric_link_flow_status_cell_format_get,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,flags,0,0,
                         int*,int,cell_format,1,0);
/*Flow status cell format - should be used just for lab validation }*/        

/* soc dnxf fabric cell snake test { */
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dnxf_cell_snake_test_prepare,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dnxf_cell_snake_test_run,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         soc_dnxf_fabric_cell_snake_test_results_t*,soc_dnxf_fabric_cell_snake_test_results_t,results,1,0);

/* soc dnxf fabric cell snake test } */

/* soc dnxf fabric { */
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dnxf_port_loopback_set,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,port,0,0,
                         portmod_loopback_mode_t,portmod_loopback_mode_t,loopback,0,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dnxf_port_loopback_get,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,port,0,0,
                         portmod_loopback_mode_t*,portmod_loopback_mode_t,loopback,1,0);
 
/* soc dnxf fabric } */

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dnxf_diag_cell_pipe_counter_get,
                         int,int,unit,0,0,
                         int,int,pipe,0,0,
                         uint64*,uint64,counter,1,0);

/* dnxf phy measure { */

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnxf_port_phy_measure_get,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,port,0,0,
                         int,int,is_rx,0,0,
                         soc_dnxc_port_phy_measure_t*,soc_dnxc_port_phy_measure_t,phy_measure,1,0);

/* dnxf phy measure } */
static cint_function_t __cint_dnxf_functions[] = 
    {
        CINT_FWRAPPER_ENTRY(soc_dnxf_cell_snake_test_prepare),
        CINT_FWRAPPER_ENTRY(soc_dnxf_cell_snake_test_run),
        CINT_FWRAPPER_ENTRY(_bcm_dnxf_fabric_link_flow_status_cell_format_set),
        CINT_FWRAPPER_ENTRY(_bcm_dnxf_fabric_link_flow_status_cell_format_get),
        CINT_FWRAPPER_ENTRY(soc_dnxf_cell_filter_set),
        CINT_FWRAPPER_ENTRY(soc_dnxf_cell_filter_clear),
        CINT_FWRAPPER_ENTRY(soc_dnxf_cell_filter_receive),
        CINT_FWRAPPER_ENTRY(soc_dnxf_control_cell_filter_set),
        CINT_FWRAPPER_ENTRY(soc_dnxf_control_cell_filter_clear),
        CINT_FWRAPPER_ENTRY(soc_dnxf_control_cell_filter_receive),
        CINT_FWRAPPER_ENTRY(soc_dnxf_port_loopback_set),
        CINT_FWRAPPER_ENTRY(soc_dnxf_port_loopback_get),
        CINT_FWRAPPER_ENTRY(soc_dnxf_diag_cell_pipe_counter_get),
        CINT_FWRAPPER_ENTRY(dnxf_port_phy_measure_get),

        CINT_ENTRY_LAST
    };  

static cint_parameter_desc_t __cint_struct_members__soc_dnxf_fabric_cell_snake_test_results_t[] =
{
    {
        "int",
        "test_failed",
        0,
        0
    },
    {
        "uint64",
        "interrupts_status",
        0,
        0
    },
    {
        "uint32",
        "failure_stage_flags",
        0,
        0
    },
    {
        "int",
        "tdm_lfsr_value",
        0,
        0
    },
    {
        "int",
        "non_tdm_lfsr_value",
        0,
        0
    },
    { NULL }
};


static cint_parameter_desc_t __cint_struct_members__soc_dnxc_captured_control_cell_t[] =
{
    {
        "int",
        "valid",
        0,
        0
    },
    {
        "soc_dnxc_control_cell_types_t",
        "control_type",
        0,
        0
    },
    {
        "int",
        "dest_device",
        0,
        0
    },
    {
        "int",
        "source_device",
        0,
        0
    },
    {
        "int",
        "dest_port",
        0,
        0
    },
    {
        "int",
        "src_queue_num",
        0,
        0
    },
    {
        "int",
        "dest_queue_num",
        0,
        0
    },
    {
        "int",
        "sub_flow_id",
        0,
        0
    },
    {
        "int",
        "flow_id",
        0,
        0
    },
    {
        "int",
        "reachability_bitmap",
        0,
        0
    },
    {
        "int",
        "base_index",
        0,
        0
    },
    {
        "int",
        "source_link_number",
        0,
        0
    },
    { NULL }
};

static cint_parameter_desc_t __cint_struct_members__dnxc_captured_cell_t[] =
{
    {
        "int",
        "dest",
        0,
        0
    },
    {
        "int",
        "source_device",
        0,
        0
    },
    {
        "int",
        "is_last_cell",
        0,
        0
    },
    { NULL }
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
__cint_maddr__soc_dnxf_fabric_cell_snake_test_results_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    soc_dnxf_fabric_cell_snake_test_results_t* s = (soc_dnxf_fabric_cell_snake_test_results_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->test_failed); break;
        case 1: rv = &(s->interrupts_status); break;
        case 2: rv = &(s->failure_stage_flags); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
__cint_maddr__dnxc_captured_cell_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    dnxc_captured_cell_t* s = (dnxc_captured_cell_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->dest); break;
        case 1: rv = &(s->source_device); break;
        case 2: rv = &(s->is_last_cell); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
__cint_maddr__soc_dnxc_captured_control_cell_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    soc_dnxc_captured_control_cell_t* s = (soc_dnxc_captured_control_cell_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->valid); break;
        case 1: rv = &(s->control_type); break;
        case 2: rv = &(s->dest_device); break;
        case 3: rv = &(s->source_device); break;
        case 4: rv = &(s->dest_port); break;
        case 5: rv = &(s->src_queue_num); break;
        case 6: rv = &(s->dest_queue_num); break;
        case 7: rv = &(s->sub_flow_id); break;
        case 8: rv = &(s->flow_id); break;
        case 9: rv = &(s->reachability_bitmap); break;
        case 10: rv = &(s->base_index); break;
        case 11: rv = &(s->source_link_number); break;
        default: rv = NULL; break;
    }

    return rv;
}

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

static cint_struct_type_t __cint_dnxf_structures[] =
{   
    {
    "soc_dnxf_fabric_cell_snake_test_results_t",
    sizeof(soc_dnxf_fabric_cell_snake_test_results_t),
    __cint_struct_members__soc_dnxf_fabric_cell_snake_test_results_t,
    __cint_maddr__soc_dnxf_fabric_cell_snake_test_results_t
    },
    {
    "dnxc_captured_cell_t",
    sizeof(dnxc_captured_cell_t),
    __cint_struct_members__dnxc_captured_cell_t,
    __cint_maddr__dnxc_captured_cell_t
    },
    {
    "soc_dnxc_captured_control_cell_t",
    sizeof(soc_dnxc_captured_control_cell_t),
    __cint_struct_members__soc_dnxc_captured_control_cell_t,
    __cint_maddr__soc_dnxc_captured_control_cell_t
    },
    {
    "soc_dnxc_port_phy_measure_t",
    sizeof(soc_dnxc_port_phy_measure_t),
    __cint_struct_members__soc_dnxc_port_phy_measure_t,
    __cint_maddr__soc_dnxc_port_phy_measure_t
    },
    { NULL }
};



static cint_enum_map_t __cint_dnxf_enum_map__soc_dnxc_control_cell_types_t[] = 
{
    { "soc_dnxc_flow_status_cell", soc_dnxc_flow_status_cell },
    { "soc_dnxc_credit_cell", soc_dnxc_credit_cell },
    { "soc_dnxc_reachability_cell", soc_dnxc_reachability_cell },
    { "soc_dnxc_fe2_filtered_cell", soc_dnxc_fe2_filtered_cell },
    { NULL }
};

static cint_enum_map_t __cint_dnxf_enum_map__soc_dnxc_control_cell_filter_type_t[] = 
{
    { "soc_dnxc_filter_control_cell_type_source_device", soc_dnxc_filter_control_cell_type_source_device },
    { "soc_dnxc_filter_control_cell_type_dest_device", soc_dnxc_filter_control_cell_type_dest_device },
    { NULL }
};

static cint_enum_map_t __cint_dnxf_enum_map__soc_dnxc_filter_type_t[] = 
{
    { "soc_dnxc_filter_type_source_id", soc_dnxc_filter_type_source_id },
    { "soc_dnxc_filter_type_multicast_id", soc_dnxc_filter_type_multicast_id },
    { "soc_dnxc_filter_type_priority", soc_dnxc_filter_type_priority },
    { NULL }
};

static cint_enum_type_t __cint_dnxf_enums[] = 
{
    { "soc_dnxc_filter_type_t", __cint_dnxf_enum_map__soc_dnxc_filter_type_t }, 
    { "soc_dnxc_control_cell_types_t", __cint_dnxf_enum_map__soc_dnxc_control_cell_types_t },
    { "soc_dnxc_control_cell_filter_type_t", __cint_dnxf_enum_map__soc_dnxc_control_cell_filter_type_t },
    { NULL }
};

static cint_constants_t __cint_dnxf_constants[] = 
{
   { "SOC_DNXF_ENABLE_MAC_LOOPBACK", SOC_DNXF_ENABLE_MAC_LOOPBACK },
   { "SOC_DNXF_ENABLE_PHY_LOOPBACK", SOC_DNXF_ENABLE_PHY_LOOPBACK },
   { "SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK", SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK },
   { "SOC_DNXF_ENABLE_ASYNC_FIFO_LOOPBACK", SOC_DNXF_ENABLE_ASYNC_FIFO_LOOPBACK },
   { "SOC_DNXF_DONT_TOUCH_MAC_INTERRUPTS", SOC_DNXF_DONT_TOUCH_MAC_INTERRUPTS },
   { "SOC_DNXF_SNAKE_INFINITE_RUN", SOC_DNXF_SNAKE_INFINITE_RUN },
   { "SOC_DNXF_SNAKE_STOP_TEST", SOC_DNXF_SNAKE_STOP_TEST },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND },
   { "SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND", SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND },
   { "SOC_CELL_FILTER_FLAG_DONT_FORWARD", SOC_CELL_FILTER_FLAG_DONT_FORWARD },   
   { "SOC_CELL_FILTER_FLAG_DONT_TRAP", SOC_CELL_FILTER_FLAG_DONT_TRAP }, 
   { NULL }
};

cint_data_t dnxf_cint_data = 
{
    NULL,
    __cint_dnxf_functions,
    __cint_dnxf_structures,
    __cint_dnxf_enums, 
    NULL, 
    __cint_dnxf_constants, 
    NULL
}; 

#endif /* INCLUDE_LIB_CINT && BCM_DNXF_SUPPORT*/

