/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_NEGEV_DEMO_INCLUDED
/* { */
#define UI_ROM_DEFI_NEGEV_DEMO_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>

/*
 * UI definition{
 */
/********************************************************
*NAME
*  Negev_demo_empty_vals
*TYPE: BUFFER
*DATE: 10/AUG/2003
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
     Negev_demo_empty_vals[]
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
*  Negev_demo_prbs_test_time_period_default
*TYPE: BUFFER
*DATE: 02/MAR/2004
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "TIME_PERIOD" IN THE CONTEXT OF SUBJECT
*  'NEGEV_DEMO' PLUS 'PRBS_TEST'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent
*      Numeric value related to string_value. Only relevant
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
     Negev_demo_prbs_test_time_period_default
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
*  Negev_demo_scheduling_scheme_vals
*TYPE: BUFFER
*DATE: 25/MARCH/2004
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
*    int numeric_equivalent
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
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
     Negev_demo_scheduling_scheme_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "default",
    {
      {
        FAP10M_SCHEME_DEFAULT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Default scheduling scheme.",
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
    "flow_base_weighted",
    {
      {
        FAP10M_SCHEME_FLOW_BASE_WEIGHTED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    flow_base weighted.",
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
    "flow_base_rate_then_weight",
    {
      {
        FAP10M_SCHEME_FLOW_BASE_RATE_THEN_WEIGHT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    2 strict priorities. Then 4/6 flow_base weighted.",
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
    "four_strict_then_weghit",
    {
      {
        FAP10M_SCHEME_STRICT_4,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    first 4 priorities are strict, then 4 flow_base weighted.",
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
    "eight_strict",
    {
      {
        FAP10M_SCHEME_STRICT_8,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    8 strict priorities.",
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
    "enhanced_strict",
    {
      {
        FAP10M_SCHEME_ENHANCED_STRICT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    4 enhanced strict priorities.",
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
    "class_base_rated",
    {
      {
        FAP10M_SCHEME_CLASS_BASE_RATE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    4/8 class_base rated priorities.",
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
    "class_base_weighted",
    {
      {
        FAP10M_SCHEME_CLASS_BASE_WEIGHTED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    4/8 class_base weighted priorities.",
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
    "class_base_rate_and_weight",
    {
      {
        FAP10M_SCHEME_CLASS_BASE_RATE_AND_WEIGHT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    4/8 classes. Each get class_base based rate and weight.",
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
    "class_base_rate_then_weight",
    {
      {
        FAP10M_SCHEME_CLASS_BASE_RATE_THEN_WEIGHT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    2 strict priorities. Then 4/6 class_base weighted.",
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
    "diff_serv",
    {
      {
        FAP10M_SCHEME_CLASS_DIFF_SERV,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    1 Strict priority, 4 Allocated rate, 1 best effort with 6% allocated rate.",
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
    "hybrid",
    {
      {
        FAP10M_SCHEME_HYBRID,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Flow control for each class.",
          /*
           * Using special mode.
           * The packets flow control arrives from the PP
           *  directly to the aggregate flow that serve the class.
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
*  Negev_demo_prbs_test_time_period_vals
*TYPE: BUFFER
*DATE: 02/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TIME_PERIOD" WITHIN 'PRBS_TEST'
*  IN SUBJECT 'NEGEV_DEMO'.
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
     Negev_demo_flow_number[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_FLOWS-1,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
     Negev_demo_enhanced_rates[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_prbs_test_force_all_links_vals
*TYPE: BUFFER
*DATE: 25/MARCH/2004
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
*    int numeric_equivalent
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
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
     Negev_demo_prbs_test_force_all_links_vals[]
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
        (long)"    Force test on all links connected between FAP and FE\r\n"
              "    regardless on whether the are \'accepting cells\' (=\'up\') or not.",
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
        (long)"    Only test links connected between FAP and FE which are\r\n"
              "    \'accepting cells\' (=\'up\').",
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
*  Negev_demo_prbs_test_force_all_links_default
*TYPE: BUFFER
*DATE: 25/MAR/2004
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "FORCE_ALL_LINKS" IN THE CONTEXT OF SUBJECT 'NEGEV_DEMO'
*  (AND 'PRBS_TEST').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to string_value. Only relevant
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
     Negev_demo_prbs_test_force_all_links_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  OFF_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"off"
  }
}
#endif
;
/********************************************************
*NAME
*  Negev_demo_prbs_test_time_period_vals
*TYPE: BUFFER
*DATE: 02/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TIME_PERIOD" WITHIN 'PRBS_TEST'
*  IN SUBJECT 'NEGEV_DEMO'.
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
     Negev_demo_prbs_test_time_period_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        20,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_fap_no_vals
*TYPE: BUFFER
*DATE: 10/AUG/2003
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
     Negev_demo_fap_no_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_MAX_NOF_FAP_IN_SYSTEM-1,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_fap_list_vals
*TYPE: BUFFER
*DATE: 03/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP_LIST" UNDER 'PRBS_TEST'.
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
     Negev_demo_fap_list_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_MAX_NOF_LINE_CARDS,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        CSYS_MAX_NOF_LINE_CARDS,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_fe_no_vals
*TYPE: BUFFER
*DATE: 10/AUG/2003
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
     Negev_demo_fe_no_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_MAX_NOF_FE_IN_SYSTEM-1,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_port_no_vals
*TYPE: BUFFER
*DATE: 10/AUG/2003
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
     Negev_demo_port_no_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        11,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_class_no_vals
*TYPE: BUFFER
*DATE: 10/AUG/2003
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
     Negev_demo_class_no_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        7,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_serdes_no_vals
*TYPE: BUFFER
*DATE: 10/AUG/2003
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
     Negev_demo_serdes_no_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        63,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*  Negev_demo_sub_flow_types
*TYPE: BUFFER
*DATE: 11/AUG/2003
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
     Negev_demo_sub_flow_types[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "edf0",
    {
      {
        FAP10M_EDF0_FLOW,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    EDF0 Wheel at the scheduler.",
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
    "edf1",
    {
      {
        FAP10M_EDF1_FLOW,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    EDF1 Wheel at the scheduler.",
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
    "wfq1",
    {
      {
        FAP10M_WFQ1_FLOW,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    WFQ1 Wheel at the scheduler.",
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
*  Negev_demo_sub_flow_values
*TYPE: BUFFER
*DATE: 11/AUG/2003
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
     Negev_demo_sub_flow_values[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        10000000,0,
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
*  Negev_demo_params
*TYPE: BUFFER
*DATE: 10/Aug/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "LINE_CARD".
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
     Negev_demo_params[]
#ifdef INIT
   =
{
  {
    PARAM_DEMO_MAC_ADDRESSES_GROUP_ID,
    "mac_addresses",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MAC address utilities.",
    "Examples:\r\n"
    "  negev_demo mac_addresses help\r\n"
    "Results in printing MAC addresses help information.",
    "",
    {BIT(18) | BIT(19) | BIT(20)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DEMO_MAC_ADDRESSES_HELP_ID,
    "help",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MAC address help information.",
    "Examples:\r\n"
    "  negev_demo mac_addresses help\r\n"
    "Results in printing MAC addresses help information.",
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
    PARAM_DEMO_MACS_ADD_UNTAGGED_ANY_TO_ANY,
    "add_untagged_any_to_any",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MAC address Add untagged any to any scheme.",
    "Examples:\r\n"
    "  negev_demo mac_addresses add_untagged_any_to_any\r\n"
    "Results in adding the MAC addresses that enable the \r\n"
    "  the untagged any to any scheme.",
    "",
    {BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DEMO_MACS_REMOVE_UNTAGGED_ANY_TO_ANY,
    "remove_untagged_any_to_any",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MAC address Remove untagged any to any scheme.",
    "Examples:\r\n"
    "  negev_demo mac_addresses remove_untagged_any_to_any\r\n"
    "Results in removing the MAC addresses that enable the \r\n"
    "  the untagged any to any scheme.",
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
    PARAM_NEGEV_DEMO_SHOW_FLOWS_ID,
    "show_flows",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Show flows first_flow - last_flow.",
    "Examples:\r\n"
    "  negev_demo show_flow first_flow 12 last_flow 150\r\n"
    "Results in printing flows 12 - 150.",
    "",
    {BIT(15)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SHOW_FLOWS_FIRST_FLOW_ID,
    "first_flow",
    (PARAM_VAL_RULES *)&Negev_demo_flow_number[0],
    (sizeof(Negev_demo_flow_number) / sizeof(Negev_demo_flow_number[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first flow to show.",
    "Examples:\r\n"
    "  negev_demo show_flow first_flow 12 last flow 150\r\n"
    "Results in printing flows 12 - 150.",
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
    PARAM_NEGEV_DEMO_SHOW_FLOWS_LAST_FLOW_ID,
    "last_flow",
    (PARAM_VAL_RULES *)&Negev_demo_flow_number[0],
    (sizeof(Negev_demo_flow_number) / sizeof(Negev_demo_flow_number[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The last flow to show.",
    "Examples:\r\n"
    "  negev_demo show_flow first_flow 12 last flow 150\r\n"
    "Results in printing flows 12 - 150.",
    "",
    {BIT(15)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SCHEDULING_SCHEME_SET_ID,
    "set_scheduling_scheme",
    (PARAM_VAL_RULES *)&Negev_demo_scheduling_scheme_vals[0],
    (sizeof(Negev_demo_scheduling_scheme_vals) / sizeof(Negev_demo_scheduling_scheme_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set scheduling scheme.",
    "Examples:\r\n"
    "  negev_demo set_scheduling_scheme eight_strict\r\n"
    "Results setting the scheduling scheme: eight strict priorities.",
    "",
    {BIT(16)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_ENHANCED_ALLOCATORS_ID,
    "update_enhanced_allocators",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Update the allocated rates in the enhanced priority scheduling scheme.",
    "Examples:\r\n"
    "  negev_demo update_enhanced_allocators prior_2 50 prior_3 100 prior_4 100\r\n"
    "Results in updating the allocated rate for priority 2 to 50\n\r"
    " and priorities 3 and for to 100.",
    "",
    {BIT(23)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_PRIOR_2_RATE_ID,
    "prior_2",
    (PARAM_VAL_RULES *)&Negev_demo_enhanced_rates[0],
    (sizeof(Negev_demo_enhanced_rates) / sizeof(Negev_demo_enhanced_rates[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the allocated rate for priority 2 users.",
    "Examples:\r\n"
    "  negev_demo update_enhanced_allocators prior_2 50 prior_3 100 prior_4 100\r\n"
    "Results in updating the allocated rate for priority 2 to 50\n\r"
    " and priorities 3 and for to 100.",
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
    PARAM_NEGEV_DEMO_PRIOR_3_RATE_ID,
    "prior_3",
    (PARAM_VAL_RULES *)&Negev_demo_enhanced_rates[0],
    (sizeof(Negev_demo_enhanced_rates) / sizeof(Negev_demo_enhanced_rates[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the allocated rate for priority 3 users.",
    "Examples:\r\n"
    "  negev_demo update_enhanced_allocators prior_2 50 prior_3 100 prior_4 100\r\n"
    "Results in updating the allocated rate for priority 2 to 50\n\r"
    " and priorities 3 and for to 100.",
    "",
    {BIT(23)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_PRIOR_4_RATE_ID,
    "prior_4",
    (PARAM_VAL_RULES *)&Negev_demo_enhanced_rates[0],
    (sizeof(Negev_demo_enhanced_rates) / sizeof(Negev_demo_enhanced_rates[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the allocated rate for priority 4 users.",
    "Examples:\r\n"
    "  negev_demo update_enhanced_allocators prior_2 50 prior_3 100 prior_4 100\r\n"
    "Results in updating the allocated rate for priority 2 to 50\n\r"
    " and priorities 3 and for to 100.",
    "",
    {BIT(23)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SCHEDULING_SCHEME_GET_ID,
    "get_scheduling_scheme",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get scheduling scheme.",
    "Examples:\r\n"
    "  negev_demo get_scheduling_scheme\r\n"
    "Results in printing the name of the scheduling scheme.",
    "",
    {BIT(17)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_GRACEFUL_SHUTDOWN_FE_ID,
    "graceful_shutdown_fe",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gracefully shuts down a single FE (by stopping reachability messages).",
    "Examples:\r\n"
    "  negev_demo graceful_shutdown_fe fe# 2\r\n"
    "Results in shutting down the second FE on the first fabric card\n\r"
    " (the lower card).",
    "",
    { BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_GRACEFUL_RESTORE_FE_ID,
    "graceful_restore_fe",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "restores a single registered FE (take out of reset + restart reachability messages).",
    "Examples:\r\n"
    "  negev_demo graceful_restore_fe fe# 2\r\n"
    "Results in restoring the second FE on the first fabric card\n\r"
    " (the lower card).",
    "",
    { BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the connectivity map of one of the FAPs.",
    "Examples:\r\n"
    "  negev_demo get_connectivity_map fap# 1\r\n"
    "Results in printing the connectivity map of the fap at the line card\n\r"
    " in slot #1 (the upper card).",
    "",
    { BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_POWER_DOWN_SERDES_ID,
    "power_down_serdes",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Powers down a single serdes of one of the FEs / FAPs.",
    "Examples:\r\n"
    "  negev_demo power_down_serdes fap# 2 serdes# 7\r\n"
    "Results in reseting the power of serdes #7 of the fap that is placed\n\r"
    " on the line card in slot #2 (the lower card).",
    "",
    { BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_POWER_UP_SERDES_ID,
    "power_up_serdes",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Powers up a single serdes of one of the FEs / FAPs.",
    "Examples:\r\n"
    "  negev_demo power_up_serdes fap# 2 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the fap that is placed\n\r"
    " on the line card in slot #2 (the lower card).",
    "",
    { BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_FAP_DEVICE_ID,
    "fap#",
    (PARAM_VAL_RULES *)&Negev_demo_fap_no_vals[0],
    (sizeof(Negev_demo_fap_no_vals) / sizeof(Negev_demo_fap_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the fap at get connectivity map / power up/down serdes commands.",
    "Examples:\r\n"
    "  negev_demo get_connectivity_map fap# 1\r\n"
    "Results in printing the connectivity map of the fap at the line card\n\r"
    "in slot #1 (upper card in Negev, leftmost card in Gobi)."
    "  negev_demo graceful_shutdown_fe fe# 2\r\n"
    "Results in shutting down the second FE on the first fabric card\n\r"
    "(the lower fabric card).",
    "",
    { BIT(1) | BIT(2) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_FE_DEVICE_ID,
    "fe#",
    (PARAM_VAL_RULES *)&Negev_demo_fe_no_vals[0],
    (sizeof(Negev_demo_fe_no_vals) / sizeof(Negev_demo_fe_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the fe for operation. E.g., at power up/down serdes commands.",
    "Examples:\r\n"
    "  negev_demo power_up_serdes fe# 2 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the second fe in the fabric card.",
    "",
    { BIT(2) | BIT(5) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SERDES_ID,
    "serdes#",
    (PARAM_VAL_RULES *)&Negev_demo_serdes_no_vals[0],
    (sizeof(Negev_demo_serdes_no_vals) / sizeof(Negev_demo_serdes_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the serdes at power up/down serdes commands.",
    "Examples:\r\n"
    "  negev_demo power_up_serdes fe# 2 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the second fe in the fabric card.",
    "",
    { BIT(2) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_GET_QOS_PARAMS_ID,
    "get_qos_params",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the Quality of Service parameters of a single flow in the system.",
    "Examples:\r\n"
    "  negev_demo get_qos_params source_fap# 1 source_port# 7 destination_fap# 2 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap#1 port 7 to fap#2 port 9 with class 7.",
    "",
    { BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SET_QOS_PARAMS_ID,
    "set_qos_params",
    (PARAM_VAL_RULES *)&Negev_demo_empty_vals[0],
    (sizeof(Negev_demo_empty_vals) / sizeof(Negev_demo_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the Quality of Service parameters of a single flow in the system.",
    "Examples:\r\n"
    "  negev_demo set_qos_params source_fap# 1 source_port# 7 destination_fap# 2 destination_port# 9 class# 7 ...\r\n"
    "Results in setting the quality of service params of the flow\r\n"
    " that starts from fap#1 port 7 to fap#2 port 9 with class 7 to ...",
    "",
    { BIT(4)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SOURCE_FAP_DEVICE_ID,
    "source_fap#",
    (PARAM_VAL_RULES *)&Negev_demo_fap_no_vals[0],
    (sizeof(Negev_demo_fap_no_vals) / sizeof(Negev_demo_fap_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the source fap at the get / set QOS command.",
    "Examples:\r\n"
    "  negev_demo get_QOS_params source_fap# 1 source_port# 7 destination_fap# 2 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap#1 port 7 to fap#2 port 9 with class 7.",
    "",
    { BIT(3) | BIT(4) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_DESTINATION_FAP_DEVICE_ID,
    "destination_fap#",
    (PARAM_VAL_RULES *)&Negev_demo_fap_no_vals[0],
    (sizeof(Negev_demo_fap_no_vals) / sizeof(Negev_demo_fap_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the destination fap at the get / set QOS command.",
    "Examples:\r\n"
    "  negev_demo get_QOS_params source_fap# 1 source_port# 7 destination_fap# 2 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap#1 port 7 to fap#2 port 9 with class 7.",
    "",
    { BIT(3) | BIT(4) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#if NEGEV_DEMO_QOS_USE_SRC_PORT
  {
    PARAM_NEGEV_DEMO_SOURCE_PORT_ID,
    "source_port#",
    (PARAM_VAL_RULES *)&Negev_demo_port_no_vals[0],
    (sizeof(Negev_demo_port_no_vals) / sizeof(Negev_demo_port_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the source port at the get / set QOS command.",
    "Examples:\r\n"
    "  negev_demo get_QOS_params source_fap# 1 source_port# 7 destination_fap# 2 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap#1 port 7 to fap#2 port 9 with class 7.",
    "",
    { BIT(3) | BIT(4) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
  {
    PARAM_NEGEV_DEMO_DESTINATION_PORT_ID,
    "destination_port#",
    (PARAM_VAL_RULES *)&Negev_demo_port_no_vals[0],
    (sizeof(Negev_demo_port_no_vals) / sizeof(Negev_demo_port_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the destination port at the get / set QOS command.",
    "Examples:\r\n"
    "  negev_demo get_QOS_params source_fap# 1 source_port# 7 destination_fap# 2 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap#1 port 7 to fap#2 port 9 with class 7.",
    "",
    { BIT(3) | BIT(4) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_DESTINATION_CLASS_ID,
    "class#",
    (PARAM_VAL_RULES *)&Negev_demo_class_no_vals[0],
    (sizeof(Negev_demo_class_no_vals) / sizeof(Negev_demo_class_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the class at the get / set QOS command.",
    "Examples:\r\n"
    "  negev_demo get_QOS_params source_fap# 1 source_port# 7 destination_fap# 2 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap#1 port 7 to fap#2 port 9 with class 7.",
    "",
    { BIT(3) | BIT(4) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SUB_FLOW_0_TYPE_ID,
    "sub_0_flow_type",
    (PARAM_VAL_RULES *)&Negev_demo_sub_flow_types[0],
    (sizeof(Negev_demo_sub_flow_types) / sizeof(Negev_demo_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The type of the the first sub flow.",
    "Examples:\r\n"
    "  negev_demo set_QOS_params source_fap# 1 source_port# 7 destination_fap# 2\r\n"
    "  destination_port# 9 class# 7 sub_0_flow_type wfq1 sub_0_flow_value 323\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 12  \r\n"
    "Resulting in updating the flow (on fap# 2 port# 9) to send credits to the\r\n"
    " queue on fap# 1. The flow will have 2 subflows.\n\r",
    "",
    {BIT(4)},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SUB_FLOW_0_VALUE_ID,
    "sub_0_flow_value",
    (PARAM_VAL_RULES *)&Negev_demo_sub_flow_values[0],
    (sizeof(Negev_demo_sub_flow_values) / sizeof(Negev_demo_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The value the first subflow will be configured too.\n\r"
    " 'value' semantics depends on the 'type' value:\n\r"
    "\n\r"
    " FAP10M_SUB_FLOW_TYPE    | 'value' semantics\n\r"
    "----------------------------------------------------------\n\r"
    " FAP10M_EDF0_FLOW        | Kilo-bits - Guaranteed rate\n\r"
    " FAP10M_EDF1_FLOW        | Kilo-bits - Maximal rate\n\r"
    " FAP10M_WFQ1_FLOW        | Weight    - Range from 0 to 127\n\r"
    "",
    "Examples:\r\n"
    "  negev_demo set_QOS_params source_fap# 1 source_port# 7 destination_fap# 2\r\n"
    "  destination_port# 9 class# 7 sub_0_flow_type wfq1 sub_0_flow_value 323\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 12  \r\n"
    "Resulting in updating the flow (on fap# 2 port# 9) to send credits to the\r\n"
    " queue on fap# 1. The flow will have 2 subflows.\n\r",
    "",
    {BIT(4)},
    9,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SUB_FLOW_1_TYPE_ID,
    "sub_1_flow_type",
    (PARAM_VAL_RULES *)&Negev_demo_sub_flow_types[0],
    (sizeof(Negev_demo_sub_flow_types) / sizeof(Negev_demo_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The type of the the second sub flow.",
    "Examples:\r\n"
    "  negev_demo set_QOS_params source_fap# 1 source_port# 7 destination_fap# 2\r\n"
    "  destination_port# 9 class# 7 sub_0_flow_type wfq1 sub_0_flow_value 323\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 12  \r\n"
    "Resulting in updating the flow (on fap# 2 port# 9) to send credits to the\r\n"
    " queue on fap# 1. The flow will have 2 subflows.\n\r",
    "",
    {BIT(4)},
    10,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NEGEV_DEMO_SUB_FLOW_1_VALUE_ID,
    "sub_1_flow_value",
    (PARAM_VAL_RULES *)&Negev_demo_sub_flow_values[0],
    (sizeof(Negev_demo_sub_flow_values) / sizeof(Negev_demo_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The value the second subflow will be configured too.\n\r"
    " 'value' semantics depends on the 'type' value:\n\r"
    "\n\r"
    " FAP10M_SUB_FLOW_TYPE    | 'value' semantics\n\r"
    "----------------------------------------------------------\n\r"
    " FAP10M_EDF0_FLOW        | Kilo-bits - Guaranteed rate\n\r"
    " FAP10M_EDF1_FLOW        | Kilo-bits - Maximal rate\n\r"
    " FAP10M_WFQ1_FLOW        | Weight    - Range from 0 to 127\n\r"
    "",
    "Examples:\r\n"
    "  negev_demo set_QOS_params source_fap# 1 source_port# 7 destination_fap# 2\r\n"
    "  destination_port# 9 class# 7 sub_0_flow_type wfq1 sub_0_flow_value 323\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 12  \r\n"
    "Resulting in updating the flow (on fap# 2 port# 9) to send credits to the\r\n"
    " queue on fap# 1. The flow will have 2 subflows.\n\r",
    "",
    {BIT(4)},
    11,
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




/*
 * }
 */
/* } */
#endif
