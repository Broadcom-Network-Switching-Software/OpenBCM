/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_FE200_INCLUDED
/* { */
#define UI_ROM_DEFI_FE200_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * User Interface include file.
 */
#include <appl/diag/dpp/dune_chips.h>
#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>

#include <soc/dpp/SAND/Management/sand_chip_consts.h>
#include <ChipSim/chip_sim_fe200_chip_defines_indirect.h>

#include <ChipSim/chip_sim.h>
#include <soc/dpp/SOC_SAND_FE200/fe200_api_general.h>
#include <soc/dpp/SOC_SAND_FE200/fe200_api_statistics.h>
#include <soc/dpp/SOC_SAND_FE200/fe200_chip_defines.h>
#include <soc/dpp/SOC_SAND_FE200/fe200_api_framework.h>


#include <ui_pure_defi.h>
#include <ui_pure_defi_fe200.h>



/********************************************************
*NAME
*  Fe200_cell_body_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_cell_body_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        8,
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
;
/********************************************************
*NAME
*  Fe200_repeater_table_body
*TYPE: BUFFER
*DATE: 06/APR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_repeater_table_body[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        64,
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
/********************************************************
*NAME
*  Fe200_list_of_links_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_list_of_links_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        2,
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


/********************************************************
*NAME
*  Fe200_initial_output_link_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_initial_output_link_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        63,0,
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
/********************************************************
*NAME
*  Fe200_fe2_link_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_fe2_link_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        63,0,
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
/********************************************************
*NAME
*  Fe200_fe2_link_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_fe2_link_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;
/********************************************************
*NAME
*  Fe200_fe3_link_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_fe3_link_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        31,0,
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
/********************************************************
*NAME
*  Fe200_fe3_link_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_fe3_link_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;
/********************************************************
*NAME
*  Fe200_source_chip_id_num_val
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_source_chip_id_num_val[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        2047,0,
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
/********************************************************
*NAME
*  Fe200_source_chip_id_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_source_chip_id_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)-1
  }
}
#endif
;

/********************************************************
*NAME
*  Fe200_local_loopback_on_vals
*TYPE: BUFFER
*DATE: 15/APR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_MIBS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_local_loopback_on_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    loppback mode is set to local.",
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
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    loppback mode is not set to local.",
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
/********************************************************
*NAME
*  Fe200_activate_prbs_generator_vals
*TYPE: BUFFER
*DATE: 15/APR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_MIBS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_activate_prbs_generator_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Activate prbs generator.",
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
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Deactivate prbs generator.",
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
/********************************************************
*NAME
*  Fe200_remain_reset_vals
*TYPE: BUFFER
*DATE: 22/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_MIBS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_remain_reset_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Device remain reset.",
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
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Device snaps out of reset immedaitely.",
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
/********************************************************
*NAME
*  Fe200_Interrupt_mockup_vals
*TYPE: BUFFER
*DATE: 02/JUN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_MIBS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_Interrupt_mockup_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "mockup_interruupts",
    {
      {
        1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Devices interrupt line is not connected.",
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
    "dont_mockup_interruupts",
    {
      {
        2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Devices interrupt line is connected.",
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
/********************************************************
*NAME
*  Fe200_Interrupt_mockup_default
*TYPE: BUFFER
*DATE: 03/JUN/2003
*FUNCTION:
*  Default number of packets to send is 1.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_Interrupt_mockup_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  2,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"dont_mockup_interruupts"
  }
}
#endif
;
/********************************************************
*NAME
*  Fe200_mac_counter_type_vals
*TYPE: BUFFER
*DATE: 15/APR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_MIBS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_mac_counter_type_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "all_cells",
    {
      {
        COUNT_ALL_CELLS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Counts all cells.",
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
    "control_cells",
    {
      {
        COUNT_CONTROL_CELLS_ONLY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Counts control cells.",
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
    "non-empty_data_cells",
    {
      {
        COUNT_PKT_DATA_CELLS_ONLY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Counts packet data cells.",
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
    "empty_data_cells",
    {
      {
        COUNT_EMPTY_DATA_CELLS_ONLY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Counts empty data cells.",
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

/********************************************************
*NAME
*  Fe200_logical_field
*TYPE: BUFFER
*DATE: 31/DEC/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DATA".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_logical_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "chip_id",
    {
      {
        FE200_CHIP_ID_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'chip id\' field on \'configuration\' register.",
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
    "mod_fe2",
    {
      {
        FE200_MODE_FE2_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'mode FE2\' field on \'configuration\' register.",
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
    "repeater",
    {
      {
        FE200_REPEATER_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'repeater\' field on \'configuration\' register.",
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
    "planes",
    {
      {
        FE200_PLANES_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'planes\' field on \'configuration\' register.",
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
    "multistage",
    {
      {
        FE200_MULTISTAGE_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'multistage\' field on \'configuration\' register.",
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
    "configuration",
    {
      {
        FE200_CONFIGURATION_REG,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write all fields on \'configuration\' register.",
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
    "mci_4_lvl_en",
    {
      {
        FE200_MCI_4_LVL_EN_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Read \'MCI4PriEn\' field.",
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
    "dcsa_mask_interrupt",
    {
      {
        FE200_DCSAINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'dcsa_mask_interrupt\' field.",
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
    "dcsb_mask_interrupt",
    {
      {
        FE200_DCSBINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'dcsb_mask_interrupt\' field.",
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
    "dcs_mask_interrupt",
    {
      {
        FE200_DCSINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'dcs_mask_interrupt\' field.",
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
    "ccs_mask_interrupt",
    {
      {
        FE200_CCSINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'ccs_mask_interrupt\' field.",
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
    "ccsa_mask_interrupt",
    {
      {
        FE200_CCSAINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'ccsa_mask_interrupt\' field.",
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
    "ccsb_mask_interrupt",
    {
      {
        FE200_CCSBINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'ccsb_mask_interrupt\' field.",
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
    "rtp_mask_interrupt",
    {
      {
        FE200_RTPINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'rtp_mask_interrupt\' field.",
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
    "mac_mask_interrupt",
    {
      {
        FE200_MACINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'mac_mask_interrupt\' field.",
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
    "maca_mask_interrupt",
    {
      {
        FE200_MACAINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'maca_mask_interrupt\' field.",
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
    "macb_mask_interrupt",
    {
      {
        FE200_MACBINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'macb_mask_interrupt\' field.",
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
    "macc_mask_interrupt",
    {
      {
        FE200_MACCINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'macc_mask_interrupt\' field.",
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
    "macd_mask_interrupt",
    {
      {
        FE200_MACDINT_MASK_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'macd_mask_interrupt\' field.",
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
/********************************************************
*NAME
*  Fe200_logical_write_field
*TYPE: BUFFER
*DATE: 11/JUN/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DATA".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_logical_write_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "chip_id",
    {
      {
        FE200_CHIP_ID_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write \'chip id\' field on \'configuration\' register.",
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
    "mci_4_lvl_en",
    {
      {
        FE200_MCI_4_LVL_EN_FIELD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"     \'MCI4PriEn\' field.",
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


/********************************************************
*NAME
*  Fe200_empty_vals
*TYPE: BUFFER
*DATE: 07/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING EMPTY LIST OF VALUES
*  RELATED TO ANY PARAMETER OF ANY SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_empty_vals[]
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

/********************************************************
*NAME
*  Fe200_indefinite_num_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_indefinite_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
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

/********************************************************
*NAME
*  Fe200_entity_type
*TYPE: BUFFER
*DATE: 1/JAN/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DATA".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_entity_type[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "fe1",
    {
      {
        SOC_SAND_FE1_ENTITY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    fe1 entity type.",
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
    "fe2",
    {
      {
        SOC_SAND_FE2_ENTITY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    fe2 entity type.",
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
    "fe3",
    {
      {
        SOC_SAND_FE3_ENTITY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    fe3 entity type.",
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
    "fip",
    {
      {
        SOC_SAND_FIP_ENTITY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    fip entity type.",
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
    "fop",
    {
      {
        SOC_SAND_FOP_ENTITY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    fop entity type.",
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
    "don't care",
    {
      {
        SOC_SAND_DONT_CARE_ENTITY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    don't care what's entity type.",
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


/********************************************************
*NAME
*  Fe200_recv_cell_links
*TYPE: BUFFER
*DATE: 06/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DATA".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_recv_cell_links[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "range_0-31",
    {
      {
        0x1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    low (fe1) recieve cell buffer",
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
    "range_32-63",
    {
      {
        0x2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    high (fe3) recieve cell buffer",
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
    "both",
    {
      {
        0x3,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    both recieve cell buffers",
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

/********************************************************
*NAME
*  Fe200_serdes_number_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_serdes_number_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        63,0,
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


/********************************************************
*NAME
*  Fe200_serdes_tpvl_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_serdes_tpvl_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FE200_MAX_TPVL_VALUE,0,
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

/********************************************************
*NAME
*  Fe200_serdes_amplitude_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_serdes_amplitude_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        7,0,
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
/********************************************************
*NAME
*  Fe200_serdes_amplitude_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_serdes_amplitude_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
*NAME
*  Fe200_serdes_common_mode_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_serdes_common_mode_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        3,0,
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
/********************************************************
*NAME
*  Fe200_serdes_common_mode_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_serdes_common_mode_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)2
  }
}
#endif
;
/********************************************************
*NAME
*  Fe200_serdes_main_control_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_serdes_main_control_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        3,0,
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
/********************************************************
*NAME
*  Fe200_serdes_main_control_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_serdes_main_control_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;
/********************************************************
*NAME
*  Fe200_serdes_power_down_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_serdes_power_down_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1,0,
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
/********************************************************
*NAME
*  Fe200_serdes_power_down_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_serdes_power_down_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;
/********************************************************
*NAME
*  Fe200_serdes_pre_emphasis_num_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe200_serdes_pre_emphasis_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        3,0,
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
/********************************************************
*NAME
*  Fe200_serdes_pre_emphasis_default
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  Default number if no amplitude.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fe200_serdes_pre_emphasis_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)2
  }
}
#endif
;

/********************************************************
*NAME
*  Fe200_params
*TYPE: BUFFER
*DATE: 29/Dec/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "SOC_SAND_FE200".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      THIS FIELD IS AUTOMATICALLY FILLED IN BY THE SYSTEM
*      and does NOT need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Fe200_params[]
#ifdef INIT
   =
{
  {
    PARAM_FE200_GENRAL_API_ID,
    "general",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fe200 driver general services.\r\n",
    "Examples:\r\n"
    "  fe200 general start_driver\r\n"
    "Results in starting the fe200 driver.\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFEREED_API_ID,
    "deferred",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fe200 driver deferred services.\r\n",
    "Examples:\r\n"
    "  fe200 deferred services deferred_physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
    "",
    {BIT(3) | BIT(6) | BIT(9) | BIT(10) | BIT(13) | BIT(15) | BIT(24) | BIT(27),
     BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_START_DRIVER_ID,
    "driver_start",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts the Fe200_fe200 driver.\r\n",
    "Examples:\r\n"
    "  fe200 start_driver\r\n"
    "Results in starting the fe200 driver.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SHUTDOWN_DRIVER_ID,
    "driver_shutdown",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Shuts down Fe200_fe200 driver completly.\r\n",
    "Examples:\r\n"
    "  fe200 shutdown_driver\r\n"
    "Results in shutting down the fe200 driver.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_REGISTER_DEVICE_ID,
    "register_device",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "registers an Fe200_fe200 device with the driver.\r\n",
    "Examples:\r\n"
    "  fe200 register_device 0x30043745 \r\n"
    "Results in registering an fe200 device at address 0x30043745 with the driver.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_UNREGISTER_DEVICE_ID,
    "unregister_device",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregisters an Fe200_fe200 device from the driver.\r\n",
    "Examples:\r\n"
    "  fe200 unregister_device 0x1 \r\n"
    "Results in unregistering an fe200 device that was registered under the handle 0x1.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_RESET_DEVICE_ID,
    "reset_device",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "resets an Fe200_fe200 device.\r\n",
    "Examples:\r\n"
    "  fe200 general reset_device 0x1 remain_reset on\r\n"
    "Results in resetting device 0x1.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_RESET_REMAIN_ACTIVE_ID,
    "remain_reset",
    (PARAM_VAL_RULES *)&Fe200_remain_reset_vals[0],
    (sizeof(Fe200_remain_reset_vals) / sizeof(Fe200_remain_reset_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "resets an Fe200_fe200 device.\r\n",
    "Examples:\r\n"
    "  fe200 general reset_device 0x1 remain_reset on\r\n"
    "Results in resetting device 0x1.\r\n",
    "",
    {BIT(0) | BIT(30)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_START_AND_REGISTER_ID,
    "start_driver_and_register_devices",
    (PARAM_VAL_RULES *)&Fe200_Interrupt_mockup_vals[0],
    (sizeof(Fe200_Interrupt_mockup_vals) / sizeof(Fe200_Interrupt_mockup_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fe200_Interrupt_mockup_default,
    0,0,0,0,0,0,0,
    "starts the fe200 driver and register 2 devices (with or without ISR).\r\n",
    "Examples:\r\n"
    "  fe200 general start_driver_and_register_devices mockup_inetrrupts \r\n"
    "Results in activating the driver, registering 2 devices, and simulating interrupts by polling.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_UNREGISTER_AND_SHUTDOWN_ID,
    "unregister_devices_and_shutdown_driver",
    (PARAM_VAL_RULES *)&Fe200_Interrupt_mockup_vals[0],
    (sizeof(Fe200_Interrupt_mockup_vals) / sizeof(Fe200_Interrupt_mockup_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregister 2 fe200 devices and shutdown the driver.\r\n",
    "Examples:\r\n"
    "  fe200 general unregister_devices_and_shutdown_driver mockup_interrupts \r\n"
    "Results in unregistering the devices and shutting down the driver.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GRACEFUL_SHUTDOWN_DEVICE_ID,
    "graceful_shutdown",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Stops reachability messages generation of device, allowing it to be gracefuly removed from the system.\r\n",
    "Examples:\r\n"
    "  fe200 graceful_shutdown 0x1 \r\n"
    "Results in gracefuly removing the fe200 device with the handle 0x1 from the switching network.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GRACEFUL_RESTORE_DEVICE_ID,
    "graceful_restore",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "restores a register fe200 device by taking it out of reset and starting reachability messages.\r\n",
    "Examples:\r\n"
    "  fe200 graceful_restore 0x1 \r\n"
    "Results in restoring the registered fe200 device with the handle 0x1.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_CLEAR_DEVICE_PENDING_SERVICES_ID,
    "clear_device_pending_services",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "clears all pending services of an Fe200_fe200 device.\r\n",
    "Examples:\r\n"
    "  fe200 clear_device_pending_services 0x1 \r\n"
    "Results in clearing all services registered for an fe200 device with the handle 0x1.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_STOP_POLLING_TRANSACTON_ID,
    "stop_polling_transaction",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregister one deferred service accroding to its handle.\r\n",
    "Examples:\r\n"
    "  fe200 stop_polling_transaction 0x28342359 \r\n"
    "Results in unregistered the service with handle 0x28342359.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_STOP_ALL_POLLING_ID,
    "stop_all_interrupt_and_polling_handling",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Stops Timed Callback Machine from performing any callback action.\r\n"
    "  (including interrupt handling)\r\n",
    "Examples:\r\n"
    "  fe200 general stop_all_polling_actions \r\n"
    "Results in stopping TCM from activating any callback action.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_START_ALL_POLLING_ID,
    "start_all_interrupt_and_polling_handling",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts Timed Callback Machine to perform callback actions (again).\r\n",
    "Examples:\r\n"
    "  fe200 general start_all_polling_actions \r\n"
    "Results in starting TCM to activate callback actions (again).\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_PROFILE_ID,
    "get_profile",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get the configuration profile that this device currently operates under.\r\n",
    "Examples:\r\n"
    "  fe200 general get_profile 0x1 \r\n"
    "Results in printing the current configuration profile device 0x1 operates under.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_PROFILE_ID,
    "set_profile",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the configuration profile that this device will operate under.\r\n"
    " Currently only default profile is defined",
    "Examples:\r\n"
    "  fe200 general set_profile 0x1 \r\n"
    "Results in setting the default configuration profile to device 0x1.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PHYSICAL_WRITE_ID,
    "physical_write",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "writes data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 physical_write 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PHYSICAL_READ_ID,
    "physical_read",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "reads data from the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 physical_read 0x1 offset 0x8 size 0x4 \r\n"
    "Results in reading data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFERRED_PHYSICAL_READ_ID,
    "physical_read",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deffred read of data from the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_LOGICAL_WRITE_ID,
    "logical_write",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "writes data to the specified device at the register / field.\r\n",
    "Examples:\r\n"
    "  fe200 logical_write 0x1 field chip_id data 0x3 \r\n"
    "Results in writing the value 0x3 to the chip_id field of fe 0x1.\r\n",
    "",
    {BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_LOGICAL_READ_ID,
    "logical_read",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "reads specified register / field from the device.\r\n",
    "Examples:\r\n"
    "  fe200 logical_read 0x1 field chip_id \r\n"
    "Results in reading the value of the chip_id field from fe 0x1.\r\n",
    "",
    {BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFERRED_LOGICAL_READ_ID,
    "logical_read",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred read of the specified register / field from the device.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_logical_read 0x1 field chip_id polling_rate 50\r\n"
    "Results in polling of the field chip_id field from fe 0x1, every 50 time ticks.\r\n",
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
    PARAM_FE200_PHYSICAL_DIRECT_ID,
    "direct",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 physical_write direct 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PHYSICAL_INDIRECT_ID,
    "indirect",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 physical_write indirect 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in indirect writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PHYSICAL_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 physical_write direct 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PHYSICAL_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 physical_write 0x1 direct offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PHYSICAL_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 physical_write 0x1 direct offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(4)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_POLLING_RATE_ID,
    "polling_rate",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of a deferred read of data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
    "",
    {BIT(3) | BIT(6) | BIT(9) | BIT(13) | BIT(15) | BIT(24) | BIT(27),
     BIT(2)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_READ_FIELD_ID,
    "field",
    (PARAM_VAL_RULES *)&Fe200_logical_field[0],
    (sizeof(Fe200_logical_field) / sizeof(Fe200_logical_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the name of the specific register / field.\r\n",
    "Examples:\r\n"
    "  fe200 logical_write 0x1 field chip_id 0x3 \r\n"
    "Results in writing the value 0x3 to the chip_id field of fe 0x1.\r\n",
    "",
    {BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_WRITE_FIELD_ID,
    "field",
    (PARAM_VAL_RULES *)&Fe200_logical_write_field[0],
    (sizeof(Fe200_logical_write_field) / sizeof(Fe200_logical_write_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the name of the specific register / field.\r\n",
    "Examples:\r\n"
    "  fe200 logical_write 0x1 field chip_id 0x3 \r\n"
    "Results in writing the value 0x3 to the chip_id field of fe 0x1.\r\n",
    "",
    {BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_CELL_API_ID,
    "traffic",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fe200 driver traffic (cell) services.\r\n",
    "Examples:\r\n"
    "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
    "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7) | BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_ID,
    "send_sr_data_cell",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sends a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
    "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_INIT_OUT_LINK_ID,
    "initial_output_link",
    (PARAM_VAL_RULES *)&Fe200_initial_output_link_num_vals[0],
    (sizeof(Fe200_initial_output_link_num_vals) / sizeof(Fe200_initial_output_link_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the link to choose when leavithis fe.\r\n",
    "Examples:\r\n"
      "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_FE2_LINK_ID,
    "fe2_link",
    (PARAM_VAL_RULES *)&Fe200_fe2_link_num_vals[0],
    (sizeof(Fe200_fe2_link_num_vals) / sizeof(Fe200_fe2_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fe200_fe2_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe2.\r\n",
    "Examples:\r\n"
      "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_FE3_LINK_ID,
    "fe3_link",
    (PARAM_VAL_RULES *)&Fe200_fe3_link_num_vals[0],
    (sizeof(Fe200_fe3_link_num_vals) / sizeof(Fe200_fe3_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fe200_fe3_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe3.\r\n",
    "Examples:\r\n"
      "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&Fe200_entity_type[0],
    (sizeof(Fe200_entity_type) / sizeof(Fe200_entity_type[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the entity this cell is destined to.\r\n",
    "Examples:\r\n"
      "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_SRC_ENTITY_TYPE_ID,
    "source_entity_type",
    (PARAM_VAL_RULES *)&Fe200_entity_type[0],
    (sizeof(Fe200_entity_type) / sizeof(Fe200_entity_type[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the entity this cell is originated in.\r\n",
    "Examples:\r\n"
      "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_ENTITY_TYPE_ID,
    "entity_type",
    (PARAM_VAL_RULES *)&Fe200_entity_type[0],
    (sizeof(Fe200_entity_type) / sizeof(Fe200_entity_type[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reporing entity or entity to access (FE1/FE2/FE3).\r\n",
    "Examples:\r\n"
    "  fe200 routing get_unicast_table 0x1 starting_row 2 entity_type fe1\r\n"
    "Results in printing the FE1 distribution unicast table of device\r\n"
    "#1 (assumed to be configured as FE13) starting at row 2.\r\n"
    "  fe200 routing get_unicast_table 0x1 starting_row 125 entity_type fe2\r\n"
    "Results in printing the routing unicast table of device #1 (assumed to\r\n"
    "be configured as FE2) starting at row 125.\r\n",
    "",
    {0,
     BIT(0) | BIT(1) | BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_SRC_CHIP_ID,
    "source_chip_id",
    (PARAM_VAL_RULES *)&Fe200_source_chip_id_num_val[0],
    (sizeof(Fe200_source_chip_id_num_val) / sizeof(Fe200_source_chip_id_num_val[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fe200_source_chip_id_default,
    0,0,0,0,0,0,0,
    "the entity this cell is originated in.\r\n",
    "Examples:\r\n"
      "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SEND_CELL_BODY_ID,
    "body",
    (PARAM_VAL_RULES *)&Fe200_cell_body_num_vals[0],
    (sizeof(Fe200_cell_body_num_vals) / sizeof(Fe200_cell_body_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the body of the cell.\r\n",
    "Examples:\r\n"
      "  fe200 traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_RECV_CELL_ID,
    "recv_sr_data_cell",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "recieves a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fe200 traffic recv_sr_data_cell 0x1 links 0-31 \r\n"
    "Results in reading a source routed data cell from device 0x1 'low' recieve cell buffer.\r\n",
    "",
    {BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_RECV_CELL_LINKS_ID,
    "links",
    (PARAM_VAL_RULES *)&Fe200_recv_cell_links[0],
    (sizeof(Fe200_recv_cell_links) / sizeof(Fe200_recv_cell_links[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "look into low (fe1) cell buffer (0-31) high (fe3) cell buffer (32-63) or both.\r\n",
    "Examples:\r\n"
    "  fe200 traffic recv_sr_data_cell 0x1 links range_0-31 \r\n"
    "Results in reading a source routed data cell from device 0x1 'low' recieve cell buffer.\r\n",
    "",
    {BIT(8) | BIT(9) | BIT(10) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFRRED_RECV_CELL_ID,
    "recv_sr_data_cell",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred receive (interrupt or polling) of a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fe200 deferred recv_sr_data_cell 0x1 links range_0-31 interrupt_mode \r\n"
    "Results in deferred reading a source routed data cell from device 0x1 'low'"
    " recieve cell buffer, which is triggered by an interrupt.\r\n",
    "",
    {BIT(9) | BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DEFERRED_FE200_RECV_CELL_POLL_MODE_ID,
    "polling_mode",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deferred call is triggered by simple polling of the device.\r\n",
    "Examples:\r\n"
    "  fe200 deferred recv_sr_data_cell 0x1 links range_0-31 polling_mode polling_rate 50 \r\n"
    "Results in deferred reading a source routed data cell from device 0x1 'low'"
    " recieve cell buffer, which is triggered by a polling action every 50 ticks.\r\n",
    "",
    {BIT(9)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DEFERRED_FE200_RECV_CELL_INT_MODE_ID,
    "interrupt_mode",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deferred call is triggered by an interrupt of the device.\r\n",
    "Examples:\r\n"
    "  fe200 deferred recv_sr_data_cell 0x1 links range_0-31 interrupt_mode\r\n"
    "Results in deferred reading a source routed data cell from device 0x1 'low'"
    " recieve cell buffer, which is triggered by an interrupt.\r\n",
    "",
    {BIT(10)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_LINKS_API_ID,
    "links",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fe200 driver links services.\r\n",
    "Examples:\r\n"
    "  fe200 cell_services send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
    "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(11) | BIT(12) | BIT (14) | BIT(17) | BIT(18) | BIT(30)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_LINKS_GET_TPVL,
    "get_tpvl",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the TPVL value (relates to all fabric serdeses).\r\n",
    "Examples:\r\n"
    "  fe200 links get_tpvl 0x1 \r\n"
    "Results in getting the TPVL value.\r\n",
    "",
    {BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_LINKS_SET_TPVL,
    "set_tpvl",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the TPVL value (relates to all fabric serdeses).\r\n",
    "Examples:\r\n"
    "  fe200 links set_tpvl 0x1 value 2\r\n"
    "Results in setting the TPVL value.\r\n",
    "",
    {BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TPVL_VALUE_ID,
    "value",
    (PARAM_VAL_RULES *)&Fe200_serdes_tpvl_vals[0],
    (sizeof(Fe200_serdes_tpvl_vals) / sizeof(Fe200_serdes_tpvl_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "value of the TPVL.\r\n",
    "Examples:\r\n"
    "  fe200 links set_tpvl 0x1 value 2\r\n"
    "Results in setting the TPVL value.\r\n",
    "",
    {BIT(18)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_SERDES_PARAMS_ID,
    "get_serdes_params",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the status report of all the serdeses.\r\n",
    "Examples:\r\n"
    "  fe200 get_serdes_params 0x1 \r\n"
    "Results in printing the ststus of all serdeses.\r\n",
    "",
    {BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_SERDES_PARAMS_ID,
    "set_serdes_params",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fe200 set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_RESET_SINGLE_SERDES_ID,
    "shutdown_single_serdes",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "resets a specific serdes.\r\n",
    "Examples:\r\n"
    "  fe200 links reset_single_serdes 0x1 serdes_number 8 remain_reset\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_SERDES_PARAMS_SERDES_NUMBER_ID,
    "serdes_number",
    (PARAM_VAL_RULES *)&Fe200_serdes_number_num_vals[0],
    (sizeof(Fe200_serdes_number_num_vals) / sizeof(Fe200_serdes_number_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fe200 set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12) | BIT(30)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_SERDES_PARAMS_AMPLITUDE_ID,
    "amplitude",
    (PARAM_VAL_RULES *)&Fe200_serdes_amplitude_num_vals[0],
    (sizeof(Fe200_serdes_amplitude_num_vals) / sizeof(Fe200_serdes_amplitude_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fe200_serdes_amplitude_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fe200 set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_SERDES_PARAMS_COMMON_MODE_ID,
    "common_mode",
    (PARAM_VAL_RULES *)&Fe200_serdes_common_mode_num_vals[0],
    (sizeof(Fe200_serdes_common_mode_num_vals) / sizeof(Fe200_serdes_common_mode_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fe200_serdes_common_mode_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fe200 set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_SERDES_PARAMS_MAIN_CONTROL_ID,
    "main_control",
    (PARAM_VAL_RULES *)&Fe200_serdes_main_control_num_vals[0],
    (sizeof(Fe200_serdes_main_control_num_vals) / sizeof(Fe200_serdes_main_control_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fe200_serdes_main_control_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fe200 set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_SERDES_PARAMS_POWER_DOWN_RX_TX_ID,
    "rx_tx_power_down",
    (PARAM_VAL_RULES *)&Fe200_serdes_power_down_num_vals[0],
    (sizeof(Fe200_serdes_power_down_num_vals) / sizeof(Fe200_serdes_power_down_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fe200_serdes_power_down_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fe200 set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_SERDES_PARAMS_PRE_EMPHASIS_ID,
    "pre_emphasis",
    (PARAM_VAL_RULES *)&Fe200_serdes_pre_emphasis_num_vals[0],
    (sizeof(Fe200_serdes_pre_emphasis_num_vals) / sizeof(Fe200_serdes_pre_emphasis_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fe200_serdes_pre_emphasis_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fe200 set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "present connectivity map the the chip sees\r\n",
    "Examples:\r\n"
    "  fe200 get_connectivity_map 0x1 \r\n"
    "Results in printing the connectivity map this chip sees.\r\n",
    "",
    {BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFERRED_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the status report of all the serdeses.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_get_connectivity_map 0x1 \r\n"
    "Results in printing the connectivity map this chip sees, and every time it changes.\r\n",
    "",
    {BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_LINKS_STATUS_ID,
    "get_links_status",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all 64 links (passed a 64bit bitmap, i.e. 2 hexa words).\r\n",
    "Examples:\r\n"
    "  fe200 get_links_status 0x1 list_of_links 0x0 0x1\r\n"
    "Results in printing the ststus of link #0.\r\n",
    "",
    {BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_LIST_OF_LINKS_ID,
    "list_of_links",
    (PARAM_VAL_RULES *)&Fe200_list_of_links_num_vals[0],
    (sizeof(Fe200_list_of_links_num_vals) / sizeof(Fe200_list_of_links_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fe200 get_links_status 0x1 list_of_links 0x0 0x1\r\n"
    "Results in printing the ststus of link #1.\r\n",
    "",
    {BIT(14) | BIT(15),
     BIT(1)},
    8,/* just so it would be after polling rate (==6) */
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFERRED_GET_LINKS_STATUS_ID,
    "get_links_status",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_get_links_status 0x1 list_of_links 0x0 0x1 interrupt_mode \r\n"
    "Results in printing the ststus of link #0 when changed.\r\n",
    "",
    {BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_ROUTING_API_ID,
    "routing",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Interface to fe200 driver routing services.\r\n",
    "Examples:\r\n"
    "  fe200 routing get_unicast_table 0x1 starting_row 2 entity_type fe1\r\n"
    "Results in printing the FE1 distribution unicast table of device\r\n"
    "#1 (assumed to be configured as FE13) starting at row 2.\r\n"
    "  fe200 routing get_unicast_table 0x1 starting_row 125 entity_type fe2\r\n"
    "Results in printing the routing unicast table of device #1 (assumed to\r\n"
    "be configured as FE2) starting at row 125.\r\n",
    "",
    {BIT(16) | BIT (20),
     BIT(0)| BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_STOP_REACHABILITY_ERRORS_ID,
    "stop_reachability_errors_collection",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "stops reachability errors collection at the device.\r\n",
    "Examples:\r\n"
    "  fe200 routing stop_reachability_errors_collection 0x1\r\n"
    "Results in stopping reachability errors collection at device #1.\r\n",
    "",
    {BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_START_REACHABILITY_ERRORS_ID,
    "start_reachability_errors_collection",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "starts reachability errors collection at the device.\r\n",
    "Examples:\r\n"
    "  fe200 routing start_reachability_errors_collection 0x1\r\n"
    "Results in starting reachability errors collection at device #1.\r\n",
    "",
    {BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_NEXT_REACHABILITY_ERROR_ID,
    "get_next_reachability_error",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets reachability error logged at the device.\r\n",
    "Examples:\r\n"
    "  fe200 get_next_reachability_error 0x1\r\n"
    "Results in printing the first pending error of device #1.\r\n",
    "",
    {BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_CLEAR_REACHABILITY_ERRORS_ID,
    "clear_reachability_errors",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "clears reachability errors logged at the device.\r\n",
    "Examples:\r\n"
    "  fe200 clear_reachability_errors 0x1\r\n"
    "Results in clearing all reachabilities errors of device #1.\r\n",
    "",
    {BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_UNICAST_TABLE_ID,
    "get_unicast_table",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets (max of 100 entires of) the unicast distribution / routing table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 get_unicast_table 0x1 starting_row 125 \r\n"
    "Results in printing the distribution / routing unicast table of device #1 starting at row 125.\r\n",
    "",
    {0,BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFERRED_GET_UNICAST_TABLE_ID,
    "get_unicast_table",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "peridically gets (max of 100 entires of) the unicast distribution / routing table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_get_unicast_table 0x1 starting_row 125 ... \r\n"
    "Results in peridically printing the distribution / routing table of device #1 starting at row 125.\r\n",
    "",
    {0,
     BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_MULTICAST_TABLE_ID,
    "get_multicast_table",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets (max of 100 entires of) the multicast distribution / routing table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 get_multicast_table 0x1 starting_row 125\r\n"
    "Results in printing the distribution / routing multicast table of device #1 starting at row 125.\r\n",
    "",
    {0,BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_STARTING_ROW_ID,
    "starting_row",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the unicast distribution / routing table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 deferred get_unicast_table 0x1 starting_row 125 entity_type fe2\r\n"
    "Results in peridically printing the routing table of device #1 (assumed to\r\n"
    "be configured as FE2).\r\n"
    "  fe200 deferred get_unicast_table 0x1 starting_row 2 entity_type fe1\r\n"
    "Results in peridically printing the distribution table of device #1 (assumed to\r\n"
    "be configured as FE13).\r\n",
    "",
    {0,
     BIT(0) | BIT(1) | BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_MULTICAST_TABLE_ID,
    "set_multicast_table_entry",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets one multicast entry in the distribution / routing table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 set_multicast_table 0x1 source_entity_type fe2 starting row 100\r\n"
    "  list_of_links 0x398 0x0456\r\n"
    "Results in setting multicast id 100 in the fe2 multicast dsitribution table\r\n"
    "to the above 64bit bitmap.\r\n",
    "",
    {0,
     BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_REPEATER_TABLE_ID,
    "get_repeater_table",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the (64 entries) repeater table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 get_repeater_table 0x1\r\n"
    "Results in printing the repeater table of device #1 (64 entries).\r\n",
    "",
    {BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_REPEATER_TABLE_ID,
    "set_repeater_table",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the (64 entries) repeater table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 set_repeater_table 0x1 table_records 1 2 3 4 5 6 7 8 9 10 11 12 ...\r\n"
    "sets the above 64 entries into device #1 repeater table (64 entries).\r\n",
    "",
    {BIT(20)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_REPEATER_TABLE_BODY_ID,
    "table_records",
    (PARAM_VAL_RULES *)&Fe200_repeater_table_body[0],
    (sizeof(Fe200_repeater_table_body) / sizeof(Fe200_repeater_table_body[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the (64 entries) repeater table of the device.\r\n",
    "Examples:\r\n"
    "  fe200 set_repeater_table 0x1 table_records 1 2 3 4 5 6 7 8 9 10 11 12 ...\r\n"
    "sets the above 64 entries into device #1 repeater table (64 entries).\r\n",
    "",
    {BIT(20)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DIAGNOSTICS_API_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fe200 driver diagnostics services.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services get_loopback_type 0x1\r\n"
    "Results in printing loopback mode device #1 is in.\r\n",
    "",
    {BIT(21) | BIT(22) | BIT(23)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_LOOPBACK_TYPE_ID,
    "get_loopback_type",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services get_loopback_type 0x1\r\n"
    "Results in printing loopback mode device #1 is in.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_LOOPBACK_TYPE_ID,
    "set_loopback_type",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_loopback_type 0x1 local_loop on\r\n"
    "Results in setting the loopback mode of device #1 to local loopback.\r\n",
    "",
    {BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_LOOPBACK_TYPE_ID,
    "local_loop",
    (PARAM_VAL_RULES *)&Fe200_local_loopback_on_vals[0],
    (sizeof(Fe200_local_loopback_on_vals) / sizeof(Fe200_local_loopback_on_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_loopback_type 0x1 local_loop on\r\n"
    "Results in setting the loopback mode of device #1 to local loopback.\r\n",
    "",
    {BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_PRBS_MODE_ID,
    "get_prbs_mode",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the prbs mode the device is in.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services get_prbs_mode 0x1\r\n"
    "Results in printing the prbs mode of device #1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_PRBS_MODE_ID,
    "set_prbs_mode",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the prbs mode of one of four the device's MAC modules.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_prbs mode 0x1 checker_link 15 generator_link 12 activate_generator on\r\n"
    "Results in setting the prbs params of MAC A.\r\n",
    "",
    {BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PRBS_CHECKER_LINK_ID,
    "checker_link",
    (PARAM_VAL_RULES *)&Fe200_serdes_number_num_vals[0],
    (sizeof(Fe200_serdes_number_num_vals) / sizeof(Fe200_serdes_number_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the link that one of the four device's MAC modules prbs checkers is sitting on (rx).\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_prbs mode 0x1 checker_link 15 generator_link 12 activate_generator on\r\n"
    "Results in setting the prbs params of MAC A.\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PRBS_GENERATOR_LINK_ID,
    "generator_link",
    (PARAM_VAL_RULES *)&Fe200_serdes_number_num_vals[0],
    (sizeof(Fe200_serdes_number_num_vals) / sizeof(Fe200_serdes_number_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the link that one of the four device's MAC modules prbs generator is sitting on (tx).\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_prbs mode 0x1 checker_link 15 generator_link 12 activate_generator on\r\n"
    "Results in setting the prbs params of MAC A.\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_PRBS_ACTIVATE_GENERATOR_ID,
    "activate_generator",
    (PARAM_VAL_RULES *)&Fe200_activate_prbs_generator_vals[0],
    (sizeof(Fe200_activate_prbs_generator_vals) / sizeof(Fe200_activate_prbs_generator_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates / deactivates one of the four device's MAC modules prbs generator (tx).\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_prbs mode 0x1 checker_link 15 generator_link 12 activate_generator on\r\n"
    "Results in setting the prbs params of MAC A.\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_START_PRBS_TEST_ID,
    "start_prbs_test",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "starts background collection of ber counters.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services start_prbs_test 0x1\r\n"
    "Results in the background collection of ber counters of device 1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_STOP_PRBS_TEST_ID,
    "stop_prbs_test",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "stops background collection of ber counters.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services stop_prbs_test 0x1\r\n"
    "Results in stopping the background collection of ber counters of device 1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_BER_COUNTERS_ID,
    "get_ber_counters",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of prbs units.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services get_ber_counters 0x1\r\n"
    "Results in printing the counter report of the 4 prbs units of device 1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DIAGNOSTICS_DUMP_REGS_ID,
    "dump_regs",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dump all direct registers.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics dump_regs 0x0\r\n"
    "Results in dump all direct registers of device 0.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFERRED_GET_BER_COUNTERS_ID,
    "get_ber_counters",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of prbs units.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_services get_ber_counters 0x1 polling_rate 60 \r\n"
    "Results in printing the counter report of the 4 prbs units of device 1 every second\r\n",
    "",
    {BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_STATISTICS_API_ID,
    "statistics",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fe200 driver statistics services.\r\n",
    "Examples:\r\n"
    "  fe200 statistics_services get_mac_counter_type 0x1\r\n"
    "Results in printing mac rx & tx counter type device #1 is in.\r\n",
    "",
    {BIT(25) | BIT(26)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_START_MAC_COUNTERS_COLLECTION_ID,
    "start_mac_counter_collection",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "starts background collection of mac cell counters.\r\n",
    "Examples:\r\n"
    "  fe200 statistics start_mac_counter_collection 0x1\r\n"
    "Results in the background collection of mac cell counters of device 1.\r\n",
    "",
    {BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_STOP_MAC_COUNTERS_COLLECTION_ID,
    "stop_mac_counter_collection",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "stops background collection of mac cell counters.\r\n",
    "Examples:\r\n"
    "  fe200 statistics stop_mac_counter_collection 0x1\r\n"
    "Results in stopping the background collection of mac cell counters of device 1.\r\n",
    "",
    {BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_MAC_COUNTER_TYPE_ID,
    "get_mac_counter_type",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets mac rx & tx counter type this device is in.\r\n",
    "Examples:\r\n"
    "  fe200 statistics_services get_mac_counter_type 0x1\r\n"
    "Results in printing mac rx & tx counter type device #1 is in.\r\n",
    "",
    {BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_SET_MAC_COUNTER_TYPE_ID,
    "set_mac_counter_type",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets mac rx & tx counter type of this device.\r\n",
    "Examples:\r\n"
    "  fe200 statistics_services set_mac_counter_type 0x1 rx_type all_cells tx_type empty_data_cells\r\n"
    "Results in setting device #1 mac rx counter type to 'all cells' and tx counter type to 'empty data calls'.\r\n",
    "",
    {BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#if !MAC_RX_COUNTER_INTERFERE_WTITH_LEAKYBUCKET
  {
    PARAM_FE200_MAC_COUNTER_RX_TYPE_ID,
    "rx_type",
    (PARAM_VAL_RULES *)&Fe200_mac_counter_type_vals[0],
    (sizeof(Fe200_mac_counter_type_vals) / sizeof(Fe200_mac_counter_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_loopback_type 0x1 local_loop on\r\n"
    "Results in setting the loopback mode of device #1 to local loopback.\r\n",
    "",
    {BIT(26)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
  {
    PARAM_FE200_MAC_COUNTER_TX_TYPE_ID,
    "tx_type",
    (PARAM_VAL_RULES *)&Fe200_mac_counter_type_vals[0],
    (sizeof(Fe200_mac_counter_type_vals) / sizeof(Fe200_mac_counter_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fe200 diagnostics_services set_loopback_type 0x1 local_loop on\r\n"
    "Results in setting the loopback mode of device #1 to local loopback.\r\n",
    "",
    {BIT(26)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_GET_MAC_COUNTERS_ID,
    "get_count_of_mac_cells",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of mac units.\r\n",
    "Examples:\r\n"
    "  fe200 statistics_services get_count_of_mac_cells 0x1\r\n"
    "Results in printing the counter report of the 64 links of device 1.\r\n",
    "",
    {BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_DEFERRED_GET_MAC_COUNTERS_ID,
    "get_counter_of_mac_cells",
    (PARAM_VAL_RULES *)&Fe200_indefinite_num_vals[0],
    (sizeof(Fe200_indefinite_num_vals) / sizeof(Fe200_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of mac units.\r\n",
    "Examples:\r\n"
    "  fe200 deferred_services get_counter_of_mac_cells 0x1 polling_rate 60 \r\n"
    "Results in printing the counter report of the 64 links of device 1 every second\r\n",
    "",
    {BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TESTS_API_ID,
    "tests",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fe200 driver tests services.\r\n",
    "Examples:\r\n"
    "  fe200 tests cells\r\n"
    "Results in testing fe200 driver cell module.\r\n",
    "",
    {BIT(31)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_1_ID,
    "chip_id",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 test scenario #1 (regression for the chip_id bug).\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_chip_id \r\n"
    "Results in activating test scenario 1.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_2_ID,
    "full_regression",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 full regression test.\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_full_regression \r\n"
    "Results in activating full regression test.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_3_ID,
    "direct_calls",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 full regression test.\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_full_regression \r\n"
    "Results in activating full regression test.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_4_ID,
    "cells",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 cells test.\r\n",
    "Examples:\r\n"
    "  fe200 tests cells\r\n"
    "Results in activating cells tests using both direct and deferred calls.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_10_ID,
    "self_cells",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Activates fe200 self cells test: Send cells via local loopback.\r\n",
    "Examples:\r\n"
    "  fe200 tests self_cells\r\n"
    "Results in activating cells tests, on local loopback, using both direct\r\n"
    "and deferred calls.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_5_ID,
    "links",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 links test.\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_links direct \r\n"
    "Results in activating links tests using direct (and not deferred calls).\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_6_ID,
    "routing",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 routing test.\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_routing direct \r\n"
    "Results in activating routing tests using direct (and not deferred calls).\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_7_ID,
    "statistics",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 statistics test.\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_statistics direct \r\n"
    "Results in activating statistics tests using direct (and not deferred calls).\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_8_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 diagnostics test.\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_diagnostics direct \r\n"
    "Results in activating diagnostics tests using direct (and not deferred calls).\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_9_ID,
    "general",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 agent general module test.\r\n",
    "Examples:\r\n"
    "  fe200 test_scenario_general \r\n"
    "Results in activating test on the general module through the FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SCENARIO_11_ID,
    "unreg_reg",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fe200 agent init_close module test.\r\n",
    "Examples:\r\n"
    "  fe200 test unreg_reg \r\n"
    "Results in unregistering all fe200-s, re-registering all fe200-s and varifying tha no memory leak occured cycle \r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SSR_RESET_ID,
    "ssr_reset",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "performs fe200 ssr reset.\r\n",
    "Examples:\r\n"
    "  fe200 test ssr_reset \r\n"
    "Results in performing SSR reset and varifying there is no memory leak\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE200_TEST_SSR_XOR_ID,
    "ssr_xor",
    (PARAM_VAL_RULES *)&Fe200_empty_vals[0],
    (sizeof(Fe200_empty_vals) / sizeof(Fe200_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "performs fe200 ssr xor validation.\r\n",
    "Examples:\r\n"
    "  fe200 test ssr_xor \r\n"
    "Results in double SSR reset, varifying the saved data is identical in each reset\r\n",
    "",
    {BIT(31)},
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
}
#endif
;

/* } */
#endif
