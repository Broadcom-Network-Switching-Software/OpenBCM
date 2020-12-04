/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 FABRIC FLOW CONTROL H
 */
 
#ifndef _SOC_FE3200_FABRIC_FLOW_CONTROL_H_
#define _SOC_FE3200_FABRIC_FLOW_CONTROL_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/error.h>
#include <soc/dcmn/fabric.h>
#include <soc/types.h>
#include <soc/dfe/fe3200/fe3200_drv.h>



#define SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_SINGLE_PIPE                (384)
#define SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_DUAL_PIPE                  (192)
#define SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_TRIPLE_PIPE                (128)
#define SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_SINGLE_PIPE           (192)
#define SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE             (96)
#define SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE           (64)
#define SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE           (380)
#define SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_DUAL_PIPE             (188)
#define SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE           (124)
#define SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_SINGLE_PIPE    (192)
#define SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_DUAL_PIPE      (96)
#define SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_TRIPLE_PIPE    (64)



#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_SINGLE_PIPE                (1152)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_DUAL_PIPE                  (576)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_TRIPLE_PIPE                (384)
#define SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_SINGLE_PIPE    (800)
#define SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_DUAL_PIPE      (400)
#define SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_TRIPLE_PIPE    (270)
#define SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_SINGLE_PIPE    (880)
#define SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_DUAL_PIPE      (440)
#define SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_TRIPLE_PIPE    (280)
#define SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_SINGLE_PIPE    (960)
#define SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_DUAL_PIPE      (480)
#define SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_TRIPLE_PIPE    (300)
#define SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_SINGLE_PIPE    (1146)
#define SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_DUAL_PIPE      (570)
#define SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_TRIPLE_PIPE    (378)
#define SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE           (2047)
#define SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_DUAL_PIPE             (2047)
#define SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE           (2047)
#define SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE    (2047)
#define SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE      (2047)
#define SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE    (2047)

#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_THRESHOLD_SINGLE_PIPE                   (640)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (360)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (360)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (160)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (160)
#define SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (100)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_THRESHOLD_SINGLE_PIPE                   (680)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (380)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (380)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (180)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (180)
#define SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (120)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_THRESHOLD_SINGLE_PIPE                  (840)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE       (400)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE      (400)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE      (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (200)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (200)
#define SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (140)

#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_THRESHOLD_SINGLE_PIPE                   (720)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (360)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (360)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (160)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (160)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_THRESHOLD_SINGLE_PIPE                   (760)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (380)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (380)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (180)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (180)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_THRESHOLD_SINGLE_PIPE                  (800)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE       (400)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE       (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE      (400)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE      (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (200)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (200)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)
#define SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (2047)


#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_SINGLE_PIPE           (1152)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_UC_MC_DUAL_PIPE_0     (1152 - 8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_UC_MC_DUAL_PIPE_1     (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_IP_TDM_DUAL_PIPE      (1152/2)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_0 (1136)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_1 (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_2 (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_0  (572)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_1  (8)
#define SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_2  (572)
#define SOC_FE3200_DCM_MAX_THRESHOLD                                        (2047)
#define SOC_FE3200_DCM_LOCAL_SWITCH_ALMOST_FULL_OFFSET                      (19)



#define SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_SINGLE_PIPE                (768)
#define SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_DUAL_PIPE                  (384)
#define SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_TRIPLE_PIPE                (256)
#define SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_SINGLE_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE          (1023)
#define SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE    (1023)
#define SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE      (1023)
#define SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE    (1023)


#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_THERSHOLD_SINGLE_PIPE (320)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE  (80)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (80)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (80)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_THRESHOLD_SINGLE_PIPE (400)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE  (120)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (120)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (120)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_THRESHOLD_SINGLE_PIPE (500)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE  (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE  (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE  (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE  (160)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (130)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (250)
#define SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (130)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_THRESHOLD_SINGLE_PIPE (762)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE  (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE  (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE  (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE  (200)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (140)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (378)
#define SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE  (140)




#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_THRESHOLD_SINGLE_PIPE                   (380)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (240)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (240)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (150)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (150)
#define SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (90)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_THRESHOLD_SINGLE_PIPE                   (420)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE        (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE        (260)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE       (260)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE       (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (170)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (170)
#define SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (110)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_THRESHOLD_SINGLE_PIPE                  (580)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE       (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE       (280)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE      (280)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE      (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (190)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE  (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (1023)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (190)
#define SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE (130)

#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_THRESHOLD_SINGLE_PIPE               (460)
#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE           (240)
#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE          (240)
#define SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_TRIPLE_PIPE                         (160)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_THRESHOLD_SINGLE_PIPE               (500)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE           (260)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE          (260)
#define SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_TRIPLE_PIPE                         (180)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_THRESHOLD_SINGLE_PIPE              (540)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE          (280)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE         (280)
#define SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_TRIPLE_PIPE                        (200)
#define SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1                               (1023)
#define SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_2                               (1023)

#define SOC_FE3200_DCM_FIFO_SIZE_BUFFER_SIZE (1152)

soc_error_t soc_fe3200_fabric_flow_control_rci_gci_control_source_set(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t val);
soc_error_t soc_fe3200_fabric_flow_control_rci_gci_control_source_get(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t* val);
soc_error_t soc_fe3200_fabric_flow_control_thresholds_flags_validate(int unit,uint32 flags);
soc_error_t soc_fe3200_fabric_links_link_type_set(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count, soc_port_t* links);
soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3);
soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_set(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_get(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_links_link_type_get(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count_max, soc_port_t* links, uint32* links_count);
soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3);
soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value, int to_check_counter_overflow);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value, int to_check_counter_overflow);
soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value, int to_check_counter_overflow);
soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int *value);
soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_type_set(int unit, int dcm_instance, int fifo_index, soc_dfe_fabric_link_fifo_type_index_t fifo_type);


int soc_fe3200_init_dch_thresholds_config(int unit, soc_dfe_drv_dch_default_thresholds_t* dch_thresholds_default_values);
int soc_fe3200_init_dcm_thresholds_config(int unit, soc_dfe_drv_dcm_default_thresholds_t* dcm_thresholds_default_values);
int soc_fe3200_init_dcl_thresholds_config(int unit, soc_dfe_drv_dcl_default_thresholds_t* dcl_thresholds_default_values);

#endif

