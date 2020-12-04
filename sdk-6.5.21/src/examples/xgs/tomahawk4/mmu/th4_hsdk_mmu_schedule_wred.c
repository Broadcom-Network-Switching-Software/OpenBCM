/*
 * Feature  : WRED
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_schedule_wred.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_schedule_wred_log.txt
 *
 * Test Topology : None
 *
 * This script can verify below cosq APIs:
 * ========
 * 	bcm_cosq_discard_profile_create
 * 	bcm_cosq_discard_profile_get
 * 	bcm_cosq_discard_profile_update
 * 	bcm_cosq_discard_profile_destroy
 * 	bcm_cosq_gport_discard_set
 * 	bcm_cosq_gport_discard_get
 *      bcm_cosq_discard_set
 *      bcm_cosq_discard_get
 *
 * Summary:
 * ========
 * This cint example demonstrate creating a WRED profile, attach it to a port & cosq, and configure
 * other WRED related parameters.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *   1) Step1 - Test Setup (Done in test_setup())
 *   ============================================
 *     a) Set up 4 WRED test cases
 *     b) Assign WRED profile and create discard profile
 *
 *   2) Step2 - Configuration (Done in test_verify())
 *   ======================================================
 *     a) For the 4 test cases:
 *        Configure WRED to a port/cos pair and check the configured paramters.
 *     b) Test global WRED setting
 *
 *   3) Step3 - Verification (Done in test_verify())
 *   ======================================================
 *    Expected Result:
 *     a) For the 4 test cases:
 *        Configure WRED to a port/cos pair and check the configured paramters.
 *     b) Test global WRED setting and check the configured paramters.
 *
 *   4) Step4 - cleanup (Done in test_cleanup())
 *   ==========================================
 *     a) Delete the created discard profile.
 */

cint_reset();

int unit = 0;
int TH4_BYTES_PER_CELL=254;
int profile_id = -1;
int COSQ_WRED_CNG_ENTRY_PER_PROFILE=12;

typedef struct wred_test_t {
    char *test_name;
    bcm_cosq_gport_discard_t *in_discardParams;
    int port;
    int cos;
    bcm_cosq_gport_discard_t out_discardParams;
};

wred_test_t my_wred[4];
int wred_test_count = 0;
int port_list[2];

/*
 * This function is written so that hard coding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
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
      if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
          port_list[port] = i;
          port++;
      }
    }

    if ((port == 0) || (port != num_ports)) {
         printf("portNumbersGet() failed \n");
         return -1;
    }

    return BCM_E_NONE;
}

int flagsAllTcp          = (BCM_COSQ_DISCARD_COLOR_ALL | BCM_COSQ_DISCARD_BYTES);
int flagsAllNonTcp       = (BCM_COSQ_DISCARD_COLOR_ALL | BCM_COSQ_DISCARD_NONTCP |
                            BCM_COSQ_DISCARD_BYTES);
int flagsGreenRedTcp     = (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_COLOR_RED |
                            BCM_COSQ_DISCARD_TCP | BCM_COSQ_DISCARD_BYTES);
int flagsYellowRedNonTcp = (BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_COLOR_RED |
                            BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_BYTES);

bcm_cosq_gport_discard_t discardParamsAllTcp =
{
    flagsAllTcp,  /* flags */
    0,            /* min_thresh */
    5*1518,       /* max_thresh */
    75,           /* drop_probability */
    0,            /* gain */
    0,            /* ecn_thresh */
    1             /* refresh_time */
};

bcm_cosq_gport_discard_t discardParamsAllNonTcp =
{
    flagsAllNonTcp,  /* flags */
    0,            /* min_thresh */
    5*1000,       /* max_thresh */
    75,           /* drop_probability */
    0,            /* gain */
    0,            /* ecn_thresh */
    1             /* refresh_time */
};

bcm_cosq_gport_discard_t discardParamsGreenRedTcp =
{
    flagsGreenRedTcp,  /* flags */
    0,            /* min_thresh */
    4*1518,       /* max_thresh */
    60,           /* drop_probability */
    0,            /* gain */
    0,            /* ecn_thresh */
    1             /* refresh_time */
};

bcm_cosq_gport_discard_t discardParamsYellowRedNonTcp =
{
    flagsYellowRedNonTcp,  /* flags */
    0,            /* min_thresh */
    4*1000,       /* max_thresh */
    60,           /* drop_probability */
    0,            /* gain */
    0,            /* ecn_thresh */
    1             /* refresh_time */
};

bcm_error_t
wred_test_init()
{
    wred_test_t wredAllTcp =
    {
         "Color All + TCP",
         &discardParamsAllTcp,
         port_list[0],  /* port */
         0,             /* cos */
         {flagsAllTcp}
    };

    wred_test_t wredAllNonTcp =
    {
         "Color All + NONTCP",
         &discardParamsAllNonTcp,
         port_list[1],  /* port */
         0,             /* cos */
         {flagsAllNonTcp}
    };

    wred_test_t wredGreenRedTcp =
    {
         "Color Green_Red + TCP",
         &discardParamsGreenRedTcp,
         port_list[0],  /* port */
         3,             /* cos */
         {flagsGreenRedTcp}
    };

    wred_test_t wredYellowRedNonTcp =
    {
        "Color Yellow_Red + NONTCP",
         &discardParamsYellowRedNonTcp,
         port_list[1],  /* port */
         2,             /* cos */
         {flagsYellowRedNonTcp}
    };

    wred_test_count = 0;
    my_wred[wred_test_count++] = wredAllTcp;
    my_wred[wred_test_count++] = wredAllNonTcp;
    my_wred[wred_test_count++] = wredGreenRedTcp;
    my_wred[wred_test_count++] = wredYellowRedNonTcp;

    printf("\n... Set up the %d WRED test cases\n", wred_test_count);

    return BCM_E_NONE;
}

int
wred_hw_dump(int port, int cos)
{
    int hw_index = 0, hw_offset = 0;

    if (port == -1) {
        return -1;
    }
    if (cos == -1) {
        hw_offset = 10;
    }

    return BCM_E_NONE;
}

int
discard_profile_create(int unit)
{
    bcm_cosq_discard_rule_t rule;
    bcm_cosq_discard_rule_t rule_out[COSQ_WRED_CNG_ENTRY_PER_PROFILE];
    int rule_count;
    int rule_max = COSQ_WRED_CNG_ENTRY_PER_PROFILE;
    int rv = BCM_E_NONE;
    int i;

    /*
     * Create a discard rule profile
     */ 
    rv = bcm_cosq_discard_profile_create(unit, &profile_id);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_discard_profile_create FAILED. rv %d\n", rv);
    }
    printf("\n... discard_profile_create profile id %d created\n", profile_id);

    rv = bcm_cosq_discard_profile_get(unit, profile_id, rule_max, rule_out, &rule_count);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_esw_cosq_discard_profile_get FAILED. rv %d\n", rv);
    }
    printf("bcm_esw_cosq_discard_profile_get profile id %d, rule_count %d \n", profile_id, rule_count);
    for(i=0; i < rule_count; i++) {
        printf("rule_out[%d]: ", i);
        print rule_out[i];
    }

    /*
     * Update a discard rule profile
     */ 
    rule.queue_min_congestion_state = BCM_COSQ_DISCARD_CONGESTION_LOW;
    rule.egress_pool_congestion_state= BCM_COSQ_DISCARD_CONGESTION_LOW;
    rule.action= BCM_COSQ_DISCARD_ACTION_FORCE_DROP;
    rv = bcm_cosq_discard_profile_update(unit, profile_id, &rule);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_discard_profile_create FAILED. rv %d\n", rv);
    }
    printf("\n... discard_profile_create profile id %d updated\n", profile_id);

    rv = bcm_cosq_discard_profile_get(unit, profile_id, rule_max, rule_out, &rule_count);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_esw_cosq_discard_profile_get FAILED. rv %d\n", rv);
    }
    printf("bcm_esw_cosq_discard_profile_get profile id %d, rule_count %d \n", profile_id, rule_count);
    for(i=0; i < rule_count; i++) {
        printf("rule_out[%d]: ", i); 
        print rule_out[i];
    }

    return rv;
}

int
discard_profile_destroy(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_cosq_discard_profile_destroy(unit, profile_id);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_discard_profile_destroy FAILED. rv %d\n", rv);
    }
    printf("discard_profile_destroy profile id %d destroyed\n\n", profile_id);

    return rv;
}

/*
 *  Convert HW drop probability to percent value
 */
int
percent_to_drop_prob(int percent)
{
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
    int i;

    for (i = 14; i > 0 ; i--) {
        if (percent >= hw_drop_prob_to_percent[i]) {
            break;
        }
    }
    return i;
}

int
wred_configure_check(bcm_cosq_gport_discard_t *in, bcm_cosq_gport_discard_t *out)
{
    char *err_msg = NULL;

    if (((in->max_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL) !=
         (out->max_thresh/TH4_BYTES_PER_CELL)) {
        printf("ERR. max_thresh(cell) in: %d, out: %d\n",
            ((in->max_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL),
            ((out->max_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL));
        printf("ERR. max_thresh(byte) in: %d, out: %d\n", in->max_thresh, out->max_thresh);
        err_msg = "MAX THD mismatch";
    } else if ( ((in->min_thresh + TH4_BYTES_PER_CELL - 1)/TH4_BYTES_PER_CELL) !=
                 (out->min_thresh/TH4_BYTES_PER_CELL)) {
        err_msg = "MIN THD mismatch";
    } else if ((percent_to_drop_prob(in->drop_probability)) !=
               (percent_to_drop_prob(out->drop_probability))) {
        err_msg = "Drop Rate mismatch";
    }
    printf("min_thresh(byte) in: %d, out: %d\n", in->min_thresh,out->min_thresh);
    printf("max_thresh(byte) in: %d, out: %d\n", in->max_thresh,out->max_thresh);
    printf("drop_prob in: %d, out: %d\n", in->drop_probability,out->drop_probability);
    printf("discard_profile in: %d, out: %d\n", in->profile_id,out->profile_id);

    if (err_msg) {
        printf("ERR. WRED_CHECK_FAIL. %s\n", err_msg);
        print in; print *in;
        print out; print *out;
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

int
wred_configure (int unit, wred_test_t *wred_tl, int debug)
{
    int rv = BCM_E_NONE;
    bcm_cosq_gport_discard_t out_discardParams;
    int port, cos; 

    port = wred_tl->port;
    cos = wred_tl->cos;

    printf("%s\n\tPort %d, Cos %d\n", wred_tl->test_name, port, cos);
    wred_tl->in_discardParams->profile_id = profile_id;
    rv = bcm_cosq_gport_discard_set(unit, port, cos, wred_tl->in_discardParams);
    if (rv != BCM_E_NONE) {
        printf("ERR. gport_discard_set FAILED. rv %d, port %d, cos %d\n", rv, port, cos);
    }

    rv = bcm_cosq_gport_discard_get(unit, port, cos, &wred_tl->out_discardParams);
    if (rv != BCM_E_NONE) {
        printf("ERR. gport_discard_get FAILED. rv %d, port %d, cos %d\n", rv, port, cos);
    }

    if (debug) {
        print wred_tl->out_discardParams;
    }

    rv = wred_configure_check(wred_tl->in_discardParams, wred_tl->out_discardParams);
    if (rv != BCM_E_NONE) {
        printf("Test FAILED (Reason: Set-Get Mismatch)\n");
    } else {
        printf("Test PASSED\n");
    }

    return rv;
}

int
test_wred_configuration(int unit, int debug)
{
    int i = 0;
    int rv = BCM_E_NONE;

    for (i = 0; i < wred_test_count; i++) {
        printf("\nTest Case(#%d): ", i+1);
        rv = wred_configure (unit, &my_wred[i], debug);
        if (rv != BCM_E_NONE) {
            printf("ERR. WRED set FAILED for profile %d. rv %d\n", i, rv);
        }

        if (debug > 0) {
            print wred_hw_dump(my_wred[i].port, my_wred[i].cos);
        }
    }

    return rv;
}

int
test_wred_set_global(int unit)
{
    int rv = BCM_E_NONE;
    uint32 flags = 0, out_flags = 0;

    printf("\n... test_wred_set_global\n");

    flags = BCM_COSQ_DISCARD_ENABLE;
    printf("Input flag: 0x%x\n", flags);

    rv = bcm_cosq_discard_set(unit, flags);
    if (rv != BCM_E_NONE) {
        printf("ERR. global_discard_set FAILED. rv %d\n", rv);
    }

    rv = bcm_cosq_discard_get(unit, &out_flags);
    if (rv != BCM_E_NONE) {
        printf("ERR. global_discard_get FAILED. rv %d\n", rv);
    }
    printf("Returned flag: 0x%x\n", out_flags);

    return rv;
}

bcm_error_t test_setup(int unit)
{
    int rv = BCM_E_NONE;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    rv = wred_test_init();
    if (rv != BCM_E_NONE) {
        printf("ERR:WRED INIT FAILED(%d)\n", rv);
        return -1;
    }
    printf("wred_test_count=%d\n", wred_test_count);

    rv = discard_profile_create(unit);
    if (rv != BCM_E_NONE) {
        printf("ERR:discard_profile_create FAILED(%d)\n", rv);
        return -1;
    }

    return rv;
}

/* Function to verify the result */
void test_verify(int unit)
{
    int rv = BCM_E_NONE;

  /* Configure WRED and check the configured paramters */
  if (BCM_FAILURE((rv = test_wred_configuration(unit, 0)))) {
      printf("test_wred_configuration() failed.\n");
      return -1;
  }
  /* Configure global WRED paramters */
  if (BCM_FAILURE((rv = test_wred_set_global(unit)))) {
      printf("test_wred_set_global() failed.\n");
      return -1;
  }

    return BCM_E_NONE;
}

bcm_error_t test_cleanup(int unit)
{
    int rv = BCM_E_NONE;

    rv = discard_profile_destroy(unit);
    if (rv != BCM_E_NONE) {
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

  if (BCM_FAILURE((rv = test_verify(unit)))) {
      printf("test_verify() failed.\n");
      return -1;
  }

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

