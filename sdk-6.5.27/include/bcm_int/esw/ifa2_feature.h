/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    ifa_feature.h
 */

#ifndef IFA2_FEATURE_H_
#define IFA2_FEATURE_H_

#define IFA2_SDK_BASE_VERSION         0x0

/* Feature to support actual egress port in MD,
 * when both IFA and VXLAN termination happens
 * on same node.
 */
#define IFA2_VXLAN_IFA_TERM_SUPPORT 1

/* Support setting the tailstamp MD LNS format. */
#define IFA2_TAILSTAMP_MD_FORMAT_SET 2

#define IFA2_SDK_VERSION              IFA2_SDK_BASE_VERSION                     | \
                                     (1 << IFA2_VXLAN_IFA_TERM_SUPPORT)         | \
                                     (1 << IFA2_TAILSTAMP_MD_FORMAT_SET)

#define IFA2_UC_MIN_VERSION           0x0

extern uint32 ifa2_firmware_version;

/* Macro for IFA2 feature check */
#define IFA2_UC_FEATURE_CHECK(feature)  (ifa2_firmware_version & (1 << feature))

#endif /* IFA2_FEATURE_H_ */
