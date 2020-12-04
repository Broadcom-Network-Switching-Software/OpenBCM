/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_DEFI_INCLUDED
/* { */
#define UI_DEFI_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>


/*
 * General include file for user interface.
 */
#include <appl/dpp/UserInterface/ui_pure_defi.h>
/*
 * Rom variables.
 * {
 */
#ifdef EXTERN
#undef EXTERN
#endif
#define EXTERN extern
#ifdef INIT
#undef INIT
#endif
#ifdef CONST
#undef CONST
#endif
#define CONST const
#include <appl/dpp/UserInterface/ui_rom_defi.h>
/*
 * }
 */
#ifdef EXTERN
#undef EXTERN
#endif
#define EXTERN extern
/*
 * Ram variables.
 * {
 */
#include <appl/dpp/UserInterface/ui_ram_defi.h>
/*
 * }
 */

#if DUNE_BCM
  #define _SPRINTF sal_sprintf
#else
  #define _SPRINTF sprintf
#endif

/*
 * Assuming existence of 'ui_ret' and 'err_msg'.
 */
#define UI_MACROS_INIT_FUNCTION(FUNCTION_NAME)             \
  int                                                      \
    ui_ret = FALSE;                                       \
  PARAM_VAL                                                \
    *param_val=NULL;                                            \
  unsigned long                                            \
    error_id = 0;                                          \
  char                                                     \
    err_msg[80*8];                                         \
  const char                                               \
    *soc_sand_proc_name = "Need to init 'soc_sand_proc_name' variable", \
    *proc_name = FUNCTION_NAME;                            \
  unsigned int                                             \
    match_index = 0,                                       \
    unit = 0;                                         \
                                                             \
  if(1)                                                      \
  {                                                          \
    _SPRINTF(err_msg,                                         \
    "\r\n"                                                   \
    "*** \'%s\' error \r\n"                                  \
    "*** or \'unknown parameter \'.\r\n"                     \
    "    Probably SW error\r\n",                             \
    FUNCTION_NAME                                            \
    ); /* Removing the unused variable warnings */           \
    SOC_SAND_IGNORE_UNUSED_VAR(error_id);                        \
    SOC_SAND_IGNORE_UNUSED_VAR(match_index);                     \
    SOC_SAND_IGNORE_UNUSED_VAR(param_val);                       \
    SOC_SAND_IGNORE_UNUSED_VAR(unit);                       \
    SOC_SAND_IGNORE_UNUSED_VAR(proc_name[0]);                    \
    SOC_SAND_IGNORE_UNUSED_VAR(soc_sand_proc_name[0]);               \
    SOC_SAND_IGNORE_UNUSED_VAR(err_msg[0]);                      \
  }




#define UI_MACROS_CHECK_GET_VAL_OF_ERROR                   \
    if (ui_ret)                                       \
    {                                                      \
      soc_sand_os_printf("\n %s.\n SW get error\n",soc_sand_proc_name) ;     \
      goto exit ;                                          \
    }



#define  UI_MACROS_ERR_UNRECOGNIZED_PARAM(_ui_current_param)  \
  soc_sand_os_printf(                                             \
            "\r\n\n"                                          \
            "*** Parameter following \'%s\' is unrecognizable\r\n",#_ui_current_param) ; \
        send_string_to_screen(err_msg,TRUE) ;                 \
        ui_ret = TRUE ;                                       \
        goto exit ;
/*
 * Assuming one appearance of value.
 * Assuming existence of 'ui_ret' and 'param_val'.
 */
#define UI_MACROS_GET_NUMMERIC_VAL(val_id)                 \
    ui_ret =                                                  \
      get_val_of(                                          \
        current_line,(int *)&match_index,                  \
        val_id,1,                                          \
        &param_val,VAL_NUMERIC,err_msg                     \
        );

#define UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(val_id, ndx) \
    ui_ret =                                               \
      get_val_of(                                          \
        current_line,(int *)&match_index,                  \
        val_id,ndx,                                        \
        &param_val,VAL_NUMERIC,err_msg                     \
        );

/*
 * Assuming one appearance of value.
 * Assuming existence of 'ui_ret' and 'param_val'.
 */
#define UI_MACROS_GET_SYMBOL_VAL(val_id)                   \
    ui_ret =                                               \
      get_val_of(                                          \
        current_line,(int *)&match_index,                  \
        val_id,1,                                          \
        &param_val,VAL_SYMBOL,err_msg                      \
        );



/*
 * Assuming one appearance of value.
 * Assuming existence of 'ui_ret' and 'param_val'.
 */
#define UI_MACROS_GET_NUM_SYM_VAL(val_id)                  \
    ui_ret =                                               \
      get_val_of(                                          \
        current_line,(int *)&match_index,                  \
        val_id,1,                                          \
        &param_val,VAL_NUMERIC|VAL_SYMBOL,err_msg          \
        );

#define UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(val_id, ndx)  \
    ui_ret =                                               \
      get_val_of(                                          \
        current_line,(int *)&match_index,                  \
        val_id, ndx,                                       \
        &param_val,VAL_NUMERIC|VAL_SYMBOL,err_msg          \
        );

/*
 * Assuming one appearance of value.
 * Assuming existence of 'ui_ret' and 'param_val'.
 */
#define UI_MACROS_GET_TEXT_VAL(val_id)                 \
    ui_ret =                                               \
      get_val_of(                                          \
        current_line,(int *)&match_index,                  \
        val_id,1,                                          \
        &param_val,VAL_TEXT,err_msg                        \
        );

/*
 * Assuming one appearance of value.
 * Assuming existence of 'param_val'.
 * Handles VAL_NUMERIC/VAL_SYMBOL/VAL_IP
 */
#define UI_MACROS_LOAD_TEXT_VAL(val)\
    if(param_val->val_type == VAL_TEXT)\
    {\
      val = param_val->value.val_text ;\
    }

/*
 * Assuming one appearance of value.
 * Assuming existence of 'param_val'.
 * Handles VAL_NUMERIC/VAL_SYMBOL/VAL_IP
 */
#define UI_MACROS_LOAD_LONG_VAL(val)\
    if(param_val->val_type == VAL_NUMERIC)\
    {\
      val = param_val->value.ulong_value ;\
    }\
    else if(param_val->val_type == VAL_SYMBOL)\
    {\
      val = param_val->numeric_equivalent ;\
    }\
    else\
    {\
      val = param_val->value.ip_value;\
    }

/*
 * Assuming existence of current_line, match_index
 */
#define UI_MACROS_MATCH_ID(_ui_val_id) \
  !search_param_val_pairs(current_line,&match_index,_ui_val_id,1)

#define UI_COMP(var_ptr1, var_ptr2, type, count, is_equal_res)         \
  {                                                                       \
    is_equal_res = SOC_SAND_NUM2BOOL_INVERSE(soc_sand_os_memcmp(                  \
            var_ptr1,                                                     \
            var_ptr2,                                                     \
            (count) * sizeof(type)                                        \
          ));                                                             \
  }

/* } */
#endif
