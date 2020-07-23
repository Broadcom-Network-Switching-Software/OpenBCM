/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
*  This CINT is meant to provide an example of building a FAP scheduling scheme in 8 priorities mode.
*  that is connect VOQ connectors to the appropriate HR according to the HRs TC.
*
* Creating Schedling Scheme done by the following steps: 
* 1. Egress: Create for each local port: following FQs, VOQ connectors. 
* 2. Ingress: Create VOQs for each system port. 
* 3. Connect Ingress VOQs <=> Egress VOQ connectors. 
* Pay attention the scheduling scheme assumes static IDs for VOQ connectors,VOQs. 
* This is depended by the local and system ports in the system.
* Conversion is done static using utils functions. 
* 
* For this example to run you must bring up the FAP device with the following SOC properties:
* diag_cosq_disable=1
*/ 
cint_reset();

int MAX_COS = 8;
int MAX_NUM_PORTS = 256;
int MAX_NUM_DEVICE = 8;
int MAX_MODIDS_PER_DEVICE = 2;
int MAX_NUM_REMOTE_MODIDS = 2;
struct info_t {
    int         my_modid;
    /* Number of devices in the system */
    int         nof_devices;
    /* Number of physical ports per device */
    int         num_ports;
    /* Number of internal ports (OLP,ERP) per device */
    int         num_internal_ports;
    /* Number of cos levels (COSQ) */
    int         num_cos;
    /* CPU system port */
    int         cpu_sys_port;
    int         internal_ports[MAX_NUM_DEVICE*MAX_NUM_PORTS];
    int         offset_start_of_internal_ports;
    /* Offset given for a system port on each device */
    int         offset_start_of_sys_port;
    /* Offset given for a voq */
    int         offset_start_of_voq;
    /* Offset given for a voq connector */    
    int         offset_start_of_voq_connector;
    /* Present local ports on each device */
    bcm_gport_t dest_local_gport[MAX_NUM_PORTS];    
    /* 
     * Present all VOQ connectors on egress device 
     * For each dest local port create 2 voq connectors, for each device 
     */
    bcm_gport_t gport_ucast_voq_connector_group[MAX_NUM_DEVICE*MAX_NUM_PORTS];
    /* 
     * Present all VOQ on ingress device, 
     * For each system port create VOQ 
     */
    bcm_gport_t gport_ucast_queue_group[MAX_NUM_DEVICE*MAX_NUM_PORTS];
    /* Present all FQ scheduling on egress device, one for each local port */
    bcm_gport_t gport_ucast_scheduler[MAX_NUM_PORTS*MAX_COS];    
    
    /* If true, a single CPU control two units, and no need to allocate resources with ID.
       Otherwise, this script runs on two CPUs, and we allocate with-ID in order to know how
       to bind ingress to egress resources. */
    int central_cpu;
    
};
info_t g_info; /* global info */

/* 
 * Utils function: Converting between IDs
 */
/* return the index for the internal port stored in the internal_ports[] array */
int
port_internal_id_get(int modid_idx, int port)
{
    int port_i;
    for (port_i=0; port_i<g_info.num_internal_ports; port_i++) {
        if (g_info.internal_ports[modid_idx*MAX_NUM_PORTS+port_i] == port) {
            return port_i;
        }
    }
    return -1;
}

/* 
 * Utils function: Converting bewtween IDs
 */
/* Convert (module id,port) id to sysport id */
int
convert_modport_to_sysport_id(int modid_idx,int port)
{
    /* ITMH cannot be sent to system port 0, so we're using system port
               100 instead (for CPU) */
    int internal_port_idx;
    if (port == 0) {
        port = g_info.cpu_sys_port;
    }
    internal_port_idx = port_internal_id_get(modid_idx, port);
    if (internal_port_idx != -1) {
        return (g_info.offset_start_of_sys_port * modid_idx + internal_port_idx
                + g_info.offset_start_of_internal_ports);
    }
    return g_info.offset_start_of_sys_port * modid_idx + port;
}

/* 
 * Convert sysport to base VOQ id 
 * The conversation includes: 
 * VOQ ID = offset + (sysport-1)*num_cos. 
 * offset specify the start of the VOQ range (right after the FMQ range) by default 4.
 */
int
convert_sysport_id_to_base_voq_id(int sysport)
{
    /* Assume, no system port 0 */
    if (sysport == 0) {
        return -1;
    }
    return g_info.offset_start_of_voq + (sysport-1)*g_info.num_cos;
}

/* 
 * Convert (local port,ingress_modid) to VOQ connector base 
 * VOQ connector ID  = offset + egress_local_port * nof_devices * max_cos + ingress_modid*max_cos
 * specify offset start of the VOQ connector ID, by default 32. 
 * max_cos is 8
 */

int
convert_modport_to_base_voq_connector(int local_port,int ingress_modid)
{
    /* In case of internal ports (higher than 255) have a port of max + (local_port - 255) */
    if (local_port > 255) {
        local_port = 256 + (local_port % 256);
    }
    return g_info.offset_start_of_voq_connector + local_port*g_info.nof_devices*MAX_MODIDS_PER_DEVICE*MAX_COS
        + ingress_modid*MAX_COS;
}

/* 
 * Purpose: Initializes Looks at current configuration and populates
 *          g_info based on that
 */
int
global_info_init (int unit, int nof_devices)
{
    int         rv = BCM_E_NONE;
    int         port_i = 0;
    int         modid_idx = 0;
    int         idx=0;
    
    bcm_port_config_t port_config;
    bcm_pbmp_t pbmp;
    int is_dnx;

    g_info.num_cos = MAX_COS;
    g_info.nof_devices = nof_devices;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    /*set num_ports in stk_init*/
    BCM_PBMP_CLEAR(pbmp);

    if (is_dnx) {
        int max_nof_fap_ids_per_device = *(dnxc_data_get(unit, "device", "general", "max_nof_fap_ids_per_device", NULL));
        MAX_MODIDS_PER_DEVICE = max_nof_fap_ids_per_device > 2 ? max_nof_fap_ids_per_device : 2;
        rv = appl_dnx_sys_device_ports_get(unit, &pbmp);
        if (rv != BCM_E_NONE) {
          printf("Error, in appl_dnx_sys_device_ports_get $rv \n");
          return rv;
        }
    } else {
        rv = bcm_port_config_get(unit, &port_config);
        if (rv != BCM_E_NONE) {
          printf("Error, in bcm_port_config_get $rv \n");
          return rv;
        }
        BCM_PBMP_OR(pbmp, port_config.all);
        BCM_PBMP_REMOVE(pbmp, port_config.sfi);
        BCM_PBMP_REMOVE(pbmp, port_config.olp);
        BCM_PBMP_REMOVE(pbmp, port_config.oamp);
        BCM_PBMP_REMOVE(pbmp, port_config.erp);
        BCM_PBMP_REMOVE(pbmp, port_config.rcy_mirror);
    }

    BCM_PBMP_COUNT(pbmp, g_info.num_ports);

    BCM_PBMP_ITER(pbmp, port_i) {
        BCM_GPORT_LOCAL_SET(g_info.dest_local_gport[idx],port_i);
        idx++;
    }
    g_info.cpu_sys_port = 255;
    /* Offset sysport per device */
    g_info.offset_start_of_sys_port = 256;
    /* Offset VOQ */
    g_info.offset_start_of_voq = 32;
    /* Offset VOQ connector */
    g_info.offset_start_of_voq_connector = 32;

    /* on local device internal ports will start at 240 */
    g_info.offset_start_of_internal_ports = 240;
    g_info.num_internal_ports = 0;

    for (modid_idx=0; modid_idx < nof_devices; modid_idx++) {
        for (port_i=0; port_i<MAX_NUM_PORTS; port_i++) {
            g_info.internal_ports[modid_idx*MAX_NUM_PORTS+port_i] = -1;
        }
    }
    g_info.central_cpu = 1;

    return rv;
}

/* This function create the scheduling scheme */
int
cosq_init_ing_egr_alloc(int unit) {
    int                         rv = BCM_E_NONE;
    int                         idx, cos, port, core_idx;
    uint32                      flags, dummy_flags;
    int queue_range = 0, sysport;
    int modid_idx, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t sysport_gport,modport_gport, e2e_gport;
    int int_flags, tm_port;
    int local_erp_port[2] = {-1};
    int num_erp_ports = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    bcm_cosq_voq_connector_gport_t config;
    int is_dnx;

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
    queue_range = 0;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    /** Min FMQ is always 0 for Jr2 */
    if (!is_dnx) {
        rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMin,queue_range);
        if (BCM_FAILURE(rv)) {
            printf("bcm_fabric_control_set queue_id :%d failed. Error:%d (%s)\n", queue_range, rv, bcm_errmsg(rv));
            return rv;
        }
    }

    queue_range = 3;
    rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMax,queue_range);
    if (BCM_FAILURE(rv)) {
        printf("bcm_fabric_control_set queue_id :%d failed. Error:%d (%s)\n", queue_range, rv, bcm_errmsg(rv));
        return rv;
    }
	/* Get ERP port */
    int_flags =
    BCM_PORT_INTERNAL_EGRESS_REPLICATION;
    rv = bcm_port_internal_get(unit, int_flags, MAX_NUM_PORTS,
                   local_erp_port, &num_erp_ports);
    if (BCM_FAILURE(rv)) {
	    printf("bcm_port_internal_get failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
	    return rv;
    }

    /*
     * Creating Schedling Scheme
     * This is done by the following steps:
     * 1. Egress: Create for each local port: following FQs, VOQ connectors.
     * 2. Ingress: Create VOQs for each system port.
     * 3. Connect Ingress VOQs <=> Egress VOQ connectors.
     * Pay attention the scheduling scheme assumes static IDs for VOQ connectors,VOQs.
     * This is depended by the local and system ports in the system.
     * Conversion is done static using utils functions.
     */
    /* Stage I: Egress Create FQs */
    for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
        /* Create FQ per traffic class i, attach it to HR SPi */
        for (cos = 0; cos < g_info.num_cos; cos++)
        {
            /* Replace HR mode to enhance */
            flags = (BCM_COSQ_GPORT_SCHEDULER |
                    BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED |
                     BCM_COSQ_GPORT_REPLACE);

            /* e2e gport */
            BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]));
            rv = bcm_cosq_gport_add(unit, g_info.dest_local_gport[idx], 1, flags,
                                    &e2e_gport);

            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add replace hr scheduler idx:%d failed. Error:%d (%s)\n", idx, rv, bcm_errmsg(rv));
                return rv;
            }
            flags = (BCM_COSQ_GPORT_SCHEDULER |
                BCM_COSQ_GPORT_SCHEDULER_FQ);

            rv = bcm_cosq_gport_add(unit, e2e_gport, 1, flags,
                                    &g_info.gport_ucast_scheduler[idx*MAX_COS+cos]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add fq scheduler idx:%d cos: %d failed. Error:%d (%s)\n", idx, cos, rv, bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_cosq_gport_sched_set(unit,
                         g_info.gport_ucast_scheduler[idx*MAX_COS+cos], 0,
                         BCM_COSQ_SP0 + cos,0);


            /* attach hr scheduler to fq */
            rv = bcm_cosq_gport_attach(unit, e2e_gport,
                             g_info.gport_ucast_scheduler[idx*MAX_COS+cos], 0);

            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_attach hr scheduler-fq idx:%d cos:%d failed. Error:%d (%s)\n",
                         idx, cos, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /* Stage I: Egress Create VOQ connectors */
    for (modid_idx = 0; modid_idx < g_info.nof_devices; modid_idx++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            /* create voq connector - with ID */
            flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;

            port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);

            /* Skip ERP port if not far end on ingress */
            for(core_idx = 0 ; core_idx < num_erp_ports ; core_idx++) {
                if ((local_erp_port[core_idx] == port) && (modid_idx == g_info.my_modid)) {
                    continue;
                }
            }

            rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
            if (BCM_FAILURE(rv)) {
                printf("bcm_port_get port %d failed. Error %d (%s)\n", port, rv, bcm_errmsg(rv));
                return rv;
            }

            /* If both devices are control by a central CPU, no need to allocate with ID,
               since we use the ID allocated by the BCM */
            if (!g_info.central_cpu) {
                flags |= BCM_COSQ_GPORT_WITH_ID;
                voq_connector_id  = convert_modport_to_base_voq_connector(port,modid_idx*MAX_MODIDS_PER_DEVICE);
                BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(g_info.gport_ucast_voq_connector_group[modid_idx*MAX_NUM_PORTS+idx],voq_connector_id, mapping_info.core);
            }

            config.flags = flags;
            config.nof_remote_cores = MAX_NUM_REMOTE_MODIDS;
            config.numq = g_info.num_cos;
            config.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE;
            BCM_COSQ_GPORT_E2E_PORT_SET(config.port, port);
            rv = bcm_cosq_voq_connector_gport_add(unit, &config, &g_info.gport_ucast_voq_connector_group[modid_idx*MAX_NUM_PORTS+idx]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_voq_connector_gport_add connector idx:%d failed. Error %d (%s)\n", idx, rv, bcm_errmsg(rv));
                return rv;
            }

            for (cos = 0; cos < g_info.num_cos; cos++) {
                /* Each VOQ connector attach suitable FQ traffic class */
                rv = bcm_cosq_gport_sched_set(unit,
                             g_info.gport_ucast_voq_connector_group[modid_idx*MAX_NUM_PORTS+idx], cos,
                             BCM_COSQ_SP0,0);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_sched_set connector idx:%d cos:%d failed. Error:%d(%s)\n",
                             idx, cos, rv, bcm_errmsg(rv));
                    return rv;
                }
                /* attach fq scheduler to connecter */
                rv = bcm_cosq_gport_attach(unit, g_info.gport_ucast_scheduler[idx*MAX_COS+cos],
                                 g_info.gport_ucast_voq_connector_group[modid_idx*MAX_NUM_PORTS+idx], cos);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_attach fq scheduler-connector idx:%d cos:%d failed. Error:%d (%s)\n",
                             idx, cos, rv, bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

    /* Stage 2: Ingress: Create VOQs for each system port. */
    for (modid_idx = 0; modid_idx < g_info.nof_devices; modid_idx++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
             port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);
             /* Skip ERP port if not far end on ingress */
             if ((local_erp_port[0] == port) && (modid_idx == g_info.my_modid)) {
                 continue;
             }



            rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
            if (BCM_FAILURE(rv)) {
                printf("bcm_port_get port %d failed. Error %d (%s)\n", port, rv, bcm_errmsg(rv));
                return rv;
            }
            tm_port = mapping_info.tm_port;
            BCM_GPORT_MODPORT_SET(modport_gport, modid_idx * MAX_MODIDS_PER_DEVICE + mapping_info.core, tm_port);



            if (!g_info.central_cpu) {
                flags |= BCM_COSQ_GPORT_WITH_ID;

                rv = bcm_stk_gport_sysport_get(unit,modport_gport,&sysport_gport);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_stk_gport_sysport_get get sys port failed. Error:%d (%s) \n",
                             rv, bcm_errmsg(rv));
                   return rv;
                }

                sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
                voq_base_id = convert_sysport_id_to_base_voq_id(sysport_id);

                BCM_GPORT_UNICAST_QUEUE_GROUP_SET(g_info.gport_ucast_queue_group[modid_idx*MAX_NUM_PORTS+idx],voq_base_id);
            }

            if (is_dnx) {
                rv = appl_dnx_logical_port_to_sysport_get(unit, modid_idx * MAX_MODIDS_PER_DEVICE, tm_port, &sysport);
                if (BCM_FAILURE(rv)) {
                    printf("appl_dnx_logical_port_to_sysport_get failed. Error:%d (%s) \n",
                             rv, bcm_errmsg(rv));
                   return rv;
                }
            } else {
                sysport = tm_port + modid_idx * 256;
            }

            /** Creating VOQs only with system port gport is mandatory for JR2 (JR1 also supports it) */
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport);
            rv = bcm_cosq_gport_add(unit, sysport_gport, g_info.num_cos,
                                    flags, &g_info.gport_ucast_queue_group[modid_idx*MAX_NUM_PORTS+idx]);

            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add UC queue failed. Error:%d (%s) \n",
                         rv, bcm_errmsg(rv));
               return rv;
            }
        }
    }
    
    return rv;
}

int
cosq_init_ing_egr_connect(int unit, int mymodid_idx) {
    int                         rv = BCM_E_NONE;
    int                         idx, port, core_idx;
    bcm_cosq_gport_connection_t connection;
    int modid_idx, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t voq_connector_gport,sysport_gport,modport_gport, voq_base_gport;    
	int local_erp_port[2] = {-1},num_erp_ports;
    int int_flags;
    int cos;

    /* Get ERP port */
    int_flags =
    BCM_PORT_INTERNAL_EGRESS_REPLICATION;
    rv = bcm_port_internal_get(unit, int_flags, MAX_NUM_PORTS,
                   local_erp_port, &num_erp_ports);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_internal_get failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Stage 3: Connect Ingress VOQs <=> Egress VOQ connectors. */
    /* Ingress: connect voqs to voq connectors */
    for (modid_idx = 0; modid_idx < g_info.nof_devices; modid_idx++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
            connection.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE;
            connection.voq = g_info.gport_ucast_queue_group[modid_idx*MAX_NUM_PORTS+idx];
            port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);

            /* Skip ERP port if not far end on ingress */
            for(core_idx = 0 ; core_idx < num_erp_ports ; core_idx++) {
                if ((local_erp_port[core_idx] == port) && (modid_idx == g_info.my_modid)) {
                    continue;
                }
            }
            connection.voq_connector = g_info.gport_ucast_voq_connector_group[mymodid_idx*MAX_NUM_PORTS+idx];

            rv = bcm_cosq_gport_connection_set(unit, &connection);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_connection_set ingress modid: %d, idx:%d failed. Error:%d (%s)\n", modid_idx, idx, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }            

    /* Egress: connect voq connectors to voqs */
    for (modid_idx = 0; modid_idx < g_info.nof_devices; modid_idx++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
            connection.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE;
            connection.voq_connector = g_info.gport_ucast_voq_connector_group[modid_idx*MAX_NUM_PORTS+idx];
            connection.voq = g_info.gport_ucast_queue_group[mymodid_idx*MAX_NUM_PORTS+idx];


            port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);
            /* Skip ERP port if not far end on ingress */
            if ((local_erp_port[0] == port) && (modid_idx == g_info.my_modid)) {
                continue;
            }

            rv = bcm_cosq_gport_connection_set(unit, &connection);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_connection_set egress modid: %d, idx:%d failed. Error:%d (%s)\n", modid_idx, idx, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }       

    return rv;
}

/*
 * Purpose: Initialize basic components of a Fap decive.
 * Note:    This is intended to configure only using BCM API.
 * This is the main function of this cint example. The following steps are being done:
 * 1. Setup the diag init structure. (global_info_init)
 * 2. Run the Cosq Application (Scheduling scheme) (cosq_init)
 * Parameters:
 *  - unit:
 *  - modid_idx: Module id index.
 *  - nof_units: Number of devices in the system. cannot be above 8
 *  - unit_arr: An array with unit ids of the rest of units controlled by this CPU. 
 *  - modid_idx_arr: An array with mod id indexes of the rest of units controlled by this CPU.
 */
int
bcm_tm_fap_cosq(int unit, int modid_idx, int nof_units, int* unit_arr, int* modid_idx_arr)
{
    int rv = BCM_E_NONE;
    int cfg_2nd_unit;    
    int unit_idx;
    if ((nof_units > MAX_NUM_DEVICE)) {
      printf("Number of units (%d) is more than %d, is negative ", nof_units,MAX_NUM_DEVICE);
      return BCM_E_PARAM;
    }
    
    /* misc init should be called first */
    rv = global_info_init(unit, nof_units);
    if (BCM_FAILURE(rv)) {
        printf("global_info_init: failed. Error:%d (%s) \n", 
                 rv, bcm_errmsg(rv));
        return rv;
    }
    
    
    cfg_2nd_unit = (g_info.central_cpu && (nof_units > 1));

    /*
     * COSQ Init
     */
    
    rv = cosq_init_ing_egr_alloc(unit);
    if (BCM_FAILURE(rv)) {
        printf("cosq_init_ing_egr_alloc: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    if (cfg_2nd_unit) {
        for (unit_idx=0;unit_idx<nof_units-1;unit_idx++) {
            rv = cosq_init_ing_egr_alloc(unit_arr[unit_idx]);
            if (BCM_FAILURE(rv)) {
                printf("cosq_init_ing_egr_alloc: failed. unit: %d, Error:%d (%s) \n", unit_arr[unit_idx], rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }
    
    rv = cosq_init_ing_egr_connect(unit, modid_idx);
    if (BCM_FAILURE(rv)) {
        printf("cosq_init_ing_egr_connect: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    if (cfg_2nd_unit) {
        for (unit_idx=0;unit_idx<nof_units-1;unit_idx++) {
            rv = cosq_init_ing_egr_connect(unit_arr[unit_idx], modid_idx_arr[unit_idx]);
            if (BCM_FAILURE(rv)) {
                printf("cosq_init_ing_egr_connect: failed. unit: %d, Error:%d (%s) \n", unit_arr[unit_idx], rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }
    return rv;
}
