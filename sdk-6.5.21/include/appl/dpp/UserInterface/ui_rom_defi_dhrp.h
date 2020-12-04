/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_DHRP_INCLUDED
/* { */
#define UI_ROM_DEFI_DHRP_INCLUDED
/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/ref_sys.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_fap20v_calibrate.h>


#include <ui_pure_defi_dhrp.h>


/*
 * UI definition{
 */

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dhrp_iddr_data_vals[]
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
        FAP20V_IDRAM_WORD_NOF_LONGS,
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
*  Dhrp_pkt_byte_size_defualt
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
     Dhrp_pkt_byte_size_defualt
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)64
  }
}
#endif
;

/********************************************************
*NAME
*  Dhrp_pad_data_vals
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
     Dhrp_pad_data_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        0xFF,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        FAP20V_CPU_TX_FRAG_BYTE_SIZE,
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
*  Dhrp_milisec_defualt
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
     Dhrp_milisec_defualt
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)5000
  }
}
#endif
;

EXTERN CONST
   PARAM_VAL_RULES
     Dhrp_on_off_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        1,
        (long)"    On. Enable. 1 value.",
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
        (long)"    Off. Disable. 0 value.",
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
*  Dhrp_nof_pkts_defualt
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
     Dhrp_nof_pkts_defualt
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
*  Dhrp_offset_correct_vals
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
     Dhrp_offset_correct_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        200,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        FAP20V_DRC_NOF_MODULES,
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
*  Dhrp_nof_ddr_vals
*TYPE: BUFFER
*DATE: 20/FEB/2002
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
     Dhrp_nof_ddr_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "two_ddrs",
    {
      {
        2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    2_ddrs.",
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
    "four_ddrs",
    {
      {
        FAP20V_DRC_NOF_MODULES,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    4_ddrs.",
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
*  Dhrp_nof_ddr_default
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Default FALSE.
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
     Dhrp_nof_ddr_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FAP20V_DRC_NOF_MODULES,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"    4_ddrs."
  }
}
#endif
;

/********************************************************
*NAME
*  Dhrp_fap_id
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
     Dhrp_fap_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_NOF_FAPS_IN_SYSTEM-1,0,
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
*  Dhrp_true_false_vals
*TYPE: BUFFER
*DATE: 20/FEB/2002
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
     Dhrp_true_false_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "true",
    {
      {
        1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    TRUE.",
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
    "false",
    {
      {
        0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    FALSE.",
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
*  Dhrp_false_default
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Default FALSE.
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
     Dhrp_false_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FALSE,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"    FALSE."
  }
}
#endif
;

/********************************************************
*NAME
*  Dhrp_true_default
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Default TRUE.
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
     Dhrp_true_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  TRUE,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"    TRUE."
  }
}
#endif
;

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dhrp_empty_vals[]
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
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dhrp_free_vals[]
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
*  Fap20v_indefinite_num_vals
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
     Dhrp_indefinite_num_vals[]
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
 */
EXTERN CONST
   PARAM
     Dhrp_params[]
#ifdef INIT
   =
/*
 ******
 * ENTRY -- 0
 ******
 * ENTRY -- 1
 *BIT() - .
 */
{
  {
    PARAM_DHRP_BSP_ID,
    "bsp",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "BSP related.",
    "Examples:\r\n"
    "  dhrp mem read 0x10\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13)| BIT(14) | BIT(15) | BIT(16) | BIT(17) | BIT(18) | BIT(19) | BIT(20) | BIT(26) | BIT(31),
     BIT(9) | BIT(10)
    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_MILISEC_ID,
    "delay_in_milisec",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Dhrp_milisec_defualt,
    0,0,0,0,0,0,0,
    "How much milisec we send traffic, before check for errors.\r\n",
    "Examples:\r\n"
    "  dhrp fap20v_1 search_dram_delay_line delay_in_milisec 5000 start_taps 10 2 0 0 0\r\n"
    "Delay for 5 seconds, before check for errors.",
    "",
    {BIT(8) | BIT(9) | BIT(14) | BIT(15) | BIT(16) | BIT(17) | BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_ZBT_TEST_RW_ID,
    "zbt_test_rw",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write ZBT test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  dhrp fap20v_1 zbt_test_rw\r\n"
    "Read/write ZBT test.\n\r"
    "  Test function of the SOC_SAND_FAP20V ZBT.\n\r"
    "  1. Write data to some ZBT offsets.\n\r"
    "  2. Read the data, and compare it.\n\r",
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
    PARAM_DHRP_INQ_TEST_RW_ID,
    "inq_test_rw",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write INQ test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  dhrp fap20v_1 inq_test_rw\r\n"
    "Read/write INQ test.\n\r"
    "  Test function of the SOC_SAND_FAP20V INQ.\n\r"
    "  1. Write data to some INQ offsets.\n\r"
    "  2. Read the data, and compare it.\n\r",
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
    PARAM_DHRP_CPU_TEST_RW_ID,
    "cpu_test_rw",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write CPU test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  dhrp fap20v_1 cpu_test_rw\r\n"
    "Test function of the SOC_SAND_FAP20V CPU interface.\n\r"
    "1. Read version register, and compare to the expected value.\n\r"
    "2. Write/Read/Compare values at spare register.\n\r",
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
    PARAM_DHRP_IDDR_TEST_RW_ID,
    "iddr_test_rw",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write IDDR test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  dhrp fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
    "Read/write IDDR test.\n\r"
    "  Write to the Ingress DRAM at offset1, 256 bits.\n\r"
    "  First long is 0x0.\n\r"
    "  Second long is 0xEDA1.\n\r"
    "  Rest will be paded with 0xAA55AA55.\n\r"
    "  Every iteration the data is inverted.\n\r"
    "  Will write the value, than read it, than compare.\n\r"
    "  If compare succeed - continue to teh next write/read.\n\r"
    "  If compare fail - stops and reports",
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
    PARAM_DHRP_IDDR_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset in the IDDR.",
    "Examples:\r\n"
    "  dhrp fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
    "Read/write IDDR test.\n\r"
    "  Write to the Ingress DRAM at offset1, 256 bits.\n\r"
    "  First long is 0x0.\n\r"
    "  Second long is 0xEDA1.\n\r"
    "  Rest will be paded with 0xAA55AA55.\n\r"
    "  Every iteration the data is inverted.\n\r"
    "  Will write the value, than read it, than compare.\n\r"
    "  If compare succeed - continue to teh next write/read.\n\r"
    "  If compare fail - stops and reports",
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
    PARAM_DHRP_NOF_ITERATION_ID,
    "nof_iteration",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of read/writes per test.",
    "Examples:\r\n"
    "  dhrp fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
    "Read/write IDDR test.\n\r"
    "  Write to the Ingress DRAM at offset1, 256 bits.\n\r"
    "  First long is 0x0.\n\r"
    "  Second long is 0xEDA1.\n\r"
    "  Rest will be paded with 0xAA55AA55.\n\r"
    "  Every iteration the data is inverted.\n\r"
    "  Will write the value, than read it, than compare.\n\r"
    "  If compare succeed - continue to teh next write/read.\n\r"
    "  If compare fail - stops and reports",
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
    PARAM_DHRP_PAD_DATA_ID,
    "data_to_test",
    (PARAM_VAL_RULES *)&Dhrp_iddr_data_vals[0],
    (sizeof(Dhrp_iddr_data_vals) / sizeof(Dhrp_iddr_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Data to read/write.",
    "Examples:\r\n"
    "  dhrp fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
    "Read/write IDDR test.\n\r"
    "  Write to the Ingress DRAM at offset1, 256 bits.\n\r"
    "  First long is 0x0.\n\r"
    "  Second long is 0xEDA1.\n\r"
    "  Rest will be paded with 0xAA55AA55.\n\r"
    "  Every iteration the data is inverted.\n\r"
    "  Will write the value, than read it, than compare.\n\r"
    "  If compare succeed - continue to teh next write/read.\n\r"
    "  If compare fail - stops and reports",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

/* { utils_fap20v_calibrate.c */

  {
    PARAM_DHRP_CALIBRATE_LOAD_ID,
    "load_params",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load calibration parameters to the device.",
    "Examples:\r\n"
    "  dhrp bsp load_params end_win 41 37 43 42\r\n"
    "Load calibration parameters to the device.\n\r",
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
    PARAM_DHRP_END_WIN_ID,
    "end_win",
    (PARAM_VAL_RULES *)&Dhrp_offset_correct_vals[0],
    (sizeof(Dhrp_offset_correct_vals) / sizeof(Dhrp_offset_correct_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "End window per DRAM calibrate.",
    "Examples:\r\n"
    "  dhrp bsp load_params end_win 41 37 43 42 avrg 9 7 9 8\r\n"
    "End window 41 37 43 42.",
    "",
    {BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_AVRG_ID,
    "avrg",
    (PARAM_VAL_RULES *)&Dhrp_offset_correct_vals[0],
    (sizeof(Dhrp_offset_correct_vals) / sizeof(Dhrp_offset_correct_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "End window per DRAM calibrate.",
    "Examples:\r\n"
    "  dhrp bsp load_params end_win 41 37 43 42 avrg 9 7 9 8 sample_delay_reg 0x294A5294\r\n"
    "avrg 9 7 9 8.",
    "",
    {BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_SAMPLE_DELAY_REG_ID,
    "sample_delay_reg",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Dhrp_nof_ddr_default,
    0,0,0,0,0,0,0,
    "Sample delay register.",
    "Examples:\r\n"
    "  dhrp bsp load_params end_win 41 37 43 42 avrg 9 7 9 8 sample_delay_reg 0x294A5294\r\n"
    "sample_delay_reg 0x294A5294.",
    "",
    {BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_SCAN_DRAM_DELAY_AUTO_ID,
    "scan_dram_delay_line_auto",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scan for DRAM delay lines - Automatic function.",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line_auto delay_in_milisec 5000 fix_factor 4\r\n"
    "Scan for DRAM delay lines.\n\r",
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
    PARAM_DHRP_CALIBRATE_DDR_FULL_ID,
    "calibrate_ddr_full",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Full Clibrate DDR.",
    "Examples:\r\n"
    "  dhrp bsp calibrate_ddr_full\r\n"
    "Scan for DRAM delay lines.\n\r",
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
    PARAM_DHRP_NOF_DDR_ID,
    "nof_ddr",
    (PARAM_VAL_RULES *)&Dhrp_nof_ddr_vals[0],
    (sizeof(Dhrp_nof_ddr_vals) / sizeof(Dhrp_nof_ddr_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Dhrp_nof_ddr_default,
    0,0,0,0,0,0,0,
    "Number of DRAM in the system to calibrate.",
    "Examples:\r\n"
    "  dhrp bsp calibrate_ddr_full nof_ddr 4_ddr\r\n"
    "Calibrating 4 ddrs (default).",
    "",
    {BIT(19) | BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_OFFSET_CORRECT_ID,
    "offset_correct",
    (PARAM_VAL_RULES *)&Dhrp_offset_correct_vals[0],
    (sizeof(Dhrp_offset_correct_vals) / sizeof(Dhrp_offset_correct_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset correction per DRAM calibrate.",
    "Examples:\r\n"
    "  dhrp bsp calibrate_ddr_full nof_ddr 4_ddr offset_correct 1 3 1 3\r\n"
    "Calibrating with offset correction 1 3 1 3.",
    "",
    {BIT(19)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_DO_EXHAUSTIVE_SEARCH_ID,
    "do_exhaustive_search",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do exhaustive search DRAM calibrate.",
    "Examples:\r\n"
    "  dhrp bsp calibrate_ddr_full nof_ddr 4_ddr offset_correct 1 3 1 3 do_exhaustive_search\r\n"
    "Calibration with exhaustive search.",
    "",
    {BIT(19)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_CALIBRATE_DDR_ID,
    "calibrate_ddr",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clibrate DDR.",
    "Examples:\r\n"
    "  dhrp bsp calibrate_ddr delay_in_milisec 5000 max_nof_send_pkts\r\n"
    "Scan for DRAM delay lines.\n\r",
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
    PARAM_DHRP_SCAN_DRAM_DELAY_AUTO_B_ID,
    "scan_dram_delay_line_auto_b",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scan for DRAM delay lines - TYPE B -Automatic function.",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 sample_num 2 sample_add 1\r\n"
    "Scan for DRAM delay lines.\n\r",
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
    PARAM_DHRP_SAMPLE_NUM_ID,
    "sample_num",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sample number to take (out of 5).\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line_auto delay_in_milisec 5000 fix_factor 4 sample_num 2 sample_add 1\r\n"
    "Fix factor here is 4.",
    "",
    {BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_PACKET_SCHEME_ID,
    "packet_scheme",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet scheme to use.\n\r"
    "0- SSO pattern. 1- Relaxed pattern (SSO and ZERO). 2- Zero pattern\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 smaple_num 2 sample_add 1 packet_scheme0\r\n"
    "Fix factor here is 4.",
    "",
    {BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_NOF_PKTS_ID,
    "nof_pkts",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)&Dhrp_nof_pkts_defualt,
    0,0,0,0,0,0,0,
    "Number of pakcets to send. Number will tranced to 32.",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 smaple_num 2 sample_add 1 packet_scheme 0 nof_pkts 32\r\n"
    "Maximum number of packet to send is 32, bigger number will be tranced to 32.",
    "",
    {BIT(16) | BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_SAMPLE_ADD_ID,
    "sample_add",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number to add to the sample value we take.\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 smaple_num 2 sample_add 1\r\n"
    "Fix factor here is 4.",
    "",
    {BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_FIX_FACTOR_ID,
    "fix_factor",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Fix factor, for average calculations.\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line_auto delay_in_milisec 5000 fix_factor 4\r\n"
    "Fix factor here is 4.",
    "",
    {BIT(15) | BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_SCAN_DRAM_DELAY_LINE_ID,
    "scan_dram_delay_line",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scan for DRAM delay lines.",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
    "Scan for DRAM delay lines.\n\r",
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
    PARAM_DHRP_START_DDR_ID,
    "start_ddr",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The DDR to start with.\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
    "The DRAM to use are 0 to 3 (ALL).",
    "",
    {BIT(9) | BIT(14) | BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_END_DDR_ID,
    "end_ddr",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The DDR to end with.\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
    "The DRAM to use are 0 to 3 (ALL).",
    "",
    {BIT(9) | BIT(14) | BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_START_SCAN_ID,
    "start_scan",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The SCAN value to start with.\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
    "The scan values are from 20 to 50.",
    "",
    {BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_END_SCAN_ID,
    "end_scan",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The SCAN value to end with.\r\n",
    "Examples:\r\n"
    "  dhrp bsp scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
    "The scan values are from 20 to 50.",
    "",
    {BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_LBG_ID,
    "lbg",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "LBG - Local Bandwidth Generation (using recycling and local route).",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg start_conf\r\n"
    "Results in configuring the SOC_SAND_FAP20V to LBG mode.",
    "",
    {BIT(10) | BIT(11) | BIT(12) | BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_VERBOSE_ID,
    "verbose",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do more printing.",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg start_conf verbose\r\n"
    "Results in configuring the SOC_SAND_FAP20V to LBG mode -- DO MORE PRINTS",
    "",
    {BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(14) | BIT(15) | BIT(16) | BIT(18) | BIT(19)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_LBG_START_CONF_ID,
    "start_conf",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Configue the FAP to LBG mode (using recycling and local route).",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg start_conf\r\n"
    "Results in configuring the SOC_SAND_FAP20V to LBG mode.",
    "",
    {BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_LBG_SEND_PKT_ID,
    "send_pkt",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send packet in LBG mode (using recycling and local route).",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg send_pkt\r\n"
    "Results in sending packet in LBG mode.",
    "",
    {BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_PAD_DATA_ID,
    "pad_data",
    (PARAM_VAL_RULES *)&Dhrp_pad_data_vals[0],
    (sizeof(Dhrp_pad_data_vals) / sizeof(Dhrp_pad_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "Up to 32 bytes of data may be inserted. The rest of the data is used with default values.",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg send_pkt nof_pkts 1000 pad_data 0xFF 0xCC 0x12\r\n"
    "Send 1000 packets. The data get sent is:\n\r"
    "  0xFFCC12FF 0x00000000 0xFFFFFFFF 0x00000000 0xFFFFFFFF 0x00000000 0xFFFFFFFF 0x00000000",
    "",
    {BIT(11)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_NOF_PKTS_ID,
    "nof_pkts",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Dhrp_nof_pkts_defualt,
    0,0,0,0,0,0,0,
    "Number of pakcets to send.",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg send_pkt nof_pkts 1000\r\n"
    "Send 1000 packets.",
    "",
    {BIT(11)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_PKT_BYTE_SIZE_ID,
    "pkt_byte_size",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MUST_APPEAR,(PARAM_VAL *)&Dhrp_pkt_byte_size_defualt,
    0,0,0,0,0,0,0,
    "Packet byte size",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg send_pkt nof_pkts 1000 pkt_byte_size 100\r\n"
    "Send 1000 packets. The packet byte size is 100",
    "",
    {BIT(11) | BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_LBG_RESTORE_CONF_ID,
    "restore_conf",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Restore the FAP to before LBG mode.",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg restore_conf\r\n"
    "Restore configuration the SOC_SAND_FAP20V to the before LBG mode.",
    "",
    {BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_GET_RATE_ID,
    "get_rate",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get device BW rates.",
    "Examples:\r\n"
    "  dhrp fap20v_1 lbg get_rate\r\n"
    "Get device BW rates.",
    "",
    {BIT(13)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_CLR_CNT_INT_ID,
    "clear_counter_and_interrupts",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clear counters and interrupts at the SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  gfa fap20v_1 clear_counter_and_interrupts\r\n"
    "Results in clear counter and interrupts.",
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
    PARAM_DHRP_CALIBRATE_CLEAR_NV_ID,
    "calibrate_params_from_nv_clear",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clear the stored calibration parameters from the Board NvRam.",
    "Examples:\r\n"
    "  dhrp bsp calibrate_params_from_nv_clear\r\n"
    " Clear the stored calibration parameters from the Board NvRam..\n\r",
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
    PARAM_DHRP_MEM_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "DHRP read",
    "Examples:\r\n"
    "  dhrp read 0x10\r\n"
    "Read address 0x10.",
    "",
    {BIT(28)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_MEM_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "DHRP write",
    "Examples:\r\n"
    "  dhrp read 0x10 data 0x1\r\n"
    "Write 0x1 to address 0x10.",
    "",
    {BIT(29)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_MEM_WRITE_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Data to write",
    "Examples:\r\n"
    "  dhrp read 0x10 data 0x1\r\n"
    "Write 0x1 to address 0x10.",
    "",
    {BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_MEM_NOF_ADDR_ID,
    "nof_addrs",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "nof consequent addressese to read/write",
    "Examples:\r\n"
    "  dhrp read 0x10 data 0x1 nod_addrs 3\r\n"
    "Write 0x1 to addresses 0x10, 0x14, 0x18.",
    "",
    {BIT(28) | BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_RESET_DEVICE_ID,
    "reset_device",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "resets a SOC_SAND_FAP20V device.\r\n",
    "Examples:\r\n"
    "  fap20v general reset_device 0x1\r\n"
    "Results in resetting device 0x1 (off-on).\r\n",
    "",
    {BIT(26) | BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DHRP_REMAIN_OFF_ID,
    "remain_off",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears the device will stay in reset.\r\n",
    "Examples:\r\n"
    "  fap20v general reset_device 0x1 remain_off\r\n"
    "Results in shutting of device 0x1.\r\n",
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
    PARAM_DHRP_DRAM_CALIBRATE_AUTO_ID,
    "calibrate_ddr_auto",
    (PARAM_VAL_RULES *)&Dhrp_empty_vals[0],
    (sizeof(Dhrp_empty_vals) / sizeof(Dhrp_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "auto calibrates the DDR.\r\n",
    "Examples:\r\n"
    "  dhrp bsp calibrate_ddr_auto dev_id 1\r\n",
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
    PARAM_DHRP_DEVICE_ID_ID,
    "dev_id",
    (PARAM_VAL_RULES *)&Dhrp_indefinite_num_vals[0],
    (sizeof(Dhrp_indefinite_num_vals) / sizeof(Dhrp_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "DHRP unit, might be added to the end of each CLI line. (0 or 1)",
    "Examples:\r\n"
    "  dhrp bsp lbg get_rate unit 1\r\n"
    "Get device BW rates.",
    "",
    {0xFFFFFFFF, 0xFFFFFFFF},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
/* } */
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


