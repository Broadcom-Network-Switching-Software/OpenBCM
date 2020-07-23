/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : traffic class to queue mapping
 *
 *  Usage    : BCM.0> cint tc_2_queue_map_per_port.c
 *
 *  config   : tc_2_queue_map_per_port_config.bcm
 *
 *  Log file : tc_2_queue_map_per_port_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq gport APIs:
 *  ========
 * 	 bcm_cosq_gport_mapping_set
 * 	 bcm_cosq_gport_mapping_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate per port traffic class to queue mapping setting/getting.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Retrieve all the gport, front panel port list and valid front panel port number.
 *
 *    2) Step2 - Configuration (Done in tc_to_q_mapping_set())
 *    ======================================================
 *      a) Set the mapping from TC to queue via bcm_cosq_gport_mapping_set.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result via tc_to_q_mapping_get
 *          Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports.
 *      b) Expected Result:
 *         ================
 *         The retrieved mapping should be aligned with the configured one as below:
 *                       TC:  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
 *       queue(for UC):  {3,2,1,0,7,6,5,4,4,3,  2,  1, 1,  2,  3, 4}
 *       queue(for MC):  {3,2,1,0,3,3,3,3,3,3,  2,  1, 1,  2,  3, 3}
 */

cint_reset();

int unit=0;

/* Glabal variable definition */
int CPU_PORT = 0;
int MAX_PORTS = 272;
int MAX_FP_PORTS = 256;
int MAX_QUEUES = 12;
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

/* Tc to Q mapping */
int MAX_TC = 16;
int newUcTcToQMap[] = {7,6,5,4,3,2,1,0,4,3,2,1,1,2,3,4};
int newMcTcToQMap[] = {3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0};

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
             is_cpu_port = ((CPU_PORT == local_port) ? 1 : 0);
             if (is_cpu_port) {
                 cpu_sched_gport[free_sched_index] = gport;
             } else {
                 sched_gport[local_port][free_sched_index] = gport;
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

int port_tc_to_q_mapping_set (int unit, int port)
{
  int rc = BCM_E_NONE;
  int tc = 0;
  int queue_id = 0;
  int queue_gport = 0;

  /* Tc to Unicast Q mapping set */
  for(tc = 0; tc < MAX_TC; tc ++)
  {
      queue_id = newUcTcToQMap[tc];
      queue_gport = ucast_q_gport[port][queue_id];
      rc = bcm_cosq_gport_mapping_set(unit, port, tc,
                                      BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                      queue_gport, 0);
      if (rc != BCM_E_NONE)
      {
          printf("Unicast: Unable to map TC to Egress queue (P,TC,Q) = (%d,%d,%x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rc));
          return rc;
      }
          printf("Unicast: Map TC to Egress queue sucessfully (P,TC,Q) = (%d,%d,%x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rc));
  }

  /* Tc to Multicast Q mapping set */
  for(tc = 0; tc < MAX_TC; tc ++)
  {
      queue_id = newMcTcToQMap[tc];
      queue_gport = mcast_q_gport[port][queue_id];
      rc = bcm_cosq_gport_mapping_set(unit, port, tc,
                                      BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                      queue_gport, 0);
      if (rc != BCM_E_NONE)
      {
          printf("Multicast: Unable to map TC to Egress queue (P,TC,Q) = (%d,%d,%x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rc));
          return rc;
      }
          printf("Multicast: map TC to Egress queue sucessfully (P,TC,Q) = (%d,%d,%x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rc));
  }

  return rc;
}

int tc_to_q_mapping_set (int unit)
{
  int rc = BCM_E_NONE;
  int i = 0;

  for (i = 0; i < valid_port_number; i++)
  {
      rc = port_tc_to_q_mapping_set(unit, port_list[i]);
      if (rc != BCM_E_NONE)
      {
          printf("Unable to tc_to_q_mapping_set on port %d\r\n", port_list[i]);
          return rc;
      }
  }

  return rc;
}

int port_tc_to_q_mapping_get (int unit, int port)
{
  int rc = BCM_E_NONE;
  int tc = 0;
  int cosq = -1;
  int queue_gport = 0;

  /* Tc to Unicast Q mapping get */
  for(tc = 0; tc < MAX_TC; tc ++)
  {
      rc = bcm_cosq_gport_mapping_get(unit, port, tc,
                                      BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                      &queue_gport, &cosq);
      if (rc != BCM_E_NONE)
      {
          printf("Unicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                 tc, port, bcm_errmsg(rc));
          return rc;
      }
          printf("Unicast: TC %d of port %d was mapped to Egress queue %d\n",
                 tc, port, cosq, bcm_errmsg(rc));
  }

  /* Tc to Multicast Q mapping get */
  for(tc = 0; tc < MAX_TC; tc ++)
  {
      rc = bcm_cosq_gport_mapping_get(unit, port, tc,
                                      BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                      &queue_gport, &cosq);
      if (rc != BCM_E_NONE)
      {
          printf("Multicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                 tc, port, bcm_errmsg(rc));
          return rc;
      }
          printf("Multicast: TC %d of port %d was mapped to Egress queue %d\n",
                 tc, port, cosq, bcm_errmsg(rc));
  }

  return rc;
}

int tc_to_q_mapping_get (int unit)
{
  int rc = BCM_E_NONE;
  int i = 0;

  for (i = 0; i < valid_port_number; i++)
  {
      rc = port_tc_to_q_mapping_get(unit, port_list[i]);
      if (rc != BCM_E_NONE)
      {
          printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", port_list[i]);
          return rc;
      }
  }

  return rc;
}

/* Function to verify the result */
bcm_error_t verify(int unit)
{
    bcm_error_t rv;

    /* Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports. */
    if (BCM_FAILURE((rv = tc_to_q_mapping_get(unit)))) {
       printf("tc_to_q_mapping_get() failed.\n");
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

    /* Set the mapping from TC to queue via bcm_cosq_gport_mapping_set */
    if (BCM_FAILURE((rv = tc_to_q_mapping_set(unit)))) {
       printf("tc_to_q_mapping_set() failed.\n");
       return -1;
    }

    /* Retrieve and verify the mapping via bcm_cosq_gport_mapping_get for all the valid
        *  front panel ports
        */
    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}

