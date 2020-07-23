/*! \file bcma_bcmbdcmd_qspi.h
 *
 * BCMBD CLI 'qspi' command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_QSPI_H
#define BCMA_BCMBDCMD_QSPI_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_QSPI_DESC  "QSPI debug command."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_QSPI_SYNOP \
    "Write <data> [ Read <len> ]\n" \
    "FlashRead <offset> <len> [ envVAR=<var_name> ]\n" \
    "FlashWrite <offset> <data>\n" \
    "FlashErase <offset> <len>\n" \
    "FlashLoaD <offset> <file>\n" \
    "FlashDev id=<val> SiZe=<val> SectorSiZe=<val> PageSiZe=<val>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_QSPI_HELP \
    "Write\n" \
    "   Write <data> to QSPI device and show the received data if 'Read'\n" \
    "   is specified. This utility could send commands to QSPI device\n" \
    "   one by one which is useful for device driver debugging.\n\n" \
    "FlashRead\n" \
    "   Display <len> bytes of flash content from address <offset>.\n" \
    "   If \"envvar\" is specified, the read data will be saved in the\n" \
    "   given environment variable <var_name>.\n\n" \
    "FlashWrite\n" \
    "   Write <data> into flash address <offset>.\n\n" \
    "FlashErase\n" \
    "   Erase <len> bytes of flash content from address <offset>.\n" \
    "   Note that <len> will be adjusted to sector size aligned.\n\n" \
    "FlashLoaD\n" \
    "   Read data from <file> and write the data to flash address\n" \
    "   <offset>.\n\n" \
    "FlashDEV\n" \
    "   Set custom device profile for flash identification.\n" \
    "   id=<val> - Manufacturer ID.\n" \
    "   SiZe=<val> - Flash total size in bytes, <val> must greater than 0.\n" \
    "   SectorSiZe=<val> - Sector size in bytes, <val> must greater than 0.\n" \
    "   PageSiZe=<val> - Page size in bytes, <val> must greater than 0."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_QSPI_EXAMPLES \
    "write 0x9f read 3\n" \
    "flashread 0x2000 32\n" \
    "flashwrite 0 0x123456789\n" \
    "flasherase 0 512\n" \
    "flashload 0 data.bin\n" \
    "flashdev id=0x20 size=0x8000000 pagesize=256 sectorsize=0x10000"

/*!
 * \brief CLI 'qspi' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_qspi(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_QSPI_H */
