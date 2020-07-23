/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        manager.h
 * Purpose:     driver driver board mananger public interfaces
 */

#ifndef   _BOARD_MANAGER_H_
#define   _BOARD_MANAGER_H_

#include <board/board.h>

/* Internal functions */

extern board_driver_t *board_driver(void);

/* Board attributes */

#define BOARD_ATTRIBUTE_MODID \
            "modid", \
            "Base modid", \
            "modid", \
            "%d"


#define BOARD_ATTRIBUTE_NUM_MODID \
            "num_modid", \
            "Number of modids required", \
            "count", \
            "%d"


#define BOARD_ATTRIBUTE_BOARD_ID  \
            "board_id", \
            "Board ID register", \
            "number", \
            "%x"

#define BOARD_ATTRIBUTE_HAS_I2C_CLOCK  \
            "has_i2c_clock", \
            "board has an I2C clock controller", \
            "boolean", \
            "%d" 


#define BOARD_ATTRIBUTE_HAS_I2C_PWR  \
            "has_i2c_pwr", \
            "board has an I2C power controller", \
            "boolean", \
            "%d" 


#define BOARD_ATTRIBUTE_HAS_I2C_VOLTAGE  \
            "has_i2c_voltage", \
            "board has an I2C voltage controller", \
            "boolean", \
            "%d" 

/* debug */

#define PLURAL(n) (((n)==1)?"":"s")



#endif /* _BOARD_MANAGER_H_ */
