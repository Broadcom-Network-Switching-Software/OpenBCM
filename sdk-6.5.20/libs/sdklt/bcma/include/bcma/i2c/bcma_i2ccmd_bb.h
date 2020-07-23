/*! \file bcma_i2ccmd_bb.h
 *
 * CLI 'bb' command for BB control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_I2CCMD_BB_H
#define BCMA_I2CCMD_BB_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_I2CCMD_BB_DESC  "Base board control user interface."

/*! Syntax for CLI command. */
#define BCMA_I2CCMD_BB_SYNOP \
    "voltage [<voltage-source> [<value>]]\n" \
    "temperature"

/*! Help for CLI command. */
#define BCMA_I2CCMD_BB_HELP \
    "This command is used to interact with the I2C voltage and\n" \
    "temperature sensors on the Broadcom SVKs.\n" \
    "Use the voltage sub-command without any parameters to see a list\n" \
    "of all voltage sources.\n" \
    "Each sub-command can be used for different purposes as shown\n" \
    "in the descriptions below:\n" \
    "\n" \
    "voltage\n" \
    "    Show all voltage outputs and total power usage.\n" \
    "voltage <voltage-source>\n" \
    "    Show requested voltage output value.\n" \
    "voltage <voltage-source> <new-value>\n" \
    "    Set requested voltage output to new value.\n" \
    "temperature \n" \
    "    Show output of onboard thermal sensors.\n"

/*! Examples for CLI command. */
#define BCMA_I2CCMD_BB_EXAMPLES \
    "voltage\n" \
    "voltage Core\n" \
    "voltage Core 0.87\n" \
    "temperature\n"

/*!
 * \brief CLI 'bb' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_i2ccmd_bb(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Set voltage of a power source
 *
 * \param [in] unit Unit number.
 * \param [in] dev_info voltage regulator details
 * \param [in] rail_name voltage regulator to be accessed
 * \param [in] voltage voltage value to be programmed
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bb_set_rail_voltage(int unit,
                         bb_i2c_device_t *dev_info,
                         const char *rail_name,
                         double *voltage);
#endif /* BCMA_I2CCMD_BB_H */
