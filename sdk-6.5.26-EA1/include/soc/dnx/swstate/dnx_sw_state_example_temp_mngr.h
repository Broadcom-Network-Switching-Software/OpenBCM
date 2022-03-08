
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef INCLUDE_CTEST

#define TEMPLATE_TEST_BAD_VALUES                        "template_test_bad_values"
#define TEMPLATE_TEST_WITH_DEFAULT                      "template_test_with_default"
#define TEMPLATE_TEST_WITHOUT_DEFAULT                   "template_test_without_default"
#define TEMPLATE_TEST_PER_CORE                          "template_test_per_core"
#define TEMPLATE_TEST_WITH_SAME_DATA                    "template_test_with_same_data_profiles"
#define TEMPLATE_TEST_WITH_DEFAULT_CHECK_VALUES         "template_with_default_check_values"
#define TEMPLATE_TEST_WITHOUT_DEFAULT_CHECK_VALUES      "template_test_without_default_check_values"
#define TEMPLATE_TEST_PER_CORE_CHECK_VALUES             "template_test_per_core_check_values"

#define FLAGS_NONE 0
#define NOF_PROFILES 16
#define MAX_REFERENCES 32
#define FIRST_PROFILE 0
#define DEFAULT_PROFILE 0
#define DEFAULT_PROFILE_DATA_8 0xFF
#define DEFAULT_PROFILE_DATA_16 0x1122
#define DEFAULT_PROFILE_DATA_32 0x00112233
#define DEFAULT_PROFILE_DATA_INT 0xFFFF
#define MIN_NOF_CORES 2
#define FIRST_CORE_ID  0
#define SECOND_CORE_ID 1
#define NEGATIVE_PROFILE -1
#define NEGATIVE_NOF_PROFILES -1
#define NEGATIVE_MAX_REFERENCES -1
#define NEGATIVE_DEFAULT_PROFILE -1
#define NEGATIVE_DATA_SIZE -1

int dnx_sw_state_example_temp_mngr_init(
    int unit);

#endif
