/*! \file bcm56780_a0_lp_intr.h
 *
 * Device-specific iProc low-priority interrupt sources for BCM56780_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_LP_INTR_H
#define BCM56780_A0_LP_INTR_H

/*!
 * \name Device-specific low-priority interrupt sources.
 *
 * Device-specific interrupt sources in the
 * ICFG_CHIP_LP_INTR_RAW_STATUS_REGx registers.
 *
 * Please refer to the hardware documentation for details.
 *
 * \{
 */
#define CE_TO_CMIC_INTR                         0
#define DLB_CREDIT_TO_CMIC_INTR                 1
#define PVTMON_INTR                             2
#define PLL_LOCK_LOSS_INTR                      3
#define MMU_TO_CMIC_INTR                        4
#define PIPE0_EP_TO_CMIC_INTR                   5
#define PIPE1_EP_TO_CMIC_INTR                   6
#define PIPE0_IP_TO_CMIC_INTR                   7
#define PIPE1_IP_TO_CMIC_INTR                   8
#define AVS_SW_MDONE_INTR                       13
#define AVS_ROSC_THRESHOLD2_INTR                14
#define AVS_ROSC_THRESHOLD1_INTR                15
#define AVS_POWERWATCHDOG_INTR                  16
#define AVS_VDDC_MON_WARNING1_INTR              17
#define AVS_VDDC_MON_WARNING0_INTR              18
#define AVS_TEMPERATURE_RESET_INTR              19
#define AVS_TEMPERATURE_LOW_THRESHOLD_INTR      20
#define AVS_TEMPERATURE_HIGH_THRESHOLD_INTR     21
#define PM0_INTR                                64
#define PM1_INTR                                65
#define PM2_INTR                                66
#define PM3_INTR                                67
#define PM4_INTR                                68
#define PM5_INTR                                69
#define PM6_INTR                                70
#define PM7_INTR                                71
#define PM8_INTR                                72
#define PM9_INTR                                73
#define PM10_INTR                               74
#define PM11_INTR                               75
#define PM12_INTR                               76
#define PM13_INTR                               77
#define PM14_INTR                               78
#define PM15_INTR                               79
#define PM16_INTR                               80
#define PM17_INTR                               81
#define PM18_INTR                               82
#define PM19_INTR                               83
#define PM_MGMT_INTR                            84
#define PIPE0_EDB_TO_CMIC_PERR_INTR             85
#define PIPE1_EDB_TO_CMIC_PERR_INTR             86
#define PIPE2_EDB_TO_CMIC_PERR_INTR             87
#define PIPE3_EDB_TO_CMIC_PERR_INTR             88
#define PIPE0_IDB_TO_CMIC_PERR_INTR             89
#define PIPE1_IDB_TO_CMIC_PERR_INTR             90
#define PIPE2_IDB_TO_CMIC_PERR_INTR             91
#define PIPE3_IDB_TO_CMIC_PERR_INTR             92
#define MACSEC0_TO_CMIC_PERR_INTR               93
#define MACSEC1_TO_CMIC_PERR_INTR               94
#define MACSEC2_TO_CMIC_PERR_INTR               95
#define MACSEC3_TO_CMIC_PERR_INTR               96
#define MACSEC4_TO_CMIC_PERR_INTR               97
#define MACSEC5_TO_CMIC_PERR_INTR               98
/*! \} */

#endif /* BCM56780_A0_LP_INTR_H */
