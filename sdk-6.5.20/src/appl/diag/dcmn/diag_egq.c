/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag_egq.c
 * Purpose:     Egq diagnostics display.
 */

#ifdef BCM_PETRA_SUPPORT

#include <shared/bsl.h>
#include <soc/drv.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <appl/diag/diag.h>
#include <appl/diag/diag_pp.h>
#include <appl/diag/diag_field.h>
#include <sal/appl/sal.h>
#include <appl/diag/dcmn/diag.h>
#include <appl/diag/system.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <bcm_int/petra_dispatch.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/port_sw_db.h>
#include <bcm/types.h>
#include <soc/defs.h>
#include <soc/field.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/cosq.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/error.h>

STATIC void
dpp_diag_print_string_in_loop(int nof_iters, const char *str, uint8 new_line) {
   int i = 0;
   for (i = 0; i < nof_iters; i++) {
       cli_out("%s", str);
   }
   if (new_line == 1) {
       cli_out("|\n| ");
   } else if (new_line == 2) {
       cli_out(" \n| ");
   } else if (new_line == 3) {
       cli_out("\n  ");
   } else if (new_line == 4) {
       cli_out("\n _");
   }
}

STATIC cmd_result_t
dpp_diag_egq_capture(int unit) {
    int  vsc128_print = 0, vsc256_primary_print = 0, vsc256_secondary_print = 0;
    uint64    rvp;
    int       core;
    soc_reg_above_64_val_t rvp_above_64, field;

    SOC_REG_ABOVE_64_CLEAR(rvp_above_64);
    SOC_REG_ABOVE_64_CLEAR(field);

    if (SOC_IS_JERICHO(unit)) {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
            if (READ_EGQ_LAST_PRP_SOP_HEADERr(unit, core, rvp_above_64) != SOC_E_NONE) {return CMD_FAIL;}
            soc_reg_above_64_field_get(unit, EGQ_LAST_PRP_SOP_HEADERr, rvp_above_64, LAST_PRP_SOP_HDRf, field);
            cli_out("Core %d: Last PRP VSC256 SOP header: 0x%X 0x%X 0x%X.\n", core, field[2], field[1], field[0]);
        }
    } else {
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.is_fe600) vsc128_print = 1;
        else {
            vsc256_primary_print = 1;
            if (SOC_DPP_CONFIG(unit)->arad->init.fabric.dual_pipe_tdm_packet) vsc256_secondary_print = 1;
        }

        /*vsc128 header print*/
        if (vsc128_print) {
            if (READ_EGQ_COPY_LAST_FSR_VSC_128_HDRr(unit, &rvp) != SOC_E_NONE) {return CMD_FAIL;}
            cli_out("Last FSR VSC128 SOP header: 0x%X 0x%X.\n", COMPILER_64_HI(rvp), COMPILER_64_LO(rvp));
        }

        /*vsc256 primary header print*/
        if (vsc256_primary_print) {
            if (READ_EGQ_COPY_LAST_PRP_SOP_HEADERr(unit, &rvp) != SOC_E_NONE) {return CMD_FAIL;}
            cli_out("Last PRP VSC256 SOP header: 0x%X 0x%X.\n", COMPILER_64_HI(rvp), COMPILER_64_LO(rvp));
        }

        /*vsc256 secondary header print*/
        if (vsc256_secondary_print) {
            if (READ_EGQ_COPY_LAST_LSR_SOP_HEADERr(unit, &rvp) != SOC_E_NONE) {return CMD_FAIL;}
            cli_out("Last LSR VSC256 SOP header: 0x%X 0x%X.\n", COMPILER_64_HI(rvp), COMPILER_64_LO(rvp));
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
dpp_diag_egq_port2scheduler_map_print(int unit) {
    int port_i, rv, core;
    soc_pbmp_t bmp;
    uint32 port_fap_index, port_base_q_pair, port_nof_q_pairs, port_to_ps_num;
    uint32 flags;

    cli_out("\n EGQ MAPPING:\n**************\n");
    cli_out(" Port #  |  Priorities  |  Base Q-Pair  |   PS #  | Core | TM Port |\n"
            "---------|--------------|---------------|---------|------|---------|\n");

    rv = soc_port_sw_db_valid_ports_get(unit, 0, &bmp);
    if(BCM_FAILURE(rv)) {
        return CMD_FAIL;
    }

    BCM_PBMP_ITER(bmp,port_i) {
        rv = soc_port_sw_db_flags_get(unit, port_i, &flags);
        if(BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }

        if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags)) {
            continue;
        }

        rv = soc_port_sw_db_local_to_tm_port_get(unit,port_i,&port_fap_index,&core);
        if(BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }
        rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, port_fap_index,  &port_base_q_pair);
        if(BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }
        rv = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, port_fap_index, &port_nof_q_pairs);
        if(BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }
        port_to_ps_num = ARAD_BASE_PORT_TC2PS(port_base_q_pair);
        cli_out("  %3u    |       %1u      |      %3u      |    %2u   |   %1u  |   %3u   |\n", port_i, port_nof_q_pairs, port_base_q_pair, port_to_ps_num, core, port_fap_index);
    }
    cli_out("For a graphic display of Port Scheduler <i> enter 'diag cosq qpair egq ps=<i>'.\n\n");
    return CMD_OK;
}

STATIC cmd_result_t
dpp_diag_egq_graphic(int unit, int port_num) {
   int      i = 0, j=0;
   uint8    is_sp_bmp = 0;
   uint32   TCG_to_TCs[8]; /*TCG_to_TCs_table[i]=if the x-th bit is on, port priority-x is attached to TCG i*/
   uint32   tm_port, port_nof_q_pairs, pps_base_q_pair, is_valid;
   int ps_num, rv;
   soc_port_t local_port_gport;
   int      core;

   sal_memset(TCG_to_TCs, 0, 8 * sizeof(uint32));

   rv = soc_port_sw_db_is_valid_port_get(unit, port_num, &is_valid);
   if(BCM_FAILURE(rv)) { 
       return CMD_FAIL;
   }    

   if (is_valid) {

      rv = soc_port_sw_db_local_to_tm_port_get(unit, port_num, &tm_port, &core);
      if (BCM_FAILURE(rv)) {
          return CMD_FAIL;
      }

      rv = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &port_nof_q_pairs);
      if(BCM_FAILURE(rv)) { 
          return CMD_FAIL;
      }
      rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &pps_base_q_pair);
      if(BCM_FAILURE(rv)) {
          return CMD_FAIL;
      }

      ps_num=ARAD_BASE_PORT_TC2PS(pps_base_q_pair);

      if (soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &local_port_gport)<0) return CMD_FAIL; /*map tm port to local port*/

      cli_out("\n Graphic representation of port %d through port scheduler number %d:\n"
              "**********************************************************************\n  ", port_num, ps_num);
      /*iterates over q_pairs, and when nof_q_pairs is reached finds next port index */
      for (i = 0; i < port_nof_q_pairs; i++) {
        cli_out("  Port Index %03d   ", port_num);
      }
      cli_out(" \n  ");

      for (i = 0; i < port_nof_q_pairs; i++) {
         cli_out("  Q-Pair %03d(P%d)   ", pps_base_q_pair + i, i);
      }
      cli_out("\n  ");
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "     | U | M |     ", 3);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "     |___|___|     ", 3);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "     |___|___|     ", 4);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "_______|___|_______", 2);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "       |   |       ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "  ---------------  ", 1);

      /*iterates over q_pairs and prints the weights or the strict priority*/
      for (i = 0; i < port_nof_q_pairs; i++) {
         int mode_uc, mode_mc, weight_uc, weight_mc;
         bcm_cosq_gport_info_t info;

         info.in_gport = local_port_gport;
         info.cosq = port_nof_q_pairs;

         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeUnicastEgress, &info)<0) return CMD_FAIL; /*get unicast queues*/
         if (bcm_cosq_gport_sched_get(unit, info.out_gport, i, &mode_uc, &weight_uc)<0) return CMD_FAIL;  /*get unicast queue number i*/
         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeMulticastEgress, &info)<0) return CMD_FAIL; /*get multicast queues*/
         if (bcm_cosq_gport_sched_get(unit, info.out_gport, i, &mode_mc, &weight_mc)<0) return CMD_FAIL;    /*get multicast queue number i*/

         if (mode_uc != -1 || mode_mc != -1) { /*strict priority queue (and not weighted queue)*/
             if (mode_uc > mode_mc) {
                 cli_out("  \\    L   H    /  ");
             } else {
                 cli_out("  \\    H   L    /  ");
             }
             is_sp_bmp = is_sp_bmp | (1 << i);
         } else {
             cli_out("  \\ %03d     %03d /  ", weight_uc, weight_mc);
         }
      }
      cli_out("|\n| ");

      for (i = 0; i < port_nof_q_pairs; i++) {
         if (is_sp_bmp % 2 == 1) {
             cli_out("   \\   -SP-    /   ");
         } else {
             cli_out("   \\   -WFQ-   /   ");
         }
         is_sp_bmp = is_sp_bmp >> 1;
      }
      cli_out("|\n| ");

      dpp_diag_print_string_in_loop(port_nof_q_pairs, "    -----------    ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "       |\\|/|       ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "       |/|\\|       ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

      /*iterates over q_pairs and prints the shapers bandwidth*/
      for (i = 0; i < port_nof_q_pairs; i++) {
         uint32 kbits_sec_max, flags, kbits_sec_min;
         int TCG, temp;
         bcm_cosq_gport_info_t info;

         info.in_gport = local_port_gport;
         info.cosq = i;
         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTC, &info)<0) return CMD_FAIL;
         if (bcm_cosq_gport_bandwidth_get(unit, info.out_gport, i, &kbits_sec_min, &kbits_sec_max, &flags)<0) return CMD_FAIL;
         if (kbits_sec_max == 0) {
             cli_out("    (unlimited)    ");
         } else {
             cli_out("  (%010uKbs)  ", kbits_sec_max);
         }

         /*fill the TCG_to_TCs with values*/
         if (port_nof_q_pairs==1) {
            cli_out("|\n|__________|_________|\n");
            cli_out("           |          \n");
            return CMD_OK;
         }
         else if (port_nof_q_pairs==2) {continue;}
         else{
            if (bcm_cosq_gport_sched_get(unit, info.out_gport, i, &TCG, &temp)) return CMD_FAIL;    /*get multicast queue number i*/
            TCG -= BCM_COSQ_SP0;
            TCG_to_TCs[TCG] = TCG_to_TCs[TCG] | (1 << i);
         }
      }
      cli_out("|\n| ");
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

      if (port_nof_q_pairs==2) {
         cli_out("     ----------------------------     |\n| ");
         cli_out("     \\   H                  L   /     |\n| ");
         cli_out("      \\           SP           /      |\n| ");
         cli_out("       ------------------------       |\n|_");
         cli_out("__________________|___________________|\n  ");
         cli_out("                  |                    \n");
         return CMD_OK;
      }
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "                   ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, "   |   |  ...  |   ", 0); dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, "         |         ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, " ----------------- ", 1);

      for (i = 0; i < port_nof_q_pairs; i++) {
         uint8 space_left = 15;
         uint32 temp_TFG_to_TCs_i = TCG_to_TCs[i];
         if (i < 4) {
             cli_out(" \\");
         } else {
             cli_out(" |");
         }
         for (j = 0; j < 8 && space_left > 0; j++) {
            if (TCG_to_TCs[i] % 2 == 1) {
               cli_out(" P%d", j);
               space_left -= 3;
            }
            TCG_to_TCs[i] = TCG_to_TCs[i] >> 1;
         }
         TCG_to_TCs[i] = temp_TFG_to_TCs_i;
         if (space_left == 15) {
            cli_out("(none attached)");
            space_left = 0;
         }
         dpp_diag_print_string_in_loop(space_left, " ", 0);
         if (i < 4) {
             cli_out("/ ");
         } else {
             cli_out("| ");
         }
      }
      cli_out("|\n| ");

      for (i = 0; i < port_nof_q_pairs; i++) {
         uint8 space_left = 10;
         int8  skip_first = 5;
         if (i < 4) {
             cli_out("  \\   ");
         } else {
             cli_out(" |    ");
         }
         for (j = 0; j < 8 && space_left > 0; j++) {
            if (TCG_to_TCs[i] % 2 == 1 && (skip_first--) <= 0) {
               cli_out("P%d ", j);
               space_left -= 3;
            }
            TCG_to_TCs[i] = TCG_to_TCs[i] >> 1;
         }
         dpp_diag_print_string_in_loop(space_left, " ", 0);
         if (i < 4) {
             cli_out("/  ");
         } else {
             cli_out(" | ");
         }

      }
      cli_out("|\n| ");
      for (i = 0; i < 8; i++) {
          if (i < 4) {
              cli_out("   \\-SP%d Group-/   ", i);
          } else {
              cli_out(" |     -SP%d-     | ", i);
          }
      }
      cli_out("|\n| ");
      dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, "    -----------    ", 0); dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, " ----------------- ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

      /*iterates over q_pairs and prints the TCG's CIR rates*/
      for (i = 0; i < port_nof_q_pairs; i++) {
         uint32 flags, kbits_sec_max, kbits_sec_min;
         bcm_cosq_gport_info_t info;
         
         info.in_gport = local_port_gport;
         info.cosq = i; /*TCG number*/
         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTCG, &info)<0) return CMD_FAIL;
         if (bcm_cosq_gport_bandwidth_get(unit, info.out_gport, i, &kbits_sec_min, &kbits_sec_max, &flags)<0) return CMD_FAIL;
         cli_out(" CIR:%010uKbs ", kbits_sec_max);
      }
      cli_out("|\n| ");

      /*iterates over q_pairs and prints the TCG's EIR rates*/
      for (i = 0; i < port_nof_q_pairs; i++) {
         int weight, mode;
         bcm_cosq_gport_info_t info;
    
         info.in_gport = local_port_gport;
         info.cosq = i; /*TCG number*/
         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTCG, &info)<0) return CMD_FAIL;
         if (bcm_cosq_gport_sched_get(unit, info.out_gport, i, &mode, &weight)<0) return CMD_FAIL;
         if (mode == -1) {
             cli_out(" EIR:%010u    ", weight);
         } else {
             cli_out(" EIR:   none       ");
         }
         pps_base_q_pair++;
      }
      cli_out("|\n| ");
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
      cli_out("         --------------------------------------------------------------------------------------------------------------------------------------         |\n| ");
      cli_out("                                                      |                                          |                                                      |\n| ");
      cli_out("                                                   All EIRs                                   All CIRs                                                  |\n| ");
      cli_out("                                                      |                                          |                                                      |\n| ");
      cli_out("                                               ---------------                            ---------------                                               |\n| ");
      cli_out("                                               \\    -WFQ-    /                            |     -FQ-    |                                               |\n| ");
      cli_out("                                                -------------                             ---------------                                               |\n| ");
      cli_out("                                                      |                                          |                                                      |\n| ");
      cli_out("                                                  ----------------------------------------------------                                                  |\n| ");
      cli_out("                                                  \\   L                                          H   /                                                  |\n| ");
      cli_out("                                                   \\                      -SP-                      /                                                   |\n|");
      cli_out("                                                     ------------------------------------------------                                                    |\n|");
      cli_out("_____________________________________________________________________________|___________________________________________________________________________|\n ");
      cli_out("                                                                             |\n\n");
   } else {
      cli_out("Error: Port number invalid.\n");
      return CMD_FAIL;
   }
   return CMD_OK;
}

cmd_result_t
dpp_diag_egq_print(int unit, int last_cell_print, int map_print, int ps_num) {
   /*capture last fabric cell received*/
   if (last_cell_print) {
      if (dpp_diag_egq_capture(unit) != CMD_OK) return CMD_FAIL;
   }

   /*mapping port->ps print*/
   if (map_print) {
      if (dpp_diag_egq_port2scheduler_map_print(unit) != CMD_OK) return CMD_FAIL;
   }

   /*graphic ps print*/
   if (ps_num != -1) {
      if (dpp_diag_egq_graphic(unit, ps_num) != CMD_OK) return CMD_FAIL;
   }
   return CMD_OK;
}

cmd_result_t
dpp_diag_e2e_ps_graphic(int unit, int port_num) {
   int      i = 0, j=0;
   uint32   TCG_to_TCs[8]; /*TCG_to_TCs_table[i]=if the x-th bit is on, port priority-x is attached to TCG i*/
   uint32   tm_port, port_nof_q_pairs, pps_base_q_pair, is_valid;
   int ps_num, rv;
   soc_port_t local_port_gport;
   int      core;

   sal_memset(TCG_to_TCs, 0, 8 * sizeof(uint32));

   rv = soc_port_sw_db_is_valid_port_get(unit, port_num, &is_valid);
   if(BCM_FAILURE(rv)) { 
       return CMD_FAIL;
   }

   if (is_valid) {

      rv = soc_port_sw_db_local_to_tm_port_get(unit, port_num, &tm_port, &core);
      if (BCM_FAILURE(rv)) {
          return CMD_FAIL;
      }

      rv = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &port_nof_q_pairs);
      if(BCM_FAILURE(rv)) {
          return CMD_FAIL;
      }
	  
      rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port,  &pps_base_q_pair);
      if(BCM_FAILURE(rv)) {
          return CMD_FAIL;
      }


      ps_num=ARAD_BASE_PORT_TC2PS(pps_base_q_pair);

      if (soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &local_port_gport)<0) return CMD_FAIL; /*map tm port to local port*/

      cli_out("\n Graphic representation of port %d through port scheduler number %d:\n"
              "**********************************************************************\n  ", port_num, ps_num);
      /*iterates over q_pairs, and when nof_q_pairs is reached finds next port index */
      for (i = 0; i < port_nof_q_pairs; i++) {
        cli_out("  Port Index %03d   ", tm_port);
      }
      cli_out(" \n  ");

      for (i = 0; i < port_nof_q_pairs; i++) {
         cli_out("  HR     %03d(P%d)   ", pps_base_q_pair + i, i);
      }
      cli_out("\n  ");

      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 3);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 3);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 3);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "_________|_________", 2);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);  

      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "       |\\|/|       ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "       |/|\\|       ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

      /*iterates over q_pairs and prints the shapers bandwidth*/
      for (i = 0; i < port_nof_q_pairs; i++) {
         uint32 kbits_sec_max, flags, kbits_sec_min;
         int TCG, temp;
         bcm_cosq_gport_info_t info;

         info.in_gport = local_port_gport;
         info.cosq = i;

         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, &info)<0) return CMD_FAIL;
         if (bcm_cosq_gport_bandwidth_get(unit, info.out_gport, i, &kbits_sec_min, &kbits_sec_max, &flags)<0) return CMD_FAIL;
         if (kbits_sec_max == 0) {
             cli_out("    (unlimited)    ");
         } else {
             cli_out("  (%010uKbs)  ", kbits_sec_max);
         }

         /*fill the TCG_to_TCs with values*/
         if (port_nof_q_pairs==1) {
            cli_out("|\n|__________|_________|\n");
            cli_out("           |          \n");
            return CMD_OK;
         }
         else if (port_nof_q_pairs==2) {continue;}
         else{
            if (bcm_cosq_gport_sched_get(unit, info.out_gport, i, &TCG, &temp)) return CMD_FAIL;    /*get multicast queue number i*/
            TCG -= BCM_COSQ_SP0;
            TCG_to_TCs[TCG] = TCG_to_TCs[TCG] | (1 << i);
         }
      }
      cli_out("|\n| ");
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

      if (port_nof_q_pairs==2) {
         cli_out("     ----------------------------     |\n| ");
         cli_out("     \\   H                  L   /     |\n| ");
         cli_out("      \\           SP           /      |\n| ");
         cli_out("       ------------------------       |\n|_");
         cli_out("__________________|___________________|\n  ");
         cli_out("                  |                    \n");
         return CMD_OK;
      }
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "  .    .    .    . ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "                   ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, "   |   |  ...  |   ", 0); 
      dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, "         |         ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, " ----------------- ", 1);

      for (i = 0; i < port_nof_q_pairs; i++) {
         uint8 space_left = 15;
         uint32 temp_TFG_to_TCs_i = TCG_to_TCs[i];
         if (i < 4) {
             cli_out(" \\");
         } else {
             cli_out(" |");
         }
         for (j = 0; j < 8 && space_left > 0; j++) {
            if (TCG_to_TCs[i] % 2 == 1) {
               cli_out(" P%d", j);
               space_left -= 3;
            }
            TCG_to_TCs[i] = TCG_to_TCs[i] >> 1;
         }
         TCG_to_TCs[i] = temp_TFG_to_TCs_i;
         if (space_left == 15) {
            cli_out("(none attached)");
            space_left = 0;
         }
         dpp_diag_print_string_in_loop(space_left, " ", 0);
         if (i < 4) {
             cli_out("/ ");
         } else {
             cli_out("| ");
         }
      }
      cli_out("|\n| ");

      for (i = 0; i < port_nof_q_pairs; i++) {
         uint8 space_left = 10;
         int8  skip_first = 5;
         if (i < 4) {
             cli_out("  \\   ");
         } else {
             cli_out(" |    ");
         }
         for (j = 0; j < 8 && space_left > 0; j++) {
            if (TCG_to_TCs[i] % 2 == 1 && (skip_first--) <= 0) {
               cli_out("P%d ", j);
               space_left -= 3;
            }
            TCG_to_TCs[i] = TCG_to_TCs[i] >> 1;
         }
         dpp_diag_print_string_in_loop(space_left, " ", 0);
         if (i < 4) {
             cli_out("/  ");
         } else {
             cli_out(" | ");
         }

      }
      cli_out("|\n| ");
      for (i = 0; i < 8; i++) {
          if (i < 4) {
              cli_out("   \\-SP%d Group-/   ", i);
          } else {
              cli_out(" |     -SP%d-     | ", i);
          }
      }
      cli_out("|\n| ");
      dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, "    -----------    ", 0); 
      dpp_diag_print_string_in_loop(port_nof_q_pairs / 2, " ----------------- ", 1);
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);

      /*iterates over q_pairs and prints the TCG's CIR rates*/
      for (i = 0; i < port_nof_q_pairs; i++) {
         uint32 flags, kbits_sec_max, kbits_sec_min;
         bcm_cosq_gport_info_t info;
         
         info.in_gport = local_port_gport;
         info.cosq = i; /*TCG number*/

         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTCG, &info)<0) return CMD_FAIL;
         if (bcm_cosq_gport_bandwidth_get(unit, info.out_gport, i, &kbits_sec_min, &kbits_sec_max, &flags)<0) return CMD_FAIL;
         cli_out(" CIR:%010uKbs ", kbits_sec_max);
      }
      cli_out("|\n| ");

      /*iterates over q_pairs and prints the TCG's EIR rates*/
      for (i = 0; i < port_nof_q_pairs; i++) {
         int weight, mode;
         bcm_cosq_gport_info_t info;
    
         info.in_gport = local_port_gport;
         info.cosq = i; /*TCG number*/

         if (bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTCG, &info)<0) return CMD_FAIL;
         if (bcm_cosq_gport_sched_get(unit, info.out_gport, i, &mode, &weight)<0) return CMD_FAIL;
         if (mode == -1) {
             cli_out(" EIR:%010u    ", weight);
         } else {
             cli_out(" EIR:   none       ");
         }
         pps_base_q_pair++;
      }
      cli_out("|\n| ");
      dpp_diag_print_string_in_loop(port_nof_q_pairs, "         |         ", 1);
      cli_out("         --------------------------------------------------------------------------------------------------------------------------------------         |\n| ");
      cli_out("                                                      |                                          |                                                      |\n| ");
      cli_out("                                                   All EIRs                                   All CIRs                                                  |\n| ");
      cli_out("                                                      |                                          |                                                      |\n| ");
      cli_out("                                               ---------------                            ---------------                                               |\n| ");
      cli_out("                                               \\    -WFQ-    /                            |     -FQ-    |                                               |\n| ");
      cli_out("                                                -------------                             ---------------                                               |\n| ");
      cli_out("                                                      |                                          |                                                      |\n| ");
      cli_out("                                                  ----------------------------------------------------                                                  |\n| ");
      cli_out("                                                  \\   L                                          H   /                                                  |\n| ");
      cli_out("                                                   \\                      -SP-                      /                                                   |\n|");
      cli_out("                                                     ------------------------------------------------                                                    |\n|");
      cli_out("_____________________________________________________________________________|___________________________________________________________________________|\n ");
      cli_out("                                                                             |\n\n");
   } else {
      cli_out("Error: Port number invalid.\n");
      return CMD_FAIL;
   }
   return CMD_OK;
}

#endif /*BCM_PETRA_SUPPORT*/
