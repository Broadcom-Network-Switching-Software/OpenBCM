/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if !defined(_TEST_CANCUN_H)
#define _TEST_CANCUN_H

#if defined(BCM_ESW_SUPPORT) && defined(CANCUN_SUPPORT)
extern int soc_cancun_pre_misc_init_load(int unit);
extern int soc_cancun_post_misc_init_load(int unit);
#endif

#endif
