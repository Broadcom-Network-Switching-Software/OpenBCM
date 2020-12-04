/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : TCB - WRED
 *  
 *  Usage    : BCM.0> cint tcb_wred.c
 *  
 *  config   : tcb_config.bcm
 *  
 *  Log file : tcb_wred_log.txt
 *  
 *  Test Topology :
 * +-----------------+            +---------+
 * |                 |            |         |
 * |                 |            |         |
 * |             XE0 <------------+         |
 * |                 |            |         |
 * |                 |            |Traffic  |
 * |     SVK     XE1 <------------+Generator|
 * |                 |            |         |
 * |                 |            |         |
 * |                 |            |         |
 * |             XE2 +------------>         |
 * |                 |            |         |
 * |                 |            |         |
 * |                 |            |         |
 * +-----------------+            +---------+
 * 
 *  Summary:
 *  ========
 * This CINT script demonstrates how to configure chip TCB to detect ingress 
 * admission control drop. 
 * 
 *  Detailed steps done in the CINT script:
 *  =====================================================
 * 1) Step1 - Test Setup
 * ============================================
 * a) Connect xe0-xe2 with traffic generator.
 * b) Make sure xe0-xe2 link up with traffic generator.
 * 
 * 2) Step2 - Configuration
 * ============================================
 * ./bcm.user
 * l2 add pbm=xe2 mac=0x0042434DFFFF vlanid=1 rp=false
 * c tcb_wred.c
 * 
 * 3) Step3 - Verification
 * ==========================================
 * a) Send wirespeed traffic into xe0
 *  0042434DFFFF0042434D007A81000001
 *  0B040000000000000000000000000000
 *  00000000000000000000000000000000
 *  00000000000000000000000000000000
 * Expected Result:
 * ===================
 * TCB and event buffer data is printed.
 */

/* Structure for WRED Configuration */
cint_reset();

struct wred_config_t
{
    bcm_port_t port;      /* port id */
    bcm_cos_queue_t cosq; /* cosq to configure wred */
    uint32 flags;
    int min_threshold;    /* queue depth in bytes to begin dropping */
    int max_threshold;    /* queue depth in bytes to drop all packets */
    int drop_rate;        /* drop probability at max threshold */
};

int i = 10; /* Number of queues assigned to port */

/* Structure to collect cosq gport information for the given port in gport_callback */
struct cosq_gport_info_t
{
	int index;            /* to iterate */
        bcm_port_t port;      /* port for which cosq gport information is needed */
	bcm_gport_t gport[i]; /* place holder for gport information */
};

/* Structure for tcb configuration and its threshold profile */
struct tcb_config_t
{
    bcm_gport_t gport;               /* monitor entity - Unicast queue/Physical port */
    bcm_cosq_dir_t direction;        /* cosq direction - Ingress/Egress */
    bcm_cosq_tcb_scope_t scope_type; /* monitor scope type - Unicast queue/Ingress port/Egress port*/
    uint32 trigger_reason;           /* trigger event - Ingress admin drop/Egress admin drop/WRED/CFAP */
    uint32 start_threshold;          /* tcb start threshold in bytes */
    uint32 stop_threshold;           /* tcb stop threshold in bytes */
};

/* Steps to configure WRED */
int
configure_wred(int unit, wred_config_t *wred_cfg)
{
    bcm_error_t rv;
    bcm_cosq_gport_discard_t discard;

    bcm_cosq_gport_discard_t_init(&discard);
    discard.flags = wred_cfg->flags;
    discard.min_thresh = wred_cfg->min_threshold;
    discard.max_thresh = wred_cfg->max_threshold;
    discard.drop_probability = wred_cfg->drop_rate;

    rv = bcm_cosq_gport_discard_set(unit, wred_cfg->port, wred_cfg->cosq,
                                    &discard);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_gport_discard_set: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Steps to collect cosq gport information for the given port */
int
gport_callback(int unit, bcm_gport_t port, int numq, uint32 flags,
               bcm_gport_t gport, void *user_data)
{
    cosq_gport_info_t *cosq_gport = user_data;
    bcm_port_t local_port;

    if (flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) {
        local_port = BCM_GPORT_MODPORT_PORT_GET(port);
        if (local_port == cosq_gport->port) {
            cosq_gport->gport[cosq_gport->index] = gport;
            cosq_gport->index++;
        }
    }
}

/* Callback function for TCB events */
void
bcm_cosq_tcb_callback(int unit, bcm_cosq_buffer_id_t buffer_id, void *user_data)
{
    const int array_max = 1024;
    int *event_count = user_data;
    bcm_cosq_tcb_buffer_t buffer_array[array_max];
    bcm_cosq_tcb_event_t event_array[array_max];
    int array_count;
    int overflow_count;
    int i;
    bcm_error_t rv;
    (*event_count)++;

    rv = bcm_cosq_tcb_control_set(unit, -1, bcmCosqTcbControlFreeze, TRUE);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_control_set: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Print "interesting" information from the buffer and event arrays. */
    rv = bcm_cosq_tcb_buffer_multi_get(unit, buffer_id, array_max, buffer_array,
                                       &array_count);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_buffer_multi_get: %s.\n",
               bcm_errmsg(rv));
        return rv;
    }

    printf("\nTCB buffer info\n");
    printf("queue 0x%x\n",buffer_array.queue);
    printf("Packet data\n");
    for(i=0; i<BCM_COSQ_TCB_MAX_BUF_SIZE; i++)
    printf("%x",buffer_array.buf_data[i]);
    printf("\n\n");

    rv = bcm_cosq_tcb_event_multi_get(unit, buffer_id, array_max, event_array,
                                      &array_count, &overflow_count);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_event_multi_get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    printf("Event buffer info\n");
    printf("TCB event %d on buffer %d\n", *event_count, buffer_id);
    printf("Egr gport 0x%x\n",event_array.egress_gport);
    printf("Drop reason %d\n",event_array.drop_reason);
    printf("queue 0x%x\n",event_array.queue);
    printf("queue size 0x%x\n",event_array.queue_size);
}

/* Helper routine for configuring tcb for WRED drop */
int
configure_wred_trigger(int unit, int buffer_id, int profile_index,
                         tcb_config_t *tcb)
{
    bcm_cosq_tcb_config_t config;
    bcm_cosq_tcb_threshold_profile_t threshold;
    bcm_cosq_object_id_t object_id;
    bcm_error_t rv;
    int flags = 0;
    bcm_cos_queue_t cosq = 0;

    rv = bcm_cosq_tcb_config_get(unit, buffer_id, &config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_config_get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    config.scope_type = tcb->scope_type;
    config.gport = tcb->gport;
    config.trigger_reason = tcb->trigger_reason;

    /* Setting the config attributes of TCB instance */
    rv = bcm_cosq_tcb_config_set(unit, buffer_id, &config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_config_set: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    bcm_cosq_tcb_threshold_profile_t_init(&threshold);
    threshold.start_threshold_bytes = tcb->start_threshold;
    threshold.stop_threshold_bytes = tcb->stop_threshold;

    /* Create threshold profile for TCB instance */
    rv = bcm_cosq_tcb_threshold_profile_create(unit, flags, buffer_id,
                                               &threshold, profile_index);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_threshols_profile_create: %s.\n",
               bcm_errmsg(rv));
        return rv;
    }

    bcm_cosq_object_id_t_init(&object_id);
    object_id.port = tcb->gport;
    object_id.cosq = cosq;
    object_id.buffer = buffer_id;

    /* Associating the threshold profile with the UCQ or port */
    rv = bcm_cosq_tcb_gport_threshold_mapping_set(unit, &object_id,
                                                  profile_index);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_gport_threshold_mapping_set: %s.\n",
               bcm_errmsg(rv));
        return rv;
    }

    /* Enabling the specific TCB instance */
    rv = bcm_cosq_tcb_control_set(unit, buffer_id, bcmCosqTcbControlEnable,
                                  TRUE);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_control_set: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Steps to configure tcb for WRED drop and unicast queue as scope */

int
scope_unicastQueue(int unit, tcb_config_t *tcb_cfg)
{
    int event_count = 0;
    const int   max_buf_ids = 4;        /* There's always two TCB engines per port */
    const void *user_data = & event_count;
    bcm_cosq_buffer_id_t buf_id[max_buf_ids];
    int         array_count;
    int         idx;
    int         profile_index[max_buf_ids];
    bcm_error_t rv;

    /*
     * To get buffers ids associated with specified port
     * "cos" parameter is reserved and should be 0
     */

    rv = bcm_cosq_buffer_id_multi_get(unit, tcb_cfg->gport, 0,
                                      tcb_cfg->direction, max_buf_ids, buf_id,
                                      &array_count);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_buffer_multi_get: %s.\n",
               bcm_errmsg(rv));
        return rv;
    }

    /* To register callback to handle freeze events */
    rv = bcm_cosq_tcb_cb_register(unit, bcm_cosq_tcb_callback, user_data);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_tcb_cb_register: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Configuring TCB for the associated XPEs [buffer id] */
    for (idx = 0; idx < array_count; idx++) {
        rv = configure_wred_trigger(unit, buf_id[idx], profile_index[idx],
                                      tcb_cfg);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configure_egress_trigger: %s.\n",bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

int tcb_wred()
{
    int unit = 0;
    bcm_port_t port = 3;    /* WRED and tcb are configured on L2 queue of this port*/
    bcm_cos_queue_t cos = 0; /* cosq on which wred and tcb is configured */

    bcm_error_t rv;

    /* User can change the below configuration structures as required */
    wred_config_t wred_config = {
        port,
        cos,
        BCM_COSQ_DISCARD_COLOR_ALL | BCM_COSQ_DISCARD_BYTES| BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_NONTCP,
        124,
        248,
        100
    };

    cosq_gport_info_t cosq = {
		0,
        port,
        {0,0,0,0,0,0,0,0,0,0}
    };

    tcb_config_t tcb_config = {
        cosq.gport[cos],
        bcmCosqEgress,
        bcmCosqTcbScopeUcastQueue,
        BCM_COSQ_TCB_WRED_DROP,
        2500,
        1500
    };

    /* WRED configuration */
    rv = configure_wred(unit, &wred_config);

    if (BCM_FAILURE(rv)) {
        printf("WRED configuration failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

   /* To get the cosq gport of port 73 queue 0 */
   rv = bcm_cosq_gport_traverse(unit, gport_callback, &cosq);
   if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_gport_traverse: %s.\n",bcm_errmsg(rv));
        return rv;
   }

   tcb_config.gport = cosq.gport[cos];

   /* TCB configuration */

   rv = scope_unicastQueue(unit, &tcb_config);
   if (BCM_FAILURE(rv)) {
        printf("SCOPE UNICAST QUEUE TEST FAILED: %s\n", bcm_errmsg(rv));
        return rv;
   }

   return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rrv;
    
    if( (rrv = tcb_wred()) != BCM_E_NONE )
    {
        printf("Fail to configure TCB %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}