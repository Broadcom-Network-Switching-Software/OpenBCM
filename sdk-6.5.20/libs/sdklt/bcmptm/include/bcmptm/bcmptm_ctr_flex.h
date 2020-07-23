/*! \file bcmptm_ctr_flex.h
 *
 * This file contains flex-ctr APIs for top-level interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CTR_FLEX_H
#define BCMPTM_CTR_FLEX_H

/*******************************************************************************
 * Includes
 */

/*******************************************************************************
 * Defines
 */

/*******************************************************************************
 * Function prototypes
 */
extern int bcmptm_ctr_flex_driver_init(int unit, bool warm);
extern int bcmptm_ctr_flex_init(int unit, bool warm);
extern int bcmptm_ctr_flex_cleanup(int unit);
extern int bcmptm_ctr_flex_enable_direct_map(int unit, bool ingress, uint32_t pool_num);
extern int bcmptm_ctr_flex_disable_direct_map(int unit, bool ingress, uint32_t pool_num);

#endif /* BCMPTM_CTR_FLEX_H */
