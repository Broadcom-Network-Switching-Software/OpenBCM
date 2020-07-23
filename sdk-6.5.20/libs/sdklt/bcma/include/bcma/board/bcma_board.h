/*! \file bcma_board.h
 *
 * Board driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BOARD_H
#define BCMA_BOARD_H

#include <sal/sal_types.h>

/*!
 * \brief Initialize board.
 *
 * This function will initialize external voltage regulators based on
 * switch OTP settings.
 *
 * \param[in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcma_board_init(int unit);

#endif /* BCMA_BOARD_H */
