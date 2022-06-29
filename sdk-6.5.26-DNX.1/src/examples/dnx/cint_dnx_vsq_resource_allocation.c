/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This cint shows example of source based VSQs resource allocation.
 *
 * Resource allocation walkthough (Focal function: cint_vsq_resource_allocation_setup()):
 * 1) dnx_gl_vsq_resource_allocate_dynamic() - global VSQs resource allocation.
 * 2) dnx_src_vsqs_create_dynamic() - create PORT-VSQ and PG-VSQ.
 * 3) dnx_src_vsqs_resource_allocate_dynamic() - allocate resources for the PORT-VSQ and PG-VSQ.
 * 4) dnx_src_vsqs_pfc_setup_dynamic() - configure flow control for PORT & PG VSQs.
 * Test Scenario - start
 *
 * ./bcm.user
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/cint_dnx_vsq_resource_allocation.c
 * cint ../../../../src/examples/dnx/internal/TM/cint_vsq_group_occupancy_dbal.c
 * cint
 * bcm_gport_t src_vsq_gport;
 * bcm_gport_t pg_base_vsq_gport;
 * rsrc_alloc port_alloc = {64, 64, 0};
 * rsrc_alloc tc0_alloc = {32, 64, 0};
 * rsrc_alloc tc7_alloc = {64, 32, 0};
 * dnx_gl_vsq_resource_allocate_dynamic(0, 0);
 * dnx_src_vsqs_create_dynamic(0, 0, 1, 0, 13, src_vsq_gport, pg_base_vsq_gport);
 * dnx_src_vsqs_resource_allocate_dynamic(0, 1, 0, 1, 0, port_alloc, tc0_alloc, tc7_alloc, src_vsq_gport, pg_base_vsq_gport);
 * dnx_src_vsqs_pfc_setup_dynamic(0, 10, 512, 256, 256, 0, src_vsq_gport, pg_base_vsq_gport);
 * exit;
 *
 * Test Scenario - end
 *
 */
struct rsrc_alloc
{
    uint32 reserved;
    uint32 shared;
    uint32 headroom;
};

/* Basic parameters */
int NOF_CORES;
int NOF_RSRC = 3;

/*
 * VSQs resource allocation focal function.
 * See comments section on top of this file for more details.
 *
 * Parameters:
 *  src_port             - the source port to which to apply source based VSQs.
 *  nof_dest_ports       - number of destination ports to assign VSQB.
 *
 * Usage example:
 *  int unit = 0;
 *  bcm_port_t src_port = 13;
 *  int nof_dest_ports = 2;
 *  cint_vsq_resource_allocation_setup(unit, src_port, nof_dest_ports);
 */
int
cint_vsq_resource_allocation_setup(int unit, bcm_port_t src_port)
{
    bcm_error_t rv;
    bcm_gport_t src_vsq_gport = 0, pg_base_vsq_gport = 0;
    rsrc_alloc port_alloc =
    {
        /* reserved */ /* shared */ /* headroom */
        1024,         5120,         1024
    };
    rsrc_alloc tc0_alloc  = {512, 2000, 256};
    rsrc_alloc tc7_alloc  = {256, 2000, 0};

    rv = dnx_gl_vsq_resource_allocate_dynamic(unit, 1);
    if (BCM_FAILURE(rv))
    {
        printf("cint_vsq_resource_allocation_setup: dnx_gl_vsq_resource_allocate_dynamic failed");
        return(rv);
    }

    rv = dnx_src_vsqs_create_dynamic(unit, 1, 0, 1, src_port, src_vsq_gport, pg_base_vsq_gport);
    if (BCM_FAILURE(rv))
    {
        printf("cint_vsq_resource_allocation_setup: dnx_src_vsqs_create_dynamic failed");
        return(rv);
    }

    rv = dnx_src_vsqs_resource_allocate_dynamic(unit, 0, 1, bcmResourceBytes, 0, port_alloc, tc0_alloc, tc7_alloc,src_vsq_gport,pg_base_vsq_gport);
    if (BCM_FAILURE(rv))
    {
        printf("cint_vsq_resource_allocation_setup: dnx_src_vsqs_resource_allocate_dynamic failed");
        return(rv);
    }

    rv = dnx_src_vsqs_pfc_setup_dynamic(unit, BCM_COSQ_PFC_BYTES, 102400, 102400, 1024, 0, src_vsq_gport, pg_base_vsq_gport);
    if (BCM_FAILURE(rv))
    {
        printf("cint_vsq_resource_allocation_setup: dnx_src_vsqs_pfc_setup_dynamic failed");
        return(rv);
    }

    printf("cint_vsq_resource_allocation_setup: PASS\n");
    return rv;
}

/*
 * Global VSQs resource allocation.
 * Configure for each core: reserved size, pool_0 & pool_1 sizes, headroom size.
 * Configure Words/SRAM-Buffers/SRAM-PDs resources.
 *
 * This is done by calling bcm_cosq_resource_allocation_set() on a global VSQ.
 *
 * Note:
 *  Mind the difference between nominal_headroom and max_headroom.
 *  nominal_headroom is the amount of resources to allocate for the headroom
 *  section. If there is a need, the headroom can temporarily expand into the shared pool section
 *  until it will occupy max_headroom resources.
 *  For more details about the connection between nominal_headroom and
 *  max_headroom, and what values to give for them
 */
int
dnx_gl_vsq_resource_allocate_dynamic(int unit, int headroom_pool)
{
    bcm_error_t rv;
    NOF_CORES = *dnxc_data_get (unit, "device", "general", "nof_cores", NULL);
    /* modid count can not be used as number of cores in J2C */
    bcm_gport_t gl_vsq_gports[NOF_CORES];
    bcm_cosq_allocation_resource_t resource[NOF_RSRC];
    bcm_cosq_allocation_entity_t target;
    bcm_cosq_resource_amounts_t amounts[NOF_RSRC];
    uint32 flags = 0;
    uint8 pool_id;
    int core = -1;
    int rsrc_type = 0;

    for (core = 0; core < NOF_CORES; ++core)
    {
        BCM_COSQ_GPORT_VSQ_GL_SET(gl_vsq_gports[core], core);
    }

    /* Allocate all global resources */
    for (core = 0; core < NOF_CORES; ++core)
    {
        for (pool_id = 0; pool_id < 2; ++pool_id)
        {
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
            target.is_ocb_only = 0;
            target.pool_id = pool_id;
            for (rsrc_type = 0; rsrc_type < NOF_RSRC; ++rsrc_type)
            {
                rv = bcm_cosq_resource_allocation_set(unit, flags, resource[rsrc_type], &target, &amounts[rsrc_type]);
                if (BCM_FAILURE(rv))
                {
                    printf("dnx_gl_vsq_resource_allocate_dynamic: bcm_cosq_resource_allocation_set failed, core=%d, rsrc_type=%d pool_id=%d.\n", core, rsrc_type, pool_id);
                    return(rv);
                }
            }
        }
    }

    printf("dnx_gl_vsq_resource_allocate_dynamic: PASS\n");
    return rv;
}

/*
 * Create Src-Port-VSQs and PG-VSQs.
 *
 * In this example we create all the PG-VSQs on pool_1, and
 * configure them as lossless (meaning headroom will be used if required).
 * Note: all lossless traffic must reside in the same pool.
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
 */
int
dnx_src_vsqs_create_dynamic(int unit, int is_lossles, uint8 odd_pg_uses_pool_1, uint8 tar_pool,  bcm_port_t src_port, bcm_gport_t *src_vsq_gport, bcm_gport_t *pg_base_vsq_gport)
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
    for (cosq = 0; cosq < vsq_config.numq; cosq++)
    {
        vsq_config.pg_attributes[cosq].cosq_bitmap = 0x1 << cosq;
        if(odd_pg_uses_pool_1)
        {
            vsq_config.pg_attributes[cosq].pool_id = (cosq % 2 == 0) ? 0 : 1;
        } else
        {
            vsq_config.pg_attributes[cosq].pool_id = tar_pool;
        }
        /*
         * In the test using this cint, TC/PG 7 is congested.
         * For the test purposes, in order to avoid FC from the source port, this PG is defined as non_lossless
         */
        if (cosq != 7)
        {
            vsq_config.pg_attributes[cosq].is_lossles = is_lossles;  /* For lossy queue (not using headroom), change to 0.
                                                           Note that all lossless traffic must reside in the
                                                           same pool. */
        } else
        {
            vsq_config.pg_attributes[cosq].is_lossles = 0;
        }
    }

    BCM_GPORT_LOCAL_SET(src_gport, src_port);
    rv = bcm_cosq_src_vsqs_gport_add(unit, src_gport, &vsq_config, src_vsq_gport, pg_base_vsq_gport);
    if (BCM_FAILURE(rv))
    {
        printf("dnx_src_vsqs_create_dynamic: bcm_cosq_src_vsqs_gport_add failed, src_port(%d)\n", src_port);
        return(rv);
    } else
    {
        printf("dnx_src_vsqs_create_dynamic: bcm_cosq_src_vsqs_gport_add success, src_port(%d) src_vsq_gport(0x%x) pg_base_vsq_gport(0x%x)\n",
                src_port, *src_vsq_gport, *pg_base_vsq_gport);
    }

    printf("dnx_src_vsqs_create_dynamic: PASS\n");
    return rv;
}

/*
 * Allocate resources for Src-Port-VSQs and PG-VSQs.
 * This is done by calling bcm_cosq_resource_allocation_set() for the right VSQs.
 *
 * There are 3 resources that can be configured - Words, SRAM-Buffers,
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
 *  max_headroom, and what values to give for them
 */
int
dnx_src_vsqs_resource_allocate_dynamic(int unit, uint8 odd_pg_uses_pool_1, uint8 tar_pool, bcm_cosq_allocation_resource_t rsrc, int dp, rsrc_alloc port_alloc, rsrc_alloc tc0_alloc, rsrc_alloc tc7_alloc, bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
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
    bcm_cosq_resource_amounts_t amounts_pg[8];

    factor = 4;
    /* src_vsq_gport     - base Port-VSQ gport(the gport got from bcm_cosq_src_vsqs_gport_add) */
    src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(src_vsq_gport);
    /* pg_base_vsq_gport - base PG-VSQ gport (the gport got from bcm_cosq_src_vsqs_gport_add) */
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
    resource = rsrc;
    amounts.flags = 0;
    amounts.reserved = port_alloc.reserved * factor;
    amounts.max_shared_pool = (pool_id == tar_pool) ? port_alloc.shared * factor: port_alloc.shared * factor;
    amounts.nominal_headroom = (pool_id == tar_pool) ? port_alloc.headroom * factor: 0;
    amounts.max_headroom = amounts.nominal_headroom; /* max_headroom must be equal to nominal_headroom for SRC-PORT-VSQ */

    /* Allocate resources for SRC_PORT VSQ */
    /* Get the right is_ocb_only gport */
    BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(vsq_gport, core_id, is_ocb_only, src_port_vsq_id);
    target.gport = vsq_gport;
    /* Here dp must be zero if you don't want to test with different colored traffic. */
    target.color = dp;
    printf("Src Port Rsrc Alloc: gport(0x%x) color(%d) reserved(0x%x) headroom(0x%x) sh_pool(0x%x)\n",
        vsq_gport, color, amounts.reserved , amounts.max_headroom, amounts.max_shared_pool);
    target.pool_id = tar_pool;
    rv = bcm_cosq_resource_allocation_set(unit, flags, resource, &target, &amounts);
    if (BCM_FAILURE(rv))
    {
        printf("resource_allocate: bcm_cosq_resource_allocation_set failed, Error (%d, %s)\n", rv, bcm_errmsg(rv));
        return(rv);
    }

    /* Allocate resources for PG VSQ.
     * In this example cosqs 0-6 are configured the same, with reserved resource,
     * whereas cosq 7 is configured without reserved resources.
     */
    for (offset = 0; offset < numq; offset++)
    {
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
        if(odd_pg_uses_pool_1)
        {
            target.pool_id = (offset%2==0)?0:1;
        } else
        {
            target.pool_id = tar_pool;
        }

        rv = bcm_cosq_resource_allocation_set(unit, flags, resource, &target, &amounts_pg[offset]);
        if (BCM_FAILURE(rv))
        {
            printf("resource_allocate: bcm_cosq_resource_allocation_set failed, Error (%d, %s)\n", rv, bcm_errmsg(rv));
            return(rv);
        }

        printf("PG Rsrc Alloc: gport(0x%x) color(%d) reserved(0x%x) headroom(0x%x) sh_pool(0x%x)\n",
                vsq_gport, color, amounts_pg[offset].reserved, amounts_pg[offset].max_headroom, amounts_pg[offset].shared_pool_fadt.thresh_max);
    }

    printf("dnx_src_vsqs_resource_allocate_dynamic: PASS\n");
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
dnx_src_vsqs_pfc_setup_dynamic(int unit, int pfc_flags, int xoff_threshold, int xon_threshold, int xoff_fadt_threshold, int xon_fadt_offset,bcm_gport_t src_vsq_gport, bcm_gport_t pg_base_vsq_gport)
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

    factor = 4;
    src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(src_vsq_gport);
    pg_base_vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    printf("pfc_vsq_setup: setting up PFC config..\n");

    /* Thresholds for SRC_PORT PFC */
    /*
     * Set source port FC threshold to a higher value than the drop threshold
     * in order to avoid FC from source port
     */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_threshold = xoff_threshold * factor;
    config.xon_threshold = xon_threshold * factor;
    printf("pfc_vsq_setup: SRC_PORT SETUP..\n");
    BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(vsq_gport, core_id, is_ocb_only, src_port_vsq_id);
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport, 0, pfc_flags, &config);
    if (BCM_FAILURE(rv))
    {
        printf("pfc_vsq_setup: pfc_config_set failed, is_ocb_only(%d)\n", is_ocb_only);
        return rv;
    } else
    {
        printf("pfc_vsq_setup: pfc_config_set success, is_ocb_only(%d)\n", is_ocb_only);
    }

    /* Thresholds for PG PFC */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_fadt_threshold.thresh_min = xoff_fadt_threshold * factor;
    config.xoff_fadt_threshold.thresh_max = xoff_fadt_threshold * factor;
    config.xoff_fadt_threshold.alpha = 0;
    config.xon_fadt_offset = xon_fadt_offset;
    for (offset = 0; offset < numq - 1; ++offset)
    {
        printf("pfc_vsq_setup: SRC_PORT COS-%d SETUP..\n",  offset);
        pg_vsq_id = pg_base_vsq_id + offset;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);

        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, 0, pfc_flags, &config);
        if (BCM_FAILURE(rv))
        {
            printf("pfc_vsq_setup: pfc_config_set failed, cosq(%d)\n", offset);
            return(rv);
        } else
        {
            printf("pfc_vsq_setup: pfc_config_set success, cosq(%d)\n", offset);
        }
    }
    printf("dnx_src_vsqs_pfc_setup_dynamic: PASS\n");
    return rv;
}