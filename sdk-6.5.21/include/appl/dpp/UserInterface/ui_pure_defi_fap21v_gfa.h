/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_PURE_DEFI_FAP21V_GFA_INCLUDED__
/* { */
#define __UI_PURE_DEFI_FAP21V_GFA_INCLUDED__

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

#define PARAM_FAP21V_GFA_GENERAL_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 1)
#define PARAM_FAP21V_GFA_MB_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 2)
#define PARAM_FAP21V_GFA_SCREENING_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 3)
#define PARAM_FAP21V_GFA_STATUS_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 4)
#define PARAM_FAP21V_GFA_MB_FLASH_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 8)
#define PARAM_FAP21V_GFA_MB_FPGA_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 9)
#define PARAM_FAP21V_GFA_SCREENING_INTERFACES_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 10)
#define PARAM_FAP21V_GFA_SCREENING_FTG_TEST_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 11)
#define PARAM_FAP21V_GFA_SCREENING_PRINT_MODE_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 12)
#define PARAM_FAP21V_GFA_MB_FGPA_DOWNLOAD_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 12)
#define PARAM_FAP21V_GFA_MB_FLASH_DOWNLOAD_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 13)
#define PARAM_FAP21V_GFA_MB_FLASH_ERASE_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 14)
#define PARAM_FAP21V_GFA_MB_FLASH_STATUS_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 15)
#define PARAM_FAP21V_GFA_MB_FLASH_BURN_FPGA_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 16)
#define PARAM_FAP21V_GFA_GENERAL_RESET_DOWN_MILISEC_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 17)
#define PARAM_FAP21V_GFA_GENERAL_RESET_STAY_IN_RESET_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 18)
#define PARAM_FAP21V_GFA_GENERAL_RESET_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 19)
#define PARAM_FAP21V_GFA_GENERAL_INQ_TEST_RW_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 20)
#define PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_DATA_TO_TEST_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 21)
#define PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_NOF_ITERATION_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 22)
#define PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_OFFSET_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 23)
#define PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 24)
#define PARAM_FAP21V_GFA_GENERAL_CPU_TEST_RW_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 25)
#define PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 26)
#define PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_FULL_ALL_OFFSET_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 27)
#define PARAM_FAP21V_GFA_GENERAL_CALIBRATE_DELETE_FILE_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 28)
#define PARAM_FAP21V_GFA_GENERAL_CALIBRATE_LBG_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 29)
#define PARAM_FAP21V_GFA_GENERAL_CALIBRATE_FULL_ALL_ID \
                       (PARAM_FAP21V_GFA_START_RANGE_ID + 30)

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_PURE_DEFI_FAP21V_GFA_INCLUDED__*/
#endif
