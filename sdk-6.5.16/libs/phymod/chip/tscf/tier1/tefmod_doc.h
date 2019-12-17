/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  $Copyright: (c) 2012 Broadcom Corp.
 *  All Rights Reserved.$
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: Main doc header file for TEFMod.
 *---------------------------------------------------------------------*/


The PMD data dump has similarly controls. The DSC states are shown with the
command below.

\code
BCM.0> phy diag xe0 dsc



***********************************
**** SERDES CORE DISPLAY STATE ****
***********************************

Average Die TMON_reg13bit = 5720
Temperature Force Val     = 0
Temperature Index         = 12  [56C to 64C]
Core Event Log Level      = 1

Core DP Reset State       = 0

Common Ucode Version       = 0xd107
Common Ucode Minor Version = 0x0
AFE Hardware Version       = 0xb0

LN (CDRxN  ,UC_CFG) SD LCK RXPPM PF(M,L) VGA DCO  DFE(1,2,3,4,5,6)        TXPPM TXEQ(n1,m,p1,p2,p3) EYE(L,R,U,D)  LINK_TIME
 0 (OSx1   ,0x 4)   1   1    0    0,2    16   4  48, -7,  5,  2,  0,  0     0     0,100,12, 0, 0   171,359, 82, 82    94.5


------------------------------------------------------------------------
Falcon PMD State
PARAMETER       	         LN0         LN1         LN2         LN3
------------------------------------------------------------------------
TX drv_hv_disable	           0           0           0           0
TX ana_rescal   	           8           8           8           8
TX amp_ctrl     	           8           8           8           8
TX pre_tap      	           0           0           0           0
TX main_tap     	         100         100         100         100
TX post1_tap    	          12          12          12          12
TX post2_tap    	           0           0           0           0
TX post3_tap    	           0           0           0           0
------------------------------------------------------------------------
Sigdet          	           1           1           1           1
PMD_lock        	           1           1           1           1
DSC_SM (1st read)	        3feh        3feh        3feh        3feh
DSC_SM (2nd read)	         80h         80h         80h         80h
------------------------------------------------------------------------
PPM             	           0           0           0           0
VGA             	          16          17          18          17
PF              	           0           0           0           0
PF2             	           2           1           1           2
main_tap        	         -28         -28         -28         -28
data_thresh     	          48          47          48          51
phase_thresh    	          63          63          63          63
lms_thresh      	           0           0           0           0
------------------------------------------------------------------------
d/dq hoffset    	          64          64          64          64
p/pq hoffset    	          65          65          66          66
l/lq hoffset    	          66          64          66          67
d/p hoffset     	          31          31          32          30
d/l hoffset     	           0           0           0         255
------------------------------------------------------------------------
dfe[2][a,b]     	      -7, -8      -3, -4      -4, -5      -5, -6
dfe[2][c,d]     	      -7, -6      -3, -3      -5, -5      -6, -6
dfe[3][a,b]     	       6,  6       7,  7       6,  6       5,  5
dfe[3][c,d]     	       5,  6       7,  7       6,  6       5,  6
dfe[4][a,b]     	       2,  2       1,  1       5,  5       2,  2
dfe[4][c,d]     	       2,  3       0,  1       6,  5       2,  2
dfe[5][a,b]     	       1,  1       4,  4       1,  1      -1, -1
dfe[5][c,d]     	       0,  0       4,  3       1,  1      -1,  0
dfe[6][a,b]     	       0,  1       1,  1       1,  1       1,  1
dfe[6][c,d]     	       1,  1       1,  1       1,  1       1,  1
------------------------------------------------------------------------
dfe[7][a,b]     	       0,  0       0,  0       0,  0       0,  0
dfe[7][c,d]     	       0,  0       0,  0       0,  0       0,  0
dfe[8][a,b]     	       0,  0       0,  0       0,  0       0,  0
dfe[8][c,d]     	       0,  0       0,  0       0,  0       0,  0
dfe[9][a,b]     	       0,  0       0,  0       0,  0       0,  0
dfe[9][c,d]     	       0,  0       0,  0       0,  0       0,  0
dfe[10][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[10][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[11][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[11][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[12][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[12][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[13][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[13][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[14][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[14][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[15][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[15][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[16][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[16][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[17][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[17][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[18][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[18][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[19][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[19][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[20][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[20][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[21][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[21][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[22][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[22][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[23][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[23][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[24][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[24][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[25][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[25][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[26][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[26][c,d]    	       0,  0       0,  0       0,  0       0,  0
------------------------------------------------------------------------
dc_offset       	           4          -7          13          12
data_p[a,b]     	      -9,  4      -1, -1      -3,  3      -9, -3
data_p[c,d]     	       2, -3      -4, -5       2,  0       1,  0
data_n[a,b]     	      -3,  0      -1,  0      -2, -9       4, -6
data_n[c,d]     	      -7,  2       1,  0      -2, -2      -4, -2
phase_p[a,b]    	      -5,  4       0, -5      -9, -6      -5, -9
phase_p[c,d]    	      -4, -1       4,  0      -4, -1       5,  9
phase_n[a,b]    	      -5,  6     -11, -1       0,  0       0, -2
phase_n[c,d]    	       2,  4      -1, -7      -3,  2      -1, -2
lms[a,b]        	     -11,  3       5,-14      -3, -1      -1,  1
lms[c,d]        	      -1, -3      -9, -4      -7, -4       1,-10
------------------------------------------------------------------------
PRBS_CHECKER    	           0           0           0           0
PRBS_ORDER      	          31          31          31          31
PRBS_LOCK       	           0           0           0           0
PRBS_ERRCNT     	  2147483648  2147483648  2147483648  2147483648
------------------------------------------------------------------------



********************************************
**** SERDES UC TRACE MEMORY DUMP ***********
********************************************

  DEBUG INFO: trace memory read index = 0x0026
  DEBUG INFO: trace memory size = 0x0300

  0x8017  0xd911  0xff00  0x0283  0xff94  0x0e81  0xfdc3  0x0e80    0
  0xfd24  0x0e82  0xfcad  0x0e80  0xd81a  0x0783  0xd818  0x0782    16
  0xd816  0x0781  0xd815  0x0700  0x0000  0x0000  0x0000  0x0000    32
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    48
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    64
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    80
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    96
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    112
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    128
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    144
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    160
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    176
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    192
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    208
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    224
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    240
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    256
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    272
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    288
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    304
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    320
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    336
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    352
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    368
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    384
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    400
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    416
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    432
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    448
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    464
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    480
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    496
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    512
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    528
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    544
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    560
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    576
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    592
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    608
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    624
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    640
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    656
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    672
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    688
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    704
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    720
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    736
  0x0000  0x0000  0x0000  0x0000  0x041c  0x0df8  0x0000  0x0000    752
  Lane 0:   t= 0 ms,  Start reading event log

      2 timestamp wraparound(s). 

  Lane 3:   t= -717 ms (+5),  Rx PMD lock
  Lane 1:   t= -722 ms (+1),  Rx PMD lock
  Lane 0:   t= -723 ms (+1),  Rx PMD lock
  Lane 2:   t= -724 ms (+94),  Rx PMD lock
  Lane 0:   t= -818 ms (+0),  Exit from restart
  Lane 3:   t= -818 ms (+0),  Exit from restart
  Lane 2:   t= -818 ms (+0),  Exit from restart
  Lane 1:   t= -818 ms
========== End of Event Log ==================
\endcode

The command below is yet to be implemented.

\code
BCM.0> phy diag xe0 dsc ber
\endcode
The command below shows a variety of PMD core AND lane configurations.
\code

BCM.0> phy diag xe0 dsc config


***********************************
**** SERDES CORE CONFIGURATION ****
***********************************

uC Config VCO Rate   = 19 (10.250GHz)
Core Config from PCS = 0

Lane Addr 0          = 0
Lane Addr 1          = 1
Lane Addr 2          = 2
Lane Addr 3          = 3
TX Lane Map 0        = 0
TX Lane Map 1        = 1
TX Lane Map 2        = 2
TX Lane Map 3        = 3



*************************************
**** SERDES LANE 0 CONFIGURATION ****
*************************************

Auto-Neg Enabled      = 0
DFE on                = 1
Brdfe_on              = 0
Media Type            = 0
Unreliable LOS        = 0
Scrambling Disable    = 0
CL72 Emulation Enable = 0
Lane Config from PCS  = 0

CL72 Training Enable  = 0
EEE Mode Enable       = 0
OSR Mode Force        = 1
OSR Mode Force Val    = 0
TX Polarity Invert    = 0
RX Polarity Invert    = 0

TXFIR Post2           = 0
TXFIR Post3           = 0
TXFIR Override Enable = 0
TXFIR Main Override   = 102
TXFIR Pre Override    = 12
TXFIR Post Override   = 0

\endcode
Get CL72 specific information with this command
\code
BCM.0> phy diag ce0 dsc CL72 
 tscf_phy_pmd_info_dump:449 type = 8192 laneMask  = 0xF
 tscf_phy_pmd_info_dump:469 type = CL72


***************************
** LANE 0 CL93n72 Status **
***************************
cl93n72_signal_detect         = 1   (1 = CL93n72 training FSM in SEND_DATA state:  0 = CL93n72 in training state)
cl93n72_ieee_training_failure = 0   (1 = Training failure detected:                0 = Training failure not detected)
cl93n72_ieee_training_status  = 0   (1 = Start-up protocol in progress:            0 = Start-up protocol complete)
cl93n72_ieee_receiver_status  = 1   (1 = Receiver trained and ready to receive:    0 = Receiver training)

 tscf_phy_pmd_info_dump:469 type = CL72
\endcode
Get DSC specific information with this command
\code
BCM.0> phy diag xe0 dsc debug


************************************
**** SERDES LANE 0 DEBUG STATUS ****
************************************

Restart Count       = 5
Reset Count         = 6
PMD Lock Count      = 3

Disable Startup PF Adaptation           = 0
Disable Startup DC Adaptation           = 0
Disable Startup Slicer Offset Tuning    = 0
Disable Startup Clk90 offset Adaptation = 1
Disable Startup P1 level Tuning         = 0
Disable Startup Eye Adaptaion           = 0
Disable Startup All Adaptaion           = 0

Disable Startup DFE Tap1 Adaptation = 0
Disable Startup DFE Tap2 Adaptation = 0
Disable Startup DFE Tap3 Adaptation = 0
Disable Startup DFE Tap4 Adaptation = 0
Disable Startup DFE Tap5 Adaptation = 0
Disable Startup DFE Tap1 DCD        = 0
Disable Startup DFE Tap2 DCD        = 0

Disable Steady State PF Adaptation           = 0
Disable Steady State DC Adaptation           = 0
Disable Steady State Slicer Offset Tuning    = 0
Disable Steady State Clk90 offset Adaptation = 1
Disable Steady State P1 level Tuning         = 0
Disable Steady State Eye Adaptaion           = 0
Disable Steady State All Adaptaion           = 0

Disable Steady State DFE Tap1 Adaptation = 0
Disable Steady State DFE Tap2 Adaptation = 0
Disable Steady State DFE Tap3 Adaptation = 0
Disable Steady State DFE Tap4 Adaptation = 0
Disable Steady State DFE Tap5 Adaptation = 0
Disable Steady State DFE Tap1 DCD        = 0
Disable Steady State DFE Tap2 DCD        = 0

Retune after Reset    = 1
Clk90 offset Adjust   = 57
Clk90 offset Override = 0
Lane Event Log Level  = 2

\endcode
