/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Wred
 *
 *  Usage    : BCM.0> cint wred.c
 *
 *  config   : wred_config.bcm
 *
 *  Log file : wred_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 * 	 bcm_cosq_discard_profile_create
 * 	 bcm_cosq_discard_profile_get
 * 	 bcm_cosq_discard_profile_update
 * 	 bcm_cosq_discard_profile_destroy
 * 	 bcm_cosq_gport_discard_set
 * 	 bcm_cosq_gport_discard_get
 *   bcm_cosq_discard_set
 *   bcm_cosq_discard_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate creating a WRED profile, attach it to a port, and configure
 *  other WRED related parameters.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Assign WRED profile and create discard profile
 *    2) Step2 - Configure WRED and check the configured paramters(Done in appl_wred_tests())
 *    ======================================================
 *      a) Configure WRED to a port/cos pair and check the configured paramters.
 *    3) Step3 - cleanup (Done in test_cleanup())
 *    ==========================================
 *      a) Clean the created discard profile.
 */

cint_reset();

/* GLOBAL VARIABLES */
int unit=0;
int glb_port = 0;
int glb_cos = 0;
int ct_wred_profile = 0;
int TH4_BYTES_PER_CELL=254;

struct my_wred_s
{
    char *test_name;
    bcm_cosq_gport_discard_t *in_discardParams;
    int port;
    int cos;
    bcm_cosq_gport_discard_t out_discardParams;
};

my_wred_s my_wred[10];

/* GLOBAL PROFILES */

int flagsAllTcp = ((BCM_COSQ_DISCARD_COLOR_ALL | BCM_COSQ_DISCARD_BYTES));
int flagsAllNonTcp = ((BCM_COSQ_DISCARD_COLOR_ALL)| BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_BYTES);
int flagsGreenRedTcp = ((BCM_COSQ_DISCARD_COLOR_GREEN|BCM_COSQ_DISCARD_COLOR_RED)| BCM_COSQ_DISCARD_TCP | BCM_COSQ_DISCARD_BYTES);
int flagsYellowRedNonTcp = ((BCM_COSQ_DISCARD_COLOR_YELLOW|BCM_COSQ_DISCARD_COLOR_RED)| BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_BYTES);

bcm_cosq_gport_discard_t *my_discardParams;

int port_list[2];

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

bcm_error_t port_get(int unit)
{
  int i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  return BCM_E_NONE;
}

print port_get(unit);
glb_port = port_list[0]; glb_cos = 0;
bcm_cosq_gport_discard_t discardParamsAllTcp = {flagsAllTcp, 0, 5*1518, 75, 0, 0, 1};
my_wred_s wredAllTcp = {"Color All + TCP", &discardParamsAllTcp, glb_port, glb_cos, {flagsAllTcp}};
ct_wred_profile++;

glb_port = port_list[1]; glb_cos = 0;
bcm_cosq_gport_discard_t discardParamsAllNonTcp = {flagsAllNonTcp, 0, 5*1000, 75, 0, 0, 1};
my_wred_s wredAllNonTcp = {"Color All + NONTCP", &discardParamsAllNonTcp, glb_port, glb_cos, {flagsAllNonTcp}};
ct_wred_profile++;

glb_port = port_list[0]; glb_cos = 3;
bcm_cosq_gport_discard_t discardParamsGreenRedTcp = {flagsGreenRedTcp, 0, 4*1518, 60, 0, 0, 1};
my_wred_s wredGreenRedTcp = {"Color Green_Red + TCP", &discardParamsGreenRedTcp, glb_port, glb_cos, {flagsGreenRedTcp}};
ct_wred_profile++;

glb_port = port_list[1]; glb_cos = 2;
bcm_cosq_gport_discard_t discardParamsYellowRedNonTcp = {flagsYellowRedNonTcp, 0, 4*1000, 60, 0, 0, 1};
my_wred_s wredYellowRedNonTcp = {"Color Yellow_Red + NONTCP", &discardParamsYellowRedNonTcp, glb_port, glb_cos, {flagsYellowRedNonTcp}};
ct_wred_profile++;


int wred_init()
{
    printf("... WRED INIT ...\n");
    my_wred[0] = wredAllTcp;
    my_wred[1] = wredAllNonTcp;
    my_wred[2] = wredGreenRedTcp;
    my_wred[3] = wredYellowRedNonTcp;

    return BCM_E_NONE;
}

/*
 *  Convert HW drop probability to percent value
 *  Same as TD2.
 */
int hw_drop_prob_to_percent[] = {
    0,     /* 0  */
    1,     /* 1  */
    2,     /* 2  */
    3,     /* 3  */
    4,     /* 4  */
    5,     /* 5  */
    6,     /* 6  */
    7,     /* 7  */
    8,     /* 8  */
    9,     /* 9  */
    10,    /* 10 */
    25,    /* 11 */
    50,    /* 12 */
    75,    /* 13 */
    100,   /* 14 */
    -1     /* 15 */
};

int percent_to_drop_prob(int percent)
{
   int i;

   for (i = 14; i > 0 ; i--) {
      if (percent >= hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}


int wred_hw_dump(int port, int cos)
{

    int hw_index = 0, hw_offset = 0;

    if (port == -1)
    {
        return -1;
    }
    if (cos == -1)
    {
        hw_offset = 10;
    }

    /*bshell(0, "d chg MMU_WRED_CONFIG");printf("\n");

    bshell(0, "d chg MMU_WRED_DROP_CURVE_PROFILE_0 0 5");printf("\n");
    bshell(0, "d chg MMU_WRED_DROP_CURVE_PROFILE_1 0 5");printf("\n");
    bshell(0, "d chg MMU_WRED_DROP_CURVE_PROFILE_2 0 5");printf("\n");
    bshell(0, "d chg MMU_WRED_DROP_CURVE_PROFILE_3 0 5");printf("\n");
    bshell(0, "d chg MMU_WRED_DROP_CURVE_PROFILE_4 0 5");printf("\n");
    bshell(0, "d chg MMU_WRED_DROP_CURVE_PROFILE_5 0 5");printf("\n");*/
    return 0;
}

int my_wred_test_check(bcm_cosq_gport_discard_t *in, bcm_cosq_gport_discard_t *out)
{
    char *err_msg = NULL;

    if (((in->max_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL) != (out->max_thresh/TH4_BYTES_PER_CELL))
    {
        printf("ERR. max_thresh(cell) in: %d, out: %d\n",
            ((in->max_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL),((out->max_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL));
        printf("ERR. max_thresh(byte) in: %d, out: %d\n",
            in->max_thresh,out->max_thresh);
        err_msg = "MAX THD mismatch";
    } else if ( ((in->min_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL) != (out->min_thresh/TH4_BYTES_PER_CELL))
    {
        err_msg = "MIN THD mismatch";
    } else if ((percent_to_drop_prob(in->drop_probability)) != (percent_to_drop_prob(out->drop_probability)))
    {
        err_msg = "Drop Rate mismatch";
    }
    printf("max_thresh(byte) in: %d, out: %d\n",
        in->max_thresh,out->max_thresh);

    if (err_msg) {
        printf("ERR. WRED_CHECK_FAIL. %s\n", err_msg);
        print in; print *in;
        print out; print *out;
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}

int test_num = 0;

int profile_id = -1;
int COSQ_WRED_CNG_ENTRY_PER_PROFILE=12;
bcm_cosq_discard_rule_t rule;
bcm_cosq_discard_rule_t rule_out[COSQ_WRED_CNG_ENTRY_PER_PROFILE];
int rule_count;
int rule_max = COSQ_WRED_CNG_ENTRY_PER_PROFILE;

int discard_profile_create(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_cosq_discard_profile_create(unit, &profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_discard_profile_create FAILED. rv %d\n", rv);
    }
    printf("discard_profile_create profile id %d created\n", profile_id);


    rv = bcm_cosq_discard_profile_get(unit, profile_id, rule_max, rule_out, &rule_count);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_esw_cosq_discard_profile_get FAILED. rv %d\n", rv);
    }
    printf("bcm_esw_cosq_discard_profile_get profile id %d, rule_count %d \n", profile_id, rule_count);

    print rule_out;

    rule.queue_min_congestion_state = BCM_COSQ_DISCARD_CONGESTION_LOW;
    rule.egress_pool_congestion_state= BCM_COSQ_DISCARD_CONGESTION_LOW;
    rule.action= BCM_COSQ_DISCARD_ACTION_FORCE_DROP;
    rv = bcm_cosq_discard_profile_update(unit, profile_id, &rule);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_discard_profile_create FAILED. rv %d\n", rv);
    }
    printf("discard_profile_create profile id %d updated\n", profile_id);


    rv = bcm_cosq_discard_profile_get(unit, profile_id, rule_max, rule_out, &rule_count);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_esw_cosq_discard_profile_get FAILED. rv %d\n", rv);
    }
    printf("bcm_esw_cosq_discard_profile_get profile id %d, rule_count %d \n", profile_id, rule_count);

    print rule_out;

    return rv;
}

int discard_profile_destroy(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_cosq_discard_profile_destroy(unit, profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_discard_profile_destroy FAILED. rv %d\n", rv);
    }
    printf("discard_profile_destroy profile id %d destroyed\n", profile_id);

    return rv;
}

int appl_wred_set(int unit, int port, int cos, my_wred_s *my_wred_l, int debug)
{
    int rv = BCM_E_NONE;
    bcm_cosq_gport_discard_t out_discardParams;

    printf("\nTest Case(#%d): %s\n\tPort %d, Cos %d\n", ++test_num, my_wred_l->test_name, port, cos);
    my_wred_l->in_discardParams->profile_id = profile_id;
    rv = bcm_cosq_gport_discard_set(unit, port, cos, my_wred_l->in_discardParams);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. gport_discard_set FAILED. rv %d, port %d, cos %d\n", rv, port, cos);
    }

    rv = bcm_cosq_gport_discard_get(unit, port, cos, &my_wred_l->out_discardParams);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. gport_discard_get FAILED. rv %d, port %d, cos %d\n", rv, port, cos);
    }

    if (debug) {
        print my_wred_l->out_discardParams;
    }
    rv = my_wred_test_check(my_wred_l->in_discardParams, my_wred_l->out_discardParams);

    if (rv != BCM_E_NONE)
    {
        printf("Test Case(%d): FAILED.(Reason: Set-Get Mismatch)\n", test_num);
    } else {
        printf("Test Case(%d): PASSED\n", test_num);
    }
    return rv;
}

int appl_wred_tests(int unit, int debug)
{
    int i = 0;
    int rv = BCM_E_NONE;

    rv = wred_init();

    for (i = 0; i < ct_wred_profile; i++)
    {
        rv = appl_wred_set(unit, my_wred[i].port, my_wred[i].cos, my_wred[i], debug);

        if (rv != BCM_E_NONE)
        {
            printf("ERR. WRED set FAILED for profile %d. rv %d\n", i, rv);
        }
        if (debug > 0)
        {
            print wred_hw_dump(my_wred[i].port, my_wred[i].cos);
        }
    }

    return rv;
}

int appl_wred_set_global(int unit)
{
    int rv = BCM_E_NONE;
    uint32 flags = 0, out_flags = 0;

    /*flags = (BCM_COSQ_DISCARD_COLOR_ALL);*/
    flags = (BCM_COSQ_DISCARD_TCP);
    printf("Input flag: 0x%x\n", flags);

    rv = bcm_cosq_discard_set(unit, flags);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. global_discard_set FAILED. rv %d\n", rv);
    }

    rv = bcm_cosq_discard_get(unit, &out_flags);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. global_discard_get FAILED. rv %d\n", rv);
    }

    printf("Returned flag: 0x%x\n", out_flags);

    return rv;
}

bcm_error_t test_setup(int unit)
{
    int rv = BCM_E_NONE;

    rv = wred_init();

    if (rv != BCM_E_NONE)
    {
        printf("ERR:WRED INIT FAILED(%d)\n", rv);
        return -1;
    }

    rv = discard_profile_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERR:discard_profile_create FAILED(%d)\n", rv);
        return -1;
    }

    return rv;
}

/* Function to verify the result */
void verify(int unit)
{

/* Check already done in appl_wred_tests */
}

bcm_error_t test_cleanup(int unit)
{
    int rv = BCM_E_NONE;

    rv = discard_profile_destroy(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERR:discard_profile_destroy FAILED(%d)\n", rv);
        return -1;
    }

    return rv;
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  /* Assign WRED profile and create discard profile */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* Configure WRED and check the configured paramters */
  if (BCM_FAILURE((rv = appl_wred_tests(unit, 0)))) {
      printf("appl_wred_tests() failed.\n");
      return -1;
  }
  /* Configure global WRED paramters */
  if (BCM_FAILURE((rv = appl_wred_set_global(unit)))) {
      printf("appl_wred_set_global() failed.\n");
      return -1;
  }

  /* NULL function here as check already done in appl_wred_tests */
  verify(unit);

  /* Clean the created discard profile */
  if (BCM_FAILURE((rv = test_cleanup(unit)))) {
     printf("test_cleanup() failed.\n");
     return -1;
  }

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


