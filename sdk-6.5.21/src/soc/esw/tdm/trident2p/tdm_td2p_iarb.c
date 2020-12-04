/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM Trident2+ IARB scheduler
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_td2p_init_iarb_tdm_ovs_table
@param: int, int, int, int, int*, int*, int[512], int[512]

IARB TDM Oversubscription Schedule:
  Input  1  => Core bandwidth - 960/720/640/480.
  Input  2  => 1 if there are 4x1G management ports, 0 otherwise.
  Input  3  => 1 if there are 4x2.5G management ports, 0 otherwise.
  Input  4  => 1 if there are 1x10G management port, 0 otherwise.
  Output 5  => The X-pipe TDM oversubscription table wrap pointer value.
  Output 6  => The Y-pipe TDM oversubscription table wrap pointer value.
  Output 7  => The X-pipe TDM oversubscription schedule.
  Output 8  => The Y-pipe TDM oversubscription schedule.
 */
void
tdm_td2p_init_iarb_tdm_ovs_table (
		int core_bw, 
		int mgm4x1, 
		int mgm4x2p5, 
		int mgm1x10, 
		int *iarb_tdm_wrap_ptr_ovs_x, 
		int *iarb_tdm_wrap_ptr_ovs_y, 
		int *iarb_tdm_tbl_ovs_x, 
		int *iarb_tdm_tbl_ovs_y)
{
  int i;

  switch (core_bw) {
    case 960 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 199;
      *iarb_tdm_wrap_ptr_ovs_y = 199;
      for (i = 0; i < 24; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[24] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[24] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 25; i < 49; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[49] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[49] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 50; i < 74; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[74] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 75; i < 99; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[99] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[99] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 100; i < 124; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[124] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[124] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 125; i < 149; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[149] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[149] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 150; i < 174; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[174] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[174] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 175; i < 199; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[24] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[49] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[99] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        /*
          Nate's comment in spreadsheet: "Not suppoted in Iarb. It's still possible
          to have a 10G management port scheduled with regular ports in PGW".
        */
      }
      break;
    case 720 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 159;
      *iarb_tdm_wrap_ptr_ovs_y = 159;
      for (i = 0; i < 15; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[15] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[15] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[16] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 17; i < 31; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 32; i < 47; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[47] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[47] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[48] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[48] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 49; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 79; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[79] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[79] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[80] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[80] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 81; i < 95; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 96; i < 111; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[111] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[111] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[112] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[112] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 113; i < 127; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 128; i < 143; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[143] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[143] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[144] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[144] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 145; i < 159; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[48] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[48] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[80] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[95] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[112] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[127] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 640 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 135;
      *iarb_tdm_wrap_ptr_ovs_y = 135;
      for (i = 0; i < 16; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[16] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 17; i < 33; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 34; i < 50; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[50] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[50] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 51; i < 67; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 68; i < 84; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[84] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[84] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 85; i < 101; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 102; i < 118; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[118] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[118] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 119; i < 135; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[50] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[33] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[67] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        /*
          Nate's comment in spreadsheet: "Not suppoted in Iarb. It's still possible
          to have a 10G management port scheduled with regular ports in PGW".
        */
      }
      break;
    case 480 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 105;
      *iarb_tdm_wrap_ptr_ovs_y = 105;
      for (i = 0; i < 10; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[10] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[10] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 11; i < 20; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[20] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[20] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_x[21] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[21] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 22; i < 30; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[30] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[30] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_y[31] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 32; i < 41; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 42; i < 52; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[52] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[52] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 53; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 73; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[73] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[73] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[74] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 75; i < 83; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[83] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[83] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_x[84] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_y[84] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 85; i < 94; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[94] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[94] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 95; i < 105; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_ovs_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[52] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[83] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
  }
}


/**
@name: tdm_td2p_init_iarb_tdm_lr_table
@param: int, int, int, int, int*, int*, int[512], int[512]

IARB TDM Linerate Schedule:
  Input  1  => Core bandwidth - 960/720/640/480.
  Input  2  => 1 if there are 4x1G management ports, 0 otherwise.
  Input  3  => 1 if there are 4x2.5G management ports, 0 otherwise.
  Input  4  => 1 if there are 1x10G management port, 0 otherwise.
  Output 5  => The X-pipe TDM linerate table wrap pointer value.
  Output 6  => The Y-pipe TDM linerate table wrap pointer value.
  Output 7  => The X-pipe TDM linerate schedule.
  Output 8  => The Y-pipe TDM linerate schedule.
 */
void
tdm_td2p_init_iarb_tdm_lr_table (
		int core_bw, 
		int mgm4x1, 
		int mgm4x2p5, 
		int mgm1x10, 
		int *iarb_tdm_wrap_ptr_lr_x, 
		int *iarb_tdm_wrap_ptr_lr_y, 
		int *iarb_tdm_tbl_lr_x, 
		int *iarb_tdm_tbl_lr_y)
{
  int i;

  switch (core_bw) {
    case 960 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 199;
      *iarb_tdm_wrap_ptr_lr_y = 199;
      for (i = 0; i < 24; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[24] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[24] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 25; i < 49; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[49] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[49] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 50; i < 74; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[74] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 75; i < 99; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[99] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[99] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 100; i < 124; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[124] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[124] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 125; i < 149; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[149] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[149] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 150; i < 174; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[174] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[174] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 175; i < 199; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[24] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[124] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[174] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[199] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[24] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[124] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[174] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[49] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[99] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[149] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[199] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 720 : 
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 159;
      *iarb_tdm_wrap_ptr_lr_y = 159;
      for (i = 0; i < 15; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[15] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[15] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 16; i < 31; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 32; i < 47; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[47] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[47] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 48; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 79; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[79] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[79] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 80; i < 95; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 96; i < 111; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[111] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[111] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 112; i < 127; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 128; i < 143; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[143] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[143] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 144; i < 159; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[15] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[47] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[15] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[47] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[79] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[95] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[111] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[127] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 640 : 
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 135;
      *iarb_tdm_wrap_ptr_lr_y = 135;
      for (i = 0; i < 16; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[16] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[16] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 17; i < 33; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 34; i < 50; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[50] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[50] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 51; i < 67; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 68; i < 84; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[84] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[84] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 85; i < 101; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 102; i < 118; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[118] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[118] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 119; i < 135; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[50] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[33] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[67] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[50] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[33] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[67] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[84] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[101] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[118] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[135] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 480 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 105;
      *iarb_tdm_wrap_ptr_lr_y = 105;
      for (i = 0; i < 10; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[10] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[10] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 11; i < 20; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[20] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_lr_y[20] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_x[21] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[21] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 22; i < 30; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[30] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[30] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_y[31] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 32; i < 41; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 42; i < 52; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[52] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[52] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 53; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 73; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[73] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_lr_y[73] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[74] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 75; i < 83; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[83] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[83] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_x[84] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_y[84] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 85; i < 94; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[94] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[94] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 95; i < 105; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[52] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[83] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
  }
}


/**
@name: tdm_td2p_set_iarb_tdm_table
@param: int, int, int, int, int, int, int*, int*, int[512], int[512]

IARB TDM Schedule Generator:
  Return    => 1 if error, 0 otherwise.
  Input  1  => Core bandwidth - 960/720/640/480.
  Input  2  => 1 if X-pipe is oversub, 0 otherwise.
  Input  3  => 1 if Y-pipe is oversub, 0 otherwise.
  Input  4  => 1 if there are 4x1G management ports, 0 otherwise.
  Input  5  => 1 if there are 4x2.5G management ports, 0 otherwise.
  Input  6  => 1 if there are 1x10G management port, 0 otherwise.
  Output 7  => The X-pipe TDM table wrap pointer value.
  Output 8  => The Y-pipe TDM table wrap pointer value.
  Output 9  => The X-pipe TDM schedule.
  Output 10 => The Y-pipe TDM schedule.
 */
int 
tdm_td2p_set_iarb_tdm_table (
		int core_bw, 
		int is_x_ovs, 
		int is_y_ovs, 
		int mgm4x1, 
		int mgm4x2p5, 
		int mgm1x10, 
		int *iarb_tdm_wrap_ptr_x, 
		int *iarb_tdm_wrap_ptr_y, 
		int iarb_tdm_tbl_x[512], 
		int iarb_tdm_tbl_y[512])
{
	/* #if ( defined(_SET_TDM_DV) || defined(_SET_TDM_DEV) ) */
		int i;
		int is_succ;
		int iarb_tdm_wrap_ptr_ovs_x, iarb_tdm_wrap_ptr_ovs_y;
		/*int iarb_tdm_tbl_ovs_x[512], iarb_tdm_tbl_ovs_y[512];*/
		int iarb_tdm_wrap_ptr_lr_x, iarb_tdm_wrap_ptr_lr_y;
		/*int iarb_tdm_tbl_lr_x[512], iarb_tdm_tbl_lr_y[512];*/
		int *iarb_tdm_tbl_ovs_x, *iarb_tdm_tbl_ovs_y;
		int *iarb_tdm_tbl_lr_x, *iarb_tdm_tbl_lr_y;
		iarb_tdm_tbl_ovs_x = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_ovs_x");
		iarb_tdm_tbl_ovs_y = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_ovs_y");
		iarb_tdm_tbl_lr_x = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_lr_x");
		iarb_tdm_tbl_lr_y = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_lr_y");
	/* #else
		int i;
		int is_succ;
		int iarb_tdm_wrap_ptr_ovs_x, iarb_tdm_wrap_ptr_ovs_y;
		int iarb_tdm_wrap_ptr_lr_x, iarb_tdm_wrap_ptr_lr_y;
		TDM_ALLOC(iarb_tdm_tbl_ovs_x, int, 512, "iarb_tdm_tbl_ovs_x");
		TDM_ALLOC(iarb_tdm_tbl_ovs_y, int, 512, "iarb_tdm_tbl_ovs_y");
		TDM_ALLOC(iarb_tdm_tbl_lr_x, int, 512, "iarb_tdm_tbl_lr_x");
		TDM_ALLOC(iarb_tdm_tbl_lr_y, int, 512, "iarb_tdm_tbl_lr_y");
	#endif */

  /*
    Initial IARB TDM table containers - to be copied into final container based
    on the TDM selected.
  */
  if (!(!mgm4x1 && !mgm4x2p5 && !mgm1x10) && !(mgm4x1 ^ mgm4x2p5 ^ mgm1x10)) {
      TDM_ERROR0("IARB -- Multiple management port settings specified!\n");
  }

	tdm_td2p_init_iarb_tdm_ovs_table(core_bw, mgm4x1, mgm4x2p5, mgm1x10, 
                          &iarb_tdm_wrap_ptr_ovs_x, &iarb_tdm_wrap_ptr_ovs_y,
                          iarb_tdm_tbl_ovs_x, iarb_tdm_tbl_ovs_y);
	tdm_td2p_init_iarb_tdm_lr_table(core_bw, mgm4x1, mgm4x2p5, mgm1x10,
                         &iarb_tdm_wrap_ptr_lr_x, &iarb_tdm_wrap_ptr_lr_y,
                         iarb_tdm_tbl_lr_x, iarb_tdm_tbl_lr_y);

  if ((is_x_ovs == 0) && (is_y_ovs == 0)) {
    /* The following TDMs have linerate X-pipe and linerate Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_lr_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_lr_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_lr_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_lr_y, sizeof(int) * 512);
  }
  if ((is_x_ovs == 0) && (is_y_ovs == 1)) {
    /* The following TDMs have linerate X-pipe and oversubscribed Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_lr_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_ovs_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_lr_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_ovs_y, sizeof(int) * 512);
  }
  if ((is_x_ovs == 1) && (is_y_ovs == 0)) {
    /* The following TDMs have oversubscribed X-pipe and linerate Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_ovs_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_lr_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_ovs_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_lr_y, sizeof(int) * 512);
  }
  if ((is_x_ovs == 1) && (is_y_ovs == 1)) {
    /* The following TDMs have oversubscribed X-pipe and oversubscribed Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_ovs_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_ovs_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_ovs_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_ovs_y, sizeof(int) * 512);
  }

  TDM_PRINT1("IARB -- iarb_tdm_wrap_ptr_x = %d\n",*iarb_tdm_wrap_ptr_x);
  for (i = 0; i <= *iarb_tdm_wrap_ptr_x; i++) {
    TDM_PRINT2("IARB -- iarb_tdm_tbl_x[%d] = %d\n",i,iarb_tdm_tbl_x[i]);
  }
  TDM_PRINT1("IARB -- iarb_tdm_wrap_ptr_y = %d\n",*iarb_tdm_wrap_ptr_y);
  for (i = 0; i <= *iarb_tdm_wrap_ptr_y; i++) {
    TDM_PRINT2("IARB -- iarb_tdm_tbl_y[%d] = %d\n",i,iarb_tdm_tbl_y[i]);
  }

  /* Always succeeds by definition. */
  is_succ = 1;
  TDM_FREE(iarb_tdm_tbl_ovs_x);
  TDM_FREE(iarb_tdm_tbl_ovs_y);
  TDM_FREE(iarb_tdm_tbl_lr_x);
  TDM_FREE(iarb_tdm_tbl_lr_y);
  return is_succ;
}
