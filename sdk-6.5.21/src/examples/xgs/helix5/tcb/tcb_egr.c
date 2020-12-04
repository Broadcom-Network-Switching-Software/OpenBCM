/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : TCB
 *  
 *  Usage    : BCM.0> cint tcb_egr.c
 *  
 *  config   : tcb_egr_config.bcm
 *  
 *  Log file : tcb_egr_log.txt
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
 *  This cint example demonstrates TCB configuration to detect egress
 *  admission control drop
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select two ingress and one egress ports
 *    2) Step2 - Configuration (Done in tcb_egress_config())
 *    =======================================================
 *      a) Sets up tcb on egress port(xe2) to detect ingress admission control
 *         drop
 *    3) Step3 - Verification
 *    =======================
 *      a) send at linerate to L2 unicast traffic onto two ingress ports xe0 and xe1
 *      b) Expected Result:
 *         ================
 *         TCB buffer and event buffer data is printed.
 */


/* Structure for tcb configuration and its threshold profile */
cint_reset();
int rv = 0;
int unit = 0;
int port;
bcm_port_t egr_gport; /*Egress gport */

struct tcb_config_t
{
    bcm_gport_t gport;               /* monitor entity - Unicast queue/Physical port */
    bcm_cosq_dir_t direction;        /* cosq direction - Ingress/Egress */
    bcm_cosq_tcb_scope_t scope_type; /* monitor scope type - Unicast queue/Ingress port/Egress port*/
    uint32 trigger_reason;           /* trigger event - Ingress admin drop/Egress admin drop/WRED/CFAP */
    uint32 start_threshold;          /* tcb start threshold in bytes */
    uint32 stop_threshold;           /* tcb stop threshold in bytes */
};

/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/

bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
     printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
     return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
       printf("portNumbersGet() failed \n");
       return -1;
  }

  return BCM_E_NONE;

}

/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports.
 */
bcm_error_t test_setup(int unit)
{
  int port_list[3], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  /* Get port gport */
  port = port_list[2];
  BCM_GPORT_LOCAL_SET(egr_gport, port);
  /*
  if (BCM_E_NONE != bcm_port_gport_get(unit, port, &egr_gport)) {
      printf("bcm_port_gport_get() failed.\n");
      return -1;
  }
  */
  return BCM_E_NONE;
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
    printf("Ing gport 0x%x\n",buffer_array.ingress_gport);
    printf("Egr gport 0x%x\n",buffer_array.egress_gport);
    printf("queue 0x%x\n",buffer_array.queue);
    printf("queue size 0x%x\n",buffer_array.queue_size);
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
    printf("Ing gport 0x%x\n",event_array.ingress_gport);
    printf("Egr gport 0x%x\n",event_array.egress_gport);
    printf("Drop reason %d\n",event_array.drop_reason);
    printf("queue 0x%x\n",event_array.queue);
    printf("queue size 0x%x\n",event_array.queue_size);

}

/* Helper routine for configuring tcb for Egress Admission Drop */
int
configure_egress_trigger(int unit, int buffer_id, int profile_index,
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
    /* Associating the threshold profile with the port or UCQ */
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

/* Steps to configure tcb for Egress Admisssion Drop and Egress Port as scope */
int
scope_egress(int unit, tcb_config_t *tcb_cfg)
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
     * To get buffer ids associated with specified port
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
        rv = configure_egress_trigger(unit, buf_id[idx], profile_index[idx],
                                      tcb_cfg);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configure_egress_trigger: %s.\n",bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

int tcb_egress_config(bcm_gport_t gport)
{
    bcm_error_t rv;

    tcb_config_t tcb_config;
    tcb_config.gport = gport;               /* monitor entity - Unicast queue/Physical port */
    tcb_config.direction = bcmCosqEgress;        /* cosq direction - Ingress/Egress */
    tcb_config.scope_type = bcmCosqTcbScopeEgressPort; /* monitor scope type - Unicast queue/Ingress port/Egress port*/
    tcb_config.trigger_reason = BCM_COSQ_TCB_EGRESS_ADMIN_DROP;           /* trigger event - Ingress admin drop/Egress admin drop/WRED/CFAP */
    tcb_config.start_threshold = 2500;          /* tcb start threshold in bytes */
    tcb_config.stop_threshold = 1500;           /* tcb stop threshold in bytes */

   /* TCB configuration */
   rv = scope_egress(unit, &tcb_config);
   if (BCM_FAILURE(rv)) {
        printf("SCOPE EGRESS TEST FAILED: %s\n", bcm_errmsg(rv));
        return rv;
   }

   return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rrv;
    int unit=0;

    bshell(unit, "config show; a ; version");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    
    if( (rrv = tcb_egress_config(egr_gport)) != BCM_E_NONE )
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

