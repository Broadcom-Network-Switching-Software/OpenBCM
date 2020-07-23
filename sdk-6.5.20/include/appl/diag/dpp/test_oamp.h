/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

/* Defines */
#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#define TEST_BFD_VLAN                       10          /* VID = 10 */
#define TEST_BFD_DETEC_MULT_3               3           /* Detect Multiplier = 3 */
#define TEST_BFD_DETECTION_TIME_10MS        100000      /* Detection time = 10ms */
#define TEST_BFD_REMOTE_IP                  0x09010164  /* Destination IP*/
#define TEST_BFD_MAX_NUM_PERIODS            7           /* Maximum number of allowed periods*/
#define TEST_BFD_NUM_DEF_PERIODS            6           /* Number of predifined values for EP periods*/
#define TEST_BFD_MAX_NUM_INTERVALS          8           /* Maximum number of values for Required Min RX and Desire Min TX Interval */
#define TEST_BFD_NUM_IP                     16          /* Maximum number of IPs */

#define NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE SOC_DPP_DEFS_GET(unit,counters_per_counter_processor) * 2
#define OAMP_TEST_DEF_PORT_1                13          /* Default value for port_1 */
#define OAMP_TEST_DEF_PORT_2                14          /* Default value for port_2*/
#define OAMP_TEST_DEF_SHORT_MAID            0           /* Default value for maid*/
#define OAMP_TEST_DEF_NUM_EP_PAIRS          1024        /* Default value for number pairs of endpoint*/
#define OAMP_TEST_DEF_SEED                  0           /* Default value for seed number */
#define OAMP_TEST_MAX_NUM_DEVICES           2           
#define OAMP_TEST_DEF_NUM_STATES            0           /* Default value for number of states to be changed */
#define OAMP_TEST_DEF_NUM_UPDATE            0           /* Default value for number update of period */
#define OAMP_TEST_DEF_NORMAL                0           /* Default value for BFD time-test flag */
#define OAMP_TEST_TIME                      1
#define TEST_OAM_NUM_LIFS                   1024

typedef struct oamp_test_init_param_s {
    char *option;                                       /* Test type (BFD/OAM) */
    int port1;                                          /* port on which endpoints will be defined*/
    int port2;                                          /* port on which endpoints will be defined*/
    int numEndpoints;                                   /* Number of endpoints to be created*/
    int seed;                                           /* Seed of the test*/
    int numStates;                                      /* The number of times that the state will be change */
    int numUpdate;                                      /* The number of random MEPS to be updated */
    int testSLM;
    int EndpointCreationTimeTest;                       /* BFD time test flag*/
    int timeoutEventsTest;                              /* BFD time out events test*/
    int is_Maid48;                                         /* Create MEPs with MAID48 */
} oamp_test_init_param_t;

typedef struct _bcm_dpp_oam_bfd_sw_db_lock_s {
    sal_mutex_t lock;
    int is_init;
} test_bfd_lock_t;

typedef struct endpoint_create_info_s {
    int id;
    int vlan;
    bcm_gport_t vlan_port_id;
    int name;
    int level;
    int port;
    int counter_base_id;
    unsigned int is_rdi;
	int mac;
} endpoint_create_info_t;

#define TEST_BFD_INFO_LOCK \
    if (sal_mutex_take(test_bfd_info_lock.lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    }

#define TEST_BFD_INFO_UNLOCK  \
   if (sal_mutex_give(test_bfd_info_lock.lock)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    }
extern int oamp_usage_parse(int unit, args_t *a, oamp_test_init_param_t *init_param);

extern int test_run_oam(int unit, int port_1, int port_2, int numEndpoints, int seed, int numUpdate, int testSLM, int is_maid48);
extern int test_oam_create_vlan_gport(int unit, int port, int vlan, int * vlan_port_id);
extern int test_oam_create_endpoint_with_rmep(int unit, endpoint_create_info_t *ep1, endpoint_create_info_t *ep2, int mep_index, int group, int set_slm);
extern int test_oam_updateLMEP(int unit,int index,int *event_index);
extern int test_oam_updateRMEP(int unit,int index);
extern int test_oam_init_and_surroundings(int unit, int * group_id1);
extern int test_oam_init_and_surroundings_48maid(int unit, int is_maid48, int * group_id1, int * group_id2);
extern int test_oam_register_events(int unit);

extern int test_run_bfd(int unit, int port_1, int port_2, int numEndpoints, int seed, int numStates, int BFDTimeTest, int timeoutEventsTest);
extern int test_bfd_Multithreding(int unit, int endpoint_id);
extern int test_bfd_update_state(int unit, int endpoint_id);
extern int test_bfd_createEndpoint(int unit,int local_port, int encap_id, int src_ip,int remote_ip, int local_discr, int remote_discr);
extern int test_bfd_updateEndpoint(int unit, int endpoint_1_id, int endpoint_2_id );
extern int test_bfd_preDefinedUpdate(int unit, int endpoint_1_id, int endpoint_2_id);
extern int test_bfd_changeState(int unit, int endpoint_id);
extern int test_bfd_add_host(int unit, int addr, int vrf, int intf);
extern int test_bfd_register_events(int unit);
extern int test_bfd_init_ports(int unit,int local_port, int remote_port, int vlan) ;
extern int test_mpls_init_ports(int unit,int local_port, int remote_port, int vlan, int vlan2) ;
extern void test_bfd_set_global_values(void);
