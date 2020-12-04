/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hand-coded support for soc_dfe routines. 
 */
typedef int dfe_cint_data_not_empty; /* Make ISO compilers happy. */

#if defined(INCLUDE_LIB_CINT) && defined(BCM_DFE_SUPPORT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dfe/cmn/dfe_fabric_cell.h>
#include <soc/dfe/cmn/dfe_fabric_cell_snake_test.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/dfe_port.h>

#include <bcm_int/dfe/fabric.h>

#include <bcm/error.h>

#ifdef BCM_88750_SUPPORT
#include <soc/dfe/fe1600/fe1600_fabric_links.h>
#endif

    

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_dfe_cell_filter_set,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         uint32,uint32,array_size,0,0,
                         soc_dcmn_filter_type_t*,soc_dcmn_filter_type_t,filter_type_arr,1,0,
                         uint32*,uint32,filter_type_val,1,0);    

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_dfe_control_cell_filter_clear,
                         int,int,unit,0,0);    

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dfe_cell_filter_receive,
                         int,int,unit,0,0,
                         dcmn_captured_cell_t*,dcmn_captured_cell_t,data_out,1,0);

/* soc dfe fabric control cell */
CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         soc_dfe_control_cell_filter_set,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         soc_dcmn_control_cell_types_t,soc_dcmn_control_cell_types_t,cell_type,0,0, 
                         uint32,uint32,array_size,0,0,
                         soc_dcmn_control_cell_filter_type_t*,soc_dcmn_control_cell_filter_type_t,filter_type_arr,1,0,
                         uint32*,uint32,filter_type_val,1,0);  

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_dfe_cell_filter_clear,
                         int,int,unit,0,0);    

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dfe_control_cell_filter_receive,
                         int,int,unit,0,0,
                         soc_dcmn_captured_control_cell_t*,soc_dcmn_captured_control_cell_t,data_out,1,0);

/* soc dfe fabric cell snake test { */
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dfe_cell_snake_test_prepare,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0);                
        
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dfe_cell_snake_test_run,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         soc_fabric_cell_snake_test_results_t*,soc_fabric_cell_snake_test_results_t,results,1,0);


/* soc dfe fabric cell snake test } */

/*Flow status cell format - should be used just for lab validation {*/        
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         _bcm_dfe_fabric_link_flow_status_cell_format_set,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,flags,0,0,
                         int,int,cell_format,0,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         _bcm_dfe_fabric_link_flow_status_cell_format_get,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,flags,0,0,
                         int*,int,cell_format,1,0);
/*Flow status cell format - should be used just for lab validation }*/        

/* soc dfe fabric { */

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_dfe_fabric_link_status_all_get,
                         int,int,unit,0,0,
                         int,int,links_array_max_size,0,0,
                         uint32*,uint32,link_status,1,0,
                         uint32*,uint32,errored_token_count,1,0, 
                         int*,int,links_array_count,1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dfe_port_loopback_set,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,port,0,0,
                         soc_dcmn_loopback_mode_t,soc_dcmn_loopback_mode_t,loopback,0,0);  

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dfe_port_loopback_get,
                         int,int,unit,0,0,
                         bcm_port_t,bcm_port_t,port,0,0,
                         soc_dcmn_loopback_mode_t*,soc_dcmn_loopback_mode_t,loopback,1,0);
 
/* soc dfe fabric } */

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dfe_diag_cell_pipe_counter_get,
                         int,int,unit,0,0,
                         int,int,pipe,0,0,
                         uint64*,uint64,counter,1,0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         soc_dfe_drv_link_to_dch_block,
                         int, int, unit, 0, 0,
                         int, int, link, 0, 0,
                         int*, int, block_instance, 1, 0,
                         int*, int, inner_link, 1, 0);

static cint_function_t __cint_dfe_functions[] = 
    {
        CINT_FWRAPPER_ENTRY(soc_dfe_cell_snake_test_prepare),
        CINT_FWRAPPER_ENTRY(soc_dfe_cell_snake_test_run),
        CINT_FWRAPPER_ENTRY(_bcm_dfe_fabric_link_flow_status_cell_format_set),
        CINT_FWRAPPER_ENTRY(_bcm_dfe_fabric_link_flow_status_cell_format_get),
        CINT_FWRAPPER_ENTRY(soc_dfe_fabric_link_status_all_get),
        CINT_FWRAPPER_ENTRY(soc_dfe_cell_filter_set),
        CINT_FWRAPPER_ENTRY(soc_dfe_cell_filter_clear),
        CINT_FWRAPPER_ENTRY(soc_dfe_cell_filter_receive),
        CINT_FWRAPPER_ENTRY(soc_dfe_control_cell_filter_set),
        CINT_FWRAPPER_ENTRY(soc_dfe_control_cell_filter_clear),
        CINT_FWRAPPER_ENTRY(soc_dfe_control_cell_filter_receive),
        CINT_FWRAPPER_ENTRY(soc_dfe_port_loopback_set),
        CINT_FWRAPPER_ENTRY(soc_dfe_port_loopback_get),
        CINT_FWRAPPER_ENTRY(soc_dfe_diag_cell_pipe_counter_get),

        CINT_FWRAPPER_ENTRY(soc_dfe_drv_link_to_dch_block),

        CINT_ENTRY_LAST
    };  

static cint_parameter_desc_t __cint_struct_members__soc_fabric_cell_snake_test_results_t[] =
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

static cint_parameter_desc_t __cint_struct_members__soc_dcmn_captured_control_cell_t[] =
{
    {
        "int",
        "valid",
        0,
        0
    },
    {
        "soc_dfe_control_cell_types_t",
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

static cint_parameter_desc_t __cint_struct_members__dcmn_captured_cell_t[] =
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

static void*
__cint_maddr__soc_fabric_cell_snake_test_results_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    soc_fabric_cell_snake_test_results_t* s = (soc_fabric_cell_snake_test_results_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->test_failed); break;
        case 1: rv = &(s->interrupts_status); break;
        case 2: rv = &(s->failure_stage_flags); break;
        case 3: rv = &(s->tdm_lfsr_value); break;
        case 4: rv = &(s->non_tdm_lfsr_value); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
__cint_maddr__dcmn_captured_cell_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    dcmn_captured_cell_t* s = (dcmn_captured_cell_t*) p;

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
__cint_maddr__soc_dcmn_captured_control_cell_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    soc_dcmn_captured_control_cell_t* s = (soc_dcmn_captured_control_cell_t*) p;

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

static cint_struct_type_t __cint_dfe_structures[] =
{   
    {
    "soc_fabric_cell_snake_test_results_t",
    sizeof(soc_fabric_cell_snake_test_results_t),
    __cint_struct_members__soc_fabric_cell_snake_test_results_t,
    __cint_maddr__soc_fabric_cell_snake_test_results_t
    },
    {
    "dcmn_captured_cell_t",
    sizeof(dcmn_captured_cell_t),
    __cint_struct_members__dcmn_captured_cell_t,
    __cint_maddr__dcmn_captured_cell_t
    },
    {
    "soc_dcmn_captured_control_cell_t",
    sizeof(soc_dcmn_captured_control_cell_t),
    __cint_struct_members__soc_dcmn_captured_control_cell_t,
    __cint_maddr__soc_dcmn_captured_control_cell_t
    },
    { NULL }
};



static cint_enum_map_t __cint_dfe_enum_map__soc_dcmn_control_cell_types_t[] = 
{
    { "soc_dcmn_flow_status_cell", soc_dcmn_flow_status_cell },
    { "soc_dcmn_credit_cell", soc_dcmn_credit_cell },
    { "soc_dcmn_reachability_cell", soc_dcmn_reachability_cell },
    { "soc_dcmn_fe2_filtered_cell", soc_dcmn_fe2_filtered_cell },
    { NULL }
};

static cint_enum_map_t __cint_dfe_enum_map__soc_dcmn_control_cell_filter_type_t[] = 
{
    { "soc_dcmn_filter_control_cell_type_source_device", soc_dcmn_filter_control_cell_type_source_device },
    { "soc_dcmn_filter_control_cell_type_dest_device", soc_dcmn_filter_control_cell_type_dest_device },
    { NULL }
};

static cint_enum_map_t __cint_dfe_enum_map__soc_dcmn_filter_type_t[] = 
{
    { "soc_dcmn_filter_type_source_id", soc_dcmn_filter_type_source_id },
    { "soc_dcmn_filter_type_multicast_id", soc_dcmn_filter_type_multicast_id },
    { "soc_dcmn_filter_type_priority", soc_dcmn_filter_type_priority },
    { NULL }
};

static cint_enum_map_t __cint_dfe_enum_map__soc_dcmn_loopback_mode_t[] = 
{
    { "soc_dcmn_loopback_mode_none", soc_dcmn_loopback_mode_none },
    { "soc_dcmn_loopback_mode_mac_outer", soc_dcmn_loopback_mode_mac_outer },
    { "soc_dcmn_loopback_mode_mac_pcs", soc_dcmn_loopback_mode_mac_pcs },
    { "soc_dcmn_loopback_mode_mac_async_fifo", soc_dcmn_loopback_mode_mac_async_fifo },
    { "soc_dcmn_loopback_mode_phy_rloop", soc_dcmn_loopback_mode_phy_rloop },
    { "soc_dcmn_loopback_mode_phy_gloop", soc_dcmn_loopback_mode_phy_gloop },
    { NULL }
};

static cint_enum_type_t __cint_dfe_enums[] = 
{
    { "soc_dcmn_filter_type_t", __cint_dfe_enum_map__soc_dcmn_filter_type_t }, 
    { "soc_dcmn_control_cell_types_t", __cint_dfe_enum_map__soc_dcmn_control_cell_types_t },
    { "soc_dcmn_control_cell_filter_type_t", __cint_dfe_enum_map__soc_dcmn_control_cell_filter_type_t },
    { "soc_dcmn_loopback_mode_t", __cint_dfe_enum_map__soc_dcmn_loopback_mode_t },    
    { NULL }
};

static cint_constants_t __cint_dfe_constants[] = 
{
   { "SOC_DFE_ENABLE_MAC_LOOPBACK", SOC_DFE_ENABLE_MAC_LOOPBACK },
   { "SOC_DFE_ENABLE_PHY_LOOPBACK", SOC_DFE_ENABLE_PHY_LOOPBACK },
   { "SOC_DFE_ENABLE_EXTERNAL_LOOPBACK", SOC_DFE_ENABLE_EXTERNAL_LOOPBACK },
   { "SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK", SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK },
   { "SOC_DFE_DONT_TOUCH_MAC_INTERRUPTS", SOC_DFE_DONT_TOUCH_MAC_INTERRUPTS },
   { "SOC_DFE_SNAKE_INFINITE_RUN", SOC_DFE_SNAKE_INFINITE_RUN },
   { "SOC_DFE_SNAKE_STOP_TEST", SOC_DFE_SNAKE_STOP_TEST },   
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG },
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET },
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION },
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION },
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND },
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND },
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND },
   { "SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND", SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND },   
   { "SOC_CELL_FILTER_FLAG_DONT_FORWARD", SOC_CELL_FILTER_FLAG_DONT_FORWARD },   
   { "SOC_CELL_FILTER_FLAG_DONT_TRAP", SOC_CELL_FILTER_FLAG_DONT_TRAP }, 
   { NULL }
};

cint_data_t dfe_cint_data = 
{
    NULL,
    __cint_dfe_functions,
    __cint_dfe_structures,
    __cint_dfe_enums, 
    NULL, 
    __cint_dfe_constants, 
    NULL
}; 

#endif /* INCLUDE_LIB_CINT && BCM_DFE_SUPPORT*/

