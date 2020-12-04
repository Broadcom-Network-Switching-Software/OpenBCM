/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hand-coded support for soc_dpp routines. 
 */

#if defined(INCLUDE_LIB_CINT) && defined(BCM_PETRA_SUPPORT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <soc/dpp/debug.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/dpp_fabric_cell.h>
#include <soc/dpp/pkt.h>
#include <bcm_int/dpp/port.h>
#include <soc/dpp/drv.h>
#include <appl/dpp/FecAllocation/fec_allocation.h>

extern int appl_dpp_stk_diag_init(int unit);
extern int appl_dpp_cosq_diag_init(int unit);

#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/ARAD/arad_api_debug.h>
#include <soc/dpp/ARAD/arad_api_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <bcm_int/dpp/cosq.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>

extern bcm_rx_t appl_dcmn_rx_cos_callback_0(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_cos_callback_1(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_cos_callback_2(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_cos_callback_3(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_cos_callback_4(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_cos_callback_5(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_cos_callback_6(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_cos_callback_other(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_intr_callback(int unit, bcm_pkt_t* pkt, void* cookie);
extern bcm_rx_t appl_dcmn_rx_nonintr_callback(int unit, bcm_pkt_t* pkt, void* cookie);

static int 
_SOC_DPP_FABRIC_PORT_TO_LINK(int unit, int port) {
    return SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
}

/* soc dpp fabric cell  { */
/* dpp dump cell   */

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dpp_voq_max_size_drop,
                         int, int, unit, 0, 0,
                         uint32*, uint32, is_max_size, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         soc_dpp_cell_filter_set,
                         int, int, unit, 0, 0,
                         uint32, uint32, array_size, 0, 0,
                         soc_dcmn_filter_type_t*, soc_dcmn_filter_type_t, filter_type_arr, 1, 0,
                         uint32*, uint32, filter_type_val, 1, 0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_dpp_cell_filter_clear,
                         int, int, unit, 0, 0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         appl_dpp_stk_diag_init,
                         int, int, unit, 0, 0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         appl_dpp_cosq_diag_init,
                         int, int, unit, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dpp_cell_filter_receive,
                         int, int, unit, 0, 0,
                         dcmn_captured_cell_t*, dcmn_captured_cell_t, data_out, 1, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_dpp_control_cell_filter_set,
                         int, int, unit, 0, 0,
                         soc_dcmn_control_cell_types_t, soc_dcmn_control_cell_types_t, cell_type, 0, 0,
                         uint32, uint32, array_size, 0, 0,
                         soc_dcmn_control_cell_filter_type_t*, soc_dcmn_control_cell_filter_type_t, control_cell_filter_type_arr, 1, 0,
                         uint32*, uint32, filter_type_val, 1, 0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_dpp_control_cell_filter_clear,
                         int, int, unit, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dpp_control_cell_filter_receive,
                         int, int, unit, 0, 0,
                         soc_dcmn_captured_control_cell_t*, soc_dcmn_captured_control_cell_t, data_out, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         soc_dpp_compilation_vendor_valid,
                         int, int, unit, 0, 0,
                         uint32, uint32, val, 0, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         _bcm_petra_cosq_core_get,
                         int, int, unit, 0, 0,
                         bcm_gport_t, bcm_gport_t, gport, 0, 0,
                         int, int, allow_core_all, 0, 0,
                         int*, int, core, 1, 0);


CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         arad_dbg_autocredit_set,
                         unsigned int, unsigned int, unit, 0, 0,
                         ARAD_DBG_AUTOCREDIT_INFO*, ARAD_DBG_AUTOCREDIT_INFO, info, 1, 0,
                         uint32*, uint32, exact_rate, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         arad_dbg_autocredit_get,
                         unsigned int, unsigned int, unit, 0, 0,
                         ARAD_DBG_AUTOCREDIT_INFO*, ARAD_DBG_AUTOCREDIT_INFO, info, 1, 0);



/*uint32                                                                  */
/*  arad_egr_prog_editor_invalidate_program_by_id(                        */
/*    SOC_SAND_IN  uint32                             unit ,              */
/*    SOC_SAND_IN  ARAD_EGR_PROG_EDITOR_PROGRAMS      prog ,              */
/*    SOC_SAND_IN  int                                prog_selection_valid*/
/*  );                                                                    */
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         arad_egr_prog_editor_invalidate_program_by_id,
                         uint32, uint32, unit, 0, 0,
                         int, int, prog, 0, 0,
                         int, int, prog_selection_valid, 0, 0);

CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_0,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);

CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_1,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);

CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_2,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);
CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_3,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);
CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_4,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);
CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_5,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);
CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_6,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);
CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_cos_callback_other,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);


CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_nonintr_callback,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);

CINT_FWRAPPER_CREATE_RP3(bcm_rx_t, bcm_rx_t, 0, 0,
                         appl_dcmn_rx_intr_callback,
                         unsigned int, unsigned int, unit, 0, 0,
                         bcm_pkt_t*, bcm_pkt_t, pkt, 1, 0,
                         void*, void, cookie, 1, 0);

#if PETRA_DEBUG
CINT_FWRAPPER_CREATE_VP2(pb_ofp_rates_test_random_unsafe,
                         uint32, uint32, unit, 0, 0,
                         uint8, uint8, silent, 0, 0);
#endif

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         _SOC_DPP_FABRIC_PORT_TO_LINK,
                         int, int, unit, 0, 0,
                         int, int, port, 0, 0);

/* reassembly context get */

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         _bcm_dpp_port_reassembly_context_get,
                         int, int, unit, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         uint32*, uint32, port_termination_context, 1, 0,
                         uint32*, uint32, reassembly_context, 1, 0);


CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_dpp_fec_allocation_main,
                         int, int, unit, 0, 0);
 
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_dpp_core_frequency_config_get,
                         int, int, unit, 0, 0,
                         int, int, dflt_freq_khz, 0, 0,
                         uint32*, uint32, freq_khz, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         soc_dpp_max_resource_get,
                         int, int, unit, 0, 0,
                         int, int, core, 0, 0,
                         int, int, resource, 0, 0,
                         uint32*, uint32, max_amount, 1, 0);


static cint_function_t __cint_dpp_functions[] = 
{
    CINT_FWRAPPER_ENTRY(soc_dpp_voq_max_size_drop),
    CINT_FWRAPPER_ENTRY(soc_dpp_cell_filter_set),
    CINT_FWRAPPER_ENTRY(soc_dpp_cell_filter_clear),
    CINT_FWRAPPER_ENTRY(appl_dpp_stk_diag_init),
    CINT_FWRAPPER_ENTRY(appl_dpp_cosq_diag_init),
    CINT_FWRAPPER_ENTRY(soc_dpp_cell_filter_receive),
    CINT_FWRAPPER_ENTRY(soc_dpp_control_cell_filter_set),
    CINT_FWRAPPER_ENTRY(soc_dpp_control_cell_filter_clear),
    CINT_FWRAPPER_ENTRY(soc_dpp_control_cell_filter_receive),
    CINT_FWRAPPER_ENTRY(soc_dpp_compilation_vendor_valid),
    CINT_FWRAPPER_ENTRY(_bcm_petra_cosq_core_get),
    CINT_FWRAPPER_ENTRY(arad_egr_prog_editor_invalidate_program_by_id),
    CINT_FWRAPPER_ENTRY(arad_dbg_autocredit_set),
    CINT_FWRAPPER_ENTRY(arad_dbg_autocredit_get),

    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_0),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_1),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_2),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_3),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_4),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_5),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_6),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_cos_callback_other),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_intr_callback),
    CINT_FWRAPPER_ENTRY(appl_dcmn_rx_nonintr_callback),

#if PETRA_DEBUG
    CINT_FWRAPPER_ENTRY(pb_ofp_rates_test_random_unsafe),
#endif

    CINT_FWRAPPER_ENTRY(_SOC_DPP_FABRIC_PORT_TO_LINK),
    CINT_FWRAPPER_ENTRY(_bcm_dpp_port_reassembly_context_get),
    CINT_FWRAPPER_ENTRY(soc_dpp_fec_allocation_main),
    CINT_FWRAPPER_ENTRY(soc_dpp_core_frequency_config_get),
    CINT_FWRAPPER_ENTRY(soc_dpp_max_resource_get),
    CINT_ENTRY_LAST
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

static cint_parameter_desc_t __cint_struct_members__soc_dcmn_captured_control_cell_t[] =
{
    {
        "int",
        "valid",
        0,
        0
    },
    {
        "soc_dcmn_control_cell_types_t",
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

static cint_parameter_desc_t __cint_struct_members__ARAD_DBG_AUTOCREDIT_INFO[] =
{
    {
        "char",
        "soc_sand_magic_num",
        0,
        0
    },
    {
        "uint32",
        "first_queue",
        0,
        0
    },
    {
        "uint32",
        "last_queue",
        0,
        0
    },
    {
        "uint32",
        "rate",
        0,
        0
    },
    { NULL }
};

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

static void*
__cint_maddr__ARAD_DBG_AUTOCREDIT_INFO(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    ARAD_DBG_AUTOCREDIT_INFO* s = (ARAD_DBG_AUTOCREDIT_INFO*) p;

    switch(mnum)
    {
    case 0: rv = &(s->soc_sand_magic_num); break;
    case 1: rv = &(s->first_queue); break;
    case 2: rv = &(s->last_queue); break;
    case 3: rv = &(s->rate); break;
    default: rv = NULL; break;
    }

    return rv;
}

static cint_struct_type_t __cint_dpp_structures[] =
{   
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
    {
        "ARAD_DBG_AUTOCREDIT_INFO",
        sizeof(ARAD_DBG_AUTOCREDIT_INFO),
        __cint_struct_members__ARAD_DBG_AUTOCREDIT_INFO,
        __cint_maddr__ARAD_DBG_AUTOCREDIT_INFO
    },
    { NULL }
};

static cint_enum_map_t __cint_enum_map__soc_dcmn_control_cell_types_t[] = 
{
    { "soc_dcmn_flow_status_cell", soc_dcmn_flow_status_cell },
    { "soc_dcmn_credit_cell", soc_dcmn_credit_cell },
    { "soc_dcmn_reachability_cell", soc_dcmn_reachability_cell },
    { NULL }
};

static cint_enum_map_t __cint_enum_map__soc_dcmn_control_cell_filter_type_t[] = 
{
    { "soc_dcmn_filter_control_cell_type_source_device", soc_dcmn_filter_control_cell_type_source_device },
    { "soc_dcmn_filter_control_cell_type_dest_device", soc_dcmn_filter_control_cell_type_dest_device },
    { NULL }
};

static cint_enum_map_t __cint_enum_map__soc_dcmn_filter_type_t[] = 
{
    { "soc_dcmn_filter_type_source_id", soc_dcmn_filter_type_source_id },
    { "soc_dcmn_filter_type_multicast_id", soc_dcmn_filter_type_multicast_id },
    { "soc_dcmn_filter_type_priority", soc_dcmn_filter_type_priority },
    { NULL }
};

static cint_enum_type_t __cint_dpp_enums[] = 
{
    { "soc_dcmn_filter_type_t", __cint_enum_map__soc_dcmn_filter_type_t }, 
    { "soc_dcmn_control_cell_types_t", __cint_enum_map__soc_dcmn_control_cell_types_t },
    { "soc_dcmn_control_cell_filter_type_t", __cint_enum_map__soc_dcmn_control_cell_filter_type_t },
    { NULL }
};

static cint_parameter_desc_t __cint_dpp_typedefs[] = 
{
    {
        "uint8",
        "soc_mac_t",
        0,
        6
    },
    {NULL}
};

cint_data_t dpp_cint_data = 
{
    NULL,
    __cint_dpp_functions,
    __cint_dpp_structures,
    __cint_dpp_enums, 
    __cint_dpp_typedefs, 
    NULL, 
    NULL
}; 

#else
typedef int _dpp_cint_data_not_empty; /* Make ISO compilers happy. */
#endif /* defined(INCLUDE_LIB_CINT) & defined(BCM_PETRA_SUPPORT) */

