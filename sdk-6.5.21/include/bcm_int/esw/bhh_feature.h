/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    bhh_feature.h
 */

#ifndef BHH_FEATURE_H_
#define BHH_FEATURE_H_

#define BHH_SDK_BASE_VERSION    0x01000001

#define BHH_SDK_VERSION    BHH_SDK_BASE_VERSION          | \
                           (1 << BHH_WB_SESS_GET)        | \
                           (1 << BHH_LOSS_GET)           | \
                           (1 << BHH_PM_MODE)            | \
                           (1 << BHH_REMOTE_EP_NAME)     | \
                           (1 << BHH_FAULTS_GET)         | \
                           (1 << BHH_PM_STAT_EXTRA_ELEM) | \
                           (1 << BHH_PASSIVE_MEP)        | \
                           (1 << BHH_OLP_ENCAP)          | \
                           (1 << BHH_OLP_ENCAP_VALID)    | \
                           (1 << BHH_PRIORITY_EVENT)     | \
                           (1 << BHH_DM_DATA_TLV)        | \
                           (1 << BHH_TRUNK_SUPPORT)      | \
                           (1 << BHH_KT2_PM_SUPPORT)

#define BHH_UC_MIN_VERSION 0x01000000

/* Indicates the feature supported : Same bit has to be used in SDK and UKERNEL
 * 0 and 24th bit cannot be used as they are set in base version string
 * BHH_SDK_VERSION : 0x01000001 BHH_APPL_VERSION : 0x01000000*/

#define BHH_FEATURE_RESERVED1  0
#define BHH_FEATURE_RESERVED2  24

#define BHH_WB_SESS_GET        1
#define BHH_LOSS_GET           2
#define BHH_PM_MODE            3
#define BHH_REMOTE_EP_NAME     4
#define BHH_FAULTS_GET         5
/* Extra elements added in pm_stat structure */
#define BHH_PM_STAT_EXTRA_ELEM 6
#define BHH_PASSIVE_MEP        7 /* Add support to create following endpoints
                                  *  - Only Rx CCM
                                  *  - Only Tx CCM
                                  *  - No Tx/Rx CCM
                                  */
#define BHH_OLP_ENCAP          8 /* Enhanced messages to support OLP encap,
                                  * loss_add, delay_add and loopback_add
                                  * enhnaced to take OLP encap. Corresponding get
                                  * messages are enahanced to return OLP encap.
                                  * SESS_SET takes inner & outer vlan for hash
                                  * calculation for Section MEP. MSG init takes
                                  * an additional param to indicate max encap
                                  * length. SESS_SET message takes num_counters
                                  * to indicate number of counters associated
                                  * this endpoint
                                  */
#define BHH_OLP_ENCAP_VALID    9 /* Indicates SDK's capability to send OLP
                                  * encapsulated BHH packets.
                                  * Note that though this is set for all platforms,
                                  * On the UKernel side this will be handled only 
                                  * for platforms for which BHH_OLP compile flag is
                                  * defined, which are Saber2 and Apache.
                                  */
#define BHH_PRIORITY_EVENT    10 /* Add support to raise priority mismatch event
                                  * for BHH CCM packets received from Peer MEP
                                  * BHH: int_pri and pkt_pri handling for
                                  * various BHH message types
                                  */
#define BHH_DM_DATA_TLV       11 /* Support data tlv in DM */
#define BHH_TRUNK_SUPPORT     12 /* Specify different Rx, Tx port for a session,
                                  * Add trunk support for TR3, Hx4 and KT
                                  */

#define BHH_KT2_PM_SUPPORT    13 /* Support for PM added in KT2 for BHH-only Firmware.
                                  * It started from (SDK-6.5.6 + uKernel-4.3.3)
                                  */

/* bhh firmware version running in ukernel */
extern uint32 bhh_firmware_version;

/*Macro for BHH feature check*/
#define BHH_UC_FEATURE_CHECK(feature)  (bhh_firmware_version & (1 << feature))

#endif /* BHH_FEAT_H_ */
