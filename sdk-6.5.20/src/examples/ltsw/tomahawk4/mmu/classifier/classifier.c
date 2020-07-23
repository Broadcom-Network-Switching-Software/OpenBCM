/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : classifier
 *
 *  Usage    : BCM.0> cint classifier.c
 *
 *  config   : classifier_config.bcm
 *
 *  Log file : classifier_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 *    bcm_cosq_classifier_t_init
 *    bcm_cosq_classifier_create
 *    bcm_cosq_classifier_destroy
 *    bcm_cosq_classifier_get
 *    bcm_cosq_classifier_mapping_clear
 *    bcm_cosq_classifier_mapping_multi_get
 *    bcm_cosq_classifier_mapping_multi_set
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate creating a classifier profile, modifying the mapping of the created
 *  profile, clearing the mapping and destroying the profile.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Do nothing.
 *    2) Step2 - Creating a classifier profile, modifying the mapping of the created profile,
 *        clearing the mapping and destroying the profile (Done in classifier_create() and
 *        classifier_mapping_test()).
 *    ======================================================
 *    3) Step3 - verify (Done in verify())
 *    ==========================================
 *      a) Do nothing.
 */

cint_reset();

/* GLOBAL VARIABLES */
int unit = 0;
bcm_cosq_classifier_t classifier, out_classifier;
int classifier_id = -1;

int TC_NUM = 16;
int array_count = TC_NUM*3;
bcm_cos_t priority_array[TC_NUM*3] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
bcm_cos_t cosq_array[TC_NUM*3] = {7,6,5,4,3,2,1,0,4,3,2,1,1,2,3,4,
                                  0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
                                  0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3};

/* These two variables are acutually not used by APIs, dumy parameters */
bcm_gport_t port = 0;
bcm_gport_t queue_group = 0;

bcm_cos_t out_priority_array[TC_NUM*3] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                          0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                          0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
bcm_cos_t out_cosq_array[TC_NUM*3] = {{0}};

int classifier_create(int unit)
{
    int rv = BCM_E_NONE;

    bcm_cosq_classifier_t_init(&classifier);

    classifier.flags = (BCM_COSQ_CLASSIFIER_FIELD);
    printf("Input flag: 0x%x\n", classifier.flags);

    rv = bcm_cosq_classifier_create(unit, &classifier, &classifier_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_classifier_create FAILED. rv %d\n", rv);
    }

    printf("Created classifier ID: 0x%x\n", classifier_id);

    bcm_cosq_classifier_t_init(&out_classifier);

    rv = bcm_cosq_classifier_get(unit, classifier_id, &out_classifier);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_classifier_get FAILED. rv %d\n", rv);
    }

    if (out_classifier.flags != classifier.flags)
    {
        printf("ERR. Retrieved classifier is not same as created one\n");
        return -1;
    }

    return rv;
}

int classifier_mapping_test(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_cosq_classifier_mapping_multi_set(unit, port, classifier_id, queue_group,
                                    array_count, &priority_array, &cosq_array);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_classifier_mapping_multi_set FAILED. rv %d\n", rv);
        return rv;
    }
/*
    printf("Printing before bcm_cosq_classifier_mapping_multi_get\n");
    print out_priority_array;
    print out_cosq_array;
*/
    rv = bcm_cosq_classifier_mapping_multi_get(unit, port, classifier_id, &queue_group,
                                    TC_NUM*3, &out_priority_array, &out_cosq_array,
                                    &array_count);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_classifier_mapping_multi_get FAILED. rv %d\n", rv);
        return rv;
    }
    printf("Printing after bcm_cosq_classifier_mapping_multi_get\n");

    print out_priority_array;
    print out_cosq_array;

    rv = bcm_cosq_classifier_mapping_clear(unit, port, classifier_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_classifier_mapping_clear FAILED. rv %d\n", rv);
        return rv;
    }

    rv = bcm_cosq_classifier_destroy(unit, classifier_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_classifier_destroy FAILED. rv %d\n", rv);
        return rv;
    }

    rv = bcm_cosq_classifier_mapping_multi_get(unit, port, classifier_id, &queue_group,
                                    TC_NUM*3, &out_priority_array, &out_cosq_array,
                                    &array_count);
    if (rv != BCM_E_NOT_FOUND)
    {
        printf("ERR. bcm_cosq_classifier_mapping_multi_get FAILED. rv %d\n", rv);
        return rv;
    }
    printf("Classifier mapping was not found after mapping_clear as expected\n");

    rv = BCM_E_NONE;

    return rv;
}


bcm_error_t test_setup(int unit)
{
    int rv = BCM_E_NONE;

    return rv;
}

/* Function to verify the result */
void verify(int unit)
{

/* Check already done in appl_classifier_tests */
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  /* Do nothing */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* Creating a classifier profile */
  if (BCM_FAILURE((rv = classifier_create(unit)))) {
      printf("classifier_create() failed.\n");
      return -1;
  }

  /* Modifying the mapping of the created profile,
        clearing the mapping and destroying the profile */
  if (BCM_FAILURE((rv = classifier_mapping_test(unit)))) {
      printf("classifier_mapping_test() failed.\n");
      return -1;
  }

  /* NULL function here as check already done in appl_classifier_tests */
  verify(unit);

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


