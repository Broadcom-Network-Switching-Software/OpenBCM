/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_PURE_DEFI_FAP21V_APP_INCLUDED__
/* { */
#define __UI_PURE_DEFI_FAP21V_APP_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
     #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
     #error  "Add your system support for packed attribute."
#endif

/*
 * Note:
 * the following definitions must range between PARAM_FAP21V_API_START_RANGE_ID
 * and PARAM_FAP21V_API_END_RANGE_ID.
 * See ui_pure_defi.h
 */

#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_PORT_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 1)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_NOMINAL_RATE_MBPS_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 2)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_SCHEME_ID_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 3)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MINIMUM_RATE_BY_PERCENTAGE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 4)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MAXIMUM_RATE_BY_PERCENTAGE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 5)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 7)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_WEIGHT_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 8)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_SWEEP21V_SCH_HR_CLASS_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 9)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MAX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 10)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MIN_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 11)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 12)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 13)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 14)
#define PARAM_FAP21V_APP_SSR_SAVE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 15)
#define PARAM_FAP21V_APP_SSR_RESTORE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 16)
#define PARAM_FAP21V_APP_SSR_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 17)
#define PARAM_FAP21V_APP_FE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 18)
#define PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 19)
#define PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_FE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 20)
#define PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 21)
#define PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_FE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 22)

#define PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 23)
#define PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_FAP_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 24)
#define PARAM_FAP21V_APP_SERDES_POWER_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 26)
#define PARAM_FAP21V_APP_SERDES_POWER_DOWN_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 27)
#define PARAM_FAP21V_APP_SERDES_POWER_DOWN_FE_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 28)
#define PARAM_FAP21V_APP_SERDES_POWER_DOWN_SERDES_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 29)
#define PARAM_FAP21V_APP_SERDES_POWER_UP_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 30)
#define PARAM_FAP21V_APP_SERDES_POWER_UP_FE_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 31)
#define PARAM_FAP21V_APP_SERDES_POWER_UP_SERDES_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 32)
#define PARAM_FAP21V_APP_LINKS_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 33)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_FAP_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 34)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_FAP_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 35)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 36)
/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_PURE_DEFI_FAP21V_APP_INCLUDED__*/
#endif
