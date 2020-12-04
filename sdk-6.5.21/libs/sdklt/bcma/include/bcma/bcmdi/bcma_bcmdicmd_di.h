/*! \file bcma_bcmdicmd_di.h
 *
 * CLI command related to firmware loader operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMDICMD_DI_H
#define BCMA_BCMDICMD_DI_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMDICMD_DI_DESC \
    "Device Init (DI) information and control operations."

/*! Syntax for CLI command. */
#define BCMA_BCMDICMD_DI_SYNOP \
    "<component> <operation> [options]"

/*! Help for CLI command. */
#define BCMA_BCMDICMD_DI_HELP \
    "This command provides the operations to retrieve Device Init (DI)\n" \
    "related information, manually loading DI values and also the capability\n" \
    "to toggle DI related debug switches.\n" \
    "The following are the components, operations and the available options:\n" \
    "<component>\n" \
    "All - Information of all loaded BCMDI components\n" \
    "DeViceCode\n" \
    "    <operation>\n" \
    "    Info - Information of loaded DeviceCode.\n" \
    "    LoaD - Load DeviceCode\n" \
    "FLexCode\n" \
    "    <operation>\n" \
    "    Info         - Information of loaded FlexCode.\n" \
    "    LIst         - List available FlexCode types\n" \
    "    LoaD <type>  - Load a type of FlexCode. Default is \"DEFAULT\"\n" \
    "    Verify <opt> - Verify loaded FlexCode\n" \
    "        <opt>\n" \
    "        DataBase: Compare device values with FlexCode database\n" \
    "        ResetVal: Compare device values with device reset values\n" \
    "    Debug <opt>  - Set/Get debug switches\n" \
    "        <opt>\n" \
    "        Show:   Display current debug switches\n" \
    "        All:    Set all debug switches\n" \
    "        Verify: Set all verify debug switches\n" \
    "        Clear:  Clear all debug switches"

/*! Examples for CLI command. */
#define BCMA_BCMDICMD_DI_EXAMPLES \
    "all\n" \
    "dvc info\n" \
    "flc list\n" \
    "flc load UFT_Mode_1\n" \
    "flc verify resetval\n" \
    "flc debug all\n" \
    "flc debug clear"

/*!
 * \brief Firmware loader command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmdicmd_di(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMDICMD_DI_H */

