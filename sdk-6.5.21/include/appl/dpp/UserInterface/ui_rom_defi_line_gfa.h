/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_LINE_GFA_INCLUDED
/* { */
#define UI_ROM_DEFI_LINE_GFA_INCLUDED
/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/ref_sys.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_line_GFA.h>
#include <appl/diag/dpp/utils_fap20v_tests.h>

#include <sweep/Fap20v/sweep_fap20v_app.h>

#include <ui_pure_defi_line_gfa.h>


/*
 * UI definition{
 */


/********************************************************
*NAME
*  Gfa_offset_correct_vals
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
     Gfa_offset_correct_vals[]
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
*  Gfa_nof_ddr_vals
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
     Gfa_nof_ddr_vals[]
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
*  Gfa_nof_ddr_default
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
     Gfa_nof_ddr_default
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
*  Gfa_true_false_vals
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
     Gfa_true_false_vals[]
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
*  Gfa_false_default
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
     Gfa_false_default
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
*  Gfa_true_default
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
     Gfa_true_default
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
*NAME
*  Gfa_start_taps_vals
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
     Gfa_start_taps_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_MAX_DLY_TAP_VAL,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        FAP20V_DPI_NOF_TAPS,
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
*  Gfa_spi4_source_cal_len_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 source calendar length value
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
     Gfa_spi4_source_cal_len_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SRC_CAL_LEN
  }
}
#endif
;
/********************************************************
*NAME
*  Gfa_spi4_sink_cal_len_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 sink calendar length value
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
     Gfa_spi4_sink_cal_len_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SNK_CAL_LEN
  }
}
#endif
;
/********************************************************
*NAME
*  Gfa_spi4_source_cal_m_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 source calendar M value
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
     Gfa_spi4_source_cal_m_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SRC_CAL_M
  }
}
#endif
;
/********************************************************
*NAME
*  Gfa_spi4_sink_cal_m_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 sink calendar M value
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
     Gfa_spi4_sink_cal_m_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SNK_CAL_M
  }
}
#endif
;


/********************************************************
*NAME
*  Gfa_spi4_tsclk_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 tsclk value
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
     Gfa_spi4_tsclk_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SRC_TSCLK_SEL
  }
}
#endif
;
/********************************************************
*NAME
*  Gfa_spi4_rsclk_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 rsclk value
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
     Gfa_spi4_rsclk_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SNK_RSCLK_SEL
  }
}
#endif
;
/********************************************************
*NAME
*  Gfa_spi4_data_max_t_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 data_max_t value
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
     Gfa_spi4_data_max_t_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SRC_DATA_MAX_T
  }
}
#endif
;
/********************************************************
*NAME
*  Gfa_spi4_init_train_seq_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*  Default spi4 init_train_seq value
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
     Gfa_spi4_init_train_seq_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_SPI4_SRC_INIT_TRAIN_SEQ
  }
}
#endif
;

/********************************************************
*NAME
*  Gfa_nominal_rate_per_port_mbps_default
*TYPE: BUFFER
*DATE: 25/JUN/2004
*FUNCTION:
*
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
     Gfa_nominal_rate_per_port_mbps_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP20V_SWP_FTG_NOMINAL_RATE_PER_PORT
  }
}
#endif
;

/********************************************************
*NAME
*  Gfa_credit_size_default
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Default credit size.
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
     Gfa_credit_size_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  SOC_SAND_CR_256,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"credit_256"
  }
}
#endif
;

/********************************************************
*NAME
*  Gfa_fap_id_default
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Default FAP ID
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
     Gfa_fap_id_default
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
*  Gfa_milisec_defualt
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
     Gfa_milisec_defualt
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

/********************************************************
*NAME
*  Gfa_nof_pkts_defualt
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
     Gfa_nof_pkts_defualt
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
*  Gfa_pkt_byte_size_defualt
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
     Gfa_pkt_byte_size_defualt
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
*  Gfa_reset_milisec_defualt
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
     Gfa_reset_milisec_defualt
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
*  Gfa_empty_vals
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
     Gfa_empty_vals[]
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
*  Gfa_pad_data_vals
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
     Gfa_pad_data_vals[]
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
*  Gfa_indefinite_num_vals
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
     Gfa_iddr_data_vals[]
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
*  Gfa_indefinite_num_vals
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
     Gfa_indefinite_num_vals[]
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
*  Gfa_fap20_credit_size_vals
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Array of all possible string values represeting the possible credit size
*   values in the SOC_SAND_FAP20V. (256, 512, 1024, 2048)
*
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
     Gfa_fap20_credit_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "credit_256",
    {
      {
        SOC_SAND_CR_256,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Credit size = 256 Bytes.",
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
    "credit_512",
    {
      {
        SOC_SAND_CR_512,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Credit size = 512 Bytes.",
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
    "credit_1024",
    {
      {
        SOC_SAND_CR_1024,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Credit size = 1024 Bytes.",
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
    "credit_2048",
    {
      {
        SOC_SAND_CR_2048,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Credit size = 2048 Bytes.",
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
    "credit_4096",
    {
      {
        SOC_SAND_CR_4096,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Credit size = 2048 Bytes.",
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
    "credit_8192",
    {
      {
        SOC_SAND_CR_8192,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Credit size = 2048 Bytes.",
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
*  Gfa_fap20_spi4_cal_len_vals
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Array of all possible values represeting the possible Spi4 calendar length
*
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
     Gfa_fap20_spi4_cal_len_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        127,1,
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
*  Gfa_fap20_spi4_cal_m_vals
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Array of all possible values represeting the possible Spi4 calendar M
*
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
     Gfa_fap20_spi4_cal_m_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        255,1,
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
*  Gfa_fap20_spi4_tsclk_rsclk_vals
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Array of all possible values represeting the possible Spi4 TSCLK/RSCLK
*   0 or 1
*
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
     Gfa_fap20_spi4_tsclk_rsclk_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
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
*  Gfa_fap20_spi4_data_max_t_vals
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Array of all possible values represeting the possible Spi4 DATA_MAX_T
*   0 or 1
*
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
     Gfa_fap20_spi4_data_max_t_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        131071, 0, /* max value is 17 bit */
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
*  Gfa_fap20_spi4_init_train_seq_vals
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Array of all possible values represeting the possible Spi4 initial training seq
*   0 or 1
*
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
     Gfa_fap20_spi4_init_train_seq_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        255, 0, /* max value is 8 bit */
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
*  Gfa_nominal_rate_per_port_mbps_vals
*TYPE: BUFFER
*DATE: 15/Jun/2004
*FUNCTION:
*  Array of all possible values represeting the possible Spi4 initial training seq
*   0 or 1
*
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
     Gfa_nominal_rate_per_port_mbps_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_MAX_SCHEDULER_PORT_MBPS, 0,
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
*  Gfa_params
*TYPE: BUFFER
*DATE: 29/Dec/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "GFA".
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
     Gfa_params[]
#ifdef INIT
   =
/*
 ******
 * ENTRY -- 0
 *BIT(0) - reset SOC_SAND_FAP20V
 *BIT(1) - load SLA table SOC_SAND_FAP20V
 *BIT(2) - clear counters and interrupts SOC_SAND_FAP20V
 *BIT(3) - iddr_test_rw
 *BIT(4) - fap20_sweep_app_stand_alone, fap20_sweep_app
 *BIT(5) - fap20v_read_and_clear_cpu_pkt
 *BIT(6) - zbt_test_rw, inq_test_rw
 *BIT(7) - cpu_test_rw
 * DRAM DELAY LINE SEARCH - A
 *BIT(8) - search_dram_delay_line
 *BIT(9) - search_dram_delay_line_auto
 *BIT(17) - search_dram_delay_line_auto_full
 * LBG - Local Bandwidth Generation
 *BIT(10)- LBG start conf
 *BIT(11)- LBG send pkt
 *BIT(12)- LBG restore conf
 *BIT(13)- get rate
 * DRAM DELAY LINE SEARCH - B
 *BIT(14) - scan_dram_delay_line
 *BIT(15) - scan_dram_delay_line_auto
 *BIT(16) - scan_dram_delay_line_auto_b
 * CALIBRATE DDR
 *BIT(18) - fap20v_calibrate_ddr
 *BIT(19) - fap20v_calibrate_ddr_full
 ******
 * ENTRY -- 1
 *BIT(0) - gfa status
 *BIT(1) - gfa MB FLASH
 */
{
  {
    PARAM_GFA_SCRN_ID,
    "screening",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Screening utils.",
    "Examples:\r\n"
    "  gfa screening FTG_test\r\n"
    "Activate the FTG screening test.",
    "",
    {0 ,
     BIT(11) | BIT(12)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_SCRN_FTG_TEST_ID,
    "ftg_test",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Screening FTG test, where packets are sent from the FTG.\n\r"
    " and expected to be received as well",
    "Examples:\r\n"
    "  gfa screening ftg_test\r\n"
    "Activate the FTG screening test.",
    "",
    {0 ,
     BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_SCRN_IF_TEST_ID,
    "interfaces",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Screening interfaces test, where the SPI DRAM CPU and ZBT are tested",
    "Examples:\r\n"
    "  gfa screening ftg_test\r\n"
    "Activate the FTG screening test.",
    "",
    {0 ,
     BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_SCRN_PRNT_USR_MD_ID,
    "print_mode",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Screening mode",
    "Examples:\r\n"
    "  gfa screening ftg_test\r\n"
    "Activate the FTG screening test.",
    "",
    {0 ,
     BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_MB_ID,
    "mb",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "GFA MB BSP.",
    "Examples:\r\n"
    "  gfa mb flash download\r\n"
    "Download and Burn flash on the GFA-MB card.",
    "",
    {0 ,
     BIT(1) | BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_FLASH_ID,
    "flash",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "GFA MB FLASH BSP. This flash holds the FPGAs executable.\n\r"
    "These FPGAs are the streaming-if and statistics.",
    "Examples:\r\n"
    "  gfa mb flash download\r\n"
    "Download and Burn flash on the GFA-MB card.",
    "",
    {0 ,
     BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_FPGA_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download and burn GFA MB FLASH.\n\r"
    "These FPGAs are the streaming-if and statistics.",
    "Examples:\r\n"
    "  gfa mb flash download\r\n"
    "Download and Burn flash on the GFA-MB card.",
    "",
    {0 ,
     BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_ERASE_ID,
    "erase",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Erase GFA MB FLASH.\n\r"
    "These FPGAs are the streaming-if and statistics.",
    "Examples:\r\n"
    "  gfa mb flash erase\r\n"
    "Erase flash on the GFA-MB card.",
    "",
    {0 ,
     BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_BURN_FPGA_ID,
    "burn_fpga",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "burn GFA MB FPGA. The exe is taken from the FLASH.\n\r"
    "These FPGAs are the streaming-if and statistics.",
    "Examples:\r\n"
    "  gfa mb flash burn_fpga\r\n"
    "Burn FPGAs on the GFA-MB card.",
    "",
    {0 ,
     BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PRINT_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print basic info on the GFA MB FLASH. Good for debug",
    "Examples:\r\n"
    "  gfa mb flash status\r\n"
    "Resulting in status printing.",
    "",
    {0 ,
     BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PRINT_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print basic info on the GFA BSP. Good for debug",
    "Examples:\r\n"
    "  gfa who_am_i\r\n"
    "Resulting in status printing.",
    "",
    {0 ,
     BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_FPGA_ID,
    "fpga",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "GFA MB FPGA BSP.\n\r"
    "These FPGAs are the streaming-if and statistics.",
    "Examples:\r\n"
    "  gfa mb fpga download\r\n"
    "Download and Burn fpga on the GFA-MB card.",
    "",
    {0 ,
     BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download and burn GFA MB FPGA.\n\r"
    "These FPGAs are the streaming-if and statistics.",
    "Examples:\r\n"
    "  gfa mb fpga download\r\n"
    "Download and Burn fpga on the GFA-MB card.",
    "",
    {0 ,
     BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_FAP20V_1_ID,
    "fap20v_1",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "BSP Access to SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  gfa fap20v_1 reset\r\n"
    "Results in resetting the SOC_SAND_FAP20V-1 on the board.",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13)| BIT(14) | BIT(15) | BIT(16) | BIT(17) | BIT(18) | BIT(19) | BIT(20),
     BIT(9) | BIT(10),
     0,
     0,
     BIT(1)
      },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_CALIB_WRITE_DQDLY_STATE,
    "write_dqdly_state",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write DQ Delay state.",
    "Examples:\r\n"
    "  gfa fap20v_1 write_dqdly_state ddr 0 posiotion 22\r\n"
    "Will set delay 22 to ddrs 0.\n\r",
    "",
    {0,0,0,0,BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_CALIB_WRITE_DQDLY_STATE_DDR,
    "ddr",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write DQ Delay state.",
    "Examples:\r\n"
    "  gfa fap20v_1 write_dqdly_state ddr 0 posiotion 22\r\n"
    "Will set delay 22 to ddrs 0.\n\r",
    "",
    {0,0,0,0,BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_CALIB_WRITE_DQDLY_STATE_POS,
    "position",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write DQ Delay state.",
    "Examples:\r\n"
    "  gfa fap20v_1 write_dqdly_state ddr 0 posiotion 22\r\n"
    "Will set delay 22 to ddrs 0.\n\r",
    "",
    {0,0,0,0,BIT(1)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_CALIBRATE_LOAD_ID,
    "load_params",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load calibration parameters to the device.",
    "Examples:\r\n"
    "  gfa fap20v_1 load_params end_win 41 37 43 42\r\n"
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
    PARAM_GFA_END_WIN_ID,
    "end_win",
    (PARAM_VAL_RULES *)&Gfa_offset_correct_vals[0],
    (sizeof(Gfa_offset_correct_vals) / sizeof(Gfa_offset_correct_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "End window per DRAM calibrate.",
    "Examples:\r\n"
    "  gfa fap20v_1 load_params end_win 41 37 43 42 avrg 9 7 9 8\r\n"
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
    PARAM_GFA_AVRG_ID,
    "avrg",
    (PARAM_VAL_RULES *)&Gfa_offset_correct_vals[0],
    (sizeof(Gfa_offset_correct_vals) / sizeof(Gfa_offset_correct_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "End window per DRAM calibrate.",
    "Examples:\r\n"
    "  gfa fap20v_1 load_params end_win 41 37 43 42 avrg 9 7 9 8 sample_delay_reg 0x294A5294\r\n"
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
    PARAM_GFA_SAMPLE_DELAY_REG_ID,
    "sample_delay_reg",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_nof_ddr_default,
    0,0,0,0,0,0,0,
    "Sample delay register.",
    "Examples:\r\n"
    "  gfa fap20v_1 load_params end_win 41 37 43 42 avrg 9 7 9 8 sample_delay_reg 0x294A5294\r\n"
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
    PARAM_GFA_SCAN_DRAM_DELAY_AUTO_ID,
    "scan_dram_delay_line_auto",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scan for DRAM delay lines - Automatic function.",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line_auto delay_in_milisec 5000 fix_factor 4\r\n"
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
    PARAM_GFA_CALIBRATE_DDR_FULL_ID,
    "calibrate_ddr_full",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Full Clibrate DDR.",
    "Examples:\r\n"
    "  gfa fap20v_1 calibrate_ddr_full\r\n"
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
    PARAM_GFA_NOF_DDR_ID,
    "nof_ddr",
    (PARAM_VAL_RULES *)&Gfa_nof_ddr_vals[0],
    (sizeof(Gfa_nof_ddr_vals) / sizeof(Gfa_nof_ddr_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_nof_ddr_default,
    0,0,0,0,0,0,0,
    "Number of DRAM in the system to calibrate.",
    "Examples:\r\n"
    "  gfa fap20v_1 calibrate_ddr_full nof_ddr 4_ddr\r\n"
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
    PARAM_GFA_OFFSET_CORRECT_ID,
    "offset_correct",
    (PARAM_VAL_RULES *)&Gfa_offset_correct_vals[0],
    (sizeof(Gfa_offset_correct_vals) / sizeof(Gfa_offset_correct_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset correction per DRAM calibrate.",
    "Examples:\r\n"
    "  gfa fap20v_1 calibrate_ddr_full nof_ddr 4_ddr offset_correct 1 3 1 3\r\n"
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
    PARAM_GFA_DO_EXHAUSTIVE_SEARCH_ID,
    "do_exhaustive_search",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do exhaustive search DRAM calibrate.",
    "Examples:\r\n"
    "  gfa fap20v_1 calibrate_ddr_full nof_ddr 4_ddr offset_correct 1 3 1 3 do_exhaustive_search\r\n"
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
    PARAM_GFA_CALIBRATE_DDR_ID,
    "calibrate_ddr",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clibrate DDR.",
    "Examples:\r\n"
    "  gfa fap20v_1 calibrate_ddr delay_in_milisec 5000 max_nof_send_pkts\r\n"
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
    PARAM_GFA_SCAN_DRAM_DELAY_AUTO_B_ID,
    "scan_dram_delay_line_auto_b",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scan for DRAM delay lines - TYPE B -Automatic function.",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 sample_num 2 sample_add 1\r\n"
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
    PARAM_GFA_SAMPLE_NUM_ID,
    "sample_num",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sample number to take (out of 5).\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line_auto delay_in_milisec 5000 fix_factor 4 sample_num 2 sample_add 1\r\n"
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
    PARAM_GFA_PACKET_SCHEME_ID,
    "packet_scheme",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet scheme to use.\n\r"
    "0- SSO pattern. 1- Relaxed pattern (SSO and ZERO). 2- Zero pattern\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 smaple_num 2 sample_add 1 packet_scheme0\r\n"
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
    PARAM_GFA_NOF_PKTS_ID,
    "nof_pkts",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)&Gfa_nof_pkts_defualt,
    0,0,0,0,0,0,0,
    "Number of pakcets to send. Number will tranced to 32.",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 smaple_num 2 sample_add 1 packet_scheme 0 nof_pkts 32\r\n"
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
    PARAM_GFA_SAMPLE_ADD_ID,
    "sample_add",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number to add to the sample value we take.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line_auto_b delay_in_milisec 5000 fix_factor 4 smaple_num 2 sample_add 1\r\n"
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
    PARAM_GFA_FIX_FACTOR_ID,
    "fix_factor",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Fix factor, for average calculations.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line_auto delay_in_milisec 5000 fix_factor 4\r\n"
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
    PARAM_GFA_SCAN_DRAM_DELAY_LINE_ID,
    "scan_dram_delay_line",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scan for DRAM delay lines.",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
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
    PARAM_GFA_START_DDR_ID,
    "start_ddr",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The DDR to start with.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
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
    PARAM_GFA_END_DDR_ID,
    "end_ddr",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The DDR to end with.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
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
    PARAM_GFA_START_SCAN_ID,
    "start_scan",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The SCAN value to start with.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
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
    PARAM_GFA_END_SCAN_ID,
    "end_scan",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The SCAN value to end with.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 scan_dram_delay_line delay_in_milisec 5000 start_ddr 0 end_ddr 3 start_scan 20 end_scan 50\r\n"
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
    PARAM_GFA_LBG_ID,
    "lbg",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "LBG - Local Bandwidth Generation (using recycling and local route).",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg start_conf\r\n"
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
    PARAM_GFA_VERBOSE_ID,
    "verbose",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do more printing.",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg start_conf verbose\r\n"
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
    PARAM_GFA_LBG_START_CONF_ID,
    "start_conf",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Configue the FAP to LBG mode (using recycling and local route).",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg start_conf\r\n"
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
    PARAM_GFA_LBG_VALIDATE_RATE_ID,
    "validate_rate",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Validate that the LBG rate is above 'rate_kb'",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg validate_rate rate_kb 12000000\r\n"
    "Results in validating LBG rate is above 12G.",
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
    PARAM_GFA_LBG_VALIDATE_RATE_RATE_ID,
    "rate_kb",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "Rate in Kbps. Rnage: 1 - 17000000",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg validate_rate rate_kb 12000000\r\n"
    "Results in validating LBG rate is above 12G.",
    "",
    {BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_LBG_SEND_PKT_ID,
    "send_pkt",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send packet in LBG mode (using recycling and local route).",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg send_pkt\r\n"
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
    PARAM_GFA_PAD_DATA_ID,
    "pad_data",
    (PARAM_VAL_RULES *)&Gfa_pad_data_vals[0],
    (sizeof(Gfa_pad_data_vals) / sizeof(Gfa_pad_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "Up to 32 bytes of data may be inserted. The rest of the data is used with default values.",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg send_pkt nof_pkts 1000 pad_data 0xFF 0xCC 0x12\r\n"
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
    PARAM_GFA_NOF_PKTS_ID,
    "nof_pkts",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_nof_pkts_defualt,
    0,0,0,0,0,0,0,
    "Number of pakcets to send.",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg send_pkt nof_pkts 1000\r\n"
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
    PARAM_GFA_PKT_BYTE_SIZE_ID,
    "pkt_byte_size",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MUST_APPEAR,(PARAM_VAL *)&Gfa_pkt_byte_size_defualt,
    0,0,0,0,0,0,0,
    "Packet byte size",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg send_pkt nof_pkts 1000 pkt_byte_size 100\r\n"
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
    PARAM_GFA_LBG_RESTORE_CONF_ID,
    "restore_conf",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Restore the FAP to before LBG mode.",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg restore_conf\r\n"
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
    PARAM_GFA_GET_RATE_ID,
    "get_rate",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get device BW rates.",
    "Examples:\r\n"
    "  gfa fap20v_1 lbg get_rate\r\n"
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
    PARAM_GFA_READ_AND_CLEAR_CPU_PKT_ID,
    "read_and_clear_cpu_pkt",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read and clear CPU packets.",
    "Examples:\r\n"
    "  gfa fap20v_1 read_and_clear_cpu_pkt\r\n"
    "Results in read and clear CPU packets from SOC_SAND_FAP20V.",
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
    PARAM_GFA_SWEEP_APP_ID,
    "demo_app_bring_up",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load SOC_SAND_FAP20V sweep application.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up\r\n"
    "Results in loading SOC_SAND_FAP20V sweep application.",
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
    PARAM_GFA_SPI4_SRC_CAL_LEN_ID,
    "spi4_source_cal_len",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_cal_len_vals[0],
    (sizeof(Gfa_fap20_spi4_cal_len_vals) / sizeof(Gfa_fap20_spi4_cal_len_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_source_cal_len_default,
    0,0,0,0,0,0,0,
    "Spi4 parameter CALENDAR_LEN, length of the calendar sequence,\r\n"
    "  in the source status channel.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_source_cal_len 64\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with spi4 calendar of length 64 in the source status channel.",
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
    PARAM_GFA_SPI4_SRC_CAL_M_ID,
    "spi4_source_cal_m",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_cal_m_vals[0],
    (sizeof(Gfa_fap20_spi4_cal_m_vals) / sizeof(Gfa_fap20_spi4_cal_m_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_source_cal_m_default,
    0,0,0,0,0,0,0,
    "Spi4 parameter CALENDAR_M, number of repetitions of the Calendar within\r\n"
    "  status frame, in the source status channel.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_source_cal_m 4\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with spi4 using 4 calendars in a status frame in the source status channel.",
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
    PARAM_GFA_SPI4_SNK_CAL_LEN_ID,
    "spi4_sink_cal_len",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_cal_len_vals[0],
    (sizeof(Gfa_fap20_spi4_cal_len_vals) / sizeof(Gfa_fap20_spi4_cal_len_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_sink_cal_len_default,
    0,0,0,0,0,0,0,
    "Spi4 parameter CALENDAR_LEN, length of the calendar sequence,\r\n"
    "  in the sink status channel.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_sink_cal_len 1\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with spi4 calendar of length 1 in the sink status channel.",
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
    PARAM_GFA_SPI4_SNK_CAL_M_ID,
    "spi4_sink_cal_m",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_cal_m_vals[0],
    (sizeof(Gfa_fap20_spi4_cal_m_vals) / sizeof(Gfa_fap20_spi4_cal_m_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_sink_cal_m_default,
    0,0,0,0,0,0,0,
    "Spi4 parameter CALENDAR_M, number of repetitions of the Calendar within\r\n"
    "  status frame, in the sink status channel.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_sink_cal_m 64\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with spi4 using 64 calendars in a status frame in the sink status channel.",
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
    PARAM_GFA_SPI4_SRC_TSCLK_SEL_ID,
    "spi4_source_tsclk_sel",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_tsclk_rsclk_vals[0],
    (sizeof(Gfa_fap20_spi4_tsclk_rsclk_vals) / sizeof(Gfa_fap20_spi4_tsclk_rsclk_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_tsclk_default,
    0,0,0,0,0,0,0,
    "Spi4 source TSCLKEdgeSel; 0 or 1.\r\n"
    "  TSCLKEdgeSel selects the edge of TSCLK where TSTAT changes.\n"
    "  When TSCLKEdgeSel = 1, TSTAT changes at the falling edge of TSCLK.\n"
    "  When TSCLKEdgeSel = 0, TSTAT changes at the rising edge of TSCLK.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_source_tsclk_sel 0\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with spi4 TSTAT changes at the rising edge of TSCLK.",
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
    PARAM_GFA_SPI4_SNK_RSCLK_SEL_ID,
    "spi4_sink_rsclk_sel",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_tsclk_rsclk_vals[0],
    (sizeof(Gfa_fap20_spi4_tsclk_rsclk_vals) / sizeof(Gfa_fap20_spi4_tsclk_rsclk_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_rsclk_default,
    0,0,0,0,0,0,0,
    "Spi4 sink RSCLKEdgeSel; 0 or 1.\r\n"
    "  RSCLKEdgeSel selects the edge of RSCLK where RSTAT changes.\n"
    "  When RSCLKEdgeSel = 1, RSTAT changes at the falling edge of RSCLK.\n"
    "  When RSCLKEdgeSel = 0, RSTAT changes at the rising edge of RSCLK.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_sink_rsclk_sel 0\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with spi4 RSTAT changes at the rising edge of RSCLK.",
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
    PARAM_GFA_SPI4_SRC_DATA_MAX_T_ID,
    "spi4_source_data_max_t",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_data_max_t_vals[0],
    (sizeof(Gfa_fap20_spi4_data_max_t_vals) / sizeof(Gfa_fap20_spi4_data_max_t_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_data_max_t_default,
    0,0,0,0,0,0,0,
    "Spi4 SourceDataMaxT; Specifies the nominal interval between training\r\n"
    "  sequences on the data path.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_source_data_max_t 15\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\r\n"
    "  with spi4 interval of 15 between training sequences\r\n"
    "  on the data path in the egress.",
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
    PARAM_GFA_SPI4_SRC_INIT_TRAIN_SEQ_ID,
    "spi4_source_init_train_seq",
    (PARAM_VAL_RULES *)&Gfa_fap20_spi4_init_train_seq_vals[0],
    (sizeof(Gfa_fap20_spi4_init_train_seq_vals) / sizeof(Gfa_fap20_spi4_init_train_seq_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_spi4_init_train_seq_default,
    0,0,0,0,0,0,0,
    "Spi4 source InitialTrainingSeq; Number of training sequences to be transmitted\r\n"
    "  when the interface is enabled.\r\n"
    "  This register is used only when the status channel is disabled\r\n"
    "  and the data source has no knowledge of the synchronization state\r\n"
    "  of the far-end data sink.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up spi4_source_init_train_seq 7\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with spi4 using 7 training sequences to be transmitted\r\n"
    "  when the interface is enabled.",
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
    PARAM_GFA_BW_RATE_ID,
    "nominal_rate_per_port_mbps",
    (PARAM_VAL_RULES *)&Gfa_nominal_rate_per_port_mbps_vals[0],
    (sizeof(Gfa_nominal_rate_per_port_mbps_vals) / sizeof(Gfa_nominal_rate_per_port_mbps_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_nominal_rate_per_port_mbps_default,
    0,0,0,0,0,0,0,
    "Nominal (physical line) rate per scheduler port.\r\n"
    "  In Mega-Bits-Sec units.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up nominal_rate_per_port_mbps 1000\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up,\n"
    "  with 1000Mbps per scheduler port.",
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
    PARAM_GFA_CREDIT_SIZE_ID,
    "credit_size",
    (PARAM_VAL_RULES *)&Gfa_fap20_credit_size_vals[0],
    (sizeof(Gfa_fap20_credit_size_vals) / sizeof(Gfa_fap20_credit_size_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_credit_size_default,
    0,0,0,0,0,0,0,
    "Credit size in bytes; credit_256 = 256 bytes, credit_512 = 512 bytes, etc...",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up credit_size credit_256\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, with credit size 256 bytes.",
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
    PARAM_GFA_USE_COEXIST_ID,
    "use_coexist",
    (PARAM_VAL_RULES *)&Gfa_true_false_vals[0],
    (sizeof(Gfa_true_false_vals) / sizeof(Gfa_true_false_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_false_default,
    0,0,0,0,0,0,0,
    "TRUE-Use CoExist application. FALSE-Do not use CoExist.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_coexist true\r\n"
    "Results in loading SOC_SAND_FAP20V demo application, using CoExist.",
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
    PARAM_GFA_USE_DUNE_FTG_ID,
    "use_dune_ftg",
    (PARAM_VAL_RULES *)&Gfa_true_false_vals[0],
    (sizeof(Gfa_true_false_vals) / sizeof(Gfa_true_false_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_true_default,
    0,0,0,0,0,0,0,
    "TRUE-Use Dune's FTG. FALSE-Do not use Dune's FTG.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_zbt true use_dune_ftg true\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, using Dune's FTG.\n\r"
    "All configuration in the SOC_SAND_FAP20V related to the FTG are loaded to the SOC_SAND_FAP20V.\n\r"
    "Example of such configuration is the INQ thresholds.",
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
    PARAM_GFA_USE_ZBT_ID,
    "use_zbt",
    (PARAM_VAL_RULES *)&Gfa_true_false_vals[0],
    (sizeof(Gfa_true_false_vals) / sizeof(Gfa_true_false_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "TRUE-Use ZBT. FALSE-Do not use ZBT.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_zbt true\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, using zbt.",
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
    PARAM_GFA_MAP_CPU_PORT_ID,
    "map_cpu_port",
    (PARAM_VAL_RULES *)&Gfa_true_false_vals[0],
    (sizeof(Gfa_true_false_vals) / sizeof(Gfa_true_false_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MAP CPU PORT. FALSE-Do not map CPU port.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up map_cpu_port true\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, with CPU port as port 0.",
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
    PARAM_GFA_USE_OFC_ID,
    "use_ofc",
    (PARAM_VAL_RULES *)&Gfa_true_false_vals[0],
    (sizeof(Gfa_true_false_vals) / sizeof(Gfa_true_false_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "TRUE-Use SLA-OFC. FALSE-Do not use SLA-OFC (Port-Flow-Control Out-Of-Band).",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_zbt true use_ofc true\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, using zbt, using sla.",
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
    PARAM_GFA_FAP_ID,
    "fap_id",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_fap_id_default,
    0,0,0,0,0,0,0,
    "FAP ID.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up nof_faps 1 fap_id 0\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, with 1 FAP that has FAP ID 0.",
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
    PARAM_GFA_MAX_NOF_FAPS_ID,
    "nof_faps",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of FAP in the All-To-All application.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_zbt true use_ofc true nof_faps 1\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, with 1 FAP in the All-To-All application.",
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
    PARAM_GFA_MAX_NOF_PORTS_ID,
    "nof_ports",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of Ports per SPI IF in the All-To-All application.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_zbt true use_ofc true nof_faps 1 nof_ports 16\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, with 1 FAP\n\r"
    "  and 16 ports per SPI interfacein the All-To-All application.",
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
    PARAM_GFA_ZBT_NOF_BUFF_MEM_FIELD_ID,
    "zbt_nof_buff_mem_field",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of Ports per SPI IF in the All-To-All application.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_zbt true use_ofc true nof_faps 1\r\n"
    "    nof_ports 16 zbt_nof_buff_mem_field 1\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, with ZBT config of 1",
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
    PARAM_GFA_DRAM_BUFF_SIZE_FIELD_ID,
    "dram_buff_size_field",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of Ports per SPI IF in the All-To-All application.",
    "Examples:\r\n"
    "  gfa fap20v_1 demo_app_bring_up use_zbt true use_ofc true nof_faps 1\r\n"
    "    nof_ports 16 dram_buff_size_field 1\r\n"
    "Results in loading SOC_SAND_FAP20V demo application bring-up, with DRAM buff size field of 1",
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
    PARAM_GFA_FAP20V_1_ZBT_TEST_RW_ID,
    "zbt_test_rw",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write ZBT test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  gfa fap20v_1 zbt_test_rw\r\n"
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
    PARAM_GFA_FAP20V_1_INQ_TEST_RW_ID,
    "inq_test_rw",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write INQ test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  gfa fap20v_1 inq_test_rw\r\n"
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
    PARAM_GFA_CALIBRATE_READ_FROM_NV_ID,
    "calibrate_params_from_nv_read",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read the stored calibration parameters from the Board NvRam.",
    "Examples:\r\n"
    "  gfa fap20v_1 calibrate_params_from_nv_read\r\n"
    " Read the stored calibration parameters from the Board NvRam..\n\r",
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
    PARAM_GFA_CALIBRATE_CLEAR_NV_ID,
    "calibrate_params_from_nv_clear",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clear the stored calibration parameters from the Board NvRam.",
    "Examples:\r\n"
    "  gfa fap20v_1 calibrate_params_from_nv_clear\r\n"
    " Clear the stored calibration parameters from the Board NvRam..\n\r",
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
    PARAM_GFA_SEARCH_DRAM_DELAY_LINE_AUTO_FULL_ID,
    "search_dram_delay_line_auto_full",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Search for DRAM delay lines - FULL.",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line_auto_full delay_in_milisec 5000\r\n"
    "Search for DRAM delay lines -- Full.\n\r",
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
    PARAM_GFA_SEARCH_DRAM_DELAY_LINE_AUTO_ID,
    "search_dram_delay_line_auto",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Search for DRAM delay lines.",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line_auto delay_in_milisec 5000\r\n"
    "Search for DRAM delay lines.\n\r",
    "",
    {BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_LBG_ON_ID,
    "lbg_on",
    (PARAM_VAL_RULES *)&Gfa_true_false_vals[0],
    (sizeof(Gfa_true_false_vals) / sizeof(Gfa_true_false_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_false_default,
    0,0,0,0,0,0,0,
    "TRUE- Local BW Generation - ON. No need to inject traffic from outside.",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line_auto delay_in_milisec 5000 lbg_on true\r\n"
    "Search for DRAM delay lines. No need to inject traffic from outside.",
    "",
    {BIT(9) | BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_INVERSE_SSO_ID,
    "inverse_sso",
    (PARAM_VAL_RULES *)&Gfa_true_false_vals[0],
    (sizeof(Gfa_true_false_vals) / sizeof(Gfa_true_false_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_false_default,
    0,0,0,0,0,0,0,
    "TRUE- Inverse the SSO pattern - ON use regular SSO.",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line_auto delay_in_milisec 5000 lbg_on true inverse_sso true\r\n"
    "Search for DRAM delay lines. Inverse the SSO pattern.",
    "",
    {BIT(9) | BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_SEARCH_DRAM_DELAY_LINE_ID,
    "search_dram_delay_line",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Search for DRAM delay lines.",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line delay_in_milisec 5000 start_taps 10 2 0 0 0\r\n"
    "Search for DRAM delay lines.\n\r",
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
    PARAM_GFA_NOF_TAP_TO_START_ID,
    "tap_to_start",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The fitsrt TAP to start looking.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line delay_in_milisec 5000 start_taps 10 2 0 0 0 tap_to_start 0\r\n"
    "The first TAP is 0. I.e., the **first** TAP.",
    "",
    {BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_MILISEC_ID,
    "delay_in_milisec",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_milisec_defualt,
    0,0,0,0,0,0,0,
    "How much milisec we send traffic, before check for errors.\r\n",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line delay_in_milisec 5000 start_taps 10 2 0 0 0\r\n"
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
    PARAM_GFA_START_TAPS_ID,
    "start_taps",
    (PARAM_VAL_RULES *)&Gfa_start_taps_vals[0],
    (sizeof(Gfa_start_taps_vals) / sizeof(Gfa_start_taps_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The start TAPs. Each can be from 0 to 11.",
    "Examples:\r\n"
    "  gfa fap20v_1 search_dram_delay_line delay_in_milisec 5000 start_taps 10 2 0 0 0\r\n"
    "Start TAPs are 10, 2, 0, 0,0",
    "",
    {BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_FAP20V_1_CPU_TEST_RW_ID,
    "cpu_test_rw",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write CPU test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  gfa fap20v_1 cpu_test_rw\r\n"
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
    PARAM_GFA_FAP20V_1_IDDR_TEST_RW_ID,
    "iddr_test_rw",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read/write IDDR test SOC_SAND_FAP20V.",
    "Examples:\r\n"
    "  gfa fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
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
    PARAM_GFA_FAP20V_1_IDDR_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset in the IDDR.",
    "Examples:\r\n"
    "  gfa fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
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
    PARAM_GFA_NOF_ITERATION_ID,
    "nof_iteration",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of read/writes per test.",
    "Examples:\r\n"
    "  gfa fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
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
    PARAM_GFA_PAD_DATA_ID,
    "data_to_test",
    (PARAM_VAL_RULES *)&Gfa_iddr_data_vals[0],
    (sizeof(Gfa_iddr_data_vals) / sizeof(Gfa_iddr_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Data to read/write.",
    "Examples:\r\n"
    "  gfa fap20v_1 iddr_test_rw offset 0x1 data_to_test 0x0 0xEDA1 nof_iteration 10000\r\n"
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
    PARAM_GFA_FAP20V_1_CLR_CNT_INT_ID,
    "clear_counter_and_interrupts",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
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
    PARAM_GFA_DO_STATIC_SPI_CALIB,
    "run_spi_static_calibration",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do SPI static calibration.",
    "Examples:\r\n"
    "  gfa fap20v_1 run_spi_static_calibration\r\n"
    "Results in running SPI static calibration.",
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
    PARAM_GFA_QUEUE_SCAN_ID,
    "queue_size_scan",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Device ID to scan.",
    "",
    "",
    { 0, BIT(9) | BIT(10) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_QUEUE_SCAN_EN_ID,
    "enable",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    { 0, BIT(9) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_QUEUE_SCAN_DIS_ID,
    "disable",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    { 0, BIT(9) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_QUEUE_SCAN_PRINT_ID,
    "print",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    { 0, BIT(9) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_QUEUE_SCAN_SET_PARAM_ID,
    "set_params",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    { 0, BIT(10) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_QUEUE_SCAN_NOF_REWRITE_ID,
    "number_of_q_size_rewrites",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    " Default is 1.",
    "",
    "",
    { 0, BIT(10) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_QUEUE_SCAN_READ_SIZE_ID,
    "read_after_write",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    " 0- not to print.",
    "",
    "",
    { 0, BIT(10) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_QUEUE_SCAN_Q_SIZE_ID,
    "max_q_size",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    " KBytes. The Max Valid Q size.",
    "",
    "",
    { 0, BIT(10) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_RESET_ID,
    "reset",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reset command.",
    "Examples:\r\n"
    "  gfa fap20v_1 reset reset_down_milisec 100 \r\n"
    "Results in resetting the PP-1 on the board.\n\r"
    "  Writing '0' to the reset bit (device off),\n\r"
    "  wait 100 mili-sec and than write '1' to the reset bit (device ON).\n\r"
    "  Note, on SOC_SAND_FAP20V - soecial init sequnce is done after device set to be in ON mode.",
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
    PARAM_GFA_STAY_DOWN_ID,
    "stay_in_reset",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Order the device to stay in reset.\r\n",
    "Examples:\r\n"
    "  line_card pp_1 reset stay_in_reset\r\n"
    "Results in resetting the PP-1 on the board.\n\r"
    "  The device will be set out of the reset mode.",
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
    PARAM_GFA_MILISEC_ID,
    "reset_down_milisec",
    (PARAM_VAL_RULES *)&Gfa_indefinite_num_vals[0],
    (sizeof(Gfa_indefinite_num_vals) / sizeof(Gfa_indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Gfa_reset_milisec_defualt,
    0,0,0,0,0,0,0,
    "How much milisec the device should stay in reset before\n\r"
    "  taking it out of reset.\r\n",
    "Examples:\r\n"
    "  line_card pp_1 reset reset_down_milisec 100 \r\n"
    "Results in resetting the PP-1 on the board.\n\r"
    "  Writing '0' to the reset bit, wait 100 mili-sec and than write '1' to the reset bit.",
    "",
    {BIT(0)},
    LAST_ORDINAL,
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


