/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ifa_feature.h
 */

#ifndef IFA_FEATURE_H_
#define IFA_FEATURE_H_

#define IFA_SDK_BASE_VERSION         0x0

/* Number of hops in the topology is
 * greater than Hop limit,  IFA MD stack
 * contains Hop limit number of MDs in
 * MD stack.
 */
#define IFA_CONFIG_TRUE_HOP_COUNT      1

#define IFA_SDK_VERSION              IFA_SDK_BASE_VERSION                     | \
                                     (1 << IFA_CONFIG_TRUE_HOP_COUNT)

#define IFA_UC_MIN_VERSION           0x0

extern uint32 ifa_firmware_version;

/* Macro for IFA feature check */
#define IFA_UC_FEATURE_CHECK(feature)  (ifa_firmware_version & (1 << feature))

#endif /* IFA_FEATURE_H_ */
