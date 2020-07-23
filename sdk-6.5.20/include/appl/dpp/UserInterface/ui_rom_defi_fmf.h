/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_FMF_INCLUDED
/* { */
#define UI_ROM_DEFI_FMF_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>
/*
 * FMF include file.
 */
#include <soc/dpp/fmfDriver/fmf_sys_wide_defs.h>
/*
 * SOC_SAND include files.
 */
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <soc/dpp/SOC_SAND_FE200/fe200_chip_defines.h>

/********************************************************
*NAME
*  Fmf_general_on_off_vals
*TYPE: BUFFER
*DATE: 31/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO PARAMETERS WHICH ARE ANSWERED BY EITHER
*  "ON" OR "OFF" (FOR EXAMPLE "USE_FLOW" IN THE
*  CONTEXT OF SUBJECT 'CONNECT_PP_TO_PP').
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
     Fmf_general_on_off_vals[]
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
        (long)"    Positive response. Activate.",
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
        (long)"    Negative response. Do not activate.",
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
*  Fmf_ip_address_vals
*TYPE: BUFFER
*DATE: 05/OCT/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE IP-ADDRESS PARAMETER.
*  FOR EXAMPLE, THE PARAMETER "UNIT_IP_ADDRESS" WITHIN
*  'ADD_DEVICE'.
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
     Fmf_ip_address_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {
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
*  Fmf_indefinite_num_vals
*TYPE: BUFFER
*DATE: 01/OCT/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER.
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
     Fmf_indefinite_num_vals[]
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
*  Fmf_chip_id_vals
*TYPE: BUFFER
*DATE: 09/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO "CHIP_ID" FIELD WITHIN "ADD_DEVICE".
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
     Fmf_chip_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        BIT(CHIP_ID_NUM_BITS) - 1,0,
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
*  Fmf_unit_vals
*TYPE: BUFFER
*DATE: 09/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO "DEVICE_ID" FIELD WITHIN "ADD_DEVICE".
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
     Fmf_unit_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        (SOC_SAND_MAX_DEVICE_TYPE * BIT(CHIP_ID_NUM_BITS)) - 1,0,
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
*  Fmf_pp_id_vals
*TYPE: BUFFER
*DATE: 09/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO "PP_ID" FIELD WITHIN any subject specifying
*  PP (such as "CONNECT_PP_TO_PP").
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
     Fmf_pp_id_vals[]
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
/********************************************************
*NAME
*  Fmf_unit_vals
*TYPE: BUFFER
*DATE: 16/OCT/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO "UNIT" FIELD WITHIN "FMF DEBUG"
*  CONTEXT.
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
     Fmf_unit_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FMF_MAX_NUM_UNITS - 1,0,
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
*  Fmf_format_vals
*TYPE: BUFFER
*DATE: 16/OCT/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FORMAT" IN THE CONTEXT
*  OF 'FMF DEBUG' SUBJECT.
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
     Fmf_format_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "long",
    {
      {
        FORMAT_LONG_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Detailed display format.",
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
    "short",
    {
      {
        FORMAT_SHORT_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Short, brief display format.",
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
*  Fmf_device_type_vals
*TYPE: BUFFER
*DATE: 21/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DEVICE_TYPE" IN THE CONTEXT
*  OF 'FMF DEBUG' SUBJECT.
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
     Fmf_device_type_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "fe2_type",
    {
      {
        FE2_DEVICE_TYPE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Operate on \'Fabric Element 2\' device.",
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
    "fe13_type",
    {
      {
        FE13_DEVICE_TYPE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Operate on \'Fabric Element 1 & 3\' device.",
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
    "fap_type",
    {
      {
        FAP_DEVICE_TYPE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Operate on \'Fabric Access\' device.",
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
    "any_device_type",
    {
      {
        ANY_DEVICE_TYPE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Operate on \'any\' device (all devices that match).",
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
   * Allow also any numeric value for debug purposes.
   */
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
*  Fmf_device_subtype_vals
*TYPE: BUFFER
*DATE: 21/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DEVICE_SUB_TYPE" IN THE CONTEXT
*  OF 'FMF DEBUG' SUBJECT.
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
     Fmf_device_subtype_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    FE200_DEVICE_SUBTYPE_STRING,
    {
      {
        FE200_DEVICE_SUBTYPE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Operate on \'SOC_SAND_FE200\' device.",
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
    FAP10M_DEVICE_SUBTYPE_STRING,
    {
      {
        FAP10M_DEVICE_SUBTYPE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Operate on \'SOC_SAND_FAP10M\' device.",
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
    "any_device_subtype",
    {
      {
        ANY_DEVICE_SUBTYPE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Operate on \'any\' device subtype (all subtypes that match).",
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
   * Allow also any numeric value for debug purposes.
   */
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
*  Fmf_debug_flags_vals
*TYPE: BUFFER
*DATE: 11/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DEBUG_FLAGS" IN THE CONTEXT
*  OF 'FMF DEBUG' SUBJECT.
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
     Fmf_debug_flags_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "fmf_debug_on",
    {
      {
        FMF_DEBUG_ON_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Activate FMF debug flag (debug mode on -> debug printouts).",
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
    "fmf_debug_off",
    {
      {
        FMF_DEBUG_OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Deactivate FMF debug flag (debug mode off -> NO debug printouts).",
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
    "fmc_n_debug_on",
    {
      {
        FMC_N_DEBUG_ON_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Activate FMC_N debug flag (debug mode on -> debug printouts).",
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
    "fmc_n_debug_off",
    {
      {
        FMC_N_DEBUG_OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Deactivate FMC_N debug flag (debug mode off -> NO debug printouts).",
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
    "dcl_debug_on",
    {
      {
        DCL_DEBUG_ON_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Activate DCL debug flag (debug mode on -> debug printouts).",
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
    "dcl_debug_off",
    {
      {
        DCL_DEBUG_OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Deactivate DCL debug flag (debug mode off -> NO debug printouts).",
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
    "show_all_debug_flags",
    {
      {
        SHOW_ALL_DEBUG_FLAGS_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Display all debug flags (DCL, FMC_N, FMF).",
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
*  Fmf_format_default
*TYPE: BUFFER
*DATE: 16/OCT/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "FORMAT" IN THE CONTEXT
*  OF 'FMF DEBUG' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to (identifier of)
*      'string_value' in 'value' below.. Only relevant
*      for val_type = VAL_SYMBOL
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
     Fmf_format_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FORMAT_SHORT_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"short"
  }
}
#endif
;
/********************************************************
*NAME
*  Fmf_empty_vals
*TYPE: BUFFER
*DATE: 18/SEP/2003
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
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fmf_empty_vals[]
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
*  Fmf_params
*TYPE: BUFFER
*DATE: 18/SEP/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "FMF".
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
*      Mutual exclusion syntax control. Up to 32*4 groups
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
     Fmf_params[]
#ifdef INIT
   =
{
  {
    PARAM_FMF_USE_FLOW_ID,
    "use_flow",
    (PARAM_VAL_RULES *)&Fmf_general_on_off_vals[0],
    (sizeof(Fmf_general_on_off_vals) / sizeof(Fmf_general_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Indicate whether to use \'flow labeling\' (\'on\') or\r\n"
    "  \'destination labeling (\'off\').",
    "Example:\r\n"
    "  fmf debug connect_pp_to_pp from_one_pp pp_id 11 to_one_pp pp_id 17\r\n"
    "  use_flow off\r\n"
    "Results in connecting the PP with id \'11\' to the PP with id \'17\'\r\n"
    "using \'destination labeling\'.\r\n",
    "",
    {
      BIT(7) | BIT(8) | BIT(9) | BIT(10),
      0
    },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_DST_PP_ID_ID,
    "dst_pp_id",
    (PARAM_VAL_RULES *)&Fmf_pp_id_vals[0],
    (sizeof(Fmf_pp_id_vals) / sizeof(Fmf_pp_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Id of destination Packet Processor to operate on.",
    "Example:\r\n"
    "  fmf debug connect_pp_to_pp from_one_pp pp_id 11 to_one_pp dst_pp_id 17\r\n"
    "  use_flow off\r\n"
    "Results in connecting the PP with id \'11\' to the PP with id \'17\'\r\n"
    "using \'destination labeling\'.\r\n",
    "",
    {
      BIT(9),
      0
    },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_TO_ALL_PPS_ID,
    "to_all_pps",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Indicate that all PPs are to be operated on.",
    "Example:\r\n"
    "  fmf debug connect_pp_to_pp from_all_pps to_all_pps use_flow off\r\n"
    "Results in connecting all registered PPs to all registered PPs\r\n"
    "using \'destination labeling\'.\r\n",
    "",
    {
      BIT(10),
      0
    },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_TO_ONE_PP_ID,
    "to_one_pp",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Indicate destination PP, to operate on, follows.",
    "Example:\r\n"
    "  fmf debug connect_pp_to_pp from_one_pp pp_id 11 to_one_pp pp_id 17\r\n"
    "  use_flow off\r\n"
    "Results in connecting the PP with id \'11\' to the PP with id \'17\'\r\n"
    "using \'destination labeling\'.\r\n",
    "",
    {
      BIT(9),
      0
    },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_PP_ID_ID,
    "pp_id",
    (PARAM_VAL_RULES *)&Fmf_pp_id_vals[0],
    (sizeof(Fmf_pp_id_vals) / sizeof(Fmf_pp_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Id of Packet Processor to operate on.",
    "Example:\r\n"
    "  fmf debug activate_device one_specific_device unit 35\r\n"
    "Results in activating FAP device with chip id 11\r\n",
    "",
    {
      BIT(7) | BIT(9) | BIT(10),
      0
    },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_FROM_ALL_PPS_ID,
    "from_all_pps",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Indicate that all PPs are to be operated on.",
    "Example:\r\n"
    "  fmf debug connect_pp_to_pp from_all_pps to_all_pps use_flow off\r\n"
    "Results in connecting all registered PPs to all registered PPs\r\n"
    "using \'destination labeling\'.\r\n",
    "",
    {
      BIT(8) | BIT(9) | BIT(10),
      0
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_FROM_ONE_PP_ID,
    "from_one_pp",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Indicate source PP, to operate on, follows.",
    "Example:\r\n"
    "  fmf debug connect_pp_to_pp from_one_pp pp_id 11 to_all_pps use_flow off\r\n"
    "Results in connecting the PP with id \'11\' to all other registered PPs\r\n"
    "using \'destination labeling\'.\r\n",
    "",
    {
      BIT(7) | BIT(9) | BIT(10),
      0
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_CONNECT_PP_TO_PP_ID,
    "connect_pp_to_pp",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set user flows from a group of PPs to a group of PPs.",
    "Example:\r\n"
    "  fmf debug connect_pp_to_pp from_one_pp pp_id 11 to_all_pps use_flow off\r\n"
    "Results in connecting the PP with id \'11\' to all other registered PPs\r\n"
    "using \'destination labeling\'.\r\n",
    "",
    {
      BIT(7) | BIT(8) | BIT(9) | BIT(10),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_ALL_SPECIFIED_DEVICES_ID,
    "all_specified_devices",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Indicate that all registered devices, of specified type, are to be operated on.",
    "Example:\r\n"
    "  fmf debug activate_device all_specified_devices device_type fap_type\r\n"
    "  device_sub_type fap10m_type\r\n"
    "Results in activating all registered SOC_SAND_FAP10M devices\r\n",
    "",
    {
      BIT(6),
      0
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_ONE_SPECIFIC_DEVICE_ID,
    "one_specific_device",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Specify one registered device to operate on.",
    "Example:\r\n"
    "  fmf debug init_base_params one_specific_device unit 35\r\n"
    "Results in loading default base parameters (See \'FAP10M_BASE_PARAMS\')\r\n"
    "into FAP device with chip id 11\r\n",
    "",
    {
      BIT(5),
      0
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_INIT_BASE_PARAMS_ID,
    "init_base_params",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Initialize base parameters on a specific registered device or on all registered.",
    "Example:\r\n"
    "  fmf debug init_base_params one_specific_device unit 35\r\n"
    "Results in loading default base parameters (See \'FAP10M_BASE_PARAMS\'\r\n"
    "into FAP device with chip id 11\r\n",
    "",
    {
      BIT(5) | BIT(6),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_DEBUG_FLAGS_ID,
    "debug_flags",
    (PARAM_VAL_RULES *)&Fmf_debug_flags_vals[0],
    (sizeof(Fmf_debug_flags_vals) / sizeof(Fmf_debug_flags_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Control (activate/deactivate) debug flags within FMF system.",
    "Example:\r\n"
    "  fmf debug debug_flags fmf_debug_on\r\n"
    "Results in activating debug flag within FMF (will produce\r\n"
    "debug printouts)\r\n",
    "",
    {
      BIT(4),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_DEVICE_ID_ID,
    "unit",
    (PARAM_VAL_RULES *)&Fmf_unit_vals[0],
    (sizeof(Fmf_unit_vals) / sizeof(Fmf_unit_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Device id of device on unit to access.",
    "Example:\r\n"
    "  fmf debug activate_device one_specific_device unit 35\r\n"
    "Results in activating FAP device with chip id 11\r\n",
    "",
    {
      BIT(3) | BIT(5),
      0
    },
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_ACTIVATE_DEVICE_ID,
    "activate_device",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Activate device within FMF system.",
    "Example:\r\n"
    "  fmf debug activate_device one_specific_device unit 35\r\n"
    "Results in activating FAP device with chip id 11\r\n",
    "",
    {
      BIT(3) | BIT(5) | BIT(6),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_DEVICE_TYPE_ID,
    "device_type",
    (PARAM_VAL_RULES *)&Fmf_device_type_vals[0],
    (sizeof(Fmf_device_type_vals) / sizeof(Fmf_device_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "General type of device on unit to access (FAP or FE).",
    "Example:\r\n"
    "  fmf debug add_device unit_ip_address 10.0.0.5 chip_id 55 device_type fap_type\r\n"
    "  device_sub_type 1\r\n"
    "Results in adding FAP device of the SOC_SAND_FAP10M type to FMF using IP address\r\n"
    "10.0.0.5 and chip_id 55.\r\n"
    "The latter identifiers are used to access the board carrying specified\r\n"
    "device and to access the device itself on that board.\r\n",
    "",
    {
      BIT(1) | BIT(6),
      0
    },
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_DEVICE_SUB_TYPE_ID,
    "device_sub_type",
    (PARAM_VAL_RULES *)&Fmf_device_subtype_vals[0],
    (sizeof(Fmf_device_subtype_vals) / sizeof(Fmf_device_subtype_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Specific type of device on unit to access (SOC_SAND_FE200 as FE13, SOC_SAND_FAP10M, etc.).",
    "Example:\r\n"
    "  fmf debug add_device unit_ip_address 10.0.0.5 chip_id 55 device_type fap_type\r\n"
    "  device_sub_type fap10m_subtype\r\n"
    "Results in adding FAP device of the SOC_SAND_FAP10M type to FMF using IP address\r\n"
    "10.0.0.5 and chip_id 55.\r\n"
    "The latter identifiers are used to access the board carrying specified\r\n"
    "device and to access the device itself on that board.\r\n",
    "",
    {
      BIT(1) | BIT(6),
      0
    },
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_DEVICE_CHIP_ID_ID,
    "chip_id",
    (PARAM_VAL_RULES *)&Fmf_chip_id_vals[0],
    (sizeof(Fmf_chip_id_vals) / sizeof(Fmf_chip_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Chip id of device on unit to access.",
    "Example:\r\n"
    "  fmf debug add_device unit_ip_address 10.0.0.5 chip_id 55 device_type 3\r\n"
    "  device_sub_type 1\r\n"
    "Results in adding FAP device of the SOC_SAND_FAP10M type to FMF using IP address\r\n"
    "10.0.0.5 and chip_id 55.\r\n"
    "The latter identifiers are used to access the board carrying specified\r\n"
    "device and to access the device itself on that board.\r\n",
    "",
    {
      BIT(1),
      0
    },
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_UNIT_IP_ADDRESS_ID,
    "unit_ip_address",
    (PARAM_VAL_RULES *)&Fmf_ip_address_vals[0],
    (sizeof(Fmf_ip_address_vals) / sizeof(Fmf_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "IP address of unit to access.",
    "Example:\r\n"
    "  fmf debug add_device unit_ip_address 10.0.0.5 chip_id 55 device_type 3\r\n"
    "  device_sub_type 1\r\n"
    "Results in adding FAP device of the SOC_SAND_FAP10M type to FMF using IP address\r\n"
    "10.0.0.5 and chip_id 55.\r\n"
    "The latter identifiers are used to access the board carrying specified\r\n"
    "device and to access the device itself on that board.\r\n",
    "",
    {
      BIT(1),
      0
    },
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_ADD_DEVICE_ID,
    "add_device",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Add device to FMF system.",
    "Example:\r\n"
    "  fmf debug add_device unit_ip_address 10.0.0.5 chip_id 55 device_type 3\r\n"
    "  device_sub_type 1\r\n"
    "Results in adding FAP device of the SOC_SAND_FAP10M type to FMF using IP address\r\n"
    "10.0.0.5 and chip_id 55.\r\n"
    "The latter identifiers are used to access the board carrying specified\r\n"
    "device and to access the device itself on that board.\r\n",
    "",
    {
      BIT(1),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_REGISTER_DRIVER_ID,
    "register_driver",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Register FMF and activate it.",
    "Example:\r\n"
    "  fmf debug register_driver\r\n"
    "Results in registering FMF and starting it. This also includes initialization\r\n"
    "of SOC_SAND and activation of TCM task\r\n",
    "",
    {
      BIT(0),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_UNREGISTER_DRIVER_ID,
    "unregister_driver",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Register FMF and activate it.",
    "Example:\r\n"
    "  fmf debug unregister_driver\r\n"
    "Results in unregistering FMF and starting it. This does NOT include SOC_SAND\r\n"
    "operations (deactivation or TCM task handling)\r\n",
    "",
    {
      BIT(0),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_FORMAT_ID,
    "format",
    (PARAM_VAL_RULES *)&Fmf_format_vals[0],
    (sizeof(Fmf_format_vals) / sizeof(Fmf_format_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fmf_format_default,
    0,0,0,0,0,0,0,
    "Select display format (detailed or brief) for specified operation.",
    "Example:\r\n"
    "  fmf debug show unit 0 format short\r\n"
    "Results in displaying information on specified unit in short\r\n"
    "(i.e., not detailed) format.\r\n",
    "",
    {
      BIT(2),
      0
    },
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_SHOW_ALL_ID,
    "all",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select ALL FMF objects for specified operation.",
    "Example:\r\n"
    "  fmf debug show all format short\r\n"
    "Results in displaying all FMF objects (units, devices, ...) in short\r\n"
    "(i.e., not detailed) format.\r\n",
    "",
    {
      BIT(2),
      0
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_SHOW_UNIT_ID,
    "unit",
    (PARAM_VAL_RULES *)&Fmf_unit_vals[0],
    (sizeof(Fmf_unit_vals) / sizeof(Fmf_unit_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select indicated unit for specified operation.",
    "Example:\r\n"
    "  fmf debug show unit 0 format short\r\n"
    "Results in displaying information on specified unit in short\r\n"
    "(i.e., not detailed) format.\r\n",
    "",
    {
      BIT(2),
      0
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_SHOW_ID,
    "show",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Show details of specified FMF object.",
    "Example:\r\n"
    "  fmf debug show all format short\r\n"
    "Results in displaying all FMF objects (units, devices, ...) in short\r\n"
    "(i.e., not detailed) format.\r\n",
    "",
    {
      BIT(2),
      0
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FMF_DEBUG_ID,
    "debug",
    (PARAM_VAL_RULES *)&Fmf_empty_vals[0],
    (sizeof(Fmf_empty_vals) / sizeof(Fmf_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Invoke various debug tools for FMF.",
    "Example:\r\n"
    "  fmf debug register_driver\r\n"
    "Results in registering FMF and starting it. This also includes initialization\r\n"
    "of SOC_SAND and activation of TCM task\r\n",
    "",
    {
      BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) |
      BIT(5) | BIT(6) | BIT(7) | BIT(8) | BIT(9) |
      BIT(10),
      0
    },
    1,
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
