/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    st_feature.h
 */

#ifndef ST_FEATURE_H_
#define ST_FEATURE_H_

#define ST_SDK_BASE_VERSION         0x0

/* Maximum number of ports to be monitored */
#define ST_MAX_NUM_PORTS            1
/* Accept Component from collector  */
#define ST_COMP_FROM_COLLECTOR      1

#define ST_SDK_VERSION              ST_SDK_BASE_VERSION                     | \
                                    (1 << ST_MAX_NUM_PORTS)                 | \
                                    (1 << ST_COMP_FROM_COLLECTOR)

#define ST_UC_MIN_VERSION           0x0

extern uint32 st_firmware_version;

/* Macro for ST feature check */
#define ST_UC_FEATURE_CHECK(feature)  (st_firmware_version & (1 << feature))

#endif /* ST_FEATURE_H_ */
