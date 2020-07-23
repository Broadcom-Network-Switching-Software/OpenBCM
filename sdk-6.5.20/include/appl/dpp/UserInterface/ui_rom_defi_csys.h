/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_CSYS_INCLUDED
/* { */
#define UI_ROM_DEFI_CSYS_INCLUDED
/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/ref_sys.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>

#include <ui_pure_defi_csys.h>

#include <CSystem/csystem_sand_mem.h>
#include <Serdes/serdes_screening.h>

/*
 * UI definition{
 */

/********************************************************
*NAME
*  Csys_empty_vals
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
     Csys_empty_vals[]
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
*  Csys_free_vals
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
     Csys_free_vals[]
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
*  Csyse_fap_id_vals
*TYPE: BUFFER
*DATE: 10/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP_LINK" OF SUBJECT
*  'GENERAL DEMO'.
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
     Csyse_fap_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_MAX_NOF_FAP_IN_SYSTEM - 1,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
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
     Csyse_link_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_FABRIC_LINKS - 1,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
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
     Csyse_fe_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_MAX_NOF_FE_IN_SYSTEM - 1,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
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
*  Csys_file_name_vals
*TYPE: BUFFER
*DATE: 31/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_FILE_NAME" IN THE
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
     Csys_file_name_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DLD_FILE_NAME_LEN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Name of application file on downloading host.",
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
   PARAM_VAL
     Csys_1_value_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  1,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;

/*
 */
EXTERN CONST
   PARAM_VAL
     Csys_0_value_default
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
/*
 */
EXTERN CONST
   PARAM_VAL_RULES
     Csys_mem_value_vals[]
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
        CSYS_MEM_VALUE_NOF_ENTRIES,
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
*  Csyse_get_connectivity_map_flavor
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
     Csys_direction_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "fe_to_fap",
    {
      {
        SERDES_RX_TX_OP_RX_ONLY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    FE send the PRBS, and FAP receive.",
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
    "fap_to_fe",
    {
      {
        SERDES_RX_TX_OP_TX_ONLY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    FAP send the PRBS, and FE receive.",
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
        SERDES_RX_TX_OP_BOTH,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Both FAP and FE send PRBS, and both receive.",
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

EXTERN CONST
   PARAM_VAL_RULES
     Csyse_get_connectivity_map_flavor[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "print_all_links",
    {
      {
        FALSE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Print Connectivity Map of all the links in the system / FAP.",
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
    "print_all_faulty_links",
    {
      {
       TRUE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Print Connectivity Map of all the faulty links in the system / FAP.",
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
*  Csyse_get_counters_flavor
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
     Csyse_get_counters_flavor[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "long",
    {
      {
        FALSE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Print all the registers.",
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
       TRUE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Will not print registers with value ZERO.",
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
*  Csys_params
*TYPE: BUFFER
*DATE: 29/Dec/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "TGS".
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
     Csys_params[]
#ifdef INIT
   =
/*
 ******
 * ENTRY -- 0
 *  BIT(0) - MEM: FAP READ
 *  BIT(1)        FAP WRITE
 *  BIT(2)        FE  READ
 *  BIT(3)        FE  WRITE
 *
 ******
 * ENTRY -- 1
 *  BIT(0 1 2 3) - SerDes
 */
{
  {
    PARAM_CSYS_MNGT_ID,
    "management",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Chassis management utilities.",
    "Example:\r\n"
    "  csys management net status\r\n"
    "Print the knowen network Chassis status.\r\n",
    "",
    {BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_MNGT_NET_ID,
    "net",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Chassis management NET utilities.",
    "Example:\r\n"
    "  csys management net status\r\n"
    "Print the knowen network Chassis status.\r\n",
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
    PARAM_CSYS_MNGT_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display the .",
    "Example:\r\n"
    "  csys management net status\r\n"
    "Print the knowen network Chassis status.\r\n",
    "",
    {BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_MEM_ID,
    "mem",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MEM utilities.",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_faps\r\n"
    "Results in reading memory offset 0x0 in all the FAPs in the Chassis.\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MEM utilities.",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_faps\r\n"
    "Results in reading memory offset 0x0 in all the FAPs in the Chassis.\r\n",
    "",
    {BIT(0) | BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_ALL_FAPS_ID,
    "all_faps",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MEM utilities.",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_faps\r\n"
    "Results in reading memory offset 0x0 in all the FAPs in the Chassis.\r\n",
    "",
    {BIT(0) | BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_FAP_ID_ID,
    "fap_id",
    (PARAM_VAL_RULES *)&Csyse_fap_id_vals[0],
    (sizeof(Csyse_fap_id_vals) / sizeof(Csyse_fap_id_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP-Id. 0 to 15",
    "Example:\r\n"
    "  csys mem read offset 0x0 fap_id 0\r\n"
    "Results in reading memory offset 0x0 in all FAP 0.\r\n",
    "",
    {BIT(0) | BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_ALL_FES_ID,
    "all_fes",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MEM utilities.",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_fes\r\n"
    "Results in reading memory offset 0x0 in all the FEs in the Chassis.\r\n",
    "",
    {BIT(2) | BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_FE_ID_ID,
    "fe_id",
    (PARAM_VAL_RULES *)&Csyse_fe_id_vals[0],
    (sizeof(Csyse_fe_id_vals) / sizeof(Csyse_fe_id_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FE-Id. 0 to 12",
    "Example:\r\n"
    "  csys mem read offset 0x0 fe_id 0\r\n"
    "Results in reading memory offset 0x0 in FE 0.\r\n",
    "",
    {BIT(2) | BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Csys_free_vals[0],
    (sizeof(Csys_free_vals) / sizeof(Csys_free_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Csys_0_value_default,
    0,0,0,0,0,0,0,
    "MEM utilities.",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_faps\r\n"
    "Results in reading memory offset 0x0 in all the FAPs in the Chassis.\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SIZE_ID,
    "nof_longs",
    (PARAM_VAL_RULES *)&Csys_free_vals[0],
    (sizeof(Csys_free_vals) / sizeof(Csys_free_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Csys_1_value_default,
    0,0,0,0,0,0,0,
    "Number of LONGs.",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_faps nof_longs 2\r\n"
    "Results in reading memory offset 0x0 in all the FAPs in the Chassis.\r\n"
    "  2 LONG will be read.\n\r",
    "",
    {BIT(0) | BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Silent Indicator.",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_faps verbose\r\n"
    "Results in reading memory offset 0x0 in all the FAPs in the Chassis.\r\n"
    "  Silent indicates to print minimal as possible.",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_ADDRESS_IN_LONGS_ID,
    "address_in_longs",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Address in long Indicator (SOC_SAND_FAP20V DS).",
    "Example:\r\n"
    "  csys mem read offset 0x0 all_faps verbose address_in_longs\r\n"
    "Results in reading memory offset 0x0 in all the FAPs in the Chassis.\r\n"
    "  Silent indicates to print minimal as possible.\n\r"
    "  Address in LONG will be used. Offsets as 0x1 are OK.",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MEM utilities.",
    "Example:\r\n"
    "  csys mem write offset 0x0 all_faps data 0x1234\r\n"
    "Results in writing memory offset 0x0 in all the FAPs in the Chassis,\r\n"
    "  value of 0x1234",
    "",
    {BIT(1) | BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Csys_mem_value_vals[0],
    (sizeof(Csys_mem_value_vals) / sizeof(Csys_mem_value_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of write data to the specified.\r\n",
    "Examples:\r\n"
    "  csys mem write offset 0x0 all_faps data 0x1234\r\n"
    "Results in writing memory offset 0x0 in all the FAPs in the Chassis,\r\n"
    "  value of 0x1234",
    "",
    {BIT(1)  | BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SERDES_ID,
    "serdes",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "SerDes utilities.",
    "Example:\r\n"
    "  csys serdes prbs_test_all_fap_links\r\n"
    "Results in PRBS testing of all FAP links in the system (FAP is TX)\r\n",
    "",
    {0,
     BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SERDES_LOAD_PARAMS_FAP20V_ID,
    "load_params_fap20v",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load SOC_SAND_FAP20V serdes parameters from file.",
    "Examples:\r\n"
    "  csys serdes load_params_fap20v download_file_name ver_0127\r\n"
    "Results loading SOC_SAND_FAP20V serdes parameters from file 'ver_0127'.\r\n",
    "",
    {0,
      BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SERDES_LOAD_PARAMS_FE200_ID,
    "load_params_fe200",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load SOC_SAND_FE200 serdes parameters from file.",
    "Examples:\r\n"
    "  csys serdes load_params_fe200 download_file_name ver_0127\r\n"
    "Results loading SOC_SAND_FE200 serdes parameters from file 'ver_0127'.\r\n",
    "",
    {0,
      BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_FILE_NAME_ID,
    "download_file_name",
    (PARAM_VAL_RULES *)&Csys_file_name_vals[0],
    (sizeof(Csys_file_name_vals) / sizeof(Csys_file_name_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'download_file_name\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  csys serdes load_params_fe200 download_file_name ver_0127\r\n"
    "Results loading SOC_SAND_FE200 serdes parameters from file 'ver_0127'.\r\n",
    "",
    {0,
      BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SERDES_FE_LINKS_ID,
    "fe_get_links_status",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get and display FEs links status in all the system.",
    "Example:\r\n"
    "  csys serdes fe_get_links_status\r\n"
    "Results in display of all FEs links status in all the system\r\n",
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
    PARAM_CSYS_SCREENING_ID,
    "screening",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This option will do the PRBS test for screening setup.",
    "Example:\r\n"
    "  csys serdes screening\r\n"
    "Results in running the screening process\r\n",
    "",
    {0,
     BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SCREENING_TIME_ID,
    "time",
    (PARAM_VAL_RULES *)&Csys_free_vals[0],
    (sizeof(Csys_free_vals) / sizeof(Csys_free_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Time in seconds, for the PRBS stream sending.",
    "Example:\r\n"
    "  csys serdes screening time 20\r\n"
    "Results in running the PRBS stream for 20 sec each iteration\r\n"
    "and 80 seconds total\n\r",
    "",
    {0,
     BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SCREENING_PRNT_LVL_ID,
    "print_level",
    (PARAM_VAL_RULES *)&Csys_free_vals[0],
    (sizeof(Csys_free_vals) / sizeof(Csys_free_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Printing level 0-3.",
    "Example:\r\n"
    "  csys serdes screening time 20 print_level 3\r\n"
    "Results in running the PRBS stream for 20 sec each iteration\r\n"
    " and 80 seconds total, and the maximum detailed printing.\n\r",
    "",
    {0,
     BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SCREENING_BM_ID,
    "bitmap",
    (PARAM_VAL_RULES *)&Csys_free_vals[0],
    (sizeof(Csys_free_vals) / sizeof(Csys_free_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Groups BitMap 0x0 - 0xF.",
    "Example:\r\n"
    "  csys serdes screening time 20 print_level 3\r\n"
    "Results in running the PRBS stream for 20 sec each iteration\r\n"
    " and 80 seconds total, and the maximum detailed printing.\n\r",
    "",
    {0,
     BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SCREENING_RX_TX_ID,
    "direction",
    (PARAM_VAL_RULES *)&Csys_direction_vals[0],
    (sizeof(Csys_direction_vals) / sizeof(Csys_direction_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Should the FAP recieve PRBS, send PRBS or both.",
    "Example:\r\n"
    "  csys serdes screening direction both\r\n"
    "Results in running the PRBS stream from the FAP to the FE,\n\r"
    " and a stream from the FE to the FAP at the same time,\n\r",
    "",
    {0,
     BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SERDES_FAP_LINKS_ID,
    "fap_get_links_status",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get and display FAPs links status in all the system.",
    "Example:\r\n"
    "  csys serdes fap_get_links_status\r\n"
    "Results in display of all FAPs links status in all the system\r\n",
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
    PARAM_CSYS_SERDES_GET_FE_BERS_ID,
    "get_fe_bers",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get FE RX BER (PRBS) counter.",
    "Example:\r\n"
    "  csys serdes get_fe_bers fap_id 0\r\n"
    "Results in getting the BER counters from all the \n\r"
    "  FEs connected to FAP 0\r\n",
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
    PARAM_CSYS_SERDES_GET_FAP_BERS_ID,
    "get_fap_bers",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get FAP RX BER (PRBS) counter.",
    "Example:\r\n"
    "  csys serdes get_fap_bers fap_id 0\r\n"
    "Results in getting the BER counters from FAP 0\r\n",
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
    PARAM_CSYS_SERDES_PRBS_FAP_TX_LINKS_ID,
    "prbs_fap_tx_links",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "SerDes utilities.",
    "Example:\r\n"
    "  csys serdes prbs_fap_tx_links start fap_id 0\r\n"
    "Results in PRBS testing on FAP 10 links in the system (FAP is TX)\r\n",
    "",
    {0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_SERDES_PRBS_FE_TX_LINKS_ID,
    "prbs_fe_tx_links",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "SerDes utilities.",
    "Example:\r\n"
    "  csys serdes prbs_fe_tx_links start fap_id 0\r\n"
    "Results in PRBS testing on FAP 10 links in the system (FE is TX)\r\n",
    "",
    {0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_FAP_ID_ID,
    "fap_id",
    (PARAM_VAL_RULES *)&Csyse_fap_id_vals[0],
    (sizeof(Csyse_fap_id_vals) / sizeof(Csyse_fap_id_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP-Id. 0 to 15",
    "Example:\r\n"
    "  csys serdes prbs_fap_tx_links start fap_id 0\r\n"
    "Results in PRBS testing on FAP 10 links in the system (FAP is TX)\r\n",
    "",
    {0,
     BIT(0) | BIT(2)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_FAP_LINK_ID_ID,
    "fap_link_id",
    (PARAM_VAL_RULES *)&Csyse_link_id_vals[0],
    (sizeof(Csyse_link_id_vals) / sizeof(Csyse_link_id_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP Link-Id. (Only needed in SOC_SAND_FAP10M systems)",
    "Example:\r\n"
    "  csys serdes prbs_fe_tx_links start fe_id 0 fap_link_id 3\r\n"
    "Results in PRBS testing on FE 0 fap link 3 (FAP is RX)\r\n",
    "",
    {0,
     BIT(0) | BIT(2)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_START_ID,
    "start",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "SerDes utilities.",
    "Example:\r\n"
    "  csys serdes prbs_fap_tx_links start fap_id 0\r\n"
    "Results in PRBS testing on FAP 10 links in the system (FAP is TX)\r\n",
    "",
    {0,
     BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_END_ID,
    "end",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "SerDes utilities.",
    "Example:\r\n"
    "  csys serdes prbs_fap_tx_links end fap_id 0\r\n"
    "Results in PRBS stoping on FAP 10 links in the system (FAP is TX)\r\n",
    "",
    {0,
     BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "STATUS utilities.",
    "Example:\r\n"
    "  csys status all_faps get_counters short\r\n"
    "Results in reading all the counters in short format.\r\n"
    "The PP / NP registers might also be printed.",
    "",
    {BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(14) | BIT(15)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_ALL_FAPS_ID,
    "all_faps",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "STATUS utilities.",
    "Example:\r\n"
    "  csys status all_faps get_counters short\r\n"
    "Results in reading all the counters in short format.",
    "",
    {BIT(10) | BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_ALL_PPS_ID,
    "all_pps",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "STATUS utilities.",
    "Example:\r\n"
    "  csys status all_pps get_counters short\r\n"
    "Results in reading all the counters in short format.\r\n"
    "The counters to be read are specific to the Packet Processors\n\r"
    "or Network Processors in the system",
    "",
    {BIT(14) | BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_PP_ID_ID,
    "pp_id",
    (PARAM_VAL_RULES *)&Csyse_fap_id_vals[0],
    (sizeof(Csyse_fap_id_vals) / sizeof(Csyse_fap_id_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "STATUS utilities.",
    "Example:\r\n"
    "  csys status all_pps get_counters short\r\n"
    "Results in reading all the counters in short format.\r\n"
    "The counters to be read are specific to the Packet Processors\n\r"
    "or Network Processors in the system",
    "",
    {BIT(14) | BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_FAP_ID_ID,
    "fap_id",
    (PARAM_VAL_RULES *)&Csyse_fap_id_vals[0],
    (sizeof(Csyse_fap_id_vals) / sizeof(Csyse_fap_id_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "STATUS utilities.",
    "Example:\r\n"
    "  csys status all_faps get_counters short\r\n"
    "Results in reading all the counters in short format.\r\n"
    "The PP / NP registers might also be printed.",
    "",
    {BIT(10) | BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_ALL_FES_ID,
    "all_fes",
    (PARAM_VAL_RULES *)&Csys_empty_vals[0],
    (sizeof(Csys_empty_vals) / sizeof(Csys_empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "STATUS utilities.",
    "Example:\r\n"
    "  csys status all_faps get_counters short\r\n"
    "Results in reading all the counters in short format.\r\n"
    "The PP / NP registers might also be printed.",
    "",
    {BIT(12) | BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_FE_ID_ID,
    "fe_id",
    (PARAM_VAL_RULES *)&Csyse_fe_id_vals[0],
    (sizeof(Csyse_fe_id_vals) / sizeof(Csyse_fe_id_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "STATUS utilities.",
    "Example:\r\n"
    "  csys status all_faps get_counters short\r\n"
    "Results in reading all the counters in short format.\r\n"
    "The PP / NP registers might also be printed.",
    "",
    {BIT(12) | BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_GET_COUNTERS_ID,
    "get_counters",
    (PARAM_VAL_RULES *)&Csyse_get_counters_flavor[0],
    (sizeof(Csyse_get_counters_flavor) / sizeof(Csyse_get_counters_flavor[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get all the counters in the system according to flavor.",
    "Example:\r\n"
    "  csys status all_faps get_counters short.\r\n"
    "When running with SOC_SAND_FAP10M system,\n\r"
    "the short/long flag indicate the following:\r\n"
    "LONG: all the FAP counters and all the PP counters\r\n"
    "  are printed.\r\n"
    "SHORT: Only non-zero counters are printed.",
    "",
    {BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(14) | BIT(15)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_CSYS_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Csyse_get_connectivity_map_flavor[0],
    (sizeof(Csyse_get_connectivity_map_flavor) / sizeof(Csyse_get_connectivity_map_flavor[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print System Connectivity Map according to flavor.",
    "Example:\r\n"
    "  csys status all_faps get_connectivity_map print_all_links.\r\n"
    "Will print the connectivity map of all the FAPs in the system.",
    "",
    {BIT(10) | BIT(11) | BIT(12) | BIT(13)},
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
}
#endif
;

/*
 * }
 */


/* } */
#endif


