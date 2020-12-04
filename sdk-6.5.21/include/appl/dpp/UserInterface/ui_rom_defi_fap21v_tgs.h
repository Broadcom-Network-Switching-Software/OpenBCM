/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_FAP21V_TGS_INCLUDED__
/* { */
#define __UI_ROM_DEFI_FAP21V_TGS_INCLUDED__

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

#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_tgs.h>

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_tgs_free_vals[]
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
     Fap21v_tgs_empty_vals[]
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

/********************************************************/

EXTERN CONST
   PARAM
     fap21v_tgs_params[]
#ifdef INIT
   =
{
  {
    PARAM_FAP21V_TGS_APP_RUN_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&Fap21v_tgs_empty_vals[0],
    (sizeof(Fap21v_tgs_empty_vals) / sizeof(Fap21v_tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_TGS_APP_RUN_RESET_ID,
    "do_fpgas_reset",
    (PARAM_VAL_RULES *)&Fap21v_tgs_empty_vals[0],
    (sizeof(Fap21v_tgs_empty_vals) / sizeof(Fap21v_tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_TGS_APP_RUN_ID,
    "tgs_app",
    (PARAM_VAL_RULES *)&Fap21v_tgs_empty_vals[0],
    (sizeof(Fap21v_tgs_empty_vals) / sizeof(Fap21v_tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_TGS_APP_ID,
    "app",
    (PARAM_VAL_RULES *)&Fap21v_tgs_empty_vals[0],
    (sizeof(Fap21v_tgs_empty_vals) / sizeof(Fap21v_tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)},
    2,
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

#endif    /* } __UI_ROM_DEFI_FAP21V_TGS_INCLUDED__*/
