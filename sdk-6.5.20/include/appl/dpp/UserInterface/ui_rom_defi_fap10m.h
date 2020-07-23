/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_FAP10M_INCLUDED
/* { */
#define UI_ROM_DEFI_FAP10M_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * User Interface include file.
 */
#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>

#include <ui_pure_defi_fap10m.h>
#include <soc/dpp/SAND/Management/sand_chip_consts.h>
#include <ChipSim/chip_sim_fe200_chip_defines_indirect.h>

#include <ChipSim/chip_sim.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_chip_defines.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_general.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_statistics.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_diagnostics.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_spatial_multicast.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_configuration.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_configuration.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_scheduler_flows.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <appl/diag/dpp/dune_rpc.h>

#include <soc/dpp/SOC_SAND_FE200/fe200_api_statistics.h>

/*
 * UI definition{
 */

/********************************************************
*NAME
*  Fap10m_pause_after_read_mem_read_vals
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "PAUSE_AFTER_READ"  IN THE CONTEXT
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
   PARAM_VAL_RULES
     Fap10m_pause_after_read_mem_read_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000000,0,
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
*  Fap10m_pause_after_read_mem_read_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "PAUSE_AFTER_READ"  IN THE CONTEXT
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
     Fap10m_pause_after_read_mem_read_default
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
*  Fap10m_nof_times_mem_read_vals
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "NOF_TIME"  IN THE CONTEXT
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
   PARAM_VAL_RULES
     Fap10m_nof_times_mem_read_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        10000,0,
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
*  Fap10m_nof_times_mem_read_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "PAUSE_AFTER_READ"  IN THE CONTEXT
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
     Fap10m_nof_times_mem_read_default
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
*  Fap10m_pp_port_default
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
     Fap10m_pp_port_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)5
  }
}
#endif
;
/********************************************************
*NAME
*  Fap10m_vlan_id_vals
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
     Fap10m_vlan_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        BIT(12)-1,0,
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
*  Fap10m_vlan_id_default
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
     Fap10m_vlan_id_default
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
*  Fap10m_wfq0_weight_default
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
     Fap10m_wfq0_weight_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)50
  }
}
#endif
;
/********************************************************
*NAME
*  FAP10M_initial_output_link_num_vals
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
     FAP10M_initial_output_link_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        8,0,
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
*  FAP10M_fe1_link_num_vals
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
     FAP10M_fe1_link_num_vals[]
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
*  Fap10m_queue_label_type_default
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
     Fap10m_queue_label_type_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FAP10M_DESTINATION_MAPPING,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"destination_mapping"
  }
}
#endif
;
/********************************************************
*NAME
*  Fap10m_Interrupt_mockup_default
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
     Fap10m_Interrupt_mockup_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  2,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)"dont_mockup_interruupts"
  }
}
#endif
;
/********************************************************
*NAME
*  Fap10m_fe1_link_default
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
     Fap10m_fe1_link_default
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
*  FAP10M_fe2_link_num_vals
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
     FAP10M_fe2_link_num_vals[]
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
*  Fap10m_fe2_link_default
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
     Fap10m_fe2_link_default
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
*  Fap10m_fe3_link_num_vals
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
     Fap10m_fe3_link_num_vals[]
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
*  Fap10m_fe3_link_default
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
     Fap10m_fe3_link_default
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
*  Fap10m_source_chip_id_num_val
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
     Fap10m_source_chip_id_num_val[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_FAPS_IN_SYSTEM-1 /* 255 */,0,
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
*  Fap10m_source_chip_id_default
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
     Fap10m_source_chip_id_default
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
*  Fap10m_cell_body_num_vals
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
     Fap10m_cell_body_num_vals[]
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
*  Fap10m_list_of_links_num_vals
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
     Fap10m_list_of_links_num_vals[]
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
*  Fap10m_entity_type
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
     Fap10m_entity_type[]
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    fe1 entity type.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    fe2 entity type.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    fe3 entity type.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    fip entity type.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    fop entity type.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    don't care what's entity type.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_local_loopback_on_vals
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
     Fap10m_local_loopback_on_vals[]
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    loppback mode is set to local.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    loppback mode is not set to local.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_activate_prbs_generator_vals
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
     Fap10m_activate_prbs_generator_vals[]
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate prbs generator.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Deactivate prbs generator.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_activate_prbs_checker_vals
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
     Fap10m_activate_prbs_checker_vals[]
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate prbs checker.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Deactivate prbs checker.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  FAP10M_mac_counter_type_vals
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
     FAP10M_mac_counter_type_vals[]
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Counts all cells.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Counts control cells.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Counts packet data cells.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Counts empty data cells.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_enable_disable
*TYPE: BUFFER
*DATE: 15/1/2005
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
     Fap10m_enable_disable[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enable",
    {
      {
        TRUE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Enable RX direction fabric link flow control.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
        FALSE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable RX direction fabric link flow control.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_tx_fc
*TYPE: BUFFER
*DATE: 15/1/2005
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
     Fap10m_tx_fc[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enable",
    {
      {
        TRUE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Enable TX direction fabric link flow control.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
        FALSE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable TX direction fabric link flow control.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_serdes_number_num_vals
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
     Fap10m_serdes_number_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NUMBER_OF_LINKS-1,0,
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
*  Fap10m_prbs_generator_links
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
     Fap10m_prbs_generator_links[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NUMBER_OF_LINKS-1,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        FAP10M_NUMBER_OF_LINKS,
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
*  Fap10m_serdes_number_default
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
     Fap10m_serdes_number_default
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
*  Fap10m_serdes_amplitude_num_vals
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
     Fap10m_serdes_amplitude_num_vals[]
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
*  Fap10m_serdes_amplitude_default
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
     Fap10m_serdes_amplitude_default
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
*  Fap10m_serdes_common_mode_num_vals
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
     Fap10m_serdes_common_mode_num_vals[]
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
*  Fap10m_serdes_common_mode_default
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
     Fap10m_serdes_common_mode_default
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
*  Fap10m_serdes_main_control_num_vals
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
     Fap10m_serdes_main_control_num_vals[]
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
*  Fap10m_serdes_main_control_default
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
     Fap10m_serdes_main_control_default
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
*  Fap10m_serdes_power_down_num_vals
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
     Fap10m_serdes_power_down_num_vals[]
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
*  Fap10m_serdes_power_down_default
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
     Fap10m_serdes_power_down_default
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
*  Fap10m_serdes_pre_emphasis_num_vals
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
     Fap10m_serdes_pre_emphasis_num_vals[]
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
*  Fap10m_percent
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
     Fap10m_percent[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100,0,
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
*  Fap10m_sp_active_links
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
     Fap10m_sp_active_links[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_ENABLE_ALL_LINKS,0,
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
*  Fap10m_sms_max_bandwidth
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
     Fap10m_sms_max_bandwidth[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        160000,0,
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
*  Fap10m_spatial_multicast_id
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
     Fap10m_spatial_multicast_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_SPATIAL_MULTICAST_NOF_GROUPS-1,0,
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
*  Fap10m_max_nof_ports
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
     Fap10m_max_nof_ports[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_SCH_PORTS-2,0,
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
*  Fap10m_spatial_multicast_queue
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
     Fap10m_spatial_multicast_queue[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_SP_QUEUE_3,FAP10M_SP_QUEUE_0,
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
*  Fap10m_flow_id
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
     Fap10m_flow_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_FLOWS-1,0,
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
*  fap10m_scheduler_recycle_flow_weight_vals
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
     fap10m_scheduler_recycle_flow_weight_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        63, 0,
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
*  Fap10m_profile_id
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
     Fap10m_profile_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_PRE_DEFINED_PROFILES-1,0,
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
*  Fap10m_nof_queues
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
     Fap10m_nof_queues[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_QUEUES,0,
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
*  Fap10m_queue_id
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
     Fap10m_queue_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_QUEUES-1,0,
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
*  Fap10m_packet_data
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
     Fap10m_packet_data[]
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
*  Fap10m_recycle_multicast_tree_id
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
     Fap10m_recycle_multicast_tree_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_RECYCLE_MULTICAST_NOF_GROUPS-1,0,
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
*  Fap10m_recycle_multicast_queues
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
     Fap10m_recycle_multicast_queues[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_QUEUES-1,0,
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
*  Fap10m_recycle_queue_index
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
     Fap10m_recycle_queue_index[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_RECYCLING_QUEUES-1,0,
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
*  Fap10m_fap_id
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
     Fap10m_fap_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_FAPS_IN_SYSTEM-1,0,
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
*  Fap10m_edf1_prior
*TYPE: BUFFER
*DATE: 15/1/2005
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
     Fap10m_edf1_prior[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enable",
    {
      {
        TRUE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    EDF1 Wheel get strict priority over WFQ1.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
        FALSE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    EDF1 Wheel doesn't get strict priority over WFQ1.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_chop_en
*TYPE: BUFFER
*DATE: 15/1/2005
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
     Fap10m_chop_en[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enable",
    {
      {
        TRUE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Enable chopping of big packets.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
        FALSE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable chopping of big packets.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_pause_after_read_mem_read_vals
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "PAUSE_AFTER_READ"  IN THE CONTEXT
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
   PARAM_VAL_RULES
     Fap10m_chop_max_mult_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_MULTICAST_GROUPS_FROM_PP-1,0,
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
*  Fap10m_chop_max_mult_id_default
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
     Fap10m_chop_max_mult_id_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)FAP10M_SPATIAL_MULTICAST_NOF_GROUPS-1
  }
}
#endif
;
/********************************************************
*NAME
*  Fap10m_sub_flow_types
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
     Fap10m_sub_flow_types[]
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    EDF0 Wheel at the scheduler.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    EDF1 Wheel at the scheduler.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "edf0_by_percentage",
    {
      {
        FAP10M_EDF0_PERCENTAGE_FLOW,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    EDF0 Wheel at the scheduler. Configured by percentage",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "edf1_by_percentage",
    {
      {
        FAP10M_EDF1_PERCENTAGE_FLOW,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    EDF1 Wheel at the scheduler. Configured by percentage",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    WFQ1 Wheel at the scheduler.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_sub_flow_values
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
     Fap10m_sub_flow_values[]
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
*  Fap10m_queue_type
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
     Fap10m_queue_type[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_QUEUE_D,FAP10M_QUEUE_A,
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
*  Fap10m_auto_queue_types
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
     Fap10m_auto_queue_types[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_QUEUE_D,FAP10M_QUEUE_A,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        FAP10M_FABRIC_CLASS_D,
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
*  Fap10m_max_queue_size
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
     Fap10m_max_queue_size[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        (1<<22)*FAP10M_QDP_WORD_BYTE_SIZE /* 2^22 * 16 (22 bit for queue size, word is 16 byte.)*/,0,
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
*  Fap10m_max_packet_size
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
     Fap10m_max_packet_size[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_WRED_MAX_PACKET_SIZE, 0,
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
*  Fap10m_min_avrg_th
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
     Fap10m_min_avrg_th[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_QWP_MIN_AVG_MAX*FAP10M_TH_WRED_GRANULARITY ,0,
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
*  Fap10m_min_avrg_th
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
     Fap10m_max_avrg_th[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_QWP_MAX_AVG_MAX*FAP10M_TH_WRED_GRANULARITY ,0,
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
*  Fap10m_drop_precedence
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
     Fap10m_drop_precedence[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        3 ,0,
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
*  Fap10m_flow_stat_msg
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
     Fap10m_flow_stat_msg[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        (1<<FAP10M_A_QTP_STS_MSG_MAX)*16, 0,
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
*  Fap10m_flow_slow_th
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
     Fap10m_flow_slow_th[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        (1<<FAP10M_QTP_STS_SLW_MAX)*16 ,0,
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
*  Fap10m_flow_slow_hyster
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
     Fap10m_flow_slow_hyster[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        (1<<FAP10M_QTP_STS_SLW_MAX)*16 ,0,
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
*  Fap10m_flow_slow_hyster_default
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
     Fap10m_flow_slow_hyster_default
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
*  Fap10m_red_exp_weight
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
     Fap10m_red_exp_weight[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_QTP_RED_EXP_MAX, 0,
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
*  Fap10m_fabric_class_entry
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
     Fap10m_fabric_class_entry[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_FABRIC_CLASS_D, 0,
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
*  Fap10m_fabric_class
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
     Fap10m_fabric_class[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "fabric_class_a",
    {
      {
        FAP10M_FABRIC_CLASS_A,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    PFH class {0,...,7} mapped to fabric class 0.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fabric_class_b",
    {
      {
        FAP10M_FABRIC_CLASS_B,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    PFH class {0,...,3} mapped to fabric class 0.\n\r"
              "    PFH class {4,...,7} mapped to fabric class 1.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fabric_class_c",
    {
      {
        FAP10M_FABRIC_CLASS_C,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    PFH class {0,1} mapped to fabric class 0.\n\r"
              "    PFH class {2,3} mapped to fabric class 1.\n\r"
              "    PFH class {4,5} mapped to fabric class 2.\n\r"
              "    PFH class {6,7} mapped to fabric class 3.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fabric_class_d",
    {
      {
        FAP10M_FABRIC_CLASS_D,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    PFH class X mapped to fabric class X,\n\r"
              "    that is, No translation occurs.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_queue_base_label
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
     Fap10m_queue_base_label[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "flow_mapping",
    {
      {
        FAP10M_FLOW_MAPPING,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Flow mapping at the LBP.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "destination_mapping",
    {
      {
        FAP10M_DESTINATION_MAPPING,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Destination mapping at the LBP.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_nominal_bandwidth_vals
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
     Fap10m_nominal_bandwidth_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        160000,0,
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
*  Fap10m_credit_bandwidth_vals
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
     Fap10m_credit_bandwidth_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        160000,0,
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
*  Fap10m_scheduler_port_id_vals
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
     Fap10m_scheduler_port_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_SCH_PORTS-1,0,
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
*  Fap10m_scheduler_port_id_no_recyle_vals
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
     Fap10m_scheduler_port_id_no_recyle_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NOF_SCH_PORTS-2,0,
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
*  Fap10m_serdes_pre_emphasis_default
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
     Fap10m_serdes_pre_emphasis_default
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
*  Fap10m_logical_mac_field
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
     Fap10m_logical_mac_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "commas_number",
    {
      {
        FAP10M_CM_BRST_SIZE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Number of consecutive commas to be transmitted.\n\r"
              "    0 means no commas will be transmitted, if data is available.\n\r"
              "    Number of commas to be transmitted is ((CmBrstSize+1)*2).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "commas_interval",
    {
      {
        FAP10M_CM_TX_PERIOD_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Number of cells between consecutive comma sequences.\n\r"
              "    Configuring 0 in this register means that no comma \n\r"
              "    sequences are generated. Number of commas to be \n\r"
              "    transmitted is 2^CmTxPeriod.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_logical_sch_field
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
     Fap10m_logical_sch_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enable_sub_flow",
    {
      {
        FAP10M_SCH_EN_SUB_FLOW_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Enables mapping a flow to sub-flows. This bit should be asserted\n\r"
              "    by sophisticated users, to allow greater flexibility in assigning\n\r"
              "    flows to schedulers. Default value is 0.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "port_cfg",
    {
      {
        FAP10M_SCH_EN_PORT_CFG_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Each one of the 8 bits represents the configuration of an 8-port\n\r"
              "    group (out of 64 possible ports). Each group can be configured as 1 or 8\n\r"
              "     ports. A bit assertion, sets a 1-port configuration, i.e., a timeline\n\r"
              "    of 32K clocks is available for that port group. If the bit is set to\n\r"
              "    0, a 4096 time-line is available for that port group. Default value is 0.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "min_slow_tti",
    {
      {
        FAP10M_SCH_EN_MIN_SLOW_TTI_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The minimum TTI for an EDF flow in slow state (to avoid waste of\n\r"
              "    credits when a flow state switches between Off and On).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "min_slow_wt",
    {
      {
        FAP10M_SCH_EN_MIN_SLOW_WT_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The minimum weight for a WFQ flow in slow state (to avoid waste\n\r"
              "    of credits when a flow state switches between Off and On).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_logical_external_dram_field
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
     Fap10m_logical_external_dram_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "queue_controller_init_trig",
    {
      {
        FAP10M_QUEUE_CONTROLLER_INIT_TRIG_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    When set to 1, it triggers initialization of Queue Cntr data structures.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "pkt_crc_ena",
    {
      {
        FAP10M_PKT_CRC_ENA_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Packet CRC Enable.\n\r"
              "    If this bit is asserted, 2 bytes CRC is added/removed when the packet\n\r"
              "    is stored/removed from external memory.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "qdp_buff_size",
    {
      {
        FAP10M_QDP_BUFF_SIZE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Buffer Size Options are:\n\r"
              "      0 = 256  bytes (8  fabric cells),\n\r"
              "      1 = 512  bytes (16 fabric cells),\n\r"
              "      2 = 1024 bytes (32 fabric cells),\n\r"
              "      3 = 2048 bytes (64 fabric cells)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "top_que_blk_0",
    {
      {
        FAP10M_TOP_QUE_BLK_0_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All queues from 0 to address top_que_blk_0 are in block-buffer0.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "top_que_blk_1",
    {
      {
        FAP10M_TOP_QUE_BLK_1_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All queues from top_que_blk_0+1 to top_que_blk_1 are in block-buffer1.\n\r"
              "    All queues from top_que_blk_1+1 to top_que_blk_2 are in block-buffer2.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "top_que_blk_2",
    {
      {
        FAP10M_TOP_QUE_BLK_2_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All queues from top_que_blk_1+1 to top_que_blk_2 are in block-buffer2.\n\r"
              "    All queues from top_que_blk_2+1 to 8K-1 are in block-buffer3.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "top_buff_blk_0",
    {
      {
        FAP10M_TOP_BUFF_BLK_0_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block-buffer1 is between 0 and top_buff_blk_0.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "top_buff_blk_1",
    {
      {
        FAP10M_TOP_BUFF_BLK_1_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block-buffer2 is between top_buff_blk_0+1 and top_buff_blk_1.\n\r"
              "    Block-buffer3 is between top_buff_blk_1+1 and top_buff_blk_2.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "top_buff_blk_2",
    {
      {
        FAP10M_TOP_BUFF_BLK_2_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block-buffer3 is between top_buff_blk_1+1 and top_buff_blk_2.\n\r"
              "    Block-buffer4 is between top_buff_blk_2+1 and external DRAM total size",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "credit_byt_size",
    {
      {
        FAP10M_CREDIT_VALUE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    credit byte size:\n\r"
              "      0 indicates 256 bytes.\n\r"
              "      1 indicates 512 bytes.\n\r"
              "      2 indicates 1024 bytes.\n\r"
              "      3 indicates 2048 bytes.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "packet_header_size",
    {
      {
        FAP10M_QDP_HDR_CMP_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"   The amount of bytes to be subtracted from the packet\n\r"
              "   size for credit accounting.:\n\r"
              "      0 indicates 0 bytes.\n\r"
              "      1 indicates 16 bytes.\n\r"
              "      2 indicates 32 bytes.\n\r"
              "      3 indicates 48 bytes.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "send_status_on_discasrd",
    {
      {
        FAP10M_QDP_STS_ON_DISCARD_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"   Enable sending flow-status message whenever there is a Discard.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_logical_interrupt_mask_field
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
     Fap10m_logical_interrupt_mask_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "unrch_dest_evt_mask",
    {
      {
        FAP10M_UN_RCH_DEST_EVT_MASK_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'UnrchDestEvt_mask\' bit on \'Interrupt Mask\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "lnk_actv_msk_cng_mask",
    {
      {
        FAP10M_LNK_ACTV_MSK_CNG_MASK_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'LnkActvMskCng_mask\' field on \'Interrupt Mask\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "cpu_data_cell_fne_mask",
    {
      {
        FAP10M_CPU_DATA_CELL_FNE_MASK_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'CPUDataCellFNE_mask\' field on \'Interrupt Mask\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "src_dv_cng_link_mask",
    {
      {
        FAP10M_SRC_DV_CNG_LINK_MASK_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'SrcDvCngLink_mask\' field on \'Interrupt Mask\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_b_logical_sms_field
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
     Fap10m_b_logical_sms_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "leaky_bucket_max_cnt",
    {
      {
        FAP10M_LKY_BKT_MAX_CNT_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The maximum value a leaky bucket can reach.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_threshold",
    {
      {
        FAP10M_LKY_BKT_CNG_TH_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The threshold above a leaky bucket will be considered congested.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_q_0_1",
    {
      {
        FAP10M_LKY_BKT_DCR_RATE_1_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The numbers of clocks to decrement leaky bucket 1.\n\r"
              "    If leaky_bucket_q_0_1 is set to 0, the bucket will not decrement.\n\r"
              "    Leaky bucket 1 is associated with spatial multicast queues 0 and 1.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_q_2",
    {
      {
        FAP10M_LKY_BKT_DCR_RATE_2_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The numbers of clocks to decrement leaky bucket 2.\n\r"
              "    If leaky_bucket_q_2 is set to 0, the bucket will not decrement.\n\r"
              "    Leaky bucket 1 is associated with spatial multicast queue 2.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_q_3",
    {
      {
        FAP10M_LKY_BKT_DCR_RATE_3_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The numbers of clocks to decrement leaky bucket 3.\n\r"
              "    If leaky_bucket_q_3 is set to 0, the bucket will not decrement.\n\r"
              "    Leaky bucket 1 is associated with spatial multicast queue 3.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "spatial_min_interval",
    {
      {
        FAP10M_SPATIAL_Q_CREDIT_MIN_INTERVAL_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The minimum number of clocks between 2 consecutive\n\r"
              "    credits awarded to queues 0, 1, 2 and 3.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "hold_mci",
    {
      {
        FAP10M_HOLD_MCI_PRD_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The period that the SMS holds an MCI indication active.\n\r"
              "    Setting this parameter to 0 disables this, and the\n\r"
              "    indication comes directly from the FE.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_a_logical_sms_field
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
     Fap10m_a_logical_sms_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "slow_multicast_weight",
    {
      {
        FAP10M_SLOW_MULTICAST_WEIGHT_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Slow Multicast Weight\' field on \'Spatial Multicast Enablers\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fast_multicast_weight",
    {
      {
        FAP10M_FAST_MULTICAST_WEIGHT_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Fast Multicast Weight\' field on \'Spatial Multicast Enablers\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "multicast_credit_gen_rate",
    {
      {
        FAP10M_MULTICAST_CREDIT_GEN_RATE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'MultiCrdGenRate\' field on \'Multicast credit generation rate\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "unicast_weight",
    {
      {
        FAP10M_UNICAST_WEIGHT_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Unicast Weight\' field on \'Spatial Multicast Enablers\' register.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_free_threshold",
    {
      {
        FAP10M_LEAKY_BUCKET_FREE_THRESHOLD_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'LkyBktFreeTrh\' field on \'Spatial Multicast Congestion Thresholds\' register.\n\r"
              "    Below this threshold the bucket is not in a congested state.\n\r"
              "    If LkyBktFreeTrh = 255 the links are always down.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_congestion_threshold",
    {
      {
        FAP10M_LEAKY_BUCKET_CONGESTION_THRESHOLD_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'LkyBktCngTrh\' field on \'Spatial Multicast Congestion Thresholds\' register.\n\r"
              "    Above this threshold the bucket is in a congested state.\n\r"
              "    If LkyBktCngTrh = 0 the links are always up.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_decrement_rate",
    {
      {
        FAP10M_LEAKY_BUCKET_DECREMENT_RATE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'LkyBktDcrRate\' field on \'Spatial Multicast Congestion Thresholds\' register.\n\r"
              "    Indicates the rate at which the leaky bucket is decremented. For\n\r"
              "    every programmable number of clocks, up to 64, all buckets are\n\r"
              "    decrement by one, down to zero. The value zero means that the\n\r"
              "    bucket value never decreases.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "leaky_bucket_max_count",
    {
      {
        FAP10M_LEAKY_BUCKET_MAX_COUNT_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'LkyBktMaxCnt\' field on \'Spatial Multicast Congestion Thresholds\' register.\n\r"
              "    Indicates the maximum value of the leaky bucket.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_logical_initialization_triggers_field
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
     Fap10m_logical_initialization_triggers_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "scheduler_init_trig",
    {
      {
        FAP10M_SCHEDULER_INIT_TRIG_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    When set to 1, initialize all Scheduler structures.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "label_processor_init_trig",
    {
      {
        FAP10M_LABEL_PROCESSOR_INIT_TRIG_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    When set to 1, it triggers initialization of Label Processor\'s data\n\r"
              "    structures. The CPU can only assert this bit. If the bit is\n\r"
              "    asserted the LBP starts to initialize all the data structures.\n\r"
              "    After the initialization process is done, the LBP negates the bit.\n\r"
              "    The ECI asserts this bit at chip reset.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "queue_controller_init_trig",
    {
      {
        FAP10M_QUEUE_CONTROLLER_INIT_TRIG_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    When set to 1, it triggers initialization of Queue Cntr data structures.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_logical_chop_field
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
     Fap10m_logical_chop_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "chunk_size",
    {
      {
        FAP10M_CHUNK_SIZE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Chunk size of chopped packets (payload data only).\n\r"
              "      0 - 512\n\r"
              "      1 - 640 \n\r"
              "      2 - 768 \n\r"
              "      3 - 896 \n\r"
              "      4 - 1024 \n\r"
              "      5 - 1152 \n\r"
              "      6 - 1280 \n\r"
              "      7 - 1536.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "last_chunk_size",
    {
      {
        FAP10M_LAST_CHUNK_SIZE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The size (in bytes) of the end chopped packets \n\r"
              "    (payload data only).\n\r"
              "    The EPC payload data size is: \n\r"
              "    chunk_size + 128 + last_chunk_size *64",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "last_device_num",
    {
      {
        FAP10M_DEV_NUM_CHOP_LIMIT_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    A unicast packet will be chopped only if: \n\r"
              "    Destination-Device <= PktDeviceNumChopLimit.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_label_processor_field
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
     Fap10m_label_processor_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "sig_class_bits",
    {
      {
        FAP10M_SIG_CLASS_BITS_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Significant Class Bits.\n\r"
              "    The number of class bits in unicast packets (up to 8 classes).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "sig_class_bits_flow",
    {
      {
        FAP10M_SIG_CLASS_BIT_FLOW_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Indicates the number of class bits to be added.\n\r"
              "    to flow-id calculations when determining a queue.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "flow_add",
    {
      {
        FAP10M_FLOW_ADD_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Indication if MappedClass should be added.\n\r"
              "    to the FlowID or concatinated.\n\r"
              "     1 - Add the MappedClass.\n\r"
              "     0 - Concatinate.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "flow_id_9",
    {
      {
        FAP10M_DMI_FLOW_ID9_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Forces the Flow-ID to be 9 bits only.\n\r"
              "    The high bits (12:9) will be forced to 0.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "tree_lvl_cnt_trh_bits",
    {
      {
        FAP10M_TREE_LVL_CNT_TRH_BITS_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Tree Level Count Threshold.\n\r"
              "    Multicast packets are discarded if their tree-level-count is\n\r"
              "    above the indicated threshold. If the threshold is 0,\n\r"
              "    no multicast packets should be discarded.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_logical_misc_field
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
     Fap10m_logical_misc_field[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "debug_ver",
    {
      {
        FAP10M_DBG_VER_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Debug Version. Indicated the version (bug correction) of the device.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "chip_ver",
    {
      {
        FAP10M_CHIP_VER_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Chip Version.\n\r"
              "    Indicates the version (different functionality) of the device.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "chip_type",
    {
      {
        FAP10M_CHIP_TYPE_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Chip Type. 0xFA010",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "dmi_trap_cpu_dev_0",
    {
      {
        FAP10M_DMI_TRAP_CPU_DEV_0_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    DMI trap cpu dev 0",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "dmi_trap_cpu_dev_1",
    {
      {
        FAP10M_DMI_TRAP_CPU_DEV_1_FIELD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    DMI trap cpu dev 1",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_indefinite_num_vals
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
     Fap10m_indefinite_num_vals[]
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
*  Fap10m_data_size_vals
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
     Fap10m_data_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        MAX_REPEATED_PARAM_VAL*sizeof(long),0,
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
*  Fap10m_data_values_vals
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
     Fap10m_data_values_vals[]
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
*  Fap10m_Interrupt_mockup_vals
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
     Fap10m_Interrupt_mockup_vals[]
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Devices interrupt line is not connected.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Devices interrupt line is connected.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_empty_vals
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
     Fap10m_empty_vals[]
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
*  Fap10m_device_access_vals
*TYPE: BUFFER
*DATE: 06/AUG/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP10M_RUN_MODE" IN THE
*  CONTEXT OF SUBJECT 'SOC_SAND_FAP10M GENERAL'.
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
     Fap10m_device_access_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "eci_access",
    {
      {
        FAP10M_INTERFACE_ECI,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All needed accesses are via the ECI (access to the I2C is done also).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "pci_access",
    {
      {
        FAP10M_INTERFACE_PCI,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All needed accesses are via the PCI/UpLink (access to the I2C is done also).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "i2c_access",
    {
      {
        FAP10M_INTERFACE_I2C,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All needed accesses are via the I2C.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_print_interval_vals
*TYPE: BUFFER
*DATE: 23/NOV/2003
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
     Fap10m_print_interval_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "five_seconds",
    {
      {
        5,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    5 seconds ",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "one_minute",
    {
      {
        60,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    1 minute ",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "one_hour",
    {
      {
        3600,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    1 hour ",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "never",
    {
      {
        0xFFFFFFFF,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    never ",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
*  Fap10m_fap_ids_vals
*TYPE: BUFFER
*DATE: 23/NOV/2003
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
     Fap10m_fap_ids_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        15,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        16,
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
*  Fap10m_bit_vals
*TYPE: BUFFER
*DATE: 23/NOV/2003
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
     Fap10m_bit_vals[]
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
*  Fap10m_offset_vals
*TYPE: BUFFER
*DATE: 23/NOV/2003
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
     Fap10m_offset_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_TOTAL_SIZE_OF_REGS,0,
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
*  Fap10m_params
*TYPE: BUFFER
*DATE: 29/Dec/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "SOC_SAND_FAP10M".
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
     Fap10m_params[]
#ifdef INIT
   =
{
  {
    PARAM_FAP10M_GENRAL_API_ID,
    "general",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver general services.",
    "Examples:\r\n"
    "  fap10m general start_driver\r\n"
    "Results in starting the fap10m driver.\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(26) | BIT(28) | BIT(29) | BIT(30),
     0,
     BIT(19),
     BIT(15)
    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_DEFEREED_API_ID,
    "deferred",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver deferred services.",
    "Examples:\r\n"
    "  fap10m deferred physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
    "",
    {BIT(3) | BIT(6) | BIT(9) | BIT(10) | BIT(13) | BIT(15) | BIT(18) | BIT(24)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_DEVICE_ACCESS_ID,
    "set_device_access",
    (PARAM_VAL_RULES *)&Fap10m_device_access_vals[0],
    (sizeof(Fap10m_device_access_vals) / sizeof(Fap10m_device_access_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the device access time in run time.\r\n",
    "Examples:\r\n"
    "  fap10m general set_device_access eci_access\r\n"
    "Results in setting the driver to access the device via the ECI.\r\n",
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
    PARAM_FAP10M_PRINT_DRIVER_INFORMATION_ID,
    "print_driver_information",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints driver specific information.\r\n",
    "Examples:\r\n"
    "  fap10m general print_driver_information \r\n"
    "Results in printing SOC_SAND_FAP10M driver information.\r\n",
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
    PARAM_FAP10M_START_DRIVER_ID,
    "driver_start",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts the Fap10m driver.\r\n",
    "Examples:\r\n"
    "  fap10m general start_driver\r\n"
    "Results in starting the fap10m driver.\r\n",
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
    PARAM_FAP10M_SHUTDOWN_DRIVER_ID,
    "driver_shutdown",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Shuts down Fap10m driver completly.\r\n",
    "Examples:\r\n"
    "  fap10m general shutdown_driver\r\n"
    "Results in shutting down the fap10m driver.\r\n",
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
    PARAM_FAP10M_REGISTER_DEVICE_ID,
    "register_device",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "registers an Fap10m device with the driver.\r\n",
    "Examples:\r\n"
    "  fap10m register_device 0x30043745 \r\n"
    "Results in registering an fap10m device at address 0x30043745 with the driver.\r\n",
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
    PARAM_FAP10M_UNREGISTER_DEVICE_ID,
    "unregister_device",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregisters an Fap10m device from the driver.\r\n",
    "Examples:\r\n"
    "  fap10m unregister_device 0x1 \r\n"
    "Results in unregistering an fap10m device that was registered under the handle 0x1.\r\n",
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
    PARAM_FAP10M_RESET_DEVICE_ID,
    "reset_device",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "resets a SOC_SAND_FAP10M device.\r\n",
    "Examples:\r\n"
    "  fap10m general reset_device 0x1\r\n"
    "Results in resetting device 0x1 (off-on).\r\n",
    "",
    {BIT(26) | BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FAP_ID_ID,
    "new_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "New FAP-ID to set in the init sequnce.\r\n",
    "Examples:\r\n"
    "  fe200 general reset_device 0x1 new_fap_id 3\r\n"
    "Results in shutting of device 0x1 and setting the FAP-ID to be 3.\r\n",
    "",
    {BIT(28)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_REMAIN_OFF_ID,
    "remain_off",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears the device will stay in reset.\r\n",
    "Examples:\r\n"
    "  fap10m general reset_device 0x1 remain_off\r\n"
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
    PARAM_FAP10M_START_AND_REGISTER_ID,
    "start_driver_and_register_devices",
    (PARAM_VAL_RULES *)&Fap10m_Interrupt_mockup_vals[0],
    (sizeof(Fap10m_Interrupt_mockup_vals) / sizeof(Fap10m_Interrupt_mockup_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_Interrupt_mockup_default,
    0,0,0,0,0,0,0,
    "starts the fap10m driver, simulator and register 2 devices (with or without ISR).\r\n",
    "Examples:\r\n"
    "  fap10m start_driver_and_register_devices mockup_inetrrupts\r\n"
    "Results in activating the driver, the simulator and registering 2 devices,\n\r"
    "  and simulating interrupts by polling.\r\n",
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
    PARAM_FAP10M_UNREGISTER_AND_SHUTDOWN_ID,
    "unregister_devices_and_shutdown_driver",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregister 2 fap10m devices, ends the simulator and shutdown the driver.\r\n",
    "Examples:\r\n"
    "  fap10m unregister_devices_and_shutdown_driver \r\n"
    "Results in unregistering the devices and shutting down the simulator and the driver.\r\n",
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
    PARAM_FAP10M_CLEAR_DEVICE_PENDING_SERVICES_ID,
    "clear_device_pending_services",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "clears all pending services of an Fap10m device.\r\n",
    "Examples:\r\n"
    "  fap10m clear_device_pending_services 0x1 \r\n"
    "Results in clearing all services registered for an fap10m device with the handle 0x1.\r\n",
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
    PARAM_FAP10M_STOP_POLLING_TRANSACTON_ID,
    "stop_polling_transaction",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregister one deferred service accroding to its handle.\r\n",
    "Examples:\r\n"
    "  fap10m stop_polling_transaction 0x28342359 \r\n"
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
    PARAM_FAP10M_LDU_WRITE_ID,
    "ldu_write",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "writes data to the specified device at the LDU offset.\r\n",
    "Examples:\r\n"
    "  fap10m general ldu_write 0x1 offset 0x8 data 0x2233\r\n"
    "Results in writing data of 32 bit at offset 0x8 (LDU) of device 0x1.\r\n",
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
    PARAM_FAP10M_LDU_READ_ID,
    "ldu_read",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "reads data from the specified device at the LDU offset.\r\n",
    "Examples:\r\n"
    "  fap10m general ldu_read 0x1 offset 0x8\r\n"
    "Results in reading data of 32 bit at offset 0x8 of device 0x1.\r\n",
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
    PARAM_FAP10M_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears - less printouts as possible.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_write 0x0 offset 0x4 data 0x2233 silent\r\n"
    "Results in writing data of 32 bit at offset 0x4 of device 0 - with no printing.\r\n",
    "",
    {BIT(1) | BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PHYSICAL_WRITE_ID,
    "physical_write",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "writes data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_write 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
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
    PARAM_FAP10M_PHYSICAL_READ_ID,
    "physical_read",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "reads data from the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_read 0x1 offset 0x8 size 0x4 \r\n"
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
    PARAM_FAP10M_DEFERRED_PHYSICAL_READ_ID,
    "physical_read",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred read of data from the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m deferred physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
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
    PARAM_FAP10M_LOGICAL_WRITE_ID,
    "logical_write",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "writes data to the specified device at the register / field.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 field chip_id data 0x3 \r\n"
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
    PARAM_FAP10M_LOGICAL_READ_ID,
    "logical_read",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "reads specified register / field from the device.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_read 0x1 field chip_id \r\n"
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
    PARAM_FAP10M_ENABLE_MSGS_ID,
    "enable_messages",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable / Disable control messages.\r\n",
    "Examples:\r\n"
    "  fap10m enable_messages 0 cntrl_msgs enable \r\n"
    "Results in Enabling control messages.\r\n",
    "",
    {0,0,BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CNTRL_MSGS_ID,
    "cntrl_msgs",
    (PARAM_VAL_RULES *)&Fap10m_enable_disable[0],
    (sizeof(Fap10m_enable_disable) / sizeof(Fap10m_enable_disable[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable / Disable control messages.\r\n",
    "Examples:\r\n"
    "  fap10m enable_messages 0 cntrl_msgs enable \r\n"
    "Results in Enabling control messages.\r\n",
    "",
    {0,0,BIT(19)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_DUMP_REGISTERS,
    "dump_register",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dump all the registers with values different from '0'.\r\n",
    "Examples:\r\n"
    "  fap10m dump_register 0x1 \r\n"
    "Results in printing all register info fap10m device with the handle 0x1.\r\n",
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
    PARAM_FAP10M_SKIP_ZEROS_ID,
    "print_zeros",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print also registers with values of '0' and '0xDEADBEEF'.\r\n",
    "Examples:\r\n"
    "  fap10m dump_register 0x1 \r\n"
    "Results in printing all register info fap10m device with the handle 0x1.\r\n",
    "",
    {BIT(0)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_INCLUDE_INDIRECT_TABLES_ID,
    "include_indirects",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print also indirect tables.\r\n",
    "Examples:\r\n"
    "  fap10m dump_register 0x1 \r\n"
    "Results in printing all register info fap10m device with the handle 0x1.\r\n",
    "",
    {BIT(0)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_DEFERRED_LOGICAL_READ_ID,
    "logical_read",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred read of the specified register / field from the device.\r\n",
    "Examples:\r\n"
    "  fap10m deferred logical_read 0x1 field chip_id polling_rate 50\r\n"
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
    PARAM_FAP10M_FIELD_PRINT_ALL_ID,
    "print_all_logical_ids",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "print all the values of the logical fields.\r\n",
    "Examples:\r\n"
    "  fap10m general print_all_logical_ids 0x1\r\n"
    "Results in printing the values of all the logical fields in device 0x1.\r\n",
    "",
    {0,0,0,BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PHYSICAL_DIRECT_ID,
    "direct",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_write direct 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
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
    PARAM_FAP10M_PHYSICAL_INDIRECT_ID,
    "indirect",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_write indirect 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
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
    PARAM_FAP10M_PHYSICAL_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_write direct 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3) | BIT(29) | BIT(30)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PHYSICAL_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&Fap10m_data_size_vals[0],
    (sizeof(Fap10m_data_size_vals) / sizeof(Fap10m_data_size_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_write 0x1 direct offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    { BIT(2) | BIT(3)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_NUM_OF_TIMES_ID,
    "nof_times",
    (PARAM_VAL_RULES *)&Fap10m_nof_times_mem_read_vals[0],
    (sizeof(Fap10m_nof_times_mem_read_vals) / sizeof(Fap10m_nof_times_mem_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_nof_times_mem_read_default,
    0,0,0,0,0,0,0,
    "Re-current read. 'nof_times' times.",
    "Examples:\r\n"
    "  fap10m general physical_read 0x1 direct offset 0x8 size 0x4 nof_times 12\r\n"
    "Results in reading, 12 times, starting at offset 0x8.\r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_PAUSE_AFTER_READ_ID,
    "pause_after_read",
    (PARAM_VAL_RULES *)&Fap10m_pause_after_read_mem_read_vals[0],
    (sizeof(Fap10m_pause_after_read_mem_read_vals) / sizeof(Fap10m_pause_after_read_mem_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_pause_after_read_mem_read_default,
    0,0,0,0,0,0,0,
    "Pause for amount of time after read (micro-sec). (Useful with 'nof_times' parameter.)",
    "Examples:\r\n"
    "  fap10m general physical_read 0x1 direct offset 0x8 size 0x4 nof_times 12 pause_after_read 10\r\n"
    "Results in reading, 12 times, starting at offset 0x8.\r\n"
    "After every read the CPU will halt for 10 micro-sec \n\r",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PHYSICAL_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Fap10m_data_values_vals[0],
    (sizeof(Fap10m_data_values_vals) / sizeof(Fap10m_data_values_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m general physical_write 0x1 direct offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(4) | BIT(29)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_POLLING_RATE_ID,
    "polling_rate",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of a deferred read of data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap10m deferred physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
    "",
    {BIT(3) | BIT(6) | BIT(9) | BIT(13) | BIT(15) | BIT(18) | BIT(24)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "interrupt",
    (PARAM_VAL_RULES *)&Fap10m_logical_interrupt_mask_field[0],
    (sizeof(Fap10m_logical_interrupt_mask_field) / sizeof(Fap10m_logical_interrupt_mask_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interrupt group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 interrupt src_dv_cng_link_mask 0x1\r\n"
    "Results in writing the value 0x1 to the src_dv_cng_link_mask field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "init_trig",
    (PARAM_VAL_RULES *)&Fap10m_logical_initialization_triggers_field[0],
    (sizeof(Fap10m_logical_initialization_triggers_field) / sizeof(Fap10m_logical_initialization_triggers_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Initialization triggers group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 init_trig queue_controller_init_trig 0x1\r\n"
    "Results in writing the value 0x1 to the queue_controller_init_trig field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "sms_rev_a",
    (PARAM_VAL_RULES *)&Fap10m_a_logical_sms_field[0],
    (sizeof(Fap10m_a_logical_sms_field) / sizeof(Fap10m_a_logical_sms_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Spatial Multicast Scheduler group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 sms unicast_weight 0x3\r\n"
    "Results in writing the value 0x3 to the unicast_weight field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "sms_rev_b",
    (PARAM_VAL_RULES *)&Fap10m_b_logical_sms_field[0],
    (sizeof(Fap10m_b_logical_sms_field) / sizeof(Fap10m_b_logical_sms_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Spatial Multicast Scheduler group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 sms unicast_weight 0x3\r\n"
    "Results in writing the value 0x3 to the unicast_weight field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "label_processor",
    (PARAM_VAL_RULES *)&Fap10m_label_processor_field[0],
    (sizeof(Fap10m_label_processor_field) / sizeof(Fap10m_label_processor_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "label-processor group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 label_processor sig_class_bits 0x3\r\n"
    "Results in writing the value 0x3 to the 'sig_class_bits' field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "misc",
    (PARAM_VAL_RULES *)&Fap10m_logical_misc_field[0],
    (sizeof(Fap10m_logical_misc_field) / sizeof(Fap10m_logical_misc_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "miscellaneous group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 misc chip_id 0x3\r\n"
    "Results in writing the value 0x3 to the chip_id field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "queue_controller",
    (PARAM_VAL_RULES *)&Fap10m_logical_external_dram_field[0],
    (sizeof(Fap10m_logical_external_dram_field) / sizeof(Fap10m_logical_external_dram_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "queueing group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 queue_controller top_que_blk_1 0x333\r\n"
    "Results in writing the value 0x333 to the top_que_blk_1 field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "sch",
    (PARAM_VAL_RULES *)&Fap10m_logical_sch_field[0],
    (sizeof(Fap10m_logical_sch_field) / sizeof(Fap10m_logical_sch_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "scheduler group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 sch port_cfg 0x3\r\n"
    "Results in writing the value 0x3 to the port_cfg field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "mac",
    (PARAM_VAL_RULES *)&Fap10m_logical_mac_field[0],
    (sizeof(Fap10m_logical_mac_field) / sizeof(Fap10m_logical_mac_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MAC group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 mac commas_number 0x3\r\n"
    "Results in writing the value 0x3 to the commas_number field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FIELD_ID,
    "chopping",
    (PARAM_VAL_RULES *)&Fap10m_logical_chop_field[0],
    (sizeof(Fap10m_logical_chop_field) / sizeof(Fap10m_logical_chop_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Big packets chopping group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap10m general logical_write 0x1 chopping chunk_size 3\r\n"
    "Results in writing the value 0x3 to the chunk_size field of device 0x1.\r\n"
    "(The chunks size will be 896) \n\r",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* miscellaneous interface { */
  {
    PARAM_FAP10M_MISC_API_ID,
    "misc",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver miscellaneous services.",
    "Examples:\r\n"
    "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
    "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {0,
     0,
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
    PARAM_FAP10M_GET_LAST_PKT_HEADER_ID,
    "get_last_pkt_header",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get the last packet header that was presented to the LBP.\r\n",
    "Examples:\r\n"
    "  fap10m misc get_last_pkt_header 0\r\n"
    "Results in getting last LBP header from device 0.\r\n",
    "",
    {0,
     0,
     0,
     BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END miscellaneous interface } */

  /* fap10m_api_cell.h interface { */
  {
    PARAM_FAP10M_CELL_API_ID,
    "traffic",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver traffic (cell/packet) services.",
    "Examples:\r\n"
    "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
    "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(7) | BIT(8),
     BIT(22) | BIT(23)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SEND_CELL_ID,
    "send_sr_data_cell",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sends a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
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
    PARAM_FAP10M_SEND_CELL_INIT_OUT_LINK_ID,
    "initial_output_link",
    (PARAM_VAL_RULES *)&FAP10M_initial_output_link_num_vals[0],
    (sizeof(FAP10M_initial_output_link_num_vals) / sizeof(FAP10M_initial_output_link_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the link to choose when leave this fap.\r\n",
    "Examples:\r\n"
      "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 2 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 2 to a fop.\r\n",
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
    PARAM_FAP10M_SEND_CELL_FE1_LINK_ID,
    "fe1_link",
    (PARAM_VAL_RULES *)&FAP10M_fe1_link_num_vals[0],
    (sizeof(FAP10M_fe1_link_num_vals) / sizeof(FAP10M_fe1_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap10m_fe1_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe1.\r\n",
    "Examples:\r\n"
      "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 4 fe1_link 12 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
      "Results in sending a source routed data cell through device 0x1 link 4 to a fop.\r\n",
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
    PARAM_FAP10M_SEND_CELL_FE2_LINK_ID,
    "fe2_link",
    (PARAM_VAL_RULES *)&FAP10M_fe2_link_num_vals[0],
    (sizeof(FAP10M_fe2_link_num_vals) / sizeof(FAP10M_fe2_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap10m_fe2_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe2.\r\n",
    "Examples:\r\n"
      "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
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
    PARAM_FAP10M_SEND_CELL_FE3_LINK_ID,
    "fe3_link",
    (PARAM_VAL_RULES *)&Fap10m_fe3_link_num_vals[0],
    (sizeof(Fap10m_fe3_link_num_vals) / sizeof(Fap10m_fe3_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap10m_fe3_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe3.\r\n",
    "Examples:\r\n"
      "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
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
    PARAM_FAP10M_SEND_CELL_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&Fap10m_entity_type[0],
    (sizeof(Fap10m_entity_type) / sizeof(Fap10m_entity_type[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the entity this cell is destined to.\r\n",
    "Examples:\r\n"
      "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
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
    PARAM_FAP10M_SEND_CELL_SRC_CHIP_ID,
    "source_chip_id",
    (PARAM_VAL_RULES *)&Fap10m_source_chip_id_num_val[0],
    (sizeof(Fap10m_source_chip_id_num_val) / sizeof(Fap10m_source_chip_id_num_val[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap10m_source_chip_id_default,
    0,0,0,0,0,0,0,
    "the entity this cell is originated in.\r\n",
    "Examples:\r\n"
      "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
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
    PARAM_FAP10M_SEND_CELL_BODY_ID,
    "body",
    (PARAM_VAL_RULES *)&Fap10m_cell_body_num_vals[0],
    (sizeof(Fap10m_cell_body_num_vals) / sizeof(Fap10m_cell_body_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the body of the cell.\r\n",
    "Examples:\r\n"
      "  fap10m traffic send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
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
    PARAM_FAP10M_RECV_CELL_ID,
    "recv_sr_data_cell",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "recieves a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fap10m traffic recv_sr_data_cell 0x1 \r\n"
    "Results in reading a source routed data cell from device 0x1 recieve cell buffer.\r\n",
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
    PARAM_FAP10M_DEFRRED_RECV_CELL_ID,
    "recv_sr_data_cell",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred receive (interrupt or polling) of a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fap10m deferred recv_sr_data_cell 0x1 interrupt_mode \r\n"
    "Results in deferred reading a source routed data cell from device 0x1 \n\r"
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
    PARAM_DEFERRED_FAP10M_RECV_CELL_POLL_MODE_ID,
    "polling_mode",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deferred call is triggered by simple polling of the device.\r\n",
    "Examples:\r\n"
    "  fap10m deferred recv_sr_data_cell 0x1 polling_mode polling_rate 50 \r\n"
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
    PARAM_DEFERRED_FAP10M_RECV_CELL_INT_MODE_ID,
    "interrupt_mode",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deferred call is triggered by an interrupt of the device.\r\n",
    "Examples:\r\n"
    "  fap10m deferred recv_sr_data_cell 0x1 links range_0-31 interrupt_mode\r\n"
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
  /* END fap10m_api_cell.h interface } */
  /* fap10m_api_links.h interface { */
  {
    PARAM_FAP10M_LINKS_API_ID,
    "links",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver links services.",
    "Examples:\r\n"
    "  fap10m cell_services send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
    "Results in sending a source routed data cell through device 0x1 link 40 to a fop.\r\n",
    "",
    {BIT(11) | BIT(12) | BIT(14) | BIT(27),
     0,
     0,
     BIT(21) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_LINK_FC_GET_ID,
    "get_link_fc",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the status report of the fabric link flow control.\r\n",
    "Examples:\r\n"
    "  fap10m links get_link_fc 0 \r\n"
    "Results in printing the ststus fabric link flow control setting.\r\n",
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
    PARAM_FAP10M_LINK_FC_SET_ID,
    "set_link_fc",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable/Disable the fabric link flow control.\r\n",
    "Examples:\r\n"
    "  fap10m links set_link_fc 0 tx_fc enable rx_fc disable.\r\n"
    "Results in setting all the link to have link level flow control\r\n"
    "in the transmit direction, and disable the fc in the receiving\r\n"
    "direction. Make sure that in the other side of the links, the\r\n"
    "receiving is enabled and the transmitting disabled.\r\n",
    "",
    {0,0,0,BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_LINK_FC_TX_ID,
    "tx_fc",
    (PARAM_VAL_RULES *)&Fap10m_tx_fc[0],
    (sizeof(Fap10m_tx_fc) / sizeof(Fap10m_tx_fc[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable/Disable the transmitting side of fabric link flow control.\r\n",
    "Examples:\r\n"
    "  fap10m links set_link_fc 0 tx_fc enable rx_fc disable.\r\n"
    "Results in setting all the link to have link level flow control\r\n"
    "in the transmit direction, and disable the fc in the receiving\r\n"
    "direction. Make sure that in the other side of the links, the\r\n"
    "receiving is enabled and the transmitting disabled.\r\n",
    "",
    {0,0,0,BIT(21)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_LINK_FC_RX_ID,
    "rx_fc",
    (PARAM_VAL_RULES *)&Fap10m_enable_disable[0],
    (sizeof(Fap10m_enable_disable) / sizeof(Fap10m_enable_disable[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable/Disable the receiving side of fabric link flow control.\r\n",
    "Examples:\r\n"
    "  fap10m links set_link_fc 0 rx_fc enable rx_fc disable.\r\n"
    "Results in setting all the link to have link level flow control\r\n"
    "in the transmit direction, and disable the fc in the receiving\r\n"
    "direction. Make sure that in the other side of the links, the\r\n"
    "receiving is enabled and the transmitting disabled.\r\n",
    "",
    {0,0,0,BIT(21)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_SERDES_PARAMS_ID,
    "get_serdes_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the status report of all the serdeses.\r\n",
    "Examples:\r\n"
    "  fap10m links get_serdes_params 0x1 \r\n"
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
    PARAM_FAP10M_SET_SERDES_PD_PARAMS_ID,
    "set_serdes_power_down",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the serdes power down/up.\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_power_down 0x1 serdes_number 12\r\n"
    "Results in setting link#12 to power down.\r\n",
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
    PARAM_FAP10M_SET_SERDES_PARAMS_ID,
    "set_serdes_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
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
    PARAM_FAP10M_SET_SERDES_PARAMS_SERDES_NUMBER_ID,
    "serdes_number",
    (PARAM_VAL_RULES *)&Fap10m_serdes_number_num_vals[0],
    (sizeof(Fap10m_serdes_number_num_vals) / sizeof(Fap10m_serdes_number_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12) | BIT(27)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SERDES_POWER_UP_ID,
    "power_up",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Power-up indication.\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_power_down 0x1 serdes_number 12 power_up\r\n"
    "Results in setting link#12 to power up.\r\n",
    "",
    {BIT(27)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_SERDES_PARAMS_AMPLITUDE_ID,
    "amplitude",
    (PARAM_VAL_RULES *)&Fap10m_serdes_amplitude_num_vals[0],
    (sizeof(Fap10m_serdes_amplitude_num_vals) / sizeof(Fap10m_serdes_amplitude_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap10m_serdes_amplitude_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
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
    PARAM_FAP10M_SET_SERDES_PARAMS_COMMON_MODE_ID,
    "common_mode",
    (PARAM_VAL_RULES *)&Fap10m_serdes_common_mode_num_vals[0],
    (sizeof(Fap10m_serdes_common_mode_num_vals) / sizeof(Fap10m_serdes_common_mode_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap10m_serdes_common_mode_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
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
    PARAM_FAP10M_SET_SERDES_PARAMS_MAIN_CONTROL_ID,
    "main_control",
    (PARAM_VAL_RULES *)&Fap10m_serdes_main_control_num_vals[0],
    (sizeof(Fap10m_serdes_main_control_num_vals) / sizeof(Fap10m_serdes_main_control_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap10m_serdes_main_control_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
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
    PARAM_FAP10M_SET_SERDES_PARAMS_POWER_DOWN_RX_TX_ID,
    "rx_tx_power_down",
    (PARAM_VAL_RULES *)&Fap10m_serdes_power_down_num_vals[0],
    (sizeof(Fap10m_serdes_power_down_num_vals) / sizeof(Fap10m_serdes_power_down_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap10m_serdes_power_down_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
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
    PARAM_FAP10M_SET_SERDES_PARAMS_PRE_EMPHASIS_ID,
    "pre_emphasis",
    (PARAM_VAL_RULES *)&Fap10m_serdes_pre_emphasis_num_vals[0],
    (sizeof(Fap10m_serdes_pre_emphasis_num_vals) / sizeof(Fap10m_serdes_pre_emphasis_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap10m_serdes_pre_emphasis_default,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap10m links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
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
    PARAM_FAP10M_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "present connectivity map the the chip sees\r\n",
    "Examples:\r\n"
    "  fap10m links get_connectivity_map 0x1 \r\n"
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
    PARAM_FAP10M_DEFERRED_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the status report of all the serdeses.\r\n",
    "Examples:\r\n"
    "  fap10m deferred get_connectivity_map 0x1 \r\n"
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
    PARAM_FAP10M_GET_LINKS_STATUS_ID,
    "get_links_status",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fap10m links get_links_status 0x1 list_of_links 0x0 0x1\r\n"
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
    PARAM_FAP10M_LIST_OF_LINKS_ID,
    "list_of_links",
    (PARAM_VAL_RULES *)&Fap10m_list_of_links_num_vals[0],
    (sizeof(Fap10m_list_of_links_num_vals) / sizeof(Fap10m_list_of_links_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fap10m links get_links_status 0x1 list_of_links 0x0 0x1\r\n"
    "Results in printing the ststus of link #1.\r\n",
    "",
    {BIT(14) | BIT(15) | BIT(19)},
    8,/* just so it would be after polling rate (==6) */
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_DEFERRED_GET_LINKS_STATUS_ID,
    "get_links_status",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fap10m deferred get_links_status 0x1 list_of_links 0x0 0x1 interrupt_mode \r\n"
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
  /* END fap10m_api_links.h interface } */
  /* fap10m_api_routing.h interface { */
  {
    PARAM_FAP10M_ROUTING_API_ID,
    "routing",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver routing services.",
    "Examples:\r\n"
    "  fap10m routing get_unicast_table 0x1 starting_row 125 nof_rows 6\r\n"
    "Results in printing the distribution unicast table of\n\r"
    "  device #1 rows (target FAPs) 125-130.\r\n",
    "",
    {BIT(16) | BIT(17) | BIT (19) | BIT (20)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_UNICAST_TABLE_ID,
    "get_unicast_table",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets (max of 100 entires of) the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap10m routing get_unicast_table 0 starting_row 0 nof_rows 2\r\n"
    "Results in printing the distribution unicast table of device #0 rows (target FAPs) 0-1.\r\n",
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
    PARAM_FAP10M_DEFERRED_GET_UNICAST_TABLE_ID,
    "get_unicast_table",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "peridically gets (max of 100 entires of) the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap10m deferred get_unicast_table 0x1 starting_row 125 ... \r\n"
    "Results in peridically printing the distribution table of device #1 starting at row 125.\r\n",
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
    PARAM_FAP10M_STARTING_ROW_ID,
    "starting_row",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap10m deferred get_unicast_table 0x1 ... \r\n"
    "Results in peridically printing the distribution table of device #1.\r\n",
    "",
    {BIT(17) | BIT(18) | BIT(19)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NOF_ROWS_ID,
    "nof_rows",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap10m deferred get_unicast_table 0x1 ... \r\n"
    "Results in peridically printing the distribution table of device #1.\r\n",
    "",
    {BIT(17) | BIT(18) | BIT(19)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap10m_api_routing.h interface } */
  /* fap10m_api_diagnostics.h interface { */
  {
    PARAM_FAP10M_DIAGNOSTICS_API_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver diagnostics services.",
    "Examples:\r\n"
    "  fap10m diagnostics get_loopback_type 0x1\r\n"
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
    PARAM_FAP10M_GET_LOOPBACK_ID,
    "get_loopback",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics get_loopback_type 0x1\r\n"
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
    PARAM_FAP10M_SET_LOOPBACK_ID,
    "set_loopback",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics set_loopback_type 0x1 local_loop on\r\n"
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
    PARAM_FAP10M_LOOPBACK_TYPE_ID,
    "local_loop",
    (PARAM_VAL_RULES *)&Fap10m_local_loopback_on_vals[0],
    (sizeof(Fap10m_local_loopback_on_vals) / sizeof(Fap10m_local_loopback_on_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics set_loopback_type 0x1 local_loop on\r\n"
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
    PARAM_FAP10M_GET_PRBS_MODE_ID,
    "get_prbs_mode",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the prbs mode the device is in.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics get_prbs_mode 0x1\r\n"
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
    PARAM_FAP10M_SET_PRBS_MODE_ID,
    "set_prbs_mode",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the prbs mode of the device\'s MAC module.\n\r",
    "Examples:\r\n"
    "  fap10m diagnostics set_prbs mode 0x1 checker_link 2 generator_link 8 activate_generator on\r\n"
    "Results in setting the prbs params of the MAC.\r\n",
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
    PARAM_FAP10M_PRBS_CHECKER_LINK_ID,
    "checker_link",
    (PARAM_VAL_RULES *)&Fap10m_serdes_number_num_vals[0],
    (sizeof(Fap10m_serdes_number_num_vals) / sizeof(Fap10m_serdes_number_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_serdes_number_default,
    0,0,0,0,0,0,0,
    "sets the link that the device\'s MAC module prbs checkers is sitting on (rx).\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics 0 set_prbs checker_link 2 generator_link 8 activate_generator on activate_checker on\r\n"
    "Results in setting the prbs params of MAC.\r\n",
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
    PARAM_FAP10M_PRBS_GENERATOR_LINK_ID,
    "generator_links",
    (PARAM_VAL_RULES *)&Fap10m_prbs_generator_links[0],
    (sizeof(Fap10m_prbs_generator_links) / sizeof(Fap10m_prbs_generator_links[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_serdes_number_default,
    0,0,0,0,0,0,0,
    "sets the links that the device\'s MAC module generate prbs on (tx).\r\n"
    "When working with Fap10m_A0, only one generator might be used.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics set_prbs checker_link 2 generator_links 0 3 4 activate_generator on activate_checker on\r\n"
    "Results in setting the prbs params of MAC.\r\n"
    "The MAC will generate prbs over links 0,3 and 4 and would count BER errors\r\n"
    "on link '0', as this is the default for 'checker_link'",
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
    PARAM_FAP10M_PRBS_ACTIVATE_GENERATOR_ID,
    "activate_generator",
    (PARAM_VAL_RULES *)&Fap10m_activate_prbs_generator_vals[0],
    (sizeof(Fap10m_activate_prbs_generator_vals) / sizeof(Fap10m_activate_prbs_generator_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates / deactivates device\'s MAC module prbs generator (tx).\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics set_prbs checker_link 1 generator_link 2 activate_generator on activate_checker on\r\n"
    "Results in setting the prbs params of MAC.\r\n",
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
    PARAM_FAP10M_PRBS_ACTIVATE_CHECKER_ID,
    "activate_checker",
    (PARAM_VAL_RULES *)&Fap10m_activate_prbs_checker_vals[0],
    (sizeof(Fap10m_activate_prbs_checker_vals) / sizeof(Fap10m_activate_prbs_checker_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates / deactivates device\'s MAC module prbs checker (rx).\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics set_prbs checker_link 1 generator_link 2 activate_generator on activate_checker on\r\n"
    "Results in setting the prbs params of MAC.\r\n",
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
    PARAM_FAP10M_START_PRBS_TEST_ID,
    "start_prbs_test",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "starts background collection of ber counters.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics start_prbs_test 0x1\r\n"
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
    PARAM_FAP10M_STOP_PRBS_TEST_ID,
    "stop_prbs_test",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "stops background collection of ber counters.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics stop_prbs_test 0x1\r\n"
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
    PARAM_FAP10M_GET_BER_COUNTERS_ID,
    "get_ber_counters",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of prbs units.\r\n",
    "Examples:\r\n"
    "  fap10m diagnostics get_ber_counters 0x1\r\n"
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
    PARAM_FAP10M_DEFERRED_GET_BER_COUNTERS_ID,
    "get_ber_counters",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of prbs units.\r\n",
    "Examples:\r\n"
    "  fap10m deferred get_ber_counters 0x1 polling_rate 60 \r\n"
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
  /* END fap10m_api_diagnostics.h interface } */
  /* fap10m_api_statistics.h interface { */
  {
    PARAM_FAP10M_STATISTICS_API_ID,
    "statistics",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver statistics services.",
    "Examples:\r\n"
    "  fap10m statistics_services get_mac_counter_type 0x1\r\n"
    "Results in printing mac rx & tx counter type device #1 is in.\r\n",
    "",
    {BIT(25)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_ALL_COUNTERS_ID,
    "get_all_counters",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets set of counters that is defined in the statistics section.\n\r"
    "  Refer to other command to see specific counter meanning.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_all_counters 0x0\r\n"
    "Results in printing all statistics counters of device 0.\r\n",
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
    PARAM_FAP10M_GET_OPERATIVE_COUNTERS_ID,
    "get_operative_counters",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets set of counters that is defined in the statistics section.\n\r"
    "  Refer to other command to see specific counter meanning.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_operative_counters 0x0\r\n"
    "Results in printing all operative statistics counters of device 0.\r\n",
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
    PARAM_FAP10M_GET_PKT_ADDED_TO_QUEUES_CNT_ID,
    "get_pkt_added_to_queues_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FIP about the number of packets\n\r"
    "  were added to the device queues.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_pkt_added_to_queues_cnt 0x1\r\n"
    "Results in printing the 'Packet Added to Queues Counter' of device 1.\r\n",
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
    PARAM_FAP10M_GET_TAIL_DISCARD_PKT_CNT_ID,
    "get_tail_discard_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FIP about the number of packets\n\r"
    "  were tail-discarded from memory due to the 'drop from front' scheme.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_tail_discard_pkt_cnt 0x1\r\n"
    "Results in printing the 'Tail-Discarded Packet Counter' of device 1.\r\n",
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
    PARAM_FAP10M_GET_HEAD_DELETE_PKT_CNT_ID,
    "get_head_delete_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FIP about the number of packets\n\r"
    "  were head-deleted from memory due to WRED or queue above maximum threshold.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_head_delete_pkt_cnt 0x1\r\n"
    "Results in printing the 'Head-Deleted Packet Counter' of device 1.\r\n",
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
    PARAM_FAP10M_GET_GOOD_PKT_CNT_ID,
    "get_good_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FOP about the number of packets\n\r"
    "  were reassembled with no errors.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_good_pkt_cnt 0x1\r\n"
    "Results in printing the 'Good Packet Counter' of device 1.\r\n",
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
    PARAM_FAP10M_GET_ERRONEOUS_PKT_CNT_ID,
    "get_erroneous_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FOP about the number of packets\n\r"
    "  had errors during reassembly.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_erroneous_pkt_cnt 0x1\r\n"
    "Results in printing the 'Erroneous Packet Counter' of device 1.\r\n",
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
    PARAM_FAP10M_DEFERRED_GET_PKT_ADDED_TO_QUEUES_CNT_ID,
    "get_pkt_added_to_queues_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FIP about the number of packets\n\r"
    "  were added to the device queues.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_pkt_added_to_queues_cnt 0x1\r\n"
    "Results in printing the 'Packet Added to Queues Counter' of device 1.\r\n",
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
    PARAM_FAP10M_DEFERRED_GET_TAIL_DISCARD_PKT_CNT_ID,
    "get_tail_discard_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FIP about the number of packets\n\r"
    "  were tail-discarded from memory due to the 'drop from front' scheme.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_tail_discard_pkt_cnt 0x1\r\n"
    "Results in printing the 'Tail-Discarded Packet Counter' of device 1.\r\n",
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
    PARAM_FAP10M_DEFERRED_GET_HEAD_DELETE_PKT_CNT_ID,
    "get_head_delete_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FIP about the number of packets\n\r"
    "  were head-deleted from memory due to WRED or queue above maximum threshold.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_head_delete_pkt_cnt 0x1\r\n"
    "Results in printing the 'Head-Deleted Packet Counter' of device 1.\r\n",
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
    PARAM_FAP10M_DEFERRED_GET_GOOD_PKT_CNT_ID,
    "get_good_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FOP about the number of packets\n\r"
    "  were reassembled with no errors.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_good_pkt_cnt 0x1\r\n"
    "Results in printing the 'Good Packet Counter' of device 1.\r\n",
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
    PARAM_FAP10M_DEFERRED_GET_ERRONEOUS_PKT_CNT_ID,
    "get_erroneous_pkt_cnt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FOP about the number of packets\n\r"
    "  had errors during reassembly.\r\n",
    "Examples:\r\n"
    "  fap10m statistics get_erroneous_pkt_cnt 0x1\r\n"
    "Results in printing the 'Erroneous Packet Counter' of device 1.\r\n",
    "",
    {BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* } END fap10m_api_statistics.h interface */
  /*
   * Test API {
   */
  {
    PARAM_FAP10M_TEST_API_ID,
    "tests",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver tests services.\n\r"
    "  The system need to be configured in a specific configuration.\n\r"
    "  *NOTE*: All tests may change your FAP configuration - without\n\r"
    "          restoring it to your original values.",
    "Examples:\r\n"
    "  fap10m tests cells\r\n"
    "Results in testing fap10m driver cell module.\r\n"
    "In all the tests the system need to be configured as follows:\n\r"
    "  1. The fabric is considered working (FE/loopbacks)\n\r"
    "  2. The PP is considered UP and initialized with PP-Id is 0\n\r"
    "  3. The is considered after its reset sequence (all HW is functional)\n\r"
    "     FAP-Id is 0.\n\r"
    "  4. No traffic should be injected from out side ports.\n\r"
    "     Unless otherwise is stated.\n\r"
    "  5. One ethernet cable should connect from port 4 to 5.\n\r"
    "     Port 1 and 2 should not be connected.\n\r"
    "  6. PP MAC-table: All port on VLAN 1.\n\r"
    "         MAC address 000000AAAA05 - port 5\n\r"
    "         MAC address 000000AAAA04 - port 4\n\r",
    "",
    {BIT(31),
     BIT(30) | BIT(31), /**/
     BIT(16) | BIT(17) | BIT(18), /*16: ,17, 18:app*/
     BIT(3) | BIT(4) | BIT(5) | BIT(23)

    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_APP_ID,
    "app",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m test application.\r\n",
    "Examples:\r\n"
    "  fap10m tests app snake_1024\r\n"
    "Results in activating 'snake_1024' application.\r\n",
    "",
    {0,
     0,
     BIT(17) | BIT(18)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_DELTA_VALUE_STOP_ID,
    "read_addr_stop",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Stops the running test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m tests read_addr_stop\r\n"
    "Stops the currently running test.\n\r",
    "",
    {0,
     0,
     0,
     BIT(3)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_DELTA_VALUE_START_ID,
    "read_addr_start",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Fap10m_offset_vals[0],
    (sizeof(Fap10m_offset_vals) / sizeof(Fap10m_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_LS_BIT_ID,
    "ls_bit",
    (PARAM_VAL_RULES *)&Fap10m_bit_vals[0],
    (sizeof(Fap10m_bit_vals) / sizeof(Fap10m_bit_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_MS_BIT_ID,
    "ms_bit",
    (PARAM_VAL_RULES *)&Fap10m_bit_vals[0],
    (sizeof(Fap10m_bit_vals) / sizeof(Fap10m_bit_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_FAP_IDS_ID,
    "fap_ids",
    (PARAM_VAL_RULES *)&Fap10m_fap_ids_vals[0],
    (sizeof(Fap10m_fap_ids_vals) / sizeof(Fap10m_fap_ids_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_DELTA_TEST_ID,
    "delta_test",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that the delta of 2 consequtive reads of certain field\r\n"
    " does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4)
    },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_NORMALIZATION_ID,
    "requires_time_normalization",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that the delta of 2 consequtive reads of certain field normalized\r\n"
    " to the (later passed) exact time interval does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_VALUE_TEST_ID,
    "value_test",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   value_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the value read from the device\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen.\r\n",
    "",
    {0,
     0,
     0,
     BIT(5)
    },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_HIGH_BOUND_ID,
    "higher_bound",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_LOW_BOUND_ID,
    "lower_bound",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_SYS_TICKS_ID,
    "polling_rate_in_system_ticks",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "A system tick is approx 16 ms (60 ticks in a second).\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_MEM_READ_PRINT_INTERVAL_ID,
    "routine_printout_every",
    (PARAM_VAL_RULES *)&Fap10m_print_interval_vals[0],
    (sizeof(Fap10m_print_interval_vals) / sizeof(Fap10m_print_interval_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap10m read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_APP_SET_QUEUE_LABEL_GROUP_ID,
    "set_queue_label_group",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Rapid LBP unicast table setting app.\r\n",
    "Examples:\r\n"
    "  fap10m tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NOF_FAP_IDS_ID,
    "nof_fap_ids",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of target faps to use\r\n",
    "Examples:\r\n"
    "  fap10m tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_START_QUEUE_ID_ID,
    "start_queue_id",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Start queue id to set\r\n",
    "Examples:\r\n"
    "  fap10m tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FAP_ID_ID,
    "start_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Start fap id to set\r\n",
    "Examples:\r\n"
    "  fap10m tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_BLOCK_SIZE_ID,
    "queue_block_size",
    (PARAM_VAL_RULES *)&Fap10m_nof_queues[0],
    (sizeof(Fap10m_nof_queues) / sizeof(Fap10m_nof_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "How many queues to add in every table entry.\r\n",
    "Examples:\r\n"
    "  fap10m tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_APP_OPEN_FLOW_QUEUE_GROUP_ID,
    "open_queue_flow_group",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Rapid flow/queue app.\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",

    "",
    {0,
     0,
     BIT(17)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FAP_ID_ID,
    "target_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target fap to use\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_ID_ID,
    "start_queue_id",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first queue id to use.\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FLOW_ID_ID,
    "start_flow_id",
    (PARAM_VAL_RULES *)&Fap10m_flow_id[0],
    (sizeof(Fap10m_flow_id) / sizeof(Fap10m_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first flow id to use.\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NOF_QUEUES_ID,
    "nof_queue_flow",
    (PARAM_VAL_RULES *)&Fap10m_nof_queues[0],
    (sizeof(Fap10m_nof_queues) / sizeof(Fap10m_nof_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Total number of queues/flows to open.\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NOF_FLOWS_PER_PORT_ID,
    "nof_flows_per_port",
    (PARAM_VAL_RULES *)&Fap10m_nof_queues[0],
    (sizeof(Fap10m_nof_queues) / sizeof(Fap10m_nof_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of flows before the port number get incremented.\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QOS_SCHEME_ID,
    "qos_scheme",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Quality of service scheme.\r\n"
    "0 - edf0 for all flows, free param the Kilo Bit Sec.\n\r"
    "1 - edf1 for all flows, free param the Kilo Bit Sec\n\r"
    "2 - wfq for all the wheels, free param the weight .\n\r"
    "3 - edf0 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "4 - edf1 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "5 - edf0 for one flow, edf1 for the second flow, free param the Kilo Bit Sec.\n\r"
    "6 - 6 - edf1 for one flow free param the Kilo Bit Sec, wfq for the other flow weight - 30.\n\r"
    "7 - rcy flow for each wheel free param the weight\n\r"
    "8 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30.\n\r"
    "    FSF 8 ==> 3 sub flows on one flows\n\r"
    "9 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30, edf0 for odd flows minimum BW.\n\r"
    "    FSF 4 ==> 4 sub flows on one flows\n\r",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QOS_FREE_PARAM_ID,
    "qos_free_param",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Quality of service parameter, changes with respect to the QOS scheme.\r\n"
    "0 - edf0 for all flows, free param the Kilo Bit Sec.\n\r"
    "1 - edf1 for all flows, free param the Kilo Bit Sec\n\r"
    "2 - wfq for all the wheels, free param the weight .\n\r"
    "3 - edf0 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "4 - edf1 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "5 - edf0 for one flow, edf1 for the second flow, free param the Kilo Bit Sec.\n\r"
    "6 - 6 - edf1 for one flow free param the Kilo Bit Sec, wfq for the other flow weight - 30.\n\r"
    "7 - rcy flow for each wheel free param the weight\n\r"
    "8 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30.\n\r"
    "    FSF 8 ==> 3 sub flows on one flows\n\r"
    "9 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30, edf0 for odd flows minimum BW.\n\r"
    "    FSF 4 ==> 4 sub flows on one flows\n\r",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_START_PORT_ID_ID,
    "start_port_id",
    (PARAM_VAL_RULES *)&Fap10m_scheduler_port_id_vals[0],
    (sizeof(Fap10m_scheduler_port_id_vals) / sizeof(Fap10m_scheduler_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first port number.\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NOF_PORTS_ID,
    "nof_ports",
    (PARAM_VAL_RULES *)&Fap10m_scheduler_port_id_vals[0],
    (sizeof(Fap10m_scheduler_port_id_vals) / sizeof(Fap10m_scheduler_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of ports.\r\n",
    "Examples:\r\n"
    "  fap10m tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_SCENARIO_PKT_ID,
    "packet_utils",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m cells test.\r\n",
    "Examples:\r\n"
    "  fap10m tests packet_utils send_packet_test_fdt 0 target_fap_id 0 pattern 0xFF0022FF pkt_byte_size 1024 nof_pkts 1000\r\n"
    "Results in activating packet tests.\r\n",
    "",
    {0,
     BIT(30) | BIT(31),
     BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_SCENARIO_1_ID,
    "cells",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m cell test.\r\n",
    "Examples:\r\n"
    "  fap10m test cells\r\n"
    "Results in activating cells tests using direct/deferred.\r\n",
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
    PARAM_FAP10M_TEST_SCENARIO_2_ID,
    "sch_flows",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m scheduler-flows test.\r\n",
    "Examples:\r\n"
    "  fap10m test sch_flows\r\n"
    "Results in activating scheduler-flows tests.\r\n",
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
    PARAM_FAP10M_TEST_SCENARIO_3_ID,
    "sch_ports",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m scheduler-ports test.\r\n",
    "Examples:\r\n"
    "  fap10m test sch_ports\r\n"
    "Results in activating scheduler-ports tests.\r\n",
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
    PARAM_FAP10M_TEST_SCENARIO_4_ID,
    "packets",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m traffic test.\r\n",
    "Examples:\r\n"
    "  fap10m test packets\r\n"
    "Results in activating pakcets tests using direct/deferred.\r\n",
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
    PARAM_FAP10M_TEST_SCENARIO_5_ID,
    "links",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m links test.\r\n",
    "Examples:\r\n"
    "  fap10m test links\r\n"
    "Results in activating links tests using direct and deferred calls.\r\n",
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
    PARAM_FAP10M_TEST_SCENARIO_6_ID,
    "routing",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m routing test.\r\n",
    "Examples:\r\n"
    "  fap10m test_scenario_routing direct \r\n"
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
    PARAM_FAP10M_TEST_SCENARIO_7_ID,
    "statistics",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m statistics test.\r\n",
    "Examples:\r\n"
    "  fap10m test_scenario_statistics direct \r\n"
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
    PARAM_FAP10M_TEST_SCENARIO_8_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m diagnostics test.\r\n",
    "Examples:\r\n"
    "  fap10m tests diagnostics nof_packets 22\r\n"
    "Results in activating diagnostics tests using direct (and not deferred calls).\r\n",
    "",
    {0,0,0, BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_SCENARIO_8_NOF_PACKETS_ID,
    "nof_packets",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m diagnostics test .\r\n",
    "Examples:\r\n"
    "  fap10m tests diagnostics nof_packets 22\r\n"
    "Results in activating diagnostics tests using direct (and not deferred calls).\r\n",
    "",
    {0,0,0, BIT(23)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TEST_SCENARIO_9_ID,
    "general",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m agent general module test.\r\n",
    "Examples:\r\n"
    "  fap10m test general\r\n"
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
    PARAM_FAP10M_TEST_SCENARIO_10_ID,
    "spatial_multicast",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m agent spatial-multicast module test.\r\n",
    "Examples:\r\n"
    "  fap10m test spatial_multicast\r\n"
    "Results in activating test on the spatial-multicast module through the FMC.\r\n",
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
    PARAM_FAP10M_TEST_SCENARIO_11_ID,
    "recycle_multicast",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m agent recycle-multicast module test.\r\n",
    "Examples:\r\n"
    "  fap10m test recycle_multicast\r\n"
    "Results in activating test on the recycle-multicast module through the FMC.\r\n",
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
    PARAM_FAP10M_TEST_SCENARIO_12_ID,
    "configuration",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap10m agent configuration module test.\r\n",
    "Examples:\r\n"
    "  fap10m test configuration\r\n"
    "Results in activating test on the configuration module through the FMC.\r\n",
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
    PARAM_FAP10M_TEST_SSR_RESET_ID,
    "ssr_reset",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "performs fap10m ssr reset.\r\n",
    "Examples:\r\n"
    "fap10m test init_close \r\n"
    "Results in activating test performing ssr save, restarting the application and loading ssr data \r\n",
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
    PARAM_FAP10M_SEND_PACKET_TEST_FDT_ID,
    "send_packet_test_fdt",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure transmits packets through the FDT\n\r"
    "  and expect to recive them through the CPU interface.\r\n"
    "  It them check that the packet payload was unchanged.",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_test_fdt 0 nof_pkts 10 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SEND_PACKET_TEST_PP_ID,
    "send_packet_to_pp",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure transmits packets through the FDT\n\r"
    "  and expect to receive them through the PP port number 5.",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_to_pp 0 nof_pkts 10\r\n"
    "Results in sending packet (fixed one), through the FDT,\n\r"
    "  via the fabric (or loop-backs), to the FAP.\n\r"
    "  Next to the PP and to PORT 5, VLAN 1.\r\n"
    "  Note: We\'ve reached 700 Meg-bit/sec through this method\r\n",
    "",
    {0,
     BIT(31) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SEND_PACKET_PRINT_ID,
    "packet_type_print",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure print the packet send via \'send_packet_to_pp\'",
    "Examples:\r\n"
    "  fap10m tests packets packet_type_print packet_type 0\r\n"
    "Results in printing packet number 0.",
    "",
    {0,
     0,
     BIT(16) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SEND_PACKET_TEST_PP_TYPE_ID,
    "packet_type",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Chose the pattern/size of data to send.\n\r"
    "  Use \'fap10m tests packets packet_type_print packet_type 0\'\n\r"
    "  to view the sent packet",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_to_pp 0 nof_pkts 10 packet_type 0\r\n"
    "Results in sending packet (64  bytes), through the FDT,\n\r"
    "  via the fabric (or loop-backs), to the FAP.\n\r"
    "  Next to the PP and to PORT 5, VLAN 1.\r\n"
    "  Note: We\'ve reached 700 Meg-bit/sec through this method\r\n",
    "",
    {0,
     BIT(31),
     BIT(16) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PORT_ID,
    "pp_port",
    (PARAM_VAL_RULES *)&Fap10m_scheduler_port_id_no_recyle_vals[0],
    (sizeof(Fap10m_scheduler_port_id_no_recyle_vals) / sizeof(Fap10m_scheduler_port_id_no_recyle_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_pp_port_default,
    0,0,0,0,0,0,0,
    "Prestera port to send packet to. Range 0-63 (63 is the CPU port)",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_to_pp 0 nof_pkts 10 packet_type 0 port 5\r\n"
    "Results in sending packet (64  bytes), through the FDT,\n\r"
    "  via the fabric (or loop-backs), to the FAP.\n\r"
    "  Next to the PP and to PORT 5, VLAN 1.\r\n"
    "  Note: We\'ve reached 700 Meg-bit/sec through this method\r\n",
    "",
    {0,
     BIT(31)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_VLAN_ID_ID,
    "vlan_id",
    (PARAM_VAL_RULES *)&Fap10m_vlan_id_vals[0],
    (sizeof(Fap10m_vlan_id_vals) / sizeof(Fap10m_vlan_id_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_vlan_id_default,
    0,0,0,0,0,0,0,
    "Prestera VLAN id. Range 0-4095",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_to_pp 0 nof_pkts 10 packet_type 0 port 5 vlan_id 1\r\n"
    "Results in sending packet (64  bytes), through the FDT,\n\r"
    "  via the fabric (or loop-backs), to the FAP.\n\r"
    "  Next to the PP and to PORT 5, VLAN 1.\r\n"
    "  Note: We\'ve reached 700 Meg-bit/sec through this method\r\n",
    "",
    {0,
     BIT(31)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NOF_PACKETS_ID,
    "nof_pkts",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure transmits packets through the FDT\n\r"
    "  and expect to recive them through the CPU interface.\r\n"
    "  It them check that the packet payload was unchanged.\n\r",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_test_fdt 0 nof_pkts 10 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) | BIT(31) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PACKET_BYTE_SIZE_ID,
    "pkt_byte_size",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet payload byte size.\n\r",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_test_fdt 0 nof_pkts 10 pkt_byte_size 60 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PACKET_PATTERN_ID,
    "pattern",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet payload byte size.\n\r",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_test_fdt 0 nof_pkts 10 pkt_byte_size 60 target_fap_id 0 pattren 0xFE\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FAP_ID_ID,
    "target_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target FAP identifier.",
    "Examples:\r\n"
    "  fap10m tests packets send_packet_test_fdt 0 nof_pkts 10 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) | BIT(31) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * END Test API }
   */
  /* { fap10m_api_auto_flow_management.h interface */
  {
    PARAM_FAP10M_AUTO_FLOW_MANAGEMENT_API_ID,
    "auto_flow_mngt",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver auto flow management services.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_auto_params 0 max_nof_ports 10 max_nof_fap 15\r\n"
    "Results in setting (on device 0) 'Fap10m_max_nof_ports' to 10 \n\r"
    "  and 'Fap10m_max_nof_faps' to 15.\n\r"
    "  This indicates on a system with 15 FAPs each FAP have 10 physical ports.\n\r",
    "",
    {0,
     0,
     BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(14) | BIT(15) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_SET_QUEUE_TYPE_PARAMS_ID,
    "set_queue_type_params",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set driver queues type to be load via 'fap_open_queues'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_queue_type_params fabric_class fabric_class_c queue_types 0 1 2 3\r\n"
    "Results in loading to the driver parameter 4 (fabric_class_c) queue types parameters\n\r",
    "",
    {0,
     0,
     BIT(15) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_QUEUE_TYPES_ID,
    "queue_types",
    (PARAM_VAL_RULES *)&Fap10m_auto_queue_types[0],
    (sizeof(Fap10m_auto_queue_types) / sizeof(Fap10m_auto_queue_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue types to load with 'fap10m_open_fap_queues()'.\n\r"
    "  Number of array entries is as indicated in 'fabric_class' 1, 2, 4 or 8.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_queue_type_params fabric_class fabric_class_c queue_types 0 1 2 3\r\n"
    "Results in loading to the driver parameter 4 (fabric_class_c) queue types parameters\n\r",
    "",
    {0,
     0,
     BIT(15) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_SET_ONE_FLOW_PARAMS_ID,
    "set_one_flow_params",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set one flow parameter to user with 'open_fap_flow'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_one_flow_params fabric_class_entry 4 sub_0_flow_type wfq1 sub_0_flow_value 222\r\n"
    "Results in setting entry 4 to 2 sub flows (with there appropriate values)\n\r"
    "  and not low jitter flow.\n\r",
    "",
    {0,
     0,
     BIT(13) | BIT(14) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FABRIC_CLASS_ENTRY_ID,
    "fabric_class_entry",
    (PARAM_VAL_RULES *)&Fap10m_fabric_class_entry[0],
    (sizeof(Fap10m_fabric_class_entry) / sizeof(Fap10m_fabric_class_entry[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "One of 8 possiable fabric class ('fap10m_get_fabric_class()').",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_one_flow_params fabric_class_entry 4 sub_0_flow_type wfq1 sub_0_flow_value 222\r\n"
    "Results in setting entry 4 to 2 sub flows (with there appropriate values)\n\r"
    "  and not low jitter flow.\n\r",
    "",
    {0,
     0,
     BIT(13) | BIT(14) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_GET_QUEUE_TYPE_PARAMS_ID,
    "get_queue_type_params",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get array queue types from the driver.\n\r"
    "  This queue types get loaded to the devices with 'open_fap_queues'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt get_queue_type_paramss fabric_class fabric_class_c\r\n"
    "Results in displaying 4 (fabric_class_c) queue types.\n\r"
    "  Note that this fabric class should be the same for all device in the fabric.\n\r",
    "",
    {0,
     0,
     BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_GET_FLOW_PARAMS_ID,
    "get_flow_params",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get array flow parameters from the driver.\n\r"
    "  This parameters get loaded to the devices with 'open_fap_flows'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt get_flow_params fabric_class fabric_class_c\r\n"
    "Results in displaying 4 (fabric_class_c) flow parameters.\n\r"
    "  Note that this fabric class should be the same for all device in the fabric.\n\r",
    "",
    {0,
     0,
     BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FABRIC_CLASS_ID,
    "fabric_class",
    (PARAM_VAL_RULES *)&Fap10m_fabric_class[0],
    (sizeof(Fap10m_fabric_class) / sizeof(Fap10m_fabric_class[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of entries in auto-management module.\n\r"
    "  This size should match the number of classes the FAP\n\r"
    "  is configured to support ('fap10m_get_fabric_class()').\n\r",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt get_flow_params fabric_class fabric_class_c\r\n"
    "Results in displaying 4 (fabric_class_c) flow parameters.\n\r"
    "  Note that this fabric class should be the same for all device in the fabric.\n\r",
    "",
    {0,
     0,
     BIT(12) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_OPEN_FAP_QUEUES_ID,
    "open_fap_queues",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Static queues to 'target_fap_id' are opened.\n\r"
    "  The queue types loaded to that queue is set with\n\r"
    "  'fap10m_set_auto_queue_type_params'.\n\r"
    "  This procedure open the queues through repeated\n\r"
    "  calls to 'fap10m_open_queue'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt open_fap_queues 1 target_fap_id 123\r\n"
    "Results in automatic queue managment opening (on device 1)\n\r"
    " input queues aimed to target fap 123.\n\r"
    " Number of queues open on the device per port is determained\n\r"
    " through 'fap10m_set_fabric_class()'.\n\r",
    "",
    {0,
     0,
     BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_CLOSE_FAP_QUEUES_ID,
    "close_fap_queues",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Static queues to 'target_fap_id' are closed.\r\n"
    "  This procedure open the queues through \n\r"
    "  repeated calls to 'fap10m_close_queue'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt close_fap_queues 0 target_fap_id 123\r\n"
    "Results in automatic queue managment closing (on device 0)\n\r"
    " input queue aimed to target fap 123.\n\r",
    "",
    {0,
     0,
     BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FAP_ID_ID,
    "target_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target FAP identifier.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt close_fap_queues 0 target_fap_id 123\r\n"
    "Results in automatic queue managment closing (on device 0)\n\r"
    " input queue aimed to target fap 123.\n\r",
    "",
    {0,
     0,
     BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_OPEN_FAP_FLOWS_ID,
    "open_fap_flows",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Static flows to 'source_fap_id' are opened.\n\r"
    "  The flows parameters loaded to these flows are set\n\r"
    "  with fap10m_auto_set_one_flow_params().\n\r"
    "  This procedure open the queues through\n\r"
    "  repeated calls to 'fap10m_open_scheduler_flow'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt open_fap_flows 1 source_fap_id 123\r\n"
    "Results in automatic flow managment opening (on device 1)\n\r"
    " scheduler flows aimed to source fap 123.\n\r"
    " Number of flow open on the devices per port is determained\n\r"
    " through 'fap10m_set_fabric_class()'.\n\r",
    "",
    {0,
     0,
     BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_CLOSE_FAP_FLOWS_ID,
    "close_fap_flows",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Static flows to 'source_fap_id' are closed.\r\n"
    "  Close the flows through repeated calls to\n\r"
    "  'fap10m_close_scheduler_flow'.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt close_fap_flows 0 source_fap_id 123\r\n"
    "Results in automatic flow managment closing (on device 0)\n\r"
    " scheduler flows aimed to source fap 123.\n\r",
    "",
    {0,
     0,
     BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SOURCE_FAP_ID_ID,
    "source_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Source FAP identifier.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt close_fap_flows 0 source_fap_id 123\r\n"
    "Results in automatic flow managment closing (on device 0)\n\r"
    " scheduler flows aimed to source fap 123.\n\r",
    "",
    {0,
     0,
     BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_FLOW_MNGT_GET_PARAMS_ID,
    "get_auto_params",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets 'Fap10m_max_nof_ports' and 'Fap10m_max_nof_faps' -\n\r"
    "  auto-flow-management parameters.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt get_auto_params 0\r\n"
    "Results in getting (on device 0) 'Fap10m_max_nof_ports' and\n\r"
    " and 'Fap10m_max_nof_faps' parameters.\n\r",
    "",
    {0,
     0,
     BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AUTO_FLOW_MNGT_SET_PARAMS_ID,
    "set_auto_params",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets 'Fap10m_max_nof_ports' and 'Fap10m_max_nof_faps'  -\n\r"
    "  auto-flow-management parameters.",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_auto_params 0 max_nof_ports 10 max_nof_fap 15 fabric_class fabric_class_c\r\n"
    "Results in setting (on device 0) 'Fap10m_max_nof_ports' to 10,\n\r"
    " 'Fap10m_max_nof_faps' to 15 and 'Fap10m_fabric_class' to fabric_class_c.\n\r"
    " This indicates on a system with 15 FAPs each FAP have 10 physical ports\n\r"
    " and that every 2 Packet-Processor-Header classes are mapped to one fabric class.",
     "",
    {0,
     0,
     BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_NOF_FAPS_ID,
    "max_nof_faps",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max number of FAPs one wishes to support.\n\r"
    "  This number should be the same on every SOC_SAND_FAP10M driver in the system",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_auto_params 0 max_nof_ports 10 max_nof_fap 15 fabric_class fabric_class_c\r\n"
    "Results in setting (on device 0) 'Fap10m_max_nof_ports' to 10,\n\r"
    " 'Fap10m_max_nof_faps' to 15 and 'Fap10m_fabric_class' to fabric_class_c.\n\r"
    " This indicates on a system with 15 FAPs each FAP have 10 physical ports\n\r"
    " and that every 2 Packet-Processor-Header classes are mapped to one fabric class.",
    "",
    {0,
     0,
     BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_NOF_PORTS_ID,
    "max_nof_ports",
    (PARAM_VAL_RULES *)&Fap10m_max_nof_ports[0],
    (sizeof(Fap10m_max_nof_ports) / sizeof(Fap10m_max_nof_ports[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max number of physical ports one wishes to support on every FAP.\n\r"
    "  This number should include the PP-CPU port.\n\r"
    "  E.g. in 12-port PP this number should be 13.\n\r"
    "  This number should be the same on every SOC_SAND_FAP10M driver in the system",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_auto_params 0 max_nof_ports 10 max_nof_fap 15 fabric_class fabric_class_c\r\n"
    "Results in setting (on device 0) 'Fap10m_max_nof_ports' to 10,\n\r"
    " 'Fap10m_max_nof_faps' to 15 and 'Fap10m_fabric_class' to fabric_class_c.\n\r"
    " This indicates on a system with 15 FAPs each FAP have 10 physical ports\n\r"
    " and that every 2 Packet-Processor-Header classes are mapped to one fabric class.",
     "",
    {0,
     0,
     BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FABRIC_CLASS_ID,
    "fabric_class",
    (PARAM_VAL_RULES *)&Fap10m_fabric_class[0],
    (sizeof(Fap10m_fabric_class) / sizeof(Fap10m_fabric_class[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of entries in auto-management module.\n\r"
    "  This size should match the number of classes the FAP\n\r"
    "  is configured to support ('fap10m_get_fabric_class()').\n\r",
    "Examples:\r\n"
    "  fap10m auto_flow_mngt set_auto_params 0 max_nof_ports 10 max_nof_fap 15 fabric_class fabric_class_c\r\n"
    "Results in setting (on device 0) 'Fap10m_max_nof_ports' to 10,\n\r"
    " 'Fap10m_max_nof_faps' to 15 and 'Fap10m_fabric_class' to fabric_class_c.\n\r"
    " This indicates on a system with 15 FAPs each FAP have 10 physical ports\n\r"
    " and that every 2 Packet-Processor-Header classes are mapped to one fabric class.",
    "",
    {0,
     0,
     BIT(12) | BIT(15) | BIT(9)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END fap10m_api_auto_flow_management.h interface }*/

  /* { fap10m_api_configuration.h interface */
  {
    PARAM_FAP10M_CONFIGURATION_API_ID,
    "configuration",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver configuration services.",
    "Examples:\r\n"
    "  fap10m configuration set_fap_id 1 fap_id 123 \r\n"
    "Results in setting on device 1, fap identifier of 123.\n\r"
    "This fap identifier is the identifier that the device is seen\n\r"
    "in the fabric system. while the 'unit' is the identifier\n\r"
    "of that fap in the context of the local driver\r\n",
    "",
    {0,
     0,
     BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(25) | BIT(26) | BIT(27)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_FABRIC_CLASS_ID,
    "get_fabric_class",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the used fabric class.",
    "Examples:\r\n"
    "  fap10m configuration get_fabric_class 0\r\n"
    "Results in getting (on device 0) the fabric class used.\n\r"
    " class a: PFH class {0,...,7} mapped to fabric class 0.\n\r"
    " class b: PFH class {0,...,3} mapped to fabric class 0.\r\n"
    "          PFH class {4,...,7} mapped to fabric class 1.\r\n"
    " class c: PFH class {0,1} mapped to fabric class 0.\r\n"
    "          PFH class {2,3} mapped to fabric class 1.\r\n"
    "          PFH class {4,5} mapped to fabric class 2.\r\n"
    "          PFH class {6,7} mapped to fabric class 3.\r\n"
    " class d: PFH class X mapped to fabric class X,\r\n"
    "          that is, No translation occurs .\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_FABRIC_CLASS_ID,
    "set_fabric_class",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the used fabric class.",
    "Examples:\r\n"
    "  fap10m configuration set_fabric_class 0 fabric_class class_b\r\n"
    "Results in setting (on device 0) for fabric class to class b.\n\r"
    " class a: PFH class {0,...,7} mapped to fabric class 0.\n\r"
    " class b: PFH class {0,...,3} mapped to fabric class 0.\r\n"
    "          PFH class {4,...,7} mapped to fabric class 1.\r\n"
    " class c: PFH class {0,1} mapped to fabric class 0.\r\n"
    "          PFH class {2,3} mapped to fabric class 1.\r\n"
    "          PFH class {4,5} mapped to fabric class 2.\r\n"
    "          PFH class {6,7} mapped to fabric class 3.\r\n"
    " class d: PFH class X mapped to fabric class X,\r\n"
    "          that is, No translation occurs .\r\n",
    "",
    {0,
     0,
     BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FABRIC_CLASS_ID,
    "fabric_class",
    (PARAM_VAL_RULES *)&Fap10m_fabric_class[0],
    (sizeof(Fap10m_fabric_class) / sizeof(Fap10m_fabric_class[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Fabric class.",
    "Examples:\r\n"
    "  fap10m configuration set_fabric_class 0 fabric_class class_b\r\n"
    "Results in setting (on device 0) for fabric class to class b.\n\r",
    "",
    {0,
     0,
     BIT(7)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_WRED_PARAMS_ID,
    "set_wred_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the WRED parameters of the specific 'queue_type' and 'drop_precedence'.",
    "Examples:\r\n"
    "  fap10m configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_PACKET_SIZE_ID,
    "max_packet_size",
    (PARAM_VAL_RULES *)&Fap10m_max_packet_size[0],
    (sizeof(Fap10m_max_packet_size) / sizeof(Fap10m_max_packet_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max packet size for the WRED algorithm.\n\r"
    "  Size of packet in bytes.",
    "Examples:\r\n"
    "  fap10m configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_PROBABILITY_ID,
    "max_probability",
    (PARAM_VAL_RULES *)&Fap10m_percent[0],
    (sizeof(Fap10m_percent) / sizeof(Fap10m_percent[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The maximum probability of discarding a packet (when the queue reaches\n\r"
    "  the maximum average size ('max_avrg_th') and the packet size is the\n\r"
    "  maximum size ('max_packet_size')).",
    "Examples:\r\n"
    "  fap10m configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MIN_AVRG_TH_ID,
    "min_avrg_th",
    (PARAM_VAL_RULES *)&Fap10m_min_avrg_th[0],
    (sizeof(Fap10m_min_avrg_th) / sizeof(Fap10m_min_avrg_th[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Relating to queue average size.\n\r"
    "  Maximum Average Threshold - Above this threshold,\n\r"
    "  packet is discarded from the queue.\n\r"
    "  Resolution of 1-Kbyte (64 is 64K bytes)\n\r"
    "  The SOC_SAND_FAP10M resolution is of 64K bytes (1-63 KB will be round up to 64 KB)",
    "Examples:\r\n"
    "  fap10m configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_AVRG_TH_ID,
    "max_avrg_th",
    (PARAM_VAL_RULES *)&Fap10m_max_avrg_th[0],
    (sizeof(Fap10m_max_avrg_th) / sizeof(Fap10m_max_avrg_th[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Relating to queue average size.\n\r"
    "  Maximum Average Threshold - Above this threshold,\n\r"
    "  packet is discarded from the queue.\n\r"
    "  Resolution of 1-Kbyte (64 is 64K bytes)\n\r"
    "  The SOC_SAND_FAP10M resolution is of 64K bytes (1-63 KB will be round up to 64 KB)",
    "Examples:\r\n"
    "  fap10m configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP10M_GET_WRED_PARAMS_ID,
    "get_wred_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the WRED parameters of the specific 'queue_type' and 'drop_precedence'.",
    "Examples:\r\n"
    "  fap10m configuration get_wred_params 0 queue_type 2 drop_precedence 3\r\n"
    "Results in getting (on device 0) for queue type 2 \n\r"
    "  and drop precedence of 3 its WRED parameters.\r\n",
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
    PARAM_FAP10M_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&Fap10m_drop_precedence[0],
    (sizeof(Fap10m_drop_precedence) / sizeof(Fap10m_drop_precedence[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Drop precedence identifier.",
    "Examples:\r\n"
    "  fap10m configuration get_wred_params 0 queue_type 2 drop_precedence 3\r\n"
    "Results in getting (on device 0) for queue type 2 \n\r"
    "  and drop precedence of 3 its WRED parameters.\r\n",
    "",
    {0,
     0,
     BIT(5) | BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL  },

  {
    PARAM_FAP10M_SET_QUEUE_TYPE_PARAMS_ID,
    "set_queue_type_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the required queue type parameters.",
    "Examples:\r\n"
    "  fap10m configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 RED_exp_weight 5 flow_slow_th 300 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_QUEUE_SIZE_ID,
    "max_queue_size",
    (PARAM_VAL_RULES *)&Fap10m_max_queue_size[0],
    (sizeof(Fap10m_max_queue_size) / sizeof(Fap10m_max_queue_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max queue size. When a queue reaches this max size,\n\r"
    "  all packets reaches this queue are droped.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap10m configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 RED_exp_weight 5 flow_slow_th 300 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FLOW_STAT_MSG_ID,
    "flow_stat_msg",
    (PARAM_VAL_RULES *)&Fap10m_flow_stat_msg[0],
    (sizeof(Fap10m_flow_stat_msg) / sizeof(Fap10m_flow_stat_msg[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Every integer multiple of this parameter, a status message is sent to\n\r"
    "  the flow related to that queue.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap10m configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 RED_exp_weight 5 flow_slow_th 300 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FLOW_SLOW_TH_ID,
    "flow_slow_th",
    (PARAM_VAL_RULES *)&Fap10m_flow_slow_th[0],
    (sizeof(Fap10m_flow_slow_th) / sizeof(Fap10m_flow_slow_th[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Beneath this threshold, the queue is in 'Slow' state.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap10m configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 RED_exp_weight 5 flow_slow_th 300 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FLOW_SLOW_HYS_ID,
    "flow_slow_hysteresis_size",
    (PARAM_VAL_RULES *)&Fap10m_flow_slow_hyster[0],
    (sizeof(Fap10m_flow_slow_hyster) / sizeof(Fap10m_flow_slow_hyster[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_flow_slow_hyster_default,
    0,0,0,0,0,0,0,
    "Hysteresis size for slow status.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap10m configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 RED_exp_weight 5 flow_slow_th 300 flow_slow_hysteresis_size 100\r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " if the queue size pass the size of 300 the queue is considered in 'Fast' state,\n\r"
    " if the queue size is beneath (300-100) the queue is back to 'Slow' state,\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_RED_EXP_WEIGHT_ID,
    "red_exp_weight",
    (PARAM_VAL_RULES *)&Fap10m_red_exp_weight[0],
    (sizeof(Fap10m_red_exp_weight) / sizeof(Fap10m_red_exp_weight[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Weight for average queue size calculation.\n\r"
    "  If not appears disable WRED algorithm to this queue type.\n\r"
    "  The average calculation:\n\r"
    "    Average_constant = 2^(-RED_exp_weight)\n\r"
    "    if (Instantaneous-Queue-size < Average-queue-size)\n\r"
    "      Average-queue-size = Instantaneous-Queue-size\n\r"
    "    else\n\r"
    "      Average-queue-size = \n\r"
    "          (1-Average_constant)*Average-queue-size +\n\r"
    "          Average_constant*Instantaneous-Queue-size\n\r"
    "  Range  0-31, I.e., make the average factor from 1 to 1^(-31).",
    "Examples:\r\n"
    "  fap10m configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 RED_exp_weight 5 flow_slow_th 300 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_QUEUE_TYPE_PARAMS_ID,
    "get_queue_type_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the queue parameters of 'queue_type'.",
    "Examples:\r\n"
    "  fap10m configuration get_queue_type_params 1 queue_type 2\r\n"
    "Results in getting (on device 1) queue type 2 parameters.\r\n",
    "",
    {0,
     0,
     BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_TYPE_ID,
    "queue_type",
    (PARAM_VAL_RULES *)&Fap10m_queue_type[0],
    (sizeof(Fap10m_queue_type) / sizeof(Fap10m_queue_type[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue type number.",
    "Examples:\r\n"
    "  fap10m configuration get_queue_type_params 1 queue_type 2\r\n"
    "Results in getting (on device 1) queue type 2 parameters.\r\n",
    "",
    {0,
     0,
     BIT(3) | BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP10M_GET_FAP_ID_ID,
    "get_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get fap identifier (as seen in the system).",
    "Examples:\r\n"
    "  fap10m configuration get_fap_id 1\r\n"
    "Results in getting the (on device 1) its system identifier.\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_FAP_ID_ID,
    "set_fap_id",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the device with its identifier (as seen in the system).",
    "Examples:\r\n"
    "  fap10m configuration set_fap_id 1 fap_id 123\r\n"
    "Results in setting on device 1, fap identifier of 123.\n\r"
    "This fap identifier is the identifier that the device is seen\n\r"
    "in the fabric system. while the 'unit' is the identifier\n\r"
    "of that fap in the context of the local driver\r\n",
    "",
    {0,
     0,
     BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FAP_ID_ID,
    "fap_id",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The designated physical FAP id.",
    "Examples:\r\n"
    "  fap10m configuration set_fap_id 1 fap_id 123\r\n"
    "Results in setting on device 1, fap identifier of 123.\n\r"
    "This fap identifier is the identifier that the device is seen\n\r"
    "in the fabric system. while the 'unit' is the identifier\n\r"
    "of that fap in the context of the local driver\r\n",
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
    PARAM_FAP10M_GET_EDF1_PRIOR_ID,
    "get_edf1_strict_prior",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get EDF1 strict priority over WFQ1.",
    "Examples:\r\n"
    "  fap10m configuration get_edf1_strict_prior 1\r\n"
    "Results in getting the edf1 strict priority setting on device 1.\n\r"
    "When enabled, all the scheduler ports will give the EDF1\n\r"
    "wheel strict priority over the WFQ1 wheel.\n\r"
    "The WFQ0 percentage field in the port setting is then meaningless\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_EDF1_PRIOR_ID,
    "set_edf1_strict_prior",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set EDF1 strict priority over WFQ1.",
    "Examples:\r\n"
    "  fap10m configuration get_edf1_strict_prior 1 edf1_prior enable\r\n"
    "Results in setting edf1 strict priority over wfq1 on device 1.\n\r"
    "All the scheduler ports will give the EDF1\n\r"
    "wheel strict priority over the WFQ1 wheel.\n\r"
    "The WFQ0 percentage field in the port setting is meaningless.\r\n",
    "",
    {0,
     0,
     BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_EDF1_PRIOR_ID,
    "edf1_prior",
    (PARAM_VAL_RULES *)&Fap10m_edf1_prior[0],
    (sizeof(Fap10m_edf1_prior) / sizeof(Fap10m_edf1_prior[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable/Disable EDF1 strict priority.",
    "Examples:\r\n"
    "  fap10m configuration get_edf1_strict_prior 1 edf1_prior enable\r\n"
    "Results in setting edf1 strict priority over wfq1 on device 1.\n\r"
    "All the scheduler ports will give the EDF1\n\r"
    "wheel strict priority over the WFQ1 wheel.\n\r"
    "The WFQ0 percentage field in the port setting is meaningless.\r\n",
    "",
    {0,
     0,
     BIT(25)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },


  {
    PARAM_FAP10M_GET_CHOP_EN_ID,
    "get_chopping_enable",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get big packets chopping configuration.",
    "Examples:\r\n"
    "  fap10m configuration get_chopping_enable 1\r\n"
    "Results in getting the big packets chopping configuration on device 1.\n\r"
    "Indicate whether the chopping is enabled, and what is the maximal\r\n"
    "multicast id to be chopped for multicast packets.",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_CHOP_EN_ID,
    "set_chopping_enable",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set big packets chopping configuration.",
    "Examples:\r\n"
    "  fap10m configuration set_chopping_enable 1 chop_en enable \r\n"
    "    maximal_multicast_id 800"
    "Results in setting big packets to be chopped on device 1.\n\r"
    "If the packet is unicast, or multicast with multicast id\n\r"
    "not bigger than 800.\r\n",
    "",
    {0,
     0,
     BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CHOP_EN_ID,
    "chop_en",
    (PARAM_VAL_RULES *)&Fap10m_chop_en[0],
    (sizeof(Fap10m_chop_en) / sizeof(Fap10m_chop_en[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable/Disable chopping of big packets.",
    "Examples:\r\n"
    "  fap10m configuration set_chopping_enable 1 chop_en enable \r\n"
    "    maximal_multicast_id 800"
    "Results in setting big packets to be chopped on device 1.\n\r"
    "If the packet is unicast, or multicast with multicast id\n\r"
    "not bigger than 800.\r\n",
    "",
    {0,
     0,
     BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CHOP_MULT_ID_ID,
    "maximal_multicast_id",
    (PARAM_VAL_RULES *)&Fap10m_chop_max_mult_id[0],
    (sizeof(Fap10m_chop_max_mult_id) / sizeof(Fap10m_chop_max_mult_id[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_chop_max_mult_id_default,
    0,0,0,0,0,0,0,
    "Set the maximal multicast id to support packet chopping.",
    "Examples:\r\n"
    "  fap10m configuration set_chopping_enable 1 chop_en enable \r\n"
    "    maximal_multicast_id 800"
    "Results in setting big packets to be chopped on device 1.\n\r"
    "If the packet is unicast, or multicast with multicast id\n\r"
    "not bigger than 800.\r\n",
    "",
    {0,
     0,
     BIT(26)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP10M_GET_PROFILE_ID,
    "get_profile",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the identifier of current loaded 'profile' \n\r"
    "  (the profile identifier loaded with 'set_profile').",
    "Examples:\r\n"
    "  fap10m configuration get_profile 1\r\n"
    "Results in getting the (on device 1) its loaded profile identifier.\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_PROFILE_ID,
    "set_profile",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets sets device with pre-defined profile.\n\r"
    "  A large set of registers is written to the device.",
    "Examples:\r\n"
    "  fap10m configuration set_profile 1 profile_id 0\r\n"
    "Results in setting the (on device 1) its profile number 1.\r\n",
    "",
    {0,
     0,
     BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PROFILE_ID_ID,
    "profile_id",
    (PARAM_VAL_RULES *)&Fap10m_profile_id[0],
    (sizeof(Fap10m_profile_id) / sizeof(Fap10m_profile_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The profile identifier, which the driver load the device.",
    "Examples:\r\n"
    "  fap10m configuration set_profile_id 1 profile_id 0\r\n"
    "Results in setting the (on device 1) its profile number 1.\r\n",
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
    PARAM_FAP10M_GET_PROFILE_VALUES_ID,
    "get_profile_values",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the profile values from the device.\n\r"
    "  A large set of register is read from the device.",
    "Examples:\r\n"
    "  fap10m configuration get_profile_values 1\r\n"
    "Results in getting the (on device 1) loaded profile values from the device.\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END fap10m_api_configuration.h interface }*/


  /* { fap10m_api_scheduler_flows.h interface */
  {
    PARAM_FAP10M_SCHEDULER_FLOW_API_ID,
    "flow",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver scheduler flow services.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay\r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(26) | BIT(29),
     0,
     BIT(0) | BIT(1),
     BIT(0)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CLOSE_SCHEDULER_FLOW_ID,
    "close_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get scheduler flow parameters.",
    "Examples:\r\n"
    "  fap10m scheduler flow close_flow 1 flow_id 54\r\n"
    "Results in closing on device 1 scheduler flow 54.\r\n",
    "",
    {0,
     BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_SCHEDULER_FLOW_PARAMS_ID,
    "get_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get scheduler flow parameters.",
    "Examples:\r\n"
    "  fap10m scheduler flow get_flow 1 flow_id 54\r\n"
    "Results in getting on device 1 the parameters of scheduler flow 54.\r\n",
    "",
    {0,
     BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_SCHEDULER_RECYCLE_FLOW_ID,
    "update_recycle_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Updates recycle scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap10m scheduler flow update_recycle_flow 1 flow_id 122 source_fap 90 ingress_queue 2305 low_delay weight 12\r\n"
    "Resulting in updating (on device 1) flow 122 to be recycle flow, \n\r"
    " which to be installed on scheduler port 64,\r\n"
    " send it credits to fap 90 queue 2035 (on the fap).\n\r"
    " The flow weight on the recycle wheel is 12.\n\r",
    "",
    {0,
     BIT(29)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_OPEN_SCHEDULER_RECYCLE_FLOW_ID,
    "open_recycle_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Opens recycle scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_recycle_flow 1 flow_id 122 source_fap 90 ingress_queue 2305 low_delay weight 12\r\n"
    "Resulting in opening (on device 1) flow 122 to be recycle flow, \n\r"
    " which to be installed on scheduler port 64,\r\n"
    " send it credits to fap 90 queue 2035 (on the fap).\n\r"
    " The flow weight on the recycle wheel is 12.\n\r",
    "",
    {0,
     BIT(29)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SCHEDULER_RECYCLE_FLOW_WEIGHT_ID,
    "weight",
    (PARAM_VAL_RULES *)&fap10m_scheduler_recycle_flow_weight_vals[0],
    (sizeof(fap10m_scheduler_recycle_flow_weight_vals) / sizeof(fap10m_scheduler_recycle_flow_weight_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "OWeight of that flow on the recycle wheel.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_recycle_flow 1 flow_id 122 source_fap 90 ingress_queue 2305 weight 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122 to be recycle flow, \n\r"
    " which to be installed on scheduler port 64,\r\n"
    " send it credits to fap 90 queue 2035 (on the fap).\n\r"
    " The flow weight on the recycle wheel is 12.\n\r",
    "",
    {0,
     BIT(29)},
    10,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_WEIGHT_ID,
    "update_weight",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Updates scheduler flow / aggregate weight.\n\r"
    "  Should only be called on active flow/aggregate.",
    "",
    "",
    {0,
     0,
     0,
     0,
     BIT(0)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_WEIGHT_SUB_INDX_ID,
    "sub_flow_index",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The index of the sub-flow.\n\r"
    "  Should be 0 or 1.",
    "",
    "",
    {0,
     0,
     0,
     0,
     BIT(0)
    },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_WEIGHT_WEIGHT_ID,
    "new_weight",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The weight to be set.\n\r"
    "  Should be 1 to 126",
    "",
    "",
    {0,
     0,
     0,
     0,
     BIT(0)
    },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_SCHEDULER_FLOW_ID,
    "update_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Updates scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap10m scheduler flow update_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in updating (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_SCHEDULER_FLOW_QOS_ID,
    "update_flow_qos",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Updates scheduler flow QOS, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap10m scheduler flow update_flow 1 flow_id 122  sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_jitter \r\n"
    "Resulting in updating (on device 1) flow 122, \n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     0,
     0,
     BIT(0) | BIT(1)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_OPEN_SCHEDULER_FLOW_ID,
    "open_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Open scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay\r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FLOW_ID_ID,
    "flow_id",
    (PARAM_VAL_RULES *)&Fap10m_flow_id[0],
    (sizeof(Fap10m_flow_id) / sizeof(Fap10m_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow, in the scheduler.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(26) | BIT(29),
     0,
     BIT(0) | BIT(1) ,
     BIT(0)
    },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SCHEDULER_PORT_ID_ID,
    "sch_port_id",
    (PARAM_VAL_RULES *)&Fap10m_scheduler_port_id_no_recyle_vals[0],
    (sizeof(Fap10m_scheduler_port_id_no_recyle_vals) / sizeof(Fap10m_scheduler_port_id_no_recyle_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port, the will is belonged to.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow will be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SOURCE_FAP_ID_ID,
    "source_fap",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(29)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_INGRESS_QUEUE_ID_ID,
    "ingress_queue",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay\r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(29) },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SUB_FLOW_0_TYPE_ID,
    "sub_0_flow_type",
    (PARAM_VAL_RULES *)&Fap10m_sub_flow_types[0],
    (sizeof(Fap10m_sub_flow_types) / sizeof(Fap10m_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25),
     BIT(13) | BIT(14),
     BIT(0) | BIT(1)
     },
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SUB_FLOW_0_VALUE_ID,
    "sub_0_flow_value",
    (PARAM_VAL_RULES *)&Fap10m_sub_flow_values[0],
    (sizeof(Fap10m_sub_flow_values) / sizeof(Fap10m_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.\n\r"
    " 'value' semantics depends on the 'type' value:\n\r"
    "\n\r"
    " FAP10M_SUB_FLOW_TYPE        | 'value' semantics\n\r"
    "----------------------------------------------\n\r"
    " FAP10M_EDF0_FLOW            | Kilo-bits - guaranteed rate\n\r"
    " FAP10M_EDF1_FLOW            | Kilo-bits -Maximal rate\n\r"
    " FAP10M_EDF0_PERCENTAGE_FLOW | 0-1000000: 90000  = 9%%\n\r"
    " FAP10M_EDF1_PERCENTAGE_FLOW | 0-1000000: 120000 = 12%%\n\r"
    " FAP10M_WFQ1_FLOW            | Weight. Range from 1 to 126.\n\r"
    "",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) ,
     BIT(13) | BIT(14),
     BIT(0) | BIT(1)
    },
    9,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SUB_FLOW_1_TYPE_ID,
    "sub_1_flow_type",
    (PARAM_VAL_RULES *)&Fap10m_sub_flow_types[0],
    (sizeof(Fap10m_sub_flow_types) / sizeof(Fap10m_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(25) ,
     BIT(13) ,
     BIT(1)
    },
    10,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SUB_FLOW_1_VALUE_ID,
    "sub_1_flow_value",
    (PARAM_VAL_RULES *)&Fap10m_sub_flow_values[0],
    (sizeof(Fap10m_sub_flow_values) / sizeof(Fap10m_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay\r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(25) ,
     BIT(13) ,
     BIT(1)
    },
    11,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_LOW_JITTER_ENABLE_ID,
    "low_delay",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, indicates that this flow will sent its credit\r\n"
    " regardless if its ingress queue has data.",
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(25) | BIT(29),
     BIT(13) ,
     BIT(1)
    },
    12,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_LOW_JITTER_ENABLE_ID,
    "low_delay",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, indicates that this flow will sent its credit\r\n"
    " regardless if its ingress queue has data.",
    "  fap10m scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 low_delay\r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) ,
     BIT(14) ,
     BIT(0)
    },
    10,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AGGREGATE_FLOW_ID,
    "aggregate_flow",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, the flow is aggregate.\r\n"
    " Only flows 0-63 might be aggregate flow. Not supported for FAP10M_A0.",
    "  fap10m scheduler flow open_flow 1 flow_id 22 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 aggregate_flow \r\n"
    "Resulting in opening (on device 1) flow 22, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(25) | BIT(29),
     BIT(13) ,
     BIT(1)
    },
    12,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_AGGREGATE_FLOW_ID,
    "aggregate_flow",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, the flow is aggregate.\r\n"
    " Only flows 0-63 might be aggregate flow. Not supported for FAP10M_A0.",
    "  fap10m scheduler flow open_flow 1 flow_id 22 sch_port_id 54 source_fap 90 ingress_queue 2305 sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12 aggregate_flow \r\n"
    "Resulting in opening (on device 1) flow 22, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) ,
     BIT(14) ,
     BIT(0)
    },
    10,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap10m_api_scheduler_flows.h interface }*/
  /* { fap10m_api_packet.h interface */
  {
    PARAM_FAP10M_PACKET_RESET_CAPTURE_BUFFR_ID,
    "packet_reset_capture_fifo",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reset the capture packet fifo at the device.\r\n",
    "Examples:\r\n"
    "  fap10m traffic packet_reset_capture_fifo 0 \r\n"
    "Results in packet capture fifo reset, all packets that were,\n\r"
    "  in the queue to the CPU to read, were deleted.\r\n",
    "",
    {0,
     BIT(22) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_RECV_PACKET_ID,
    "recv_packet",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure receives packet.\r\n",
    "Examples:\r\n"
    "  fap10m traffic recv_packet 0 \r\n"
    "Results in checking if there is packet in the device.\n\r"
    "Case there is, read and print it.\r\n",
    "",
    {0,
     BIT(22) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_DEFRRED_RECV_PACKET_ID,
    "recv_packet",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred receive (polling) packet.\r\n",
    "Examples:\r\n"
    "  fap10m deferred recv_packet 0x1 polling_rate 160\r\n"
    "Results in deferred reading a packets (were designated to the CPU)\n\r"
    " from device 0x1.\r\n",
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
    PARAM_FAP10M_SEND_PACKET_ID,
    "send_packet",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure transmits one packet.\r\n",
    "Examples:\r\n"
    "  fap10m traffic send_packet 0 target_fap 120 data 0x12 0x775487 0x76767 \r\n"
    "Results in sending to FAP with id of 120, from device 0, one packet\r\n"
    "  with 12 bytes with the above data.\r\n",
    "",
    {0,
     BIT(23) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FAP_ID_ID,
    "target_fap",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "One of FAPs identifier in the system.\r\n",
    "Examples:\r\n"
    "  fap10m traffic send_packet 0 target_fap 120 data 0x12 0x775487 0x76767 \r\n"
    "Results in sending to FAP with id of 120, from device 0, one packet\r\n"
    "  with 12 bytes with the above data.\r\n",
    "",
    {0,
     BIT(23) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PACKET_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Fap10m_packet_data[0],
    (sizeof(Fap10m_packet_data) / sizeof(Fap10m_packet_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Data to carry on packet payload. In case that data length is less \r\n"
    "then 64 bytes (16 long) it will be completed to 16 long with zero padding.\r\n",
    "Examples:\r\n"
    "  fap10m traffic send_packet 0 target_fap 120 data 0x12 0x775487 0x76767 \r\n"
    "Results in sending to FAP with id of 120, from device 0, one packet\r\n"
    "  with 12 bytes with the above data and 52 bytes of zero.\r\n",
    "",
    {0,
     BIT(23) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap10m_api_packet.h interface }*/
  /* { fap10m_api_recycle_multicast.h interface */
  {
    PARAM_FAP10M_RECYCLE_MULTICAST_API_ID,
    "recycle",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver recycle multicast module services.",
    "Examples:\r\n"
    "  fap10m multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) | BIT(18) | BIT(19) | BIT(20) | BIT(21) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_RECYCLING_QUEUE_ID,
    "get_recycling_queue",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure gets recycling queue parameters.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle get_recycling_queue 0 recycle_queue_index 12\r\n"
    "Results in get and display recycle queue index 12.\r\n",
    "",
    {0,
     BIT(21) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CLOSE_RECYCLING_QUEUE_ID,
    "close_recycling_queue",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure deletes a recycling queue by setting the FAP - \n\r"
    "  'recycle_queue_index' to be invalid.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle close_recycling_queue 0 recycle_queue_index 12\r\n"
    "Results in recycle queue index 12 to be invalid.\r\n",
    "",
    {0,
     BIT(21) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_OPEN_RECYCLING_QUEUE_ID,
    "open_recycling_queue",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure adds a recycling queue by setting the\r\n"
    "  'recycle_queue_index' to point to a the new 'queue_id'.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_recycling_queue 0 recycle_queue_index 127 queue_id 8191\r\n"
    "Results in recycle queue index 127 points to queue 8191 on device 0.\r\n",
    "",
    {0,
     BIT(20) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_RECYCLE_Q_INDEX_ID,
    "recycle_queue_index",
    (PARAM_VAL_RULES *)&Fap10m_recycle_queue_index[0],
    (sizeof(Fap10m_recycle_queue_index) / sizeof(Fap10m_recycle_queue_index[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Index in the Recycling-Queue table.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_recycling_queue 0 recycle_queue_index 127 queue_id 8191\r\n"
    "Results in recycle queue index 127 points to queue 8191 on device 0.\r\n",
    "",
    {0,
     BIT(20) | BIT(21)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_ID_ID,
    "queue_id",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue identifier.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_recycling_queue 0 recycle_queue_index 127 queue_id 8191\r\n"
    "Results in recycle queue index 127 points to queue 8191 on device 0.\r\n",
    "",
    {0,
     BIT(20) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CLOSE_RECYCLE_NODE_ID,
    "close_node",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure deletes one node form the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle close_node 0 tree_id 1221 \r\n"
    "Results in closing on device 0 for tree id 1221.\r\n",
    "",
    {0,
     BIT(19) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_RECYCLE_NODE_ID,
    "get_node",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure get and display one node form the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle get_node 0 tree_id 1221 \r\n"
    "Results in getting and displaying on device 0 node on tree id 1221.\r\n",
    "",
    {0,
     BIT(19) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_OPEN_RECYCLE_LAST_NODE_ID,
    "open_last_node",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure adds one last node to the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_last_node 0 tree_id 1221 queues 125 8191 \r\n"
    "Results in setting on device 0 for tree id 1221 to be:\n\r"
    "Last node, which point to 2 queues: 125 and 8191.\r\n",
    "",
    {0,
     BIT(18) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUES_ID_ID,
    "queues",
    (PARAM_VAL_RULES *)&Fap10m_recycle_multicast_queues[0],
    (sizeof(Fap10m_recycle_multicast_queues) / sizeof(Fap10m_recycle_multicast_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "1 or 2 queues identifiers.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_last_node 0 tree_id 1221 queues 125 8191 \r\n"
    "Results in setting on device 0 for tree id 1221 to be:\n\r"
    "Last node, which point to 2 queues: 125 and 8191.\r\n",
    "",
    {0,
     BIT(18) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_OPEN_RECYCLE_INTERNAL_NODE_ID,
    "open_internal_node",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure adds one last node to the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to nect tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TREE_ID_ID,
    "tree_id",
    (PARAM_VAL_RULES *)&Fap10m_recycle_multicast_tree_id[0],
    (sizeof(Fap10m_recycle_multicast_tree_id) / sizeof(Fap10m_recycle_multicast_tree_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Tree id in recycle tree.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to nect tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) | BIT(18) | BIT(19)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NEXT_TREE_ID_ID,
    "next_tree_id",
    (PARAM_VAL_RULES *)&Fap10m_recycle_multicast_tree_id[0],
    (sizeof(Fap10m_recycle_multicast_tree_id) / sizeof(Fap10m_recycle_multicast_tree_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Next level tree id.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_indices 100 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_RECYCLE_Q_INDEX_0_ID,
    "recycle_q_index_0",
    (PARAM_VAL_RULES *)&Fap10m_recycle_queue_index[0],
    (sizeof(Fap10m_recycle_queue_index) / sizeof(Fap10m_recycle_queue_index[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "index to \'Multicast Recycling-Queue\' table.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_RECYCLE_Q_INDEX_1_ID,
    "recycle_q_index_1",
    (PARAM_VAL_RULES *)&Fap10m_recycle_queue_index[0],
    (sizeof(Fap10m_recycle_queue_index) / sizeof(Fap10m_recycle_queue_index[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "index to \'Multicast Recycling-Queue\' table.\r\n",
    "Examples:\r\n"
    "  fap10m multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap10m_api_recycle_multicast.h interface }*/

  /* { fap10m_api_ingress_queue.h interface */
  {
    PARAM_FAP10M_INGRESS_QUEUE_API_ID,
    "in_queue",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver ingress queue module services.",
    "Examples:\r\n"
    "  fap10m in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
    "",
    {0,
     BIT(13) | BIT(14) | BIT(15) | BIT(16),
     0,
     BIT(25)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_QUEUE_SIZE_ID,
    "get_queue_size",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get from the device the ingress FAP queue byte size\r\n",
    "Examples:\r\n"
    "  fap10m in_queue get_queue_size 0 queue_id 11\r\n"
    "Results in getting on device 0 queue 11 byte size.\r\n",
    "",
    {0,
     BIT(16) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_NON_EMPTY_QUEUES_ID,
    "get_non_empty_queues",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get from the device the ingress FAP non empty queues\r\n",
    "Examples:\r\n"
    "  fap10m in_queue get_non_empty_queues 0\r\n"
    "Results in getting on device 0 non-empty queues. 8K queues are getting scanned\r\n",
    "",
    {0,
     0,
     0,
     BIT(25) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP10M_GET_QUEUE_PARAMS_ID,
    "get_queue_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get from the device the ingress FAP queue configured parameters\r\n",
    "Examples:\r\n"
    "  fap10m in_queue get_queue_params 0 queue_id 11\r\n"
    "Results in getting on device 0 queue 11 parameters.\r\n",
    "",
    {0,
     BIT(16) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CLOSE_QUEUE_ID,
    "close_queue",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Close an ingress FAP queue.\r\n"
    "Note the device is set with some default parameters.\n\r"
    "These parameters do NOT cause the device not to send packets.\n\r",
    "Examples:\r\n"
    "  fap10m in_queue close_queue 0 queue_id 11 \r\n"
    "Results in closing on device 0 queue 11.\r\n",
    "",
    {0,
     BIT(16) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_QUEUE_ID,
    "update_queue",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Update an ingress FAP queue parameters.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue update_queue 0 queue_id 11 target_fap 90 target_flow 1255 queue_type 0\r\n"
    "Results in updating on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_OPEN_QUEUE_ID,
    "open_queue",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Open at the device an ingress FAP queue to a target FAP scheduler flow.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue open_queue 0 queue_id 11 target_fap 90 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_ID_ID,
    "queue_id",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue identifier.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue open_queue 0 queue_id 11 target_fap 90 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) | BIT(16)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FAP_ID_ID,
    "target_fap",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The target FAP for the queue.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue open_queue 0 queue_id 11 target_fap 90 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FLOW_ID_ID,
    "target_flow",
    (PARAM_VAL_RULES *)&Fap10m_flow_id[0],
    (sizeof(Fap10m_flow_id) / sizeof(Fap10m_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow, in egress scheduler, assigned for that queue.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue open_queue 0 queue_id 11 target_fap 90 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_TYPE_ID,
    "queue_type",
    (PARAM_VAL_RULES *)&Fap10m_queue_type[0],
    (sizeof(Fap10m_queue_type) / sizeof(Fap10m_queue_type[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The queue type.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue open_queue 0 queue_id 11 target_fap 90 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_QUEUE_LABELING_ID,
    "get_queue_labeling",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get the labeling parameters of packets to target device.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue get_queue_labeling 0 target_fap 10 \r\n"
    "Results in getting on device 0 the labeling parameters for \n\r"
    "packets destined to fap 10.\r\n",
    "",
    {0,
     BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_QUEUE_LABELING_ID,
    "set_queue_labeling",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the labeling type of packets destined to target device.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
    "",
    {0,
     BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_TARGET_FAP_ID_ID,
    "target_fap",
    (PARAM_VAL_RULES *)&Fap10m_fap_id[0],
    (sizeof(Fap10m_fap_id) / sizeof(Fap10m_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target FAP identifier.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
    "",
    {0,
     BIT(13) | BIT(14)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_BASE_QUEUE_ID_ID,
    "base_queue",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Base queue identifier to use with the target FAP.\r\n",
    "Examples:\r\n"
    "  fap10m in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
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
    PARAM_FAP10M_QUEUE_LABEL_TYPE_ID,
    "label_type",
    (PARAM_VAL_RULES *)&Fap10m_queue_base_label[0],
    (sizeof(Fap10m_queue_base_label) / sizeof(Fap10m_queue_base_label[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_queue_label_type_default,
    0,0,0,0,0,0,0,
    "Which labeling to use: flow mapping or mapping .\r\n",
    "Examples:\r\n"
    "  fap10m in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
    "",
    {0,
     BIT(13)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap10m_api_ingress_queue.h interface }*/
  /* { fap10m_api_queue_id_allocator.h interface */
  {
    PARAM_FAP10M_ALLOCATOR_API_ID,
    "allocator",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver ingress-queue-id/scheduler-flow-id allocator\r\n"
    "  module services. This module is a management aid of queue identifiers.\r\n"
    "  All of this services DO NOT read/write the device but rather\r\n"
    "  effect driver private structure.",
    "Examples:\r\n"
    "  fap10m allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10) | BIT(11) | BIT(12)| BIT(27) | BIT(7) | BIT(8) | BIT(9) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_ID_ALLOCATOR_API_ID,
    "queue",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver ingress queue id allocator module services.\r\n"
    "  This module is a management aid of queue identifiers. All of this\r\n"
    "  services DO NOT read/write the device but rather effect driver\r\n"
    "  private structure.",
    "Examples:\r\n"
    "  fap10m allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10) | BIT(11) | BIT(12)| BIT(27) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PRINT_ALLOC_QUEUE_ID,
    "display_allocator",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display the driver queue-id allocator.\r\n",
    "Examples:\r\n"
    "  fap10m allocator queue display_allocator 1\r\n"
    "Results in displaying, on device 1, queue identifier allocator\r\n",
    "",
    {0,
     BIT(27)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_QUEUE_ID_RANGE_ID,
    "set_range",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the range, out of [0-8191], to search queue identifier from.\r\n",
    "Examples:\r\n"
    "  fap10m allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MIN_QUEUE_ID_ID,
    "min",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Minimum queue id to search from.\r\n",
    "Examples:\r\n"
    "  fap10m allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_QUEUE_ID_ID,
    "max",
    (PARAM_VAL_RULES *)&Fap10m_queue_id[0],
    (sizeof(Fap10m_queue_id) / sizeof(Fap10m_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Maximal queue id to search from (this number id is included in the search).\r\n",
    "Examples:\r\n"
    "  fap10m allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_ALLOC_QUEUE_ID_BLOCK_ID,
    "alloc",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get free queue identifier within the specified range.\r\n"
    "  Mark that queues identifiers as 'taken'.\r\n",
    "Examples:\r\n"
    "  fap10m allocator queue alloc 1\r\n"
    "Results in allocating on device 1, block of consecutive queue idetifiers.\r\n"
    "The base queue is given back. The block size is taken from the device class.\n\r"
    "class_a: 1 queue block, class_b: 2, class_c: 4 and class_a: 8 queues block.\n\r",
    "",
    {0,
     BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FREE_QUEUE_ID_ID,
    "free",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Free the queue identifiers block of identifiers.\r\n",
    "Examples:\r\n"
    "  fap10m allocator queue free 0 base_queue 11\r\n"
    "Results in freeing on device 0, block of consecutive queue idetifiers,\r\n"
    "starting from base queue 11. The block size is taken from the device class.\n\r"
    "class_a: 1 queue block, class_b: 2, class_c: 4 and class_a: 8 queues block.\n\r",
    "",
    {0,
     BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_QUEUE_BASE_ID_ID,
    "base_queue",
    (PARAM_VAL_RULES *)&Fap10m_flow_id[0],
    (sizeof(Fap10m_flow_id) / sizeof(Fap10m_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "queue base identifier to free.\r\n",
    "Examples:\r\n"
    "  fap10m allocator queue free 0 base_queue 11\r\n"
    "Results in freeing on device 0, block of consecutive queue idetifiers,\r\n"
    "starting from base queue 11. The block size is taken from the device class.\n\r"
    "class_a: 1 queue block, class_b: 2, class_c: 4 and class_a: 8 queues block.\n\r",
    "",
    {0,
     BIT(12)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END fap10m_api_queue_id_allocator.h interface }*/
  /* { fap10m_api_flow_id_allocator.h interface */
  {
    PARAM_FAP10M_FLOW_ID_ALLOCATOR_API_ID,
    "flow",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver scheduler-flow/queue id allocator module services.\r\n"
    "  This module is a management aid of flow identifiers. All of this\r\n"
    "  services DO NOT read/write the device but rather effect driver\r\n"
    "  private structure.",
    "Examples:\r\n"
    "  fap10m allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7) | BIT(8) | BIT(9) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_PRINT_ALLOC_FLOW_ID,
    "display_allocator",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display the driver flow-id allocator.\r\n",
    "Examples:\r\n"
    "  fap10m allocator flow display_allocator 1\r\n"
    "Results in displaying, on device 1, flow identifier allocator\r\n",
    "",
    {0,
     BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_SCHEDULER_FLOW_ID_RANGE_ID,
    "set_range",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the range, out of [0-8191], to search flow identifier from.\r\n",
    "Examples:\r\n"
    "  fap10m allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MIN_FLOW_ID_ID,
    "min",
    (PARAM_VAL_RULES *)&Fap10m_flow_id[0],
    (sizeof(Fap10m_flow_id) / sizeof(Fap10m_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Minimum flow id to search from.\r\n",
    "Examples:\r\n"
    "  fap10m allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_FLOW_ID_ID,
    "max",
    (PARAM_VAL_RULES *)&Fap10m_flow_id[0],
    (sizeof(Fap10m_flow_id) / sizeof(Fap10m_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Maximal flow id to search from (this number id is.\r\n"
    "  included in the search).\r\n",
    "Examples:\r\n"
    "  fap10m allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_ALLOC_SCHEDULER_FLOW_ID_ID,
    "alloc",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get free flow identifier within the specified range.\r\n"
    "  Mark that flows identifiers as 'taken'\r\n",
    "Examples:\r\n"
    "  fap10m allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FREE_SCHEDULER_FLOW_ID_ID,
    "free",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Free the flow identifier id.\r\n",
    "Examples:\r\n"
    "  fap10m allocator flow free 0 flow_id 11\r\n"
   "Results in setting on device 0, that flow id 11 is free for allocation again.\r\n",
    "",
    {0,
     BIT(9)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_FLOW_ID_ID,
    "flow_id",
    (PARAM_VAL_RULES *)&Fap10m_flow_id[0],
    (sizeof(Fap10m_flow_id) / sizeof(Fap10m_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "flow identifier to free.\r\n",
    "Examples:\r\n"
    "  fap10m allocator flow free 0 flow_id 11\r\n"
    "Results in setting on device 0, that flow id 11 is free for allocation again.\r\n",
    "",
    {0,
     BIT(9)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap10m_api_flow_id_allocator.h interface }*/
  /* { fap10m_api_scheduler_ports.h interface */
  {
    PARAM_FAP10M_SCHEDULER_API_ID,
    "scheduler",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver scheduler ports/flows services.",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(0) | BIT(1) | BIT(2) | BIT(24) | BIT(25) | BIT(26) | BIT(28) | BIT(29),
     0,
     BIT(0) | BIT(1),
     BIT(0)
    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SCHEDULER_PORT_API_ID,
    "port",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver scheduler ports services.",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(0) | BIT(1) | BIT(2) | BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CLOSE_SCHEDULER_PORT_ID,
    "close_scheduler_port",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "close the scheduler port on the device.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port close_scheduler_port 0x1 port_id 10\r\n"
    "Results in closing on device 1 scheduler port 10.\r\n",
    "",
    {0,
     BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_SCHEDULER_PORT_ID,
    "get_scheduler_port",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port get_scheduler_port 0x1 port_id 10\r\n"
    "Results in getting on device 1 scheduler port 10 parameters.\r\n",
    "",
    {0,
     BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_SCHEDULER_RECYCLE_PORT_ID,
    "set_scheduler_recycle_port",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_recycle_port 0x1 credit_bandwidth 100\r\n"
    "Results in setting on device 1 scheduler port 64 (recycle) to:\r\n"
    " 100 MegbitSec credit.\r\n",
    "",
    {0,
     BIT(28)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_SCHEDULER_PORT_ID,
    "set_scheduler_port",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
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
    PARAM_FAP10M_SCHEDULER_PORT_ID_ID,
    "port_id",
    (PARAM_VAL_RULES *)&Fap10m_scheduler_port_id_vals[0],
    (sizeof(Fap10m_scheduler_port_id_vals) / sizeof(Fap10m_scheduler_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add. Range: 0-64.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(1) | BIT(2)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SCHEDULER_PORT_ID_ID,
    "port_id",
    (PARAM_VAL_RULES *)&Fap10m_scheduler_port_id_no_recyle_vals[0],
    (sizeof(Fap10m_scheduler_port_id_no_recyle_vals) / sizeof(Fap10m_scheduler_port_id_no_recyle_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add. Range: 0-64.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(0)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CREDIT_BANDWIDTH_ID,
    "credit_bandwidth",
    (PARAM_VAL_RULES *)&Fap10m_credit_bandwidth_vals[0],
    (sizeof(Fap10m_credit_bandwidth_vals) / sizeof(Fap10m_credit_bandwidth_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Bandwidth which the port is to generate credit to (Megabits-Second).\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(0) | BIT(28) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_NOMINAL_BANDWIDTH_ID,
    "nominal_bandwidth",
    (PARAM_VAL_RULES *)&Fap10m_nominal_bandwidth_vals[0],
    (sizeof(Fap10m_nominal_bandwidth_vals) / sizeof(Fap10m_nominal_bandwidth_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The physical bandwidth the port has (Megabits-Second).\r\n"
    "   Not meaningful in recycling port.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(0)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_WFQ0_WEIGHT_ID,
    "wfq0_weight",
    (PARAM_VAL_RULES *)&Fap10m_percent[0],
    (sizeof(Fap10m_percent) / sizeof(Fap10m_percent[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap10m_wfq0_weight_default,
    0,0,0,0,0,0,0,
    "The weight of the WFQ0 wheel - EDF-WFQ Arbiter.\r\n"
    "   1 : mean  1% for WFQ1 and 99% EDF1\r\n"
    "   20: mean 20% for WFQ1 and 80% EDF1\r\n"
    "   Not meaningful in recycling port.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_END_PORT_ID_ID,
    "end_port_id",
    (PARAM_VAL_RULES *)&Fap10m_scheduler_port_id_no_recyle_vals[0],
    (sizeof(Fap10m_scheduler_port_id_no_recyle_vals) / sizeof(Fap10m_scheduler_port_id_no_recyle_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, initiailze from 'port_id' to 'end_port_id' with the same parameters.\r\n"
    "  This command is usefull when all ports have the same configuration.",
    "Examples:\r\n"
    "  fap10m scheduler port set_scheduler_port 0x1 port_id 0 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2 end_port_id 11\r\n"
    "Results in setting on device 1 scheduler ports 0-11 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,
     BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END fap10m_api_scheduler_ports.h interface } */
  /* { fap10m_api_spatial_multicast.h interface */
  {
    PARAM_FAP10M_MULTICAST_API_ID,
    "multicast",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver spatial multicast services.",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf 0x1 max_bandwidth 100 max_sm_percent 2\r\n"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial\r\n"
    "multicast packets, and with 2% maximal Spatial multicast traffic percentage\r\n"
    "from rate assigned to all ingress queues.\r\n",
    "",
    {0,
     BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(17) | BIT(18) | BIT(19) | BIT(20) | BIT(21),
     BIT(20),
     BIT(8) | BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_MULTICAST_API_ID,
    "spatial",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap10m driver spatial multicast services.",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf 0x1 max_bandwidth 100 max_sm_percent 2\r\n"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial\r\n"
    "multicast packets, and with 2% maximal Spatial multicast traffic percentage\r\n"
    "from rate assigned to all ingress queues.\r\n",
    "",
    {0,
     BIT(3) | BIT(4) | BIT(5) | BIT(6),
     BIT(20),
     BIT(8) | BIT(6) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_SET_IGNORE_BIT7_ID,
    "set_ignore_bit7",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "set ignore_bit7 flag.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial set_ignore_bit7 0x1 flag_val 0 \r\n"
    "Results in setting the IGNORE_BIT7 flag",
    "",
    {0,
     0,
     BIT(20)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_IGNORE_BIT7_FLAG_VAL_ID,
    "flag_val",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "set ignore_bit7 flag.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial set_ignore_bit7 0x1 flag_val 0 \r\n"
    "Results in setting the IGNORE_BIT7 flag",
    "",
    {0,
     0,
     BIT(20)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_CLOSE_SPATIAL_MULTICAST_FLOW_ID,
    "close_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "close spatial multicast flow at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial close_flow 0x1 multicast_group_id 1500 \r\n"
    "Results in closing on device 1 spatial multicast group 1500.",
    "",
    {0,
     BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_SPATIAL_MULTICAST_FLOW_PARAMS_ID,
    "get_flow_params",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "get spatial multicast flow parameters at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial get_flow_params 0x1 multicast_group_id 1500 \r\n"
    "Results in getting on device 1 spatial multicast group parameters.",
    "",
    {0,
     BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_OPEN_SPATIAL_MULTICAST_FLOW_ID,
    "open_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "open spatial multicast flow at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial open_flow 0x1 multicast_group_id 1500 spatial_queue 2\r\n"
    "Results in opening on device 1 spatial multicast flow with parameters:\r\n"
    "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_UPDATE_SPATIAL_MULTICAST_FLOW_ID,
    "update_flow",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "update spatial multicast flow at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial update_flow 0x1 multicast_group_id 1500 spatial_queue 2\r\n"
    "Results in updating on device 1 spatial multicast flow to parameters:\r\n"
    "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_MULTICAST_NUM_ID,
    "multicast_group_id",
    (PARAM_VAL_RULES *)&Fap10m_spatial_multicast_id[0],
    (sizeof(Fap10m_spatial_multicast_id) / sizeof(Fap10m_spatial_multicast_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "spatial multicast group identifier.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial open_flow 0x1 multicast_group_id 1500 spatial_queue 2\r\n"
     "Results in opening on device 1 spatial multicast flow with parameters:\r\n"
     "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5) | BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_TYPE_ID,
    "spatial_queue",
    (PARAM_VAL_RULES *)&Fap10m_spatial_multicast_queue[0],
    (sizeof(Fap10m_spatial_multicast_queue) / sizeof(Fap10m_spatial_multicast_queue[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Spatial multicast queue. One of 4 spatial queues, to store flow packets.\n\r"
    "  Packet destinate to queue number 3 have absolute priority for credit.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial open_sm_flow 0x1 multicast_group_id 1500 spatial_queue 2\r\n"
    "Results in opening on device 1 spatial multicast flow with parameters:\r\n"
    "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SM_USE_MAP_CLS_ID,
    "use_preset_class",
    (PARAM_VAL_RULES *)&Fap10m_empty_vals[0],
    (sizeof(Fap10m_empty_vals) / sizeof(Fap10m_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Spatial multicast Use Preset Class. When on, the spatial queue\n\r"
    "  would be determined by the queue number. the class in the packet\n\r"
    "  descriptor is ignored.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial open_sm_flow 0x1 multicast_group_id 1500 spatial_queue 2 use_preset_class\r\n"
    "Results in opening on device 1 spatial multicast flow with parameters:\r\n"
    "spatial multicast identifier 1500 and queue 2.\n\r"
    "All the packets with multicast ID 1500 would be inserted to queue 2.",
    "",
    {0,
     BIT(5)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_GET_SMS_CONFIGURATION_ID,
    "get_sms_conf",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets Spatial Multicast Scheduler parameters from the device.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial get_sms_conf 0x1 \r\n"
    "Results in getting on device 1 spatial multicast scheduler parameters.\r\n",
    "",
    {0,
     BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SET_SMS_CONFIGURATION_ID,
    "set_sms_conf_rev_a",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets Spatial Multicast Scheduler parameters at the device.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf_rev_a 0x1 max_bandwidth 100 max_sm_percent 2\r\n"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial\r\n"
    "multicast packets, and with 2% maximal Spatial multicast traffic percentage\r\n"
    "from rate assigned to all ingress queues.\r\n",
    "",
    {0,
     BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_BANDWIDTH_ID,
    "max_bandwidth",
    (PARAM_VAL_RULES *)&Fap10m_sms_max_bandwidth[0],
    (sizeof(Fap10m_sms_max_bandwidth) / sizeof(Fap10m_sms_max_bandwidth[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max bandwidth parameter in the SMS parameters.\r\n"
    "  Resolution of MegbitSec.\n\r",
    "  fap10m scheduler port set_sms_conf_rev_a 0x1 max_bandwidth 100 max_sm_percent 2\r\n"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial\r\n"
    "multicast packets, and with 2% maximal Spatial multicast traffic percentage\r\n"
    "from rate assigned to all ingress queues.\r\n",
    "",
    {0,
     BIT(3)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SM_PERCENT_ID,
    "max_sm_percent",
    (PARAM_VAL_RULES *)&Fap10m_percent[0],
    (sizeof(Fap10m_percent) / sizeof(Fap10m_percent[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max bandwidth parameter in the SMS parameters.\r\n",
    "Examples:\r\n"
    "  fap10m scheduler port set_sms_conf_rev_a 0x1 max_bandwidth 100 max_sm_percent 2\r\n"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial\r\n"
    "multicast packets, and with 2% maximal Spatial multicast traffic percentage\r\n"
    "from rate assigned to all ingress queues.\r\n",
    "",
    {0,
     BIT(3)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_B_SET_SMS_CONFIGURATION_ID,
    "set_sms_conf_rev_b",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets Spatial Multicast Scheduler parameters at the device.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf_rev_b 0x1 guaranteed_rate 50\r\n"
    "    guaranteed_burst_size 200 best_effort_rate 100 best_effort_burst_size 70\n\r"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "50 MegbitSec credit - Maximal bandwidth to assign to spatial queue no. 3.\r\n"
    "spatial queue no.3 has higher priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is guaranteed.\n\r"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial queues no. 0-2.\r\n"
    "spatial queues no.0-2 has lower priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is only best effort.\n\r"
    "200 Kilobytes credit can be accumulate to queue 3\r\n"
    "70 Kilobytes credit can be accumulate to queues 0-2.\r\n",
    "",
    {0,0,0, BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_GR_BANDWIDTH_ID,
    "guaranteed_rate",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max bandwidth for spatial queue no.3 parameter in the SMS parameters.\r\n"
    "  Resolution of MegbitSec.\n\r",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf_rev_b 0x1 guaranteed_rate 50\r\n"
    "    guaranteed_burst_size 200 best_effort_rate 100 best_effort_burst_size 70\n\r"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "50 MegbitSec credit - Maximal bandwidth to assign to spatial queue no. 3.\r\n"
    "spatial queue no.3 has higher priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is guaranteed.\n\r"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial queues no. 0-2.\r\n"
    "spatial queues no.0-2 has lower priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is only best effort.\n\r"
    "200 Kilobytes credit can be accumulate to queue 3\r\n"
    "70 Kilobytes credit can be accumulate to queues 0-2.\r\n",
    "",
    {0,0,0, BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_BE_BANDWIDTH_ID,
    "best_effort_rate",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max bandwidth for spatial queues no.0-2 parameter in the SMS parameters.\r\n"
    "  Resolution of MegbitSec.\n\r",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf_rev_b 0x1 guaranteed_rate 50\r\n"
    "    guaranteed_burst_size 200 best_effort_rate 100 best_effort_burst_size 70\n\r"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "50 MegbitSec credit - Maximal bandwidth to assign to spatial queue no. 3.\r\n"
    "spatial queue no.3 has higher priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is guaranteed.\n\r"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial queues no. 0-2.\r\n"
    "spatial queues no.0-2 has lower priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is only best effort.\n\r"
    "200 Kilobytes credit can be accumulate to queue 3\r\n"
    "70 Kilobytes credit can be accumulate to queues 0-2.\r\n",
    "",
    {0,0,0, BIT(6)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_GR_BURST_ID,
    "guaranteed_burst_size",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max burst size for spatial queue no.3 parameter in the SMS parameters.\r\n"
    "  Resolution of Kilobytes.\n\r",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf_rev_b 0x1 guaranteed_rate 50\r\n"
    "    guaranteed_burst_size 200 best_effort_rate 100 best_effort_burst_size 70\n\r"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "50 MegbitSec credit - Maximal bandwidth to assign to spatial queue no. 3.\r\n"
    "spatial queue no.3 has higher priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is guaranteed.\n\r"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial queues no. 0-2.\r\n"
    "spatial queues no.0-2 has lower priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is only best effort.\n\r"
    "200 Kilobytes credit can be accumulate to queue 3\r\n"
    "70 Kilobytes credit can be accumulate to queues 0-2.\r\n",
    "",
    {0,0,0, BIT(6)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_MAX_BE_BURST_ID,
    "best_effort_burst_size",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max burst size for spatial queue no.3 parameter in the SMS parameters.\r\n"
    "  Resolution of Kilobytes.\n\r",
    "Examples:\r\n"
    "  fap10m multicast spatial set_sms_conf_rev_b 0x1 guaranteed_rate 50\r\n"
    "    guaranteed_burst_size 200 best_effort_rate 100 best_effort_burst_size 70\n\r"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "50 MegbitSec credit - Maximal bandwidth to assign to spatial queue no. 3.\r\n"
    "spatial queue no.3 has higher priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is guaranteed.\n\r"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial queues no. 0-2.\r\n"
    "spatial queues no.0-2 has lower priority than the unicast traffic and therefore\n\r"
    "the traffic assigned to that queue is only best effort.\n\r"
    "200 Kilobytes credit can be accumulate to queue 3\r\n"
    "70 Kilobytes credit can be accumulate to queues 0-2.\r\n",
    "",
    {0,0,0, BIT(6)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_ACTIVE_LINKS_GET_ID,
    "get_active_links",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets spatial multicast active links from the device.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial get_active_links 0x1 \r\n"
    "Results in getting on device 1 spatial multicast active links.\r\n",
    "",
    {0,
     BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_ACTIVE_LINKS_SET_ID,
    "set_active_links",
    (PARAM_VAL_RULES *)&Fap10m_indefinite_num_vals[0],
    (sizeof(Fap10m_indefinite_num_vals) / sizeof(Fap10m_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets spatial multicast active links from to device.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial set_active_links 0x1 0x14F\r\n"
    "Results in setting on device 1 spatial multicast active links\r\n"
    "to be 0,1,2,3,6,8.\r\n",
    "",
    {0,
     0,
     0,
     BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP10M_SPATIAL_ACTIVE_LINKS_ID,
    "links_bitmap",
    (PARAM_VAL_RULES *)&Fap10m_sp_active_links[0],
    (sizeof(Fap10m_sp_active_links) / sizeof(Fap10m_sp_active_links[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets spatial multicast active links to the device.\r\n",
    "Examples:\r\n"
    "  fap10m multicast spatial set_active_links 0x1 0x14F\r\n"
    "Results in setting on device 1 spatial multicast active links\r\n"
    "to be 0,1,2,3,6,8.\r\n",
    "",
    {0,
     0,
     0,
     BIT(8)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap10m_api_spatial_multicast.h interface } */
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

/*
 * { Definitions for ChipSim
 */
/********************************************************
*NAME
*  Fap10m_all_regs_group_str
*TYPE: BUFFER
*DATE: 07/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  all SOC_SAND_FAP10M registers.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fap10m_all_regs_group_str[]
#ifdef INIT
   =
{
  {
    sizeof(FAP10M_REGS)/4,0,
    0x0

  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Desc_fap10m_reg
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING MEMORY MAP OF
*  REGISTERS WITHIN THE SOC_SAND_FAP10M CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'DESC_FE_REG':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  DESC_FE_REG
    Desc_fap10m_reg[]
#ifdef INIT
   =
{
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS, eci_int_regs.detail.interrupt[0]),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS, eci_int_regs.detail.interrupt[0]",
    "0:31 - interrupt bits - one per source\n\r"
    "",
    "",
    "",
    "",
    "",
    "ECI-INT - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS, eci_int_regs.detail.interrupt[1]),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS, eci_int_regs.detail.interrupt[1]",
    "0:31 - interrupt bits - one per source\n\r"
    "",
    "",
    "",
    "",
    "",
    "ECI-INT - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS, eci_int_regs.detail.interrupt[2]),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS, eci_int_regs.detail.interrupt[2]",
    "0:31 - interrupt bits - one per source\n\r"
    "",
    "",
    "",
    "",
    "",
    "ECI-INT - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS, fdr_status_regs.detail.overflow_and_fifo_statuses),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS, fdr_status_regs.detail.overflow_and_fifo_statuses",
    "Various statuses\n\r"
    "bit 3 - CPUDataCellFNE",
    "",
    "",
    "",
    "",
    "FDR - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS, fdt_status_regs.detail.unreachable_destination),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS, fdt_status_regs.detail.unreachable_destination",
    "0:12 - unreachable information\n\r"
    "13 - the interrupt source",
    "",
    "",
    "",
    "",
    "FDT - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS, mac9_status_regs.detail.ber_counters),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS, mac9_status_regs.detail.ber_counters",
    "0:31-bit counter holds the PRBS errors detected by\n\r"
    "the PRBS checker.\n\r"
    "31-bit BERvlid",
    "",
    "",
    "",
    "",
    "MAC9 - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS,qdp_status_regs.detail.add_pkt_cnt),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS,qdp_status_regs.detail.add_pkt_cnt",
    "",
    "",
    "",
    "",
    "",
    "QDP - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS,qdp_status_regs.detail.head_del_pkt_cnt),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS,qdp_status_regs.detail.head_del_pkt_cnt",
    "",
    "",
    "",
    "",
    "",
    "QDP - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS,qdp_status_regs.detail.tail_del_pkt_cnt),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS,qdp_status_regs.detail.tail_del_pkt_cnt",
    "",
    "",
    "",
    "",
    "",
    "QDP - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS,dme_status_regs.detail.err_pkt_cnt),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS,dme_status_regs.detail.err_pkt_cnt",
    "",
    "",
    "",
    "",
    "",
    "DME - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    SOC_SAND_OFFSETOF(FAP10M_REGS,dme_status_regs.detail.good_pkt_cnt),
    0xFFFFFFFF,
    0,
    "FAP10M_REGS,dme_status_regs.detail.good_pkt_cnt",
    "",
    "",
    "",
    "",
    "",
    "DME - Status",
    "",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)NULL,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    0x0,
    0x0,
    0x0,
    0x0,
    0xFFFFFFFF,
    0x0,
    0x0,
    "ALL SOC_SAND_FAP10M registers",
    "ALL SOC_SAND_FAP10M registers",
    "ALL SOC_SAND_FAP10M registers",
    "ALL SOC_SAND_FAP10M registers",
    "ALL SOC_SAND_FAP10M registers",
    "ALL SOC_SAND_FAP10M registers",
    "ALL SOC_SAND_FAP10M registers",
    "ALL SOC_SAND_FAP10M registers",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fap10m_all_regs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
/*
 * Last element. Do not remove.
 */
  {
    FE_LAST_FIELD
  }
}
#endif
;
/*
 * }
 */

/* } */
#endif
