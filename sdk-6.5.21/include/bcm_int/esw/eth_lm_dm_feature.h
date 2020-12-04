/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    eth_lm_dm_feature.h
 */

#ifndef ETH_LM_DM_FEATURE_H_
#define ETH_LM_DM_FEATURE_H_

#define ETH_LM_DM_SDK_BASE_VERSION    0x01000001

#define ETH_LM_DM_SDK_VERSION ETH_LM_DM_SDK_BASE_VERSION |      \
                        (1 << ETH_LM_DM_PM) |   \
                        (1 << ETH_LM_DM_PM_STAT_EXTRA_ELEM)

#define ETH_LM_DM_UC_MIN_VERSION  0x01000000 

/* Indicates the feature supported : Same bit has to be used in SDK and UKERNEL
 * 0 and 24th bit cannot be used as they are set in base version string
 * ETH_LM_DM_SDK_VERSION : 0x01000001 ETH_LM_DM_APPL_VERSION : 0x01000000*/

#define ETH_LM_DM_FEATURE_RESERVED1  0
#define ETH_LM_DM_FEATURE_RESERVED2  24

#define ETH_LM_DM_PM                    1
/* Extra elements added in pm_stat structure */
#define ETH_LM_DM_PM_STAT_EXTRA_ELEM    2
/* eth_lm_dm firmware version running in ukernel */
extern uint32 eth_lm_dm_firmware_version;

/*Macro for ETH_LM_DM feature check*/
#define ETH_LM_DM_UC_FEATURE_CHECK(feature)  (eth_lm_dm_firmware_version & (1 << feature))

#endif /* ETH_LM_DM_FEATURE_H_ */
