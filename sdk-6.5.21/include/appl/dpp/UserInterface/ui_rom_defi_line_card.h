/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_LINE_CARD_INCLUDED
/* { */
#define UI_ROM_DEFI_LINE_CARD_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <ui_pure_defi_line_card.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>

/*
 * UI definition{
 */
/********************************************************
*NAME
*  Packet_size_vals
*TYPE: BUFFER
*DATE: 29/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "PACKET_SIZE" IN THE CONTEXT OF
*  SUBJECT 'LINE_CARD' AND 'UTILITIES'.
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
     Packet_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        2000,32,
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
*  Linecard_io_pad_vals
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
     Linecard_io_pad_vals[]
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
*  Linecard_ftdll_parameter_vals
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
     Linecard_ftdll_parameter_vals[]
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
*  Linecard_dfcdl_parameter_vals
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
     Linecard_dfcdl_parameter_vals[]
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
*  Pattern_data_vals
*TYPE: BUFFER
*DATE: 29/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PATTERN_DATA" IN THE
*  CONTEXT OF "LINE_CARD UTILITIES PACKET_INJECT_TEST".
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
     Pattern_data_vals[]
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
/********************************************************
*NAME
*  Num_packets_vals
*TYPE: BUFFER
*DATE: 27/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "NUM_PACKETS" IN THE CONTEXT OF
*  SUBJECT 'LINE_CARD' AND 'UTILITIES'.
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
     Num_packets_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "infinite",
    {
      {
        NUM_PACKETS_INFINITE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    No limit on number of packets (infinite).",
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
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100000,1,
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
*  Packet_size_default
*TYPE: BUFFER
*DATE: 27/JULT/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "PACKET_SIZE"  IN THE CONTEXT
*  OF SUBJECT 'LINE_CARD' AND 'UTILITIES'.
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
     Packet_size_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)125
  }
}
#endif
;



EXTERN CONST
   PARAM_VAL_RULES
     I2c_max_read_buff[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        31,0,
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
     Byte_size[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        0xff,0,
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
     I2c_unit[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        0x7f,0,
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
     I2c_internal_addr[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        0xff,0,
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
     Trimmer_id[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "trim_3_3v",
    {
      {
        LINE_CARD_TRIM_3_3,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Controls 3.3V.",
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
    "trim_2_5v_fap",
    {
      {
        LINE_CARD_TRIM_2_5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Controls FAP's 2.5V.",
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
    "trim_2_5v_uplink",
    {
      {
        LINE_CARD_TRIM_2_5_UPLINK,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Controls Uplink's 2.5V.",
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
    "trim_1_5v",
    {
      {
        LINE_CARD_TRIM_1_5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Controls FAP's 1.5V.",
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
*  Num_packets_default
*TYPE: BUFFER
*DATE: 27/JULT/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "NUM_PACKETS"  IN THE CONTEXT
*  OF SUBJECT 'LINE_CARD' AND 'UTILITIES'.
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
     Num_packets_default
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
*  General_on_off_vals
*TYPE: BUFFER
*DATE: 27/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO ANY PARAMETER OF THE "ON/OFF".
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
     General_on_off_vals[]
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
        (long)"    Activate option (general).",
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
        (long)"    Disable option (general).",
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
*  General_enable_disable_vals
*TYPE: BUFFER
*DATE: 27/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO ANY PARAMETER OF THE "ON/OFF".
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
     General_enable_disable_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enable",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Activate option (general).",
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
    "disable",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Disable option (general).",
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
*  General_on_off_default_off
*TYPE: BUFFER
*DATE: 27/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO A
*  GENERAL ON/OFF PARAMETER. DEFAULT IS "OFF"
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
     General_on_off_default_off
#ifdef INIT
   =
{
  VAL_SYMBOL,
  OFF_EQUIVALENT,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"off"
  }
}
#endif
;
/********************************************************
*NAME
*  General_on_off_default_on
*TYPE: BUFFER
*DATE: 27/JULY/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO A
*  GENERAL ON/OFF PARAMETER. DEFAULT IS "ON"
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
     General_on_off_default_on
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
*  Line_card_len_mem_vals
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LEN" IN THE CONTEXT
*  OF SUBJECT 'MEM'.
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
     Line_card_len_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,1,
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
*  Line_card_len_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "LEN"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
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
     Line_card_len_mem_default
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
*  Line_card_val_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "VAL" WITHIN  "VAL_READ" WITHIN 'MEM' SUBJECT.
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
     Line_card_val_mem_default
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
*  Line_card_mask_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "MASK" WITHIN  "VAL_READ" WITHIN 'MEM' SUBJECT.
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
     Line_card_mask_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)(-1)
  }
}
#endif
;
/********************************************************
*NAME
*  Line_card_operation_mem_vals
*TYPE: BUFFER
*DATE: 06/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "OPERATION" IN THE
*  CONTEXT OF SUBJECT 'MEM' (AND 'VAL_READ').
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
     Line_card_operation_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "equal",
    {
      {
        EQUAL_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Mask read memory contents and compare until specified value\r\n"
              "    is EQUAL to memory contents (or timeout has expired).",
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
    "not_equal",
    {
      {
        NOT_EQUAL_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Mask read memory contents and compare until specified value\r\n"
              "    is NOT EQUAL to memory contents (or timeout has expired).",
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
*  Linecard_c2c_interface_vals
*TYPE: BUFFER
*DATE: 06/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "OPERATION" IN THE
*  CONTEXT OF SUBJECT 'MEM' (AND 'VAL_READ').
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
     Linecard_c2c_interface_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "narrow",
    {
      {
        NARROW_C2C_INTERFACE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Narrow C2C interface.",
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
    "wide",
    {
      {
        WIDE_C2C_INTERFACE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Wide C2C interface.",
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
*  Line_card_operation_mem_default
*TYPE: BUFFER
*DATE: 06/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "operation" IN THE CONTEXT OF SUBJECT 'MEM'
*  (AND 'VAL_READ').
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
     Line_card_operation_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  EQUAL_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"equal"
  }
}
#endif
;
/********************************************************
*NAME
*  Line_card_timeout_mem_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TIMEOUT" WITHIN 'VAL_READ'
*  IN SUBJECT 'MEM'.
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
     Line_card_timeout_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,1,
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
*  Line_card_timeout_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "TIMEOUT" WITHIN  "VAL_READ" WITHIN 'MEM' SUBJECT.
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
     Line_card_timeout_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1000
  }
}
#endif
;
/********************************************************
*NAME
*  Linecard_clock_synthesizer_oscillator_vals
*TYPE: BUFFER
*DATE: 15/APR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "" IN THE
*  CONTEXT OF SUBJECT 'line_card'.
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
     Linecard_clock_synthesizer_oscillator_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "synthesizer",
    {
      {
        LINE_CARD_SYNTHESIZER,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Clock generator is synthesizer.",
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
    "oscillator",
    {
      {
        LINE_CARD_OSCILLATOR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Clock generator is oscillator.",
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
    "external",
    {
      {
        LINE_CARD_EXTERNAL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Clock generator is external.",
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
*  Linecard_clock_synthesizers_od_vals
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
     Linecard_clock_synthesizers_od_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        LINE_CARD_CLOCK_SYNTHESIZERS_OD_MAX-1,LINE_CARD_CLOCK_SYNTHESIZERS_OD_MIN,
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
*  Linecard_clock_synthesizers_rdw_vals
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
     Linecard_clock_synthesizers_rdw_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        LINE_CARD_CLOCK_SYNTHESIZERS_RDW_MAX-1,0,
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
*  Linecard_clock_synthesizers_vdw_vals
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
     Linecard_clock_synthesizers_vdw_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        LINE_CARD_CLOCK_SYNTHESIZERS_VDW_MAX-1,0,
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
*  Linecard_clock_synthesizer_clk_freq_vals
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
     Linecard_clock_synthesizer_clk_freq_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        199,6,
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
*  Linecard_clock_synthesizers_vals
*TYPE: BUFFER
*DATE: 15/APR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "" IN THE
*  CONTEXT OF SUBJECT 'line_card'.
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
     Linecard_clock_synthesizers_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "core",
    {
      {
        FAP10M_PLL1_CORE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Core-Clock, PLL1.",
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
    "sdram",
    {
      {
        FAP10M_PLL2_SDRAM,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    SDRAM-Clock, PLL2.",
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
    "uplink",
    {
      {
        FAP10M_PLL0_UPLINK,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Uplink-Clock, PLL0.",
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
*  Linecard_reset_milisec_defualt
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
     Linecard_reset_milisec_defualt
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)100
  }
}
#endif
;

/********************************************************
*NAME
*  Linecard_indefinite_num_vals
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
     Linecard_indefinite_num_vals[]
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
*  Linecard_empty_vals
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
     Linecard_empty_vals[]
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
*  Linecard_data_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "Data".
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
     Linecard_data_vals[]
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
*  Linecard_offset_vals
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
     Linecard_offset_vals[]
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
*  Line_card_params
*TYPE: BUFFER
*DATE: 29/Dec/2002
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
     Line_card_params[]
#ifdef INIT
   =
/*
 ******
 * ENTRY -- 0
 *BIT(1) - write
 *BIT(2) - read
 *BIT(4-5) - clock synthesizer
 *BIT(6) - val_read
 *BIT(7) - erase.
 *BIT(12) -
 *BIT(13) - PP
 *BIT(14) - C2C FAP dfcdl load
 *BIT(15) - C2C PP dfcdl load
 *BIT(16) - EDDR/IDDR FAP dfcdl load
 *BIT(17) - IDDR PP dfcdl load
 *BIT(20) - FPGA download
 ******
 * ENTRY -- 1
 *BIT(11-12) - utilities.
 */
{
  {
    PARAM_LINE_CARD_CLEAR_AFTER_TREAD_ID,
    "clear_after_read",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clear error counters of C2C (uplink) after display (restart).",
    "Examples:\r\n"
    "  line_card utilities display_uplink_err_counters clear_after_read\r\n"
    "Results in displaying uplink error counters, as collected by reading\n\r"
    "\'Miscellaneous Interrupt Cause Register\' and then clearing local memory\r\n"
    "image of those counters. (If \'clear_after_read\' is omitted the counting\r\n"
    "is accumulative.",
    "",
    { 0,
      BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_DISP_ERR_COUNTERS_ID,
    "display_uplink_err_counters",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display error counters of C2C (uplink) as collected from Int Cause Reg.",
    "Examples:\r\n"
    "  line_card utilities display_uplink_err_counters clear_after_read\r\n"
    "Results in displaying uplink error counters, as collected by reading\n\r"
    "\'Miscellaneous Interrupt Cause Register\' and then clearing local memory\r\n"
    "image of those counters. (If \'clear_after_read\' is omitted the counting\r\n"
    "is accumulative.",
    "",
    { 0,
      BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PACKET_SIZE_ID,
    "packet_size",
    (PARAM_VAL_RULES *)&Packet_size_vals[0],
    (sizeof(Packet_size_vals) / sizeof(Packet_size_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Packet_size_default,
    0,0,0,0,0,0,0,
    "Size of each packet in bytes (including header) to send.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test\r\n"
    "  repeated_pattern packet_size 300\r\n"
    "Results in activating \'packet inject\' test: Injecting 1 packet\r\n"
    "of 300 bytes (including headers).",
    "",
    {0,
     BIT(12) | BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PACKET_INJECT_NUM_PKTS_ID,
    "num_packets",
    (PARAM_VAL_RULES *)&Num_packets_vals[0],
    (sizeof(Num_packets_vals) / sizeof(Num_packets_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Num_packets_default,
    0,0,0,0,0,0,0,
    "Number of packets to send \'packet inject test\' on FAP.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test\r\n"
    "  repeated_pattern num_packets infinite\r\n"
    "Results in activating \'packet inject\' test: Injecting packets continuously\r\n"
    "without stopping (unless error is encountered).",
    "",
    {0,
     BIT(12) | BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_TRACE_PRINT_ID,
    "trace_print",
    (PARAM_VAL_RULES *)&General_on_off_vals[0],
    (sizeof(General_on_off_vals) / sizeof(General_on_off_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&General_on_off_default_off,
    0,0,0,0,0,0,0,
    "Activate or deactivate trace printing related to \'packet inject test\' on FAP.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test \r\n"
    "  repeated_pattern trace_print on\r\n"
    "Results in activating \'packet inject\' test: Injecting packets continuously\r\n"
    "while printing every FAP/PP meory access and packet contents. Use ONLY in\r\n"
    "debug stage.",
    "",
    {0,
     BIT(12) | BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PATTERN_DATA_ID,
    "pattern_data",
    (PARAM_VAL_RULES *)&Pattern_data_vals[0],
    (sizeof(Pattern_data_vals) / sizeof(Pattern_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Valus of pattern write into data section of packet.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test repeated_pattern\r\n"
    "  pattern_data 0xAA551122 0xFF00FF00 0xAA551122 0xFF00FF00 0x11111111\r\n"
    "  0x88888888 0xAA55AA55 0x55AA55AA\r\n"
    "Results in activating \'packet inject\' test: Injecting one packet of\r\n"
    "default size (125 bytes) with contents of the specified repeated pattern\r\n"
    "of 0xAA551122 0xFF00FF00 0xAA551122 0xFF00FF00 0x11111111 0x88888888\r\n"
    "0xAA55AA55 0x55AA55AA. The pattern is of fixed size of 8 long words. If less\r\n"
    "than 8 words are specified, filler words of value \'0xFF00AA55\' are used.\r\n",
    "",
    {0,
     BIT(12)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_REPEATED_PATTERN_ID,
    "repeated_pattern",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Activate repeated pattern to be put into data section of packet.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test\r\n"
    "  repeated_pattern\r\n"
    "Results in activating \'packet inject\' test: Injecting one packet\r\n"
    "with contents of the default repeated pattern\r\n",
    "",
    {0,
     BIT(12)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_RANDOM_PATTERN_ID,
    "random_pattern",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Activate random pattern to be put into data section of packet.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test\r\n"
    "  random_pattern\r\n"
    "Results in activating \'packet inject\' test: Injecting one packet\r\n"
    "with contents of random bit pattern\r\n",
    "",
    {0,
     BIT(13)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_DISPLAY_ALL_COUNTERS_ID,
    "display_all_counters",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display all counters related to \'packet inject test\' on FAP.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test display_all_counters\r\n"
    "Results in display of all counters related to \'packet inject\' test.",
    "",
    {0,
     BIT(14)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PACKET_INJECT_START_ID,
    "start_test",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Start accepting parameters data for \'packet inject test\' on FAP.",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test\r\n"
    "  repeated_pattern\r\n"
    "Results in activating \'packet inject\' test: Injecting packets continuously\n\r"
    "through FAP.",
    "",
    {0,
     BIT(12) | BIT(13)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PACKET_INJECT_TEST_ID,
    "packet_inject_test",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Actions related to \'packet inject test\' on FAP. Very specific configuration!",
    "Examples:\r\n"
    "  line_card utilities packet_inject_test start_test\r\n"
    "  repeated_pattern\r\n"
    "Results in activating \'packet inject\' test: Injecting packets continuously\n\r"
    "through FAP.",
    "",
    { 0,
      BIT(12) | BIT(13) | BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_UTILS_ID,
    "utilities",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "General utilities for line card.",
    "Examples:\r\n"
    "  line_card utilities display_uplink_err_counters clear_after_read\r\n"
    "Results in displaying uplink error counters, as collected by reading\n\r"
    "\'Miscellaneous Interrupt Cause Register\' and then clearing local memory\r\n"
    "image of those counters",
    "",
    { 0,
      BIT(11) | BIT(12) | BIT(13) | BIT(14)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_FPGA_WRITE_ID,
    "fpga_write",
    (PARAM_VAL_RULES *)&Byte_size[0],
    (sizeof(Byte_size) / sizeof(Byte_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write to FPGA.",
    "Examples:\r\n"
    "  line_card fpga_write 0x14 data 0x23\r\n"
    "  would result in modifying address 0x50000014.  \n\r"
    "  (The Oscillator / Synthesizer Selection)",
    "",
    {0,
     0,
     BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_FPGA_READ_ID,
    "fpga_read",
    (PARAM_VAL_RULES *)&Byte_size[0],
    (sizeof(Byte_size) / sizeof(Byte_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read from FPGA.",
    "Examples:\r\n"
    "  line_card fpga_read 0x14\r\n"
    "  would result in reading address 0x50000014.  \n\r"
    "  (The Oscillator / Synthesizer Selection)",
    "",
    {0,
     0,
     BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_FPGA_WRITE_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Byte_size[0],
    (sizeof(Byte_size) / sizeof(Byte_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The value to be written. Byte size.",
    "Examples:\r\n"
    "  line_card fpga_write 0x14 data 0x23\r\n"
    "  would result in modifying address 0x50000014.  \n\r"
    "  (The Oscillator / Synthesizer Selection)",
    "",
    {0,
     0,
     BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_TRIMMERS_READ_ID,
    "trimmers_read",
    (PARAM_VAL_RULES *)&Trimmer_id[0],
    (sizeof(Trimmer_id) / sizeof(Trimmer_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read from trimmer.",
    "Examples:\r\n"
    "  line_card trimmers_read trim_3_3 trimmer_internal_addr 0x12 nof_bytes 4\r\n"
    "  would result in reading from the trimmer that control 3.3V 4 bytes, starting\n\r"
    "  from offset 0x12.",
    "",
    {0,
     0,
     BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_TRIMMERS_WRITE_ID,
    "trimmers_write",
    (PARAM_VAL_RULES *)&Trimmer_id[0],
    (sizeof(Trimmer_id) / sizeof(Trimmer_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write to trimmer.",
    "Examples:\r\n"
    "  line_card trimmers_write trim_3_3 trimmer_internal_addr 0x12 write_byte 4\r\n"
    "  would result in writing to the trimmer that control 3.3V \n\r"
    "  the value 4 in offset 0x12.",
    "",
    {0,
     0,
     BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_TRIMMERS_INTERNAL_ADDR_ID,
    "trimmer_internal_addr",
    (PARAM_VAL_RULES *)&I2c_internal_addr[0],
    (sizeof(I2c_internal_addr) / sizeof(I2c_internal_addr[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The offset inside the trimmer.",
    "Examples:\r\n"
    "  line_card trimmers_read trim_3_3 trimmer_internal_addr 0x12 nof_bytes 4\r\n"
    "  would result in reading from the trimmer that control 3.3V 4 bytes, starting\n\r"
    "  from offset 0x12.",
    "",
    {0,
     0,
     BIT(1) | BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_TRIMMERS_WRITE_DATA_ID,
    "write_byte",
    (PARAM_VAL_RULES *)&Byte_size[0],
    (sizeof(Byte_size) / sizeof(Byte_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The value to be written. Currentlly, only one byte.",
    "Examples:\r\n"
    "  line_card trimmers_write trim_3_3 trimmer_internal_addr 0x12 write_byte 4\r\n"
    "  would result in writing to the trimmer that control 3.3V \n\r"
    "  the value 4 in offset 0x12.",
    "",
    {0,
     0,
     BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_TRIMMERS_READ_BYTE_NUM_ID,
    "nof_bytes",
    (PARAM_VAL_RULES *)&I2c_max_read_buff[0],
    (sizeof(I2c_max_read_buff) / sizeof(I2c_max_read_buff[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Determines the number of bytes to be written from the I2C device.",
    "Examples:\r\n"
    "  line_card trimmers_read trim_3_3 trimmer_internal_addr 0x12 nof_bytes 4\r\n"
    "  would result in reading from the trimmer that control 3.3V 4 bytes, starting\n\r"
    "  from offset 0x12.",
    "",
    {0,
     0,
     BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_1_I2C_ID,
    "fap10m_1_i2c",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to SOC_SAND_FAP10M-1 via the I2C.",
    "Examples:\r\n"
    "  line_card dev_type fap10m_1_i2c read 0x12\r\n"
    "Results in reading SOC_SAND_FAP10M-1 device on the line-card, via the I2C,\n\r"
    "  at offset 0x12.",
    "",
    { BIT(1) | BIT(2) | BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_1_I2C_ID,
    "pp_1_i2c",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to PP-1 via the I2C.",
    "Examples:\r\n"
    "  line_card dev_type pp_1_i2c read 0x12\r\n"
    "Results in reading PP-1 device on the line-card, via the I2C,\n\r"
    "  at offset 0x12.",
    "",
    { BIT(1) | BIT(2) | BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_1_PCI_ID,
    "pp_1_pci",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to PP-1 via the PCI.",
    "Examples:\r\n"
    "  line_card pp_1_pci read 0x12\r\n"
    "Results in reading PP-1 device on the line-card, via the I2C,\n\r"
    "  at offset 0x12.",
    "",
    { BIT(1) | BIT(2) | BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_1_E2PROM_ID,
    "fap10m_1_e2prom",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to SOC_SAND_FAP10M-1 EEPROM via the I2C.",
    "Examples:\r\n"
    "  line_card dev_type fap10m_1_e2prom read 0x12\r\n"
    "Results in reading SOC_SAND_FAP10M-1-EEPROM on the line-card, via the I2C,\n\r"
    "  at offset 0x12.",
    "",
    {BIT(1) | BIT(2) | BIT(6) | BIT(7)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_1_E2PROM_ID,
    "pp_1_e2prom",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to PP-1 EEPROM via the I2C.",
    "Examples:\r\n"
    "  line_card pp_1_e2prom read 0x12\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    " via the I2C, at offset 0x12.",
    "",
    {BIT(1) | BIT(2) | BIT(6) | BIT(7)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FPGA_DOWNLOAD_ID,
    "fpga_download",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download directly into the FPGA the fpga_linecard rbf file.",
    "Examples:\r\n"
    "  line_card fpga_download\r\n"
    "Results in downloading 'fpga_linecard' rbf file from the downloaing host,\r\n"
    " and load it into the FPGA. if no error is reported, the file will remain\r\n"
    " there untill next reset",
    "",
    {BIT(20)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_ERASE_ID,
    "erase",
    (PARAM_VAL_RULES *)&Linecard_offset_vals[0],
    (sizeof(Linecard_offset_vals) / sizeof(Linecard_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset fo erase (writing 0xFFFFFFFF). Should be 32 bit aligned.",
    "Examples:\r\n"
    "  line_card pp_1_e2prom erase 0x12\r\n"
    "Results in erasing Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    " via the I2C, at offset 0x12 (writing 0xFFFFFFFF).",
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
    PARAM_LINE_CARD_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Linecard_offset_vals[0],
    (sizeof(Linecard_offset_vals) / sizeof(Linecard_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset fo read. Should be 32 bit aligned.",
    "Examples:\r\n"
    "  line_card pp_1_e2prom read 0x12\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    " via the I2C, at offset 0x12.",
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
    PARAM_LINE_CARD_NOF_READ_ID,
    "len",
    (PARAM_VAL_RULES *)&Line_card_len_mem_vals[0],
    (sizeof(Line_card_len_mem_vals) / sizeof(Line_card_len_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Line_card_len_mem_default,
    0,0,0,0,0,0,0,
    "Number of memory access actions to carry out ('read' only).",
    "Examples:\r\n"
    "  line_card pp_1_e2prom read 0x12 len 8\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    "  via the I2C, at offset 0x12 - eight long variable.\n\r"
    "  displaying the result in hex format (default), on the screen",
    "",
    {BIT(1) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_VAL_READ_ID,
    "val_read",
    (PARAM_VAL_RULES *)&Linecard_offset_vals[0],
    (sizeof(Linecard_offset_vals) / sizeof(Linecard_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset fo read. Should be 32 bit aligned.",
    "Examples:\r\n"
    "  line_card pp_1_e2prom val_read 0x12 val 1 mask 0xff timeout 200\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    "  via the I2C, at offset 0x12:\n\r"
    "  - one long variable.\n\r"
    "  - comparing the LS 8 bits with a value of 0x01.\n\r"
    "  - Once a match is found, within 200 microseconds, control returns to the user.\n\r"
    "  - If a match is not found then an indication plus value actually found are displayed\n\r"
    ,
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
    PARAM_LINE_CARD_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&Linecard_indefinite_num_vals[0],
    (sizeof(Linecard_indefinite_num_vals) / sizeof(Linecard_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Line_card_val_mem_default,
    0,0,0,0,0,0,0,
    "Value to compare memory with, using one access method, and indicate\r\n"
    "  whether a match has been found (under timeout).",
    "Examples:\r\n"
    "  line_card pp_1_e2prom val_read 0x12 val 1 mask 0xff timeout 200\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    "  via the I2C, at offset 0x12:\n\r"
    "  - one long variable.\n\r"
    "  - comparing the LS 8 bits with a value of 0x01.\n\r"
    "  - Once a match is found, within 200 microseconds, control returns to the user.\n\r"
    "  - If a match is not found then an indication plus value actually found are displayed\n\r"
    ,
    "",
    {BIT(6)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&Linecard_indefinite_num_vals[0],
    (sizeof(Linecard_indefinite_num_vals) / sizeof(Linecard_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Line_card_mask_mem_default,
    0,0,0,0,0,0,0,
    "Value to mask memory with, then compare, using one access method,\r\n"
    "  and indicate whether a match has been found (under timeout).",
    "Examples:\r\n"
    "  line_card pp_1_e2prom val_read 0x12 val 1 mask 0xff timeout 200\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    "  via the I2C, at offset 0x12:\n\r"
    "  - one long variable.\n\r"
    "  - comparing the LS 8 bits with a value of 0x01.\n\r"
    "  - Once a match is found, within 200 microseconds, control returns to the user.\n\r"
    "  - If a match is not found then an indication plus value actually found are displayed\n\r"
    ,
    "",
    {BIT(6)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_TIMEOUT_ID,
    "timeout",
    (PARAM_VAL_RULES *)&Line_card_timeout_mem_vals[0],
    (sizeof(Line_card_timeout_mem_vals) / sizeof(Line_card_timeout_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Line_card_timeout_mem_default,
    0,0,0,0,0,0,0,
    "Time, in microseconds, to wait for mask + compare, to indicate\r\n"
    "  whether a match has been found.",
    "Examples:\r\n"
    "  line_card pp_1_e2prom val_read 0x12 val 1 mask 0xff timeout 200\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    "  via the I2C, at offset 0x12:\n\r"
    "  - one long variable.\n\r"
    "  - comparing the LS 8 bits with a value of 0x01.\n\r"
    "  - Once a match is found, within 200 microseconds, control returns to the user.\n\r"
    ,
    "  - If a match is not found then an indication plus value actually found are displayed\n\r"
    "",
    {BIT(6)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_OPERATION_ID,
    "operation",
    (PARAM_VAL_RULES *)&Line_card_operation_mem_vals[0],
    (sizeof(Line_card_operation_mem_vals) / sizeof(Line_card_operation_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Line_card_operation_mem_default,
    0,0,0,0,0,0,0,
    "Operation to apply after mask + compare, to indicate\r\n"
    "  whether a match has been found.",
    "Examples:\r\n"
    "  line_card pp_1_e2prom val_read 0x12 val 1 mask 0xff timeout 200\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    "  via the I2C, at offset 0x12:\n\r"
    "  - one long variable.\n\r"
    "  - comparing the LS 8 bits with a value of 0x01.\n\r"
    "  - Once a match is found, within 200 microseconds, control returns to the user.\n\r"
    "  - If a match is not ound then an indication plus value actually found are displayed\n\r"
    ,
    "",
    {BIT(6)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Linecard_offset_vals[0],
    (sizeof(Linecard_offset_vals) / sizeof(Linecard_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset fo write. Should be 32 bit aligned",
    "Examples:\r\n"
    "  line_card pp_1_e2prom write 0x12 data 11\r\n"
    "Results in writing Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    " via the I2C, at offset 0x12 value 11.",
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
    PARAM_LINE_CARD_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears minimal printout is done",
    "Examples:\r\n"
    "  line_card pp_1_e2prom read 0x12 silent\r\n"
    "Results in reading Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    "  via the I2C, at offset 0x12.\n\r"
    "  Minimal printout is done. Output is in Hexadecimal-format.\n\r"
    "  '$' sign is prefix to the value written.",
    "",
    {BIT(2) | BIT(1) | BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Linecard_data_vals[0],
    (sizeof(Linecard_data_vals) / sizeof(Linecard_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "data fo write. 32 bit data.",
    "Examples:\r\n"
    "  line_card pp_1_e2prom write 0x12 data 11\r\n"
    "Results in writing Prestera Packet-Processor-1-EEPROM on the line-card,\n\r"
    " via the I2C, at offset 0x12 value 11.",
    "",
    {BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_1_ID,
    "fap10m_1",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to SOC_SAND_FAP10M.",
    "Examples:\r\n"
    "  line_card fap10m_1 reset\r\n"
    "Results in resetting the SOC_SAND_FAP10M-1 on the board.",
    "",
    {BIT(3) | BIT(4) | BIT(5) | BIT(9) | BIT(10) | BIT(12) | BIT(14) |
     BIT(16) | BIT(19) | BIT(22) | BIT(24) | BIT(25) | BIT(27) | BIT(29),
     0,0,BIT(9) | BIT(10) | BIT(11) | BIT(19)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_1_ID,
    "pp_1",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to PP.",
    "Examples:\r\n"
    "  line_card pp_1 reset\r\n"
    "Results in resetting the PP-1 on the board.",
    "",
    {BIT(3) | BIT(9) | BIT(10) | BIT(13) | BIT(15) | BIT(17) | BIT(18) | BIT(28) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_CHECK_EDDR_ID,
    "eddr_check",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Check SOC_SAND_FAP10M - EDDR, through the PP.",
    "Examples:\r\n"
    "  line_card fap10m_1 eddr_check\r\n"
    "Results in checking the EDDR through simple writes and reads.",
    "",
    { BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_LOAD_EDDR_DFCDL_ID,
    "eddr_dfcdl_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load EDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 eddr_dfcdl_load 0x00007044\r\n"
    "Results in setting EDDR DFCDL values to 0x00007044.",
    "",
    { BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_LOAD_IDDR_DFCDL_ID,
    "iddr_dfcdl_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_dfcdl_load auto_poll qds 10 rx_clk 3 tx_clk 54\r\n"
    "Results in setting IDDR DFCDL values to qds 10 rx_clk 3 tx_clk 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_DFCDL_DQS_ID,
    "dqs_dfcdl",
    (PARAM_VAL_RULES *)&Linecard_dfcdl_parameter_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_dfcdl_load auto_poll qds 10 rx_clk 3 tx_clk 54\r\n"
    "Results in setting IDDR DFCDL values to qds 10 rx_clk 3 tx_clk 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(16) | BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FTDLL_DQS_ID,
    "dqs_ftdll",
    (PARAM_VAL_RULES *)&Linecard_ftdll_parameter_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_dfcdl_load auto_poll qds 10 rx_clk 3 tx_clk 54\r\n"
    "Results in setting IDDR DFCDL values to qds 10 rx_clk 3 tx_clk 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(29)| BIT(18) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_DFCDL_RX_CLK_ID,
    "rx_clk",
    (PARAM_VAL_RULES *)&Linecard_dfcdl_parameter_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_dfcdl_load auto_poll qds 10 rx_clk 3 tx_clk 54\r\n"
    "Results in setting IDDR DFCDL values to qds 10 rx_clk 3 tx_clk 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(16) | BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_DFCDL_TX_CLK_ID,
    "tx_clk",
    (PARAM_VAL_RULES *)&Linecard_dfcdl_parameter_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_dfcdl_load auto_poll qds 10 rx_clk 3 tx_clk 54\r\n"
    "Results in setting IDDR DFCDL values to qds 10 rx_clk 3 tx_clk 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(16) | BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_LOAD_C2C_DFCDL_ID,
    "c2c_dfcdl_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load C2C DFCDL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_dfcdl_load narrow 0x4 wide 0x6\r\n"
    "Results in setting C2C DFCDL values to of narrow 0x4 wide 0x6.",
    "",
    { BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_C2C_DFCDL_NARROW_ID,
    "narrow",
    (PARAM_VAL_RULES *)&Linecard_dfcdl_parameter_vals[0],
    (sizeof(Linecard_dfcdl_parameter_vals) / sizeof(Linecard_dfcdl_parameter_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Narrow value of C2C DFCDL.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_dfcdl_load narrow 0x4 wide 0x6\r\n"
    "Results in setting C2C DFCDL values to of narrow 0x4 wide 0x6.",
    "",
    { BIT(14) | BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_C2C_DFCDL_WIDE_ID,
    "wide",
    (PARAM_VAL_RULES *)&Linecard_dfcdl_parameter_vals[0],
    (sizeof(Linecard_dfcdl_parameter_vals) / sizeof(Linecard_dfcdl_parameter_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Narrow value of C2C DFCDL.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_dfcdl_load narrow 0x4 wide 0x6\r\n"
    "Results in setting C2C DFCDL values to of narrow 0x4 wide 0x6.",
    "",
    { BIT(14) | BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_C2C_FTDLL_NARROW_ID,
    "narrow",
    (PARAM_VAL_RULES *)&Linecard_ftdll_parameter_vals[0],
    (sizeof(Linecard_ftdll_parameter_vals) / sizeof(Linecard_ftdll_parameter_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Narrow value of C2C FTDLL.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_ftdll_load narrow 0x4 wide 0x6\r\n"
    "Results in setting C2C FTDLL values to of narrow 0x4 wide 0x6.",
    "",
    {BIT(27) | BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_C2C_FTDLL_WIDE_ID,
    "wide",
    (PARAM_VAL_RULES *)&Linecard_ftdll_parameter_vals[0],
    (sizeof(Linecard_ftdll_parameter_vals) / sizeof(Linecard_ftdll_parameter_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Narrow value of C2C FTDLL.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_ftdll_load narrow 0x4 wide 0x6\r\n"
    "Results in setting C2C FTDLL values to of narrow 0x4 wide 0x6.",
    "",
    { BIT(27) | BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_LOAD_EDDR_FTDLL_ID,
    "eddr_ftdll_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load EDDR FTDLL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 eddr_ftdll_load 0x00007044\r\n"
    "Results in setting EDDR FTDLL values to 0x00007044.",
    "",
    { BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_LOAD_IDDR_FTDLL_ID,
    "iddr_ftdll_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR FTDLL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_ftdll_load auto_poll qds 10 rx_clk 3 tx_clk 54\r\n"
    "Results in setting IDDR FTDLL values to qds 10 rx_clk 3 tx_clk 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FTDLL_START_BURST_ID,
    "start_burst",
    (PARAM_VAL_RULES *)&Linecard_ftdll_parameter_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR FTDLL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_ftdll_load auto_poll qds 10 start_burst 3 tx_clk 54\r\n"
    "Results in setting IDDR FTDLL values to qds 10 start_burst 3 tx_clk 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(29) | BIT(18) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FTDLL_CLK_OUT_ID,
    "tx_clk",
    (PARAM_VAL_RULES *)&Linecard_ftdll_parameter_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR FTDLL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_ftdll_load auto_poll qds 10 start_burst 3 clk_out 54\r\n"
    "Results in setting IDDR FTDLL values to qds 10 start_burst 3 clk_out 54\n\r"
    "  and than auto poll for errors.",
    "",
    { BIT(29) | BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_LOAD_C2C_FTDLL_ID,
    "c2c_ftdll_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load C2C FTDLL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_ftdll_load narrow 0x4 wide 0x6\r\n"
    "Results in setting C2C FTDLL values to of narrow 0x4 wide 0x6.",
    "",
    { BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_LOAD_C2C_FTDLL_ID,
    "c2c_ftdll_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load C2C FTDLL values.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_ftdll_load narrow 0x4 wide 0x6\r\n"
    "Results in setting C2C FTDLL values to of narrow 0x4 wide 0x6.",
    "",
    { BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID,
    "auto_poll",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Narrow value of C2C DFCDL.",
    "Examples:\r\n"
    "  line_card fap10m_1 c2c_dfcdl_load narrow 0x4 wide 0x6 auto_poll\r\n"
    "Results in setting C2C DFCDL values to of narrow 0x4 wide 0x6, than automatic\n\r"
    "  poll for erros on the C2C and dipaly them to the screen.\n\r"
    "  Press ESC to stop the polling",
    "",
    { BIT(14) | BIT(15) | BIT(16) | BIT(27) | BIT(28) | BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_FAP10M_CHECK_IDDR_ID,
    "iddr_check",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Check SOC_SAND_FAP10M - IDDR.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_check\r\n"
    "Results in checking the IDDR through simple writes and reads.",
    "",
    { BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_LOAD_C2C_DFCDL_ID,
    "c2c_dfcdl_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load C2C DFCDL values.",
    "Examples:\r\n"
    "  line_card pp_1 c2c_dfcdl_load 0x00001041\r\n"
    "Results in setting C2C DFCDL values to 0x00001041.",
    "",
    { BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_LOAD_IDDR_DFCDL_ID,
    "ddr_dfcdl_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card pp_1 iddr_dfcdl_load 0x00001041\r\n"
    "Results in setting IDDR DFCDL values to 0x00001041.",
    "",
    { BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_LOAD_DDR_FTDLL_ID,
    "ddr_ftdll_load",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load IDDR DFCDL values.",
    "Examples:\r\n"
    "  line_card pp_1 ddr_ftdll_load 0x00001041\r\n"
    "Results in setting DDR FTDLL values to 0x00001041.",
    "",
    { BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_PP_CHECK_IDDR_ID,
    "iddr_check",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Check PP - IDDR.",
    "Examples:\r\n"
    "  line_card pp_1 iddr_check\r\n"
    "Results in setting IDDR DFCDL values to 0x00001041.",
    "",
    { BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_C2C_RESET_ID,
    "c2c_reset",
    (PARAM_VAL_RULES *)&Linecard_c2c_interface_vals[0],
    (sizeof(Linecard_c2c_interface_vals) / sizeof(Linecard_c2c_interface_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reset the C2C interface.",
    "Examples:\r\n"
    "  line_card pp_1 c2c_reset wide\r\n"
    "Results in resetting the PP-1 on the board.",
    "",
    { BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_IO_PAD_CALIBRATION_GET_ID,
    "get_io_pad_calibration",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reset the C2C interface.",
    "Examples:\r\n"
    "  line_card pp_1 c2c_reset wide\r\n"
    "Results in resetting the PP-1 on the board.",
    "",
    { 0,0,0,BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_IDDR_PAD_CALIBRATION_ID,
    "iddr_pad_calibration",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the IO pads calibration of the IDDR.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_pad_calibration data_p 11 data_n 11 addr_p 9 addr_n 8\r\n"
    "Results in resetting the Fap10m on the board.",
    "",
    { 0,0,0,BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_EDDR_PAD_CALIBRATION_ID,
    "eddr_pad_calibration",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the IO pads calibration of the EDDR.",
    "Examples:\r\n"
    "  line_card fap10m_1 eddr_pad_calibration data_p 11 data_n 11 addr_p 9 addr_n 8\r\n"
    "Results in resetting the Fap10m on the board.",
    "",
    { 0,0,0,BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_C2C_PAD_CALIBRATION_ID,
    "c2c_pad_calibration",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the IO pads calibration of the C2C.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_pad_calibration data_p 11 data_n 11\r\n"
    "Results in resetting the Fap10m on the board.",
    "",
    { 0,0,0,BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PAD_CALIBRATION_DATA_P_ID,
    "data_p",
    (PARAM_VAL_RULES *)&Linecard_io_pad_vals[0],
    (sizeof(Linecard_io_pad_vals) / sizeof(Linecard_io_pad_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "DATA Positive.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_pad_calibration data_p 11 data_n 11 addr_p 9 addr_n 8\r\n"
    "Results in resetting the Fap10m on the board.",
    "",
    { 0,0,0, BIT(9) | BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PAD_CALIBRATION_DATA_N_ID,
    "data_n",
    (PARAM_VAL_RULES *)&Linecard_io_pad_vals[0],
    (sizeof(Linecard_io_pad_vals) / sizeof(Linecard_io_pad_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "DATA Negative.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_pad_calibration data_p 11 data_n 11 addr_p 9 addr_n 8\r\n"
    "Results in resetting the Fap10m on the board.",
    "",
    { 0,0,0, BIT(9) | BIT(10)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PAD_CALIBRATION_ADDR_P_ID,
    "addr_p",
    (PARAM_VAL_RULES *)&Linecard_io_pad_vals[0],
    (sizeof(Linecard_io_pad_vals) / sizeof(Linecard_io_pad_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Address Positive.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_pad_calibration data_p 11 data_n 11 addr_p 9 addr_n 8\r\n"
    "Results in resetting the Fap10m on the board.",
    "",
    { 0,0,0, BIT(9)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PAD_CALIBRATION_ADDR_N_ID,
    "addr_n",
    (PARAM_VAL_RULES *)&Linecard_io_pad_vals[0],
    (sizeof(Linecard_io_pad_vals) / sizeof(Linecard_io_pad_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Address Negative.",
    "Examples:\r\n"
    "  line_card fap10m_1 iddr_pad_calibration data_p 11 data_n 11 addr_p 9 addr_n 8\r\n"
    "Results in resetting the Fap10m on the board.",
    "",
    { 0,0,0, BIT(9)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_RUN_BIST_TEST_ID,
    "run_bist_test_0",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Run the DDR BIST test.(ver 0)",
    "Examples:\r\n"
    "  line_card fap10m_1 run_bist_test\r\n"
    "Results in Running the DDR BIST test.\n\r",
    "",
    { 0,0,0, BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_RUN_BIST_TEST_1_ID,
    "run_bist_test_1",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Run the DDR BIST test (ver 1).",
    "Examples:\r\n"
    "  line_card fap10m_1 run_bist_test_1\r\n"
    "Results in Running the DDR BIST test.\n\r",
    "",
    { 0,0,0, BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_ID,
    "clocks",
    (PARAM_VAL_RULES *)&Linecard_clock_synthesizers_vals[0],
    (sizeof(Linecard_clock_synthesizers_vals) / sizeof(Linecard_clock_synthesizers_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load clock synthesizers values onto the line-card FPGA.",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core vdw 3 rdw 3 od 3\r\n"
    "Results in loading clock synthesizers to the FPGA.\n\r"
    "  For each one of the FAP PLLs we have the ability\n\r"
    "  to switch between a low jitter oscillator and a \n\r"
    "  frequency configurable clock synthesizer.\n\r"
    "  Unlike previous designs, such as the Fabric Card,\n\r"
    "  in the line card the synthesizer clock frequency is not\n\r"
    "  set by Dip switches but via the FPGA.\n\r",
    "",
    {BIT(4) | BIT(5) | BIT(19) | BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_CONFIGURE_WITH_FREQ_ID,
    "clk_freq",
    (PARAM_VAL_RULES *)&Linecard_clock_synthesizer_clk_freq_vals[0],
    (sizeof(Linecard_clock_synthesizer_clk_freq_vals) / sizeof(Linecard_clock_synthesizer_clk_freq_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the clock synthesizers values.",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core clock_synthesizers and_a_half\r\n"
    "Results in setting that the core clock synthesizer with 62.5 MHtz.\n\r",
    "",
    { BIT(19)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_GET_ID,
    "get_clk_freq",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get the clock synthesizer input rate. (For high level configuration)",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core get_clk_freq\r\n"
    "Results in getting the core clock synthesizer val in MHtz.\n\r",
    "",
    { BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_ADD_HALF_ID,
    "and_a_half",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the clock generator synthesizer/oscillator.",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core clock_synthesizers and_a_half\r\n"
    "Results in setting that the core clock synthesizer with 62.5 MHtz.\n\r",
    "",
    { BIT(19)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_SYNTHESIZER_OSCILLATOR_ID,
    "pll_source",
    (PARAM_VAL_RULES *)&Linecard_clock_synthesizer_oscillator_vals[0],
    (sizeof(Linecard_clock_synthesizer_oscillator_vals) / sizeof(Linecard_clock_synthesizer_oscillator_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the clock generator synthesizer/oscillator.",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core clock_synthesizers synthesizer\r\n"
    "Results in setting that the clock generator for the fap10m_1 core is synthesizer.\n\r",
    "",
    { BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_VDW_ID,
    "vdw",
    (PARAM_VAL_RULES *)&Linecard_clock_synthesizers_vdw_vals[0],
    (sizeof(Linecard_clock_synthesizers_vdw_vals) / sizeof(Linecard_clock_synthesizers_vdw_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "VCO Divider.",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core vdw 3 rdw 3 od 3\r\n"
    "Results in loading clock synthesizers to the FPGA.\n\r"
    "  For each one of the FAP PLLs we have the ability\n\r"
    "  to switch between a low jitter oscillator and a \n\r"
    "  frequency configurable clock synthesizer.\n\r"
    "  Unlike previous designs, such as the Fabric Card,\n\r"
    "  in the line card the synthesizer clock frequency is not\n\r"
    "  set by Dip switches but via the FPGA.\n\r",
    "",
    {BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_RDW_ID,
    "rdw",
    (PARAM_VAL_RULES *)&Linecard_clock_synthesizers_rdw_vals[0],
    (sizeof(Linecard_clock_synthesizers_rdw_vals) / sizeof(Linecard_clock_synthesizers_rdw_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reference Divider.",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core vdw 3 rdw 3 od 3\r\n"
    "Results in loading clock synthesizers to the FPGA.\n\r"
    "  For each one of the FAP PLLs we have the ability\n\r"
    "  to switch between a low jitter oscillator and a \n\r"
    "  frequency configurable clock synthesizer.\n\r"
    "  Unlike previous designs, such as the Fabric Card,\n\r"
    "  in the line card the synthesizer clock frequency is not\n\r"
    "  set by Dip switches but via the FPGA.\n\r",
    "",
    {BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_OD_ID,
    "od",
    (PARAM_VAL_RULES *)&Linecard_clock_synthesizers_od_vals[0],
    (sizeof(Linecard_clock_synthesizers_od_vals) / sizeof(Linecard_clock_synthesizers_od_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Output Divider. (For low level configuration)",
    "Examples:\r\n"
    "  line_card fap10m_1 clock_synthesizers core vdw 3 rdw 3 od 3\r\n"
    "Results in loading clock synthesizers to the FPGA.\n\r"
    "  For each one of the FAP PLLs we have the ability\n\r"
    "  to switch between a low jitter oscillator and a \n\r"
    "  frequency configurable clock synthesizer.\n\r"
    "  Unlike previous designs, such as the Fabric Card,\n\r"
    "  in the line card the synthesizer clock frequency is not\n\r"
    "  set by Dip switches but via the FPGA.\n\r",
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
    PARAM_LINE_CARD_RESET_ID,
    "reset",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reset command.",
    "Examples:\r\n"
    "  line_card pp_1 reset reset_down_milisec 100 \r\n"
    "Results in resetting the PP-1 on the board.\n\r"
    "  Writing '0' to the reset bit, wait 100 mili-sec and than write '1' to the reset bit.",
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
    PARAM_LINE_CARD_STAY_DOWN_ID,
    "stay_in_reset",
    (PARAM_VAL_RULES *)&Linecard_empty_vals[0],
    (sizeof(Linecard_empty_vals) / sizeof(Linecard_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Order the device to stay in reset.\r\n",
    "Examples:\r\n"
    "  line_card pp_1 reset stay_in_reset\r\n"
    "Results in resetting the PP-1 on the board.\n\r"
    "  The device will be set out of the reset mode.",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LINE_CARD_ENABLE_JUMBO_ID,
    "enable_jumbo_packets",
    (PARAM_VAL_RULES *)&General_enable_disable_vals[0],
    (sizeof(General_enable_disable_vals) / sizeof(General_enable_disable_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable/Disable jumbo packets.\n\r"
    "  The DRAM is changed to support up to 10 Mega packets.\r\n"
    "  Note: This change must be done before any traffic was streamed between\r\n"
    "    the FAP and the PP!!!.",
    "Examples:\r\n"
    "  line_card enable_jumbo_packets enable \r\n"
    "Results eabling jumbo packets.",
    "",
    {BIT(31)},
    1,
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
