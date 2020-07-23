/*! \file bcmlrd_init.h
 *
 * \brief Logical Table Resource Database
 *
 * Logical Table Initialization and cleanup
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_INIT_H
#define BCMLRD_INIT_H

/*!
 * \brief Initialize the Logical Resource Database.
 *
 * Initialize the Logical Resource Database.
 *
 * Must be called first before any other LRD calls.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_init(void);

/*!
 * \brief Clean up the Logical Resource Database.
 *
 * Release any resources allocated by the LRD. No other LRD calls
 * other than bcmlrd_init can be called after this.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_cleanup(void);

#endif /* BCMLRD_INIT_H */
