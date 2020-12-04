/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_PURE_DEFI_FMF_INCLUDED
/* { */
#define UI_PURE_DEFI_FMF_INCLUDED
/*
 * Note:
 * the following definitions must range between PARAM_FMF_START_RANGE_ID
 * and PARAM_FMF_END_RANGE_ID.
 * See ui_pure_defi.h
 */
#define PARAM_FMF_REGISTER_DRIVER_ID                  1600
#define PARAM_FMF_UNREGISTER_DRIVER_ID                1601
#define PARAM_FMF_DEBUG_ID                            1602
#define PARAM_FMF_ADD_DEVICE_ID                       1603
#define PARAM_FMF_UNIT_IP_ADDRESS_ID                  1604
#define PARAM_FMF_DEVICE_CHIP_ID_ID                   1605
#define PARAM_FMF_DEVICE_TYPE_ID                      1606
#define PARAM_FMF_DEVICE_SUB_TYPE_ID                  1607
#define PARAM_FMF_SHOW_ID                             1608
#define PARAM_FMF_SHOW_ALL_ID                         1609
#define PARAM_FMF_SHOW_UNIT_ID                        1610
#define PARAM_FMF_FORMAT_ID                           1611
#define PARAM_FMF_ACTIVATE_DEVICE_ID                  1612
#define PARAM_FMF_DEVICE_ID_ID                        1613
#define PARAM_FMF_DEBUG_FLAGS_ID                      1614
#define PARAM_FMF_INIT_BASE_PARAMS_ID                 1615
#define PARAM_FMF_ONE_SPECIFIC_DEVICE_ID              1616
#define PARAM_FMF_ALL_SPECIFIED_DEVICES_ID            1617
#define PARAM_FMF_CONNECT_PP_TO_PP_ID                 1618
#define PARAM_FMF_FROM_ONE_PP_ID                      1619
#define PARAM_FMF_PP_ID_ID                            1620
#define PARAM_FMF_FROM_ALL_PPS_ID                     1621
#define PARAM_FMF_TO_ONE_PP_ID                        1622
#define PARAM_FMF_TO_ALL_PPS_ID                       1623
#define PARAM_FMF_DST_PP_ID_ID                        1624
#define PARAM_FMF_USE_FLOW_ID                         1625
/*
 * Definitions related to 'format' parameter (within 'fmf')
 * {
 */
#define FORMAT_SHORT_EQUIVALENT 1
#define FORMAT_LONG_EQUIVALENT  2
/*
 * }
 */
/*
 * Definitions related to 'debug_flags' parameter (within 'fmf')
 * {
 */
#define FMF_DEBUG_ON_EQUIVALENT         1
#define FMF_DEBUG_OFF_EQUIVALENT        2
#define DCL_DEBUG_ON_EQUIVALENT         3
#define DCL_DEBUG_OFF_EQUIVALENT        4
#define FMC_N_DEBUG_ON_EQUIVALENT       5
#define FMC_N_DEBUG_OFF_EQUIVALENT      6
#define SHOW_ALL_DEBUG_FLAGS_EQUIVALENT 7
/*
 * }
 */
/*
 * Definitions related to 'device_type' parameter (within 'fmf')
 * {
 */
#define ANY_DEVICE_TYPE_EQUIVALENT   0
#define FE2_DEVICE_TYPE_EQUIVALENT   1
#define FE13_DEVICE_TYPE_EQUIVALENT  2
#define FAP_DEVICE_TYPE_EQUIVALENT   3
/*
 * }
 */
/*
 * Definitions related to 'device_subtype' parameter (within 'fmf')
 * {
 */
#define ANY_DEVICE_SUBTYPE_EQUIVALENT    0
#define FE200_DEVICE_SUBTYPE_EQUIVALENT  1
#define FAP10M_DEVICE_SUBTYPE_EQUIVALENT 2
#define FE200_DEVICE_SUBTYPE_STRING      "fe200_subtype"
#define FAP10M_DEVICE_SUBTYPE_STRING     "fap10m_subtype"
/*
 * }
 */
/* } */
#endif
