/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_FE600_BSP_INCLUDED__
/* { */
#define __UI_ROM_DEFI_FE600_BSP_INCLUDED__

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

#include <appl/dpp/UserInterface/ui_pure_defi_fe600_bsp.h>
#include <appl/diag/dpp/utils_fe600_card.h>

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     fe600_bsp_free_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
EXTERN CONST
   PARAM_VAL_RULES
     fe600_bsp_empty_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

EXTERN CONST
   PARAM_VAL_RULES
     fe600_bsp_synt_sizer_type_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "none",
    {
      {
        FE600_SYNT_TYPE_NONE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  FE600_TG_SYNT_TYPE.FE600_SYNT_TYPE_NONE:\r\n"
        "  None \r\n"
        "",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "serdes",
    {
      {
        FE600_SYNT_TYPE_SERDES,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  FE600_TG_SYNT_TYPE.FE600_SYNT_TYPE_SERDES:\r\n"
        "  Serdes \r\n"
        "",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "core",
    {
      {
        FE600_SYNT_TYPE_CORE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  FE600_TG_SYNT_TYPE.FE600_SYNT_TYPE_CORE:\r\n"
        "  Core \r\n"
        "",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************/
/********************************************************/
EXTERN CONST
   PARAM
     Fe600_bsp_params[]
#ifdef INIT
   =
{
  {
    PARAM_FE600_BSP_I2C_BYTE_0,
    "byte_0",
    (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
    (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "","","",
    {BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_1,
      "byte_1",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    4,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_2,
      "byte_2",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    5,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_3,
      "byte_3",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    6,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_4,
      "byte_4",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    7,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_5,
      "byte_5",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    8,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_6,
      "byte_6",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    9,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_7,
      "byte_7",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    10,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_8,
      "byte_8",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    11,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_BYTE_9,
      "byte_9",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    12,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_NOF_BYTES,
      "nof_bytes",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(0)},
    2,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_GEN_WRITE,
      "gen_i2c_write",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Write Generic I2C device. Get device address","","7 bits device address",
    {BIT(0)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_GEN_READ,
      "gen_i2c_read",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Read from Generic I2C device. Get device address","","7 bits device address",
    {BIT(1)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FE600_BSP_I2C_VAL,
      "val",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","",
    {BIT(2)},
    2,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_FE600_WRITE,
      "fe600_i2c_write",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Write Generic I2C device.","","16 bits address",
    {BIT(2)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_I2C_FE600_READ,
      "fe600_i2c_read",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Read from Generic I2C device.","","16 bits address",
    {BIT(3)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FE600_BSP_RESET_DEVICE_REMAIN_OFF,
      "remain_off",
      (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
      (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "","","1: Do not come out of reset",
    {BIT(4)},
    2,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_RESET_DEVICE,
      "reset_device",
      (PARAM_VAL_RULES *)&fe600_bsp_empty_vals[0],
      (sizeof(fe600_bsp_empty_vals) / sizeof(fe600_bsp_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "BSP device reset.","","",
    {BIT(4)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_INIT_HOST_BOARD,
      "init_host_board",
      (PARAM_VAL_RULES *)&fe600_bsp_empty_vals[0],
      (sizeof(fe600_bsp_empty_vals) / sizeof(fe600_bsp_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Init the host board.","","",
    {BIT(5)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_SYNT_SET_ID,
    "synt",
    (PARAM_VAL_RULES *)&fe600_bsp_empty_vals[0],
    (sizeof(fe600_bsp_empty_vals) / sizeof(fe600_bsp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_SYNT_SET_SYN_TYPE_ID,
    "synt_type",
    (PARAM_VAL_RULES *)&fe600_bsp_synt_sizer_type_rule[0],
    (sizeof(fe600_bsp_synt_sizer_type_rule) / sizeof(fe600_bsp_synt_sizer_type_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  synt_type:\r\n"
    "  The synthisizer Type: \r\n"
    "",
    "",
    "",
    {BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_BSP_SYNT_SET_FREQ_VAL_ID,
    "freq",
    (PARAM_VAL_RULES *)&fe600_bsp_free_vals[0],
    (sizeof(fe600_bsp_free_vals) / sizeof(fe600_bsp_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  freq:\r\n"
    "  frequency value. \r\n"
    "",
    "",
    "",
    {BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * Last element. Do not remove.
   */
  {
    PARAM_END_OF_LIST
  }
};

#endif    /* } INIT*/

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif

#endif    /* } __UI_ROM_DEFI_FE600_BSP_INCLUDED__*/
