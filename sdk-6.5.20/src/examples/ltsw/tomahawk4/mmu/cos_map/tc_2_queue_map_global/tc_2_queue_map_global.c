/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : traffic class to queue mapping
 *
 *  Usage    : BCM.0> cint tc_2_queue_map_global.c
 *
 *  config   : tc_2_queue_map_global_config.bcm
 *
 *  Log file : tc_2_queue_map_global_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq gport APIs:
 *  ========
 * 	 bcm_cosq_mapping_set
 * 	 bcm_cosq_mapping_get
 * 	 bcm_cosq_gport_mapping_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate traffic class to queue mapping setting/getting globally.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Retrieve all the front panel port list and valid front panel port number
 *
 *    2) Step2 - Configuration (Done in tc_to_q_mapping_set())
 *    ======================================================
 *      a) Set the mapping from TC to queue via bcm_cosq_mapping_set.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result via tc_to_q_mapping_get
 *          Retrieve the mapping via bcm_cosq_mapping_get;
 *          Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports.
 *      b) Expected Result:
 *         ================
 *         The retrieved mapping should be aligned with the configured one as below:
 *                 TC:  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
 *            queue:  {3,2,1,0,7,6,5,4,4,3,  2,  1, 1,  2,  3, 4}
 */


cint_reset();

int unit=0;

/* Glabal variable definition */
int MAX_PORTS = 272;
int MAX_FP_PORTS = 256;
int MAX_TC = 16;

int valid_port_number = 0;
int port_list[MAX_FP_PORTS];

/* Tc to Q mapping */
int newTcToQMap[] = {3,2,1,0,7,6,5,4,4,3,2,1,1,2,3,4};

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

    /* Retrieved all the front panel port lists */
    if (BCM_E_NONE != portNumbersGet(unit, port_list, MAX_FP_PORTS)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    return BCM_E_NONE;
}

int tc_to_q_mapping_set (int unit)
{
    int rc = BCM_E_NONE;
    int tc = 0;
    int queue_id = 0;

    /* Set TC to Q mapping globally for both UC and MC with the same mapping */
    for(tc = 0; tc < MAX_TC; tc ++)
    {
        queue_id = newTcToQMap[tc];
        rc = bcm_cosq_mapping_set(unit, tc, queue_id);
        if (rc != BCM_E_NONE)
        {
            printf("Unicast: Unable to map TC to Egress queue (TC,Q) = (%d,%x) - %s\n",
                 tc, queue_id, bcm_errmsg(rc));
            return rc;
        }
          printf("Unicast: Map TC to Egress queue sucessfully (TC,Q) = (%d,%x) - %s\n",
                 tc, queue_id, bcm_errmsg(rc));
    }
    return rc;
}

int port_tc_to_q_mapping_get (int unit, int port)
{
  int rc = BCM_E_NONE;
  int tc = 0;
  int cosq = -1;
  int queue_gport = 0;

  /* TC to Unicast Q mapping get */
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

  /* TC to Multicast Q mapping get */
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
    int tc = 0;
    int cosq = -1;

    printf("\r\nMapping retrieved via bcm_cosq_mapping_get\r\n");
    printf("==========================================\r\n");
    for(tc = 0; tc < MAX_TC; tc ++)
    {
        rc = bcm_cosq_mapping_get(unit, tc, &cosq);
        if (rc != BCM_E_NONE)
        {
          printf("Unable to retrieve mapping via bcm_cosq_mapping_get\r\n");
          return rc;
        }
          printf("Mapping: TC %d was mapped to Egress queue %d global\n",
                 tc, cosq);
    }

    printf("\r\nMapping retrieved via bcm_cosq_gport_mapping_get\r\n");
    printf("==========================================\r\n");
    for (i = 0; i < valid_port_number; i++)
    {
      rc = port_tc_to_q_mapping_get(unit, port_list[i]);
      if (rc != BCM_E_NONE)
      {
          printf("Unable to retrieve mapping via bcm_cosq_gport_mapping_get\r\n");
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
    * Retrieve the mapping via bcm_cosq_mapping_get;
    * Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports.
    */
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

    /* Retrieve all the front panel port lists */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
    }

    /* Set TC to Q mapping globally for both UC and MC with the same mapping */
    if (BCM_FAILURE((rv = tc_to_q_mapping_set(unit)))) {
       printf("tc_to_q_mapping_set() failed.\n");
       return -1;
    }

    /* Verify the result via tc_to_q_mapping_get */
    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}

