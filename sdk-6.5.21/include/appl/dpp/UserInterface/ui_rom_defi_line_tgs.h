/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_LINE_TGS_INCLUDED
/* { */
#define UI_ROM_DEFI_LINE_TGS_INCLUDED
/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/ref_sys.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_line_TGS.h>
#include <appl/diag/dpp/utils_app_tgs.h>
#include <appl/diag/dpp/utils_fap20v_tests.h>

#include <sweep/Fap20v/sweep_fap20v_app.h>

#include <ui_pure_defi_line_tgs.h>



/*
 * UI definition{
 */

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tgs_on_off_vals[]
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
 */
EXTERN CONST
   PARAM_VAL
     Tgs_1_value_default
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

/********************************************************
 */
EXTERN CONST
   PARAM_VAL
     Tgs_0_value_default
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
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tgs_empty_vals[]
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
     Tgs_free_vals[]
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
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tgs_port_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_NOF_DATA_PORTS,0,
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
   PARAM_VAL_RULES
     Tgs_fap_dest_id_entry_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_MAX_LBP_DESTINATION,0,
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
   PARAM_VAL_RULES
     Tgs_dp_entry_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_MAX_DROP_PRECEDENCE,0,
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
   PARAM_VAL_RULES
     Tgs_fap_flow_id_entry_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_PKT_HDR_F_FLOW_ID_MAX,0,
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
   PARAM_VAL_RULES
     Tgs_fap_multi_id_entry_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_NOF_MULTICAST_GROUPS_FROM_PP,0,
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
   PARAM_VAL_RULES
     Tgs_fap_class_entry_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_MAX_FABRIC_UNICAST,0,
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
   PARAM_VAL_RULES
     Tgs_fpga_vals[]
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
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tgs_in_spi_head_entry_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        255,0,
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
     Tgs_params[]
#ifdef INIT
   =
/*
 ******
 * ENTRY -- 0
 *  BIT(0) - SPI_HEADER_TABLE - PRINT
 *  BIT(1) - SPI_HEADER_TABLE - ADD_raw_entry
 *  BIT(6, 7, 8) - SPI_HEADER_TABLE - MAC (logical)
 *  BIT(2) - FCT - get
 *  BIT(3) - FCT - set
 *  BIT(4) - reset&app / Clear
 *  BIT(5) - APP reset
 *  BIT(9) - IF internal loop-back
 *  BIT(10) - HELP MAC
 ******
 * ENTRY -- 1
 *BIT() - .
 */
{
  {
    PARAM_TGS_HELP_ID,
    "help",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Help Information.",
    "Examples:\r\n"
    "  tgs help mac_address\r\n"
    "Results in printing MAC addresses help information.",
    "",
    {BIT(10)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_IF_ID,
    "if",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "TGS Interfaces Utilities",
    "Examples:\r\n"
    "  tgs if mac internal_loopback on fpga_i 0\r\n"
    "Set FPGA A MAC interface with local loopback\n\r"
    "  That is, NO need for fiber loopback",
    "",
    {BIT(9)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "MAC Interface Utilities (from/to the tester generator)",
    "Examples:\r\n"
    "  tgs if mac internal_loopback on fpga_i 0\r\n"
    "Set FPGA A MAC interface with local loopback\n\r"
    "  That is, NO need for fiber loopback",
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
    PARAM_TGS_INTERNAL_LOOPBACK_ID,
    "internal_loopback",
    (PARAM_VAL_RULES *)&Tgs_on_off_vals[0],
    (sizeof(Tgs_on_off_vals) / sizeof(Tgs_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Internal-Loopback: From FAP, back to the FAP.",
    "Examples:\r\n"
    "  tgs if mac internal_loopback on fpga_i 0\r\n"
    "Set FPGA A MAC interface with local loopback\n\r"
    "  That is, NO need for fiber loopback",
    "",
    {BIT(9)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_APP_ID,
    "app",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "TGS application CLIs.",
    "Examples:\r\n"
    "  tgs app snake_mesh all_chassis\r\n"
    "Results in resetting FPGA-A&B and run the application.",
    "",
    {BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_APP_SNAKE_MESH_ID,
    "snake_mesh",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dp Snake-Mesh application on all the 256 MACs.",
    "Examples:\r\n"
    "  tgs app snake_mesh all_chassis\r\n"
    "MESH-SNAKE:\n\r"
    "1. Created 256 different routes of SNAKE traffic."
    "   + 256 different routes is true where system have "
    "     enough line-cards."
    " 2. All 256 TGS L2-MAC get over-written."
    "    + To restore call 'tgs app run_app [verbose/all_chassis]'"
    " 3. One case use one 10G port with 256x10G/256 streams "
    "    to test the chassis",
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
    PARAM_TGS_APP_TRIPLE_PLAY_ID,
    "triple_play",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
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
    PARAM_TGS_RESET_AND_APP_ENTRY_ID,
    "run_app_with_reset",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reset the TGS-FPGAs and run the application (both FPGAs).",
    "Examples:\r\n"
    "  tgs app run_app_with_reset\r\n"
    "Results in resetting FPGA-A&B and run the application.",
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
    PARAM_TGS_APP_FPGA_ID,
    "run_app",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do the application fo the TGS-FPGAs (both FPGAs, No Reset).",
    "Examples:\r\n"
    "  tgs app run_app\r\n"
    "Results in FPGA-A&B run the application -- reload SPIN table and configuration values.",
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
    PARAM_TGS_CLR_INT_ENTRY_ID,
    "get_and_clear_stats",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get and clear statuses.",
    "Examples:\r\n"
    "  tgs get_and_clear_stats\r\n"
    "Results in getting and clearing FPGA-A&B statuses.",
    "",
    {BIT(4)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FCT_ID,
    "fct",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to flow-control table.",
    "Examples:\r\n"
    "  tgs fct get_rate fpga_id 0 port_id 1\r\n"
    "Results in getting & printing the port flow-control rate.",
    "",
    {BIT(2) | BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FCT_SET_ENTRY_ID,
    "set_rate",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the port flow-control rate.",
    "Examples:\r\n"
    "  tgs fct set_rate fpga_id 0 port_id 1 rate_in_mbps 12500\r\n"
    "Results in setting the port flow-control rate.",
    "",
    {BIT(3) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FCT_GET_ENTRY_ID,
    "get_rate",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Getting print the port flow-control rate.",
    "Examples:\r\n"
    "  tgs fct get_rate fpga_id 0 port_id 1\r\n"
    "Results in getting & printing the port flow-control rate.",
    "",
    {BIT(2) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FREE_VAL_ID,
    "rate_in_mbps",
    (PARAM_VAL_RULES *)&Tgs_free_vals[0],
    (sizeof(Tgs_free_vals) / sizeof(Tgs_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Mega-bits-per-second. Range 0-0xFFFFFFFF.",
    "Examples:\r\n"
    "  tgs fct set_rate fpga_id 0 port_id 1 rate_in_mbps 12500\r\n"
    "Results in setting the port flow-control rate.",
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
    PARAM_TGS_PORT_ID_ENTRY_ID,
    "port_id",
    (PARAM_VAL_RULES *)&Tgs_port_id_vals[0],
    (sizeof(Tgs_port_id_vals) / sizeof(Tgs_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Port Id.",
    "Examples:\r\n"
    "  tgs fct get_rate fpga_id 0 port_id 1\r\n"
    "Results in getting & printing the port flow-control rate.",
    "",
    {BIT(2) | BIT(3) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_SPI_HEADER_TABLE_ID,
    "spi_header_table",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to SPI header table.",
    "Examples:\r\n"
    "  tgs spi_header_table print fpga_id 0\r\n"
    "Results in printing the SPI header table in short format.",
    "",
    {BIT(0) | BIT(1) | BIT(6) | BIT(7) | BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_MAC_ENTRY_ID,
    "mac",
    (PARAM_VAL_RULES *)&Tgs_in_spi_head_entry_vals[0],
    (sizeof(Tgs_in_spi_head_entry_vals) / sizeof(Tgs_in_spi_head_entry_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "TGS MAC address (8 msb of L2 MAC address).",
    "Examples:\r\n"
    "  tgs spi_header_table mac 0x12 fpga_id 0 unicast_dest 1 class 1\r\n"
    "Results in adding to SPI header table, MAC 12:0:0:0:0:0, FAP Unicast header, dest=1, class=1.",
    "",
    {BIT(6) | BIT(7) | BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_DP_ENTRY_ID,
    "dp",
    (PARAM_VAL_RULES *)&Tgs_dp_entry_vals[0],
    (sizeof(Tgs_dp_entry_vals) / sizeof(Tgs_dp_entry_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Tgs_0_value_default,
    0,0,0,0,0,0,0,
    "Drop Precedence.",
    "Examples:\r\n"
    "  tgs spi_header_table mac 0x12 fpga_id 0 unicast_dest 1 class 1 dp 1\r\n"
    "Results in adding to SPI header table, MAC 12:0:0:0:0:0, FAP Unicast header, dest=1, class=1 dp=1.",
    "",
    {BIT(6) | BIT(7) | BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FAP_FLOW_ID_ENTRY_ID,
    "flow_val",
    (PARAM_VAL_RULES *)&Tgs_fap_flow_id_entry_vals[0],
    (sizeof(Tgs_fap_flow_id_entry_vals) / sizeof(Tgs_fap_flow_id_entry_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP unicast Flow-Id (value to be converted to queue id).",
    "Examples:\r\n"
    "  tgs spi_header_table mac 0x12 fpga_id 0 flow 1\r\n"
    "Results in adding to SPI header table, MAC 12:0:0:0:0:0, FAP Unicast flow header, flow=1.",
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
    PARAM_TGS_FAP_DEST_ID_ENTRY_ID,
    "unicast_dest",
    (PARAM_VAL_RULES *)&Tgs_fap_dest_id_entry_vals[0],
    (sizeof(Tgs_fap_dest_id_entry_vals) / sizeof(Tgs_fap_dest_id_entry_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP unicast destination-Id (entry to the FAP LBP table). This is NOT FAP-ID.",
    "Examples:\r\n"
    "  tgs spi_header_table mac 0x12 fpga_id 0 unicast_dest 1 class 1\r\n"
    "Results in adding to SPI header table, MAC 12:0:0:0:0:0, FAP Unicast header, dest=1, class=1.",
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
    PARAM_TGS_FAP_MULTI_ID_ENTRY_ID,
    "m_id",
    (PARAM_VAL_RULES *)&Tgs_fap_multi_id_entry_vals[0],
    (sizeof(Tgs_fap_multi_id_entry_vals) / sizeof(Tgs_fap_multi_id_entry_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP multicast destination-Id (entry to the FAP multicast table).",
    "Examples:\r\n"
    "  tgs spi_header_table mac 0x12 fpga_id 0 m_id 1 class 1\r\n"
    "Results in adding to SPI header table, MAC 12:0:0:0:0:0, FAP Multicast header, m_id=1, class=1.",
    "",
    {BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FAP_CLASS_ENTRY_ID,
    "class",
    (PARAM_VAL_RULES *)&Tgs_fap_class_entry_vals[0],
    (sizeof(Tgs_fap_class_entry_vals) / sizeof(Tgs_fap_class_entry_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Tgs_0_value_default,
    0,0,0,0,0,0,0,
    "Class that will be in the packet header. Unicast 0:7, Multicast 0:3.",
    "Examples:\r\n"
    "  tgs spi_header_table mac 0x12 fpga_id 0 unicast_dest 1 class 1\r\n"
    "Results in adding to SPI header table, MAC 12:0:0:0:0:0, FAP Unicast header, dest=1, class=1.",
    "",
    {BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_ADD_RAW_ENTRY_ID,
    "add_raw_entry",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Add entry to the ingress SPI mapping table.",
    "Examples:\r\n"
    "  tgs spi_header_table add_raw_entry fpga_id 0 entry 0x12 value 0x80000000\r\n"
    "Results in adding to SPI header table, entry 0x12, value of 0x80000000 (Fap-ID).",
    "",
    {BIT(1) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_IN_SPI_HEAD_ENTRY_ID,
    "entry",
    (PARAM_VAL_RULES *)&Tgs_in_spi_head_entry_vals[0],
    (sizeof(Tgs_in_spi_head_entry_vals) / sizeof(Tgs_in_spi_head_entry_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress SPI Header table entry number. Range 0-255.\n\r"
    "  This corresponds to the 8 lower bit in the destination MAC address",
    "Examples:\r\n"
    "  tgs spi_header_table add_raw_entry fpga_id 0 entry 0x12 value 0x80000000\r\n"
    "Results in adding to SPI header table, entry 0x12, value of 0x80000000 (Fap-ID).",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FREE_VAL_ID,
    "value",
    (PARAM_VAL_RULES *)&Tgs_free_vals[0],
    (sizeof(Tgs_free_vals) / sizeof(Tgs_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Free value. Range 0-0xFFFFFFFF.",
    "Examples:\r\n"
    "  tgs spi_header_table add_raw_entry fpga_id 0 entry 0x12 value 0x80000000\r\n"
    "Results in adding to SPI header table, entry 0x12, value of 0x80000000 (Fap-ID).",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_PRINT_ID,
    "print",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print the SPI header table.",
    "Examples:\r\n"
    "  tgs spi_header_table print fpga_id 0\r\n"
    "Results in printing the SPI header table in short format.",
    "",
    {BIT(0) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_FPGA_ID,
    "fpga_id",
    (PARAM_VAL_RULES *)&Tgs_fpga_vals[0],
    (sizeof(Tgs_fpga_vals) / sizeof(Tgs_fpga_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FPGA Id to access '0' or '1'.",
    "Examples:\r\n"
    "  tgs spi_header_table print fpga_id 0\r\n"
    "Results in printing the SPI header table in short format.",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(6) | BIT(7) | BIT(8) | BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_LONG_FORMAT_ID,
    "long_format",
    (PARAM_VAL_RULES *)&Tgs_empty_vals[0],
    (sizeof(Tgs_empty_vals) / sizeof(Tgs_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Printing in long format.",
    "Examples:\r\n"
    "  tgs spi_header_table print fpga_id 0 long_format\r\n"
    "Results in printing the SPI header table in LONG format.",
    "",
    {BIT(0) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_VERBOSE_ID,
    "verbose",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do more printing.",
    "",
    "",
    {BIT(5) | BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TGS_ALL_CHASSIS_ID,
    "all_chassis",
    (PARAM_VAL_RULES *)&Gfa_empty_vals[0],
    (sizeof(Gfa_empty_vals) / sizeof(Gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Remote Call: Do action in all of the chassis.",
    "",
    "",
    {BIT(5) | BIT(9)},
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


