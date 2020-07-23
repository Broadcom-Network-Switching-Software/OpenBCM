/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : classifier
 *
 *  Usage    : BCM.0> cint cosq_control.c
 *
 *  config   :
 *
 *  Log file : cosq_control_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 *    bcm_cosq_control_dynamic_get
 *    bcm_cosq_control_set
 *    bcm_cosq_control_get
 *    bcm_cosq_control_extended_set
 *    bcm_cosq_control_extended_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate cosq control set/get for different cosq control types.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Retrieved the port to be operated on.
 *    ======================================================
 *    2) Step2 - verify (Done in verify())
 *    ==========================================
 *      a) Carry set and get operation over different cosq control types, then compare the values.
 */

cint_reset();

uint16_t UC = 1;
uint16_t MC = 2;

typedef struct cosq_control_test_t {
    bcm_cosq_control_t type;    /* cosq feature. */
    int arg;                    /* cosq feature value. */
    uint16_t flags;             /* UC/MC flag. */
};

/* GLOBAL VARIABLES */
int unit = 0;

cosq_control_test_t cosq_control_bindings[] =
{
    {bcmCosqControlBandwidthBurstMax, 2048, UC},
    {bcmCosqControlBandwidthBurstMin, 1024, UC},
    {bcmCosqControlEgressPool, 1, UC},
    {bcmCosqControlUCEgressPool, 1, UC},
    {bcmCosqControlMCEgressPool, 1, MC},
    {bcmCosqControlEgressPoolLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolYellowLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolRedLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolLimitEnable, 1, UC},
    {bcmCosqControlEgressUCQueueSharedLimitBytes, 2032, UC},
    {bcmCosqControlEgressMCQueueSharedLimitBytes, 2032, MC},
    {bcmCosqControlEgressUCQueueResumeOffsetBytes, 2032, UC},
    {bcmCosqControlEgressMCQueueResumeOffsetBytes, 2032, MC},
    {bcmCosqControlEgressUCQueueMinLimitBytes, 2032, UC},
    {bcmCosqControlEgressMCQueueMinLimitBytes, 2032, MC},
    {bcmCosqControlEgressUCSharedDynamicEnable, 1, UC},
    {bcmCosqControlEgressMCSharedDynamicEnable, 1, MC},
    {bcmCosqControlEgressUCQueueLimitEnable, 1, UC},
    {bcmCosqControlEgressMCQueueLimitEnable, 1, MC},
    {bcmCosqControlEgressUCQueueColorLimitDynamicEnable, 1, UC},
    {bcmCosqControlEgressMCQueueColorLimitDynamicEnable, 1, MC},
    {bcmCosqControlEgressUCQueueColorLimitEnable, 1, UC},
    {bcmCosqControlEgressMCQueueColorLimitEnable, 1, MC},
    {bcmCosqControlEgressUCQueueRedLimit, 2, UC},
    {bcmCosqControlEgressUCQueueYellowLimit, 2, UC},
    {bcmCosqControlEgressMCQueueRedLimit, 1, MC},
    {bcmCosqControlEgressMCQueueYellowLimit,1, MC},
    {bcmCosqControlEgressUCQueueGroupMinEnable, 1, UC},
    {bcmCosqControlEgressMCQueueGroupMinEnable, 1, MC},
    {bcmCosqControlEgressUCQueueGroupMinLimitBytes, 2032, UC},
    {bcmCosqControlEgressMCQueueGroupMinLimitBytes, 2032, MC},
    {bcmCosqControlEgressPoolSharedLimitBytes, 3048, UC},
    {bcmCosqControlEgressPoolResumeLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolYellowSharedLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolYellowResumeLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolRedSharedLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolRedResumeLimitBytes, 2032, UC},
    {bcmCosqControlEgressPoolHighCongestionLimitBytes, 1524, UC},
    {bcmCosqControlEgressPoolLowCongestionLimitBytes, 508, UC},
    {bcmCosqControlEgressPoolAdtMargin0, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin1, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin2, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin3, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin4, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin5, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin6, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin7, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin8, 2032, UC},
    {bcmCosqControlEgressPoolAdtMargin9, 2032, UC},
    {bcmCosqControlEgressPoolAdtHighPriAlpha, 1, UC},
    {bcmCosqControlEgressPortPoolYellowLimitBytes,  2032, UC},
    {bcmCosqControlEgressPortPoolRedLimitBytes, 2032, UC},
    {bcmCosqControlEgressPortPoolSharedLimitBytes, 2032, UC},
    {bcmCosqControlEgressPortPoolSharedResumeBytes, 2032, UC},
    {bcmCosqControlEgressPortPoolYellowResumeBytes, 2032, UC},
    {bcmCosqControlEgressPortPoolRedResumeBytes, 2032, UC},
    {bcmCosqControlIngressPortPGSharedLimitBytes, 2032, UC},
    {bcmCosqControlIngressPortPGMinLimitBytes, 2032, UC},
    {bcmCosqControlIngressPortPGHeadroomLimitBytes, 2032, UC},
    {bcmCosqControlIngressPortPGResetFloorBytes, 2032, UC},
    {bcmCosqControlIngressPortPGResetOffsetBytes, 2032, UC},
    {bcmCosqControlIngressPortPGSharedDynamicEnable, 1, UC},
    {bcmCosqControlIngressPortPoolMinLimitBytes, 508, UC},
    {bcmCosqControlIngressPortPoolSharedLimitBytes, 1524, UC},
    {bcmCosqControlIngressPortPoolResumeLimitBytes, 2032, UC},
    {bcmCosqControlIngressPool, 1, UC},
    {bcmCosqControlIngressHeadroomPool, 1, UC},
    {bcmCosqControlIngressPoolLimitBytes, 508, UC},
    {bcmCosqControlIngressPoolResetOffsetLimitBytes, 2032, UC},
    {bcmCosqControlIngressHeadroomPoolLimitBytes, 1524, UC},
    {bcmCosqControlDropLimitAlpha, 1, UC},
    {bcmCosqControlAdtPriGroup, 1, UC},
    {bcmCosqControlAdtLowPriMonitorPool, 1, UC},
};

bcm_port_t port;
bcm_gport_t gport;
bcm_cos_queue_t cosq = 2;
bcm_cos_queue_t cosq_mc = 8;
bcm_cosq_control_t type;
int arg, out_arg;

/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/

bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0, port=0, rv=0;
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

  port = port_list[0];
  return BCM_E_NONE;
}

int cosq_control_test(int unit)
{
    int rv = BCM_E_NONE;
    int i, max_bindings;
    int temp_type;
    bcm_cos_queue_t temp_cosq;
    bcm_port_t temp_port;
    bcm_cosq_dynamic_setting_type_t dynamic;

    printf("************************ UC test start ************************\n" );

    /*BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(gport, port, cosq);*/
	gport = port;

    max_bindings = sizeof(cosq_control_bindings) / sizeof(cosq_control_bindings[0]);
    for (i = 0; i < max_bindings; i++) {
        type = cosq_control_bindings[i].type;
        arg = cosq_control_bindings[i].arg;
        temp_type = type;
        if (cosq_control_bindings[i].flags & UC) {
            printf("Cosq control type %d is applicable to UC\n", temp_type);

            if (temp_type == bcmCosqControlAdtLowPriMonitorPool) {
                temp_port = -1;
                temp_cosq = -1;
            } else {
                temp_port = port;
                temp_cosq = cosq;
            }
        } else if (cosq_control_bindings[i].flags & MC) {
            printf("Cosq control type %d is applicable to MC\n", temp_type);
            temp_port = port;
            temp_port = cosq_mc;
        } else {
            printf("Cosq control type %d is not applicable to both UC&MC, skipped\n", temp_type);
            continue;
        }

        rv = bcm_cosq_control_dynamic_get(unit, type, &dynamic);
        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_control_dynamic_get FAILED for type %d. rv %d\n",
                    temp_type, rv);
            continue;
        } else if (dynamic == bcmCosqDynamicTypeFixed)
        {
            printf("ERR. Cosq control type %d is fixed\n", temp_type);
            continue;
        }

        rv = bcm_cosq_control_set(unit, temp_port, temp_cosq, type, arg);

        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_control_set FAILED. rv %d\n", rv);
        }

        out_arg = -1;
        rv = bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_control_get FAILED. rv %d\n", rv);
        }

        if (cosq_control_bindings[i].flags & UC) {
            if (arg != out_arg)
            {
                printf("=========================ERROR================================\n");
                printf("ERR. Set/get not match for UC type %d, value %d of port %d, cosq %d\n",
                    temp_type, arg, port, temp_cosq);
                printf("Retrieved value %d \n", out_arg);
            }
            printf("************UC test finished for type: %d ************\n", temp_type);

        } else if (cosq_control_bindings[i].flags & MC) {
            if (arg != out_arg)
            {
                printf("=========================ERROR================================\n");
                printf("ERR. Set/get not match for MC type %d, value %d of port %d, cosq %d\n",
                    temp_type, arg, port, temp_cosq);
                printf("Retrieved value %d \n", out_arg);
            }
            printf("************MC test finished for type: %d ************\n", temp_type);
        }

    }

/*    return rv;*/
    return BCM_E_NONE;

}


int cosq_control_extend_test(int unit)
{
    int rv = BCM_E_NONE;
    int i, max_bindings;
    int temp_type;
    bcm_cosq_object_id_t id;
    bcm_cosq_control_data_t control;
    bcm_cosq_dynamic_setting_type_t dynamic;

    printf("************************ UC test start ************************\n" );

    /*BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(gport, port, cosq);*/
	gport = port;
    id.buffer = 0;

    max_bindings = sizeof(cosq_control_bindings) / sizeof(cosq_control_bindings[0]);
    for (i = 0; i < max_bindings; i++) {
        type = cosq_control_bindings[i].type;
        arg = cosq_control_bindings[i].arg;
        temp_type = type;
        if (cosq_control_bindings[i].flags & UC) {
            printf("Cosq control type %d is applicable to UC\n", temp_type);

            if (temp_type == bcmCosqControlAdtLowPriMonitorPool) {
                id.port = -1;
                id.cosq = -1;
            } else {
                id.port = port;
                id.cosq = cosq;
            }
        } else if (cosq_control_bindings[i].flags & MC) {
            printf("Cosq control type %d is applicable to MC\n", temp_type);
            id.port = port;
            id.cosq = cosq_mc;
        } else {
            printf("Cosq control type %d is not applicable to both UC&MC, skipped\n", temp_type);
            continue;
        }

        rv = bcm_cosq_control_dynamic_get(unit, type, &dynamic);
        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_control_dynamic_get FAILED for type %d. rv %d\n",
                    temp_type, rv);
            continue;
        } else if (dynamic == bcmCosqDynamicTypeFixed)
        {
            printf("ERR. Cosq control type %d is fixed\n", temp_type);
            continue;
        }

        control.type= temp_type;
        control.arg = arg;
        rv = bcm_cosq_control_extended_set(unit, &id, &control);

        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_control_set FAILED. rv %d\n", rv);
        }

        out_arg = -1;
        control.arg = -1;
        rv = bcm_cosq_control_extended_get(unit, &id, &control);
        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_control_get FAILED. rv %d\n", rv);
        }

        out_arg = control.arg;

        if (cosq_control_bindings[i].flags & UC) {
            if (arg != out_arg)
            {
                printf("=========================ERROR================================\n");
                printf("ERR. Set/get not match for UC type %d, value %d of port %d, cosq %d\n",
                    temp_type, arg, port, id.cosq);
                printf("Retrieved value %d \n", out_arg);
            }
            printf("************UC test finished for type: %d ************\n", temp_type);

        } else if (cosq_control_bindings[i].flags & MC) {
            if (arg != out_arg)
            {
                printf("=========================ERROR================================\n");
                printf("ERR. Set/get not match for MC type %d, value %d of port %d, cosq %d\n",
                    temp_type, arg, port, id.cosq);
                printf("Retrieved value %d \n", out_arg);
            }
            printf("************MC test finished for type: %d ************\n", temp_type);
        }

    }

    return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit)
{

/* Check already done in cosq_control_test and  cosq_control_extend_test*/
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  /*bshell(unit, "config show; a ; version");*/

  /* Do nothing */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* Carry set and get operation over different cosq control types, then compare the values. */
  printf("cosq_control_test started.\n");
  if (BCM_FAILURE((rv = cosq_control_test(unit)))) {
      printf("cosq_control_test() failed.\n");
      return -1;
  }
  printf("cosq_control_test end.\n");

  printf("cosq_control_extend_test started.\n");
  if (BCM_FAILURE((rv = cosq_control_extend_test(unit)))) {
      printf("cosq_control_extend_test() failed.\n");
      return -1;
  }
  printf("cosq_control_extend_test end.\n");

  /* Check already done in cosq_control_test and  cosq_control_extend_test*/
  verify(unit);

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}



