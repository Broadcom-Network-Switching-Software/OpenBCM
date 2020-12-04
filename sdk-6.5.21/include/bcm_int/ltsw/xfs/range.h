/*! \file range.h
 *
 * Range headfiles to declare internal APIs for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMINT_LTSW_XFS_RANGE_H
#define BCMINT_LTSW_XFS_RANGE_H

extern int
xfs_ltsw_range_clear(int unit);

extern int
xfs_ltsw_range_rtype_add(int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config);

extern int
xfs_ltsw_range_rtype_get(int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config);

#endif /* BCMINT_LTSW_XFS_RANGE_H */
