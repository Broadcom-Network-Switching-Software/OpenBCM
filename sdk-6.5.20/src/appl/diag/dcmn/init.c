/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        init.c
 * Purpose:     Device initialization commands.
 *
 * DPP diag init code using bcm API. The code in here is intended to 
 * configure the system for simple packet flows.
 */

#include <shared/bsl.h>

#if defined(__DUNE_GTO_BCM_CPU__) && (defined(LINUX) || defined(UNIX)) && (defined(DCMN_INIT_DMA_DEBUG))
/* Debug - DMA */
#include <fcntl.h> /* open */
#include <sys/ioctl.h> /* ioctl */
#endif

#if defined(__DUNE_GTO_BCM_CPU__) && (defined(LINUX) || defined(UNIX)) && !defined(__KERNEL__)
#include <stdlib.h>
#endif

#include <ibde.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm/switch.h>
#include <bcm/types.h>
#include <appl/diag/diag.h>
#include <appl/diag/bslenable.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/uc_msg.h>
#include <soc/shared/llm_msg.h>
#include <soc/shared/llm_pack.h>
#include <soc/shared/llm_appl.h>
#include <soc/cmicm.h>
#endif

#ifdef BCM_SBUSDMA_SUPPORT
#include <soc/sbusdma_internal.h>
#endif

#include <soc/dcmn/utils_fpga.h>
#include <soc/dcmn/dcmn_utils_eeprom.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#endif

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif

#include <soc/i2c.h>

#if defined(INCLUDE_DUNE_UI)
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#endif

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dcmn/init.h>
#include <appl/diag/dcmn/utils_board_general.h>
#include <appl/diag/system.h>  
#include <appl/diag/dcmn/credit_request_thresholds.h>

#include <appl/reference/sand/appl_ref_sand.h>

#ifdef BCM_PETRA_SUPPORT
#include <appl/diag/dpp/utils_arad_card.h>
#endif

#ifdef BCM_DFE_SUPPORT
#include <appl/diag/dfe/utils_fe1600_card.h>
#endif


#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/SAND/Management/sand_device_management.h>

#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/petra_dispatch.h>
#endif

#include <sal/appl/sal.h>
#include <appl/diag/dcmn/init_deinit.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/ARAD/arad_mgmt.h>
#endif /* BCM_PETRA_SUPPORT */

#ifdef PLISIM
#include <../systems/sim/dpp/ChipSim/chip_sim.h>
#endif /* PLISIM */
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)
extern int _cpu_write(int d, uint32 addr, uint32 *buf);
extern int _cpu_read(int d, uint32 addr, uint32 *buf);
extern int _cpu_pci_register(int d);
#endif


/* Update the macro in case of a new device that not yet support MC groups */
#define MC_GROUPS_SUPPORTED(unit) 1

#define PCP_DEVICE_ID (SOC_SAND_MAX_DEVICE-1)

#define MAX_NUM_DEVICES (SOC_SAND_MAX_DEVICE)
#define MAX_COS         (8)
#define MAX_MODIDS_PER_DEVICE 2
#define MODIDS_PER_DEVICE (g_dii.is_symmetric ? MAX_MODIDS_PER_DEVICE : 1)
#define OLP_DEFAULT_CORE 0
#define CORE_ID2INDEX(core_id) ((core_id == BCM_CORE_ALL) ? 0 : core_id)
#define NOF_SYS_PORTS_PER_DEVICE 1024

#define NOF_DEF_SLOW_SPEEDS (4)

#define DNX_DEFAULT_ERP_PORT(core_id) (254 + core_id)

#ifdef BCM_PETRA_SUPPORT
typedef struct appl_dpp_diag_init_s {
    int         my_modid;
    int         my_modid_idx;
    int         base_modid;
    int         nof_devices;
    int         num_ports;
    int         num_cos;
    int         cpu_sys_port;
    int         internal_ports[MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
    int         num_internal_ports;
    int         offset_start_of_internal_ports;
    int         offset_start_of_sys_port;
    int         offset_start_of_voq;
    int         offset_start_of_voq_connector;
    bcm_gport_t dest_local_gport[SOC_MAX_NUM_PORTS];
    bcm_gport_t dest_local_gport_nof_priorities[SOC_MAX_NUM_PORTS];   /* number of priorities per port */ 
    bcm_gport_t gport_ucast_voq_connector_group[MAX_NUM_DEVICES][MAX_MODIDS_PER_DEVICE][SOC_MAX_NUM_PORTS];
    bcm_gport_t gport_ucast_queue_group[MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
    bcm_gport_t gport_ucast_scheduler[SOC_MAX_NUM_PORTS][MAX_COS];
    uint32 nof_active_cores;
    uint32 is_symmetric;
}appl_dpp_diag_init_t;

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
typedef struct { /* h/w desc structure */
    uint32 cntrl;    /* DMA control info */
    uint32 req;      /* DMA request info (refer h/w spec for details) */
    uint32 count;    /* DMA count */
    uint32 opcode;   /* Schan opcode (refer h/w spec for details) */
    uint32 addr;     /* Schan address */
    uint32 hostaddr; /* h/w mapped host address */
} soc_sbusdma_desc_t;
#endif

static appl_dpp_diag_init_t   g_dii; /* global diag init info */

static int  g_base_modid; /* global base_modid */

/* 
 * Utils function: Converting between IDs
 */
/* return the index for the internal port stored in the internal_ports[] array */
STATIC int
appl_dpp_port_internal_id_get(int modid_idx, int port)
{
    int port_i;
    for (port_i=0; port_i<g_dii.num_internal_ports; port_i++) {
    if (g_dii.internal_ports[modid_idx][port_i] == port) {
        return port_i;
    }
    }
    return -1;
}

/* Convert (mod,port) id to sysport id */
STATIC int
appl_dpp_convert_modport_to_sysport_id(int unit, int modid_idx,int port)
{

    int sys_port = 0;
    int rv, is_dnx_app;

    /* Run DNX system ports allocation */
    is_dnx_app = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "dnx_scheme", 0);
    if (is_dnx_app)
    {
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t   mapping_info;
        uint32 dummy_flags;

        rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_port_get failed. Error:%d (%s)\n"),
                       rv, bcm_errmsg(rv)));
            return rv;
        }

        /** override local_port id for ERP port since in DPP application the local port for ERP ports is bigger than 512 */
        if (port == BCM_DPP_PORT_INTERNAL_ERP(mapping_info.core))
        {
            port = DNX_DEFAULT_ERP_PORT(mapping_info.core);
        }

        rv = appl_sand_device_to_sysport_convert(unit, modid_idx, NOF_SYS_PORTS_PER_DEVICE, port, &sys_port);
        if (rv < 0)
        {
            sys_port = -1;
        }
    }
    else /* Run DPP system ports allocation */
    {
        int internal_port_idx;

        /* Petra-B ITMH cannot be sent to system port 0, so we're using system port
                   different than 0 instead (for CPU). ARAD can use system port 0 but we will do the same as Petra-B */
        if (port == 0) {
            port = g_dii.cpu_sys_port;
        }

        internal_port_idx = appl_dpp_port_internal_id_get(modid_idx, port);
        if (internal_port_idx != -1) {
        return (g_dii.offset_start_of_sys_port * modid_idx + internal_port_idx 
            + g_dii.offset_start_of_internal_ports);
        }

        sys_port = g_dii.offset_start_of_sys_port * modid_idx + port;
    }

    return sys_port;
}

/* Convert sysport to base VOQ id */
STATIC int
appl_dpp_convert_sysport_id_to_base_voq_id(int unit, int sysport)
{

    int base_voq = 0;
    int is_dnx_app, rv;

    is_dnx_app = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "dnx_scheme", 0);

    /* Run DNX voqs allocation */
    if (is_dnx_app)
    {
        rv = appl_sand_sysport_id_to_base_voq_id_convert(unit, sysport, &base_voq);
        if (rv < 0)
        {
            return -1;
        }
    }
    else /* Run DPP voqs allocation */
    {
        /* Assume, no system port 0 */
        if (sysport == 0) {
            return -1;
        }

        base_voq = g_dii.offset_start_of_voq + (sysport-1)*g_dii.num_cos;
    }

    return base_voq;
}

/* Convert (local port,ingress_modid) to VOQ connector base */
STATIC int
appl_dpp_convert_modport_to_base_voq_connector(int unit, int local_port, int core_id, int ingress_modid)
{
    int voq_conn_id;
    int is_dnx_app, rv;

    is_dnx_app = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "dnx_scheme", 0);

    /* Run DNX voqs allocation */
    if (is_dnx_app)
    {
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t   mapping_info;
        uint32 dummy_flags;
        int ingress_device;

        ingress_device = ingress_modid/MAX_MODIDS_PER_DEVICE;

        rv = bcm_port_get(unit, local_port, &dummy_flags, &interface_info, &mapping_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_port_get failed. Error:%d (%s)\n"),
                       rv, bcm_errmsg(rv)));
            return rv;
        }

        /** override local_port id for ERP port since in DPP application the local port for ERP ports is bigger than 512 */
        if (local_port == BCM_DPP_PORT_INTERNAL_ERP(mapping_info.core))
        {
            local_port = DNX_DEFAULT_ERP_PORT(mapping_info.core);
        }

        rv = appl_sand_device_to_base_voq_connector_id_convert(unit, local_port, core_id, ingress_device, g_dii.nof_devices, &voq_conn_id);
        if (rv < 0)
        {
            return -1;
        }
    }
    else /* Run DPP voq connectors allocation */
    {
        int num_of_512, voq_conn_internal_id;
     
        voq_conn_id = g_dii.offset_start_of_voq_connector + local_port * (g_dii.nof_devices * MAX_MODIDS_PER_DEVICE + g_dii.base_modid) * MAX_COS
            + ingress_modid * MAX_COS;

        /* 
         * For dual core support (SYMMETRIC MODE)
         * Each 1024 connectors are devided to 512 connected to core 0 and 512 connected to core 1
         * so this map remap connectors to valid range in symmetric mode
         */
        num_of_512 = voq_conn_id / 512;
        voq_conn_internal_id = voq_conn_id % 512;
        voq_conn_id = (num_of_512 * 1024) + (CORE_ID2INDEX(core_id) * 512) + voq_conn_internal_id;
    }

    return voq_conn_id;
}

/* 
 * Convert local port to an example of Remote XGS port and XGS Modid 
 * See convert details in function 
 */
STATIC void
appl_dpp_convert_port_to_remote_modid_port(int unit, int local_port, int* remote_modid, int* remote_port)
{
    /* 
     * Remote Modid 0 <=> local_port 0-63
     * Remote Modid 1 <=> local_port 64-127
     * Remote Modid 2 <=> local_port 128-191
     * Remote Modid 3 <=> local_port 192-255
     */
    *remote_modid = (local_port >> 6);

    /* Remote Port = local_port first 6 bits */
    *remote_port =  (local_port & 0x3F);    
}

STATIC int
appl_dpp_is_xgs_mac_extender_port(int unit, int local_port)
{
    char *propkey, *propval;

    propkey = spn_TM_PORT_HEADER_TYPE;
    propval = soc_property_port_suffix_num_get_str(unit, local_port, 0, propkey, "in");

    if (propval) {
      if (sal_strcmp(propval, "XGS_MAC_EXT") == 0) {
        return 1;
      }
    }

    return 0;
}

/* 
 * Purpose: Initializes Looks at current configuration and populates
 *          g_dii based on that
 */
STATIC int
appl_dpp_misc_diag_init (int unit, int mymodid, int base_modid, int nof_devices)
{
    int         rv = BCM_E_NONE;
    int         port_i = 0;
    int         modid_idx = 0;

    g_dii.num_cos = NUM_COS(unit);
    LOG_VERBOSE(BSL_LS_APPL_COMMON,
                (BSL_META_U(unit,
                            "g_dii.num_cos is %d \n"),
                 g_dii.num_cos));

    /* Assuming modid is between base_modid-(base_modid+nof_devices*MAX_MODIDS_PER_DEVICE-1) */
    g_dii.my_modid = mymodid;
    g_dii.nof_devices = nof_devices;

    rv = soc_dpp_device_core_mode(unit, &(g_dii.is_symmetric), &(g_dii.nof_active_cores));
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "Device_core_mode parsing failed\n"))); 
        return rv;
    }
    if (base_modid != -1) {
        g_base_modid = base_modid;  /* Range configuration between base_modid-(base_modid + nof_devices*MAX_MODIDS_PER_DEVICE - 1) */
    }
    g_dii.base_modid = g_base_modid;
    g_dii.my_modid_idx = (mymodid - g_dii.base_modid) / MAX_MODIDS_PER_DEVICE;
    
    if (mymodid >= (g_dii.nof_devices * MAX_MODIDS_PER_DEVICE + g_dii.base_modid) || mymodid < g_dii.base_modid) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "appl_dpp_misc_diag_init failed. "
                               "Diag application assumes mymodid (%d) is in a valid range of modids"
                               " between base_modid(%d) and (base_modid+nof_devices*MAX_MODIDS_PER_DEVICE-1)=%d.\n"), 
                               mymodid, g_dii.base_modid, g_dii.nof_devices * MAX_MODIDS_PER_DEVICE + g_dii.base_modid));
        return BCM_E_PARAM;
    }

    g_dii.cpu_sys_port = 255;

    /* Offset sysport per device */
    g_dii.offset_start_of_sys_port = 256;
    /* Offset VOQ */
    g_dii.offset_start_of_voq = 32;
    /* Offset VOQ connector */
    g_dii.offset_start_of_voq_connector = 32;

    /* on local device internal ports will start at 240 */
    g_dii.offset_start_of_internal_ports = 240;
    g_dii.num_internal_ports = 0;

    for (modid_idx = 0; modid_idx < nof_devices; modid_idx++) {
        for (port_i=0; port_i<SOC_MAX_NUM_PORTS; port_i++) {
            g_dii.internal_ports[modid_idx][port_i] = -1;
        }
    }    

    return rv;
}

int
appl_dpp_stk_diag_init(int unit)
{
    int rv = BCM_E_NONE;
    int idx, index, port, sys_port, modid_idx;
    bcm_gport_t internal_gport[SOC_MAX_NUM_PORTS];
    bcm_gport_t modport_gport,sysport_gport;
    int int_flags;
    uint32 dummy_flags;
    bcm_port_config_t port_config;
    bcm_pbmp_t pbmp;
    int remote_modid = 0;
    int remote_port  = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;


    /* Set modid */
    rv = bcm_stk_my_modid_set(unit, g_dii.my_modid);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_stk_my_modid_set failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }


    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_port_config_get failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /* Internal ports are not part of the pbmp */

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_OR(pbmp, port_config.nif);
    BCM_PBMP_OR(pbmp, port_config.cpu);
    BCM_PBMP_OR(pbmp, port_config.rcy);
    BCM_PBMP_OR(pbmp, port_config.sat);
    BCM_PBMP_OR(pbmp, port_config.ipsec);


    BCM_PBMP_OR(pbmp, PBMP_LBG_ALL(unit));

    BCM_PBMP_COUNT(pbmp, g_dii.num_ports);

    /* 
     * Have XGS MAC extender default mappings example.
     * Map (HG.Modid, HG.Port) to local_port. 
     * In example we have convert function between ARAD local port to HG.Port and HG.Modid. 
     * Note: XGS MAC extender ports: the first port interface channel 0 (ucode_port_x=INTF.0) must be between 0-15.
     */
    BCM_PBMP_ITER(pbmp, port) {
        if (appl_dpp_is_xgs_mac_extender_port(unit, port)) {
            appl_dpp_convert_port_to_remote_modid_port(unit, port, &remote_modid, &remote_port);

#ifdef DEBUG_PORTS
            cli_out("XGS MAC extender: ARAD port(%d) remote modid(%d) remote port(%d\n", 
                    (port), remote_modid, remote_port);
#endif  
            rv = bcm_stk_modport_remote_map_set(unit, port, 0,
                                           remote_modid, remote_port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_stk_modport_remote_map_get(%d, %d) failed. "
                                       "Error:%d (%s)\n"), unit, port, 
                           rv, bcm_errmsg(rv)));
                return rv;
            }
        }

    }

    idx = 0;
    BCM_PBMP_ITER(pbmp, port) {
        
        /* initialize local ports array */
        BCM_GPORT_LOCAL_SET(g_dii.dest_local_gport[idx],port);

        /* per local port, indicate port number of priorities */

        rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_port_get failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }

        g_dii.dest_local_gport_nof_priorities[idx] = mapping_info.num_priorities;

        idx++;

    }

    /* 
     * Creation of system ports in the system 
     * Iterate over port + internal ports. After the iteration, num_ports will
     *  be incremented by internal_num_ports 
     * Assuming each device has the same num ports + internal ports  
     */    
    for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {       
        
        BCM_PBMP_ITER(pbmp, port) {

            /* System port id depends on modid + port id */ 
            sys_port = appl_dpp_convert_modport_to_sysport_id(unit, modid_idx, port);
            
            if (BCM_FAILURE(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info))) {
                LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Invalid local port %d\n"), port));
                return BCM_E_PARAM;
            }
            BCM_GPORT_MODPORT_SET(modport_gport, modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid + mapping_info.core, mapping_info.tm_port);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
                
            rv = bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_stk_sysport_gport_set(%d, %d, %d) failed. "
                                       "Error:%d (%s)\n"), unit, sysport_gport, 
                           modport_gport, rv, bcm_errmsg(rv)));
                return rv;
            }
        }


        /* Now set up internal ports ERP/OLP/OAMP */
        /* Get number of internal ports */
        /* Assuming all devices have the same amount internal ports */    
        int_flags =
            BCM_PORT_INTERNAL_EGRESS_REPLICATION |
            BCM_PORT_INTERNAL_OAMP |
            BCM_PORT_INTERNAL_OLP;
        rv = bcm_port_internal_get(unit, int_flags, SOC_MAX_NUM_PORTS,
                       internal_gport, &g_dii.num_internal_ports);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_port_internal_get failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }
        
        for (index = 0; index < g_dii.num_internal_ports; ++index) {
            
            if (BCM_GPORT_IS_LOCAL(internal_gport[index])) {
                port = BCM_GPORT_LOCAL_GET(internal_gport[index]);
                g_dii.internal_ports[modid_idx][index] = port;
            } else if (BCM_GPORT_IS_MODPORT(internal_gport[index])) {
                port = BCM_GPORT_MODPORT_PORT_GET(internal_gport[index]);
                g_dii.internal_ports[modid_idx][index] = port;
            } else {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_port_internal_get internal gport type unsupported gport(0x%08x). "
                                       "Error:%d (%s)\n"),internal_gport[index],rv, bcm_errmsg(rv)));
                return rv;
            }

            BCM_GPORT_LOCAL_SET(g_dii.dest_local_gport[idx], port);

            if (BCM_FAILURE(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info))) {
                LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Invalid local port %d\n"), port));
                return BCM_E_PARAM;
            }
            g_dii.dest_local_gport_nof_priorities[idx] = mapping_info.num_priorities;

            /* System port id depends on modid + port id */ 
            sys_port = appl_dpp_convert_modport_to_sysport_id(unit, modid_idx, port);

            if (port == BCM_DPP_PORT_INTERNAL_ERP(mapping_info.core)) {mapping_info.tm_port = SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID;} 
            BCM_GPORT_MODPORT_SET(modport_gport, modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid + mapping_info.core, mapping_info.tm_port);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
    #ifdef DEBUG_PORTS            
            cli_out("idx(%d) modid_idx(%d) port(%d) sysport(%d) dest_local(0x%08x) internal\n", 
                    (idx), modid_idx, port, sys_port, g_dii.dest_local_gport[idx]);
    #endif        
            rv = bcm_stk_sysport_gport_set(unit, sysport_gport, 
                           modport_gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_stk_sysport_gport_set(%d, %d, %d) failed. "
                                       "Error:%d (%s)\n"), unit, sysport_gport, 
                           modport_gport, rv, bcm_errmsg(rv)));
                return rv;
            }
            idx++;
        }

        if (g_dii.num_internal_ports > 0) {
            LOG_VERBOSE(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "appl_dpp_stk_diag_init: Adding %d internal ports modid_idx(%d)\n"),
                         g_dii.num_internal_ports, modid_idx));
        }
    }

    g_dii.num_ports += g_dii.num_internal_ports;

    LOG_VERBOSE(BSL_LS_APPL_COMMON,
                (BSL_META_U(unit,
                            "g_dii.num_ports is %d \n"),
                 g_dii.num_ports));
    
    return rv;
}

/*
 */
int
appl_dpp_cosq_diag_scheduler_slow_rate_single_fap_set(
    int unit)
{
    int rv = BCM_E_NONE;

    int default_slow_rates[8] = {
        15,   /** level 0 -- rate in GBits per second */
        30,   /** level 1 -- rate in GBits per second */
        50,  /** level 2 -- rate in GBits per second */
        60,  /** level 3 -- rate in GBits per second */
        70,  /** level 4 -- rate in GBits per second */
        80,  /** level 5 -- rate in GBits per second */
        90,  /** level 6 -- rate in GBits per second */
        99   /** level 7 -- rate in GBits per second */
    };

    int level, slow_type;
    int rate_kbps;
    bcm_cosq_slow_level_t slow_level;
    bcm_cosq_slow_profile_attributes_t attr;

    /** set slow rates */
    for (level = 0; level < 8; level++)
    {

        rate_kbps = default_slow_rates[level] * 1000000; /** Gbits to Kbits */

        for (slow_type = 1; slow_type <= 2; slow_type++)
        {
            slow_level.core = BCM_CORE_ALL;
            slow_level.profile = slow_type;
            slow_level.level = level;

            attr.max_rate = rate_kbps;

            rv = bcm_petra_cosq_slow_profile_set(unit, &slow_level, &attr);
            if (BCM_FAILURE(rv))
            {
                LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_petra_cosq_slow_profile_set failed, Error:%d (%s)\n"), 
                               rv, bcm_errmsg(rv)));
                return rv;
            }
        }
    }

    return rv;
}

/* 
 * Define credit request profiles
 */
STATIC int
appl_dpp_cosq_diag_credit_request_thresholds_define(int unit)
{
    int rv = BCM_E_NONE;
    int is_coldboot = TRUE;

#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            is_coldboot = FALSE;
        }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (is_coldboot) {
        uint32 speed_idx;
        int is_slow = 0;
        char * fabric_connect_mode = NULL;
        APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE system_mode = APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_MGMT;
        uint32 default_port_speeds[NOF_DEF_SLOW_SPEEDS] = {10000, 40000, 100000, 200000}; /* default speeds */

        if (SOC_IS_JERICHO(unit)) {
            /* In case of local switch, slow level thresholds should be divided by a factor (AE feedback).
             * Local switch thresholds havn't been tested for Arad, so it won't be applied for Arad devices. 
             */
            fabric_connect_mode = soc_property_get_str(unit, spn_FABRIC_CONNECT_MODE);
            if (fabric_connect_mode != NULL && sal_strcmp(fabric_connect_mode, "SINGLE_FAP") == 0) {
                system_mode = APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_SINGLE_FAP;

                /* Set different slow profile than the default one if working in a single switch mode*/
                rv = appl_dpp_cosq_diag_scheduler_slow_rate_single_fap_set(unit);
                if (BCM_FAILURE(rv))
                {
                    LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "appl_dpp_cosq_diag_scheduler_slow_rate_single_fap_set, Error:%d (%s)\n"), 
                                   rv, bcm_errmsg(rv)));
                    return rv;
                }
            }
        }

        /* Define 1G credit request profile */
        rv = appl_dpp_cosq_diag_credit_request_thresholds_set(unit, 1000, is_slow, system_mode);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "appl_dpp_cosq_diag_credit_request_thresholds_set failed. speed(1000), is_slow(%d), Error:%d (%s)\n"), 
                           is_slow, rv, bcm_errmsg(rv)));
            return rv;
        }

        /* Define SLOW and LOW_DELAY credit request profiles */
        for (is_slow = 0; is_slow < 2; ++is_slow) {
            for (speed_idx = 0; speed_idx < NOF_DEF_SLOW_SPEEDS; ++speed_idx) {
                rv = appl_dpp_cosq_diag_credit_request_thresholds_set(unit, default_port_speeds[speed_idx], is_slow, system_mode);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "appl_dpp_cosq_diag_credit_request_thresholds_set failed. speed(%u), is_slow(%d), Error:%d (%s)\n"), 
                                   default_port_speeds[speed_idx], is_slow, rv, bcm_errmsg(rv)));
                    return rv;
                }
            }
        }

        if (SOC_IS_JERICHO(unit)) {
            /* Define high priority credit request profile */
            rv = appl_dpp_cosq_diag_credit_request_thresholds_set(unit, 200000, 0, APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_HIGH_PRIO);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "appl_dpp_cosq_diag_credit_request_thresholds_set failed(HIGH_PRIO). Error:%d (%s)\n"), 
                               rv, bcm_errmsg(rv)));
                return rv;
            }
        }
    }

    return rv;
}

/* 
 * Configure queue bundle gport according to port.
 */
int appl_dpp_cosq_diag_credit_request_thresholds_config(int unit, int flags, bcm_port_t port, bcm_gport_t modport_gport, bcm_port_interface_info_t *interface_info, bcm_cosq_ingress_queue_bundle_gport_config_t *config)
{
    int rv = BCM_E_NONE;
    int cosq;
    int default_delay_tolerance_level;
    int speed;

    if (interface_info->interface == BCM_PORT_IF_CPU || BCM_PBMP_IS_NULL(interface_info->phy_pbmp)) {
        speed = -1;
    } else if (interface_info->interface == BCM_PORT_IF_LBG) {
        speed = 10000;
    } else {
        rv = bcm_port_speed_get(unit, port, &speed);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_port_speed_get port(%d) failed. Error:%d (%s) \n"), 
                       port, rv, bcm_errmsg(rv)));
           return rv;
        }
        if (interface_info->interface == BCM_PORT_IF_ILKN) {
            speed *= interface_info->num_lanes;
        }
    }
    /* Set credit request profile according to port's speed */
    if (speed <= 1000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_1G;
    } else if (speed <= 10000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
    } else if (speed <= 40000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED;
    } else if (speed <= 100000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_100G_SLOW_ENABLED;
    } else {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_200G_SLOW_ENABLED;
    }
    config->flags = flags;
    config->port = modport_gport;
    config->local_core_id = BCM_CORE_ALL;
    config->numq = g_dii.num_cos;
    for (cosq = 0 ; cosq < BCM_COS_COUNT; cosq++) {
        /*setting all of the credit request profiles to default slow enabled mode (corresponds to port speed)*/
        config->queue_atrributes[cosq].delay_tolerance_level = default_delay_tolerance_level;
        /*setting all of the rate classes zero*/
        config->queue_atrributes[cosq].rate_class = 0;
    }

    return rv;
}

int
appl_dpp_cosq_diag_init(int unit)
{
    int                         rv = BCM_E_NONE;
    int                         idx, cos, port;
    uint32                      flags, dummy_flags;
    bcm_cosq_gport_connection_t connection;
    int mc_min_queue_range = 0, mc_max_queue_range = 0;
    int modid_idx, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t voq_connector_gport, sysport_gport, modport_gport, voq_base_gport, e2e_gport;
    bcm_gport_t gport_mcast_queue_group;
    bcm_cos_t tc_i;
    bcm_cos_queue_t to_cosq;
    int int_flags, core_id, is_pdm_wide_mode, is_hdr_comp_tag_exists;
    int local_erp_port[BCM_PIPES_MAX] = {-1};
    int num_erp_ports = 0;
    bcm_gport_t egress_q_gport;          
    bcm_port_config_t port_config;
    int egress_port, my_domain, domain;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_ingress_queue_bundle_gport_config_t ingress_config;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    bcm_pbmp_t                pbmp;
    bcm_cosq_voq_connector_gport_t config;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;


    {
        rv = bcm_petra_stk_domain_modid_get(unit, g_dii.my_modid, &my_domain);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_petra_stk_domain_modid_get failed. "
                                   "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
                return rv;
        }
        
        /* 
         * Before creating VOQs, User must specify the range of the FMQs in
         * the device.
         * In most cases, where fabric multicast is only defined by packet tc,
         * range should set between 0-3. 
         * Set range that is different than 0-3, need to change fabric scheduler
         * mode. 
         * Please see more details in the UM, Cint example: 
         * cint_enhance_application.c 
         * and API: 
         * bcm_fabric_control_set type: bcmFabricMulticastSchedulerMode. 
         */
        mc_min_queue_range = 0;
        rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMin,mc_min_queue_range);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_fabric_control_set mc min queue_id :%d failed. "
                               "Error:%d (%s)\n"), mc_min_queue_range, rv, bcm_errmsg(rv)));
            return rv;
        }
        mc_max_queue_range = 3;
        rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMax,mc_max_queue_range);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_fabric_control_set mc max queue_id :%d failed. "
                               "Error:%d (%s)\n"), mc_max_queue_range, rv, bcm_errmsg(rv)));
            return rv;
        }

        /* Create Multicast VOQ */
        BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_mcast_queue_group, BCM_CORE_ALL, mc_min_queue_range);            
        flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
        rv = bcm_cosq_gport_add(unit, 0, BCM_COS_DEFAULT, flags, &gport_mcast_queue_group);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_cosq_gport_add MC queue failed. Error:%d (%s) \n"), 
                       rv, bcm_errmsg(rv)));
           return rv;
        }

        LOG_VERBOSE(BSL_LS_APPL_COMMON,
                    (BSL_META_U(unit,
                                "mcast gport(0x%08x)\n"),
                     gport_mcast_queue_group));

    
        /* Get ERP port */
        int_flags =
            BCM_PORT_INTERNAL_EGRESS_REPLICATION;
        rv = bcm_port_internal_get(unit, int_flags, BCM_PIPES_MAX,
                   local_erp_port, &num_erp_ports);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
              (BSL_META_U(unit,
                          "bcm_port_internal_get failed. Error:%d (%s)\n"),  
               rv, bcm_errmsg(rv)));
            return rv;
        }

        /* 
         * Creating Scheduling Scheme 
         * This is done with the following stages: 
         * 1. Egress: Create for each local port: following FQs, VOQ connectors. 
         * 2. Ingress: Create VOQs for each system port. 
         * 3. Connect Ingress VOQs <=> Egress VOQ connectors. 
         * Pay attention the scheduling scheme assumes static IDs for VOQ connectors,VOQs. 
         * This is depended by the local and system ports in the system.
         * Conversion is done static using utils functions. 
         */             

        /* Stage I: Egress change HR to enhanced mode */
        for (idx = 0; idx < g_dii.num_ports; idx++) {
            /* Replace HR mode to enhance */
            flags = (BCM_COSQ_GPORT_SCHEDULER |
                    BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED |
                     BCM_COSQ_GPORT_REPLACE);
            
            /* e2e gport */
            BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]));        
            rv = bcm_cosq_gport_add(unit, g_dii.dest_local_gport[idx], 1, flags, 
                                    &e2e_gport);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_cosq_gport_add replace hr scheduler idx:%d failed. "
                                       "Error:%d (%s)\n"), idx, rv, bcm_errmsg(rv)));
                return rv;
            }       
        }
        
        /* Stage I: Egress Create VOQ connectors */
        for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
            for (idx = 0; idx < g_dii.num_ports; idx++) {
                for (core_id = 0; core_id < g_dii.nof_active_cores; core_id++) {
                    /* create voq connector */
                    if (g_dii.base_modid == 0) {
                        flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;
                    } else {         
                        flags = BCM_COSQ_GPORT_VOQ_CONNECTOR; 
                    }
                    port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);

                    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit,
                                              "bcm_port_get failed. Error:%d (%s)\n"),  
                                   rv, bcm_errmsg(rv)));
                        return rv;
                    }

                    if (g_dii.base_modid == 0) {
                        voq_connector_id = appl_dpp_convert_modport_to_base_voq_connector(unit, port, core_id, modid_idx * MAX_MODIDS_PER_DEVICE);
                        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], voq_connector_id, mapping_info.core);
                    }

                    config.flags = flags;
                    config.nof_remote_cores = MODIDS_PER_DEVICE;
                    config.numq = g_dii.num_cos;
                    config.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid;
                    BCM_COSQ_GPORT_E2E_PORT_SET(config.port, port);    

                    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx]);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit,
                                              "bcm_cosq_gport_add connector idx:%d failed. "
                                               "Error %d (%s)\n"), idx, rv, bcm_errmsg(rv)));
                        return rv;
                    }
            
                    for (cos = 0; cos < g_dii.num_cos; cos++) {
                        /* Each VOQ connector attach suitable HR */
                        rv = bcm_cosq_gport_sched_set(unit, 
                                     g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], cos, 
                                     BCM_COSQ_SP0,0);
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_APPL_COMMON,
                                      (BSL_META_U(unit,
                                                  "bcm_cosq_gport_sched_set connector idx:%d cos:%d "
                                                   "failed. Error:%d(%s)\n"), 
                                       idx, cos, rv, bcm_errmsg(rv)));
                            return rv;
                        }

                        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, g_dii.dest_local_gport[idx]);
                        gport_info.cosq = (g_dii.dest_local_gport_nof_priorities[idx] == 8 ? cos : 0);
                        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);     
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_APPL_COMMON,
                                      (BSL_META_U(unit,
                                                  "bcm_cosq_gport_handle_get failed %d. "
                                                   "Error:%d (%s)\n"), gport_info.in_gport, rv, bcm_errmsg(rv)));
                            return rv;
                        }
                          
                        /* attach HR SPi to connecter */

                        rv = bcm_cosq_gport_attach(unit, gport_info.out_gport,
                                         g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], cos);
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_APPL_COMMON,
                                      (BSL_META_U(unit,
                                                  "bcm_cosq_gport_attach fq scheduler-connector idx:%d "
                                                   "cos:%d failed. Error:%d (%s)\n"), 
                                       idx, cos, rv, bcm_errmsg(rv)));
                            return rv;
                        }

                        /* Set Slow Rate to be bcmCosqControlFlowSlowRate1 */
                        rv = bcm_cosq_control_set(unit, g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], cos, bcmCosqControlFlowSlowRate, 1);
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_APPL_COMMON,
                                      (BSL_META_U(unit,
                                                  "bcm_cosq_control_set bcmCosqControlFlowSlowRate connector idx:%d "
                                                   "cos:%d failed. Error:%d (%s)\n"), 
                                       idx, cos, rv, bcm_errmsg(rv)));
                            return rv;
                        }
                    }

                    if (g_dii.is_symmetric) {
                        int nof_active_cores = g_dii.nof_active_cores;
                        for (core_id++; core_id < nof_active_cores; core_id++) {
                            g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx] = g_dii.gport_ucast_voq_connector_group[modid_idx][0][idx];
                        }
                    }
                }
            }
        }
    }

    /* Define credit request profiles */
    rv = appl_dpp_cosq_diag_credit_request_thresholds_define(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "Failed to define credit request profiles. Error:%d (%s) \n"), 
                   rv, bcm_errmsg(rv)));
       return rv;
    }

    /* Stage 2: Ingress: Create VOQs for each system port. */
    for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
        for (idx = 0; idx < g_dii.num_ports; idx++) {

            port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);
            if (BCM_FAILURE(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info))) {
                LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Invalid local port %d\n"), port));
                return BCM_E_PARAM;
            }

            BCM_GPORT_MODPORT_SET(modport_gport, modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid + mapping_info.core, mapping_info.tm_port);

            rv = bcm_stk_gport_sysport_get(unit,modport_gport,&sysport_gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_stk_gport_sysport_get get sys port failed. Error:%d (%s) \n"), 
                           rv, bcm_errmsg(rv)));
               return rv;
            }


            sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
            voq_base_id = appl_dpp_convert_sysport_id_to_base_voq_id(unit, sysport_id);
                       
             
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(g_dii.gport_ucast_queue_group[modid_idx][idx],voq_base_id);            

            /* Map sysport_gport to voq_base_id. Could have used sysport_gport instead of modport_gport in the function call below */
            flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
            
            /* Configure credit request profile for the port */
            rv = appl_dpp_cosq_diag_credit_request_thresholds_config(unit, flags, port, modport_gport, &interface_info, &ingress_config);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "Failed to configure credit request profile for port(%d). Error:%d (%s) \n"), 
                           port, rv, bcm_errmsg(rv)));
               return rv;
            }

            /* Map the port with the credit request profile */
            rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &ingress_config, &g_dii.gport_ucast_queue_group[modid_idx][idx]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_cosq_ingress_queue_bundle_gport_add UC queue failed. Error:%d (%s) \n"), 
                           rv, bcm_errmsg(rv)));
               return rv;
            }

            LOG_VERBOSE(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "ucast gport(0x%08x)\n"),
                         g_dii.gport_ucast_queue_group[modid_idx][idx]));

            is_pdm_wide_mode = soc_property_get(unit, "bcm886xx_pdm_mode", 0);
            is_hdr_comp_tag_exists = soc_property_get(unit, "ingress_congestion_management_pkt_header_compensation_enable", 0);

            /* Ingress compensation configuration */
            rv = bcm_petra_stk_domain_modid_get(unit, modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid, &domain);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_petra_stk_domain_modid_get failed. "
                                   "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
                return rv;
            }
            
            if(domain == my_domain) {
                if ((SOC_IS_QAX(unit) && is_hdr_comp_tag_exists) || (is_pdm_wide_mode && is_hdr_comp_tag_exists)) /* new mechanism */
                {
                    int comp_value;
                    /* Constant compensation value should be 20 bytes for Preamble and IPG + 4 bytes for CRC = 24 bytes
                       In devices before QAX, we decrease 2 bytes for internal CRC (20+4-2 = 22).
                       In QAX, these 2 bytes are encountered as  part of Header Delta compensation */
                    comp_value = (SOC_IS_QAX(unit) ? 24 :22);
                    for (tc_i = 0; tc_i < g_dii.num_cos; tc_i++) { /* set compensation at ingress to +22 */
                        adjust_info.cosq = tc_i;
                        adjust_info.flags = 0;
                        adjust_info.gport = g_dii.gport_ucast_queue_group[modid_idx][idx];
                        adjust_info.source_info.source_id = 0; /* irrelevant for scheduler */
                        adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceScheduler; /* scheduler type */
                        rv = bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, comp_value);
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_APPL_COMMON,
                                      (BSL_META_U(unit,
                                                  "bcm_cosq_gport_pkt_size_adjust_set failed. "
                                                  "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
                            return rv;
                        }
                    }
                }
                else /* Old mechanism */
                {
                    for (tc_i = 0; tc_i < g_dii.num_cos; tc_i++) { /* set compensation at ingress to +2 */
                        rv = bcm_cosq_control_set(unit, g_dii.gport_ucast_queue_group[modid_idx][idx], tc_i, bcmCosqControlPacketLengthAdjust, 2);
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_APPL_COMMON,
                                      (BSL_META_U(unit,
                                                  "bcm_cosq_control_set failed in setting compensation. Error:%d (%s) \n"), 
                                       rv, bcm_errmsg(rv)));
                           return rv;
                        }
                    }
                }
            }

        }
    }
    DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "after Creating VOQs per port");
    {

        /* Stage 3: Connect Ingress VOQs <=> Egress VOQ connectors. */
        /* Ingress: connect voqs to voq connectors */
        for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
            for (idx = 0; idx < g_dii.num_ports; idx++) {
                for (core_id = (g_dii.is_symmetric ? BCM_CORE_ALL : 0);
                     core_id < (g_dii.is_symmetric ? BCM_CORE_ALL + 1: g_dii.nof_active_cores);
                     core_id++){
                    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
                    connection.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid;
                    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(
                       connection.voq, core_id, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(g_dii.gport_ucast_queue_group[modid_idx][idx]));

                    port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);

                    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit,
                                              "bcm_port_get failed. Error:%d (%s)\n"),  
                                   rv, bcm_errmsg(rv)));
                        return rv;
                    }

                    if (g_dii.base_modid == 0) {
                        voq_connector_id = appl_dpp_convert_modport_to_base_voq_connector(unit, port, core_id, g_dii.my_modid);
                        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(voq_connector_gport, voq_connector_id, mapping_info.core);

                        connection.voq_connector = voq_connector_gport;
                    } else {                       
                        connection.voq_connector = g_dii.gport_ucast_voq_connector_group[g_dii.my_modid_idx][CORE_ID2INDEX(core_id)][idx];
                    }

                    rv = bcm_cosq_gport_connection_set(unit, &connection);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit,
                                              "bcm_cosq_gport_connection_set ingress modid_idx: %d, idx:%d failed. "
                                               "Error:%d (%s)\n"), modid_idx, idx, rv, bcm_errmsg(rv)));
                        return rv;
                    }
                }
            }
        }            
        DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "after connecting VOQs to connectors in ingress");

        /* Egress: connect voq connectors to voqs */
        for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
            for (idx = 0; idx < g_dii.num_ports; idx++) {
                connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
                connection.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid;
                port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);

                if (BCM_FAILURE(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info))) {
                    LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Invalid local port %d\n"), port));
                    return BCM_E_PARAM;
                }

                if (port == BCM_DPP_PORT_INTERNAL_ERP(mapping_info.core)) {mapping_info.tm_port = SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID;} 
                BCM_GPORT_MODPORT_SET(modport_gport, g_dii.my_modid + mapping_info.core, mapping_info.tm_port);
                rv = bcm_stk_gport_sysport_get(unit,modport_gport,&sysport_gport);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit,
                                          "bcm_stk_gport_sysport_get get sys port failed. Error:%d (%s) \n"), 
                               rv, bcm_errmsg(rv)));
                   return rv;
                }

                sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);

                for (core_id = (g_dii.is_symmetric ? BCM_CORE_ALL : 0);
                     core_id < (g_dii.is_symmetric ? BCM_CORE_ALL + 1 : g_dii.nof_active_cores);
                     core_id++) {
                    connection.voq_connector = g_dii.gport_ucast_voq_connector_group[modid_idx][CORE_ID2INDEX(core_id)][idx];

                    voq_base_id = appl_dpp_convert_sysport_id_to_base_voq_id(unit, sysport_id);
                    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_base_gport, core_id, voq_base_id);

                    connection.voq = voq_base_gport;
                    
                    rv = bcm_cosq_gport_connection_set(unit, &connection);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit,
                                              "bcm_cosq_gport_connection_set egress modid_idx: %d, idx:%d failed. "
                                               "Error:%d (%s)\n"), modid_idx, idx, rv, bcm_errmsg(rv)));
                        return rv;
                    }
                }
            }
        }
        DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "after connecting VOQs to connectors in egress");

        if (g_dii.num_cos == 4) {
            to_cosq = 3;
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "appl_dpp_cosq_diag_init: num_cos=4. Mapping TCs 4-7 to 3\n")));
            for (tc_i = 4; tc_i < 8; ++tc_i) {
                rv = bcm_cosq_mapping_set(unit, tc_i, to_cosq);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit,
                                          "bcm_cosq_mapping_set tc: %d, cosq:%d failed. "
                                           "Error:%d (%s)\n"), tc_i, to_cosq, rv, bcm_errmsg(rv)));
                    return rv;
                }
            }
        }

        /*Configure egress compensation to -2*/
        rv = bcm_port_config_get(unit, &port_config);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_port_config_get failed")));
            return rv;
        }

        /*
         * Init egress compensation.
         */

        /* Internal ports are not part of the pbmp */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_OR(pbmp, port_config.nif);
        BCM_PBMP_OR(pbmp, port_config.cpu);
        BCM_PBMP_OR(pbmp, port_config.rcy);
        BCM_PBMP_ITER(pbmp, egress_port) {
            BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(egress_q_gport, egress_port);
            rv = bcm_cosq_control_set(unit, egress_q_gport, 0, bcmCosqControlPacketLengthAdjust, -2);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                  "bcm_cosq_control_set failed")));
                return rv;
            }
        }
    }
    return rv;
}

/* Open a multicast group that contains all the OLP port from all the devices in the fabric */
int open_ingress_mc_group_for_olp(int unit, bcm_multicast_t *mc_group_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t olp_port, sys_port;
    bcm_gport_t olp[BCM_PIPES_MAX];
    int flags;
    int i = 0;
    int count;
    int modid_idx;
    int num_of_ports = 0;
    int cud = 0;
    bcm_gport_t gports[20] = {0}; /* Asume that the maximum number of devices is up to 20 */

    if (!mc_group_id) {
        return BCM_E_PARAM;
    }

    *mc_group_id = (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) - 1;

    flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;

    rv = bcm_multicast_create(unit, flags, mc_group_id);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
              (BSL_META_U(unit,
                          "Error, in mc create. "
                           "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /* Get the OLP's port number */
    rv = bcm_petra_port_internal_get(unit,BCM_PORT_INTERNAL_OLP,BCM_PIPES_MAX,olp,&count);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
              (BSL_META_U(unit,
                          "bcm_petra_port_internal_get failed. "
                           "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }
    olp_port = olp[OLP_DEFAULT_CORE];

    /* Get all the OLP ports */
    for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
        /* Add the mod ID prefix */
        sys_port = appl_dpp_convert_modport_to_sysport_id(unit, modid_idx, BCM_GPORT_LOCAL_GET(olp_port));
        BCM_GPORT_SYSTEM_PORT_ID_SET(gports[modid_idx], sys_port);
        num_of_ports++;
    }

    /* Add the ports to the multicast group */
    for(i=0; i<num_of_ports ; i++) {
        rv = bcm_multicast_ingress_add(unit, *mc_group_id, gports[i], cud);
        if (rv != BCM_E_NONE) {
            cli_out("Error, in mc ingress add, mc_group=%d, gport=0x%x \n", *mc_group_id, gports[i]);
            return rv;
        }
    }

    return rv;
}

/* L2 event callback */
void l2_entry_event_handle(int unit,
         bcm_l2_addr_t *l2addr,
         int operation,
         void *userdata)
{
    int rv = BCM_E_NONE;

    if (operation == BCM_L2_CALLBACK_LEARN_EVENT) {
        rv = bcm_l2_addr_add(unit, l2addr);
    } else if (operation == BCM_L2_CALLBACK_AGE_EVENT) {
        rv = bcm_l2_addr_delete(unit, l2addr->mac, l2addr->vid);
    }
    if (BCM_FAILURE(rv)) {
        cli_out("L2 event handling failed");
    }
}

STATIC int appl_l2_init(int unit, int l2mode)
{
    int rv = BCM_E_NONE;
    int mode = 0;
    int cf_eg_ind;
    bcm_l2_learn_msgs_config_t learn_msgs;  
    bcm_multicast_t mc_group_id;

    uint8 src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
    uint8 dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

 
    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING | BCM_L2_LEARN_MSG_SHADOW;
 
    if (MC_GROUPS_SUPPORTED(unit))
    {
        learn_msgs.flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST;
        /* Create a mc group with all the OLP ports */
        rv = open_ingress_mc_group_for_olp(unit, &mc_group_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "open_ingress_mc_group_for_olp failed. "
                               "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }

        /* Set the destination of the queue to be multicast group with all the OLP ports in the fabric */
        learn_msgs.dest_group = mc_group_id;
    }

    else
    {
        /* MC_GROUPS_SUPPORTED not defined*/
        learn_msgs.dest_port = 240;
    }

 
    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    sal_memcpy(learn_msgs.src_mac_addr, src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, dest_mac_address, 6);
 
    rv = bcm_petra_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
              (BSL_META_U(unit,
                          "bcm_petra_l2_learn_msgs_config_set failed. "
                           "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /* Configure L2 mode */
    switch(l2mode) {
    case 1 :
        mode = BCM_L2_INGRESS_DIST;
        break;
    case 2 :
        mode = BCM_L2_INGRESS_CENT;
        break;
    case 3 :
        mode = BCM_L2_EGRESS_DIST;
        break;
    case 4 :
        mode = BCM_L2_EGRESS_CENT;
        break;
    case 5 :
        mode = BCM_L2_EGRESS_INDEPENDENT;
        break;
    case 6 :
        mode = (BCM_L2_INGRESS_CENT | BCM_L2_LEARN_CPU);
        bcm_l2_addr_register(unit, l2_entry_event_handle, NULL);
        break;
    case 7 :
        mode = (BCM_L2_EGRESS_CENT | BCM_L2_LEARN_CPU);
        bcm_l2_addr_register(unit, l2_entry_event_handle, NULL);
        break;
    }
    /* Give higher priority to custom_feature_egress_independent soc property*/
    cf_eg_ind = soc_property_get(unit, "custom_feature_egress_independent_default_mode", 0);
    if (mode && !cf_eg_ind) {
        rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, mode); 
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_switch_control_set failed. "
                               "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    return rv;
}

STATIC int
appl_dpp_tpid_diag_init(int unit)
{
    int rv = BCM_E_NONE;
    bcm_port_config_t port_config;
    bcm_gport_t gport;
    bcm_port_tpid_class_t
        port_tpid_class;
    int port;

    int parse_advance_mode;
    parse_advance_mode = SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED((unit));

    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_port_config_get failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    BCM_PBMP_ITER(port_config.e, port) { 
        BCM_GPORT_LOCAL_SET(gport,port);

        if (!parse_advance_mode) {
            rv = bcm_port_tpid_set(unit, gport, 0x8100);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_port_tpid_set on port:%d failed. Error:%d (%s)\n"), 
                           port, rv, bcm_errmsg(rv)));
                return rv;
            }
            
            rv = bcm_port_discard_set(unit, gport, BCM_PORT_DISCARD_NONE);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_port_discard_set on port:%d failed. Error:%d (%s)\n"), 
                           port, rv, bcm_errmsg(rv)));
                return rv;
            }
        }
        else{ /* advance omde */
            rv = bcm_port_tpid_add(unit, gport, 0x8100,0);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_port_tpid_add on port:%d failed. Error:%d (%s)\n"), 
                           port, rv, bcm_errmsg(rv)));
                return rv;
            }

            bcm_port_tpid_class_t_init(&port_tpid_class);
            port_tpid_class.port = gport;
            port_tpid_class.tpid1 = 0x8100;
            port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
            port_tpid_class.tag_format_class_id = 2;
            port_tpid_class.vlan_translation_qos_map_id = 0;
            port_tpid_class.flags = 0;
            port_tpid_class.vlan_translation_action_id = 0;
            rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "bcm_port_tpid_class_set on port:%d failed. Error:%d (%s)\n"), 
                           port_tpid_class.port, rv, bcm_errmsg(rv)));
                return rv;
            }
        }
    }

    return rv;
}

STATIC int
appl_dpp_oam_init(int unit) {
   int rv = BCM_E_NONE;
   bcm_port_config_t port_config;
   bcm_gport_t ethernet_port, phy_port;
   uint64 arg;
   uint32 flags;
   bcm_port_interface_info_t interface_info;
   bcm_port_mapping_info_t mapping_info;

   /* Init OAM */
   rv = bcm_oam_init(unit);
   if (BCM_FAILURE(rv)) {
      LOG_ERROR(BSL_LS_APPL_COMMON,
                (BSL_META_U(unit,
                            "bcm_oam_init failed. Error:%d (%s)\n"),
                 rv, bcm_errmsg(rv)));
      return rv;
   }

   rv = bcm_port_config_get(unit, &port_config);
   if (BCM_FAILURE(rv)) {
      LOG_ERROR(BSL_LS_APPL_COMMON,
                (BSL_META_U(unit,
                            "bcm_port_config_get failed. Error:%d (%s)\n"),
                 rv, bcm_errmsg(rv)));
      return rv;
   }

   /* Enabling outbound mirroring on all ETH ports */
   BCM_PBMP_ITER(port_config.e, ethernet_port) {
      if (!BCM_PBMP_MEMBER(port_config.rcy, ethernet_port)) {
         /* Special ETH ports (stat / kbp / lb_modem) should not be set with outbound mirroring */
         if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            /* Phy ports 29 - 32 (Quad 8) are dedicated to statistics ports in ARAD */
            uint8 is_stat_port = 0;

            rv = bcm_port_get(unit, ethernet_port, &flags, &interface_info, &mapping_info);
            if (BCM_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit,
                                     "bcm_port_get failed. Error:%d (%s)\n"),
                          rv, bcm_errmsg(rv)));
               return rv;
            }

            BCM_PBMP_ITER(interface_info.phy_pbmp, phy_port) {
                if (phy_port >= 29 && phy_port <= 32) {
                    is_stat_port++;
                }
            }

            if (is_stat_port) {
               /* Skip if port is stat port */
               continue;
            }
         }

         if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
             rv = soc_port_sw_db_flags_get(unit, ethernet_port, &flags);
             if (BCM_FAILURE(rv)) {
                 LOG_ERROR(BSL_LS_APPL_COMMON,
                           (BSL_META_U(unit,
                                       "soc_port_sw_db_flags_get failed on port(%d). Error:%d (%s)\n"),
                           ethernet_port, rv, bcm_errmsg(rv)));
                 return rv;
             }

             if (SOC_PORT_IS_LB_MODEM(flags)) {
                 continue;
             }
         }

         rv = bcm_mirror_port_dest_add(unit, ethernet_port, BCM_MIRROR_PORT_EGRESS_ACL, 0);
         if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_mirror_port_dest_add failed. Error:%d (%s)\n"),
                       rv, bcm_errmsg(rv)));
            return rv;
         }
      }
   }

   if (SOC_IS_JERICHO(unit)) {
      /* Configure range to allow not pcp counter_base:
         Assuming pcp is not used we set all range for not-pcp. OAM_COUNTER_MIN and OAM_COUNTER_MAX are 15 bit registers
         So the range should be 0 - 0x7fff */
      COMPILER_64_ZERO(arg);
      rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
      COMPILER_64_SET(arg, 0, ((SOC_DPP_DEFS_GET(unit, nof_counters) - 1) & 0x7fff));
      rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
      BCM_IF_ERROR_RETURN(rv);
   }
   return rv;
}

STATIC int
appl_dpp_bfd_init(int unit)
{
    int rv = BCM_E_NONE;

    /* Init BFD */
    rv = bcm_bfd_init(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_bfd_init failed. Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    return rv;
}

/* static function: appl_dpp_itmh_prog_mode_diag_init
* This function sets all required HW configuration for ITMH processing when working in ITMH programmable mode.
* The function configures field database of direct extraction mode in order to extract all itmh information form the header. 
* The header information is used to perform actions (such as mirror, snoop command, change traffic class, drop presedence) 
* In adding the new header profile is set according to the type (unicast/mulicast). 
* The ITMH base and extension (if exist) are also removed at the end of processing. 
*/

STATIC int
appl_dpp_itmh_prog_mode_diag_init(int unit)
{
    int rv = BCM_E_NONE;
    /* coverity[stack_use_overflow:FALSE] */
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_extraction_action_t ext_action;
    bcm_field_extraction_field_t ext_field;
    bcm_field_extraction_field_t ext_field2[2];
    bcm_field_data_qualifier_t data_qual;
    int qual_id[7] = {58,59,60,61,62,63,57};
    bcm_field_group_t entry_groups[15]={122,123,122,121,127,122,122,124,124,124,124,124,125,126,126};
    bcm_field_group_t entry_groups_stat_en[16]={122,123,122,121,127,122,122,124,124,124,124,124,125,126,126,126};
    uint32 entry_ndx, nof_itmh_entries=15;
    uint32 itmh_length = 32;
    uint8 q_data, q_mask;
    int presel_id;
    int presel_flags = 0;
    bcm_field_presel_set_t psset;
    int stat_en;
    uint32
        udh2_size,
        injection_with_user_header = 0;

    udh2_size = soc_property_port_get(unit, 1, spn_FIELD_CLASS_ID_SIZE, 0);
    udh2_size += soc_property_port_get(unit, 3, spn_FIELD_CLASS_ID_SIZE, 0);
    udh2_size /= 8; /* convert to bytes */

    if (udh2_size > 2) {
        injection_with_user_header = 
            soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0);

        if (injection_with_user_header == 0) {
            /* In ITMH programmable mode, outlif/MC_ID in ITMH base extension are direct extracted to UDH2 using PMF, 
            * and IPT stamps outlif from UDH2. Refer to IHB_HEADER_PROFILE.OUT_LIF_IN_USER_HEADER_2.
            * If UDH2 size is larger than 2 Bytes, then PMF could not extract outlif/MC ID from ITMH base extension.
            * Thus UDH2 needs to be injected by customer application, in which case the following SOC property 
            * should be set:
            *     custom_feature_injection_with_user_header_enable
            */
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "custom_feature_injection_with_user_header_enable needs to be set "
                                  "when UDH2 size is larger than 2 Bytes in ITMH programmable mode\n")));
            return BCM_E_INIT;
        }
    }
    
    /* Check ccm statistic enable */
    stat_en = SOC_IS_JERICHO(unit) & (soc_property_get(unit, spn_OAM_STATISTICS_PER_MEP_ENABLED, FALSE) != 0);
    if(stat_en)
        nof_itmh_entries=16;
  /* 
   * Define the data qualifier : set offset as ITMH header base
   */
    bcm_field_data_qualifier_t_init(&data_qual); 
    data_qual.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_WITH_ID; 
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header; 
    data_qual.offset = 0 ; 
    data_qual.length = itmh_length; 
    data_qual.qual_id = qual_id[0];
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*Data Qualifier for In mirror command - take the 29th bit for the ITMH header*/
    bcm_field_data_qualifier_t_init(&data_qual); 
    data_qual.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES  | BCM_FIELD_DATA_QUALIFIER_WITH_ID; 
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header; 
    data_qual.offset = 5 ; 
    data_qual.length = 1; 
    data_qual.qual_id = qual_id[1];
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*Data Qualifier for ITMH extension exist  + ITMH base.type*/
    bcm_field_data_qualifier_t_init(&data_qual); 
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES  | BCM_FIELD_DATA_QUALIFIER_WITH_ID; 
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header; 
    data_qual.offset = 24 ; 
    data_qual.length = 4; 
    data_qual.qual_id = qual_id[2];
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*Data Qualifier for ITMH Base Extension.value*/
    bcm_field_data_qualifier_t_init(&data_qual); 
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES  | BCM_FIELD_DATA_QUALIFIER_WITH_ID; 
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header; 
    data_qual.offset = 35 ; 
    data_qual.length = 20; 
    data_qual.qual_id = qual_id[3];
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*Data Qualifier for ITMH Base.FWD_DEST_INFO[0:1] = ITMH Base[26:25]*/
    bcm_field_data_qualifier_t_init(&data_qual); 
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES  | BCM_FIELD_DATA_QUALIFIER_WITH_ID; 
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header; 
    data_qual.offset = 2 ; 
    data_qual.length = 2; 
    data_qual.qual_id = qual_id[4];
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*Data Qualifier for ITMH Base.FWD_DEST_INFO[2:18] = ITMH Base[24:8]*/
    bcm_field_data_qualifier_t_init(&data_qual); 
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES  | BCM_FIELD_DATA_QUALIFIER_WITH_ID; 
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header; 
    data_qual.offset = 0 ; 
    data_qual.length = 17; 
    data_qual.qual_id = qual_id[5];
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /* If ccm statistic enable take qualifier(OAM-ID) from the packet, 2 bytes after PTCH-2*/
    if(stat_en)
    {
        /*Data Qualifier for Counter-ID between PTCH and ITMH for OAM,BFD application */
        bcm_field_data_qualifier_t_init(&data_qual);
        data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_WITH_ID;
        data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
        data_qual.offset = 2 ;
        data_qual.length = 16;
        data_qual.qual_id = qual_id[6];
        rv = bcm_field_data_qualifier_create(unit, &data_qual);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                    (BSL_META_U(unit,
                            "bcm_field_data_qualifier_create failed. Error:%d (%s)\n"),
                            rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    /* Define the preselector-set */
    BCM_FIELD_PRESEL_INIT(psset);
    /* 
    * Set the preselector 
    */
    /* Create a presel entity */
    presel_id=6 ;
    if(!soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_field_presel_create_id failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }
    } else {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_field_presel_create_stage_id failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    }

    /*Ingress-PMF stage*/
    rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qualify_Stage failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /* TM forwarding */
    rv = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeTrafficManagement);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qualify_ForwardingType failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*Add preselector id t presel set*/
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /*Intialize field group*/
    bcm_field_group_config_t_init(&grp);
    grp.group = 122;

    /* 
    * Define the QSET - use data qualifier. 
    */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[0]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
    *  Define the ASET 
    */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPphPresentSet);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);

    /*  Create the Field group with type Direct Extraction */
    grp.priority = 122;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_config_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_action_set failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    grp.group = 123;

    /* 
    * Define the QSET - use data qualifier. 
    */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[1]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*  Create the Field group with type Direct Extraction */
    grp.priority = 123;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_config_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionMirrorIngress);


    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_action_set failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    grp.group = 124;

    /* 
     * Define the QSET - use data qualifier. 
     */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[2]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }


    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[3]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }


    /*  Create the Field group with type Direct Extraction */
    grp.priority = 124;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_config_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVportNew);
    if (injection_with_user_header == 0) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassSourceSet);
    }
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionFabricHeaderSet);

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_action_set failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    grp.group = 125;

    /* 
     * Define the QSET - use data qualifier. 
     */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[2]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[3]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*  Create the Field group with type Direct Extraction */
    grp.priority = 125;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_config_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionISQ);

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_action_set failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    grp.group = 126;

    /* 
     * Define the QSET - use data qualifier. 
     */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[2]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    if(stat_en)
    {
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyPtch);
        rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[6]);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                    (BSL_META_U(unit,
                            "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),
                            rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    /*  Create the Field group with type Direct Extraction */
    grp.priority = 126;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_config_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);
    if(stat_en)
    {
       if(soc_property_get(unit, spn_OAM_STATISTICS_PER_MEP_ENABLED, FALSE) == 1)
       {
           BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat0);
       } else {
           BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat1);
       }
    }

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_action_set failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    if (injection_with_user_header == 0) {
        bcm_field_group_config_t_init(&grp);
        grp.group = 127;

        /* 
         * Define the QSET - use data qualifier. 
         */
        BCM_FIELD_QSET_INIT(grp.qset);
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
        rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[4]);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }
        rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[5]);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }

        /*  Create the Field group with type Direct Extraction */
        grp.priority = 127;
        grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
        grp.mode = bcmFieldGroupModeDirectExtraction;
        grp.preselset = psset;
        rv = bcm_field_group_config_create(unit, &grp);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_field_group_config_create failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }

        BCM_FIELD_ASET_INIT(aset);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassSourceSet);


        /*  Attach the action set */
        rv = bcm_field_group_action_set(unit, grp.group, aset);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_field_group_action_set failed. Error:%d (%s)\n"),  
                       rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    bcm_field_group_config_t_init(&grp);
    grp.group = 121;

    /*
    * Define the QSET - use data qualifier.
    */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, qual_id[0]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*
    *  Define the ASET
    */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirectPort);


    /*  Create the Field group with type Direct Extraction */
    grp.priority = 121;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_config_create failed. Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_action_set failed. Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /* Create Direct Extraction entries, each for ITMH field (1:1 mapping):
    *  1. create the entry
    *  2. Construct the action:
    *  3. Install entry (HW configuration)
    */
    for (entry_ndx = 0; entry_ndx < nof_itmh_entries; entry_ndx++) {
      ent = (63 - entry_ndx) + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
      if(stat_en)
      {
          if (injection_with_user_header && entry_groups_stat_en[entry_ndx] == 127) {
              /* Group 127 not created when injection_with_user_header == 1*/
              continue;
          }
          rv = bcm_field_entry_create_id(unit, entry_groups_stat_en[entry_ndx], ent);
      } else {
          if (injection_with_user_header && entry_groups[entry_ndx] == 127) {
              /* Group 127 not created when injection_with_user_header == 1*/
              continue;
          }
          rv = bcm_field_entry_create_id(unit, entry_groups[entry_ndx], ent);
      }
      if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_entry_create failed. Error:%d (%s)\n"),  
                   rv, bcm_errmsg(rv)));
        return rv;
      }

     /*build extraction fields*/
      switch (entry_ndx) {
      case 0: /* PPH type */
          
          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[0];
          ext_field.lsb = 30;
          ext_field.bits = 2; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionPphPresentSet; /*new action*/

          break;
      case 1: /* in_mirror*/

          q_data = 0x1;
          q_mask = 0x1;
          rv = bcm_field_qualify_data(unit, ent, qual_id[1], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field.bits  = 4;
          ext_field.value = 0x0;

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionMirrorIngress;

          break;
      case 2: /* Drop Precedence  */
          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[0];
          ext_field.lsb = 27;
          ext_field.bits = 2; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionDropPrecedence;

          break;
      case 3: /* destination */
          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[0];
          ext_field.lsb = 8;
          ext_field.bits = 19; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionRedirect;

          break;
      case 4: /* multicast destination id */
          if (injection_with_user_header) {
            break;
          }

          q_data = 0x2;
          q_mask = 0x3;
          rv = bcm_field_qualify_data(unit, ent, qual_id[4], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[5];
          ext_field.lsb = 0;
          ext_field.bits = 17; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionClassSourceSet;

          break;
      case 5: /* snoop command */

          bcm_field_extraction_field_t_init(&(ext_field2[1]));
          ext_field2[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field2[1].bits  = 6;
          ext_field2[1].value = 0x3d;

          bcm_field_extraction_field_t_init(&(ext_field2[0]));
          ext_field2[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field2[0].qualifier = qual_id[0];
          ext_field2[0].bits  = 4;
          ext_field2[0].lsb   = 4;

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionSnoop;

          break;
      case 6: /* Traffic Class */
          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[0];
          ext_field.lsb = 1;
          ext_field.bits = 3; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionPrioIntNew; 

          break;
      case 7: /* ITMH_DESTINATION_INFO EXTENSION_EXISTS field =1 , type = outlif*/
          q_data = 0x8;
          q_mask = 0xf;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[3];
          ext_field.lsb = 0;
          ext_field.bits = 16; 
          if (SOC_IS_JERICHO(unit)) {
              ext_field.bits = 18; 
          }

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionVportNew;

          break;
     case 8:  /*ITMH_DESTINATION_INFO EXTENSION_EXISTS field =1 , type = outlif*/
         if (injection_with_user_header) {
           break;
         }
          q_data = 0x8;
          q_mask = 0xf;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[3];
          ext_field.lsb = 0;
          ext_field.bits = 18; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionClassSourceSet;

          break;

     case 9:  /* ITMH_DESTINATION_INFO EXTENSION_EXISTS field =1 , type = outlif*/
          q_data = 0x8;
          q_mask = 0xf;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field.bits  = 4;
          ext_field.value = 0x5;

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionFabricHeaderSet;

          break;

      case 10: /* ITMH_DESTINATION_INFO EXTENSION_EXISTS field =1 , type = multicast*/
          if (injection_with_user_header) {
            break;
          }

          q_data = 0x9;
          q_mask = 0xf;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[3];
          ext_field.lsb = 0;
          ext_field.bits = 17; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionClassSourceSet;

          break;

       case 11: /* ITMH_DESTINATION_INFO EXTENSION_EXISTS field =1 , type = multicast*/
          q_data = 0x9;
          q_mask = 0xf;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field.bits  = 4;
          ext_field.value = 0x6;

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionFabricHeaderSet;

          break;

      case 12: /* ITMH_DESTINATION_INFO EXTENSION_EXISTS field =1 , type = ISQ*/
          q_data = 0xA;
          q_mask = 0xf;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field.qualifier = qual_id[3];
          ext_field.lsb = 0;
          ext_field.bits = 17; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionISQ ;

          break;

       case 13: /* ITMH_DESTINATION_INFO EXTENSION_EXISTS - remove ITMH + extension = 7 Bytes*/
          q_data = 0x8;
          q_mask = 0x8;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&(ext_field2[0]));
          ext_field2[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field2[0].bits  = 6;
          ext_field2[0].value = 0x7;

          bcm_field_extraction_field_t_init(&(ext_field2[1]));
          ext_field2[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field2[1].bits  = 2;
          ext_field2[1].value = 0x1; /*bcmFieldStartToL2Strip*/

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionStartPacketStrip;

          break;

       case 14: /* ITMH_DESTINATION_INFO EXTENSION_EXISTS =0- remove ITMH = 4 Bytes*/
          q_data = 0x0;
          q_mask = 0x8;
          rv = bcm_field_qualify_data(unit, ent, qual_id[2], &q_data, &q_mask, 1);
          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_field_qualify_data failed. Error:%d (%s)\n"),  
                         rv, bcm_errmsg(rv)));
              return rv;
          }

          bcm_field_extraction_field_t_init(&(ext_field2[0]));
          ext_field2[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field2[0].bits  = 6;
          ext_field2[0].value = 0x4;

          bcm_field_extraction_field_t_init(&(ext_field2[1]));
          ext_field2[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field2[1].bits  = 2;
          ext_field2[1].value = 0x1; /*bcmFieldStartToL2Strip*/

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionStartPacketStrip;

          break;       
        case 15: /* ITMH_DESTINATION_INFO EXTENSION_EXISTS field =1 , type = ISQ*/
            /* case 15 related to ccm statistic feature.
             * we should never get here if ccm statistic disable.
             * if statement added just for indication tha this code related to ccm statistic.
             */
            if(stat_en)
            {
                q_data = 0x7;
                q_mask = 0x7;
                rv = bcm_field_qualify_Ptch(unit, ent, q_data, q_mask);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                            (BSL_META_U(unit,
                                    "bcm_field_qualify_Ptch failed. Error:%d (%s)\n"),  
                                    rv, bcm_errmsg(rv)));
                    return rv;
                }

                /* Valid bit for counter  and 1'st bit is tx*/
                bcm_field_extraction_field_t_init(&(ext_field2[0]));
                ext_field2[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
                ext_field2[0].bits  = 2;
                ext_field2[0].value = 0x1;

                /* OAM-ID*/
                bcm_field_extraction_field_t_init(&(ext_field2[1]));
                ext_field2[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
                ext_field2[1].qualifier = qual_id[6];
                ext_field2[1].lsb = 0;
                ext_field2[1].bits = 13;

                bcm_field_extraction_action_t_init(&ext_action);
                if(soc_property_get(unit, spn_OAM_STATISTICS_PER_MEP_ENABLED, FALSE) == 1) {
                    ext_action.action = bcmFieldActionStat0 ;
                } else {
                    ext_action.action = bcmFieldActionStat1 ;
                }
            }
          break;
      }

      ext_action.bias = 0;
      rv = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  ext_action,
                                                  (entry_ndx == 5 || entry_ndx == 13 || entry_ndx == 14 || entry_ndx == 15) ? 2 : 1 /* count */,
                                                  (entry_ndx == 5 || entry_ndx == 13 || entry_ndx == 14 || entry_ndx == 15) ? ext_field2 : &ext_field );

      if (BCM_FAILURE(rv)) {
          LOG_ERROR(BSL_LS_APPL_COMMON,
                    (BSL_META_U(unit,
                                "bcm_field_direct_extraction_action_add: failed. Error:%d (%s) \n"), 
                     rv, bcm_errmsg(rv)));
          return rv;
      }


      /* Write entry to HW */
      rv = bcm_field_entry_install(unit, ent);
      if (BCM_FAILURE(rv)) {
          LOG_ERROR(BSL_LS_APPL_COMMON,
                    (BSL_META_U(unit,
                                "bcm_field_entry_install: failed. Error:%d (%s) \n"), 
                     rv, bcm_errmsg(rv)));
          return rv;
      }
    }

    return rv;
}

/*
 * Purpose: Initialize basic components of Petra-B on GFA-BI card.
 * Note:    This is intended to configure only using BCM API.
 */
int
appl_dpp_bcm_diag_init(int unit, appl_dcmn_init_param_t* init_param)
{
    int rv = BCM_E_NONE;
    char *prop;
    int module_enable = 0x1;

    if (init_param->warmboot) {
        /* exit after rx activation if warm reboot */
        return rv;
    }


    /* misc init should be called first */
    rv = appl_dpp_misc_diag_init(unit, init_param->modid, init_param->base_modid, init_param->nof_devices);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "appl_dpp_misc_diag_init: failed. Error:%d (%s) \n"), 
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    if (!init_param->no_appl_stk) {
        DCMN_RUNTIME_DEBUG_PRINT(unit);
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init Stk Appl.\n"), unit));
        rv = appl_dpp_stk_diag_init(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "appl_dpp_stk_diag_init: failed. Error:%d (%s) \n"), 
                       rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    module_enable = soc_property_get(unit, spn_STACK_ENABLE, module_enable);
    if ((module_enable) && (init_param->appl_traffic_enable_stage == TRAFFIC_EN_STAGE_AFTER_STK)) {
        rv = bcm_stk_module_enable(unit, g_dii.my_modid, g_dii.num_ports, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "bcm_stk_module_enable failed. Error:%d (%s)\n"),
                       rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    DISPLAY_MEM;

    if (init_param->cosq_disable == 0) {
        DCMN_RUNTIME_DEBUG_PRINT(unit);
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init Cosq Appl.\n"), unit));
        rv = appl_dpp_cosq_diag_init(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "appl_dpp_cosq_diag_init: failed. Error:%d (%s) \n"),
                       rv, bcm_errmsg(rv)));
            return rv;
        }
    }

    DISPLAY_MEM;

    prop = soc_property_get_str(unit, spn_FAP_DEVICE_MODE);

    if (prop != NULL && !sal_strcmp(prop, "PP")) {
        /* Check if l2 application is enabled */
        prop = soc_property_get_str(unit, spn_DIAG_L2_DISABLE);

        if (prop == NULL || sal_strcmp(prop, "1")) {
            DCMN_RUNTIME_DEBUG_PRINT(unit);
            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,"%d: Init L2 Appl.\n"), unit));
            rv = appl_l2_init(unit, init_param->l2_mode);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit, "appl_l2_init: failed. Error:%d (%s) \n"),
                           rv, bcm_errmsg(rv)));
                return rv;
            }
        }

        /* Configure TPID only if PP is enabled */
        DCMN_RUNTIME_DEBUG_PRINT(unit);
        LOG_INFO(BSL_LS_BCM_INIT,
                (BSL_META_U(unit,
                        "%d: Init TPID Appl.\n"), unit));
        rv = appl_dpp_tpid_diag_init(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                    (BSL_META_U(unit,
                            "appl_dpp_tpid_diag_init: failed. Error:%d (%s) \n"),
                            rv, bcm_errmsg(rv)));
            return rv;
        }

        /* Check if OAM application is enabled */
        if (SOC_DPP_CONFIG(unit)->pp.oam_enable) {
            DCMN_RUNTIME_DEBUG_PRINT(unit);
            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,"%d: Init OAM Appl.\n"), unit));
            rv = appl_dpp_oam_init(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit, "appl_dpp_oam_init: failed. Error:%d (%s) \n"),
                           rv, bcm_errmsg(rv)));
                return rv;
            }
        }

        /* Check if BFD application is enabled */
        if (SOC_DPP_CONFIG(unit)->pp.bfd_enable) {
            DCMN_RUNTIME_DEBUG_PRINT(unit);
            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,"%d: Init BFD Appl.\n"), unit));
            rv = appl_dpp_bfd_init(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit, "appl_dpp_bfd_init: failed. Error:%d (%s) \n"),
                           rv, bcm_errmsg(rv)));
                return rv;
            }
        }
    }

    DISPLAY_MEM;

    if (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) && !init_param->no_itmh_prog_mode) {
        DCMN_RUNTIME_DEBUG_PRINT(unit);
        LOG_INFO(BSL_LS_BCM_INIT,
                (BSL_META_U(unit,
                        "%d: Init ITMH-programmable Appl.\n"), unit));
        rv = appl_dpp_itmh_prog_mode_diag_init(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                    (BSL_META_U(unit,
                            "appl_dpp_itmh_prog_mode_diag_init: failed. Error:%d (%s) \n"),
                            rv, bcm_errmsg(rv)));
            return rv;
        }
    }

#ifdef DCMN_RUNTIME_DEBUG
    dcmn_runtime_debug_per_device[unit].run_stage = dcmn_runtime_debug_state_running;
    DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "end of appl_dpp_bcm_diag_init");
#endif /* DCMN_RUNTIME_DEBUG */

    return rv;
}
#endif /* BCM_PETRA_SUPPORT */

#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_DFE_SUPPORT)
char cmd_dpp_fe1600_card_usage[] =
    "\n\tFE1600 card board specific commands\n\t"
    "Usages:\n\t"
    "fe1600 [OPTION] <parameters> ...\n\t"
    "\n\t"
    "OPTION can be:\n\t"
    "device_init - perform fe1600 device init, (set new PUC)."
    "board_init - perform init host board, which include rbf load and config, "
    "board struct init\n\t"
    "board_init parameters can be:\n\t"
    "\tio_agent - load only io_agent rbf\n\t"
    "utils parameters can be:\n\t"
    "\t\tsynt_set - set board syntisizers by <synt_num> <freq>.\n\t"
    "\t\t\tsynt_num: 0 = serdes 0, 1 = srdes  1, 2 = serdes 2, 3 = serdes 3, 4  = core\n\t"
    "\t\ttemp - read board temprature sensor by \"temp\".\n\t"
    "\t\tpower - read board Voltage/curret monitorby \"power\".\n\t"
    "\n\t"
    "Examples:\n\t"
    "fe1600 device_init 0xa188\n\t"
    "fe1600 utils synt_set 1 250000000\n\t"
    "fe1600 utils temp\n\t"
    "fe1600 utils power\n\t"
    "fe1600 utils fpga_burn pcp.rbf\n"
    ;

cmd_result_t
cmd_dpp_fe1600_card(int unit, args_t* a)
{
#ifndef __KERNEL__
    char      *function, *option;
    int strnlen_function;
    int strnlen_option;
    SOC_BSP_CARDS_DEFINES card_type;

    /* synt set configuration */
    char *synt, *targetfreq, *force;
    uint32 ret = 0, int_synt, int_targetfreq, int_force, nominal_freq = 0;

    /* Fpga download and burn */
    char *file_name;

#if (defined(LINUX) || defined(UNIX)) 
    int int_value = 0;
#endif

    host_board_type_from_nv(&card_type);
    if (card_type != FABRIC_CARD_FE1600 && card_type != FABRIC_CARD_FE1600_BCM88754) {
        cli_out("%s(): ERROR: Using fe1600 utility on wrong card type (0x%x) !!!\n", FUNCTION_NAME(), card_type);
        return SOC_SAND_ERR;
    }
    
    function = ARG_GET(a);
    if (! function )
    {
        return CMD_USAGE;
    }
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(function, "device_init",strnlen_function)){

        char *puc;
        uint32 int_puc;

        /* Turn off all writes to pcie */
        /* Stop counter proc */
        ret = soc_counter_stop(unit);
        if (ret < 0) {
            cli_out("%s: Error: Could not set counter mode: stop\n", FUNCTION_NAME());
            return CMD_FAIL;
        }

        /* FE1600 - Reset  */
        cli_out("%s: FE1600 - Reset\n", FUNCTION_NAME());
         cpu_i2c_write(0x40, 0xe, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x6);

         /* Configure PUC*/
        puc = ARG_GET(a);
        if (!puc) {
             cli_out("%s: Use old PUC\n", FUNCTION_NAME());
        } else {
            int_puc = sal_ctoi(puc, 0);

             cli_out("%s: Use PUC=0x%x\n", FUNCTION_NAME(), int_puc);
            cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, (int_puc &0xff));
            cpu_i2c_write(0x40, 0x3, CPU_I2C_ALEN_LONG_DLEN_LONG, ((int_puc >> 8) & 0xff));
        }

         /* FE1600 - Out of resert */
         cli_out("%s: FE1600 - Out of resert\n", FUNCTION_NAME());
         cpu_i2c_write(0x40, 0xe, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x7);

         sal_usleep(100000);

         /* bring up PCIE */
         cli_out("%s: bring up PCIE\n", FUNCTION_NAME());
#if (defined(LINUX) || defined(UNIX))
         _cpu_pci_register(0);
#endif

         cli_out("%s: Rerun dfe.soc\n", FUNCTION_NAME());

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (! sal_strncasecmp(function, "board_init",strnlen_function) ){

        option = ARG_GET(a);
        if (! option ) {
    
          cli_out("%s(): init fe1600 card\n", FUNCTION_NAME());
#if (defined(LINUX) || defined(UNIX))
          fe1600_card_init_host_board(
            card_type                          /* card_type, */
          );
#endif
        } else {
            return CMD_USAGE;
        }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (! sal_strncasecmp(function, "utils", strnlen_function) ){

#if (defined(LINUX) || defined(UNIX))
        if (fe1600_card_fpga_io_regs_init()) {
            cli_out("%s(): ERROR: fe1600_card_fpga_io_regs_init(). FAILED !!!\n", FUNCTION_NAME());
            return SOC_SAND_ERR;
        }

        if (fe1600_BOARD_SPECIFICATIONS_clear(card_type)) {
            cli_out("%s(): ERROR: fe1600_BOARD_SPECIFICATIONS_clear(). FAILED !!!\n", FUNCTION_NAME());
            return SOC_SAND_ERR;
        }
#endif

        option = ARG_GET(a);
        if (! option )
        {
            return CMD_USAGE; 
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (! sal_strncasecmp(option, "synt_set", strnlen_option) ){ 

            synt = ARG_GET(a);
            targetfreq = ARG_GET(a);
            if ((! synt ) || (! targetfreq)){
                return CMD_USAGE;
            } else {
                int_synt = sal_ctoi(synt, 0);
                int_targetfreq = sal_ctoi(targetfreq, 0);

                force = ARG_GET(a);                

                if (force) {
                    int_force = sal_ctoi(force, 0);
                } else {
                    int_force = 0;
                }
 
#if (defined(LINUX) || defined(UNIX))      
                if (!int_force) {
                    nominal_freq = fe1600_card_board_synt_nominal_freq_get(int_synt);
                }
#endif

                if ((int_force) || (int_targetfreq != nominal_freq)) {
#if (defined(LINUX) || defined(UNIX)) 
                    ret = fe1600_card_board_synt_set(int_synt, int_targetfreq, FALSE);
#endif
                    if (ret) {
                        cli_out("Error in %s(): fe1600_card_board_synt_set(). "
                                "FAILED !!!\n", FUNCTION_NAME());
                    }
                }
            }
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      } else if (! sal_strncasecmp(option, "temp", strnlen_option) ){ 
#if (defined(LINUX) || defined(UNIX)) 
            /* Set i2c mux */
            cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x80);

            /* Read Temperature */
            cpu_i2c_read(0x4c, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Ambient temperature %d (0x%x)\n", FUNCTION_NAME(), int_value, int_value);

            cpu_i2c_read(0x4c, 0x1, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): chip temperature %d (0x%x).\n", FUNCTION_NAME(), int_value, int_value);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "power", strnlen_option) ){

            char *param;
            int int_param = 0;

            param = ARG_GET(a);
            if (!param) {
                int_param = 0x1;
            } else {
                int_param = sal_ctoi(param, 0);
            }
#if (defined(LINUX) || defined(UNIX)) 
            fe1600_card_power_monitor(int_param);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "fpga_burn", strnlen_option) ){ 
            
            file_name = ARG_GET(a);
            if (! file_name ) {
                return CMD_USAGE;
            } 
            
            cli_out("%s(): Download and Burn %s\n", FUNCTION_NAME(), file_name);
#if (defined(LINUX) || defined(UNIX))
            soc_dpp_fpga_load(unit, file_name);         
#endif
        } else {
            return CMD_USAGE;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
#endif /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
}

char cmd_dpp_fe3200_card_usage[] =
    "\n\tFE3200 card board specific commands\n\t"
    "Usages:\n\t"
    "fe1600 [OPTION] <parameters> ...\n\t"
    "\n\t"
    "OPTION can be:\n\t"
    "utils parameters can be:\n\t"
    "\t\tfpga_burn - download and burn fpga by <file_name>\n\t"
    "\t\tpower - read board Voltage/curret monitorby \"power\".\n\t"
    "\t\tsynt_set - set board syntisizers by <target freq> <nominal freq>.\n\t"
    "\n\t"
    "Examples:\n\t"
    "fe3200 utils fe1600_card_io_agent.rbf\n\t"
    "fe3200 utils power\n\t"
    "fe3200 utils temp\n\t"
    "fe3200 utils synt_set 250000000 156250000\n"
    ;

cmd_result_t
cmd_dpp_fe3200_card(int unit, args_t* a)
{
#ifndef __KERNEL__
    char      *function, *option;
    int strnlen_function, strnlen_option;
    SOC_BSP_CARDS_DEFINES card_type;

    /* synt set configuration */
    char *target_freq, *nominal_freq;
    uint32 int_target_freq, int_nominal_freq;

    /* Fpga download and burn */
    char *file_name;

    host_board_type_from_nv(&card_type);
    LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "card_type=0x%x\n"), card_type));
      
    function = ARG_GET(a);
    if (! function )
    {
        return CMD_USAGE;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(function, "utils", strnlen_function) ){

        option = ARG_GET(a);
        if (! option )
        {
            return CMD_USAGE; 
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (! sal_strncasecmp(option, "fpga_burn", strnlen_option) ){ 
            
            file_name = ARG_GET(a);
            if (! file_name ) {
                return CMD_USAGE;
            } 
            
            LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Download and Burn %s\n"), file_name));

#if (defined(LINUX) || defined(UNIX))
            soc_dpp_fpga_load(unit, file_name);         
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "power", strnlen_option) ){

            char *param;
            int int_param = 0;

            param = ARG_GET(a);
            if (!param) {
                int_param = 0x1;
            } else {
                int_param = sal_ctoi(param, 0);
            }
#if (defined(LINUX) || defined(UNIX)) 
            fe3200_card_power_monitor(int_param);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        }  else if (! sal_strncasecmp(option, "temp", strnlen_option) ){ 
#if (defined(LINUX) || defined(UNIX))
            int int_value = 0;
                
            /* Set i2c mux */
            cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x80);

            /* Read Temperature */
            cpu_i2c_read(0x4c, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Ambient temperature %d (0x%x)\n", FUNCTION_NAME(), int_value, int_value);

            cpu_i2c_read(0x4c, 0x1, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): chip temperature %d (0x%x).\n", FUNCTION_NAME(), int_value, int_value);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "synt_set", strnlen_option)) { 

            target_freq = ARG_GET(a);
            nominal_freq = ARG_GET(a);
            if ((!target_freq ) || (!nominal_freq)) {
                return CMD_USAGE;
            } else {
                int_nominal_freq = sal_ctoi(nominal_freq, 0);
                int_target_freq = sal_ctoi(target_freq, 0);

#if (defined(LINUX) || defined(UNIX)) 
                fe3200_card_board_synt_set(int_target_freq, int_nominal_freq);
#endif
            }
        } else {
            return CMD_USAGE;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
#endif /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
}

#endif /* (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_DFE_SUPPORT) */

#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_PETRA_SUPPORT)

/* Time Measure functions */
cmd_result_t _appl_sbusdma_desc_measure_run(int unit, int cmc, int ch, soc_sbusdma_desc_t *sbusdma_desc, int i)
{
    int rv = CMD_OK;
    uint32 
        ctrl = 0x0, 
        status = 0x0;
    soc_timeout_t to;

    ctrl = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl, MODEf, 1);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl, ABORTf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl, STARTf, 0);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);
    
    /* write desc address */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR(cmc, ch), soc_cm_l2p(unit, sbusdma_desc));

    /* Start DMA */
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &ctrl, STARTf, 1);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch), ctrl);

    /* Read status */
    soc_timeout_init(&to, 100000, 100);
    rv = CMD_FAIL;
    do {
        status = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr, status, DONEf)) {
            rv = CMD_OK;
            if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr, status, ERRORf)) {
                LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ERROR: sbusdma desc returned with error. status=0x%x. i=%d\n"), status, i));
                rv = CMD_FAIL;
            }
            break;
        }
    } while(!(soc_timeout_check(&to)));

    if (rv != CMD_OK) {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ERROR: done bit is still cleared or error. status=0x%x, i=%d\n"), status, i));
#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
        cmicm_sbusdma_curr_op_details(unit, cmc, ch);
#endif
    }

    return rv;
}

void _appl_sbusdma_desc_measure_buff_print(int unit, uint32 *buff)
{
    LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "buff: 0x%x 0x%x 0x%x 0x%x.\n"), buff[3], buff[2], buff[1], buff[0]));
    LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "buff: 0x%x 0x%x 0x%x 0x%x.\n"), buff[8], buff[7], buff[6], buff[5]));
    LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "buff: 0x%x 0x%x 0x%x 0x%x.\n"), buff[12], buff[11], buff[10], buff[9]));
    LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "buff: 0x%x 0x%x 0x%x 0x%x.\n"), buff[16], buff[15], buff[14], buff[13]));
    LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "buff: 0x%x 0x%x 0x%x 0x%x.\n"), buff[20], buff[19], buff[18], buff[17]));
    LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "buff: 0x%x 0x%x 0x%x 0x%x.\n"), buff[24], buff[23], buff[22], buff[21]));
}
            
void _appl_sbusdma_desc_measure_cb(int unit, int status, sbusdma_desc_handle_t handle, void *data)
{
    uint32 *buff = (uint32 *)data;
    if (status == SOC_E_NONE) {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "CB: Complete: sbusdma_desc.\n")));
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "CB: buff: 0x%x 0x%x 0x%x 0x%x.\n"), buff[3], buff[2], buff[1], buff[0]));
     } else {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "CB: SBUSDMA desc failed: Handle:%d.\n"), handle));
    }

    soc_sand_ll_timer_stop(7);
}

char cmd_dpp_time_measure_usage[] =
    "\n\tPerform Time Measurement for: PCIE, Register access ...\n\t"
    "Usages:\n\t"
    "TimeMeasurement <Parameters> ...\n\t"
    "\n\t"
    "Parameters:\n\t"
    "count - Number of loops. Default: 100000."
    "DmaDescRegRead - If set enable SbusDMA single descriptor mode reg read\n"
    "DmaDescRegMultiRead - If set enable SbusDMA mulipule (accurding to CountSbusDmaDesc) descriptor mode reg read\n"
    "DmaDescRegWrite - If set enable SbusDMA single descriptor mode reg write\n"
    "DmaDescRegMultiWrite - If set enable SbusDMA mulipule (accurding to CountSbusDmaDesc) descriptor mode reg write\n"
    "DmaDescThreadRegRead - If set enable SbusDMA single descriptor mode reg read using sbusdma Thread\n"
    "CountSbusDmaDesc - Set number of descriptor to use in SbusDMA chain\n"
    "DmaDescRegMultiRead480 - If set enable mulipule (accurding to CountSbusDmaDesc) descriptor mode wide (<= 480bit) reg read\n"
    "DmaDescRegMultiWrite480 - If set enable mulipule (accurding to CountSbusDmaDesc) descriptor mode wide (<= 480bit) reg write\n"
    "DmaDescMemMultiRead480 - If set enable mulipule (accurding to CountSbusDmaDesc) descriptor mode wide (<= 480bit) memory write\n"
    "DmaDescMemMultiWrite480 - If set enable mulipule (accurding to CountSbusDmaDesc) descriptor mode wide (<= 480bit) memory write\n"
    "DmaDescFewMemMultiRead480 - If set enable mulipule (accurding to CountSbusDmaDesc) descriptor mode wide (<= 480bit) memories Read.\n"
    "\tRead 2 different memories.\n"  
    "DmaDescMemMultiReadWrite480 - If set enable mulipule (accurding to CountSbusDmaDesc) descriptor mode wide (<= 480bit) memories Read and write chain.\n"
    "\tRead memory entry and write this entry to different memory.\n"
    "\n"
    "Examples:\n\t"
    "TimeMeasurement count=100000\n"
    "timem DmaDescRegRead=1 DmaDescRegMultiRead=1 DmaDescRegWrite=1 DmaDescRegMultiWrite=1 DmaDescThreadRegRead=0 CountSbusDmaDesc=1 count=1\n"
    "timem DmaDescRegWrite=1 CountSbusDmaDesc=1 count=1 DmaDescMemMultiWrite480=1\n"
    "timem CountSbusDmaDesc=2 count=1 DmaDescFewMemMultiRead480=1\n"
    ;

#define APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX 2001

cmd_result_t cmd_dpp_time_measure(int unit, args_t *a)
{
    int i, j,
        blk = 0x0,
        count = 1,
        dma_desc_reg_read = 0,
        count_sbusdma_desc = 100,
        dma_desc_reg_write = 0,
        dma_desc_thread_reg_read = 0,
        dma_desc_reg_multi_read = 0,
        dma_desc_reg_multi_read_480 = 0,
        dma_desc_reg_multi_write = 0,
        dma_desc_reg_multi_write_480 = 0,
        dma_desc_mem_multi_write_480 = 0,
        dma_desc_mem_multi_read_480 = 0,
        dma_desc_few_mem_multi_read_480 = 0,
        dma_desc_mem_multi_read_write_480 = 0x0,
        alloc_size;
    uint32
        data = 0x1, 
        addr = 0x1103C,
        sbus_addr;
    uint8 acc = 0x0;
    parse_table_t   pt;
    cmd_result_t    rv = CMD_OK;
    ibde_dev_t *dev = (ibde_dev_t*)bde->get_dev(CMDEV(unit).dev.dev);

    sbusdma_desc_handle_t _soc_sbusdma_desc_handles[APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX];
    soc_sbusdma_desc_ctrl_t ctrl = {0};
    soc_sbusdma_desc_cfg_t cfg;
    soc_sbusdma_desc_t *sbusdma_desc;
    int cmc = 0x0,
        ch = 0x2;
    uint32 *buff;

    /* uint32 cpu_regs_addr, cpu_regs_value; */

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt, "count"                   , PQ_INT, (void *)(1)       , &count                        , NULL);
        parse_table_add(&pt, "data"                    , PQ_INT, (void *)(1)       , &data                         , NULL);
        parse_table_add(&pt, "addr"                    , PQ_INT, (void *)(0x1103C) , &addr                         , NULL);
        parse_table_add(&pt, "DmaDescRegRead"          , PQ_INT, (void *)(0)       , &dma_desc_reg_read            , NULL);
        parse_table_add(&pt, "DmaDescRegMultiRead"     , PQ_INT, (void *)(0)       , &dma_desc_reg_multi_read      , NULL);
        parse_table_add(&pt, "CountSbusDmaDesc"        , PQ_INT, (void *)(0)       , &count_sbusdma_desc           , NULL);
        parse_table_add(&pt, "DmaDescRegWrite"         , PQ_INT, (void *)(0)       , &dma_desc_reg_write           , NULL);
        parse_table_add(&pt, "DmaDescRegMultiWrite"    , PQ_INT, (void *)(0)       , &dma_desc_reg_multi_write     , NULL);
        parse_table_add(&pt, "DmaDescThreadRegRead"    , PQ_INT, (void *)(0)       , &dma_desc_thread_reg_read     , NULL);
        parse_table_add(&pt, "DmaDescRegMultiRead480"  , PQ_INT, (void *)(0)       , &dma_desc_reg_multi_read_480  , NULL);
        parse_table_add(&pt, "DmaDescRegMultiWrite480" , PQ_INT, (void *)(0)       , &dma_desc_reg_multi_write_480 , NULL);
        parse_table_add(&pt, "DmaDescMemMultiWrite480" , PQ_INT, (void *)(0)       , &dma_desc_mem_multi_write_480 , NULL);
        parse_table_add(&pt, "DmaDescMemMultiRead480"  , PQ_INT, (void *)(0)       , &dma_desc_mem_multi_read_480  , NULL);
        parse_table_add(&pt, "DmaDescFewMemMultiRead480"    , PQ_INT, (void *)(0)    , &dma_desc_few_mem_multi_read_480    , NULL);
        parse_table_add(&pt, "DmaDescMemMultiReadWrite480"  , PQ_INT, (void *)(0)    , &dma_desc_mem_multi_read_write_480  , NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }
    
    cli_out("count=0x%x,%d, data=0x%x, addr=0x%x, CountSbusDmaDesc=%d.\n", 
            count, count, data, addr, count_sbusdma_desc);

    cli_out("DmaDescRegRead=%d, DmaDescRegMultiRead=%d, DmaDescRegwrite=%d, DmaDescRegMultiWrite=%d, DmaDescThreadRegRead=%d.\n", 
            dma_desc_reg_read, dma_desc_reg_multi_read, dma_desc_reg_write, dma_desc_reg_multi_write, dma_desc_thread_reg_read);
            
    cli_out("DmaDescRegMultiRead480=%d, DmaDescRegMultiWrite480=%d, DmaDescMemMultiWrite480=%d, DmaDescMemMultiRead480=%d, DmaDescFewMemMultiRead480=%d, DmaDescMemMultiReadWrite480=%d.\n", 
            dma_desc_reg_multi_read_480, dma_desc_reg_multi_write_480, dma_desc_mem_multi_write_480, dma_desc_mem_multi_read_480, dma_desc_few_mem_multi_read_480, dma_desc_mem_multi_read_write_480);

    soc_sand_ll_timer_clear();

    soc_sand_ll_timer_set("pci read", 1);
    for(i=0 ; i<count ; i++) {
        soc_pci_read(unit, 0x1103C /*CMIC_MIIM_SCAN_PORTS_0*/);
    }
    soc_sand_ll_timer_stop(1);

    soc_sand_ll_timer_set("pci write", 2);
    for(i=0 ; i<count ; i++) {
        soc_pci_write(unit, 0x1103C /*CMIC_MIIM_SCAN_PORTS_0*/, data);
    }
    soc_sand_ll_timer_stop(2);
 
    soc_sand_ll_timer_set("addr get", 3);
    for(i=0 ; i<count ; i++) {
        soc_reg_addr_get(unit, ECI_VERSION_REGISTERr, REG_PORT_ANY /* port */, 0 /* index */, SOC_REG_ADDR_OPTION_NONE, &blk, &acc);
    }
    soc_sand_ll_timer_stop(3); 
    
    soc_sand_ll_timer_set("vbase write", 4);
    for(i=0 ; i<count ; i++) {
        ((uint32 *)(dev->base_address))[0x1103C/sizeof(uint32)] = data;
    }
    soc_sand_ll_timer_stop(4);

    soc_sand_ll_timer_set("vbase read", 5);
    for(i=0 ; i<count ; i++) {
        data = ((uint32 *)(dev->base_address))[0x1103C/sizeof(uint32)];
    }
    soc_sand_ll_timer_stop(5);

    soc_sand_ll_timer_set("vbase write + read + cmp", 6);
    for(i=0 ; i<count ; i++) {
        ((uint32 *)(dev->base_address))[0x1103C/sizeof(uint32)] = i % 32;
        data = ((uint32 *)(dev->base_address))[0x1103C/sizeof(uint32)];
        if (data != (i % 32)) {
            cli_out("%s(): ERROR: data=0x%x != (i  32)=0x%x !!!\n", FUNCTION_NAME(), data, (i % 32));
            return CMD_USAGE;
        }
    }
    soc_sand_ll_timer_stop(6);

    /* DMA desc mechanizem */
    sal_memset(_soc_sbusdma_desc_handles, 0, APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX * sizeof(sbusdma_desc_handle_t));

    /* allocate memory for host mem address - sbusdma response*/
    alloc_size = (SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * sizeof(uint32);
    if ((buff = (uint32 *)soc_cm_salloc(unit, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX, "SBUSDMA desc mode response")) == NULL) {
        cli_out("%s(): Error: Fail to allocate memory for SBUSDMA desc mode response!!!\n", FUNCTION_NAME());
        return CMD_USAGE;
    }
    sal_memset(buff, 0, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);

    sbusdma_desc = soc_cm_salloc(unit, sizeof(soc_sbusdma_desc_t) * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX, "soc_sbusdma_desc_t");
    if (sbusdma_desc == NULL) {
        cli_out("%s(): Error: Fail to allocate memory for SBUSDMA desc!!!\n", FUNCTION_NAME());
        return CMD_USAGE;
    }
    LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "buff=%p, sbusdma_desc=%p\n"), (void *)buff, (void *)sbusdma_desc));
  

    
    if (dma_desc_thread_reg_read) {
        soc_sand_ll_timer_set("dma_desc_thread_reg_read", 7);
        
        ctrl.flags = 0; /* SOC_SBUSDMA_CFG_COUNTER_IS_MEM */
        ctrl.cfg_count = 1;
        ctrl.buff = buff;
        ctrl.cb = _appl_sbusdma_desc_measure_cb;
        ctrl.data = (void *)buff;
#define _SBUSDMA_DESC_TIME "SBUSDMA_DESC_TI"
        sal_strncpy_s(ctrl.name, _SBUSDMA_DESC_TIME, sizeof(ctrl.name));
        LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ctrl: ctrl.flags=0x%x, ctrl.cfg_count=%d, ctrl.buff=%p, ctrl.data=%p, ctrl.name=%s\n"), ctrl.flags, ctrl.cfg_count, (void *)(ctrl.buff), (void *)(ctrl.data), ctrl.name));
        sbus_addr = soc_reg_addr_get(unit, ECI_VERSION_REGISTERr, REG_PORT_ANY /* port */, 0 /* index */, SOC_REG_ADDR_OPTION_NONE, &blk, &acc);
        cfg.acc_type = acc;
        cfg.blk = blk;
        cfg.addr = sbus_addr;
        cfg.width = SOC_REG_IS_64(unit, ECI_VERSION_REGISTERr) ? 2 : 1;
        cfg.count = 1;
        cfg.addr_shift = 8;
        LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "cfg: cfg.acc_type=0x%x, cfg.blk=%d, cfg.addr=0x%x, cfg.width=%d, cfg.count=%d, cfg.addr_shift=%d\n"), cfg.acc_type, cfg.blk, cfg.addr, cfg.width, cfg.count, cfg.addr_shift));
        SOC_IF_ERROR_RETURN(soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE, &_soc_sbusdma_desc_handles[0]));
        soc_sbusdma_desc_run(unit, _soc_sbusdma_desc_handles[0]);
    }

    /* Let the sbusdma desc thread the option to call the CB */
    sal_usleep(1000);
    
    if (dma_desc_reg_read) {
        soc_sand_ll_timer_set("dma_desc_reg_read", 8);
        for(i=0 ; i<count ; i++) {
            sal_memset(sbusdma_desc, 0, sizeof(soc_sbusdma_desc_t));
            sal_memset(buff, 0, alloc_size /* * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX */);

            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            sbusdma_desc->cntrl  = 0x80000000;
            sbusdma_desc->req    = 0x08000c01;
            sbusdma_desc->count  = 0x00000001;
            sbusdma_desc->opcode = 0x2c000200; /* read ECI_TEST_REGISTER, ECI_VERSION_REGISTER = 0x2c000200; */
            sbusdma_desc->addr   = 0x00000230; /* ECI_TEST_REGISTER = 0x00000230; */ /* ECI_VERSION_REGISTER = 0x00000000 */
            sbusdma_desc->hostaddr = soc_cm_l2p(unit, buff);

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
        soc_sand_ll_timer_stop(8);
    }
    
    if (dma_desc_reg_write) {
        soc_sand_ll_timer_set("dma_desc_reg_write", 9);
        for(i=0 ; i < count ; i++) {
            sal_memset(sbusdma_desc, 0, sizeof(soc_sbusdma_desc_t));
            sal_memset(buff, 0, alloc_size /* * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX */);

            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            sbusdma_desc->cntrl  = 0x80000000;
            sbusdma_desc->req    = 0x00000c20;
            sbusdma_desc->count  = 0x00000001;
            sbusdma_desc->opcode = 0x34000200; /* write ECI_TEST_REGISTER 0x34000200; */ 
            sbusdma_desc->addr   = 0x00000230; /* ECI_TEST_REGISTER = 0x00000230; */
            buff[0] = i;
            sbusdma_desc->hostaddr = soc_cm_l2p(unit, buff);

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
        soc_sand_ll_timer_stop(9);
    }
    

    sal_memset(sbusdma_desc, 0, sizeof(soc_sbusdma_desc_t) * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    sal_memset(buff, 0, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    
    if (dma_desc_reg_multi_read) {
        soc_sand_ll_timer_set("dma_desc_reg_multi_read", 10);

        for(i=0 ; i<count ; i++) {

            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j++) {
                sbusdma_desc[j].cntrl  = SOC_SBUSDMA_CTRL_APND;
                sbusdma_desc[j].req    = 0x08000c01;
                sbusdma_desc[j].count  = 0x00000001;
                sbusdma_desc[j].opcode = /*0x2f100200;*/ 0x2c000200;
                sbusdma_desc[j].addr   = /*0x00000179;*/ 0x00000000;
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
        soc_sand_ll_timer_stop(10);
    }
    

    sal_memset(sbusdma_desc, 0, sizeof(soc_sbusdma_desc_t) * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    sal_memset(buff, 0, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);

    
    if (dma_desc_reg_multi_write) {
        soc_sand_ll_timer_set("dma_desc_reg_multi_write", 11);

        for(i=0 ; i<count ; i++) {
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j++) {
                sbusdma_desc[j].cntrl  = SOC_SBUSDMA_CTRL_APND;
                sbusdma_desc[j].req    = 0x00000c20;
                sbusdma_desc[j].count  = 0x00000001;
                sbusdma_desc[j].opcode = 0x34000200; /* write ECI_TEST_REGISTER 0x34000200; */
                sbusdma_desc[j].addr   = 0x00000230; /* ECI_TEST_REGISTER = 0x00000230; */
                buff[0] = j;
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
        soc_sand_ll_timer_stop(11);
    }
    

    sal_memset(sbusdma_desc, 0, sizeof(soc_sbusdma_desc_t) * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    sal_memset(buff, 0, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);

   
    if (dma_desc_reg_multi_read_480) {
        soc_sand_ll_timer_set("dma_desc_reg_multi_read_480", 12);

        for(i=0 ; i<count ; i++) {

            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j++) {
                sbusdma_desc[j].cntrl  = SOC_SBUSDMA_CTRL_APND;
                sbusdma_desc[j].req    = 0x08000c01;
                sbusdma_desc[j].count  = 0x00000001;
                /* Jer: d IHB_FIFO_PIPE_40_TO_50 0 1 => opcode 0x1de00200 */
                /* g OAMP_ICC_MAP_REG_1 opcode 0x2f801e00*/
                sbusdma_desc[j].opcode = 0x2f801e00 ; 
                sbusdma_desc[j].addr   = 0x00000144; /* OAMP_ICC_MAP_REG_1 address 0x00000144 */
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
        soc_sand_ll_timer_stop(12);
    }
    

    sal_memset(sbusdma_desc, 0, sizeof(soc_sbusdma_desc_t) * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    sal_memset(buff, 0, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    for(i=0 ; i< (alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX / sizeof(uint32)) ; i++) {
        buff[i] = i;
    }

    
    if (dma_desc_reg_multi_write_480) {
        soc_sand_ll_timer_set("dma_desc_reg_multi_write_480", 13);

        for(i=0 ; i<count ; i++) {
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j++) {
                /* sbusdma_desc[j].cntrl  = SOC_SBUSDMA_CTRL_APND; */
                sbusdma_desc[j].cntrl  = 0x0;
                /* req_words, bit 5:9 = 15 = 0xf = 480bit/32 0x00000de0 */
                /* req_words, bit 5:9 = 17 = 0x11 = 0x00000e20 */
                sbusdma_desc[j].req    = 0x00000de0; 
                sbusdma_desc[j].count  = 0x00000001;
                /* Jer: w IHB_FIFO_PIPE_40_TO_50 0 1 => opcode 0x27882200 */ 
                /* Arad: s OAMP_ICC_MAP_REG_1 opcode 0x37801e00*/
                /* Jer: s OAMP_ICC_MAP_REG_1 opcode 0x35c01e00 */ 
                sbusdma_desc[j].opcode = 0x35c01e00; 
                /* Jer: IHB_FIFO_PIPE_40_TO_50 address 0x00b00000 */ 
                /* Jer: OAMP_ICC_MAP_REG_1 address 0x0000016b */
                /* Arad: OAMP_ICC_MAP_REG_1 address 0x00000144 */
                /* sbusdma_desc[j].addr   = 0x00b00000 + (j % 28); */
                sbusdma_desc[j].addr   = 0x0000016b;
                /*sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);*/
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * j]);
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
        soc_sand_ll_timer_stop(13);
    }
    

    sal_memset(sbusdma_desc, 0, sizeof(soc_sbusdma_desc_t) * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    sal_memset(buff, 0, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    for(i=0 ; i< (alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX / sizeof(uint32)) ; i++) {
        buff[i] = i;
    }

    WRITE_IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 1);
    
    if (dma_desc_mem_multi_write_480) {
        soc_sand_ll_timer_set("dma_desc_mem_multi_write_480", 14);

        for(i=0 ; i<count ; i++) {
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j++) {
                /* sbusdma_desc[j].cntrl  = SOC_SBUSDMA_CTRL_APND; */
                sbusdma_desc[j].cntrl  = 0x0;
                /* req_words, bit 5:9 = 15 = 0xf = 480bit/32 0x00000de0 */
                /* req_words, bit 5:9 = 17 = 0x11 = 0x00000e20 */
                sbusdma_desc[j].req    = 0x00000e20; 
                sbusdma_desc[j].count  = 0x00000001;
                /* Jer: w IHB_FIFO_PIPE_40_TO_50 0 1 => opcode 0x27882200 */ 
                /* Arad: s OAMP_ICC_MAP_REG_1 opcode 0x37801e00*/
                /* Jer: s OAMP_ICC_MAP_REG_1 opcode 0x35c01e00 */ 
                sbusdma_desc[j].opcode = 0x27882200; 
                /* Jer: IHB_FIFO_PIPE_40_TO_50 address 0x00b00000 */ 
                /* Jer: OAMP_ICC_MAP_REG_1 address 0x0000016b */
                /* Arad: OAMP_ICC_MAP_REG_1 address 0x00000144 */
                /* sbusdma_desc[j].addr   = 0x00b00000 + (j % 28); */
                sbusdma_desc[j].addr   = 0x00b00000;
                /*sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);*/
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * j]);
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc,  i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
        soc_sand_ll_timer_stop(14);
    }
    
    
    if (dma_desc_mem_multi_read_480) {
        soc_sand_ll_timer_set("dma_desc_mem_multi_read_480", 15);
        for(i=0 ; i<count ; i++) {
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j++) {
                /* sbusdma_desc[j].cntrl  = SOC_SBUSDMA_CTRL_APND; */
                sbusdma_desc[j].cntrl  = 0x0;
                /* req_words, bit 5:9 = 15 = 0xf = 480bit/32 0x00000de0 */
                /* rep_words, bit 4:0 = 17 = 0x11 = 0x08000C11 */
                sbusdma_desc[j].req    = 0x08000c11; 
                sbusdma_desc[j].count  = 0x00000001;
                /* Jer: d IHB_FIFO_PIPE_40_TO_50 0 1 => opcode 0x1de00200 */ 
                sbusdma_desc[j].opcode = 0x1de00200; 
                /* Jer: IHB_FIFO_PIPE_40_TO_50 address 0x00b00000 */ 
                sbusdma_desc[j].addr   = 0x00b00000;
                /*sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);*/
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * j]);
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */
        }
    }
    soc_sand_ll_timer_stop(15);
    
    sal_memset(buff, 0, alloc_size * APPL_DCMN_INIT_TIME_MEASURE_SBUSDMA_DESC_NUM_MAX);
    
    if (dma_desc_few_mem_multi_read_480) {
        soc_sand_ll_timer_set("dma_desc_few_mem_multi_read_480", 16);

        for(i=0 ; i<count ; i++) {
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j += 2) {
                /* sbusdma_desc[j].cntrl  = SOC_SBUSDMA_CTRL_APND; */
                sbusdma_desc[j].cntrl  = 0x0;
                /* rep_words, bit 4:0 = 17 = 0x11 = 0x08000C11 */
                sbusdma_desc[j].req    = 0x08000c11; 
                sbusdma_desc[j].count  = 0x00000001;
                /* Jer: d disable_cache IHP_STP_TABLE 0 1 => opcode 0x1dd00200 */ 
                sbusdma_desc[j].opcode = 0x1dd00200; 
                /* Jer: IHP_STP_TABLE address 0x003a0000 */ 
                sbusdma_desc[j].addr   = 0x003a0000;
                /*sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);*/
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * j]);
                
                sbusdma_desc[j+1].cntrl  = 0x0;
                /* rep_words, bit 4:0 = 17 = 0x11 = 0x08000C11 */
                sbusdma_desc[j+1].req    = 0x08000c11; 
                sbusdma_desc[j+1].count  = 0x00000001;
                /* Jer: d IHB_FIFO_PIPE_40_TO_50 0 1 => opcode 0x1de00200 */ 
                sbusdma_desc[j+1].opcode = 0x1de00200; 
                /* Jer: IHB_FIFO_PIPE_40_TO_50 address 0x00b00000 */ 
                sbusdma_desc[j+1].addr   = 0x00b00000;
                /*sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, buff);*/
                sbusdma_desc[j+1].hostaddr = soc_cm_l2p(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * (j+1)]);              
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * 0]); 
            _appl_sbusdma_desc_measure_buff_print(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * 1]); */
        }
        soc_sand_ll_timer_stop(16);
    }
    
    if (dma_desc_mem_multi_read_write_480) {
        soc_sand_ll_timer_set("dma_desc_mem_multi_read_write_480", 17);

        for(i=0 ; i<count ; i++) {
            /* _appl_sbusdma_desc_measure_buff_print(unit, buff); */

            for (j=0; j < count_sbusdma_desc; j += 2) {
                /* Read from "IHP_STP_TABLE 0 1" */
                sbusdma_desc[j].cntrl  = 0x0;
                /* rep_words, bit 4:0 = 17 = 0x11 = 0x08000C11 */
                sbusdma_desc[j].req    = 0x08000c11; 
                sbusdma_desc[j].count  = 0x00000001;
                /* Jer: d disable_cache IHP_STP_TABLE 0 1 => opcode 0x1dd00200 */ 
                sbusdma_desc[j].opcode = 0x1dd00200; 
                /* Jer: IHP_STP_TABLE address 0x003a0000 */ 
                sbusdma_desc[j].addr   = 0x003a0000;
                sbusdma_desc[j].hostaddr = soc_cm_l2p(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * j]);
                
                /* Write to "IHB_FIFO_PIPE_40_TO_50" */
                sbusdma_desc[j+1].cntrl  = 0x0;
                /* req_words, bit 5:9 = 17 = 0x11 = 0x00000e20 */
                sbusdma_desc[j+1].req    = 0x00000e20; 
                sbusdma_desc[j+1].count  = 0x00000001;
                /* Jer: w IHB_FIFO_PIPE_40_TO_50 0 1 => opcode 0x27882200 */ 
                sbusdma_desc[j+1].opcode = 0x27882200; 
                /* Jer: IHB_FIFO_PIPE_40_TO_50 address 0x00b00000 */ 
                sbusdma_desc[j+1].addr   = 0x00b00000;
                sbusdma_desc[j+1].hostaddr = soc_cm_l2p(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * j]);
            }

            sbusdma_desc[count_sbusdma_desc - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;            

            rv = _appl_sbusdma_desc_measure_run(unit, cmc, ch, sbusdma_desc, i);
            if (rv != CMD_OK) {
                return CMD_FAIL;
            }
            /* _appl_sbusdma_desc_measure_buff_print(unit, &buff[(SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) * 0]); */
        }
        soc_sand_ll_timer_stop(17);
    }
    

#if (defined(LINUX) || defined(UNIX))
/*
    soc_sand_ll_timer_set("kernel write", 6);
    cpu_regs_addr = 0x6;
    _cpu_write(unit, cpu_regs_addr, &cpu_regs_value);
    soc_sand_ll_timer_stop(6);

    soc_sand_ll_timer_set("kernel read", 7);
    cpu_regs_addr = 0x7;
    _cpu_read(unit, cpu_regs_addr, &cpu_regs_value);
    soc_sand_ll_timer_stop(7);  

    soc_sand_ll_timer_set("kernel write single", 8);
    cpu_regs_addr = 0x8;
    _cpu_write(unit, cpu_regs_addr, &cpu_regs_value);
    soc_sand_ll_timer_stop(8);

    soc_sand_ll_timer_set("kernel read single", 9);
    cpu_regs_addr = 0x9;
    _cpu_read(unit, cpu_regs_addr, &cpu_regs_value);
    soc_sand_ll_timer_stop(9);  
*/
#endif

    if (buff != NULL) {
        soc_cm_sfree(unit, buff);
    }
    if (sbusdma_desc != NULL) {
        soc_cm_sfree(unit, sbusdma_desc);
    }

    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();
    soc_sand_ll_timer_clear();

    return rv;
}

char cmd_dpp_arad_card_usage[] =
    "\n\tARAD card board specific commands\n\t"
    "Usages:\n\t"
    "arad [OPTION] <parameters> ...\n\t"
    "\n\t"
    "OPTION can be:\n\t"
    "device_init - perform arad device init (set new PUC)."
    "acp_device_init - perform ACP device init (buring up PCIE)."
    "board_init - perform init host board, which include rbf load and config, board struct init\n\t"
    "pcie test - perform PCIE time measurment test\n\t"
    "board_init parameters can be:\n\t"
    "\tio_agent - load only io_agent rbf\n\t"
    "utils parameters can be:\n\t"
    "\t\tsynt_set - set board syntisizers by <synt_num> <freq>.\n\t"
    "\t\t\tsynt_num: 0 = fabric, 1 = nif, 2 = dram, 3 = core\n\t"
    "\t\ttemp - read board temprature sensor by \"temp\".\n\t"
    "\t\tpower - read board Voltage/curret monitorby \"power\".\n\t"
    "\t\tport_speed_set - set port speed by <port> <speed>\n\t"
    "\n\t"
    "Examples:\n\t"
    "arad device_init 0xa188\n\t"
    "arad acp_device_init\n\t"
    "arad utils synt_set 1 250000000\n\t"
    "arad utils temp\n\t"
    "arad utils power\n\t"
    "arad utils fpga_burn io_agent.rbf\n"
    "arad pcie_test count=100000\n"
    "arad utile port_speed_set 13 10000\n"
    ;


char cmd_dpp_qmx_card_usage[] =
    "\nQMX card board specific commands\n"
    "Usages:\n"
    "qmx [OPTION] <parameters> ...\n"
    "\n"
    "OPTION can be:\n"
    "utils parameters can be:\n"
    "\ttemp     - read board temprature sensor by \"temp\".\n"
    "\tpower    - read board Voltage/curret monitor by \"power\".\n"
    "\tfpga_burn - burn fpga from specified file by \"fpga_burn\".\n"
    "\n"
    "Examples:\n"
    "qmx utils power\n"
    "qmx utils fpga_burn io_agent.rbf\n"
    "qmx utils temp\n"
    ;

char cmd_dpp_jericho_card_usage[] =
    "\nJERICHO card board specific commands\n"
    "Usages:\n"
    "jericho [OPTION] <parameters> ...\n"
    "\n"
    "OPTION can be:\n"
    "device_init - perform arad device init."
    "utils parameters can be:\n"
    "\ttemp     - read board temprature sensor by \"temp\".\n"
    "\tpower    - read board Voltage/curret monitor by \"power\".\n"
    "\tfpga_burn - burn fpga from specified file by \"fpga_burn\".\n"
    "\n"
    "Examples:\n"
    "jericho utils power\n"
    "jericho utils fpga_burn io_agent.rbf\n"
    "jericho utils temp\n"
    ;

cmd_result_t
cmd_dpp_arad_card(int unit, args_t* a)
{
#ifndef __KERNEL__
    char      *function, *option;
    int strnlen_function, strnlen_option;
    SOC_BSP_CARDS_DEFINES card_type;
#if (defined(LINUX) || defined(UNIX)) 
    int int_value = 0;
#endif

    /* synt set configuration */
    char *synt, *targetfreq, *force;
    uint32 ret = 0, int_synt, int_targetfreq, int_force, nominal_freq = 0;

    /* Fpga download and burn */
    char *file_name;

    host_board_type_from_nv(&card_type);
    if ((card_type != LINE_CARD_ARAD) && (card_type != LINE_CARD_ARAD_DVT) && (card_type != LINE_CARD_ARAD_NOACP) ) {
        cli_out("%s(): ERROR: Using utility on wrong card type (0x%x) !!!\n", FUNCTION_NAME(), card_type);
        return SOC_SAND_ERR;
    }
    
    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(function, "device_init", strnlen_function)){

        char *puc;
        uint32 int_puc;

        /* Turn off all writes to pcie */
        /* Stop counter proc */
        ret = soc_counter_stop(unit);
        if (ret < 0) {
            cli_out("%s: Error: Could not set counter mode: stop\n", FUNCTION_NAME());
            return CMD_FAIL;
        }

        /* ARAD - Reset  */
        cli_out("%s: Arad - Reset\n", FUNCTION_NAME());
         cpu_i2c_write(0x40, 0x6, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x33);

         /* Configure PUC*/
        puc = ARG_GET(a);
        if (!puc) {
             cli_out("%s: Use old PUC\n", FUNCTION_NAME());
        } else {
            int_puc = sal_ctoi(puc, 0);

            int_puc = int_puc << 1;
             cli_out("%s: Use PUC=0x%x\n", FUNCTION_NAME(), int_puc);
            cpu_i2c_write(0x40, 0x9, CPU_I2C_ALEN_LONG_DLEN_LONG, (int_puc &0xff));
            cpu_i2c_write(0x40, 0xa, CPU_I2C_ALEN_LONG_DLEN_LONG, ((int_puc >> 8) & 0xff));
            cpu_i2c_write(0x40, 0xb, CPU_I2C_ALEN_LONG_DLEN_LONG, ((int_puc >> 16) & 0xff));
            cpu_i2c_write(0x40, 0xc, CPU_I2C_ALEN_LONG_DLEN_LONG, ((int_puc >> 24) & 0xff));
        }

          /* ARAD - Out of resert */
         cli_out("%s: Arad - Out of resert\n", FUNCTION_NAME());
         cpu_i2c_write(0x40, 0x6, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x37);

         sal_usleep(100000);

         /* bring up PCIE */
         cli_out("%s: bring up PCIE\n", FUNCTION_NAME());
#if (defined(LINUX) || defined(UNIX))
         _cpu_pci_register(0);
#endif

         cli_out("%s: Rerun arad.soc\n", FUNCTION_NAME());

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (! sal_strncasecmp(function, "acp_device_init", strnlen_function)){

         /* bring up PCIE */
         cli_out("%s: bring up PCIE\n", FUNCTION_NAME());
#if defined (BCM_PETRA_SUPPORT) && (defined(LINUX) || defined(UNIX))
         _cpu_pci_register(0);
#endif

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    }else if (! sal_strncasecmp(function, "board_init", strnlen_function) ){
        cli_out("%s(): Not implemented!!!\n", FUNCTION_NAME());

    } else if (! sal_strncasecmp(function, "utils", strnlen_function) ){

#if (defined(LINUX) || defined(UNIX))
        if (arad_card_fpga_io_regs_init()) {
            cli_out("%s(): ERROR: arad_card_fpga_io_regs_init(). FAILED !!!\n", FUNCTION_NAME());
            return SOC_SAND_ERR;
        }

        if (arad_BOARD_SPECIFICATIONS_clear(card_type)) {
            cli_out("%s(): ERROR: arad_BOARD_SPECIFICATIONS_clear(). FAILED !!!\n", FUNCTION_NAME());
            return SOC_SAND_ERR;
        }
#endif

        option = ARG_GET(a);
        if (! option ) {
            return CMD_USAGE; 
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (! sal_strncasecmp(option, "synt_set", strnlen_option) ){ 

            synt = ARG_GET(a);
            targetfreq = ARG_GET(a);
            if ((! synt ) || (! targetfreq)){
                return CMD_USAGE;
            } else {
                int_synt = sal_ctoi(synt, 0);
                int_targetfreq = sal_ctoi(targetfreq, 0);

                force = ARG_GET(a);                

                if (force) {
                    int_force = sal_ctoi(force, 0);
                } else {
                    int_force = 0;
                }
 
#if (defined(LINUX) || defined(UNIX))      
                if (!int_force) {
                    nominal_freq = arad_card_board_synt_nominal_freq_get(int_synt);
                }
#endif

                if ((int_force) || (int_targetfreq != nominal_freq)) {
#if (defined(LINUX) || defined(UNIX)) 
                    ret = arad_card_board_synt_set(int_synt, int_targetfreq, FALSE);
#endif
                    if (ret) {
                        cli_out("Error in %s(): arad_card_board_synt_set(). "
                                "FAILED !!!\n", FUNCTION_NAME());
                    }
                }
            }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "fpga_burn", strnlen_option) ){ 
            
            file_name = ARG_GET(a);
            if (! file_name ) {
                return CMD_USAGE;
            } 
            
            cli_out("%s(): Download and Burn %s\n", FUNCTION_NAME(), file_name);
#if (defined(LINUX) || defined(UNIX))
            soc_dpp_fpga_load(unit, file_name);         
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "temp", strnlen_option) ){ 
#if (defined(LINUX) || defined(UNIX)) 
            /* Set i2c mux */
            cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x80);

            /* Read Temperature */
            cpu_i2c_read(0x4c, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Ambient temperature %d Cel (0x%x - 0x%x)\n", FUNCTION_NAME(), int_value - 64, int_value, 64);

            cpu_i2c_read(0x4c, 0x1, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): chip temperature %d Cel (0x%x - 0x%x).\n", FUNCTION_NAME(), int_value - 64, int_value, 64);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "power", strnlen_option) ){

            char *param;
            int int_param = 0;

            param = ARG_GET(a);
            if (!param) {
                int_param = 0x1;
            } else {
                int_param = sal_ctoi(param, 0);
            }
#if (defined(LINUX) || defined(UNIX)) 
            card_power_monitor(unit, int_param);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "port_speed_set", strnlen_option) ){

            char *param;
            bcm_port_t port = 0;
            int speed = 0;
            bcm_port_config_t config;
            
            param = ARG_GET(a);
            if (!param) {
                return CMD_USAGE;
            } else {
                port = sal_ctoi(param, 0);
            }
            param = ARG_GET(a);
            if (!param) {
                return CMD_USAGE;
            } else {
                speed = sal_ctoi(param, 0);
            }
            ret = bcm_port_speed_set(unit, port, speed);
            if (ret < 0) {
                cli_out("%s: Error: %d could not change port speed\n", FUNCTION_NAME(), ret);
                return CMD_FAIL;
            }
            ret = bcm_port_config_get(unit, &config);
            if (ret < 0) {
                cli_out("%s: Error: %d could not get ports config\n", FUNCTION_NAME(), ret);
                return CMD_FAIL;
            }
            if((!BCM_PBMP_MEMBER(config.sfi, port)) && (speed == 12500)){
                ret = bcm_port_phy_control_set(unit, port ,BCM_PORT_PHY_CONTROL_PREEMPHASIS, 0x4278); 
                if (ret < 0) {
                    cli_out("%s: Error: %d could not set port control\n", FUNCTION_NAME(), ret);
                    return CMD_FAIL;
                }
            }
        } else {
            return CMD_USAGE;
        }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (! sal_strncasecmp(function, "set_device", strnlen_function)){
    
        char *param;
        uint32 dev_id;
        uint32 rev_id;
        ibde_dev_t *dev = (ibde_dev_t*)bde->get_dev(CMDEV(unit).dev.dev);        
        /* get dev id */ 
        param = ARG_GET(a);
        if (!param) {
            return CMD_USAGE;
        } else {
            dev_id = sal_ctoi(param, 0);
        } 
        
        /* get rev id */ 
        param = ARG_GET(a);
        if (!param) {            
            rev_id= ARAD_B1_REV_ID;
        } else {
            rev_id = sal_ctoi(param, 0);
        } 
        CMDEV(unit).dev.dev_id = dev_id;
        CMDEV(unit).dev.rev_id = rev_id;
        
        dev->device = dev_id;
        dev->rev = rev_id;
        
    } else {
        return CMD_USAGE;
   } 

    return CMD_OK;
#endif /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
}

char cmd_dpp_ardon_card_usage[] =
    "\n\tArdon card board specific commands\n\t"
    "Usages:\n\t"
    "ardon [OPTION] <parameters> ...\n\t"
    "\n\t"
    "OPTION can be:\n\t"
    "utils parameters can be:\n\t"
    "\t\tardon_dvt_temp - read board temprature sensor.\n\t"
    "\n\t"
    "Examples:\n\t"
    "ardon utils ardon_dvt_temp\n\t"
    "\n"
    ;

cmd_result_t
cmd_dpp_ardon_card(int unit, args_t* a)
{
#ifndef __KERNEL__
    int rv;
    uint8 buffer = 0x0;
    char      *function, *option;
    int strnlen_function, strnlen_option;

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(function, "utils", strnlen_function) ){

        option = ARG_GET(a);
        if (! option ) {
            return CMD_USAGE; 
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (! sal_strncasecmp(option, "ardon_dvt_temp", strnlen_option)) {

            if (!soc_i2c_is_attached(unit)) {
                cli_out("ardon_dvt_temp command require probe ('i2c probe') to be called first. running probing !\n");
                rv = soc_i2c_probe(unit); /* Will attach if not yet attached */
                if (SOC_FAILURE(rv)) {
                    cli_out("I2C Test Failed: I2C probe Failed\n");
                    return CMD_FAIL;
                }
            }

            /* Read Thernal Diode 0 (at address 0x18) */
            rv = soc_i2c_read_byte_data(unit, 0x18, 0x0, &buffer);
            if (SOC_FAILURE(rv)) {
                cli_out("ardon_dvt_temp: fail to read Ambient temperature from device 0x18 address 0x0x\n");
                return CMD_USAGE;
            }
            cli_out("%s(): Thernal Diode 0 (0x18): Ambient temperature %d (0x%x)\n", FUNCTION_NAME(), buffer, buffer);

            rv = soc_i2c_read_byte_data(unit, 0x18, 0x1, &buffer);
            if (SOC_FAILURE(rv)) {
                cli_out("ardon_dvt_temp: fail to read Ambient temperature from device 0x18 address 0x1x\n");
                return CMD_USAGE;
            }
            cli_out("%s(): Thernal Diode 0 (0x18): chip temperature %d (0x%x)\n", FUNCTION_NAME(), buffer, buffer);

            /* Read Thernal Diode 1 (at address 0x19) */
            rv = soc_i2c_read_byte_data(unit, 0x19, 0x0, &buffer);
            if (SOC_FAILURE(rv)) {
                cli_out("ardon_dvt_temp: fail to read Ambient temperature from device 0x19 address 0x0x\n");
                return CMD_USAGE;
            }
            cli_out("%s(): Thernal Diode 1 (0x19): Ambient temperature %d (0x%x)\n", FUNCTION_NAME(), buffer, buffer);

            rv = soc_i2c_read_byte_data(unit, 0x19, 0x1, &buffer);
            if (SOC_FAILURE(rv)) {
                cli_out("ardon_dvt_temp: fail to read Ambient temperature from device 0x19 address 0x1x\n");
                return CMD_USAGE;
            }
            cli_out("%s(): Thernal Diode 1 (0x19): chip temperature %d (0x%x)\n", FUNCTION_NAME(), buffer, buffer);

        } else {
            return CMD_USAGE;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
#endif /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
}

#ifndef __KERNEL__
/*
 * Set severity corresponding to bsl meta identifier as specified
 * and return originally stored severity.
 */
static bsl_severity_t set_log_level_to(
    unsigned long meta_identifier,
    bsl_severity_t required_severity)
{
    int layer, source ;
    bsl_severity_t stored_severity ;

    layer = BSL_LAYER_GET(meta_identifier);
    source = BSL_SOURCE_GET(meta_identifier);
    stored_severity = bslenable_get(layer, source) ;
    bslenable_set((bsl_layer_t)layer, (bsl_source_t)source, required_severity) ;
    return (stored_severity) ;
}
#endif /* __KERNEL__ */

cmd_result_t
cmd_dpp_qmx_card(int unit, args_t* a)
{
#ifndef __KERNEL__
    char      *function, *option ;
    char strnlen_function, strnlen_option ;
    SOC_BSP_CARDS_DEFINES card_type ;
    bsl_severity_t original_severity ;
#if (defined(LINUX) || defined(UNIX)) 
    int int_value = 0 ;
#endif
    /* Fpga download and burn */
    char *file_name ;
    cmd_result_t ret ;
    /*
     * Set log level of I2C to 'warning'. Restore at exit.
     */
    original_severity = set_log_level_to(BSL_LS_SOC_I2C,bslSeverityWarn) ; 

    host_board_type_from_nv(&card_type);
    
    if (card_type != LINE_CARD_JERICHO) {
        cli_out("%s(): ERROR: Using utility on wrong card type (0x%x) !!!\n", FUNCTION_NAME(), card_type);
        ret = SOC_SAND_ERR;
        goto exit ;
    }
    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(function, "device_init", strnlen_function)){
        /* Turn off all writes to pcie */
        /* Stop counter proc */
        ret = soc_counter_stop(unit);
        if (ret < 0) {
            cli_out("%s: Error: Could not set counter mode: stop\n", FUNCTION_NAME());
            return CMD_FAIL;
        }

        /* Jericho - Reset  */
        cli_out("%s: Jericho - Reset\n", FUNCTION_NAME());


        cpu_i2c_write(0x40, 0x6, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xfb);

        cpu_i2c_write(0x40, 0x29, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x01);
        cpu_i2c_write(0x40, 0x29, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x00);

         /* Jericho - Out of resert */
         cli_out("%s: Jericho - Out of resert\n", FUNCTION_NAME());
         sal_usleep(1000000);
         cpu_i2c_write(0x40, 0x6, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xff);
         sal_usleep(1000000);

         /* bring up PCIE */
         cli_out("%s: bring up PCIE\n", FUNCTION_NAME());
#if (defined(LINUX) || defined(UNIX))
         _cpu_pci_register(0);
#endif

         cli_out("%s: Rerun jer.soc\n", FUNCTION_NAME());

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (! sal_strncasecmp(function, "utils", strnlen_function) ){
#if (defined(LINUX) || defined(UNIX))
        if (qmx_BOARD_SPECIFICATIONS_clear(card_type)) {
            cli_out("%s(): ERROR: qmx_BOARD_SPECIFICATIONS_clear() FAILED !!!\n", FUNCTION_NAME());
            ret = SOC_SAND_ERR;
            goto exit ;
        }
#endif
        option = ARG_GET(a);
        if (! option ) {
            ret = CMD_USAGE; 
            goto exit ;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (! sal_strncasecmp(option, "fpga_burn", strnlen_option) ){
            file_name = ARG_GET(a);
            if (! file_name ) {
                ret = CMD_USAGE;
                goto exit ;
            } 
            
            LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Download and Burn %s\n"), file_name));

#if (defined(LINUX) || defined(UNIX))
            soc_dpp_fpga_load(unit, file_name);         
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "temp", strnlen_option) ){ 
#if (defined(LINUX) || defined(UNIX)) 
            /* Set (open) i2c mux */
            cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x80);

            /* Check 'temperature range select' on configuration register. If it is 0 - 127 */
            /* then change it to 'extended range' (to include sub-zero temperatures)        */

            /* Read configuration register and offset register. */
            cpu_i2c_read(0x4c, 0x03, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Configuration register is 0x%x\n", FUNCTION_NAME(), int_value) ;
            cpu_i2c_read(0x4c, 0x11, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Offset register is        0x%x\n", FUNCTION_NAME(), int_value) ;

            /* Read Temperatures */
            cpu_i2c_read(0x4c, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Ambient temperature %d Cel (0x%x)\n", FUNCTION_NAME(), int_value, int_value);

            cpu_i2c_read(0x4c, 0x1, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Chip temperature    %d Cel (0x%x).\n", FUNCTION_NAME(), int_value, int_value);

            /* Set (close) i2c mux */
            cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x00);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "power", strnlen_option) ){
             char *param;
            int int_param = 0;
             param = ARG_GET(a);
            if (!param) {
                int_param = 0x1;
            } else {
                int_param = sal_ctoi(param, 0);
            }
#if (defined(LINUX) || defined(UNIX)) 
            card_power_monitor(unit, int_param);
#endif
        } else {
            ret = CMD_USAGE;
            goto exit ;
        }
    } else {
        ret = CMD_USAGE;
        goto exit ;
    } 
    ret = CMD_OK ;
exit:
    /*
     * Restore log level of I2C.
     */
    set_log_level_to(BSL_LS_SOC_I2C,original_severity) ; 
    return (ret) ;
#else /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
#endif /* __KERNEL__ */
}

cmd_result_t
cmd_dpp_jericho_card(int unit, args_t* a)
{
#ifndef __KERNEL__
    char      *function, *option ;
    int strnlen_function, strnlen_option ;
    SOC_BSP_CARDS_DEFINES card_type ;
    bsl_severity_t original_severity ;
#if (defined(LINUX) || defined(UNIX)) 
    int int_value = 0 ;
#endif
    /* Fpga download and burn */
    char *file_name ;
    cmd_result_t ret ;
    /*
     * Set log level of I2C to 'warning'. Restore at exit.
     */
    original_severity = set_log_level_to(BSL_LS_SOC_I2C,bslSeverityWarn) ; 

    host_board_type_from_nv(&card_type);

    if (card_type != LINE_CARD_JERICHO) {
        cli_out("%s(): ERROR: Using utility on wrong card type (0x%x) !!!\n", FUNCTION_NAME(), card_type);
        ret = SOC_SAND_ERR;
        goto exit ;
    }
    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(function, "utils", strnlen_function) ){
#if (defined(LINUX) || defined(UNIX))
        if (jericho_BOARD_SPECIFICATIONS_clear(card_type)) {
            cli_out("%s(): ERROR: jericho_BOARD_SPECIFICATIONS_clear() FAILED !!!\n", FUNCTION_NAME());
            ret = SOC_SAND_ERR;
            goto exit ;
        }
#endif
        option = ARG_GET(a);
        if (! option ) {
            ret = CMD_USAGE; 
            goto exit ;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (! sal_strncasecmp(option, "fpga_burn", strnlen_option) ){ 
            
            file_name = ARG_GET(a);
            if (! file_name ) {
                ret = CMD_USAGE;
                goto exit ;
            } 
            
            LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Download and Burn %s\n"), file_name));

#if (defined(LINUX) || defined(UNIX))
            soc_dpp_fpga_load(unit, file_name);         
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "temp", strnlen_option) ){ 
#if (defined(LINUX) || defined(UNIX)) 
            /* Set (open) i2c mux */
            cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x80);

            /* Check 'temperature range select' on configuration register. If it is 0 - 127 */
            /* then change it to 'extended range' (to include sub-zero temperatures)        */

            /* Read configuration register and offset register. */
            cpu_i2c_read(0x4c, 0x03, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Configuration register is 0x%x\n", FUNCTION_NAME(), int_value) ;
            cpu_i2c_read(0x4c, 0x11, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Offset register is        0x%x\n", FUNCTION_NAME(), int_value) ;

            /* Read Temperatures */
            cpu_i2c_read(0x4c, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Ambient temperature %d Cel (0x%x)\n", FUNCTION_NAME(), int_value, int_value);

            cpu_i2c_read(0x4c, 0x1, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &int_value);
            cli_out("%s(): Chip temperature    %d Cel (0x%x).\n", FUNCTION_NAME(), int_value, int_value);

            /* Set (close) i2c mux */
            cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x00);
#endif
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (! sal_strncasecmp(option, "power", strnlen_option) ){
             char *param;
            int int_param = 0;
             param = ARG_GET(a);
            if (!param) {
                int_param = 0x1;
            } else {
                int_param = sal_ctoi(param, 0);
            }
#if (defined(LINUX) || defined(UNIX)) 
            card_power_monitor(unit, int_param);
#endif
        } else {
            ret = CMD_USAGE;
            goto exit ;
        }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (! sal_strncasecmp(function, "set_device", strnlen_function)){

        char *param;
        uint32 dev_id;
        uint32 rev_id;
        ibde_dev_t *dev = (ibde_dev_t*)bde->get_dev(CMDEV(unit).dev.dev);
        /* get dev id */
        param = ARG_GET(a);
        if (!param) {
            return CMD_USAGE;
        } else {
            dev_id = sal_ctoi(param, 0);
        }

        /* get rev id */
        param = ARG_GET(a);
        if (!param) {
            rev_id= JERICHO_A0_REV_ID;
        } else {
            rev_id = sal_ctoi(param, 0);
        }
        CMDEV(unit).dev.dev_id = dev_id;
        CMDEV(unit).dev.rev_id = rev_id;

        dev->device = dev_id;
        dev->rev = rev_id;
    } else {
        ret = CMD_USAGE;
        goto exit ;
    } 
    ret = CMD_OK ;
exit:
    /*
     * Restore log level of I2C.
     */
    set_log_level_to(BSL_LS_SOC_I2C,original_severity) ; 
    return (ret) ;
#else /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
#endif /* __KERNEL__ */
}

#endif  /*(defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_PETRA_SUPPORT) */

#ifdef BCM_PETRA_SUPPORT
#if defined(__DUNE_GTO_BCM_CPU__)
char cmd_dcmn_negev_chassis_usage[] =
    "\nNegev Chassis specific commands\n"
    "Usage:\n"
    "\tnegev [OPTION] <parameters> ...\n"
    "\n"
    "OPTION can be:\n"
    "\teeprom - read/write data from/to gfa_bi eeprom by: <read/write> <address/param> <val>. \n"
    "\t\tparam: board_type, board_sn, pcb_ver, assembly_ver, board_desc, address\n"
    "\t\tboard_desc - up to 30 bytes tring\n"
    "\tread_all - read all the information."
    "\tfan_speed - read the speed of the fans."
    "\ti2c_mux_set - set the mux on one of the cards if exist\n."
    "\t\tparam: lc0, lc1, fc, mng\n"
    "\n"
    "Examples:\n"
    "\tnegev eeprom read board_type\n"
    "\tnegev eeprom read board_desc\n"
    "\tnegev eeprom read address 0x77\n"
    "\tnegev eeprom read_all\n"
    "\tnegev eeprom write board_desc Arad SVK Board\n"
    "\tnegev fan_speed\n"
    "\tnegev i2c_mux_set lc1\n"
    ;

int
cmd_dcmn_negev_chassis_eeprom_read_all(int unit)
{
    uint32 ret = 0;
    int int_value=0;
    char eeprom_str[NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC + 1]; /* for eeprom_str*/
     
    sal_memset(eeprom_str, 0x0, (NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC + 1) * sizeof(char));
    
    /* print board type */
    ret = eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE, 4, &int_value);
    if (ret != 0) {         
        cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
        return ret;
    }
    cli_out("board type:        0x%x = %d" , int_value, int_value); 

#if (defined(LINUX) || defined(UNIX))

    cli_out("   %s", bsp_card_enum_to_str((SOC_BSP_CARDS_DEFINES)int_value));
   
#endif

    cli_out("\n");
          
     /* print board sn */
    ret = eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_SN, 4, &int_value);
    if (ret != 0) {
           cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
           return ret;
    }
    cli_out("board sn:          0x%x = %d\n", int_value, int_value); 
     
     /* print pcb version */
    ret = eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_PCB_VERSION, 4, &int_value);
    if (ret != 0) {
        cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
        return ret;
    }
    cli_out("pcb version:       0x%x \n", int_value); 
     
    /* print assembly version */
    ret = eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_ASSEMBLY_VERSION, 4, &int_value);
    if (ret != 0) {
        cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
        return ret;
    }
    cli_out("assmbly version:   0x%x \n", int_value); 

    /* print eeprom_str */
    ret = eeprom_read_str(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_DESC, NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC, eeprom_str);
    if (ret != 0) {
        cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
        return ret;
    }
    cli_out("board desc:        %s\n", eeprom_str);
    
    return 0;
}

int cmd_dcmn_negev_chassis_read_fan_speed(int unit, int fan_num, int* fun_speed)
{
#ifndef __KERNEL__
    int ret;
    /* read the speed */
    ret = cpu_i2c_write(0x42, 0x20, CPU_I2C_ALEN_LONG_DLEN_LONG, fan_num);
    if( ret != SOC_E_NONE){
        cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
        return ret;
    }

    ret = cpu_i2c_write(0x42, 0x20, CPU_I2C_ALEN_LONG_DLEN_LONG, fan_num + 0x10);
    if( ret != SOC_E_NONE){
        cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
        return ret;
    }

    sal_usleep(250000);

    ret = cpu_i2c_read(0x42, 0x21, CPU_I2C_ALEN_LONG_DLEN_LONG, fun_speed);
    if( ret != SOC_E_NONE){
        cli_out("Error in %s(): cpu_i2c_read(). FAILED !!!\n", FUNCTION_NAME());
        return ret;
    }

    return 0;
#endif /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return SOC_E_UNAVAIL;
}

int convert_fan_speed_to_rmp_unit(int fan_speed)
{
    int res1, res2;

    if( fan_speed <= 4 ) {
        return 0;
    }

    res1 = (160000000/fan_speed);/* ((60*40/15)*10^7)/fan_speed */
    res2 = ((160000000 - res1*fan_speed)*100)/fan_speed; /* add the remainer from 160000000/fan_speed */
    res1 *= 100;/* *10^2 */

    return res1 + res2;
}

cmd_result_t
cmd_dcmn_negev_chassis(int unit, args_t* a)
{
#ifndef __KERNEL__
    char      *function;
    int strnlen_function;
    uint32 ret = 0;
    /* parse_table_t pt; */

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(function, "eeprom", strnlen_function) ) {

        char *function, *param, *value;
        int strnlen_param;
        int int_data_addr, int_value = 0, eeprom_access_size = 1;

        function = ARG_GET(a);
        if (!function) {
            return CMD_USAGE;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(function, "write", strnlen_function)) {

            param = ARG_GET(a);
            if (!param) {
                return CMD_USAGE;
            }
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            strnlen_param = sal_strnlen(param,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
            if (!sal_strncasecmp(param, "board_desc", strnlen_param)) {

                char eeprom_str[NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC + 2];
                char *str;
                int str_len;

                sal_memset(eeprom_str, 0x0, (NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC + 2) * sizeof(char));
                eeprom_str[0] = '\0';

                int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_DESC;
                eeprom_access_size = NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC;

                str = ARG_GET(a);
                while (str) {

                    if ((sal_strlen(eeprom_str) + sal_strlen(str)) > NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC) {
                        return CMD_USAGE;
                    }

                    sal_memcpy(eeprom_str + sal_strlen(eeprom_str), str, sal_strlen(str));
                    str_len = sal_strlen(eeprom_str);

                    eeprom_str[str_len] = ' ';
                    eeprom_str[str_len + 1] = '\0';
                    
                    str = ARG_GET(a);
                }

                cli_out("eeprom_str=%s\n", eeprom_str);

                ret = eeprom_write_str(NEGEV_CHASSIS_CARD_TYPE, int_data_addr, eeprom_access_size, eeprom_str);
                    if (ret != 0) {
                        cli_out("Error in %s(): eeprom_write_str(). FAILED !!!\n", FUNCTION_NAME());
                        return CMD_USAGE;
                    }
                
            } else {

                /*
                 * Make sure at least one of the strings is NULL terminated.
                 */
                 if (!sal_strncasecmp(param, "board_type", strnlen_param)) {
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "board_sn", strnlen_param)) {
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_SN;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "pcb_version", strnlen_param)) {
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_PCB_VERSION;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "assembly_version", strnlen_param)) {
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_ASSEMBLY_VERSION;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "address", strnlen_param)) {

                    char* addr = ARG_GET(a);
                    if(!addr) {
                        return CMD_USAGE;
                    }

                    int_data_addr = sal_ctoi(addr, 0);
                    eeprom_access_size = 1;

                } else {
                    return CMD_USAGE;
                }

                value = ARG_GET(a);
                if (!value) {
                    return CMD_USAGE;
                }
                int_value = sal_ctoi(value, 0);

                ret = eeprom_write(NEGEV_CHASSIS_CARD_TYPE, int_data_addr, eeprom_access_size, int_value);
                if (ret != 0) {
                    cli_out("Error in %s(): eeprom_write(). FAILED !!!\n", FUNCTION_NAME());
                    return CMD_FAIL;
                }
            }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (!sal_strncasecmp(function, "read", strnlen_function) ){

            param = ARG_GET(a);
            if (!param) {
                return CMD_USAGE;
            }
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            strnlen_param = sal_strnlen(param,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
            if (!sal_strncasecmp(param, "board_desc", strnlen_param)) {

                char eeprom_str[NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC + 1];

                sal_memset(eeprom_str, 0x0, NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC * sizeof(char));
                eeprom_str[NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC] = '\0';

                int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_DESC;
                eeprom_access_size = NEGEV_CHASSIS_EEPROM_BYTE_SIZE_ADRESS_BOARD_DESC;

                ret = eeprom_read_str(NEGEV_CHASSIS_CARD_TYPE, int_data_addr, eeprom_access_size, eeprom_str);
                if (ret != 0) {
                    cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
                    return CMD_FAIL;
                }
                cli_out("eeprom_str=%s\n", eeprom_str);
                

            } else {

                /*
                 * Make sure at least one of the strings is NULL terminated.
                 */
                if (!sal_strncasecmp(param, "board_type", strnlen_param)) {
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "board_sn", strnlen_param)) {
                
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_SN;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "pcb_version", strnlen_param)) {
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_PCB_VERSION;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "assembly_version", strnlen_param)) {
                    int_data_addr = NEGEV_CHASSIS_EEPROM_ADRESS_ASSEMBLY_VERSION;
                    eeprom_access_size = 4;
                } else if (!sal_strncasecmp(param, "address", strnlen_param)) {

                    char* addr = ARG_GET(a);
                    if(!addr) {
                        return CMD_USAGE;
                    }

                    int_data_addr = sal_ctoi(addr, 0);
                    eeprom_access_size = 1;
                } else {
                    return CMD_USAGE;
                }

                ret = eeprom_read(NEGEV_CHASSIS_CARD_TYPE, int_data_addr, eeprom_access_size, &int_value);
                if (ret != 0) {
                    cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
                    return CMD_FAIL;
                }

                cli_out("data=0x%x,%d\n", int_value, int_value);
#if (defined(LINUX) || defined(UNIX))
                if (int_data_addr == NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE) {
                    cli_out("%s(): board type: %s\n", FUNCTION_NAME(), bsp_card_enum_to_str((SOC_BSP_CARDS_DEFINES)int_value));
                }
#endif
            }   

        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (!sal_strncasecmp(function, "read_all", strnlen_function) ) {

            if(!sys_board_type_mng_get()) {

            cli_out(" Negev - eeprom read \n"    
                    "=====================\n");

            ret = cmd_dcmn_negev_chassis_eeprom_read_all(unit);
            if(ret!=0) {
                return CMD_FAIL;
            }
            } else {

                int mng_card_io_agent_status_reg = 0;

                /* read the card status to see witch card exist */
                cpu_i2c_read(0x42, 0x10, CPU_I2C_ALEN_LONG_DLEN_LONG, &mng_card_io_agent_status_reg);

                cli_out(" Negev - eeprom read mng\n"    
                        "=========================\n"); 
                   
                /* read from mng card */
                cli_out("\nManagement card:\n"    
                        "----------------\n");

                cpu_i2c_write(0x77, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, 0x0);

                cpu_i2c_read(0x42, 0xf, CPU_I2C_ALEN_LONG_DLEN_LONG, &int_value);  /* turn on bit 6*/
                cpu_i2c_write(0x42, 0xf, CPU_I2C_ALEN_LONG_DLEN_LONG, int_value| 0x40 );

                ret = cmd_dcmn_negev_chassis_eeprom_read_all(unit);
                if(ret!=0) {
                    return CMD_FAIL;
                }

                cpu_i2c_write(0x42, 0xf, CPU_I2C_ALEN_LONG_DLEN_LONG, int_value); /* turn off bit 6 */

                /* read from arad 1 */
                if (mng_card_io_agent_status_reg & 0x1){
                
                    cli_out("\nArad 1:\n"    
                            "-------\n");  
                      
                    cpu_i2c_write(0x77, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, 0x1);

                    ret = cmd_dcmn_negev_chassis_eeprom_read_all(unit);
                    if(ret!=0) {
                        return CMD_FAIL;
                    }
                }

               /* read from arad 2 */
                if (mng_card_io_agent_status_reg & 0x2){

                    cli_out("\nArad 2:\n"    
                            "-------\n");   
                     
                    cpu_i2c_write(0x77, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, 0x2);

                    ret = cmd_dcmn_negev_chassis_eeprom_read_all(unit);
                    if(ret!=0) {
                        return CMD_FAIL;
                    }
                 }

                /* read from fabric */
                if (mng_card_io_agent_status_reg & 0x4){
                  
                    cli_out("\nFabric:\n"    
                            "-------\n"); 
                       
                    cpu_i2c_write(0x77, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, 0x4);
                    
                    ret = cmd_dcmn_negev_chassis_eeprom_read_all(unit);
                    if(ret!=0) {
                        return CMD_FAIL;
                    }
                }
            }
            } else {

                return CMD_USAGE;                
            }

        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (!sal_strncasecmp(function, "fan_speed", strnlen_function)) {

                int int_value;

                if(sys_board_type_mng_get()) {
                cli_out("fans speed:\n"
                        "-----------\n");

                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0x1, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Line Card Fan Side Right Close to MP:        0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));
              

                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0x3, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Line Card Fan Side Right Far from MP:        0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));

                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0x5, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Line Card Fan Side Left Close to MP:         0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));
               
                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0x7, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Line Card Fan Side Left Far from MP:         0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));
              
                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0x8, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Fabric Card Fan Side Right Close to MP:      0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));
          
                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0x9, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Fabric Card Fan Side Right Far from MP:      0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));
               
                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0xa, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Fabric Card Fan Side Left Close to MP:       0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));
              
                ret = cmd_dcmn_negev_chassis_read_fan_speed(unit, 0xb, &int_value);
                if(ret != 0) {
                    return CMD_FAIL;
                }
                cli_out("Fabric Card Fan Side Left Far from MP:       0x%x - %drpm\n", int_value, convert_fan_speed_to_rmp_unit(int_value));

                }else {
                    cli_out("disable to read fans speed (probably because there is no mng card)\n");    
                }
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            } else if (!sal_strncasecmp(function, "i2c_mux_set", strnlen_function)){

                char* param;
                int strnlen_param;
                int set_num, mng_card_io_agent_status_reg = 0, int_value/*, int_value*/;

                param = ARG_GET(a);
                if (! param ) {
                    return CMD_USAGE;
                } 

                 /* check if there is mng card */
                if(!sys_board_type_mng_get()) {
                    cli_out("Error: there is no mng card in the system\n");
                    return CMD_FAIL;    
                }
                strnlen_param = sal_strnlen(param,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
                /* check param */
                /*
                 * Make sure at least one of the strings is NULL terminated.
                 */
                if (!sal_strncasecmp(param, "lc0", strnlen_param)) {
                    set_num = 0x1;
                } else  if (!sal_strncasecmp(param, "lc1", strnlen_param)) {
                    set_num = 0x2;
                } else  if (!sal_strncasecmp(param, "fc", strnlen_param)) {
                    set_num = 0x4;
                } else  if (!sal_strncasecmp(param, "mng", strnlen_param)) {
                    set_num = 0x0;
                } else {
                    return CMD_USAGE;
                }
                /* disconnect the Management's EEPROM from the main I2C bus.*/ 
                cpu_i2c_write(0x77, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, 0x0);
                cpu_i2c_read(0x42, 0xf, CPU_I2C_ALEN_LONG_DLEN_LONG, &int_value);  /* turn on bit 6*/
                cpu_i2c_write(0x42, 0xf, CPU_I2C_ALEN_LONG_DLEN_LONG, int_value| 0x40 );

                /* read the card status to see witch card exist */
                cpu_i2c_read(0x42, 0x10, CPU_I2C_ALEN_LONG_DLEN_LONG, &mng_card_io_agent_status_reg);
                if( set_num && (!(mng_card_io_agent_status_reg & set_num))) {
                    cli_out("there is no card %s in the system\n", param);
                    return CMD_FAIL;
                }
  
                cpu_i2c_write(0x77, 0x0, CPU_I2C_ALEN_BYTE_DLEN_BYTE, set_num);

            }  else {
                return CMD_USAGE;
            } 
            

    return CMD_OK;
#endif /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
}


#ifdef BCM_CMICM_SUPPORT

static char* shr_llm_msg_notification_type_name[LLM_EVENT_MAX+1] = { SHR_LLM_MSG_NOTIFICATION_TYPE_NAME};

char cmd_dpp_llm_usage[] =
    "\nLLM Application  commands\n"
    "Usage:\n"
    "\tLLm [OPTION] <parameters> ...\n"
    "\n"
    "OPTION:\n"
    "\tinit   - ARM uKernel Application init, DMA allocations\n"
    "\tstop  - Stop all threads related to LLM connectivity\n"
    "\tstart  - Start ARM uKernel LLM application\n"
    "\tParameters:\n"
    "\t\tDspEthType     - Ethernet Destination Type. Default: 0x0\n"
    "\t\tVoqMapMode     - Voq Mapping mode. Default: 0x0\n"
    "\t\tDirTableBank   - Direct table Bank. Default: 0xa\n"
    "\t\tDirTableOffset - Direct table offset. Default: 0x0\n"
    "\t\tEEDBFirstIndex - First index of the EEDB. Default: 0x0\n"
    "\t\tEEDBSize       - The size of the database(upto 8k). Default: 8,192\n"
    "\tpon_att - Start ARM uKernel LLM application\n"
    "\tParameters:\n"
    "\t\tmac_bitmap_set port=<port_num> tunnel=<tunnel_num> tunnel_count=<tunnel_count> bitmap=<bitmap> type=<type> - Set MAC bitmap configuration.\n"
    "\t\tmac_bitmap_get port=<port_num> tunnel=<tunnel_num>                 - Get MAC bitmap configuration.\n"
    "\t\tservice read_reply_fifo=<enable> limit_enable=<enable> type=<type> - Enable/disable read reply FIFO and MAC limit function.\n"
    "\tinfo     - Retrieve  Application  info.\n"
    "\tParameters:\n"
    "\t\tappl_info      - General application info.\n"
    "\t\tpon_id_info port=<port_num>   - All information regarding PON port.\n"
    "\t\teedb_info      - Info of the EEDB pointer database status.\n"
    "\n"
    ;

/*
 * Function:
 *      _bcm_llm_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to BFD info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_llm_callback_thread(int unit)
{
    int rv;
    llm_appl_info_t *llm_info = shr_llm_appl_info(unit);      
    llm_msg_event_t event_msg;
    llm_info->event_thread_id   = sal_thread_self();
    llm_info->event_thread_kill = 0;

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(unit, llm_info->uc_num,
                                     MOS_MSG_CLASS_LLM_EVENT, &event_msg,
                                     sal_sem_FOREVER);

        if (BCM_FAILURE(rv) || (llm_info->event_thread_kill)) {
            cli_out("Event reiceved failed. Thread will exit");
            break;   /* Thread exit */
        }
        if (event_msg.s.len <LLM_EVENT_MAX)
        {
            cli_out("EVENT REICEVED: type = [%d][%s]; Data = [%d]\n", event_msg.s.len,shr_llm_msg_notification_type_name[event_msg.s.len],event_msg.s.data);
        }
        else
        {
             cli_out("EVENT REICEVED: type = [%d][NOT_DEFINED]; Data = [%d]\n", event_msg.s.len,event_msg.s.data);
        }
        

     }

    llm_info->event_thread_kill = 0;
    llm_info->event_thread_id   = NULL;
    sal_thread_exit(0);
}


STATIC int
_bcm_en_llm_msg_send_receive(int unit, uint8 s_subclass, uint16 s_len, uint32 s_data, uint8 r_subclass, uint16 *r_len)
{
    int rv = BCM_E_NONE;
    mos_msg_data_t send, reply;
    uint8 *dma_buffer;
    int dma_buffer_len;
    uint32 uc_rv;
    llm_appl_info_t *llm_info = shr_llm_appl_info(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_LLM;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    dma_buffer = llm_info->dma_buffer;
    dma_buffer_len = llm_info->dma_buffer_len;
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, llm_info->uc_num,
                                      &send, &reply,
                                      SHR_LLM_UC_MSG_TIMEOUT_USECS);

    /* Check reply class, subclass */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != MOS_MSG_CLASS_LLM) ||
        (reply.s.subclass != r_subclass)) {
        return BCM_E_INTERNAL;
    }

    /* Convert BHH uController error code to BCM */
    rv = uc_rv = bcm_ntohl(reply.s.data);
       
    *r_len = bcm_ntohs(reply.s.len);

    return rv;
}


cmd_result_t
cmd_dpp_llm(int unit, args_t* a)
{
    char      *function, *option;
    int strnlen_function, strnlen_option;
    uint32 ret = 0;
    llm_appl_info_t *llm_info = shr_llm_appl_info(unit);
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    /* parse_table_t pt; */

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(function, "init", strnlen_function)) {

        int uc = 0;
        int size = 0
            ;
        ret = soc_cmic_uc_appl_init(unit, uc,
                                    MOS_MSG_CLASS_LLM,
                                    SHR_LLM_UC_MSG_TIMEOUT_USECS,
                                    SHR_LLM_SDK_VERSION,
                                    SHR_LLM_UC_MIN_VERSION,
                                    NULL, NULL);
        if (ret != SOC_E_NONE) {
            cli_out("Error in %s(): FAILED !!!\n", FUNCTION_NAME());
            return CMD_USAGE;
        }
        llm_info->uc_num = uc;
        
        /*
         * Allocate DMA buffers
         *
         * DMA buffer will be used to send and receive 'long' messages
         * between SDK Host and uController (BTE).
         */
        size = shr_max_buffer_get();
        

        llm_info->dma_buffer_len = size;
        llm_info->dma_buffer = soc_cm_salloc(unit, llm_info->dma_buffer_len,"LLM DMA buffer");
        if (!llm_info->dma_buffer) {
            cli_out("Error in %s(): DMA buffer allocation FAILED !!!\n", FUNCTION_NAME());
            return CMD_USAGE;
        }
        sal_memset(llm_info->dma_buffer, 0, llm_info->dma_buffer_len);

        llm_info->dmabuf_reply = soc_cm_salloc(unit, size, "LLM uC reply");
        if (!llm_info->dmabuf_reply) {
            cli_out("Error in %s(): DMA buffer allocation FAILED !!!\n", FUNCTION_NAME());
            return CMD_USAGE;
        }
        sal_memset(llm_info->dmabuf_reply, 0, size);

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (!sal_strncasecmp(function, "stop", strnlen_function)) {
        llm_info->event_thread_kill=0;/*indicates to task to stop*/
        soc_cmic_uc_msg_receive_cancel(unit, llm_info->uc_num,MOS_MSG_CLASS_LLM_EVENT); /*release the task from waiting*/
        sal_usleep(1000000);/*give it a chanse*/
        if (0 != llm_info->event_thread_kill)/*check that task cleared the bit*/
        {
            cli_out("Error in %s(): Unable to stop the tread. Indication[%d]\n", FUNCTION_NAME(),llm_info->event_thread_kill);
        }
    }

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    else if (!sal_strncasecmp(function, "start", strnlen_function)) {

        parse_table_t  pt;
        cmd_result_t rv = CMD_OK;
        shr_llm_msg_ctrl_init_t msg;

        /* Set control message data */
        sal_memset(&msg, 0x0, sizeof(msg));
        msg.direct_table_bank = 2;
        msg.eedb_pointer_database_size = 8192;
        msg.eedb_pointer_database_offset = 8192;
        if (ARG_CNT(a) > 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "DspEthType", PQ_INT, (void *) 0, &(msg.dsp_eth_type),  NULL);
            parse_table_add(&pt, "VoqMapMode", PQ_INT, (void *) 0, &(msg.voq_mapping_mode),  NULL);
            parse_table_add(&pt, "DirTableBank", PQ_INT, (void *) 2, &(msg.direct_table_bank),  NULL);
            parse_table_add(&pt, "DirTableOffset", PQ_INT, (void *) 0, &(msg.direct_table_offset),  NULL);
            parse_table_add(&pt, "EEDBFirstIndex", PQ_INT, (void *) 8192, &(msg.eedb_pointer_database_offset),  NULL);
            parse_table_add(&pt, "EEDBSize", PQ_INT, (void *) 8192, &(msg.eedb_pointer_database_size),  NULL);
            if (!parseEndOk(a, &pt, &rv)) {
                if (CMD_OK != rv) {
                    return CMD_USAGE;
                }
            }
        }

        /* Pack control message data into DMA buffer */
        buffer     = llm_info->dma_buffer;
        buffer_ptr = shr_llm_msg_ctrl_init_pack(buffer, &msg);
        buffer_len = sizeof(shr_llm_msg_ctrl_init_t);

        /* Send LMM Start message to uC */
        ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_INIT, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_INIT_REPLY, &reply_len);
        if (ret != SOC_E_NONE) {
            cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
            return CMD_USAGE;
        }

        llm_info->event_thread_kill = 0;
        if (llm_info->event_thread_id == NULL) {
            if (sal_thread_create("bcmLLM", SAL_THREAD_STKSZ, 
                                  SHR_LLM_EVENT_THREAD_PRIORTY,
                                  (void (*)(void*))_bcm_llm_callback_thread,
                                  INT_TO_PTR(unit)) == SAL_THREAD_ERROR) {
                if (SAL_BOOT_QUICKTURN) {
                    /* If emulation, then wait */
                    sal_usleep(1000000);
                }

                if (llm_info->event_thread_id == NULL) {
                    cli_out("Error in %s(): _bcm_llm_callback_thread() FAILED !!!\n", FUNCTION_NAME());
                    return BCM_E_MEMORY;
                }
            }
        }
        

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (!sal_strncasecmp(function, "pon_att", strnlen_function)) {

        option = ARG_GET(a);
        if (! option ) {
            return CMD_USAGE;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(option, "service",strnlen_option)) {

            parse_table_t  pt;
            cmd_result_t rv = CMD_OK;
            shr_llm_msg_pon_att_enable_t msg;
            uint8  limit_enable = 0;
            uint8  read_reply_fifo = 0;

            /* Set control message data */
            sal_memset(&msg, 0x0, sizeof(msg));
            if (ARG_CNT(a) > 0) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "read_reply_fifo", PQ_INT, (void *) 0, &(read_reply_fifo),  NULL);
                parse_table_add(&pt, "limit_enable", PQ_INT, (void *) 1, &(limit_enable),  NULL);
                parse_table_add(&pt, "type", PQ_INT, (void *) 1, &(msg.type_of_service),  NULL);
                if (!parseEndOk(a, &pt, &rv)) {
                    if (CMD_OK != rv) {
                        return CMD_USAGE;
                    }
                }
            }
            soc_sand_bitstream_set_field(&msg.att_val, 
                                         SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_LSB,
                                         SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_NOF_BITS,
                                         SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_GLOBAL);
            soc_sand_bitstream_set(&msg.att_val, 
                                   SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_BIT,
                                   limit_enable);
            soc_sand_bitstream_set(&msg.att_val, 
                                   SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_MASK,
                                   TRUE);
            soc_sand_bitstream_set(&msg.att_val, 
                                   SHR_LLM_MSG_PON_ATT_REPLY_FIFO_ENABLE_BIT,
                                   read_reply_fifo);
            soc_sand_bitstream_set(&msg.att_val, 
                                   SHR_LLM_MSG_PON_ATT_REPLY_FIFO_ENABLE_MASK,
                                   TRUE);

            /* Pack control message data into DMA buffer */
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pon_att_enable_pack(buffer, &msg);
            buffer_len = sizeof(shr_llm_msg_pon_att_enable_t);

            /* Send LMM Start message to uC */
            ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_SERVICE_ENABLE, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_PON_ATT_SERVICE_ENABLE_REPLY, &reply_len);
            if (ret != SOC_E_NONE) {
                cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
                return CMD_USAGE;
            }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (!sal_strncasecmp(option, "mac_bitmap_set", strnlen_option)) {

            parse_table_t  pt;
            cmd_result_t rv = CMD_OK;
            shr_llm_msg_pon_att_t msg;

            /* Set control message data */
            sal_memset(&msg, 0x0, sizeof(msg));
            if (ARG_CNT(a) > 0) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "port", PQ_INT, (void *) 0, &(msg.port),  NULL);
                parse_table_add(&pt, "tunnel", PQ_INT, (void *) 0, &(msg.tunnel),  NULL);
                parse_table_add(&pt, "tunnel_count", PQ_INT, (void *) 1, &(msg.tunnel_count),  NULL);
                parse_table_add(&pt, "bitmap", PQ_INT, (void *) 0, &(msg.bitmap),  NULL);
                parse_table_add(&pt, "type", PQ_INT, (void *) 1, &(msg.type_of_service),  NULL);
                if (!parseEndOk(a, &pt, &rv)) {
                    if (CMD_OK != rv) {
                        return CMD_USAGE;
                    }
                }
            }

            /* Pack control message data into DMA buffer */
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pon_att_pack(buffer, &msg);
            buffer_len = sizeof(shr_llm_msg_pon_att_t);

            /* Send LMM Start message to uC */
            ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_SET, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_SET_REPLY, &reply_len);
            if (ret != SOC_E_NONE) {
                cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
                return CMD_USAGE;
            }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        }else if (!sal_strncasecmp(option, "mac_bitmap_increase", strnlen_option)) {
        
          parse_table_t  pt;
          cmd_result_t rv = CMD_OK;
          shr_llm_msg_pon_att_t msg;

          /* Set control message data */
          sal_memset(&msg, 0x0, sizeof(msg));
          if (ARG_CNT(a) > 0) {
              parse_table_init(unit, &pt);
              parse_table_add(&pt, "port", PQ_INT, (void *) 0, &(msg.port),  NULL);
              parse_table_add(&pt, "tunnel", PQ_INT, (void *) 0, &(msg.tunnel),  NULL);
              parse_table_add(&pt, "tunnel_count", PQ_INT, (void *) 1, &(msg.tunnel_count),  NULL);
              parse_table_add(&pt, "bitmap", PQ_INT, (void *) 0, &(msg.bitmap),  NULL);
              parse_table_add(&pt, "type", PQ_INT, (void *) 1, &(msg.type_of_service),  NULL);
              if (!parseEndOk(a, &pt, &rv)) {
                  if (CMD_OK != rv) {
                      return CMD_USAGE;
                  }
              }
          }

          /* Pack control message data into DMA buffer */
          buffer     = llm_info->dma_buffer;
          buffer_ptr = shr_llm_msg_pon_att_pack(buffer, &msg);
          buffer_len = sizeof(shr_llm_msg_pon_att_t);

          /* Send LMM Start message to uC */
          ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_INCREASE, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_INCREASE_REPLY, &reply_len);
          if (ret != SOC_E_NONE) {
              cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
              return CMD_USAGE;
          }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (!sal_strncasecmp(option, "mac_bitmap_decrease", strnlen_option)) {
        
          parse_table_t  pt;
          cmd_result_t rv = CMD_OK;
          shr_llm_msg_pon_att_t msg;
      
          /* Set control message data */
          sal_memset(&msg, 0x0, sizeof(msg));
          if (ARG_CNT(a) > 0) {
              parse_table_init(unit, &pt);
              parse_table_add(&pt, "port", PQ_INT, (void *) 0, &(msg.port),  NULL);
              parse_table_add(&pt, "tunnel", PQ_INT, (void *) 0, &(msg.tunnel),  NULL);
              parse_table_add(&pt, "tunnel_count", PQ_INT, (void *) 1, &(msg.tunnel_count),  NULL);
              parse_table_add(&pt, "bitmap", PQ_INT, (void *) 0, &(msg.bitmap),  NULL);
              parse_table_add(&pt, "type", PQ_INT, (void *) 1, &(msg.type_of_service),  NULL);
              if (!parseEndOk(a, &pt, &rv)) {
                  if (CMD_OK != rv) {
                      return CMD_USAGE;
                  }
              }
          }
      
          /* Pack control message data into DMA buffer */
          buffer     = llm_info->dma_buffer;
          buffer_ptr = shr_llm_msg_pon_att_pack(buffer, &msg);
          buffer_len = sizeof(shr_llm_msg_pon_att_t);
      
          /* Send LMM Start message to uC */
          ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_DECREASE, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_DECREASE_REPLY, &reply_len);
          if (ret != SOC_E_NONE) {
              cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
              return CMD_USAGE;
          }
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      } else if (!sal_strncasecmp(option, "mac_bitmap_get", strnlen_option)) {

            parse_table_t  pt;
            cmd_result_t rv = CMD_OK;
            shr_llm_msg_pon_att_t msg;

            /* Set control message data */
            sal_memset(&msg, 0x0, sizeof(msg));
            if (ARG_CNT(a) > 0) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "port", PQ_INT, (void *) 0, &(msg.port),  NULL);
                parse_table_add(&pt, "tunnel", PQ_INT, (void *) 0, &(msg.tunnel),  NULL);
                if (!parseEndOk(a, &pt, &rv)) {
                    if (CMD_OK != rv) {
                        return CMD_USAGE;
                    }
                }
            }

            /* Pack control message data into DMA buffer */
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pon_att_pack(buffer, &msg);
            buffer_len = sizeof(shr_llm_msg_pon_att_t);

            /* Send LMM Start message to uC */
            ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_GET, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_GET_REPLY, &reply_len);
            if (ret != SOC_E_NONE) {
                cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
                return CMD_USAGE;
            }

            /* Pack control message data into DMA buffer */
            sal_memset(&msg, 0x0, sizeof(msg));
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pon_att_unpack(buffer, &msg);
            buffer_len = buffer_ptr - buffer;
            cli_out("%s(): PON_ATT_MAC_BITMAP_GET(). msg.port=%d, msg.tunnel=%d, msg.bitmap=0x%x msg.type=0x%x\n", FUNCTION_NAME(), msg.port, msg.tunnel, msg.bitmap, msg.type_of_service);

        } else {
            return CMD_USAGE;
        }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (!sal_strncasecmp(function, "info", strnlen_function)) {
        option = ARG_GET(a);
        if (! option ) {
            return CMD_USAGE;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(option, "appl_info", strnlen_option)) {
            shr_llm_msg_app_info_get_t msg;

            /* Set control message data */
            sal_memset(&msg, 0x0, sizeof(msg));

            /* Pack control message data into DMA buffer */
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_app_info_get_pack(buffer, &msg);
            buffer_len = sizeof(shr_llm_msg_pon_att_t);

            /* Send LMM Start message to uC */
            ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_APP_INFO_GET, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_APP_INFO_GET_REPLY, &reply_len);
            if (ret != SOC_E_NONE) {
                cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
                return CMD_USAGE;
            }

            /* Pack control message data into DMA buffer */
            sal_memset(&msg, 0x0, sizeof(msg));
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_app_info_get_unpack(buffer, &msg);
            buffer_len = buffer_ptr - buffer;
            cli_out("%s(): Total: counterEvents=%d, Learn=%d, Delete=%d, Transp=%d, Unknown=%d\n", 
                    FUNCTION_NAME(), msg.counterEvents , msg.counterEventsLearn, msg.counterEventsDelete, msg.counterEventsTransp, msg.counterEventsUnKnown);
            cli_out("%s(): Success: Learn=%d, Delete=%d, Transp=%d,\n", 
                    FUNCTION_NAME(), msg.suc_counterEventsLearn, msg.suc_counterEventsDelete, msg.suc_counterEventsTransp);
            cli_out("%s(): num_of_activation=%d, time_of_activation=0x%x\n", 
                    FUNCTION_NAME(), msg.num_of_activation, msg.time_of_activation);

        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        else if (!sal_strncasecmp(option, "pon_id_info" , strnlen_option)) 
        {
            shr_llm_PON_ID_attributes_t msg;
            parse_table_t  pt;
            cmd_result_t rv = CMD_OK;
            uint16 port;

            /* Set control message data */
            sal_memset(&msg, 0x0, sizeof(msg));
            if (ARG_CNT(a) > 0) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "port", PQ_INT, (void *) 0, &(msg.port),  NULL);
                if (!parseEndOk(a, &pt, &rv)) {
                    if (CMD_OK != rv) {
                        return CMD_USAGE;
                    }
                }
            }
            port=msg.port;
            /* Pack control message data into DMA buffer */
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pon_db_pack(buffer, &msg);
            buffer_len = sizeof(msg)-1;

            /* Send LMM Start message to uC */
            ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ID_DB_GET, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_PON_ID_DB_GET_REPLY, &reply_len);
            if (ret != SOC_E_NONE) {
                cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED !!!\n", FUNCTION_NAME());
                return CMD_USAGE;
            }

            /* Pack control message data into DMA buffer */
            sal_memset(&msg, 0x0, sizeof(msg));
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pon_db_unpack(buffer, &msg);
            buffer_len = buffer_ptr - buffer;
            {
                int i;
                cli_out("|Port | Tunnel ID | PON ID | Thr/bitmap |Srv|Rprt|\n");
                cli_out("+-----+-----------+--------+------------+---+----+\n");    
                for (i=0;i<SHR_LLM_MAX_TUNNEL_INDEX;i++)
                {
                    if (0 == msg.entries[i])
                    {
                        continue;
                    }
                    /*typedef union {
   struct {
       uint16_t word1;
       uint8_t  word2;       
   };
   struct {
       union {
           uint16_t  status;                
           uint16_t  LLID_MAC_indx_bitmap;  
       };
       uint8_t   Limit_Reported:1;       
       uint8_t   type:2;      
       uint8_t   reserved:5;       
   };
} llm_PON_ID_attributes_t;
*/                  if (((msg.entries[i]>>17) & 0x3)==2)  
                    {                                   /*HEX for VMAC*/
                    cli_out("|%5d|   %04d    | 0x%04x | 0x%04x     | %d | %s  |\n",
                            port,/*port*/
                            i,/*tunnnel id*/
                            ((port ) << 11) | (i),/*pon id*/
                            msg.entries[i] & 0xffff,/*bitmap*/
                            (msg.entries[i]>>17) & 0x3 ,/*type Vmac/MACT limit*/
                            1==((msg.entries[i]>>16) & 0x1) ?"V":" ");  /*is limit reached reported*/  
                    }
                    else
                    {                                   /*DEC  for MACT*/
                        cli_out("|%5d|   %04d    | 0x%04x | %06d     | %d | %s  |\n",
                                port,/*port*/
                                i,/*tunnnel id*/
                                ((port ) << 11) | (i),/*pon id*/
                                msg.entries[i] & 0xffff,/*bitmap*/
                                (msg.entries[i]>>17) & 0x3 ,/*type Vmac/MACT limit*/
                                1==((msg.entries[i]>>16) & 0x1) ?"V":" ");  /*is limit reached reported*/  
                    }



                    cli_out("+-----+-----------+--------+------------+---+----+\n");    
                }
            }

            /*cli_out("%s(): Total: counterEvents=%d, Learn=%d, Delete=%d, Transp=%d, Unknown=%d\n", 
                      FUNCTION_NAME(), msg.counterEvents , msg.counterEventsLearn, msg.counterEventsDelete, msg.counterEventsTransp, msg.counterEventsUnKnown);
            cli_out("%s(): Success: Learn=%d, Delete=%d, Transp=%d,\n", 
                    FUNCTION_NAME(), msg.suc_counterEventsLearn, msg.suc_counterEventsDelete, msg.suc_counterEventsTransp);
            cli_out("%s(): num_of_activation=%d, time_of_activation=0x%x\n", 
                    FUNCTION_NAME(), msg.num_of_activation, msg.time_of_activation);*/

        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        else if (!sal_strncasecmp(option, "eedb_info", strnlen_option)) 
        {
            shr_llm_pointer_pool_t msg;

            /* Set control message data */
            sal_memset(&msg, 0x0, sizeof(msg));

            /* Pack control message data into DMA buffer */
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pointer_pool_pack(buffer, &msg);
            buffer_len = sizeof(msg);

            /* Send LMM Start message to uC */
            ret = _bcm_en_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_POINTER_DB_INFO_GET, buffer_len, 0, MOS_MSG_SUBCLASS_LLM_POINTER_DB_INFO_GET_REPLY, &reply_len);
            if (ret != SOC_E_NONE) {
                cli_out("Error in %s(): _bcm_en_llm_msg_send_receive() FAILED %d!!!\n", FUNCTION_NAME(),ret);
                return CMD_USAGE;
            }

            /* Pack control message data into DMA buffer */
            sal_memset(&msg, 0x0, sizeof(msg));
            buffer     = llm_info->dma_buffer;
            buffer_ptr = shr_llm_msg_pointer_pool_unpack(buffer, &msg);
            buffer_len = buffer_ptr - buffer;
            {
                int i;
                cli_out("Size                %d\n",msg.size);
                cli_out("Reserved            %d\n",msg.numOfReservedIndexes);
                cli_out("Free                %d\n",msg.numOfFreeIndexes);
                cli_out("First index offset  %d\n",msg.firstIndexOffset);
                cli_out("Recent entry index  %d\n",msg.recentEntry);
                cli_out("Recent entry value  0x%08x\n",msg.entries[msg.recentEntry]);
                cli_out("Line/Row  |");
                for (i=0;i<16;i++)
                {
                    cli_out("   %x    |",i);
                }
                for(i=0;i<SHR_LLM_PULL_MAX_SIZE;i++)
                {
                    if (i%16 == 0)
                    {
                        cli_out("\n    %03d   |",i>>4);
                    }
                    if (msg.entries[i]==0)
                    {
                        cli_out("---0----|");
                    }
                    else
                    {
                        cli_out("%08x|",msg.entries[i]);
                    }
                }
                cli_out("\n");

            }

        }

    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif /* BCM_CMICM_SUPPORT */

#endif

#endif /* BCM_PETRA_SUPPORT */


char cmd_avs_usage[] = {
    "AVS value - Adjustable Voltage Scaling .\n"
    "Usage:\n"
    "\tAVS read - read the AVS value\n"
};

cmd_result_t
    cmd_avs(int unit, args_t* a)
{
    uint32 avs_val;
    char* func = ARG_GET(a);

    if (func == NULL) {    /* Nothing to do    */
        return CMD_USAGE;      /* Print usage line */
    }

    if(!sal_strncasecmp(func, "read", sal_strnlen(func,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {

        if (SOC_IS_DFE(unit)||SOC_IS_DPP(unit)) {
            if(bcm_switch_rov_get(unit, 0, &avs_val) != 0) {
                return CMD_FAIL;
            }
        }
        else
        {
            cli_out("Unit doesn't support AVS reading\n");
            return CMD_FAIL;
        }

    } else {
         return CMD_USAGE;      /* Print usage line */
    }

    cli_out("AVS val = 0x%x\n", avs_val);

    return CMD_OK;
}



