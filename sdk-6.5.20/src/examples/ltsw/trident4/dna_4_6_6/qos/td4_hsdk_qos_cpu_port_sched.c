/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : CPU port scheduler 
 *
 *  Usage    : BCM.0> cint td4_hsdk_qos_cpu_port_sched.c
 *
 *  config   : bcm56880_a0-generic-32x400.config.yml 
 *
 *  Log file : td4_hsdk_qos_cpu_port_sched_log.txt
 *
 *  Test Topology : None
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate creating customized cpu port hierarchy on TD4 
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) set port to cpu port on which custom hieracrhy is created 
 *    2) Step2 - Configuration (Done in cpu_port_sched_config())
 *    ======================================================
 *      a) Create customized cpu port hierarchy as specified in 
 *         cpu queue to L0 node mapping array - cpu_queue_map_to_l0[].  
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify cpu port hiearchy using 'dump sw cosq'
 *      b) Expected Result:
 *         ================
 *         In 'dump sw cosq' output
 *             - CPU queues 0-47 should be attached to L0.0-L0.11 nodes as specified in cpu_queue_map_to_l0
 */

cint_reset();
int unit = 0;
bcm_port_t port = 0;
int NUM_CPU_Q = 48;
int NUM_PORT_L0 = 12;
bcm_gport_t cpu_cosq_gp_l0[NUM_PORT_L0];
bcm_gport_t cpu_cosq_gp_mc[NUM_CPU_Q];
int i = 0;
int j = 0;
int l0_node=0;
int user_data;
bcm_cos_queue_t cosq;
bcm_gport_t input_port;

/* to get the current Queue to L0 attachment */
struct q_to_l0_s {
  bcm_gport_t L0;
  bcm_cos_queue_t cosq;
};

q_to_l0_s q_to_l0[48];

/* cpu_queue_map_to_l0 - array of size 48 is used to specifiy cpu queues to L0 node mapping. 
 * For TD4, there are 12 L0 nodes per port, L0.0-L0.11 
 */

/* user supplied array of mapping between cpu queues to L0 node */
int cpu_queue_map_to_l0[48] = {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5};

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

bcm_error_t test_setup(int unit)
{
  int port_list[1], i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }
 
  /* For this cint example port is cpu port. Hence port is assigned as 0 */
  port = 0;
  return BCM_E_NONE;
}

/* Create custom hierarchy as per user specified mapping in - cpu_queue_map_to_l0 */ 
bcm_error_t cpu_port_sched_config(int unit, bcm_gport_t port)
{
  int rv=0;  
  /* get the cpu L0 and Queue port gports */
  rv = bcm_cosq_gport_traverse(unit, cpu_cosq_gport_traverse_cb, &user_data );
  if (rv != BCM_E_NONE) {
      printf ("bcm_cosq_gport_traverse failed() :%s\n", bcm_errmsg(rv));
      return rv;
  }
 
  /* retrieve current cpu queue to L0 node mapping */
  for (i=0; i< 48; ++i) {
    rv = bcm_cosq_gport_attach_get(unit, cpu_cosq_gp_mc[i], &input_port, &cosq);
    if (rv != BCM_E_NONE) {
        printf ("bcm_cosq_gport_attach_get failed() :%s\n", bcm_errmsg(rv));
        return rv;
    }
    q_to_l0[i].L0 = input_port;
    q_to_l0[i].cosq = cosq;
  }
 
  /* Detach all cpu queues from L0 nodes */
  for (i=0; i< 48; ++i) {
    rv = bcm_cosq_gport_detach(unit, cpu_cosq_gp_mc[i], q_to_l0[i].L0, q_to_l0[i].cosq);
    if (rv != BCM_E_NONE) {
        printf ("bcm_cosq_gport_detach failed() :%s\n", bcm_errmsg(rv));
        return rv;
    }
  }

  /* Attach cpu queues to L0 node as per user provided mapping in cpu_queue_map_to_l0 */
  for (i=0; i< 48; ++i) {
     l0_node = cpu_queue_map_to_l0[i];
     rv = bcm_cosq_gport_attach(unit, cpu_cosq_gp_mc[i], cpu_cosq_gp_l0[l0_node], i); 
     if (rv != BCM_E_NONE) {
         printf ("bcm_cosq_gport_attach_get failed() :%s\n", bcm_errmsg(rv));
         return rv;
     }
  }

 /* Store the current queue to L0 node mapping */
 for (i=0; i< 48; ++i) {
   rv = bcm_cosq_gport_attach_get(unit, cpu_cosq_gp_mc[i], &input_port, &cosq);
   if (rv != BCM_E_NONE) {
       printf ("bcm_cosq_gport_attach_get failed() :%s\n", bcm_errmsg(rv));
       return rv;
   }
   q_to_l0[i].L0 = input_port;
   q_to_l0[i].cosq = cosq;
 }
 return BCM_E_NONE; 
}

/* Traverse callback function - stores cpu port L0 gports and Queue gports */
bcm_error_t cpu_cosq_gport_traverse_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{

    int local_port;

    local_port = BCM_GPORT_MODPORT_PORT_GET(port);     
    if (local_port == 0) {
      if (flags & BCM_COSQ_GPORT_SCHEDULER)
      {
            cpu_cosq_gp_l0[i++] = gport;
          }
        else if (flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)
          {
            cpu_cosq_gp_mc[j++] = gport;
          }
        else
          {
            printf("ERR: INVALID FLAG for port 0x%x(localport %d)\n",
                    port, local_port);
          }
    }
    return BCM_E_NONE;
}

bcm_error_t td4_dump_cpu_gports (int port)
{
  int start = 0, end = 0, idx, g_idx, num_q = 0, n_idx, q_idx;
  if (port == 0) {
      printf("\nPrinting Gport Hierarchy for CPU Port %d\n", port);
      for (n_idx = 0; n_idx < NUM_PORT_L0; n_idx++) {
           printf ("    L0.%d: 0x%x\n", n_idx, cpu_cosq_gp_l0[n_idx]);
		for (q_idx = 0; q_idx < NUM_CPU_Q; q_idx++) {
                     if (q_to_l0[q_idx].L0 == cpu_cosq_gp_l0[n_idx]) {
	       	       printf ("        MC.%d: 0x%x\n", q_idx, cpu_cosq_gp_mc[q_idx]);
                     } 
         	}
        }
   }
   return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit)
{
  /*check created cpu port custom scheduler hieararchy */
  td4_dump_cpu_gports(0);
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  /* select port */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* configure scheduler profile and attach it to port*/
  if (BCM_FAILURE((rv = cpu_port_sched_config(unit, port)))) {
      printf("port_sched_config() failed.\n");
      return -1;
  }

  /* verify the result */
  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
