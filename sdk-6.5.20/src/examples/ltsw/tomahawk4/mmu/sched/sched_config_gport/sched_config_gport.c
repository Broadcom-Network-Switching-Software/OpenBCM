/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : scheduling policy, and weights
 *
 *  Usage    : BCM.0> cint sched_config_gport.c
 *
 *  config   : sched_config_gport.bcm
 *
 *  Log file : sched_config_gport_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 * 	 bcm_cosq_gport_sched_set
 * 	 bcm_cosq_gport_sched_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate configuring scheduling policy, and weights on gport base.
 *  The input gport can be UC, MC, and Sch type.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Retrieve all the gport, front panel port list and valid front panel port number.
 *
 *    2) Step2 - Configuration (Done in sched_weight_config())
 *    ======================================================
 *      a) Configure scheduling policy, and weights for all UC, MC, and Sch belong
 *          to all valid front panel ports.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the configured scheduling mode and weight using 'dump sw cosq';
 *          On the other hand, retrieve and print the configured scheduling policy, and weights for
 *          all UC, MC, and Sch belongto all valid front panel ports via bcm_cosq_gport_sched_get.
 *      b) Expected Result:
 *         ================
 *         In 'dump sw cosq' output and printing of gport_sched_weight_get
 *             - Weight of sch node/UC/MC 0-11 for all valid ports were confiugured to 0-11
 */

cint_reset();

int unit=0;

/* Glabal variable definition */
int CPU_PORT = 0;
int MAX_PORTS = 272;
int MAX_FP_PORTS = 256;
int MAX_QUEUES = 12;
int MAX_UC_QUEUES = 8;
int MAX_MC_QUEUES = 4;
int MAX_CPU_QUEUES = 48;
int MAX_SCH_NODES = 12;

int prev_port = -1;
int free_ucast_index = 0;
int free_mcast_index = 0;
int free_sched_index = 0;
int valid_port_number = 0;
int port_list[MAX_FP_PORTS];

bcm_gport_t ucast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t mcast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t sched_gport[MAX_PORTS][MAX_SCH_NODES];
bcm_gport_t cpu_mcast_q_gport[MAX_CPU_QUEUES];
bcm_gport_t cpu_sched_gport[MAX_SCH_NODES];


/* This function gives required number of ports. */
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
    for (port= 1; port < MAX_PORTS; port++) {
        port_list[i] = -1;
        if (BCM_PBMP_MEMBER(&ports_pbmp,port)&& (i < num_ports)) {
            port_list[i] = port;
            i++;
        }
    }

    if (i == 0) {
       printf("portNumbersGet() failed \n");
       return -1;
    }
    valid_port_number = i;

    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    /* Retrieve all the gport */
    BCM_IF_ERROR_RETURN(cosq_gport_traverse(unit, cosq_gport_traverse_callback, NULL));

    /* Retrieved all the front panel port lists */
    if (BCM_E_NONE != portNumbersGet(unit, port_list, MAX_FP_PORTS)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    return BCM_E_NONE;
}

static bcm_error_t cosq_gport_traverse_callback(int unit, bcm_gport_t port,
                                     int numq, uint32 flags,
                                     bcm_gport_t gport, void *user_data)
{
     int local_port = 0;
     int is_cpu_port;

     switch (flags)
     {
         case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 free_ucast_index = 0;
                 free_mcast_index = 0;
                 free_sched_index = 0;
                 prev_port = local_port;
                 printf("cosq_gport_traverse_callback: gport for local_port: %d\n",local_port);
             }

             /*printf("cosq_gport_traverse_callback ucast queue: local_port=%d, mod_port=0x%x, numq=%d, gport=0x%x\n",
                local_port, port, numq, gport);*/
             is_cpu_port = ((CPU_PORT == local_port) ? 1 : 0);
             if (is_cpu_port)
                 return BCM_E_PARAM;
             ucast_q_gport[local_port][free_ucast_index] = gport;
             free_ucast_index++;
             break;

         case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 free_ucast_index = 0;
                 free_mcast_index = 0;
                 free_sched_index = 0;
                 prev_port = local_port;
                 printf("cosq_gport_traverse_callback: gport for local_port: %d\n",local_port);
             }

             /*printf("cosq_gport_traverse_callback mcast queue: local_port=%d, mod_port=0x%x, numq=%d, gport=0x%x\n",
                local_port, port, numq, gport);*/
             is_cpu_port = ((CPU_PORT == local_port) ? 1 : 0);
             if (is_cpu_port) {
                 cpu_mcast_q_gport[free_mcast_index] = gport;
             } else {
                 mcast_q_gport[local_port][free_mcast_index] = gport;
             }
             free_mcast_index++;
             break;

         case BCM_COSQ_GPORT_SCHEDULER:
             /*printf("cosq_gport_traverse_callback scheduler: local_port=%d, mod_port=0x%x, numq=%d, gport=0x%x\n",
                prev_port, port, numq, gport);*/
             is_cpu_port = ((CPU_PORT == prev_port) ? 1 : 0);
             if (is_cpu_port) {
                 cpu_sched_gport[free_sched_index] = gport;
             } else {
                 sched_gport[prev_port][free_sched_index] = gport;
             }
             free_sched_index++;
             break;
         default:
             return BCM_E_PARAM;
     }

    return 1;
 }

static bcm_error_t cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                            void *user_data)
{
    int ret_val = 0;

    ret_val = bcm_cosq_gport_traverse(unit, cb, NULL);
    if (ret_val  < BCM_E_NONE) {
        printf("ERR(%d): bcm_cosq_gport_traverse function unit = %d\n", ret_val, unit);
        return -1;
    }

    free_ucast_index = 0;
    free_mcast_index = 0;
    free_sched_index = 0;
    prev_port = -1;

    return 1;
}

int gport_sched_weight_config (int unit, int port)
{
    int rc = BCM_E_NONE;
    int queue_gport = 0;
    int cos, mode, weight;

    mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;

    /* Unicast Q sheduling mode and weight config */
    for(cos = 0; cos < MAX_UC_QUEUES; cos++) {
        weight = cos;
        queue_gport = ucast_q_gport[port][cos];
        rc = bcm_cosq_gport_sched_set(unit, queue_gport, cos, mode, weight);

        if (rc != BCM_E_NONE)
        {
            printf("Unicast: Unable to config Q sheduling mode and weight for (P,cos) = (%d,%d) - %s\n",
                 port, cos, bcm_errmsg(rc));
            return rc;
        }
        printf("Unicast: config Q sheduling mode and weight (P,cos: mode,weight) = (%d,%d: %d,%d) - %s\n",
             port, cos, mode, weight, bcm_errmsg(rc));
    }

    /* Multicast Q sheduling mode and weight config */
    for(cos = 0; cos < MAX_MC_QUEUES; cos++) {
        weight = cos;
        queue_gport = mcast_q_gport[port][cos];
        rc = bcm_cosq_gport_sched_set(unit, queue_gport, cos, mode, weight);

        if (rc != BCM_E_NONE)
        {
            printf("Multicast: Unable to config Q sheduling mode and weight for (P,cos) = (%d,%d) - %s\n",
                 port, cos, bcm_errmsg(rc));
            return rc;
        }
        printf("Multicast: config Q sheduling mode and weight (P,cos: mode,weight) = (%d,%d: %d,%d) - %s\n",
             port, cos, mode, weight, bcm_errmsg(rc));
    }

    /* Sch node sheduling mode and weight config */
    for(cos = 0; cos < MAX_QUEUES; cos++) {
      weight = cos;
      queue_gport = sched_gport[port][cos];
      rc = bcm_cosq_gport_sched_set(unit, queue_gport, cos, mode, weight);

      if (rc != BCM_E_NONE)
      {
          printf("Sch node: Unable to config sheduling mode and weight for (P,cos) = (%d,%d) - %s\n",
               port, cos, bcm_errmsg(rc));
          return rc;
      }
      printf("Sch node: config sheduling mode and weight (P,cos: mode,weight) = (%d,%d: %d,%d) - %s\n",
           port, cos, mode, weight, bcm_errmsg(rc));
    }

  return rc;
}

/* 
 *  Configure scheduling policy, and weights for all UC, MC, and Sch belong
 *  to all valid front panel ports.
 */
int sched_weight_config (int unit)
{
  int rc = BCM_E_NONE;
  int i = 0;

  for (i = 0; i < valid_port_number; i++)
  {
      rc = gport_sched_weight_config(unit, port_list[i]);
      if (rc != BCM_E_NONE)
      {
          printf("Unable to gport_sched_weight_config on port %d\r\n", port_list[i]);
          return rc;
      }
  }

  return rc;
}

int gport_sched_weight_get (int unit, int port)
{
      int rc = BCM_E_NONE;
      int queue_gport = 0;
      int cos, mode, weight;
    
      mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
    
      /* Unicast Q sheduling mode and weight config */
      for(cos = 0; cos < MAX_UC_QUEUES; cos++) {
          weight = -1;
          mode = -1;
          queue_gport = ucast_q_gport[port][cos];
          rc = bcm_cosq_gport_sched_get(unit, queue_gport, cos, &mode, &weight);
    
          if (rc != BCM_E_NONE)
          {
              printf("Unicast: Unable to get Q sheduling mode and weight for (P,cos) = (%d,%d) - %s\n",
                   port, cos, bcm_errmsg(rc));
              return rc;
          }
          printf("Unicast: Retrieved Q sheduling mode and weight (P,cos: mode,weight) = (%d,%d: %d,%d) - %s\n",
               port, cos, mode, weight, bcm_errmsg(rc));
      }
    
      /* Multicast Q sheduling mode and weight config */
      for(cos = 0; cos < MAX_MC_QUEUES; cos++) {
          weight = -1;
          mode = -1;
          queue_gport = mcast_q_gport[port][cos];
          rc = bcm_cosq_gport_sched_get(unit, queue_gport, cos, &mode, &weight);
    
          if (rc != BCM_E_NONE)
          {
              printf("Multicast: Unable to get Q sheduling mode and weight for (P,cos) = (%d,%d) - %s\n",
                   port, cos, bcm_errmsg(rc));
              return rc;
          }
          printf("Multicast: Retrieved Q sheduling mode and weight (P,cos: mode,weight) = (%d,%d: %d,%d) - %s\n",
               port, cos, mode, weight, bcm_errmsg(rc));
      }
    
      /* Sch node sheduling mode and weight config */
      for(cos = 0; cos < MAX_QUEUES; cos++) {
          weight = -1;
          mode = -1;
          queue_gport = sched_gport[port][cos];
          rc = bcm_cosq_gport_sched_get(unit, queue_gport, cos, &mode, &weight);
    
          if (rc != BCM_E_NONE)
          {
              printf("Sch node: Unable to get sheduling mode and weight for (P,cos) = (%d,%d) - %s\n",
                   port, cos, bcm_errmsg(rc));
              return rc;
          }
          printf("Sch node: Retrieved sheduling mode and weight (P,cos: mode,weight) = (%d,%d: %d,%d) - %s\n",
               port, cos, mode, weight, bcm_errmsg(rc));
      }
    
    return rc;

}

/* 
 *  Retrieve and print the configured scheduling policy, and weights for all UC, MC,
 *  and Sch belongto all valid front panel ports.
 */
int sched_weight_get (int unit)
{
    int rc = BCM_E_NONE;
    int i = 0;

    for (i = 0; i < valid_port_number; i++)
    {
      rc = gport_sched_weight_get(unit, port_list[i]);
      if (rc != BCM_E_NONE)
      {
          printf("Unable to gport_sched_weight_get on port %d\r\n", port_list[i]);
          return rc;
      }
    }

    return rc;
}

/* Function to verify the result */
bcm_error_t verify(int unit)
{
    bcm_error_t rv;

    /* 
     *  Retrieve and print the configured scheduling policy, and weights for all UC, MC,
     *  and Sch belong to all valid front panel ports.
     */
    if (BCM_FAILURE((rv = sched_weight_get(unit)))) {
       printf("sched_weight_get() failed.\n");
       return -1;
    }
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");

    /* Retrieve all the gport and all the front panel port lists */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
    }

    /* 
       *  Configure scheduling policy, and weights for all UC, MC, and Sch belong
       *  to all valid front panel ports.
       */
    if (BCM_FAILURE((rv = sched_weight_config(unit)))) {
       printf("sched_weight_config() failed.\n");
       return -1;
    }

    /* 
       *  Retrieve and print the configured scheduling policy, and weights for all UC, MC,
       *  and Sch belongto all valid front panel ports.
       */
    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}


