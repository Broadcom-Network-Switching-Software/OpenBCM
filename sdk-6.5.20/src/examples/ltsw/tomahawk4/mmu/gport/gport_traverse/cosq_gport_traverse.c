/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : cosq gport
 *
 *  Usage    : BCM.0> cint cosq_gport_traverse.c
 *
 *  config   : cosq_gport_traverse_config.bcm
 *
 *  Log file : cosq_gport_traverse_log.txt
 *
 *  Test Topology : None
 *
 *  API tested:
 *      bcm_cosq_gport_traverse
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates the retrieval of all gports of the system for both front panel
 *  and CPU ports.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      Do nothing in setup step.
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Traverse the all the ports and print them
 *      b) Expected Result:
 *         ================
 *         Printing out all gports of the system for both active front panel and CPU ports.
 */


cint_reset();

int unit=0;
int rv;
int CPU_PORT = 0;
int MAX_PORTS = 272;
int MAX_QUEUES = 12;
int MAX_CPU_QUEUES = 48;
int MAX_SCH_NODES = 12;

int prev_port = -1;
int free_ucast_index = 0;
int free_mcast_index = 0;
int free_sched_index = 0;

bcm_gport_t ucast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t mcast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t sched_gport[MAX_PORTS][MAX_SCH_NODES];
bcm_gport_t cpu_mcast_q_gport[MAX_CPU_QUEUES];
bcm_gport_t cpu_sched_gport[MAX_SCH_NODES];

bcm_error_t test_setup(int unit)
{
    return BCM_E_NONE;
}

static bcm_error_t cosq_gport_traverse_callback(int unit, bcm_gport_t port, int numq, uint32 flags,
                                 bcm_gport_t gport, void *user_data) {
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

        printf("cosq_gport_traverse_callback ucast queue: local_port=%d,mod_port=0x%x, numq=%d, gport=0x%x\n",
        local_port, port, numq, gport);
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

        printf("cosq_gport_traverse_callback mcast queue: local_port=%d,mod_port=0x%x, numq=%d, gport=0x%x\n",
        local_port, port, numq, gport);
        is_cpu_port = ((CPU_PORT == local_port) ? 1 : 0);
        if (is_cpu_port) {
            cpu_mcast_q_gport[free_mcast_index] = gport;
        } else {
            mcast_q_gport[local_port][free_mcast_index] = gport;
        }
        free_mcast_index++;
        break;

    case BCM_COSQ_GPORT_SCHEDULER:
        printf("cosq_gport_traverse_callback scheduler: local_port=%d,mod_port=0x%x, numq=%d, gport=0x%x\n",
        prev_port, port, numq, gport);
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

static bcm_error_t cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb, void *user_data) {
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

/* Function to verify the result */
void verify(int unit)
{
    /* Traverse the all the ports and print them */
    if (BCM_FAILURE((rv = cosq_gport_traverse(0, cosq_gport_traverse_callback, NULL)))) {
        printf("cosq_gport_traverse() failed.\n");
        return -1;
    }

    printf("Kindly refer to above gport printing!\n");
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");

    /* No initial setup needed */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    /* No configuration needed for this case */

    /* Verify the result */
    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}

