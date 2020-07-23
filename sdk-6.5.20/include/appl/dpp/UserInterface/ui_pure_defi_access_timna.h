/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_PURE_DEFI_ACCESS_TIMNA_INCLUDED__
/* { */
#define __UI_PURE_DEFI_ACCESS_TIMNA_INCLUDED__

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
 * the following definitions must range between PARAM_ACCESS_TIMNA_START_RANGE_ID
 * and PARAM_ACCESS_TIMNA_END_RANGE_ID.
 * See ui_pure_defi.h
 */
#define PARAM_ACCESS_TIMNA_READ_FIELD_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 1)
#define PARAM_ACCESS_TIMNA_WRITE_FIELD_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 2)
#define PARAM_ACCESS_TIMNA_WRITE_FIELD_VALUE_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 3)
#define PARAM_ACCESS_TIMNA_DEVICE_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 4)
#define PARAM_ACCESS_TIMNA_READ_IND_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 5)
#define PARAM_ACCESS_TIMNA_WRITE_IND_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 6)
#define PARAM_ACCESS_TIMNA_WRITE_IND_VAL_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 7)
#define PARAM_ACCESS_TIMNA_WRITE_BANK_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 8)
#define PARAM_ACCESS_TIMNA_WRITE_TBL_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 9)
#define PARAM_ACCESS_TIMNA_WRITE_IS_EX_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 10)
#define PARAM_ACCESS_TIMNA_IND_TABLE_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 11)
#define PARAM_ACCESS_TIMNA_READ_TO_ADD_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 12)


#define PARAM_ACCESS_TIMNA_TCAM_WRITE_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 21)
#define PARAM_ACCESS_TIMNA_TCAM_WRITE_VAL_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 22)
#define PARAM_ACCESS_TIMNA_TCAM_WRITE_MASK_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 23)

#define PARAM_ACCESS_TIMNA_TCAM_READ_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 24)

#define PARAM_ACCESS_TIMNA_TCAM_CLEAR_ENTRY_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 25)

#define PARAM_ACCESS_TIMNA_TCAM_INIT_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 31)

#define PARAM_ACCESS_TIMNA_TCAM_COMPARE_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 32)
#define PARAM_ACCESS_TIMNA_TCAM_COMPARE_VAL_ID \
                       (PARAM_ACCESS_TIMNA_START_RANGE_ID + 33)


/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_PURE_DEFI_ACCESS_TIMNA_INCLUDED__*/
#endif
