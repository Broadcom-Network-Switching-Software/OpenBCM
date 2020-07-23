/*! \file ngknet_procfs.h
 *
 * Procfs-related definitions and APIs for NGKNET module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef NGKNET_PROCFS_H
#define NGKNET_PROCFS_H

/*!
 * \brief Initialize procfs for KNET driver.
 *
 * Create procfs read/write interfaces.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
ngknet_procfs_init(void);

/*!
 * \brief Clean up procfs for KNET driver.
 *
 * Clean up resources allocated by \ref ngknet_procfs_init.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
ngknet_procfs_cleanup(void);

#endif /* NGKNET_PROCFS_H */

