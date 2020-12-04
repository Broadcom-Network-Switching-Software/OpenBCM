/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls_lm_dm_feature.h
 */

#ifndef MPLS_LM_DM_FEATURE_H_
#define MPLS_LM_DM_FEATURE_H_

/* Indicates the feature supported : Same bit to be used in SDK and UKERNEL
 * Reserved Bits (0, 24)
 */
#define MPLS_LM_DM_SDK_BASE_VERSION     0x00000001
#define MPLS_LM_DM_UC_MIN_VERSION       0x01000000



#define MPLS_LM_DM_PM                     1
#define MPLS_LM_DM_PM_STAT_EXTRA_ELEM     2 /* Extra elements added in
                                             * pm_stat structure.
                                             */
#define MPLS_LM_DM_FLEX_CTR_SESS_ID       3 /* Flex ctr based session id */

/* LM/DM hierarchial counter info in delay_get SDK-UC internal messaging */
#define MPLS_LM_DM_COUNTER_INFO_GET       4



#define MPLS_LM_DM_SDK_VERSION          MPLS_LM_DM_SDK_BASE_VERSION          | \
                                        (1 << MPLS_LM_DM_PM)                 | \
                                        (1 << MPLS_LM_DM_PM_STAT_EXTRA_ELEM) | \
                                        (1 << MPLS_LM_DM_FLEX_CTR_SESS_ID)   | \
                                        (1 << MPLS_LM_DM_COUNTER_INFO_GET)

/* mpls_lm_dm firmware version running in uKernel*/
extern uint32 mpls_lm_dm_firmware_version;
/*Macro for MPLS_LM_DM feature check*/
#define MPLS_LM_DM_UC_FEATURE_CHECK(feature)  (mpls_lm_dm_firmware_version & (1 << feature))

#endif /* MPLS_LM_DM_FEATURE_H_ */
