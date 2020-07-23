/*! \file bcma_i2c_board_type.h
 *
 * BCMA I2C supported board types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_I2C_BOARD_TYPE_H
#define BCMA_I2C_BOARD_TYPE_H

/*! Base board type information. */
typedef struct bcma_i2c_base_board_s {

    /*! Base board description. */
    const char *desc;

    /*! Base board ID. */
    uint32_t id;

} bcma_i2c_base_board_t;

#endif /* BCMA_I2C_BOARD_TYPE_H */
