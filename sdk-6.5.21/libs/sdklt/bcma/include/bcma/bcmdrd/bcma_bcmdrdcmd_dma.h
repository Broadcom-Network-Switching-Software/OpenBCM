/*! \file bcma_bcmdrdcmd_dma.h
 *
 * CLI 'dma' command for accessing DMA-able memory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMDRDCMD_DMA_H
#define BCMA_BCMDRDCMD_DMA_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMDRDCMD_DMA_DESC \
    "DMA facilities interface."

/*! Syntax for CLI command. */
#define BCMA_BCMDRDCMD_DMA_SYNOP \
    "alloc [<mem-unit>] <count> [<name>]\n" \
    "free <dma-id>|all\n" \
    "load <dma-id> [<mem-unit>] <data>\n" \
    "fill <dma-id> [<mem-unit>] <value> [<count>]\n" \
    "edit <dma-id> [<mem-unit>]\n" \
    "dump <dma-id> [<mem-unit>] [<count>]\n" \
    "info"

/*! Help for CLI command. */
#define BCMA_BCMDRDCMD_DMA_HELP \
    "This command provides access to the DMA memory API via the following\n" \
    "sub-commands:\n" \
    "    alloc - Allocate cache-coherent DMA memory region.\n" \
    "        <mem-unit> - Memory unit (see below).\n" \
    "        <count>    - Number of memory units to allocate.\n" \
    "        <name>     - Optional string to identify the DMA region.\n" \
    "    free  - Free DMA memory region.\n" \
    "        <dma-id>   - DMA memory ID (see below).\n" \
    "    load  - Write data o DMA memory.\n" \
    "        <dma-id>   - DMA memory ID (see below).\n" \
    "        <mem-unit> - Memory unit (see below).\n" \
    "        <data>     - Space-separated list of data values which will be\n" \
    "                     parsed according to the selected memory unit.\n" \
    "                     If the memory unit is Byte, data may be specified\n" \
    "                     as a sequence of hex bytes. e.g. 0x123456abcdef.\n" \
    "                     Underscores in the hex bytes will be ignored.\n" \
    "                     If the memory unit is HEXdigit, data may be\n" \
    "                     specified as a sequence of hex digits. e.g. 12ef.\n" \
    "                     Spaces, commas, underscores or hyphens in the hex\n" \
    "                     digits will be ignored. This is for the\n" \
    "                     convenience of data copy and paste.\n" \
    "    fill  - Fill DMA memory with a single value.\n" \
    "        <dma-id>   - DMA memory ID (see below).\n" \
    "        <mem-unit> - Memory unit (see below).\n" \
    "        <value>    - Value to fill into the DMA memory.\n" \
    "        <count>    - Number of memory units to fill. If not specified,\n" \
    "                     the entire DMA region will be filled.\n" \
    "    edit  - Edit DMA memory interactively.\n" \
    "        <dma-id>   - DMA memory ID (see below).\n" \
    "        <mem-unit> - Memory unit (see below).\n" \
    "    dump  - Dump the contents of DMA memory.\n" \
    "        <dma-id>   - DMA memory ID (see below).\n" \
    "        <mem-unit> - Memory unit (see below).\n" \
    "        <count>    - Number of memory units to dump. If not specified,\n" \
    "                     the entire DMA region will be dumped.\n" \
    "    info  - List all allocated DMA memory regions, including the\n" \
    "            logical address, size in bytes, physical address,\n" \
    "            cache-coherent information, and the region name (if any).\n" \
    "            No map/unmap is required for DMA memory regions with\n" \
    "            cache-coherent attribute.\n" \
    "\n" \
    "The default memory unit is Byte, but Halfword (16-bits) or\n" \
    "Word (32-bits) can be used if this is more suitable for the DMA memory\n" \
    "contents. HEXdigit is an extra type supported in sub-command \'load\'.\n" \
    "\n" \
    "The DMA memory ID is either the logical address of the DMA memory or\n" \
    "the associated name (optionally) specified during allocation.\n" \
    "\n" \
    "If the option <name> parameter is specified with the allocation\n" \
    "command, then a set of local environment variables will be created\n" \
    "as follows:\n" \
    "    $dma_<name>_laddr - Logical address of the DMA memory region.\n" \
    "    $dma_<name>_paddr - Physical address of the DMA memory region.\n" \
    "    $dma_<name>_size  - Size of the DMA memory region.\n" \
    "\n" \
    "The following local variables will be created/updated for each\n" \
    "successful memory allocation whether the <name> is specified or not:\n" \
    "    $dma_laddr - Logical address of the DMA memory region.\n" \
    "    $dma_paddr - Physical address of the DMA memory region.\n" \
    "    $dma_size  - Size of the DMA memory region."

/*! Examples for CLI command. */
#define BCMA_BCMDRDCMD_DMA_EXAMPLES \
    "alloc 35\n" \
    "alloc w 10\n" \
    "alloc h 20 xyz\n" \
    "info\n" \
    "load xyz hex 01,02-03_04 05060708090a0b0c\n" \
    "load xyz 1 2 3 4 5 6 7 8 9 0xa 11 12\n" \
    "load xyz h 0x0201 0x0403 0x0605 0x0807 0x0a09 0xc0b\n" \
    "load xyz w 0x04030201 0x08070605 0x0c0b0a09\n" \
    "fill 0x7fa11543ba83 b 0x5a 10\n" \
    "fill 0x7fa11543ba80 w 0x5aa5a55a 5\n" \
    "fill xyz 0\n" \
    "edit 0x7fa1_1543ba84 w\n" \
    "edit xyz\n" \
    "dump xyz\n" \
    "dump xyz w 5\n" \
    "dump $dma_laddr 10\n" \
    "free xyz\n" \
    "free all"

/*!
 * \brief CLI 'dma' command handler.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmdrdcmd_dma(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clean up the resource used in the 'dma' command.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmdrdcmd_dma_cleanup(bcma_cli_t *cli);

#endif /* BCMA_BCMDRDCMD_DMA_H */
