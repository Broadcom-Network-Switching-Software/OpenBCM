/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_SWEEP_APP_INCLUDED
/* { */
#define UI_ROM_DEFI_SWEEP_APP_INCLUDED


#include <ui_pure_defi_sweep_app.h>
#include <appl/dpp/UserInterface/ui_cli_files.h>

#include <sweep/Fap20v/sweep_fap20v_app.h>
#include <sweep/Fap20v/sweep_fap20v_qos.h>

#include <CSystem/csystem_consts.h>

#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_scheduler_flows.h>


#define UI_DEMY_VAL 0

/********************************************************
*NAME
*  Sweep_app_true_false_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_app_true_false_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "true",
    {
      {
        1,
        (long)"    TRUE.",
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
        (long)"    FALSE.",
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
*  Sweep_app_true_default
 */
EXTERN CONST
   PARAM_VAL
     Sweep_app_true_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  1,
  {
    (unsigned long)"    TRUE."
  }
}
#endif
;

/********************************************************
*NAME
*  Sweep_app_false_default
 */
EXTERN CONST
   PARAM_VAL
     Sweep_app_false_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  0,
  {
    (unsigned long)"    FALSE."
  }
}
#endif
;

/********************************************************
*NAME
*  Sweep_app_on_off_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_app_on_off_vals[]
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
*  Sweep_app_true_default
 */
EXTERN CONST
   PARAM_VAL
     Sweep_app_on_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  1,
  {
    (unsigned long)"ON."
  }
}
#endif
;

/********************************************************
*NAME
*  Sweep_app_false_default
 */
EXTERN CONST
   PARAM_VAL
     Sweep_app_off_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  0,
  {
    (unsigned long)"OFF."
  }
}
#endif
;


/********************************************************
*NAME
*  Sweep_ZERO_default
 */
EXTERN CONST
   PARAM_VAL
     Sweep_ZERO_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    CSYS_ALL_THE_DEVICES
  }
}
#endif
;


/********************************************************
*NAME
*  Sweep_app_empty_vals
*TYPE: BUFFER
*DATE: 15/JUN/2004
*FUNCTION:
*
*  Empty Array containing NO values for commands/parameters with no values
*   in the fap20v sweep application.
*
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
     Sweep_app_empty_vals[]
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
*  Sweep_indefinite_num_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_indefinite_num_vals[]
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
*  Sweep_app_fap_no_vals
*TYPE: BUFFER
*DATE: 15/JUN/2004
*FUNCTION:
*
*  Array containing the optional values of 'source/destination_fap' in 'get/set_qos_params' command
*   in the fap20v sweep application.
*
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
     Sweep_app_fap_no_vals[]
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
*  Sweep_app_fe_no_vals
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
     Sweep_app_fe_no_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        15,0,
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
*  Sweep_app_serdes_no_vals
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
     Sweep_app_serdes_no_vals[]
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
*  Sweep_app_port_no_vals
*TYPE: BUFFER
*DATE: 15/JUN/2004
*FUNCTION:
*
*  Array containing the optional values of 'destination_port' in 'get/set_qos_params' command
*   in the fap20v sweep application.
*
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
     Sweep_app_port_no_vals[]
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
*  Sweep_app_class_no_vals
*TYPE: BUFFER
*DATE: 15/JUN/2004
*FUNCTION:
*
*  Array containing the optional values of 'class' in 'get/set_qos_params' command
*   in the fap20v sweep application.
*
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
     Sweep_app_class_no_vals[]
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
*  Sweep_app_sub_flow_types
*TYPE: BUFFER
*DATE: 15/JUN/2004
*FUNCTION:
*
*  Array containing the optional values of 'sub_flow_type' in 'set_qos_params' command
*   in the fap20v sweep application.
*
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
     Sweep_app_sub_flow_types[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "edf0",
    {
      {
        FAP20V_EDF0_FLOW,
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
        FAP20V_EDF1_FLOW,
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
        FAP20V_WFQ1_FLOW,
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
*  Sweep_app_sub_flow_values
*TYPE: BUFFER
*DATE: 15/JUN/2004
*FUNCTION:
*
*  Array containing the optional values of 'sub_flow_value' in 'set_qos_params' command
*   in the fap20v sweep application.
*
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
     Sweep_app_sub_flow_values[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        16000000,0,
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



EXTERN CONST
   PARAM_VAL_RULES
     Sweep_app_download_host_ip[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
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
     Sweep_app_file_name_vals[]
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
        CLI_FILE_NAME_LEN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Name of file on downloading host.",
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
*  Sweep_app_on_msg_task_interval_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_app_on_msg_task_interval_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000000000,0,
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
*  Sweep_app_params
 */
EXTERN CONST
   PARAM
     Sweep_app_params[]
#ifdef INIT
   =
{
/*
 ******
 * ENTRY -- 0
 *SOC_SAND_BIT(0) -
 *SOC_SAND_BIT(1) - get_connectivity_map
 *SOC_SAND_BIT(2) - power_down_serdes/power_up_serdes
 *SOC_SAND_BIT(3) - get_qos_params
 *SOC_SAND_BIT(4) - set_qos_params
 *SOC_SAND_BIT(5) - graceful_shutdown_fe
 *SOC_SAND_BIT(6) - queue_on_msg_task_interval (set/get)
 *SOC_SAND_BIT(7) - aggr_on_msg_task_interval (set/get)
 ******
 * ENTRY -- 1
 *SOC_SAND_BIT() - .
*/
  {
    PARAM_SWEEP_APP_GRACEFUL_SHUTDOWN_FE_ID,
    "graceful_shutdown_fe",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gracefully shuts down a single FE (by stopping reachability messages).",
    "Examples:\r\n"
    "  sweep graceful_shutdown_fe fe# 2\r\n"
    "Results in shutting down the second FE on the first fabric card\n\r"
    " (the lower card).",
    "",
    { SOC_SAND_BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_GRACEFUL_RESTORE_FE_ID,
    "graceful_restore_fe",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "restores a single registered FE (setting out of reset + restarting reachability messages).",
    "Examples:\r\n"
    "  sweep graceful_restore_fe fe# 2\r\n"
    "Results in restoring the second FE on the first fabric card\n\r"
    " (the lower card).",
    "",
    { SOC_SAND_BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the connectivity map of one of the FAPs.",
    "Examples:\r\n"
    "  sweep get_connectivity_map fap# 0\r\n"
    "Results in printing the connectivity map of the fap at the line card\n\r"
    " in the first slot (the upper card).",
    "",
    { SOC_SAND_BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_POWER_DOWN_SERDES_ID,
    "power_down_serdes",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Powers down a single serdes of one of the FEs / FAPs.",
    "Examples:\r\n"
    "  sweep power_down_serdes fap# 1 serdes# 7\r\n"
    "Results in reseting the power of serdes #7 of the fap that is placed\n\r"
    " on the line card in slot #2 (the lower card).",
    "",
    { SOC_SAND_BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_POWER_UP_SERDES_ID,
    "power_up_serdes",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Powers up a single serdes of one of the FEs / FAPs.",
    "Examples:\r\n"
    "  sweep power_up_serdes fap# 1 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the fap that is placed\n\r"
    " on the line card in slot #2 (the lower card).",
    "",
    { SOC_SAND_BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_FAP_DEVICE_ID,
    "fap#",
    (PARAM_VAL_RULES *)&Sweep_app_fap_no_vals[0],
    (sizeof(Sweep_app_fap_no_vals) / sizeof(Sweep_app_fap_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the fap at get connectivity map command.",
    "Examples:\r\n"
    "  sweep get_connectivity_map fap# 0\r\n"
    "Results in printing the connectivity map of the fap at the line card\n\r"
    "in the first slot (upper card in Negev, leftmost card in Gobi).",
    "",
    { SOC_SAND_BIT(1) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_FE_DEVICE_ID,
    "fe#",
    (PARAM_VAL_RULES *)&Sweep_app_fe_no_vals[0],
    (sizeof(Sweep_app_fe_no_vals) / sizeof(Sweep_app_fe_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the fe for operation. E.g., at power up/down serdes commands.",
    "Examples:\r\n"
    "  sweep power_up_serdes fe# 1 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the second fe in the fabric card.",
    "",
    { SOC_SAND_BIT(2) | SOC_SAND_BIT(5) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SERDES_ID,
    "serdes#",
    (PARAM_VAL_RULES *)&Sweep_app_serdes_no_vals[0],
    (sizeof(Sweep_app_serdes_no_vals) / sizeof(Sweep_app_serdes_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the serdes at power up/down serdes commands.",
    "Examples:\r\n"
    "  sweep power_up_serdes fe# 1 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the second fe in the fabric card.",
    "",
    { SOC_SAND_BIT(2) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_GET_QOS_PARAMS_ID,
    "get_qos_params",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the Quality of Service parameters of a single flow in the system.",
    "Examples:\r\n"
    "  sweep get_qos_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap# 0 to fap# 1 port 9 with class 7.",
    "",
    { SOC_SAND_BIT(3) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SET_QOS_PARAMS_ID,
    "set_qos_params",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the Quality of Service parameters of a single flow in the system.",
    "Examples:\r\n"
    "  sweep set_qos_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7 ...\r\n"
    "Results in setting the quality of service params of the flow\r\n"
    " that starts from fap# 0 to fap# 1 port 9 with class 7 to ...",
    "",
    { SOC_SAND_BIT(4) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SOURCE_FAP_DEVICE_ID,
    "source_fap#",
    (PARAM_VAL_RULES *)&Sweep_app_fap_no_vals[0],
    (sizeof(Sweep_app_fap_no_vals) / sizeof(Sweep_app_fap_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the source fap at the get / set QOS command.",
    "Examples:\r\n"
    "  sweep get_qos_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap# 0 to fap# 1 port 9 with class 7.",
    "",
    { SOC_SAND_BIT(3) | SOC_SAND_BIT(4) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_DESTINATION_FAP_DEVICE_ID,
    "destination_fap#",
    (PARAM_VAL_RULES *)&Sweep_app_fap_no_vals[0],
    (sizeof(Sweep_app_fap_no_vals) / sizeof(Sweep_app_fap_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the destination fap at the get / set QOS command.",
    "Examples:\r\n"
    "  sweep get_qos_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap# 0 to fap# 1 port 9 with class 7.",
    "",
    { SOC_SAND_BIT(3) | SOC_SAND_BIT(4) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_DESTINATION_PORT_ID,
    "destination_port#",
    (PARAM_VAL_RULES *)&Sweep_app_port_no_vals[0],
    (sizeof(Sweep_app_port_no_vals) / sizeof(Sweep_app_port_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the destination port at the get / set QOS command.",
    "Examples:\r\n"
    "  sweep get_qos_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap# 0 to fap# 1 port 9 with class 7.",
    "",
    { SOC_SAND_BIT(3) | SOC_SAND_BIT(4) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_DESTINATION_CLASS_ID,
    "class#",
    (PARAM_VAL_RULES *)&Sweep_app_class_no_vals[0],
    (sizeof(Sweep_app_class_no_vals) / sizeof(Sweep_app_class_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the class at the get / set QOS command.",
    "Examples:\r\n"
    "  sweep get_QOS_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "Results in printing the quality of service params of the flow\r\n"
    " that starts from fap# 0 to fap# 1 port 9 with class 7.",
    "",
    { SOC_SAND_BIT(3) | SOC_SAND_BIT(4) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SUB_FLOW_0_TYPE_ID,
    "sub_0_flow_type",
    (PARAM_VAL_RULES *)&Sweep_app_sub_flow_types[0],
    (sizeof(Sweep_app_sub_flow_types) / sizeof(Sweep_app_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The type of the the FIRST sub flow.",
    "Examples:\r\n"
    "  sweep set_QOS_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "  sub_0_flow_type wfq1 sub_0_flow_value 15\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 1000\r\n"
    "Resulting in updating the flow (on fap# 1 port# 9) to send credits to the\r\n"
    " queue on fap# 0. The flow will have 2 subflows.\n\r"
    " The first subflow is on WFQ1 and it will have a relative weight of 15\r\n"
    "   (relative to other subflows on WFQ1 with different weights).\n\r"
    " The second subflow is on EDF1 and it will have a Maximal rate of 1000 Kilo-bits.\r\n",
    "",
    { SOC_SAND_BIT(4) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SUB_FLOW_0_VALUE_ID,
    "sub_0_flow_value",
    (PARAM_VAL_RULES *)&Sweep_app_sub_flow_values[0],
    (sizeof(Sweep_app_sub_flow_values) / sizeof(Sweep_app_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The value that the FIRST subflow will be configured to.\n\r"
    " 'value' semantics depends on the 'type' value:\n\r"
    "\n\r"
    " FAP20V_SUB_FLOW_TYPE    | 'value' semantics\n\r"
    "----------------------------------------------------------\n\r"
    " FAP20V_EDF0_FLOW        | Kilo-bits - Guaranteed rate\n\r"
    " FAP20V_EDF1_FLOW        | Kilo-bits - Maximal rate\n\r"
    " FAP20V_WFQ1_FLOW        | Weight    - Range from 0 to 127\n\r"
    "",
    "Examples:\r\n"
    "  sweep set_QOS_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "  sub_0_flow_type wfq1 sub_0_flow_value 15\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 1000\r\n"
    "Resulting in updating the flow (on fap# 1 port# 9) to send credits to the\r\n"
    " queue on fap# 0. The flow will have 2 subflows.\n\r"
    " The first subflow, on WFQ1, will have a relative weight of 15\r\n"
    "   (relative to other subflows on WFQ1 with different weights).\n\r"
    " The second subflow, on EDF1, will have a Maximal rate of 1000 Kilo-bits.\r\n",
    "",
    { SOC_SAND_BIT(4) },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SUB_FLOW_1_TYPE_ID,
    "sub_1_flow_type",
    (PARAM_VAL_RULES *)&Sweep_app_sub_flow_types[0],
    (sizeof(Sweep_app_sub_flow_types) / sizeof(Sweep_app_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The type of the the SECOND sub flow.",
    "Examples:\r\n"
    "  sweep set_QOS_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "  sub_0_flow_type wfq1 sub_0_flow_value 15\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 1000\r\n"
    "Resulting in updating the flow (on fap# 1 port# 9) to send credits to the\r\n"
    " queue on fap# 0. The flow will have 2 subflows.\n\r"
    " The first subflow is on WFQ1 and it will have a relative weight of 15\r\n"
    "   (relative to other subflows on WFQ1 with different weights).\n\r"
    " The second subflow is on EDF1 and it will have a Maximal rate of 1000 Kilo-bits.\r\n",
    "",
    { SOC_SAND_BIT(4) },
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SUB_FLOW_1_VALUE_ID,
    "sub_1_flow_value",
    (PARAM_VAL_RULES *)&Sweep_app_sub_flow_values[0],
    (sizeof(Sweep_app_sub_flow_values) / sizeof(Sweep_app_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The value that the SECOND subflow will be configured to.\n\r"
    " 'value' semantics depends on the 'type' value:\n\r"
    "\n\r"
    " FAP20V_SUB_FLOW_TYPE    | 'value' semantics\n\r"
    "----------------------------------------------------------\n\r"
    " FAP20V_EDF0_FLOW        | Kilo-bits - Guaranteed rate\n\r"
    " FAP20V_EDF1_FLOW        | Kilo-bits - Maximal rate\n\r"
    " FAP20V_WFQ1_FLOW        | Weight    - Range from 0 to 127\n\r"
    "",
    "Examples:\r\n"
    "  sweep set_QOS_params source_fap# 0 destination_fap# 1 destination_port# 9 class# 7\r\n"
    "  sub_0_flow_type wfq1 sub_0_flow_value 15\r\n"
    "  sub_1_flow_type edf1 sub_1_flow_value 1000\r\n"
    "Resulting in updating the flow (on fap# 1 port# 9) to send credits to the\r\n"
    " queue on fap# 0. The flow will have 2 subflows.\n\r"
    " The first subflow, on WFQ1, will have a relative weight of 15\r\n"
    "   (relative to other subflows on WFQ1 with different weights).\n\r"
    " The second subflow, on EDF1, will have a Maximal rate of 1000 Kilo-bits.\r\n",
    "",
    { SOC_SAND_BIT(4) },
    9,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_QUEUE_ON_MSG_INTERVAL_ID,
    "queue_on_msg_task_interval",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The Queue 'ON' message task interval in microSecs (0 = disabled).",
    "Examples:\r\n"
    "  sweep queue_on_msg_task_interval set 50000\r\n"
    "Results in the modification of the interval to 50 miliseconds.\r\n",
    "",
    { SOC_SAND_BIT(6) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_AGGR_ON_MSG_INTERVAL_ID,
    "aggr_on_msg_task_interval",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The Aggregate 'ON' message task interval in microSecs (0 = disabled).",
    "Examples:\r\n"
    "  sweep aggr_on_msg_task_interval set 50000\r\n"
    "Results in the modification of the interval to 50 miliseconds.\r\n",
    "",
    { SOC_SAND_BIT(7) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SET_INTERVAL_ID,
    "set",
    (PARAM_VAL_RULES *)&Sweep_app_on_msg_task_interval_vals[0],
    (sizeof(Sweep_app_on_msg_task_interval_vals) / sizeof(Sweep_app_on_msg_task_interval_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The Aggregate 'ON' message task interval in microSecs (0 = disabled).",
    "Examples:\r\n"
    "  sweep aggr_on_msg_task_interval set 50000\r\n"
    "Results in the modification of the interval to 50 miliseconds.\r\n",
    "",
    { SOC_SAND_BIT(6) | SOC_SAND_BIT(7) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_GET_INTERVAL_ID,
    "get",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The Aggregate 'ON' message task interval in microSecs (0 = disabled).",
    "Examples:\r\n"
    "  sweep aggr_on_msg_task_interval get\r\n"
    "Results in the reading the value of the interval in microseconds.\r\n",
    "",
    { SOC_SAND_BIT(6) | SOC_SAND_BIT(7) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_REFRESH_ALL_AGGRS,
    "refresh_all_aggrs",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Task sends 'ON' message to all aggregates.",
    "Examples:\r\n"
    "  sweep aggr_on_msg_task_interval set 50000 refresh_all_aggrs\r\n"
    "Results in the modification of the interval to 50 miliseconds.\r\n"
    "  All aggregates will be targeted.\r\n",
    "",
    { SOC_SAND_BIT(7) },
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


/* } */
#endif



