/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    PTP_feature.h
 *
 * Holds SDK features version info, to be shared with PTP FW
 */

#ifndef __PTP_FEATURE_H__
#define __PTP_FEATURE_H__

/*
#define PTP_SDK_VERSION         0x01000000
#define PTP_UC_MIN_VERSION      0x01000200
*/

#define ENABLE       1
#define DISABLE      0
#define PTP_SDK_BASE_VERSION         0x01000000
#define PTP_SDK_VERSION   PTP_SDK_BASE_VERSION         | \
                            (ENABLE  << PTP_VP_TOD_IN_SERIAL)        | \
                            (ENABLE  << PTP_CMICM_CHANNEL_CONF_L1BKUP) |\
                            (ENABLE  << PTP_CMICM_CHANNEL_CONF_NOL1MUXSEL)        | \
                            (ENABLE  << PTP_G8275P1_MAX_STEPS_REMOVED)  | \
                            (ENABLE  << PTP_TIMEPROPERTIES_PKT_FLAGS)  | \
                            (ENABLE  << PTP_TOD_TS_SYNC_ENABLE)  |\
                            (ENABLE  << PTP_TOD_UART_PARITY)  |\
                            (ENABLE  << PTP_GPS_SWITCH_OFFSET)


#define PTP_UC_MIN_VERSION     0x01000200

/*PTP FW version*/
extern uint32 ptp_firmware_version;

#define PTP_UC_FEATURE_CHECK(feature)  (ptp_firmware_version & (1 << feature))

/* Feature support bit should be shared between SDK and UKERNEL version.
 * 24th bit are reserved in SDK as they are set in the legacy version
 * PTP_VERSION : 0x01000200 MIN_SDK_VERSION : 0x01000000*/
#define PTP_FEATURE_RESERVED1  9
#define PTP_FEATURE_RESERVED2  24

/* Features added in uKernel PTP FW */
#define PTP_VP_TOD_IN_SERIAL                1 /*Virtual Port TODIn Source Etherenet/Serial Support*/
#define PTP_CMICM_CHANNEL_CONF_L1BKUP       2 /* CMICM L1 backup conf support in ptp channel config */
#define PTP_CMICM_CHANNEL_CONF_NOL1MUXSEL   3 /* CMICM synce conf support without L1 mux config*/
#define PTP_G8275P1_MAX_STEPS_REMOVED       4 /* Support for maxStepsRemoved as per ITU-T G.8275.1 (06/2016) */
#define PTP_TIMEPROPERTIES_PKT_FLAGS        5 /* PTP time properties packet flags */
#define PTP_TOD_TS_SYNC_ENABLE              6 /* TOD -  enable timestamper sync feature */
#define PTP_TOD_UART_PARITY                 7 /* TOD - UART Parity configuration */
#define PTP_GPS_SWITCH_OFFSET               8 /* Support for GPS - PTP offset (auto & manual) */


#endif /*__PTP_FEATURE_H__*/

