/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
*/
#ifndef UI_PURE_DEFI_DMEO_INCLUDED
/* { */
#define UI_PURE_DEFI_DMEO_INCLUDED

/*
 * Note:
 * the following definitions must range between PARAM_DEMO_START_RANGE_ID (1800)
 * and PARAM_DEMO_END_RANGE_ID (2099).
 * See ui_pure_defi.h
 */
#define PARAM_NEGEV_DEMO_SET_QOS_PARAMS_ID            1800
#define PARAM_NEGEV_DEMO_GET_QOS_PARAMS_ID            1801
#define PARAM_NEGEV_DEMO_POWER_UP_SERDES_ID           1802
#define PARAM_NEGEV_DEMO_POWER_DOWN_SERDES_ID         1803
#define PARAM_NEGEV_DEMO_GET_CONNECTIVITY_MAP_ID      1804
#define PARAM_NEGEV_DEMO_GRACEFUL_SHUTDOWN_FE_ID      1805
#define PARAM_NEGEV_DEMO_PRBS_TEST_ID                 1806
#define PARAM_NEGEV_DEMO_PRBS_TEST_ALL_FAPS_ID        1807
#define PARAM_NEGEV_DEMO_PRBS_TEST_SPECIFIC_FAPS_ID   1808
#define PARAM_NEGEV_DEMO_PRBS_TEST_FAP_LIST_ID        1809
#define PARAM_NEGEV_DEMO_PRBS_TEST_TIME_PERIOD_ID     1810
#define PARAM_NEGEV_DEMO_PRBS_TEST_FORCE_ALL_LINKS_ID 1811
#define PARAM_NEGEV_DEMO_SOURCE_FAP_DEVICE_ID         1820
#define PARAM_NEGEV_DEMO_GRACEFUL_RESTORE_FE_ID      1821

#define NEGEV_DEMO_QOS_USE_SRC_PORT 0
#if NEGEV_DEMO_QOS_USE_SRC_PORT
  #define PARAM_NEGEV_DEMO_SOURCE_PORT_ID             1822
#endif

#define PARAM_NEGEV_DEMO_DESTINATION_FAP_DEVICE_ID    1822
#define PARAM_NEGEV_DEMO_DESTINATION_PORT_ID          1823
#define PARAM_NEGEV_DEMO_DESTINATION_CLASS_ID         1824
#define PARAM_NEGEV_DEMO_SUB_FLOW_1_VALUE_ID          1826
#define PARAM_NEGEV_DEMO_SUB_FLOW_1_TYPE_ID           1827
#define PARAM_NEGEV_DEMO_SUB_FLOW_0_VALUE_ID          1828
#define PARAM_NEGEV_DEMO_SUB_FLOW_0_TYPE_ID           1829
#define PARAM_NEGEV_DEMO_SERDES_ID                    1830
#define PARAM_NEGEV_DEMO_FE_DEVICE_ID                 1840
#define PARAM_NEGEV_DEMO_FAP_DEVICE_ID                1850

#define PARAM_NEGEV_DEMO_SCHEDULING_SCHEME_SET_ID     1860
#define PARAM_NEGEV_DEMO_SCHEDULING_SCHEME_GET_ID     1861
#define PARAM_NEGEV_DEMO_SHOW_FLOWS_ID                1864
#define PARAM_NEGEV_DEMO_SHOW_FLOWS_FIRST_FLOW_ID     1865
#define PARAM_NEGEV_DEMO_SHOW_FLOWS_LAST_FLOW_ID      1866
#define PARAM_NEGEV_DEMO_ENHANCED_ALLOCATORS_ID       1870
#define PARAM_NEGEV_DEMO_PRIOR_2_RATE_ID              1871
#define PARAM_NEGEV_DEMO_PRIOR_3_RATE_ID              1872
#define PARAM_NEGEV_DEMO_PRIOR_4_RATE_ID              1873

#define PARAM_DEMO_MAC_ADDRESSES_GROUP_ID             1880
#define PARAM_DEMO_MAC_ADDRESSES_HELP_ID              1881
#define PARAM_DEMO_MACS_ADD_UNTAGGED_ANY_TO_ANY       1882
#define PARAM_DEMO_MACS_REMOVE_UNTAGGED_ANY_TO_ANY    1883


/* } */
#endif /*UI_PURE_DEFI_DMEO_INCLUDED*/
