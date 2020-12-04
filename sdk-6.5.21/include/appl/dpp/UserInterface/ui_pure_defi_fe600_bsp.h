/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_PURE_DEFI_FE600_BSP_INCLUDED__
/* { */
#define __UI_PURE_DEFI_FE600_BSP_INCLUDED__

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
 * the following definitions must range between PARAM_FE600_BSP_START_RANGE_ID
 * and PARAM_FE600_BSP_END_RANGE_ID.
 * See ui_pure_defi.h
 */

#define PARAM_FE600_BSP_RESET_DEVICE_REMAIN_OFF    (PARAM_FE600_BSP_START_RANGE_ID + 0 )
#define PARAM_FE600_BSP_RESET_DEVICE               (PARAM_FE600_BSP_START_RANGE_ID + 1 )
#define PARAM_FE600_BSP_INIT_HOST_BOARD            (PARAM_FE600_BSP_START_RANGE_ID + 2 )

#define PARAM_FE600_BSP_I2C_GEN_READ               (PARAM_FE600_BSP_START_RANGE_ID + 4 )
#define PARAM_FE600_BSP_I2C_GEN_WRITE              (PARAM_FE600_BSP_START_RANGE_ID + 5 )
#define PARAM_FE600_BSP_I2C_BYTE_0                 (PARAM_FE600_BSP_START_RANGE_ID + 6 )
#define PARAM_FE600_BSP_I2C_BYTE_1                 (PARAM_FE600_BSP_START_RANGE_ID + 7 )
#define PARAM_FE600_BSP_I2C_BYTE_2                 (PARAM_FE600_BSP_START_RANGE_ID + 8 )
#define PARAM_FE600_BSP_I2C_BYTE_3                 (PARAM_FE600_BSP_START_RANGE_ID + 9 )
#define PARAM_FE600_BSP_I2C_BYTE_4                 (PARAM_FE600_BSP_START_RANGE_ID + 10)
#define PARAM_FE600_BSP_I2C_BYTE_5                 (PARAM_FE600_BSP_START_RANGE_ID + 11)
#define PARAM_FE600_BSP_I2C_BYTE_6                  (PARAM_FE600_BSP_START_RANGE_ID + 12)
#define PARAM_FE600_BSP_I2C_BYTE_7                 (PARAM_FE600_BSP_START_RANGE_ID + 13)
#define PARAM_FE600_BSP_I2C_BYTE_8                 (PARAM_FE600_BSP_START_RANGE_ID + 14)
#define PARAM_FE600_BSP_I2C_BYTE_9                 (PARAM_FE600_BSP_START_RANGE_ID + 15)
#define PARAM_FE600_BSP_I2C_NOF_BYTES              (PARAM_FE600_BSP_START_RANGE_ID + 16)

#define PARAM_FE600_BSP_I2C_VAL                    (PARAM_FE600_BSP_START_RANGE_ID + 19)
#define PARAM_FE600_BSP_I2C_FE600_WRITE            (PARAM_FE600_BSP_START_RANGE_ID + 20)
#define PARAM_FE600_BSP_I2C_FE600_READ             (PARAM_FE600_BSP_START_RANGE_ID + 21)

#define PARAM_FE600_BSP_SYNT_SET_ID                (PARAM_FE600_BSP_START_RANGE_ID + 22)
#define PARAM_FE600_BSP_SYNT_SET_SYN_TYPE_ID       (PARAM_FE600_BSP_START_RANGE_ID + 23)
#define PARAM_FE600_BSP_SYNT_SET_FREQ_VAL_ID       (PARAM_FE600_BSP_START_RANGE_ID + 24)
/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_PURE_DEFI_FE600_BSP_INCLUDED__*/
#endif
