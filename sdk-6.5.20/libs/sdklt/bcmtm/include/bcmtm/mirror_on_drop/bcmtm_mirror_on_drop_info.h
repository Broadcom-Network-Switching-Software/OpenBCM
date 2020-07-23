/*! \file bcmtm_mirror_on_drop_info.h
 *
 * This file contains Mirror-on-drop
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_MIRROR_ON_DROP_INFO_H
#define BCMTM_MIRROR_ON_DROP_INFO_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * BCMTM Mirror-on-drop information.
 */
typedef struct bcmtm_mirror_on_drop_info {

    /*! Current fill level in cells for the Mirror-on-drop queue. */
    uint32_t cell_usage;

    /*! High watermark of fill level in cells for the Mirror-on-drop queue. */
    uint32_t hwm_cell_usage;

} bcmtm_mirror_on_drop_info_t;

/*!
 * \brief Initialize device-specific data.
 *
 * \param [in] unit Unit nunmber.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmtm_mirror_on_drop_info_init(int unit);

#endif /* BCMTM_MIRROR_ON_DROP_INFO_H */
