/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_PSS_INCLUDED
/* { */
#define UI_ROM_DEFI_PSS_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <ui_pure_defi_pss.h>
/* { */
#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>
/* } */


/*
 * UI definition{
 */
/********************************************************
*NAME
*  Pss_clear_after_read_vals
*TYPE: BUFFER
*DATE: 18/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PORT_UTILS" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'DISPLAY_ALL_MAC_COUNTERS').
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
     Pss_clear_after_read_vals[]
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
        (long)"    Clear counters after reading. Next reading starts from last read action",
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
        (long)"    Do not change counters after reading. Readings are accumulative\r\n"
              "    Note, however, that counting restarts when changing target port",
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
*  Pss_port_force_link_pass_vals
*TYPE: BUFFER
*DATE: 01/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "port_utils" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'ctrl').
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
     Pss_port_force_link_pass_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Link in PASS state.\r\n",
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
        0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Link in normal state.\r\n",
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
*  Pss_learn_enable_vals
*TYPE: BUFFER
*DATE: 01/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "port_utils" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'ctrl').
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
     Pss_learn_enable_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Auto-learn new mac addresses.\r\n",
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
        0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    NO auto-learn new mac addresses.\r\n",
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
*  Pss_unknown_cmd_vals
*TYPE: BUFFER
*DATE: 01/MAY/2003
*FUNCTION:
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
     Pss_unknown_cmd_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "forward",
    {
      {
        GT_LOCK_FRWRD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    do not drop (forward as usual).\r\n",
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
    "drop",
    {
      {
        GT_LOCK_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    drop packets.\r\n",
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
    "trap_to_cpu",
    {
      {
        GT_LOCK_TRAP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Trap to CPU.\r\n",
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
*  Pss_phy_reg_offset_vals
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
     Pss_phy_reg_offset_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        31, 0,
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
*  Pss_indefinite_num_vals
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
     Pss_indefinite_num_vals[]
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
*  Pss_phy_data_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "write_reg" WITHIN 'pss port_utils phy'.
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
     Pss_phy_data_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        0xFFFF, 0, /*Phy registers are 16 bits long*/
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
*  Pss_table_vals
*TYPE: BUFFER
*DATE: 01/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PRINT_TABLE" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'BTIGDE').
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
     Pss_table_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL_LAST,
    "vlan_table",
    {
      {
        FDB_VLAN_TBL_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    VLAN-table.\r\n",
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
    "mac_address_table",
    {
      {
        MAC_ADDR_TBL_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    MAC Address Table for specifi VLAN id.\r\n",
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
*  Pss_cmd
*TYPE: BUFFER
*DATE: 31/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PSS_CMD" IN THE
*  CONTEXT OF SUBJECT 'PSS'.
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
     Pss_cmd[]
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
        MAX_SIZE_OF_TEXT_VAR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Command line arguments to pass to the PSS.",
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
*  Update_pss_fields_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "FIELDS" IN THE CONTEXT OF SUBJECT 'PSS'.
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
     Update_pss_fields_default
#ifdef INIT
   =
{
  VAL_TEXT,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)0
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_traffic_class_default
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "TRAFFIC_CLASS" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_traffic_class_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)3
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_display_format_vals
*TYPE: BUFFER
*DATE: 18/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DISPLAY_FORMAT" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'PORT_UTILS').
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
     Pss_display_format_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "short",
    {
      {
        SHORT_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Display MAC counters in short format: Name and value.",
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
    "long",
    {
      {
        LONG_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Display MAC counters in long format: Detailed description, name and value.",
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
*  Pss_encapsulation_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "ENCAPSULATION" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_encapsulation_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "regular",
    {
      {
        GT_REGULAR_ENCAPSULATION_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Transmit packet using Non-control encapsulation. Packet is subject to\r\n"
              "    egress bridge filtering due to VLAN egress filtering or Spann. Tree\r\n"
              "    filtering.",
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
    "control",
    {
      {
        GT_CONTROL_ENCAPSULATION_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Transmit packet using control encapsulation. Packet is NOT subject to\r\n"
              "    egress bridge filtering due to VLAN egress filtering or Spann. Tree\r\n"
              "    filtering.",
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
*  Pss_encapsulation_default
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "ENCAPSULATION" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_encapsulation_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  GT_REGULAR_ENCAPSULATION_EQUIVALENT,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"regular"
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_clear_after_read_default
*TYPE: BUFFER
*DATE: 18/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "CLEAR_AFTER_READ" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'PORT_UTILS').
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
     Pss_clear_after_read_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  ON_EQUIVALENT,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"on"
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_display_format_default
*TYPE: BUFFER
*DATE: 18/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "DISPLAY_FORMAT" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'PORT_UTILS').
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
     Pss_display_format_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  SHORT_EQUIVALENT,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"short"
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_entry_id_vals
*TYPE: BUFFER
*DATE: 30/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "ENTRY_ID" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_entry_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100,
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
*  Pss_entry_id_default
*TYPE: BUFFER
*DATE: 30/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "ENTRY_ID" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_entry_id_default
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
*  Pss_data_mem_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DATA" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_data_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        PSS_DATA_NUM_VALS,
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
*  Pss_data_mem_default
*TYPE: BUFFER
*DATE: 30/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "DATA" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_data_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0xAABBCCDD
  }
}
#endif
;
/********************************************************
*NAME
*  Port_tagged_vals
*TYPE: BUFFER
*DATE: 21/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PORT_TAGGED" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'VLAN').
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
     Port_tagged_vals[]
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
        (long)"    Add Vlan tag to incoming traffic on this port.",
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
        (long)"    Do NOT add Vlan tag to incoming traffic on this port.",
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
*  Port_tagged_default
*TYPE: BUFFER
*DATE: 21/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "PORT_TAGGED" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'VLAN').
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
     Port_tagged_default
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
*  Bridge_general_commands_vals
*TYPE: BUFFER
*DATE: 22/APR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "GENERAL_COMMANDS" OF SUBJECT
*  'PSS' WITHIN 'BRIDGE' CONTEXT.
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
     Bridge_general_commands_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "set_mac_da_sa_trap",
    {
      {
        SET_MAC_DA_SA_TRAP_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Select da-sa pair of MAC address identifiers.",
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
    VAL_SYMBOL_LAST,
    "display_host_counters",
    {
      {
        HOST_COUNTERS_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Display \'host\' counters (InPkts,InOctets,OutPkts,OutOctets,\r\n"
              "    OutBroadcastPkts,OutMulticastPkts)",
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
*  Cpu_included_vals
*TYPE: BUFFER
*DATE: 23/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "CPU_INCLUDED" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'VLAN').
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
     Cpu_included_vals[]
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
        (long)"    Add this CPU to new Vlan.",
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
        (long)"    Do NOT add this CPU to new Vlan.",
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
*  Cpu_included_default
*TYPE: BUFFER
*DATE: 21/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "CPU_INCLUDED" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'VLAN').
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
     Cpu_included_default
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
*  Pss_append_crc_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "APPEND_CRC" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_append_crc_vals[]
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
        (long)"    Append CRC to this message before transmitting.",
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
        (long)"    Do NOT append CRC to this message before transmitting.",
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
*  Pss_append_crc_default
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "APPEND_CRC" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_append_crc_default
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
*  Pss_tagged_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TAGGED" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_tagged_vals[]
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
        (long)"    Add 802.3P tag to this message before transmitting.",
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
        (long)"    Do NOT Add 802.3P tag to this message before transmitting.",
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
*  Pss_tagged_default
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "TAGGED" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_tagged_default
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
*  Pss_cmd_type_vals
*TYPE: BUFFER
*DATE: 30/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "CMD_TYPE" IN
*  THE CONTEXT OF "PSS" AND "USER_TX_TABLE".
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
     Pss_cmd_type_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "tx_by_vlan",
    {
      {
        GT_CMD_TX_BY_VLAN_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Transmit message using mac address and vlan id (using \"netSendPktByVid\"\r\n"
              "    function).",
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
    "tx_by_if",
    {
      {
        GT_CMD_TX_BY_IF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Transmit message using specified interface (using \"netSendPktByIf\"\r\n"
              "    function).",
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
    "tx_by_if",
    {
      {
        GT_CMD_TX_BY_LPORT_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Transmit message using specified logical port (using \"netSendPktByLport\"\r\n"
              "    function).",
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
*  Pss_logical_if_index_vals
*TYPE: BUFFER
*DATE: 01/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LOGICAL_IF_INDEX" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_logical_if_index_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100,
        1,
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
*  Pss_logical_if_index_default
*TYPE: BUFFER
*DATE: 01/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  SUBJECT 'PSS' PARAMETER "LOGICAL_IF_INDEX" IN THE
*  CONTEXT OF (AND 'USER_TX_TABLE').
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
     Pss_logical_if_index_default
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
*  Pss_index_vals
*TYPE: BUFFER
*DATE: 27/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "INDEX" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_index_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100,
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
*  Pss_index_default
*TYPE: BUFFER
*DATE: 27/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "INDEX" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_index_default
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
*  Gap_between_cycles_vals
*TYPE: BUFFER
*DATE: 21/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "GAP_BETWEEN_CYCLES" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_TX_TABLE SET_TX_MODE').
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
     Gap_between_cycles_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_MAX_VALUE,
        10000,
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
*  Gap_between_cycles_default
*TYPE: BUFFER
*DATE: 21/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "GAP_BETWEEN_CYCLES" IN THE CONTEXT OF
*  SUBJECT 'PSS' (AND 'USER_TX_TABLE SET_TX_MODE').
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
     Gap_between_cycles_default
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
*  Tot_num_packets_vals
*TYPE: BUFFER
*DATE: 21/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TOT_NUM_PACKETS" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_TX_TABLE SET_TX_MODE').
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
     Tot_num_packets_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100000,
        1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
  {
    VAL_SYMBOL,
    "as_specified_per_entry",
    {
      {
        GT_AS_SPECIFIED_PER_ENTRY_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Transmit packets without limitation on the number of cycles\r\n",
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
*  Tot_num_packets_default
*TYPE: BUFFER
*DATE: 21/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "NUM_LOOP_CYCLES" IN THE CONTEXT OF
*  SUBJECT 'PSS' (AND 'USER_TX_TABLE SET_TX_MODE').
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
     Tot_num_packets_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  GT_AS_SPECIFIED_PER_ENTRY_EQUIVALENT,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"as_specified_per_entry"
  }
}
#endif
;
/********************************************************
*NAME
*  Buffer_mode_vals
*TYPE: BUFFER
*DATE: 01/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BUFFER_MODE" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_RX_TABLE SET_RX_MODE').
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
     Buffer_mode_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "cyclic",
    {
      {
        GT_CYCLIC_BUFFER_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Load received messages in a cyclic manner into user table of received\r\n"
              "    messages.",
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
    "fill_once_and_stop",
    {
      {
        GT_USE_ONCE_BUFFER_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Load received messages until user table of received messages is full\r\n"
              "    and, then, stop receiving.",
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
*  Num_entries_vals
*TYPE: BUFFER
*DATE: 02/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "NUM_ENTRIES" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_RX_TABLE SET_RX_MODE').
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
     Num_entries_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        300,
        5,
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
*  Buff_size_vals
*TYPE: BUFFER
*DATE: 02/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BUFF_SIZE" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_RX_TABLE SET_RX_MODE').
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
     Buff_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        10000,
        200,
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
*  Num_loop_cycles_vals
*TYPE: BUFFER
*DATE: 21/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "NUM_LOOP_CYCLES" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_TX_TABLE SET_TX_MODE').
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
     Num_loop_cycles_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100000,
        1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
  {
    VAL_SYMBOL,
    "infinite",
    {
      {
        GT_INFINITE_CYCLES_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Transmit packets without limitation on the number of cycles\r\n",
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
*  Tx_action_vals
*TYPE: BUFFER
*DATE: 22/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TX_ACTION" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_TX_TABLE SET_TX_MODE').
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
     Tx_action_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL_LAST,
    "start_tx",
    {
      {
        GT_START_TX_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Start transmission of packets set in table by the user\r\n",
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
    VAL_SYMBOL_LAST,
    "stop_tx",
    {
      {
        GT_STOP_TX_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Stop transmission of packets set in table by the user (tx assumed to \r\n"
              "    have been started).\r\n",
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
*  Rx_action_vals
*TYPE: BUFFER
*DATE: 01/MAY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "RX_ACTION" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND
*  'USER_RX_TABLE SET_RX_MODE').
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
     Rx_action_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL_LAST,
    "start_rx_capture",
    {
      {
        GT_START_RX_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Start allowing captured messages into user table of received messages\r\n",
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
    VAL_SYMBOL_LAST,
    "stop_rx_capture",
    {
      {
        GT_STOP_RX_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Start allowing captured messages into user table of received messages\r\n"
              "    (Rx assumed to have been started).\r\n",
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
*  Num_loop_cycles_default
*TYPE: BUFFER
*DATE: 21/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "NUM_LOOP_CYCLES" IN THE CONTEXT OF
*  SUBJECT 'PSS' (AND 'USER_TX_TABLE SET_TX_MODE').
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
     Num_loop_cycles_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  GT_INFINITE_CYCLES_EQUIVALENT,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"infinite"
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_user_priority_vals
*TYPE: BUFFER
*DATE: 30/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "USER_PRIORITY" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_user_priority_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        7,
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
*  Pss_user_priority_default
*TYPE: BUFFER
*DATE: 30/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "USER_PRIORITY" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_user_priority_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)3
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_drop_precedence_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DROP_PRECEDENCE" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_drop_precedence_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        2,
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
*  Pss_drop_precedence_default
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "DROP_PRECEDENCE" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_drop_precedence_default
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
*  Pss_pckts_num_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "NUM_PACKETS_IN_GROUP" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_pckts_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        50,
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
*  Pss_pckts_num_default
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "NUM_PACKETS_IN_GROUP" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_pckts_num_default
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
*  Pss_data_len_vals
*TYPE: BUFFER
*DATE: 01/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DATA_LEN" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_data_len_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1500,
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
*  Pss_data_len_default
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "DATA_LEN" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_data_len_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)4
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_gap_vals
*TYPE: BUFFER
*DATE: 01/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "GAP" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_gap_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100,
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
*  Pss_gap_default
*TYPE: BUFFER
*DATE: 01/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "GAP" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'USER_TX_TABLE').
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
     Pss_gap_default
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
*  Pss_wait_between_groups_vals
*TYPE: BUFFER
*DATE: 01/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "WAIT_BETWEEN_GROUPS" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_wait_between_groups_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100,
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
*  Pss_wait_between_groups_default
*TYPE: BUFFER
*DATE: 01/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "WAIT_BETWEEN_GROUPS" IN THE CONTEXT OF
*  SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_wait_between_groups_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)4
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_traffic_class_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TRAFFIC_CLASS" IN THE
*  CONTEXT OF SUBJECT 'PSS' (AND 'USER_TX_TABLE').
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
     Pss_traffic_class_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        7,
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
*  Pss_destination_pp_id_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PSS" WITHIN 'test send_buff'.
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
     Pss_destination_pp_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        127,0,
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
*  Pss_destination_fe_id_vals
*TYPE: BUFFER
*DATE: 18/JUN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PSS" WITHIN 'test send_buff'.
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
     Pss_destination_fe_id_vals[]
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
*  Pss_data_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PSS" WITHIN 'test send_buff'.
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
     Pss_data_vals[]
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
        MAX_REPEATED_PARAM_VAL,
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
*  Pss_buff_size_vals
*TYPE: BUFFER
*DATE: 25/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PSS" WITHIN 'test send_buff'.
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
     Pss_buff_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        2*1024,0,
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
*  Pss_empty_vals
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
     Pss_empty_vals[]
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
*  Memory_size_vals
*TYPE: BUFFER
*DATE: 13/JUNE/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "size" IN
*  THE CONTEXT OF "PSS" AND "DIAGNOSTICS TEST_MEM".
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
     Memory_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
          /*
           * No max and no min here.
           */
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * No attched procedure to call when item value is entered.
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
*  Memory_size_default
*TYPE: BUFFER
*DATE: 15/JUNE/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "size" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'DIAGNOSTICS TEST_MEM').
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
     Memory_size_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)16
  }
}
#endif
;
/********************************************************
*NAME
*  Memory_offset_vals
*TYPE: BUFFER
*DATE: 13/JUNE/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "offset" IN
*  THE CONTEXT OF "PSS" AND "DIAGNOSTICS TEST_MEM".
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
     Memory_offset_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
          /*
           * No max and no min here.
           */
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
*  Memory_offset_default
*TYPE: BUFFER
*DATE: 15/JUNE/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "offset" IN THE CONTEXT OF SUBJECT 'PSS'
*  (AND 'DIAGNOSTICS TEST_MEM').
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
     Memory_offset_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)0
  }
}
#endif
;
/********************************************************
*NAME
*  Memory_type_vals
*TYPE: BUFFER
*DATE: 13/JUNE/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "memory_type" IN
*  THE CONTEXT OF "PSS" AND "DIAGNOSTICS TEST_MEM".
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
     Memory_type_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "buffer_dram",
    {
      {
        BUFFER_DRAM_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    The external packet buffer SDRAM",
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
    "wide_sram",
    {
      {
        WIDE_SRAM_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    The external Wide SRAM, or internal Control SRAM if the device was\r\n"
              "    configured without external wide-SRAM",
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
    "narrow_sram",
    {
      {
        NARROW_SRAM_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    The external Narrow SRAM, or internal IP SRAM if the device was configured\r\n"
              "    without external wide-SRAM",
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
    "flow_dram",
    {
      {
        FLOW_DRAM_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    The external flow DRAM",
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
    "mac_tbl_mem",
    {
      {
        MAC_TBL_MEM_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Internal MAC table memory",
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
    "vlan_tbl_mem",
    {
      {
        VLAN_TBL_MEM_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Internal VLAN table memory",
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
*  Learning_mode_vals
*TYPE: BUFFER
*DATE: 20/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "SELECT_LEARNING_MODE" IN
*  THE CONTEXT OF "PSS" AND "VLAN".
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
     Learning_mode_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "individual",
    {
      {
        VLAN_INDIVIDUAL_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    IVL - Independent Vlan Learning. Both Valn and Mac are station identifiers",
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
    "shared",
    {
      {
        VLAN_SHARED_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    SVL - Shared Vlan Learning. Only Mac address is station identifier",
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
*  Mac_addr_vals
*TYPE: BUFFER
*DATE: 24/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "MAC_ADDR" IN THE
*  CONTEXT OF SUBJECT 'PSS'.
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
     Mac_addr_vals[]
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
        12,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    MAC address parameter. 12 hex digits.",
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
*  Traffic_class_vals
*TYPE: BUFFER
*DATE: 15/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "SOURCE_TRAFFIC_CLASS"  or
*  "DEST_TRAFFIC_CLASS" IN THE CONTEXT OF SUBJECT 'PSS'.
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
     Traffic_class_vals[]
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
*  Pss_nof_consecutive_macs_vals
*TYPE: BUFFER
*DATE: 15/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER ""  or
*  "" IN THE CONTEXT OF SUBJECT 'PSS'.
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
     Pss_nof_consecutive_macs_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        BIT(16),1,
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
*  Pss_increment_unit_every_n_vals
*TYPE: BUFFER
*DATE: 15/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER ""  or
*  "" IN THE CONTEXT OF SUBJECT 'PSS'.
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
     Pss_increment_unit_every_n_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        BIT(16),0,
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
*  Vlan_id_vals
*TYPE: BUFFER
*DATE: 20/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "VLAN_ID" IN THE
*  CONTEXT OF SUBJECT 'PSS'.
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
     Vlan_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1024,1,
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
*  Id_of_trunk_port_vals
*TYPE: BUFFER
*DATE: 02/APRIL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TRUNK_PORT" IN THE
*  CONTEXT OF SUBJECT 'PSS'.
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
     Id_of_trunk_port_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        7,1,
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
*  Unit_vals
*TYPE: BUFFER
*DATE: 20/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "UNIT" IN THE
*  CONTEXT OF SUBJECT 'PSS'.
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
     Unit_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        10,0,
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
*  Traffic_class_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "SOURCE_TRAFFIC_CLASS" OR "DEST_TRAFFIC_CLASS"
*  IN THE CONTEXT OF SUBJECT 'PSS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent
*       Numeric value related to string_value. Only relevant
*       for val_type = VAL_SYMBOL
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
     Traffic_class_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)0
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_nof_consecutive_macs_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "
*  IN THE CONTEXT OF SUBJECT 'PSS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent
*       Numeric value related to string_value. Only relevant
*       for val_type = VAL_SYMBOL
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
     Pss_nof_consecutive_macs_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)1
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_increment_unit_every_n_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "
*  IN THE CONTEXT OF SUBJECT 'PSS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent
*       Numeric value related to string_value. Only relevant
*       for val_type = VAL_SYMBOL
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
     Pss_increment_unit_every_n_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)0
  }
}
#endif
;
/********************************************************
*NAME
*  Pss_unit_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "FIELDS" IN THE CONTEXT OF SUBJECT 'PSS'.
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
     Pss_unit_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)0
  }
}
#endif
;
/********************************************************
*NAME
*  Device_vals
*TYPE: BUFFER
*DATE: 20/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DEVICE" IN THE
*  CONTEXT OF SUBJECT 'PSS'.
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
     Device_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        126,0,
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
*  Port_vals
*TYPE: BUFFER
*DATE: 20/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PORT" IN THE
*  CONTEXT OF SUBJECT 'PSS'.
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
     Port_vals[]
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
*  Pss_device_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "FIELDS" IN THE CONTEXT OF SUBJECT 'PSS'.
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
     Pss_device_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)0
  }
}
#endif
;
/********************************************************
*NAME
*  Test_profile_vals
*TYPE: BUFFER
*DATE: 15/JUNE/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "test_profile" IN
*  THE CONTEXT OF "PSS" AND "DIAGNOSTICS TEST_MEM".
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
     Test_profile_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "incremental",
    {
      {
        INCREMENTAL_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Write incremental values and verify",
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
    "bit_toggle",
    {
      {
        BIT_TOGGLE_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Set bit and verify",
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
    "aa_55",
    {
      {
        AA_55_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Set 0xAAAAAAAA/0x55555555 and verify",
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
*  PSS_params
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "pss".
*ORGANIZATION:
* look at 'PARAM' comments
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Pss_params[]
#ifdef INIT
   =
{
#if !INCLUDE_DUNE_RPC
/* { */
    /*
     * Allow initialization only if DUNE_RPC has not done it anyway.
     */
  {
    PARAM_PSS_MODULE_OPEN,
    "module_open",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Open the PSS module",
    "Examples:\r\n"
    "  pss module_open\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
/* } */
#endif
  {
    PARAM_PSS_DIAGNOSTICS_TESTPROFILE_ID,
    "test_profile",
    (PARAM_VAL_RULES *)&Test_profile_vals[0],
    (sizeof(Test_profile_vals) / sizeof(Test_profile_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select type of memory test to carry out (e.g. aa_55)",
    "Example:\r\n"
    "  pss diagnostics test_mem device 0 memory_type buffer_dram offset 0 size 16\r\n"
    "  test_profile incremental\r\n"
    "Results in destructively testing external packet buffer SDRAM memory on local\r\n"
    "device 0. Test will start from byte offset 0 (offset must be a multiple of 4)\r\n"
    "and will cover 16 bytes. Selected test kind (profile) is \'incremental\':\r\n"
    "Write incremental values and verify.\r\n",
    "",
     {0,
    BIT(0)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&Memory_size_vals[0],
    (sizeof(Memory_size_vals) / sizeof(Memory_size_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Memory_size_default,
    0,0,0,0,0,0,0,
    "Select size of memory block to test for selected memory type",
    "Example:\r\n"
    "  pss diagnostics test_mem device 0 memory_type buffer_dram offset 0 size 16\r\n"
    "  test_profile incremental\r\n"
    "Results in destructively testing external packet buffer SDRAM memory on local\r\n"
    "device 0. Test will start from byte offset 0 (offset must be a multiple of 4)\r\n"
    "and will cover 16 bytes. Selected test kind (profile) is \'incremental\':\r\n"
    "Write incremental values and verify.\r\n",
    "",
     {0,
    BIT(0)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_OFFEST_ID,
    "offset",
    (PARAM_VAL_RULES *)&Memory_offset_vals[0],
    (sizeof(Memory_offset_vals) / sizeof(Memory_offset_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Memory_offset_default,
    0,0,0,0,0,0,0,
    "Select offset to start test on for selected memory type",
    "Example:\r\n"
    "  pss diagnostics test_mem device 0 memory_type buffer_dram offset 0 size 16\r\n"
    "  test_profile incremental\r\n"
    "Results in destructively testing external packet buffer SDRAM memory on local\r\n"
    "device 0. Test will start from byte offset 0 (offset must be a multiple of 4)\r\n"
    "and will cover 16 bytes. Selected test kind (profile) is \'incremental\':\r\n"
    "Write incremental values and verify.\r\n",
    "",
     {0,
    BIT(0)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_MEMTYPE_ID,
    "memory_type",
    (PARAM_VAL_RULES *)&Memory_type_vals[0],
    (sizeof(Memory_type_vals) / sizeof(Memory_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select memory type to test (e.g. wide_sram)",
    "Example:\r\n"
    "  pss diagnostics test_mem device 0 memory_type buffer_dram offset 0 size 16\r\n"
    "  test_profile incremental\r\n"
    "Results in destructively testing external packet buffer SDRAM memory on local\r\n"
    "device 0. Test will start from byte offset 0 (offset must be a multiple of 4)\r\n"
    "and will cover 16 bytes. Selected test kind (profile) is \'incremental\':\r\n"
    "Write incremental values and verify.\r\n",
    "",
     {0,
    BIT(0)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_TESTREG_ID,
    "test_reg",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Test the device read/write-ability of the on-chip r/w registers",
    "Example:\r\n"
    "  pss diagnostics test_reg device 0\r\n"
    "Results in destructively testing ,on device 0, read/write-ability\n\r"
    "of the on-chip r/w registers.\r\n",
    "",
    {0,
     BIT(1)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_DUMPREG_ID,
    "dump_reg",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dumping device register values.",
    "Example:\r\n"
    "  pss diagnostics dump_reg device 0\r\n"
    "Results in dumping the device register values.\r\n",
    "",
    {0,
     BIT(1)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_TESTMACUPDMSG_ID,
    "test_mac_upd_msg",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Test the ability to NA message, send a QA message and receive QR.",
    "Example:\r\n"
    "  pss diagnostics test_mac_upd_msg device 0\r\n"
    "Results in:\r\n"
    " 1) send a New Address (NA) message to a local device.\r\n"
    " 2) send a Query Address (QA) message for this MAC entry.\r\n"
    " 3) Receive an Update Message interrupt containing the Query Response (QR)\r\n"
    "from the device\r\n",
    "",
    {0,
     BIT(1)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_TESTMEM_ID,
    "test_mem",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out memory test on various memory blocks",
    "Example:\r\n"
    "  pss diagnostics test_mem device 0 memory_type buffer_dram offset 0 size 16\r\n"
    "  test_profile incremental\r\n"
    "Results in destructively testing external packet buffer SDRAM memory on local\r\n"
    "device 0. Test will start from byte offset 0 (offset must be a multiple of 4)\r\n"
    "and will cover 16 bytes. Selected test kind (profile) is \'incremental\':\r\n"
    "Write incremental values and verify.\r\n",
    "",
    {0,
    BIT(0)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DIAGNOSTICS_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Invoke various diagnostic tools such as memory tests.",
    "Example:\r\n"
    "  pss diagnostics test_mem device 0 memory_type buffer_dram offset 0 size 16\r\n"
    "  test_profile incremental\r\n"
    "Results in destructively testing external packet buffer SDRAM memory on local\r\n"
    "device 0. Test will start from byte offset 0 (offset must be a multiple of 4)\r\n"
    "and will cover 16 bytes. Selected test kind (profile) is \'incremental\':\r\n"
    "Write incremental values and verify.\r\n",
    "",
    {0,
    BIT(0) | BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TEST_ID,
    "test",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Testing and develop utilities for the PSS module",
    "Examples:\r\n"
    "  pss test send_buff destination_pp_id 2 buff_size 13 data 0x1234 0x7654\n\r"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to packet processor with identifier 2. If less data is given pending\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.",
    "",
    {0,
     0,
     BIT(1) | BIT(2) | BIT(3) | BIT(4)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_LEARN_ENABLE_ID,
    "learn_enable",
    (PARAM_VAL_RULES *)&Pss_learn_enable_vals[0],
    (sizeof(Pss_learn_enable_vals) / sizeof(Pss_learn_enable_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Control learning of new MAC addresses in received packets.",
    "Example:\r\n"
    "  pss bridge learn_enable off\r\n"
    "Set PP to not auto-learn new mac address.\r\n",
    "",
    {BIT(24)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_UNKNOWN_CMD_ID,
    "unknown_cmd",
    (PARAM_VAL_RULES *)&Pss_unknown_cmd_vals[0],
    (sizeof(Pss_unknown_cmd_vals) / sizeof(Pss_unknown_cmd_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "How to forward packets with unknown/changed SA if learn is disable.",
    "Example:\r\n"
    "  pss bridge learn_enable off unknown_cmd drop\r\n"
    "Set PP to not auto-learn new mac address and drop all packet\r\n"
    "  received from un-known source addresses",
    "",
    {BIT(24)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DEL_MAC_ENTRY_ID,
    "delete_mac_entry",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deletes an existing entry in MAC address table.",
    "Example:\r\n"
    "  pss bridge delete_mac_entry vlan_id 1 dest_addr 756767676767\r\n"
    "Deletes the existing entry in MAC address table on Vlan 1 with MAC address\r\n"
    "0x756767676767\r\n",
    "",
    {BIT(27)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_ADD_MAC_ENTRY_ID,
    "add_mac_entry",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Creates the new entry in MAC address table.",
    "Example:\r\n"
    "  pss bridge add_mac_entry vlan_id 1 dest_addr 756767676767\r\n"
    "  device 0 unit 0 port 10 source_traffic_class 4\r\n"
    "Creates the new entry in MAC address table on Vlan 1 with MAC address\r\n"
    "0x756767676767, which is assigned to unit;device;port = 0;0;10. If this\r\n"
    "MAC is found to be the source address of an incoming, untagged, packet on\r\n"
    "that port, the packet is assigned traffic class 4\r\n"
    "NOTE: This command may also be used to update an existing MAC table entry!\r\n",
    "",
    {BIT(22)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_SRC_TRAFFIC_CLASS_ID,
    "source_traffic_class",
    (PARAM_VAL_RULES *)&Traffic_class_vals[0],
    (sizeof(Traffic_class_vals) / sizeof(Traffic_class_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Traffic_class_default,
    0,0,0,0,0,0,0,
    "Traffic class to assign to incoming packet with specified source-MAC;VLAN.",
    "Example:\r\n"
    "  pss bridge add_mac_entry vlan_id 1 dest_addr 756767676767\r\n"
    "  device 0 unit 0 port 10 source_traffic_class 4\r\n"
    "Creates the new entry in MAC address table on Vlan 1 with MAC address\r\n"
    "0x756767676767, which is assigned to unit;device;port = 0;0;10. If this\r\n"
    "MAC is found to be the source address of an incoming, untagged, packet on\r\n"
    "that port, the packet is assigned traffic class 4\r\n",
    "",
    {BIT(22)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DST_TRAFFIC_CLASS_ID,
    "dest_traffic_class",
    (PARAM_VAL_RULES *)&Traffic_class_vals[0],
    (sizeof(Traffic_class_vals) / sizeof(Traffic_class_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Traffic_class_default,
    0,0,0,0,0,0,0,
    "Traffic class to assign to incoming packet with specified dest-MAC;VLAN.",
    "Example:\r\n"
    "  pss bridge add_mac_entry vlan_id 1 dest_addr 756767676767\r\n"
    "  device 0 unit 0 port 10 dest_traffic_class 4\r\n"
    "Creates the new entry in MAC address table on Vlan 1 with MAC address\r\n"
    "0x756767676767, which is assigned to unit;device;port = 0;0;10. If this\r\n"
    "MAC is found to be the destination address of an incoming, untagged, packet on\r\n"
    "that port, the packet is assigned traffic class 4\r\n",
    "",
    {BIT(22)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_NOF_CONSECUTIVE_MACS_ID,
    "nof_consecutive_macs",
    (PARAM_VAL_RULES *)&Pss_nof_consecutive_macs_vals[0],
    (sizeof(Pss_nof_consecutive_macs_vals) / sizeof(Pss_nof_consecutive_macs_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_nof_consecutive_macs_default,
    0,0,0,0,0,0,0,
    "Create several MACs. There address are incrementing.\n\r"
    "  NOTE: If bigger than 1, the 16 lower bits gets overwritten.",
    "Example:\r\n"
    "  pss bridge add_mac_entry vlan_id 1 dest_addr 75676767FFFF nof_consecutive_macs 11\r\n"
    "  device 0 unit 0 port 10 dest_traffic_class 4\r\n"
    "Creates 11 new entry in MAC address table on Vlan 1 with MAC addressed\r\n"
    "0x756767670000, 0x756767670001, 0x756767670002 .., 0x75676767000A\r\n",
    "",
    {BIT(22)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_INCREMENT_DEVICE_ID_EVERY_N_ID,
    "increment_unit_every_n",
    (PARAM_VAL_RULES *)&Pss_increment_unit_every_n_vals[0],
    (sizeof(Pss_increment_unit_every_n_vals) / sizeof(Pss_increment_unit_every_n_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_increment_unit_every_n_default,
    0,0,0,0,0,0,0,
    "If appears, increment the device every N MAC addresses.",
    "Example:\r\n"
    "  pss bridge add_mac_entry vlan_id 1 dest_addr 75676767FFFF nof_consecutive_macs 11 increment_unit_every_n 2\r\n"
    "  device 0 unit 0 port 10 dest_traffic_class 4\r\n"
    "Creates 11 new entry in MAC address table on Vlan 1 with MAC addressed\r\n"
    "  0x756767670000, 0x756767670001, 0x756767670002 .., 0x75676767000A\r\n"
    "  The fisrt 2 use device 0, the next 2 device id 1, ...\n\r",
    "",
    {BIT(22)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PRINT_TABLE_ID,
    "print_table",
    (PARAM_VAL_RULES *)&Pss_table_vals[0],
    (sizeof(Pss_table_vals) / sizeof(Pss_table_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Pretty-print specific table.",
    "Example:\r\n"
    "  pss test print_table vlan_table\r\n"
    "Results in printing the VLAN table\r\n",
    "",
    {BIT(21)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_VLAN_ID,
    "vlan_id",
    (PARAM_VAL_RULES *)&Vlan_id_vals[0],
    (sizeof(Vlan_id_vals) / sizeof(Vlan_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "vlan_id for the MAC table.",
    "Example:\r\n"
    "  pss test print_table mac_address_table vlan_id 1\r\n"
    "Results in printing the VLAN table\r\n",
    "",
    {BIT(21)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TEST_DMA_STATUS_ID,
    "dma_status",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print DMA status.",
    "Examples:\r\n"
    "  pss test dma_status\n\r"
    "Print DMA status.",
    "",
    {0,
     0,
     BIT(3)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TEST_SEND_BUFF_TO_CORE_ID,
    "send_buff_to_core",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send buffer to other PP utility. Testing the XDR and IPC. Develop purposes.",
    "Examples:\r\n"
    "  pss test send_buff_to_core destination_pp_id 2 buff_size 13 data 0x1234 0x7654\n\r"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to packet processor with identifier 2. If less data is given pending\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.",
    "",
    {0,
     0,
     BIT(1)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TEST_SEND_BUFF_TO_TAPI_ID,
    "send_buff_to_tapi",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send buffer to other PP utility. Testing the XDR and IPC. Develop purposes.",
    "Examples:\r\n"
    "  pss test send_buff_to_tapi buff_size 13 data 0x1234 0x7654\n\r"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to TAPI. If less data is given pending\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.",
    "",
    {0,
     0,
     BIT(2)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DESTINATION_PP_ID_ID,
    "destination_pp_id",
    (PARAM_VAL_RULES *)&Pss_destination_pp_id_vals[0],
    (sizeof(Pss_destination_pp_id_vals) / sizeof(Pss_destination_pp_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet processor identifier",
    "Examples:\r\n"
    "  pss test send_buff_to_core destination_pp_id 2 buff_size 13 data 0x1234 0x7654\n\r"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to packet processor with identifier 2. If less data is given padding\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.",
    "",
    {0,
     0,
     BIT(1)},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DESTINATION_NOT_PP_ID_ID,
    "destination_other_unit",
    (PARAM_VAL_RULES *)&Pss_destination_fe_id_vals[0],
    (sizeof(Pss_destination_fe_id_vals) / sizeof(Pss_destination_fe_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet processor identifier",
    "Examples:\r\n"
    "  pss test send_buff_to_core destination_other_unit 2 buff_size 13 data 0x1234 0x7654\n\r"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to other device (probably FE) with identifier 2. If less data is given padding\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.",
    "",
    {0,
     0,
     BIT(1)},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TEST_BUFF_SIZE_ID,
    "buff_size",
    (PARAM_VAL_RULES *)&Pss_buff_size_vals[0],
    (sizeof(Pss_buff_size_vals) / sizeof(Pss_buff_size_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "buffer size (in bytes) to send",
    "Examples:\r\n"
    "  pss test send_buff_to_core destination_pp_id 2 buff_size 13 data 0x1234 0x7654\n"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to packet processor with identifier 2. If less data is given pending\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.",
    "",
    {0,
     0,
     BIT(1) | BIT(2) },
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Pss_data_vals[0],
    (sizeof(Pss_data_vals) / sizeof(Pss_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "buffer data",
    "Examples:\r\n"
    "  pss test send_buff_to_core destination_pp_id 2 buff_size 13 data 0x1234 0x7654\n"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to packet processor with identifier 2. If less data is given pending\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.",
    "",
    {0,
     0,
     BIT(1) | BIT(2) },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TEST_SEND_BUFF_TO_CORE_DEFERRED_ID,
    "deferred",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deferred call. Sending to core with NON_BLOCKED. Wait for sec and GET_RESULT.",
    "Examples:\r\n"
    "  pss test send_buff_to_core destination_pp_id 2 buff_size 13 data 0x1234 0x7654 deferred\n"
    "Send buffer with 13 bytes (byte-0: 0x0, b-1:0x0, b-2 0x12, b-3 0x34...),\n\r"
    "  to packet processor with identifier 2. If less data is given pending\n\r"
    "  of zeros is done. If more data is given, the extra bytes are ignored.\n\r"
    "  Sending to core with NON_BLOCKED. Wait for sec and GET_RESULT, through IPC.",
    "",
    {0,
     0,
     BIT(1) | BIT(2) },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_MODULE_CLOSE,
    "module_close",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Close the PSS module",
    "Examples:\r\n"
    "  pss module_close\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_VLAN_SELECT_LEARNING_MODE_ID,
    "select_learning_mode",
    (PARAM_VAL_RULES *)&Learning_mode_vals[0],
    (sizeof(Learning_mode_vals) / sizeof(Learning_mode_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select VLAN related learning mode: Individual or Shared.",
    "Examples:\r\n"
    "  pss vlan select_learning_mode shared\r\n"
    "Results in selecting shared learning mode on master and all slaves:\r\n"
    "Only Mac address goes into the learning table and is used as a station\r\n"
    "identifier.\r\n",
    "",
    {BIT(3)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_VLAN_CPU_INCLUDED_ID,
    "cpu_included",
    (PARAM_VAL_RULES *)&Cpu_included_vals[0],
    (sizeof(Cpu_included_vals) / sizeof(Cpu_included_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Cpu_included_default,
    0,0,0,0,0,0,0,
    "Select VLAN option: Add this CPU to a newly created Vlan or not.",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "  port_tagged on cpu_included on\r\n"
    "Create a new vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is tagged. CPU is\r\n"
    "a member of this Vlan.\r\n",
    "",
    {BIT(5) | BIT(7) | BIT(16) | BIT(18)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_VLAN_PORT_TAGGED_ID,
    "port_tagged",
    (PARAM_VAL_RULES *)&Port_tagged_vals[0],
    (sizeof(Port_tagged_vals) / sizeof(Port_tagged_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Port_tagged_default,
    0,0,0,0,0,0,0,
    "Select VLAN option: Tag all incoming packets or not.",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "  port_tagged on cpu_included on\r\n"
    "Create a new vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is tagged. CPU is\r\n"
    "a member of this Vlan.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(13) | BIT(14) | BIT(15) | BIT(16)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PORT_ID,
    "port",
    (PARAM_VAL_RULES *)&Port_vals[0],
    (sizeof(Port_vals) / sizeof(Port_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select PORT identifier.",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "Create a new vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is NOT tagged. CPU is\r\n"
    "NOT a member of this Vlan.\r\n",
    "",
    {BIT(5) | BIT(7) | BIT(10) | BIT(13) | BIT(15) |
     BIT(17) | BIT(22) | BIT(24) | BIT(25) | BIT(28),
     0,
     BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DEVICE_ID,
    "device",
    (PARAM_VAL_RULES *)&Device_vals[0],
    (sizeof(Device_vals) / sizeof(Device_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_device_default,
    0,0,0,0,0,0,0,
    "Select DEVICE identifier.",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "Create a new vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is NOT tagged. CPU is\r\n"
    "NOT a member of this Vlan.\r\n",
    "",
    {BIT(5) | BIT(7) | BIT(10) | BIT(13) | BIT(15) |
     BIT(17) | BIT(22) | BIT(24) | BIT(25) | BIT(28),
     BIT(0) | BIT(1),
     BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_UNIT_ID,
    "unit",
    (PARAM_VAL_RULES *)&Unit_vals[0],
    (sizeof(Unit_vals) / sizeof(Unit_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_unit_default,
    0,0,0,0,0,0,0,
    "Select UNIT identifier.",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "Create a new vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is NOT tagged. CPU is\r\n"
    "NOT a member of this Vlan.\r\n",
    "",
    {BIT(5) | BIT(7) | BIT(10) | BIT(13) | BIT(15) | BIT(17) |
     BIT(22) | BIT(24) | BIT(25) | BIT(28),
     0,
     BIT(4) | BIT(5) | BIT(6)  | BIT(7) | BIT(8)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_VLAN_ID_ID,
    "vlan_id",
    (PARAM_VAL_RULES *)&Vlan_id_vals[0],
    (sizeof(Vlan_id_vals) / sizeof(Vlan_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select VLAN identifier.",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "Create a new vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is NOT tagged. CPU is\r\n"
    "NOT a member of this Vlan.\r\n",
    "",
    {BIT(4) | BIT(5)  | BIT(6)  | BIT(7)  | BIT(10) |
    BIT(13) | BIT(14) | BIT(15) | BIT(16) | BIT(17) |
    BIT(18) | BIT(19) | BIT(25) | BIT(22) |
    BIT(26) | BIT(27),
    BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_ID_OF_TRUNK_PORT_ID,
    "id_of_trunk_port",
    (PARAM_VAL_RULES *)&Id_of_trunk_port_vals[0],
    (sizeof(Id_of_trunk_port_vals) / sizeof(Id_of_trunk_port_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select TRUNK port identifier.",
    "Example:\r\n"
    "  pss vlan create_new trunk_port vlan_id 14 id_of_trunk_port 2\r\n"
    "Create a new vlan whose id is \'14\' and add to it a trunk port (not standard\r\n"
    "port). New trunk port id is 2. New port is NOT tagged. CPU is NOT a member\r\n"
    "of this Vlan.\r\n",
    "",
    {BIT(4) | BIT(6) | BIT(14) | BIT(16) | BIT(18) |
     BIT(19) | BIT(26) | BIT(29)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_MAC_ADDR_ID_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&Mac_addr_vals[0],
    (sizeof(Mac_addr_vals) / sizeof(Mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select MAC address identifier.",
    "Example:\r\n"
    "  pss multicast add_port standard_port vlan_id 14 mac_addr beeb1234beeb unit 0\r\n"
    "  device 1 port 2 \r\n"
    "Add a port to an existing multicast group on vlan whose id is \'14\'. This is a\r\n"
    "standard port (not trunk port). New port is on unit 0, device 1, port 2.\r\n",
    "",
    {BIT(6) | BIT(7) | BIT(10) | BIT(17) | BIT(18) | BIT(19),
    BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_INDEX_ID,
    "index",
    (PARAM_VAL_RULES *)&Pss_index_vals[0],
    (sizeof(Pss_index_vals) / sizeof(Pss_index_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_index_default,
    0,0,0,0,0,0,0,
    "Select index of tx message to display from user table.",
    "Example:\r\n"
    "  pss user_tx_table get index 0\r\n"
    "Results in display of transmit message which the user placed in the table\r\n"
    "at index \'0\' (first message). User either gets message or an indication\r\n"
    "that there is no message to read at that offset in the table.\r\n",
    "",
    {BIT(8)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_LOGICAL_IF_INDEX_ID,
    "logical_if_index",
    (PARAM_VAL_RULES *)&Pss_logical_if_index_vals[0],
    (sizeof(Pss_logical_if_index_vals) / sizeof(Pss_logical_if_index_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_logical_if_index_default,
    0,0,0,0,0,0,0,
    "Select logical interface index to use for this tx message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_if entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 logical_if_index 8\r\n"
    "See full explanation on help of \'add\' and \'tx_by_if\'.\r\n",
    "",
    {0,BIT(11)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PCKT_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Pss_data_mem_vals[0],
    (sizeof(Pss_data_mem_vals) / sizeof(Pss_data_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_data_mem_default,
    0,0,0,0,0,0,0,
    "Select data to send on this message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1 data 0x1234abcd\r\n"
    "  0x89bead01 data_len 8\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. Data on this message\r\n"
    "will be \'1234abcd89bead01\' (8 bytes).\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_WAIT_BET_GROUPS_ID,
    "wait_between_groups",
    (PARAM_VAL_RULES *)&Pss_wait_between_groups_vals[0],
    (sizeof(Pss_wait_between_groups_vals) / sizeof(Pss_wait_between_groups_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_wait_between_groups_default,
    0,0,0,0,0,0,0,
    "Select time (ms) to wait between transmission of groups of this message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 num_packets_in_group 2 gap 5 wait_between_groups 10\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. This message\r\n"
    "will be sent is groups of 2 and with 5 ms delay between each message.\r\n"
    "Time delay between groups will be 10 ms.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_GAP_BET_PCKTS_ID,
    "gap",
    (PARAM_VAL_RULES *)&Pss_gap_vals[0],
    (sizeof(Pss_gap_vals) / sizeof(Pss_gap_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_gap_default,
    0,0,0,0,0,0,0,
    "Select time (ms) to wait between transmission of copies of this message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 num_packets_in_group 2 gap 5 wait_between_groups 10\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. This message\r\n"
    "will be sent is groups of 2 and with 5 ms delay between each message.\r\n"
    "Time delay between groups will be 10 ms.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PCKT_DATA_LEN_ID,
    "data_len",
    (PARAM_VAL_RULES *)&Pss_data_len_vals[0],
    (sizeof(Pss_data_len_vals) / sizeof(Pss_data_len_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_data_len_default,
    0,0,0,0,0,0,0,
    "Select length of data (no. of data bytes) to send on this message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1 data 0x1234abcd\r\n"
    "  0x89bead01 data_len 8\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. Data on this message\r\n"
    "will be \'1234abcd89bead01\' (8 bytes).\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PCKTS_NUM_ID,
    "num_packets_in_group",
    (PARAM_VAL_RULES *)&Pss_pckts_num_vals[0],
    (sizeof(Pss_pckts_num_vals) / sizeof(Pss_pckts_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_pckts_num_default,
    0,0,0,0,0,0,0,
    "Select number of times to send this messages in one group.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1\r\n"
    "  num_packets_in_group 30 gap 3\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. 30 copies of this\r\n"
    "message will be sent with time gap of 3 ms between messages.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TAGGED_ID,
    "tag_this_packet",
    (PARAM_VAL_RULES *)&Pss_tagged_vals[0],
    (sizeof(Pss_tagged_vals) / sizeof(Pss_tagged_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_tagged_default,
    0,0,0,0,0,0,0,
    "Select whether to append CRC to this message, when transmitted.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1\r\n"
    "  tag_this_packet on\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. Message will be added\r\n"
    "802.3P tag when tramsmitted.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_APPEND_CRC_ID,
    "append_crc",
    (PARAM_VAL_RULES *)&Pss_append_crc_vals[0],
    (sizeof(Pss_append_crc_vals) / sizeof(Pss_append_crc_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_append_crc_default,
    0,0,0,0,0,0,0,
    "Select whether to append CRC to this message, when transmitted.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1\r\n"
    "  append_crc on\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. Message will be\r\n"
    "appended CRC when tramsmitted.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_ENCAPSULATION_ID,
    "encapsulation",
    (PARAM_VAL_RULES *)&Pss_encapsulation_vals[0],
    (sizeof(Pss_encapsulation_vals) / sizeof(Pss_encapsulation_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_encapsulation_default,
    0,0,0,0,0,0,0,
    "Select encapsulation (control or regular) of tx message to add.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1\r\n"
    "  encapsulation control\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. Message is assigned\r\n"
    "\'control\' encapsulation which means that it is NOT subject to egress bridge\r\n"
    "filtering due to VLAN egress filtering or Spanning Tree filtering\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&Pss_traffic_class_vals[0],
    (sizeof(Pss_traffic_class_vals) / sizeof(Pss_traffic_class_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_traffic_class_default,
    0,0,0,0,0,0,0,
    "Select egress tx traffic class queue (range 0-7) of tx message to add.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&Pss_drop_precedence_vals[0],
    (sizeof(Pss_drop_precedence_vals) / sizeof(Pss_drop_precedence_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_drop_precedence_default,
    0,0,0,0,0,0,0,
    "Select drop precedence (color, range 0-2) of tx message to add to user table.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2 drop_precedence 1\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_USER_PRIORITY_ID,
    "user_priority",
    (PARAM_VAL_RULES *)&Pss_user_priority_vals[0],
    (sizeof(Pss_user_priority_vals) / sizeof(Pss_user_priority_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_user_priority_default,
    0,0,0,0,0,0,0,
    "Select IEEE802.1P priority (range 0-7) of tx message to add to user table.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14 user_priority 2\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_ENTRY_ID_ID,
    "entry_id",
    (PARAM_VAL_RULES *)&Pss_entry_id_vals[0],
    (sizeof(Pss_entry_id_vals) / sizeof(Pss_entry_id_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_entry_id_default,
    0,0,0,0,0,0,0,
    "Select entry_id of tx message to add to user table.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_STANDARD_PORT_TYPE_ID,
    "standard_port",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select type of port: Standard port (NOT trunk port).",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "Create a new vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is NOT tagged. CPU is\r\n"
    "NOT a member of this Vlan.\r\n",
    "",
    {BIT(5) | BIT(7) | BIT(10) | BIT(13) | BIT(15) |
     BIT(17) | BIT(25) | BIT(28)},
    (unsigned int)5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TRUNK_PORT_TYPE_ID,
    "trunk_port",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select type of port: Trunk port (NOT standard port).",
    "Example:\r\n"
    "  pss vlan create_new trunk_port vlan_id 14 id_of_trunk_port 2\r\n"
    "Create a new vlan whose id is \'14\' and add to it a trunk port (not standard\r\n"
    "port). New trunk port id is 2. New port is NOT tagged. CPU is NOT a member\r\n"
    "of this Vlan.\r\n",
    "",
    {BIT(4) | BIT(6) | BIT(14) | BIT(16) | BIT(18) |
     BIT(19) | BIT(26) | BIT(29)},
    (unsigned int)5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_EXCLUDE_NO_PORT_ID,
    "exclude_no_port",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select whether to exclude indicated port from indicated multicast group\r\n"
    "  implied by mac address and vlan id.",
    "Examples:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_one_port standard_port entry_id 0\r\n"
    "  vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1 unit 0 device 1\r\n"
    "  port 2 mac_address dead1234dead\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. From the multicast group\r\n"
    "implied by mac address \'dead1234dead\' and vlan id \'14\', take out the standard\r\n"
    "port no. 2 on device 1 on unit 0.\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address dead1234dead\r\n"
    "  vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. From the multicast group\r\n"
    "implied by mac address \'dead1234dead\' and vlan id \'14\', take out NO port.\r\n",
    "",
    {0,
    BIT(9)},
    (unsigned int)4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_EXCLUDE_ONE_PORT_ID,
    "exclude_one_port",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select whether to exclude indicated port from indicated multicast group\r\n"
    "  implied by mac address and vlan id.",
    "Examples:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_one_port standard_port entry_id 0\r\n"
    "  vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1 unit 0 device 1\r\n"
    "  port 2 mac_address dead1234dead\r\n"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. From the multicast group\r\n"
    "implied by mac address \'dead1234dead\' and vlan id \'14\', take out the standard\r\n"
    "port no. 2 on device 1 on unit 0.\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address dead1234dead\r\n"
    "  vlan_id 14 user_priority 2 drop_precedence 1 traffic_class 1"
    "See full explanation on help of \'add\' and \'tx_by_vlan\'. From the multicast group\r\n"
    "implied by mac address \'dead1234dead\' and vlan id \'14\', take out NO port.\r\n",
    "",
    {BIT(10) | BIT(19)},
    (unsigned int)4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TX_BY_VLAN_ID,
    "tx_by_vlan",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select tx_by_vlan command type for transmission of this message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 mac_address\r\n"
    "  dead1234dead vlan_id 14\r\n"
    "Results in adding a transmit message into the table at next available index.\r\n"
    "The message is of \'tx_by_vlan\' type which means the \'mac address/vlan id\'\r\n"
    "conbination is going to be used to identify the port to transmit to (or\r\n"
    "the \'vidx\', in case of multicast). Identifier of entry is \'0\'.\r\n"
    "Mac address is \'dead1234dead\' and vlan id is \'14\'.\r\n"
    "No port is to be excluded from the list indicated by this mac/vlan_id\r\n"
    "combination\r\n"
    "All other parameters get default values:\r\n"
    "IEEE 802.1p User Priority is 3, Drop precedence is 0, Egress tx class queue is\r\n"
    "3. Message is of standard-format and crc is appended. Message is to be\r\n"
    "transmitted every 4 ms. Data to transmit is of 4 bytes. Data is: 0xAABBCCDD.\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(9)},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TX_BY_IF_ID,
    "tx_by_if",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select tx by interface index command type for transmission of this message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_if entry_id 0 logical_if_index 8\r\n"
    "Results in adding a transmit message into the table at next available index.\r\n"
    "The message is of \'tx_by_if\' type which means interface index is going to\r\n"
    "be used to identify the port(s) to transmit to. Identifier of entry is \'0\'.\r\n"
    "Message is transmitted to all ports related to logical if no. 8\r\n"
    "All other parameters get default values:\r\n"
    "IEEE 802.1p User Priority is 3, Drop precedence is 0, Egress tx class queue is\r\n"
    "3. Message is of standard-format and crc is appended. Message is to be\r\n"
    "transmitted every 4 ms. Data to transmit is of 4 bytes. Data is: 0xAABBCCDD.\r\n",
    "",
    {0,BIT(11)},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TX_BY_LPORT_ID,
    "tx_by_lport",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select tx by logical port command type for transmission of this message.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_lport trunk_port entry_id 0 id_of_trunk_port 2\r\n"
    "  vlan_id 12\r\n"
    "Results in adding a transmit message into the table at next available index.\r\n"
    "The message is of \'tx_by_lport\' type which means logical port is going to\r\n"
    "be used to identify the port(s) to transmit to. Identifier of entry is \'0\'.\r\n"
    "Message is transmitted to all ports related to trunk port identified as no. 2\r\n"
    "and vlan no. 12 (vidx)\r\n"
    "All other parameters get default values:\r\n"
    "IEEE 802.1p User Priority is 3, Drop precedence is 0, Egress tx class queue is\r\n"
    "3. Message is of standard-format and crc is appended. Message is to be\r\n"
    "transmitted every 4 ms. Data to transmit is of 4 bytes. Data is: 0xAABBCCDD.\r\n",
    "",
    {BIT(13) | BIT(14)},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Add message to table of messages to tx which are initiated by the user.",
    "Example:\r\n"
    "  pss user_tx_table add tx_by_vlan exclude_no_port entry_id 0 user_priority 3\r\n"
    "  drop_precedence 0 traffic_class 2 encapsulation regular append_crc on\r\n"
    "  num_packets_in_group 2 gap 3 wait_between_groups 4 data 0x1234abcd 0x89bead01\r\n"
    "  data_len 8 mac_address dead1234dead vlan_id 14\r\n"
    "Results in adding a transmit message into the table at next available index.\r\n"
    "The message is of \'tx_by_vlan\' type which means the \'mac address/vlan id\'\r\n"
    "conbination is going to be used to identify the port to transmit to (or\r\n"
    "the \'vidx\', in case of multicast).\r\n"
    "Identifier of entry is \'0\' and required IEEE 802.1p User Priority is 3\r\n"
    "(range 0-7).\r\n"
    "Drop precedence (\'color\') of message is 0 (range 0-2).\r\n"
    "Egress tx class queue is 2 (range 0-7).\r\n"
    "Specified data is required to be wrapped into a regular-format (non control)\r\n"
    "message and crc is to be added. \'Regular\' messages are subject to egress\r\n"
    "bridge filtering due to VLAN egress filtering or Spanning Tree filtering.\r\n"
    "Message is to be transmitted in groups of two with a time gap of 3 ms between\r\n"
    "messages and 4 ms between groups.\r\n"
    "Data to transmit is of length 8 bytes.\r\n"
    "Data starts with the following bytes (hex format): 0x1234abcd89bead01.\r\n"
    "The mac address and vlan id to use for finding target ports is \'0xDEAD1234DEAD\'\r\n"
    "and \'14\' respectively.\r\n"
    "No port is to be excluded from the list indicated by this mac/vlan_id\r\n"
    "combination\r\n",
    "",
    {BIT(10) | BIT(13) | BIT(14) | BIT(19),
    BIT(11) | BIT(9)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display message from table of messages to tx which are initiated by the user.",
    "Example:\r\n"
    "  pss user_tx_table get index 0\r\n"
    "Results in display of transmit message which the user placed in the table\r\n"
    "at index \'0\' (first message). User either gets message or an indication\r\n"
    "that there is no message to read at that offset in the table.\r\n",
    "",
    {BIT(8)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_ADD_PORT_ID,
    "add_port",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Add a port to an existing VLAN or multicast group",
    "Example:\r\n"
    "  pss vlan add_port standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "  port_tagged on\r\n"
    "Add to existing vlan whose id is \'14\' a standard port (not trunk port).\r\n"
    "New port is on unit 0, device 1, port 2. New port is tagged.\r\n",
    "",
    {BIT(4) | BIT(6) | BIT(15) | BIT(17)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_CREATE_NEW_ID,
    "create_new",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Create new VLAN or multicast group and add to it at least one port.",
    "Example:\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "  port_tagged on cpu_included on\r\n"
    "Create a new Vlan whose id is \'14\' and add to it a standard port (not trunk\r\n"
    "port). New port is on unit 0, device 1, port 2. New port is tagged. CPU is also\r\n"
    "a member of this Vlan.\r\n",
    "",
    {BIT(5) | BIT(7) | BIT(16) | BIT(18)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_GAP_BETWEEN_CYCLES_ID,
    "gap_between_cycles",
    (PARAM_VAL_RULES *)&Gap_between_cycles_vals[0],
    (sizeof(Gap_between_cycles_vals) / sizeof(Gap_between_cycles_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gap_between_cycles_default,
    0,0,0,0,0,0,0,
    "Set number of ms to wait between transmission of two consecutive cycles.",
    "Example:\r\n"
    "  pss user_tx_table set_tx_mode num_loop_cycles 3 tot_num_packets 20\r\n"
    "  gap_between_cycles 30\r\n"
    "Results in setting for 3 transmit cycles with a time gap of 30 ms between\r\n"
    "any two consecutive cycles and so that the total number of packets is 20,\r\n"
    "overriding some parameters specified per each table entry.\r\n",
    "",
    {0,
    BIT(21)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TOT_NUM_PACKETS_ID,
    "tot_num_packets",
    (PARAM_VAL_RULES *)&Tot_num_packets_vals[0],
    (sizeof(Tot_num_packets_vals) / sizeof(Tot_num_packets_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Tot_num_packets_default,
    0,0,0,0,0,0,0,
    "Set total number of packets to transmit next activation.",
    "Example:\r\n"
    "  pss user_tx_table set_tx_mode num_loop_cycles 3 tot_num_packets 20\r\n"
    "  gap_between_cycles 30\r\n"
    "Results in setting for 3 transmit cycles with a time gap of 30 ms between\r\n"
    "any two consecutive cycles and so that the total number of packets is 20,\r\n"
    "overriding some parameters specified per each table entry. The parameter\r\n"
    "\'tot_num_packets\' may also be set to \'as_specified_per_entry\' in which\r\n"
    "case number of packets to transmit is as specified in \'num_packets_in_group\'\r\n"
    "while going through the whole table as per \'num_loop_cycles\'\r\n",
    "",
    {0,
    BIT(21)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_NUM_TX_CYCLES_ID,
    "num_loop_cycles",
    (PARAM_VAL_RULES *)&Num_loop_cycles_vals[0],
    (sizeof(Num_loop_cycles_vals) / sizeof(Num_loop_cycles_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Num_loop_cycles_default,
    0,0,0,0,0,0,0,
    "Set number of cycles of transmit messages to send on next activation.",
    "Example:\r\n"
    "  pss user_tx_table set_tx_mode num_loop_cycles 3 tot_num_packets 20\r\n"
    "  gap_between_cycles 30\r\n"
    "Results in setting for 3 transmit cycles with a time gap of 30 ms between\r\n"
    "any two consecutive cycles and so that the total number of packets is 20,\r\n"
    "overriding some parameters specified per each table entry. The parameter\r\n"
    "\'num_loop_cycles\' may also be set to \'infinite\' in which case transmission\r\n"
    "can only be stopped manually (\'pss user_tx_table stop_tx\')\r\n",
    "",
    {0,
    BIT(21)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_SET_TX_MODE_ID,
    "set_tx_mode",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set transmit parameters for table of transmit messages prepared by the user.",
    "Example:\r\n"
    "  pss user_tx_table set_tx_mode num_loop_cycles 3 tot_num_packets 20\r\n"
    "  gap_between_cycles 30\r\n"
    "Results in setting for 3 transmit cycles with a time gap of 30 ms between\r\n"
    "any two consecutive cycles and so that the total number of packets is 20,\r\n"
    "overriding some parameters specified per each table entry.\r\n",
    "",
    {0,BIT(21)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_TX_ACTION_ID,
    "tx_action",
    (PARAM_VAL_RULES *)&Tx_action_vals[0],
    (sizeof(Tx_action_vals) / sizeof(Tx_action_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Selection required action on table of messages: \'start tx\' or \'stop tx\'",
    "Example:\r\n"
    "  pss user_tx_table tx_action start_tx\r\n"
    "Results in spawning the transmit task and starting transmission of contents\r\n"
    "of table as per parameters set via \'set_tx_mode\' and via \'add\' of specific\r\n"
    "entries.\r\n",
    "",
    {0,
    BIT(21)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_USER_TX_TABLE_ID,
    "user_tx_table",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Handle table of messages to transmit which are initiated by the user.",
    "Example:\r\n"
    "  pss user_tx_table get index 0\r\n"
    "Results in display of transmit message which the user placed in the table\r\n"
    "at index \'0\' (first message). User either gets message or an indication\r\n"
    "that there is no message to read at that offset in the table.\r\n",
    "",
    {BIT(8) | BIT(10) | BIT(13) |
    BIT(14) | BIT(19),
    BIT(21) | BIT(11) | BIT(9)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_NUM_ENTRIES_ID,
    "num_entries",
    (PARAM_VAL_RULES *)&Num_entries_vals[0],
    (sizeof(Buff_size_vals) / sizeof(Buff_size_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set number of buffers in user table of received messages.",
    "Example:\r\n"
    "  pss user_rx_table set_rx_mode num_entries 100\r\n"
    "Results in setting the number of buffers in user table of received messages\r\n"
    "(number of entries in table). In this case, set number of entries to 100.\r\n"
    "The other two parameters (\'buffer_mode\' and \'buff_size\')\r\n"
    "remain unchanged.\r\n"
    "Note that the whole user table of received messages is cleared!\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_BUFF_SIZE_ID,
    "buff_size",
    (PARAM_VAL_RULES *)&Buff_size_vals[0],
    (sizeof(Buff_size_vals) / sizeof(Buff_size_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set size of each buffer in user table of received messages.",
    "Example:\r\n"
    "  pss user_rx_table set_rx_mode buff_size 2000\r\n"
    "Results in setting the size of each buffer in user table of received\r\n"
    "messages. In this case, all buffers are set to a size of 2000 bytes.\r\n"
    "The other two parameters (\'buffer_mode\' and \'num_entries\')\r\n"
    "remain unchanged.\r\n"
    "Note that the whole user table of received messages is cleared!\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_BUFFER_MODE_ID,
    "buffer_mode",
    (PARAM_VAL_RULES *)&Buffer_mode_vals[0],
    (sizeof(Buffer_mode_vals) / sizeof(Buffer_mode_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set mode of operating user table of received messages: Cyclic or\r\n"
    "  Delete when full (\'fill_once_and_stop\').",
    "Example:\r\n"
    "  pss user_rx_table set_rx_mode buffer_mode fill_once_and_stop\r\n"
    "Results in setting the way user table of received messages should be\r\n"
    "handled. In this case, all buffers should be used untill there is no free\r\n"
    "buffer. After that, messages are dropped until some space is cleared (by\r\n"
    "the user). The other two parameters (\'buff_size\' and \'num_entries\')\r\n"
    "remain unchanged.\r\n"
    "Note that the whole user table of received messages is cleared!\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_SET_RX_MODE_ID,
    "set_rx_mode",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set parameters of handling the table of received (captured) messages.",
    "Example:\r\n"
    "  pss user_rx_table set_rx_mode buffer_mode cyclic buff_size 2000 num_entries 20\r\n"
    "Results in setting (display) of the  3 receive parameters: \r\n"
    "  buffer_mode - \'Cyclic\' or \'fill once and stop\'\r\n"
    "  buff_size   - Packet buffer size (assigned to each entry)\r\n"
    "  num_entries - Number of entries in user rx table\r\n"
    "Note that the whole user table of received messages is cleared!\r\n",
    "",
    {BIT(23)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_GET_RX_MODE_ID,
    "get_rx_mode",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get parameters of handling the table of received (captured) messages.",
    "Example:\r\n"
    "  pss user_rx_table get_rx_mode\r\n"
    "Results in getting (display) of the  3 receive parameters: \r\n"
    "  buffer_mode - \'Cyclic\' or \'fill once and stop\'\r\n"
    "  buff_size   - Packet buffer size (assigned to each entry)\r\n"
    "  num_entries - Number of entries in user rx table\r\n",
    "",
    {0,
    BIT(22)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_RX_ACTION_ID,
    "rx_action",
    (PARAM_VAL_RULES *)&Rx_action_vals[0],
    (sizeof(Rx_action_vals) / sizeof(Rx_action_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Selection of required action on table of received messages:\r\n"
    "  \'start_rx_capture\' or \'stop_rx_capture\'",
    "Example:\r\n"
    "  pss user_rx_table rx_action start_rx_capture\r\n"
    "Results in allowing messages captured by the packet processor to go into\r\n"
    "user table of received messages. Entries are accessible via, say, \'get\'.\r\n",
    "",
    {0,BIT(22)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_CLEAR_RX_ID,
    "clear_all",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clear all messages on table of received messages",
    "Example:\r\n"
    "  pss user_rx_table clear_all\r\n"
    "Results in clearing all messages, captured by the packet processor, which are\r\n"
    "in user table of received messages.\r\n",
    "",
    {0,
    BIT(22)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_GET_FIRST_RX_ID,
    "get_first",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display first message on table of received messages",
    "Example:\r\n"
    "  pss user_rx_table get_first\r\n"
    "Results in displaying first message, captured by the packet processor, which is\r\n"
    "in user table of received messages. This action fails if table is empty.\r\n",
    "",
    {0,
    BIT(22)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_GET_NEXT_RX_ID,
    "get_next",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display next message on table of received messages",
    "Example:\r\n"
    "  pss user_rx_table get_next\r\n"
    "Results in displaying next message, captured by the packet processor, which is\r\n"
    "in user table of received messages. This action fails if all messages have\r\n"
    "already been read. To restart a reading cycle, apply \'get_first\'.\r\n",
    "",
    {0,
    BIT(22)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_USER_RX_TABLE_ID,
    "user_rx_table",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Handle table of received messages captured as per user specifications.",
    "Example:\r\n"
    "  pss user_rx_table rx_action start_rx_capture\r\n"
    "Results in allowing messages captured by the packet processor to go into\r\n"
    "user table of received messages. Entries are accessible via, say, \'get_next\'.\r\n",
    "",
    {BIT(23),
    BIT(22)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_MULTICAST_ID,
    "multicast",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send mac multicast related command to PSS.",
    "Examples:\r\n"
    "  pss multicast create_new standard_port vlan_id 14 mac_addr \'beeb1234beeb\' unit 0\r\n"
    "  device 1 port 2 cpu_included on\r\n",
    "",
    {BIT(6) | BIT(7) | BIT(17) | BIT(18)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_SOURCE_MAC_ID,
    "source_addr",
    (PARAM_VAL_RULES *)&Mac_addr_vals[0],
    (sizeof(Mac_addr_vals) / sizeof(Mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select source address of da-sa pair of MAC address identifiers.",
    "Example:\r\n"
    "  pss bridge general_commands set_mac_da_sa_trap dest_addr 1234ABCD5678\r\n"
    "  source_addr DEEDBEEFDEED\r\n"
    "Results in setting a receive trap on all messages with the pair -\r\n"
    "[destination address 0x1234ABCD5678 ; source address 0xDEEDBEEFDEED]."
    "All messages with this pair will be monitored by host and matrix group counters\r\n",
    "",
    {BIT(20)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DEST_MAC_ID,
    "dest_addr",
    (PARAM_VAL_RULES *)&Mac_addr_vals[0],
    (sizeof(Mac_addr_vals) / sizeof(Mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select destination address of da-sa pair of MAC address identifiers.",
    "Example:\r\n"
    "  pss bridge general_commands set_mac_da_sa_trap dest_addr 1234ABCD5678\r\n"
    "  source_addr DEEDBEEFDEED\r\n"
    "Results in setting a receive trap on all messages with the pair -\r\n"
    "[destination address 0x1234ABCD5678 ; source address 0xDEEDBEEFDEED]."
    "All messages with this pair will be monitored by host and matrix group counters\r\n",
    "",
    {BIT(20) | BIT(22) | BIT(27)},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_BRIDGE_GEN_CMNDS_ID,
    "general_commands",
    (PARAM_VAL_RULES *)&Bridge_general_commands_vals[0],
    (sizeof(Bridge_general_commands_vals) / sizeof(Bridge_general_commands_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out some general utilities on bridge-related section of PSS.",
    "Example:\r\n"
    "  pss bridge general_commands set_mac_da_sa_trap dest_addr 1234ABCD5678\r\n"
    "  source_addr DEEDBEEFDEED\r\n"
    "Results in setting a receive trap on all messages with the pair -\r\n"
    "[destination address 0x1234ABCD5678 ; source address 0xDEEDBEEFDEED]."
    "All messages with this pair will be monitored by host and matrix group counters\r\n",
    "",
    {BIT(20)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_BRIDGE_ID,
    "bridge",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out bridge-related commands in PSS.",
    "Example:\r\n"
    "  pss bridge general_commands set_mac_da_sa_trap dest_addr 1234ABCD5678\r\n"
    "  source_addr DEEDBEEFDEED\r\n"
    "Results in setting the pair -\r\n"
    "[destination address 0x1234ABCD5678 ; source address 0xDEEDBEEFDEED]."
    "All messages with this pair will be monitored by host and matrix group counters\r\n",
    "",
    {BIT(20) | BIT(21) | BIT(22) | BIT(24) | BIT(27)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_VLAN_ID,
    "vlan",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send VLAN related command to PSS.",
    "Examples:\r\n"
    "  pss vlan select_learning_mode individual\r\n"
    "or\r\n"
    "  pss vlan create_new standard_port vlan_id 14 unit 0 device 1 port 2\r\n"
    "  port_tagged on cpu_included on\r\n",
    "",
    {BIT(3) | BIT(4) | BIT(5) | BIT(15) | BIT(16)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_SEND_CMD_ID,
    "cmd",
    (PARAM_VAL_RULES *)&Pss_cmd[0],
    (sizeof(Pss_cmd) / sizeof(Pss_cmd[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send raw ASCII command to PSS.",
    "Examples:\r\n"
    "  pss cmd \"brgSetVlanPortsTblEntry\" fields \"1 14 0 0 1 2 1 0\"\r\n"
    "or\r\n"
    "  pss cmd \"gfdbSetMacVlMode 1\" fields \"\"\r\n"
    "or\r\n"
    "  pss cmd \"brgSetMcEgPortsTblEntry\" fields \"1 14 \'deed1234beeb\' 0 0 1 5 0\"\r\n"
    " Notes:\r\n"
    "1. Use single quote instead of double quote for text within free text field\r\n"
    "2. \'fields\' parameters is optional.",
    "",
    {BIT(2)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_CMD_FIELDS_ID,
    "fields",
    (PARAM_VAL_RULES *)&Pss_cmd[0],
    (sizeof(Pss_cmd) / sizeof(Pss_cmd[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Update_pss_fields_default,
    0,0,0,0,0,0,0,
    "The \'fields\' part of this PSS command.",
     "Examples:\r\n"
    "  pss cmd \"brgSetVlanPortsTblEntry\" fields \"1 14 0 0 1 2 1 0\"\r\n"
    "\'fields\' parameters is optional.",
    "",
    {BIT(2)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_SET_DEFAULT_VLAN_ID,
    "set_default_vlan",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select default VLAN to be applied to received packets on specified port.",
    "Example:\r\n"
    "  pss port_utils set_default_vlan standard_port unit 3 device 2 port 1 vlan_id 5\r\n"
    "Results in setting the default VLAN of standard (non-trunk) port 1, on device 2\r\n"
    "on unit 3, to be 5. This means that when an untagged packet is received on port 1\r\n"
    "(unit 3 device 2) it is assigned, virtually, to VLAN 5.",
    "",
    { BIT(25) | BIT(26),
      0,
      0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_MAC_COUNTERS_DISP_FORMAT_ID,
    "display_format",
    (PARAM_VAL_RULES *)&Pss_display_format_vals[0],
    (sizeof(Pss_display_format_vals) / sizeof(Pss_display_format_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_display_format_default,
    0,0,0,0,0,0,0,
    "Select display format of MAC counters.",
    "Example:\r\n"
    "  pss port_utils display_all_mac_counters standard_port unit 3 device 2 port 1\r\n"
    "  display_format short\r\n"
    "Results in displaying all MAC counters of standard (non-trunk) port 1, on device 2\r\n"
    "on unit 3. Display format is short: name of counter followed by value. Selecting long\r\n"
    "format results, in addition, in displaying explanation on each counter.\r\n",
    "",
    { BIT(28) | BIT(29),
      0,
      0},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_CLEAR_AFTER_READ_ID,
    "clear_after_read",
    (PARAM_VAL_RULES *)&Pss_clear_after_read_vals[0],
    (sizeof(Pss_clear_after_read_vals) / sizeof(Pss_clear_after_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pss_clear_after_read_default,
    0,0,0,0,0,0,0,
    "Select mode of handling counters: Restart after every reading or accumulative.",
    "Example:\r\n"
    "  pss port_utils display_all_mac_counters standard_port unit 3 device 2 port 1\r\n"
    "  clear_after_read off\r\n"
    "Results in displaying all MAC counters of standard (non-trunk) port 1, on device 2\r\n"
    "on unit 3. Display format is short: name of counter followed by value. Counters will\r\n"
    "NOT be cleared after reading so consecutive readings will be accumulative.\r\n"
    "NOTE that accumulation restarts when changing monitored port..\r\n",
    "",
    { BIT(28) | BIT(29),
      0,
      0},
    (unsigned int)LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_DISP_ALL_MAC_COUNTERS_ID,
    "display_all_mac_counters",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display all MAC counters relating to Ethernet activity on specified port.",
    "Example:\r\n"
    "  pss port_utils display_all_mac_counters standard_port unit 3 device 2 port 1\r\n"
    "  display_format short\r\n"
    "Results in displaying all MAC counters of standard (non-trunk) port 1, on device 2\r\n"
    "on unit 3. Display format is short: name of counter followed by value.",
    "",
    { BIT(28) | BIT(29),
      0,
      0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PORT_UTILS_ID,
    "port_utils",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out port-related commands in PSS.",
    "Example:\r\n"
    "  pss port_utils phy read_reg 0x1 port 1\r\n"
    "Results in reading from device-0 unit-0 port-1 PHY register at offset 0x1.\r\n"
    "  The PHY accessing is via the Serial Management Interface.\n\r",
    "",
    { BIT(25) | BIT(26) | BIT(28) | BIT(29),
      0,
      BIT(4) | BIT(5) | BIT(6)  | BIT(7) | BIT(8)},
      /*
       * 4-5 - PHY
       * 6-7 - PORT CTRL
       */
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PORT_CTRL_ID,
    "ctrl",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out port-control commands in PSS.",
    "Example:\r\n"
    "  pss port_utils ctrl force_link_pass on port 1 device 0\r\n"
    "Results (device 0, unit 0) forcing port 1 to be in pass state (link up).\r\n",
    "",
    { 0,
      0,
      BIT(6) | BIT(7) | BIT(8) },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PORT_LOOPBACK_ID,
    "loopback",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set link in loopback.",
    "Example:\r\n"
    "  pss port_utils ctrl loopback port 1 device 0 unit 0\r\n"
    "Results (device 0, unit 0) link in loopback.\r\n",
    "",
    { 0,
      0,
      BIT(7)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PORT_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get and print the port status.",
    "Example:\r\n"
    "  pss port_utils ctrl port 1 device 0 unit 0 status\r\n"
    "Results (device 0, unit 0) get and print port 1 status.\r\n",
    "",
    { 0,
      0,
      BIT(8)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PORT_FORCE_LINK_PASS_ID,
    "force_link_pass",
    (PARAM_VAL_RULES *)&Pss_port_force_link_pass_vals[0],
    (sizeof(Pss_port_force_link_pass_vals) / sizeof(Pss_port_force_link_pass_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Forcing specific link to be in PASS state (link always up).",
    "Example:\r\n"
    "  pss port_utils ctrl force_link_pass on port 1 device 0\r\n"
    "Results (device 0, unit 0) forcing port 1 to be in pass state (link up).\r\n",
    "",
    { 0,
      0,
      BIT(6)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PHY_ID,
    "phy",
    (PARAM_VAL_RULES *)&Pss_empty_vals[0],
    (sizeof(Pss_empty_vals) / sizeof(Pss_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out port-phy-related commands in PSS.\n\r"
    "  Using Prestera as interface to teh PHY - via SMI.",
    "Example:\r\n"
    "  pss port_utils phy read_reg 0x1 port 1\r\n"
    "Results in reading from device-0 unit-0 port-1 PHY register at offset 0x1.\r\n"
    "  The PHY accessing is via the Serial Management Interface.\n\r",
    "",
    { 0,
      0,
      BIT(4) | BIT(5)},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_READ_ID,
    "read_reg",
    (PARAM_VAL_RULES *)&Pss_phy_reg_offset_vals[0],
    (sizeof(Pss_phy_reg_offset_vals) / sizeof(Pss_phy_reg_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset of register at the PHY.",
    "Example:\r\n"
    "  pss port_utils phy read_reg 0x1 port 1\r\n"
    "Results in reading from device-0 unit-0 port-1 PHY register at offset 0x1.\r\n"
    "  The PHY accessing is via the Serial Management Interface.\n\r",
    "",
    { 0,
      0,
      BIT(4)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_WRITE_ID,
    "write_reg",
    (PARAM_VAL_RULES *)&Pss_phy_reg_offset_vals[0],
    (sizeof(Pss_phy_reg_offset_vals) / sizeof(Pss_phy_reg_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset of register at the PHY.",
    "Example:\r\n"
    "  pss port_utils phy write_reg 0x1 data 0x12 port 1\r\n"
    "Results in writing to device-0 unit-0 port-1 PHY register at offset 0x1 - data 0x12.\r\n"
    "  The PHY accessing is via the Serial Management Interface.\n\r",
    "",
    { 0,
      0,
      BIT(5)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PHY_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Pss_phy_data_vals[0],
    (sizeof(Pss_phy_data_vals) / sizeof(Pss_phy_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "16 bit data to write to the PHY-port register.",
    "Example:\r\n"
    "  pss port_utils phy write_reg 0x1 data 0x12 port 1\r\n"
    "Results in writing to device-0 unit-0 port-1 PHY register at offset 0x1 - data 0x12.\r\n"
    "  The PHY accessing is via the Serial Management Interface.\n\r",
    "",
    { 0,
      0,
      BIT(5)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PSS_PHY_DATA_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&Pss_phy_data_vals[0],
    (sizeof(Pss_phy_data_vals) / sizeof(Pss_phy_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If apear, do read/modify/write with this mask.",
    "Example:\r\n"
    "  pss port_utils phy write_reg 0x1 data 0x12 port 1 mask 0xFF\r\n"
    "Results in reading from device-0 unit-0 port-1 PHY register at offset 0x1\n\r"
    "  the value there. Than set the 8 lower bit to the data 0x12.\r\n"
    "  Finally, writing this value to the PHY.\n\r",
    "",
    { 0,
      0,
      BIT(5)},
    LAST_ORDINAL,
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
