/*   
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This cint shows example of source based VSQs resource allocation.
 *
 * Resource allocation walkthough (Focal function: cint_vsq_resource_allocation_setup()):
 * 1) cint_gl_vsq_resource_allocate() - global VSQs resource allocation.
 * 2) cint_src_vsqs_create() - create PORT-VSQ and PG-VSQ.
 * 3) cint_src_vsqs_resource_allocate() - allocate resources for the PORT-VSQ and PG-VSQ.
 * 4) cint_src_vsqs_pfc_setup() - configure flow control for PORT & PG VSQs.
 *
 * Following functions required only for Jericho:
 * 5) cint_vsq_setup() - create VSQB.
 * 6) cint_voq_add() - assign VOQ to VSQB.
 */
struct rsrc_alloc
{
    uint32 reserved;
    uint32 shared;
    uint32 headroom;
};

enum device_type_e {
    DEVICE_TYPE_JERICHO = 0,
    DEVICE_TYPE_QAX
};
int g_device_type;

/* Basic parameters */
int NOF_CORES;
int NOF_RSRC = 3; /* relevant for QAX */
bcm_gport_t cttc_vsq[8];
int is_dnx = 0;

/**************************
 *
 *  Jeicho Functions    ***
 */

/*
 * Global VSQs resource allocation.
 * Configure for each core: reserved size, pool_0 & pool_1 sizes, headroom size.
 * Configure both DRAM-mix and OCB-only resources.
 *
 * This is done by calling bcm_cosq_resource_allocation_set() on a global VSQ.
 *
 * To see how the resources are divided, type 'diag resource_alloc' from BCM shell.
 */
int
cint_gl_vsq_resource_allocate_jer(int unit)
{
    bcm_error_t rv = 0;
    bcm_gport_t gl_vsq_gports[NOF_CORES];
    bcm_cosq_allocation_resource_t resource = bcmReservationResourceBufferDescriptors;
    bcm_cosq_allocation_entity_t target;
    bcm_cosq_resource_amounts_t amounts[2];
    uint32 flags = 0;
    uint8 pool_id = 0, is_ocb_only = 0;
    int core = -1;

    for (core = 0; core < NOF_CORES; ++core) {
        BCM_COSQ_GPORT_VSQ_GL_SET(gl_vsq_gports[core], core);
    }

    /* DRAM mix amounts */
    amounts[0].flags = 0;
    amounts[0].reserved = 40000;
    amounts[0].max_shared_pool = 100000;
    amounts[0].max_headroom = 20000;

    /* OCB only amounts */
    amounts[1].flags = 0;
    amounts[1].reserved = 3000;
    amounts[1].max_shared_pool = 14000;
    amounts[1].max_headroom = 1000;

    /* target's color and flags are not used for global resource allocation */

    /* Allocate all global resources */
    for (core = 0; core < NOF_CORES; ++core) {
        target.gport = gl_vsq_gports[core];
        for (is_ocb_only = 0; is_ocb_only < 2; ++is_ocb_only) {
            target.is_ocb_only = is_ocb_only;
            for (pool_id = 0; pool_id < 2; ++pool_id) {
                target.pool_id = pool_id;
                rv = bcm_cosq_resource_allocation_set(unit, flags, resource, target, amounts[is_ocb_only]);
                if (BCM_FAILURE(rv)) {
                    printf("cint_gl_vsq_resource_allocate_jer: bcm_cosq_resource_allocation_set failed, core=%d, is_ocb_only=%d pool_id=%d.\n", core, is_ocb_only, pool_id);
                    return(rv);
                }
            }
        }
    }

    printf("cint_gl_vsq_resource_allocate_jer: PASS\n");
    return rv;
}


/*
 * Create Port-VSQs and PG-VSQs.
 *
 * In this example we create all the PG-VSQs on pool_0 of OCB-only resources, and
 * configure them as lossles (meaning headroom will be used if required).
 *
 * Parameters:
 *  src_port (IN)           - the source port for which to create the VSQs to.
 *  src_vsq_gport (OUT)     - the resulted base PORT-VSQ gport.
 *  pg_base_vsq_gport (OUT) - the resulted base PG-VSQ gport.
 *
 * Note: The resulted gports are the base gports, i.e. several VSQ gports are configured,
 *       but the API return only the first.
 *       PORT-VSQ: The gport is DRAM-mix gport. To get OCB-only gport, use BCM_COSQ_GPORT_VSQ_SRC_PORT_SET.
 *       PG-VSQ: The gport is group 0. If, for example (like in this function), there are
 *               8 cosq configured, the result gport will be of cosq 0. To get gports of 
 *               other cosqs, use BCM_COSQ_GPORT_VSQ_PG_GET to retrieve the pg_vsq_id, add to it
 *               the required cosq (the gports are allocated consecutivly) and use this new id to get the gport by
 *               using BCM_COSQ_GPORT_VSQ_PG_SET.
 */
int
cint_src_vsqs_create_jer(int unit, bcm_port_t src_port, bcm_gport_t *src_vsq_gport, bcm_gport_t *pg_base_vsq_gport)
{
    bcm_error_t rv = 0;
    bcm_cosq_src_vsqs_gport_config_t vsq_config;
    int cosq = 0;
    int pool_id = 0;    /* to associate with pool_1, change to 1 */
    uint32 flags = 0;
    bcm_gport_t src_gport;

    sal_memset(&vsq_config, 0, sizeof(vsq_config));

    vsq_config.flags = 0;
    vsq_config.numq = BCM_COSQ_VSQ_NOF_PG;

    for (cosq = 0; cosq < vsq_config.numq; cosq++) {
        vsq_config.pg_attributes[cosq].cosq_bitmap = 0x1 << cosq;
        vsq_config.pg_attributes[cosq].pool_id = pool_id;
        vsq_config.pg_attributes[cosq].ocb_only = 1;    /* For DRAM-mix, change to 0 */
        vsq_config.pg_attributes[cosq].is_lossles = 1;  /* For lossy queue (not using headroom), change to 0 */
    }

    BCM_GPORT_LOCAL_SET(src_gport, src_port);
    rv = bcm_cosq_src_vsqs_gport_add(unit, src_gport, &vsq_config, src_vsq_gport, pg_base_vsq_gport);
    if (BCM_FAILURE(rv)) {
        printf("cint_src_vsqs_create_jer: bcm_cosq_src_vsqs_gport_add failed, src_port(%d), Error (%d, %s)\n", src_port, rv, bcm_errmsg(rv));
        return(rv);
    } else {
        printf("cint_src_vsqs_create_jer: bcm_cosq_src_vsqs_gport_add success, src_port(%d) src_vsq_gport(0x%x) pg_base_vsq_gport(0x%x)\n",
                src_port, *src_vsq_gport, *pg_base_vsq_gport);
    }

    printf("cint_src_vsqs_create_jer: PASS\n");
    return rv;
}

/*
 * Allocate resources for Port-VSQs and PG-VSQs.
 * This is done by calling bcm_cosq_resource_allocation_set() for the right VSQs.
 *
 * Parameters:
 *  numq              - number of gports in pg_base_vsq bundle.
 *  pool_id           - for which pool to allocate resources (needed for Port-VSQ. PG-VSQ's pool is configured on creation).
 *  src_vsq_gport     - base Port-VSQ gport(the gport got from bcm_cosq_src_vsqs_gport_add).
 *  pg_base_vsq_gport - base PG-VSQ gport (the gport got from bcm_cosq_src_vsqs_gport_add).
 */
int
cint_src_vsqs_resource_allocate_jer(int unit, int numq, uint8 pool_id, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{
    bcm_error_t rv;
    int index = -1;
    int offset = -1;
    uint32 flags = 0;
    bcm_gport_t vsq_gport = -1;
    int core_id = -1, pg_vsq_id = -1, pg_base_vsq_id = -1, src_port_vsq_id = -1;

    int color = -1, is_ocb_only = -1;
    bcm_cosq_allocation_entity_t target;

    uint32 reserved = 0;
    uint32 max_headroom = 0;
    uint32 max_shared_pool = 0;
    bcm_cosq_resource_amounts_t amounts;
    bcm_cosq_allocation_resource_t resource;

    src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(src_vsq_gport);

    pg_base_vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    resource = bcmReservationResourceBufferDescriptors;

    /* Parameters for src_port vsq */
    target.pool_id = pool_id;
    /* ignore target.is_ocb_only - is_ocb_only is configured in the gport */
    amounts.reserved = 14;
    amounts.max_headroom = 3;
    amounts.max_shared_pool = 8;

    /* Allocate resources for SRC_PORT VSQ */
    for (is_ocb_only = 0; is_ocb_only < 2; ++is_ocb_only) {
        /* Get the right is_ocb_only gport */
        BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(vsq_gport, core_id, is_ocb_only, src_port_vsq_id);
        target.gport = vsq_gport;
        target.is_ocb_only = is_ocb_only;

        for (color = bcmColorGreen; color < bcmColorPreserve; ++color) {
            printf("Src Port Rsrc Alloc: gport(0x%x) color(%d) reserved(0x%x) headroom(0x%x) sh_pool(0x%x)\n",
                    vsq_gport, color, reserved, max_headroom, max_shared_pool);

            target.color = color;

            rv = bcm_cosq_resource_allocation_set(unit, flags, resource, &target, &amounts);
            if (BCM_FAILURE(rv)) {
                printf("resource_allocate: bcm_cosq_resource_allocation_set failed, Error (%d, %s)\n", rv, bcm_errmsg(rv));
                return(rv);
            }
        }
    } /* for is_ocb_only */

    /* Allocate resources for PG VSQ.
     * In this example cosqs 0-6 are configured the same, with reserved resource,
     * whereas cosq 7 is configured without reserved resources.
     */
    for (offset = 0; offset < numq; offset++) {
        pg_vsq_id = pg_base_vsq_id + offset;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);

        switch (offset) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                reserved = 2;
                max_headroom = 1;
                max_shared_pool = 1;
            break;
            case 7:
                reserved = 0;
                max_headroom = 3;
                max_shared_pool = 8;
            break;
        }

        for (color = bcmColorGreen; color < bcmColorPreserve; ++color) {
            printf("PG Rsrc Alloc: gport(0x%x) color(%d) reserved(0x%x) headroom(0x%x) sh_pool(0x%x)\n",
                    vsq_gport, color, reserved, max_headroom, max_shared_pool);

            target.gport = vsq_gport;
            target.color = color;
            target.pool_id = 0;     /* target.pool_id should match how this PG-VSQ was created */
            target.is_ocb_only = 1;     /* is_ocb_only should match how this PG-VSQ was created */
            amounts.reserved = reserved;
            amounts.max_headroom = max_headroom;
            amounts.max_shared_pool = max_shared_pool;

            rv = bcm_cosq_resource_allocation_set(unit, flags, resource, &target, &amounts);
            if (BCM_FAILURE(rv)) {
                printf("resource_allocate: bcm_cosq_resource_allocation_set failed, Error (%d, %s)\n", rv, bcm_errmsg(rv));
                return(rv);
            }
        }
    } /* for offset */

    printf("cint_src_vsqs_resource_allocate_jer: PASS\n");
    return rv;
}

/* 
 * Configure VSQs Flow Control.
 *
 * Need to configure VSQs PFC, because the default thresholds are 0, 
 * which mean that for lossles VSQs the queue will use the reserved 
 * resources and when they would fill up it will start using the 
 * headroom resources, without using the pools allocation at all.
 *
 * Parameters:
 *  numq              - number of gports in pg_base_vsq bundle.
 *  src_vsq_gport     - base Port-VSQ gport(the gport got from bcm_cosq_src_vsqs_gport_add).
 *  pg_base_vsq_gport - base PG-VSQ gport (the gport got from bcm_cosq_src_vsqs_gport_add).
 */
int
cint_src_vsqs_pfc_setup_jer(int unit, int numq, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{
    bcm_error_t rv = 0;
    int offset = 0;
    bcm_cosq_pfc_config_t config;
    int pg_vsq_id = -1, pg_base_vsq_id = -1, src_port_vsq_id = -1;
    bcm_gport_t vsq_gport = -1;
    uint32 flags = 0;
    int core_id = -1, is_ocb_only = -1;

    src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(src_vsq_gport);

    pg_base_vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    printf("pfc_vsq_setup: setting up PFC config..\n");

    /* Thresholds for SRC_PORT PFC */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_threshold = 0;
    config.xon_threshold = 0;
    config.xoff_threshold_bd = 8;
    config.xon_threshold_bd = 8;
    config.drop_threshold = 0;

    for (is_ocb_only = 0; is_ocb_only < 2; ++is_ocb_only) {
        printf("pfc_vsq_setup: SRC_PORT SETUP..\n");
        BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(vsq_gport, core_id, is_ocb_only, src_port_vsq_id);

        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, 0, flags, &config);
        if (BCM_FAILURE(rv)) {
            printf("pfc_vsq_setup: pfc_config_set failed, is_ocb_only(%d) Error (%d, %s)\n", is_ocb_only, rv, bcm_errmsg(rv));
            return(rv);
        } else {
            printf("pfc_vsq_setup: pfc_config_set success, is_ocb_only(%d)\n", is_ocb_only);
        }
    }

    /* Thresholds for PG PFC */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_threshold = 0;
    config.xon_threshold = 0;
    config.xoff_threshold_bd = 8;
    config.xon_threshold_bd = 8;
    config.drop_threshold = 0;

    for (offset = 0; offset < numq; ++offset) {
        printf("pfc_vsq_setup: SRC_PORT COS-%d SETUP..\n",  offset);
        pg_vsq_id = pg_base_vsq_id + offset;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);

        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, 0, flags, &config);
        if (BCM_FAILURE(rv)) {
            printf("pfc_vsq_setup: pfc_config_set failed, cosq(%d) Error (%d, %s)\n", offset, rv, bcm_errmsg(rv));
            return(rv);
        } else {
            printf("pfc_vsq_setup: pfc_config_set success, cosq(%d)\n", offset);
        }
    }

    printf("cint_src_vsqs_pfc_setup_jer: PASS\n");
    return rv;
}

/*
 * Setting up VSQ-B for all TCs
 */
/****
Note: cint_vsq_setup() Steps 1 and 2 should be done during init
Step 3 should be done after VOQ groups are created on ingress.
Basically, step 3 will associate the unicast queue groups to relevant VSQs.
 */
int
cint_vsq_setup(int unit)
{
    bcm_error_t rv;
    int cos, ct, tc;
    bcm_cosq_vsq_info_t vsq_info;
    
    printf("cint_vsq_setup begin...\n");
    /* 1. Set vsq categories */
    rv = bcm_fabric_control_set(unit,bcmFabricVsqCategory,bcmFabricVsqCatagoryMode2);
    if (BCM_FAILURE(rv)) {
        printf("error in bcm_fabric_control_set type: bcmFabricVsqCategory\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_fabric_control_set(unit,bcmFabricQueueMin,0);
    if (BCM_FAILURE(rv)) {
        printf("error in bcm_fabric_control_set type: bcmFabricQueueMin\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_fabric_control_set(unit,bcmFabricQueueMax,32767);
    if (BCM_FAILURE(rv)) {
        printf("error in bcm_fabric_control_set type: bcmFabricQueueMax\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
 
    sal_memset(&vsq_info, 0, sizeof(vsq_info));
    vsq_info.flags = BCM_COSQ_VSQ_CTTC;

    /* 2. Create VSQs gport with category 2 for each traffic class */
    ct = 2;
    for (tc = 0; tc < 8; ++tc) {
        printf("vsq ct-%d tc-%d \n", ct, tc);
        vsq_info.ct_id = ct;
        vsq_info.traffic_class = tc;

        rv = bcm_cosq_gport_vsq_create(unit,&vsq_info,&cttc_vsq[tc]);
        if (BCM_FAILURE(rv)) {
            printf("error in bcm_cosq_gport_vsq_create for vsq ct-%d tc-%d failed\n", ct, tc);
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    printf("cint_vsq_setup DONE...\n");
    return rv;
}

int
cint_voq_add(int unit, int base_queue)
{
    bcm_error_t rv;
    int cos, ct, tc;
    bcm_gport_t voq;

    /* 3. Assign VOQ queue to VSQB, queue should be related to base_queue */
    /* This needs to be done per VOQ-base on ingress.
     * Above vsq creation can only be done during init and the VSQ per TC
     * should be used to add the VOQs each time new VOQs are allocated
     */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq, base_queue);
    for (cos = 0; cos < 8; cos++) {
        rv = bcm_cosq_gport_vsq_add(unit, cttc_vsq[cos], voq, cos);
        if (BCM_FAILURE(rv)) {
            printf("error in bcm_cosq_gport_vsq_add cos=%d\n", cos);
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    printf("cint_vsq_setup DONE...\n");

    return rv;
}

/*
 * VSQs resource allocation focal function.
 * See comments section on top of this file for more details.
 *
 * Note there are different functions for Jericho/QAX.
 * Jericho2 and above should be treated as QAX
 *
 * Parameters:
 *  src_port             - the source port to which to apply source based VSQs.
 *  nof_dest_ports       - number of destination ports to assign VSQB.
 *  dest_port_base_queue  - the base queue that correspond to the destination port.
 *                          Assign this queue to VSQB (relevant only for Jericho).
 *  device_type          - DEVICE_TYPE_JERICHO/QAX.
 *
 * Usage example:
 *  int unit = 0;
 *  bcm_port_t src_port = 13;
 *  int nof_dest_ports = 2;
 *  int dest_port_base_queue[2] = {128, 136};
 *  cint_vsq_resource_allocation_setup(unit, src_port, nof_dest_ports, dest_port_base_queue);
 */
int
cint_vsq_resource_allocation_setup(int unit, bcm_port_t src_port, int nof_dest_ports, int *dest_port_base_queue, int device_type)
{  
    bcm_error_t rv;
    bcm_gport_t src_vsq_gport = 0, pg_base_vsq_gport = 0;
    int numq = 8, i;
    uint8 pool_id;

    g_device_type = device_type;
    if (device_type == DEVICE_TYPE_JERICHO) {
        pool_id = 0;
    } else if (device_type == DEVICE_TYPE_QAX) {
        pool_id = 1;
    } else {
        printf("cint_vsq_resource_allocation_setup: Unknown device type (%d)\n", device_type);
        return BCM_E_UNAVAIL;
    }

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv)) {
        printf("cint_vsq_resource_allocation_setup: bcm_device_member_get failed");
        return(rv);
    }

    if (is_dnx)
    {
        NOF_CORES = *dnxc_data_get (unit, "device", "general", "nof_cores", NULL);
        /** modid count can not be used as number of cores in J2C */
    }
    else
    {
        rv = bcm_stk_modid_count(unit, &NOF_CORES);
        if (BCM_FAILURE(rv)) {
            printf("cint_vsq_resource_allocation_setup: bcm_stk_modid_count failed");
            return(rv);
        }
    }
    

    rv = cint_gl_vsq_resource_allocate(unit);
    if (BCM_FAILURE(rv)) {
        printf("cint_vsq_resource_allocation_setup: cint_gl_vsq_resource_allocate failed");
        return(rv);
    }

    rv = cint_src_vsqs_create(unit, src_port, &src_vsq_gport, &pg_base_vsq_gport);
    if (BCM_FAILURE(rv)) {
        printf("cint_vsq_resource_allocation_setup: cint_src_vsqs_create failed");
        return(rv);
    }

    rv = cint_src_vsqs_resource_allocate(unit, numq, pool_id, src_vsq_gport, pg_base_vsq_gport);
    if (BCM_FAILURE(rv)) {
        printf("cint_vsq_resource_allocation_setup: cint_src_vsqs_resource_allocate failed");
        return(rv);
    }

    rv = cint_src_vsqs_pfc_setup(unit, numq, src_vsq_gport, pg_base_vsq_gport);
    if (BCM_FAILURE(rv)) {
        printf("cint_vsq_resource_allocation_setup: cint_src_vsqs_pfc_setup failed");
        return(rv);
    }

    /* Functions relevant only for Jericho */
    if (device_type == DEVICE_TYPE_JERICHO) {
        rv = cint_vsq_setup(unit);
        if (BCM_FAILURE(rv)) {
            printf("cint_vsq_resource_allocation_setup: cint_vsq_setup failed");
            return(rv);
        }

        for (i = 0; i < nof_dest_ports; ++i) {
            rv = cint_voq_add(unit, dest_port_base_queue[i]);
            if (BCM_FAILURE(rv)) {
                printf("cint_vsq_resource_allocation_setup: cint_voq_add failed, base_queue=%d\n", dest_port_base_queue[i]);
                return(rv);
            }
        }
    }

    printf("cint_vsq_resource_allocation_setup: PASS\n");

    return rv;
}


int
cint_gl_vsq_resource_allocate(int unit)
{
    if (g_device_type == DEVICE_TYPE_JERICHO) {
        return cint_gl_vsq_resource_allocate_jer(unit);
    } else if (g_device_type == DEVICE_TYPE_QAX) {
        return cint_gl_vsq_resource_allocate_qax(unit);
    } else {
        printf("Unknown device type\n");
        return BCM_E_UNAVAIL;
    }
}

/*******************************
 *  Dispatching Functions    ***
 */

int
cint_src_vsqs_create(int unit, bcm_port_t src_port, bcm_gport_t *src_vsq_gport, bcm_gport_t *pg_base_vsq_gport)
{
    if (g_device_type == DEVICE_TYPE_JERICHO) {
        return cint_src_vsqs_create_jer(unit, src_port, src_vsq_gport, pg_base_vsq_gport);
    } else if (g_device_type == DEVICE_TYPE_QAX) {
        return cint_src_vsqs_create_qax(unit, src_port, src_vsq_gport, pg_base_vsq_gport);
    } else {
        printf("Unknown device type\n");
        return BCM_E_UNAVAIL;
    }
}

int
cint_src_vsqs_resource_allocate(int unit, int numq, int pool_id, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{
    if (g_device_type == DEVICE_TYPE_JERICHO) {
        return cint_src_vsqs_resource_allocate_jer(unit, numq, pool_id, src_vsq_gport, pg_base_vsq_gport);
    } else if (g_device_type == DEVICE_TYPE_QAX) {
        return cint_src_vsqs_resource_allocate_qax(unit, numq, pool_id, src_vsq_gport, pg_base_vsq_gport);
    } else {
        printf("Unknown device type\n");
        return BCM_E_UNAVAIL;
    }
}

int
cint_src_vsqs_pfc_setup(int unit, int numq, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{
    if (g_device_type == DEVICE_TYPE_JERICHO) {
        return cint_src_vsqs_pfc_setup_jer(unit, numq, src_vsq_gport, pg_base_vsq_gport);
    } else if (g_device_type == DEVICE_TYPE_QAX) {
        return cint_src_vsqs_pfc_setup_qax(unit, numq, src_vsq_gport, pg_base_vsq_gport);
    } else {
        printf("Unknown device type\n");
        return BCM_E_UNAVAIL;
    }
}

int
cint_gl_vsq_resource_allocate_qax(int unit)
{
    return cint_gl_vsq_resource_allocate_qax_dynamic(unit, 1);    
}

int
cint_src_vsqs_create_qax(int unit, bcm_port_t src_port, bcm_gport_t *src_vsq_gport, bcm_gport_t *pg_base_vsq_gport)
{
    return cint_src_vsqs_create_qax_dynamic(unit, 1, 0, 1, src_port, src_vsq_gport, pg_base_vsq_gport);
}

int
cint_src_vsqs_resource_allocate_qax(int unit, int numq, uint8 pool_id, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{

    rsrc_alloc port_alloc = {
    /* reserved */ /* shared */ /* headroom */
        1024,         5120,         1024
    };
    rsrc_alloc tc0_alloc  = {512, 2000, 256};
    rsrc_alloc tc7_alloc  = {256, 2000, 0};
    return cint_src_vsqs_resource_allocate_qax_dynamic(unit, 0, 1, bcmResourceBytes, 0, port_alloc, tc0_alloc, tc7_alloc,src_vsq_gport,pg_base_vsq_gport);
}

int
cint_src_vsqs_pfc_setup_qax(int unit, int numq, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{
    return cint_src_vsqs_pfc_setup_qax_dynamic(unit, BCM_COSQ_PFC_BYTES, 102400, 102400, 1024, 0, src_vsq_gport, pg_base_vsq_gport);
}

    

/***********************
 *
 *  QAX Functions    ***
 */

/*
 * Global VSQs resource allocation.
 * Configure for each core: reserved size, pool_0 & pool_1 sizes, headroom size.
 * Configure Words/SRAM-Buffers/SRAM-PDs resources.
 *
 * This is done by calling bcm_cosq_resource_allocation_set() on a global VSQ.
 *
 * To see how the resources are divided, type 'diag resource_alloc' from BCM shell.
 *
 * Note:
 *  Mind the difference between nominal_headroom and max_headroom.
 *  nominal_headroom is the amount of resources to allocate for the headroom
 *  section. If there is a need, the headroom can temporarily expand into the shared pool section
 *  until it will occupy max_headroom resources.
 *  For more details about the connection between nominal_headroom and
 *  max_headroom, and what values to give for them, please refer to QAX
 *  Architecture Guide.
 */
int
cint_gl_vsq_resource_allocate_qax_dynamic(int unit, int headroom_pool)
{
    bcm_error_t rv;
    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv)) {
        printf("cint_vsq_resource_allocation_setup: bcm_device_member_get failed");
        return(rv);
    }
    if (is_dnx)
    {
        NOF_CORES = *dnxc_data_get (unit, "device", "general", "nof_cores", NULL);
        /** modid count can not be used as number of cores in J2C */
    }
    else
    {
        rv = bcm_stk_modid_count(unit, &NOF_CORES);
        if (BCM_FAILURE(rv)) {
            printf("cint_vsq_resource_allocation_setup: bcm_stk_modid_count failed");
            return(rv);
        }
    }
    bcm_gport_t gl_vsq_gports[NOF_CORES];
    bcm_cosq_allocation_resource_t resource[NOF_RSRC];
    bcm_cosq_allocation_entity_t target;
    bcm_cosq_resource_amounts_t amounts[NOF_RSRC];
    uint32 flags = 0;
    uint8 pool_id, is_ocb_only = 0;
    int core = -1;
    int rsrc_type = 0;

    for (core = 0; core < NOF_CORES; ++core) {
        BCM_COSQ_GPORT_VSQ_GL_SET(gl_vsq_gports[core], core);
    }

    /* Allocate all global resources */
    for (core = 0; core < NOF_CORES; ++core) {
        for (pool_id = 0; pool_id < 2; ++pool_id) {
            /* Words amounts */
            resource[0] = bcmResourceBytes;
            amounts[0].flags = 0;
            amounts[0].reserved = 0; /* Not relevant for global allocation */
            amounts[0].max_shared_pool = (pool_id == 0) ? 10000000 : 20000000;
            amounts[0].nominal_headroom = (pool_id == headroom_pool) ? 1000000 : 0;
            amounts[0].max_headroom = amounts[0].nominal_headroom; /* max_headroom mast be equal to nominal_headroom for total Words */

            /* SRAM-Buffers amounts */
            resource[1] = bcmResourceOcbBuffers;
            amounts[1].flags = 0;
            amounts[1].reserved = 0; /* Not relevant for global allocation */
            amounts[1].max_shared_pool = (pool_id == 0) ? 4000: 5000;
            amounts[1].nominal_headroom = (pool_id == headroom_pool) ? 1000 : 0;
            amounts[1].max_headroom = amounts[1].nominal_headroom * 2;

            /* SRAM-PDs amounts */
            resource[2] = bcmResourceOcbPacketDescriptors;
            amounts[2].flags = 0;
            amounts[2].reserved = 0; /* Not relevant for global allocation */
            amounts[2].max_shared_pool = (pool_id == 0) ? 9000 : 10000;
            amounts[2].nominal_headroom = (pool_id == headroom_pool) ? 2000 : 0;
            amounts[2].max_headroom = amounts[2].nominal_headroom * 2;

            target.gport = gl_vsq_gports[core];
            target.color = 0; /* Not relevant for global allocation */
            target.is_ocb_only = 0; /* Not relevant for QAX */
            target.pool_id = pool_id;
            for (rsrc_type = 0; rsrc_type < NOF_RSRC; ++rsrc_type) {
                rv = bcm_cosq_resource_allocation_set(unit, flags, resource[rsrc_type], &target, &amounts[rsrc_type]);
                if (BCM_FAILURE(rv)) {
                    printf("cint_gl_vsq_resource_allocate_qax: bcm_cosq_resource_allocation_set failed, core=%d, rsrc_type=%d pool_id=%d.\n", core, rsrc_type, pool_id);
                    return(rv);
                }
            }
        }
    }

    printf("cint_gl_vsq_resource_allocate_qax: PASS\n");
    return rv;
}

/*
 * Create Src-Port-VSQs and PG-VSQs.
 *
 * In this example we create all the PG-VSQs on pool_1, and
 * configure them as lossless (meaning headroom will be used if required).
 * Note: In QAX, all lossless traffic must reside in the same pool.
 *
 * Parameters:
 *  src_port (IN)           - the source port for which to create the VSQs to.
 *  src_vsq_gport (OUT)     - the resulted base Src-PORT-VSQ gport.
 *  pg_base_vsq_gport (OUT) - the resulted base PG-VSQ gport.
 *
 * Note: The resulted gports are the base gports, i.e. several VSQ gports are configured,
 *       but the API return only the first.
 *       PG-VSQ: The gport is group 0. If, for example (like in this function), there are
 *               8 cosq configured, the result gport will be of cosq 0. To get gports of 
 *               other cosqs, use BCM_COSQ_GPORT_VSQ_PG_GET to retrieve the pg_vsq_id, add to it
 *               the required cosq (the gports are allocated consecutivly) and use this new id to get the gport by
 *               using BCM_COSQ_GPORT_VSQ_PG_SET.
 *               See cint_src_vsqs_resource_allocate_qax() for usage example.
 */
int
cint_src_vsqs_create_qax_dynamic(int unit, int is_lossles, uint8 odd_pg_uses_pool_1, uint8 tar_pool,  bcm_port_t src_port, bcm_gport_t *src_vsq_gport, bcm_gport_t *pg_base_vsq_gport)
{
    bcm_error_t rv = 0;
    bcm_cosq_src_vsqs_gport_config_t vsq_config;
    int cosq = 0;
    uint8 pool_id = tar_pool;
    uint32 flags = 0;
    bcm_gport_t src_gport;

    sal_memset(&vsq_config, 0, sizeof(vsq_config));

    vsq_config.flags = 0;
    vsq_config.numq = BCM_COSQ_VSQ_NOF_PG;

    for (cosq = 0; cosq < vsq_config.numq; cosq++) {
        vsq_config.pg_attributes[cosq].cosq_bitmap = 0x1 << cosq;
        if(odd_pg_uses_pool_1)
        {
            vsq_config.pg_attributes[cosq].pool_id =(cosq%2==0)?0:1;
        }else
            vsq_config.pg_attributes[cosq].pool_id =tar_pool;
        /**
         * In the test using this cint, TC/PG 7 is congested.
         * For the test purposes, in order to avoid FC from the source port, this PG is defined as non_lossless
         */
        if (cosq != 7) {
            vsq_config.pg_attributes[cosq].is_lossles = is_lossles;  /* For lossy queue (not using headroom), change to 0.
                                                           Note that all lossless traffic must reside in the
                                                           same pool. */
        } else {
            vsq_config.pg_attributes[cosq].is_lossles = 0;
        }
    }
    BCM_GPORT_LOCAL_SET(src_gport, src_port);
    rv = bcm_cosq_src_vsqs_gport_add(unit, src_gport, &vsq_config, src_vsq_gport, pg_base_vsq_gport);
    if (BCM_FAILURE(rv)) {
        printf("cint_src_vsqs_create_qax: bcm_cosq_src_vsqs_gport_add failed, src_port(%d)\n", src_port);
        return(rv);
    } else {
        printf("cint_src_vsqs_create_qax: bcm_cosq_src_vsqs_gport_add success, src_port(%d) src_vsq_gport(0x%x) pg_base_vsq_gport(0x%x)\n",
                src_port, *src_vsq_gport, *pg_base_vsq_gport);
    }

    printf("cint_src_vsqs_create_qax: PASS\n");
    return rv;
}

/*
 * Allocate resources for Src-Port-VSQs and PG-VSQs.
 * This is done by calling bcm_cosq_resource_allocation_set() for the right VSQs.
 *
 * In QAX there are 3 resources that can be configured - Words, SRAM-Buffers,
 * SRAM-PDs.
 * Usually Bytes resources will be configured when working with DRAMs, and
 * SRAM-Buffers & SRAM-PDs will be configured when working without DRAMs.
 * This example configures Bytes only for simplicity.
 * This example also use arbitrary values for simplicity. 
 * Please use suitable resources and values when working on a real system.
 *
 * Parameters:
 *  numq              - number of gports in pg_base_vsq bundle.
 *  pool_id           - for which pool to allocate resources (needed for Port-VSQ. PG-VSQ's pool is configured on creation).
 *  src_vsq_gport     - base Port-VSQ gport(the gport got from bcm_cosq_src_vsqs_gport_add).
 *  pg_base_vsq_gport - base PG-VSQ gport (the gport got from bcm_cosq_src_vsqs_gport_add).
 *
 * Note:
 *  Mind the difference between nominal_headroom and max_headroom.
 *  nominal_headroom is the amount of resources to allocate for the headroom
 *  section. If there is a need, the headroom can temporarily expand into the shared pool section
 *  until it will occupy max_headroom resources.
 *  For more details about the connection between nominal_headroom and
 *  max_headroom, and what values to give for them, please refer to QAX
 *  Architecture Guide.
 */
int
cint_src_vsqs_resource_allocate_qax_dynamic(int unit, uint8 odd_pg_uses_pool_1, uint8 tar_pool, bcm_cosq_allocation_resource_t rsrc, int dp, rsrc_alloc port_alloc, rsrc_alloc tc0_alloc, rsrc_alloc tc7_alloc, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{
    bcm_error_t rv;
    int index = -1;
    int offset = -1;
    uint32 flags = BCM_COSQ_RSRC_ALLOC_COLOR_BLIND;
    int pool_id = tar_pool;
    bcm_gport_t vsq_gport = -1;
    int core_id = -1, pg_vsq_id = -1, pg_base_vsq_id = -1, src_port_vsq_id = -1;
    int color = -1, is_ocb_only = 0;
    bcm_cosq_allocation_entity_t target;
    bcm_cosq_allocation_resource_t resource;
    bcm_cosq_resource_amounts_t amounts;
    int rsrc_type;
    int factor = 1;
    int numq =8;

    if (is_dnx)
    {
        /*
         * For DNX, credit size is 4x larger than for DPP.
         * thus the thresholds should be 4x bigger
         */
        factor = 4;
    }

    /*  src_vsq_gport     - base Port-VSQ gport(the gport got from bcm_cosq_src_vsqs_gport_add) */
    src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(src_vsq_gport);

    /*  pg_base_vsq_gport - base PG-VSQ gport (the gport got from bcm_cosq_src_vsqs_gport_add) */
    pg_base_vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);
    
    /* Parameters for src_port vsq */
    /* 
     * This example configures only Bytes resources.
     * To configure other resouces amounts use:
     * Bytes        - resource = bcmResourceBytes;
     * SRAM-Buffers - resource = bcmResourceOcbBuffers;
     * SRAM-PDs     - resource = bcmResourceOcbPacketDescriptors;
     */
    /* Allocate resources for port VSQ.*/
    resource=rsrc;
    amounts.flags = 0;
    amounts.reserved = port_alloc.reserved * factor;
    amounts.max_shared_pool = (pool_id == tar_pool) ? port_alloc.shared * factor: port_alloc.shared * factor;
    amounts.nominal_headroom = (pool_id == tar_pool) ? port_alloc.headroom * factor: 0;
    amounts.max_headroom = amounts.nominal_headroom; /* max_headroom must be equal to nominal_headroom for SRC-PORT-VSQ */
        /* Allocate resources for SRC_PORT VSQ */
    /* Get the right is_ocb_only gport */
    BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(vsq_gport, core_id, is_ocb_only, src_port_vsq_id);
    target.gport = vsq_gport;

    /*Here dp must be zero if you don't want to test with different colored traffic. */
    target.color = dp;
    printf("Src Port Rsrc Alloc: gport(0x%x) color(%d) reserved(0x%x) headroom(0x%x) sh_pool(0x%x)\n",
        vsq_gport, color, amounts.reserved , amounts.max_headroom, amounts.max_shared_pool);
    target.pool_id = tar_pool;
    rv = bcm_cosq_resource_allocation_set(unit, flags, resource, &target, &amounts);
    if (BCM_FAILURE(rv)) {
        printf("resource_allocate: bcm_cosq_resource_allocation_set failed, Error (%d, %s)\n", rv, bcm_errmsg(rv));
        return(rv);
    }

    /* Allocate resources for PG VSQ.
     * In this example cosqs 0-6 are configured the same, with reserved resource,
     * whereas cosq 7 is configured without reserved resources.
     */
    bcm_cosq_resource_amounts_t amounts_pg[8];
    for (offset = 0; offset < numq; offset++) {
        pg_vsq_id = pg_base_vsq_id + offset;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);
        switch (offset)
        {
            case 7:
                amounts_pg[offset].reserved = tc7_alloc.reserved * factor;
                amounts_pg[offset].nominal_headroom = tc7_alloc.headroom * factor;
                amounts_pg[offset].max_headroom = tc7_alloc.headroom * factor + 1;
                amounts_pg[offset].shared_pool_fadt.thresh_max = tc7_alloc.shared * factor;
                amounts_pg[offset].shared_pool_fadt.thresh_min = tc7_alloc.shared * factor;
                amounts_pg[offset].shared_pool_fadt.alpha = 0;
            break;
            /* here cosq0-6 configured the same*/
            default:
                amounts_pg[offset].reserved = tc0_alloc.reserved * factor;
                amounts_pg[offset].nominal_headroom = tc0_alloc.headroom * factor;
                amounts_pg[offset].max_headroom = tc0_alloc.headroom * factor + 1;
                amounts_pg[offset].shared_pool_fadt.thresh_max = tc0_alloc.shared * factor;
                amounts_pg[offset].shared_pool_fadt.thresh_min = tc0_alloc.shared * factor;
                amounts_pg[offset].shared_pool_fadt.alpha = 0;
            break;
        }
        target.gport = vsq_gport;
        target.color = dp;
        target.is_ocb_only = 0;
        if(odd_pg_uses_pool_1){
            target.pool_id = (offset%2==0)?0:1;
        } else {
            target.pool_id = tar_pool;
        }
        rv = bcm_cosq_resource_allocation_set(unit, flags, resource, &target, &amounts_pg[offset]);
        if (BCM_FAILURE(rv)) {
            printf("resource_allocate: bcm_cosq_resource_allocation_set failed, Error (%d, %s)\n", rv, bcm_errmsg(rv));
            return(rv);
        }
        printf("PG Rsrc Alloc: gport(0x%x) color(%d) reserved(0x%x) headroom(0x%x) sh_pool(0x%x)\n",
                vsq_gport, color, amounts_pg[offset].reserved, amounts_pg[offset].max_headroom, amounts_pg[offset].shared_pool_fadt.thresh_max);
    }
    printf("cint_src_vsqs_resource_allocate_qax: PASS\n");
    return rv;
}

/* 
 * Configure VSQs Flow Control.
 *
 * Need to configure VSQs PFC, because the default thresholds are max, 
 * which mean that for lossles VSQs the queue will use the reserved 
 * resources and when they would fill up it will start using the 
 * shared pool resources until the end, without using the headroom allocation at all,
 * because for lossless traffic the threshold for stop using the shared pool
 * and start using the headroom is the flow control threshold and not the
 * shared pool threshold.
 *
 * Parameters:
 *  numq              - number of gports in pg_base_vsq bundle.
 *  src_vsq_gport     - base Port-VSQ gport(the gport got from bcm_cosq_src_vsqs_gport_add).
 *  pg_base_vsq_gport - base PG-VSQ gport (the gport got from bcm_cosq_src_vsqs_gport_add).
 */
int
cint_src_vsqs_pfc_setup_qax_dynamic(int unit, int pfc_flags, int xoff_threshold, int xon_threshold, int xoff_fadt_threshold, int xon_fadt_offset,bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
{
    bcm_error_t rv = 0;
    int offset = 0;
    bcm_cosq_pfc_config_t config;
    int pg_vsq_id = -1, pg_base_vsq_id = -1, src_port_vsq_id = -1;
    bcm_gport_t vsq_gport = -1;
    int core_id = -1, is_ocb_only = 0;
    int i;
    int factor = 1;
    int numq = 8;

    if (is_dnx)
    {
        /*
         * For DNX, credit size is 4x larger than for DPP.
         * thus the thresholds should be 4x bigger
         */
        factor = 4;
    }

    src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(src_vsq_gport);

    pg_base_vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    printf("pfc_vsq_setup: setting up PFC config..\n");

    /* Thresholds for SRC_PORT PFC */
    /**
     * Set source port FC threshold to a higher value than the drop threshold 
     * in order to avoid FC from source port
     */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_threshold = xoff_threshold * factor;
    config.xon_threshold = xon_threshold * factor;

    printf("pfc_vsq_setup: SRC_PORT SETUP..\n");
    BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(vsq_gport, core_id, is_ocb_only, src_port_vsq_id);

    rv = bcm_cosq_pfc_config_set(unit, vsq_gport, 0, pfc_flags, &config);
    if (BCM_FAILURE(rv)) {
        printf("pfc_vsq_setup: pfc_config_set failed, is_ocb_only(%d)\n", is_ocb_only);
        return rv;
    } else {
        printf("pfc_vsq_setup: pfc_config_set success, is_ocb_only(%d)\n", is_ocb_only);
    }

    /* Thresholds for PG PFC */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_fadt_threshold.thresh_min = xoff_fadt_threshold * factor;
    config.xoff_fadt_threshold.thresh_max = xoff_fadt_threshold * factor;
    config.xoff_fadt_threshold.alpha = 0;
    config.xon_fadt_offset = xon_fadt_offset;

    for (offset = 0; offset < numq-1; ++offset) {
        printf("pfc_vsq_setup: SRC_PORT COS-%d SETUP..\n",  offset);
        pg_vsq_id = pg_base_vsq_id + offset;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);

        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, 0, pfc_flags, &config);
        if (BCM_FAILURE(rv)) {
            printf("pfc_vsq_setup: pfc_config_set failed, cosq(%d)\n", offset);
            return(rv);
        } else {
            printf("pfc_vsq_setup: pfc_config_set success, cosq(%d)\n", offset);
        }
    }
    printf("cint_src_vsqs_pfc_setup_qax: PASS\n");
    return rv;
}