/*! \file bcma_i2ccmd_i2c.h
 *
 * CLI 'i2c' command for I2C control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_I2CCMD_I2C_H
#define BCMA_I2CCMD_I2C_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_I2CCMD_I2C_DESC  "I2C control user interface."

/*! Syntax for CLI command. */
#define BCMA_I2CCMD_I2C_SYNOP \
    "reset\n" \
    "show\n" \
    "read saddr <comm> <len>\n" \
    "readw saddr <comm>\n" \
    "readb saddr <len>\n" \
    "write saddr <comm> <data>\n" \
    "writew saddr <comm> <word_data>\n"

/*! Help for CLI command. */
#define BCMA_I2CCMD_I2C_HELP \
    "reset \n" \
    "    - reset I2C bus controller core. \n" \
    "show \n" \
    "    - show devices found and their attributes. \n" \
    "read saddr <comm> <len> \n" \
    "    - generic interface to read devices, do probe first \n" \
    "readw saddr <comm> \n" \
    "    - generic interface to read one word from devices, do probe first \n" \
    "readb saddr <len> \n" \
    "    - generic interface to read devices without register based access, \n" \
    "      do probe first \n" \
    "write saddr <comm> <data> \n" \
    "    - generic interface to write a byte to devices, do probe first \n" \
    "    - comm is data when writing directly otherwise command register \n" \
    "writew saddr <comm> <word_data> \n" \
    "    - generic interface to write a word to devices, do probe first \n" \
    "    - comm is data when writing directly otherwise command register \n"

/*! Examples for CLI command. */
#define BCMA_I2CCMD_I2C_EXAMPLES \
    "show\n" \
    "readb 0x27 1\n" \
    "read 0x5c 0xe6 1\n" \
    "readw 0x5c 0xe7\n" \
    "write 0x73 0x8 0x8\n"

/*!
 * \brief CLI 'i2c' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_i2ccmd_i2c(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_I2CCMD_I2C_H */
