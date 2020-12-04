/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_PURE_DEFI_FAP21V_TGS_INCLUDED__
/* { */
#define __UI_PURE_DEFI_FAP21V_TGS_INCLUDED__

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

#define PARAM_FAP21V_TGS_APP_ID \
                       (PARAM_FAP21V_TGS_START_RANGE_ID + 1)
#define PARAM_FAP21V_TGS_APP_RUN_ID \
                       (PARAM_FAP21V_TGS_START_RANGE_ID + 2)
#define PARAM_FAP21V_TGS_APP_RUN_RESET_ID \
                       (PARAM_FAP21V_TGS_START_RANGE_ID + 3)
#define PARAM_FAP21V_TGS_APP_RUN_SILENT_ID \
                       (PARAM_FAP21V_TGS_START_RANGE_ID + 4)
/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_PURE_DEFI_FAP21V_TGS_INCLUDED__*/
#endif
