/*! \file bcma_bcmpktcmd_pmddecode.h
 *
 * CLI command related to PMDDECODE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_PMDDECODE_H
#define BCMA_BCMPKTCMD_PMDDECODE_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_PMDDECODE_DESC \
    "Metadata decode."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_PMDDECODE_SYNOP \
    "<object> [Device=device_name] [Variant=variant_name]\\\n" \
    "[FlexProfile=profile] [<word0> <word1> <word2> <word3> ...]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_PMDDECODE_HELP \
    "This command is used to decode TXPMD, RXPMD and Higig3 objects followed\n" \
    "by raw 32-bit word values.\n" \
    "    Txpmd      - Decode TXPMD data.\n" \
    "    Rxpmd      - Decode RXPMD data, including fixed and flex data.\n" \
    "    RxpmdFlex  - Decode RXPMD flex data.\n" \
    "    Hg3        - Decode Higig3 data.\n\n" \
    "The command supports data decode with the format of " \
    "device and/or variant specified by below arguments. Data is " \
    "decoded with the format of current " \
    "device if no device/variant specified.\n" \
    "    Device=<device_name>   - Device name.\n" \
    "    Variant=<variant_name> - Variant name.\n" \
    "    FlexProfile=<profile>  - Flex Profile ID. This is MUST when\\\n" \
    "                             decode RxpmdFlex."

/*! Examples for CLI command. */
#define BCMA_BCMPKTCMD_PMDDECODE_EXAMPLES \
    "txpmd 0x81000000 0x2 0x6000 0x0\n" \
    "rxpmd 0x520200 0x0 0x701202 0x1100003 0x11000 0x0 0x10002 \\\n" \
    "      0xff0000 0x0 0x20000 0x10000 0x0 0x20002 0x8a 0x0 0x0 \\\n" \
    "      0x40000 0x8003009a\n" \
    "rxpmdflex fp=3 0x11000 0x0 0x10002 0xff0000 0x0 0x20000 \\\n" \
    "               0x10000 0x0 0x20002 0x8a 0x0 0x0 0x40000\n" \
    "hg3 0xaf08800f 0x10000 0x30000000 0x2\n"

/*!
 * \brief PMDDECODE command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_pmddecode(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_PMDDECODE_H */

