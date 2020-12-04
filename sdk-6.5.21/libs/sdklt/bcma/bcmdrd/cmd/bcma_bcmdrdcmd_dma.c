/*! \file bcma_bcmdrdcmd_dma.c
 *
 * CLI 'dma' command for accessing DMA-able memory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_assert.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>

#include <shr/shr_pb.h>
#include <shr/shr_pb_format.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_hal.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>

#include <bcma/bcmdrd/bcma_bcmdrdcmd_dma.h>

#define MAX_DMA_INFO_NAME_SIZE      32

/* Structure to keep the allocated DMA memory blocks information */
typedef struct dma_info_s {
    char name[MAX_DMA_INFO_NAME_SIZE];
    void *laddr;
    uint64_t paddr;
    size_t size;
    bool cache_coherent;
    struct dma_info_s *next;
} dma_info_t;

/* List of DMA memory blocks */
static dma_info_t *dma_info_list[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*
 * Transfer a ASCII character to a hexadecimal value.
 * The input character should be a hexadecimal digit.
 */
static int
xdigit2i(int xdigit)
{
    if (xdigit >= '0' && xdigit <= '9') {
        return (xdigit - '0');
    }
    if (xdigit >= 'a' && xdigit <= 'f') {
        return (xdigit - 'a' + 10);
    }
    if (xdigit >= 'A' && xdigit <= 'F') {
        return (xdigit - 'A' + 10);
    }
    return 0;
}

/*
 * Add the DMA memory block information to local variables.
 */
static int
dma_info_var_add(bcma_cli_t *cli, dma_info_t *di)
{
    char  laddr[64], paddr[64], size[64];
    char prefix[40], var_name[64];
    shr_pb_t *pb;
    uint64_t laddr64;

    pb =shr_pb_create();
    laddr64 = (uintptr_t)di->laddr;
    shr_pb_format_uint64(pb, NULL, &laddr64, 1, 0);
    sal_snprintf(laddr, sizeof(laddr), "%s", shr_pb_str(pb));
    shr_pb_reset(pb);
    shr_pb_format_uint64(pb, NULL, &di->paddr, 1, 0);
    sal_snprintf(paddr, sizeof(paddr), "%s", shr_pb_str(pb));
    sal_snprintf(size, sizeof(size), "%d", (int)di->size);
    shr_pb_destroy(pb);

    if (di->name[0] != '\0') {
        sal_strcpy(prefix, "dma_");
        sal_strcat(prefix, di->name);
        /* $dma_<name>_laddr */
        sal_strcpy(var_name, prefix);
        sal_strcat(var_name, "_laddr");
        if (bcma_cli_var_set(cli, var_name, laddr, true) < 0) {
            cli_out("%sFailed to set local variable %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, var_name);
        }
        /* $dma_<name>_paddr */
        sal_strcpy(var_name, prefix);
        sal_strcat(var_name, "_paddr");
        if (bcma_cli_var_set(cli, var_name, paddr, true) < 0) {
            cli_out("%sFailed to set local variable %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, var_name);
        }
        /* $dma_<name>_size */
        sal_strcpy(var_name, prefix);
        sal_strcat(var_name, "_size");
        if (bcma_cli_var_set(cli, var_name, size, true) < 0) {
            cli_out("%sFailed to set local variable %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, var_name);
        }
    }
    bcma_cli_var_set(cli, "dma_laddr", laddr, true);
    bcma_cli_var_set(cli, "dma_paddr", paddr, true);
    bcma_cli_var_set(cli, "dma_size", size, true);

    return 0;
}

/*
 * Allocate a DMA memory block.
 */
static dma_info_t *
dma_info_alloc(int unit, const char *name, size_t size, bool cache_coherent)
{
    dma_info_t *di;

    di = sal_alloc(sizeof(dma_info_t), "bcmaCliDmaInfo");
    if (di == NULL) {
        return NULL;
    }
    sal_memset(di, 0, sizeof(*di));

    if (name) {
        sal_strlcpy(di->name, name, sizeof(di->name));
    }
    di->size = size;
    di->cache_coherent = cache_coherent;
    di->laddr = bcmdrd_hal_dma_alloc(unit, size, name ? name : "cli",
                                     cache_coherent ? &di->paddr : NULL);
    if (di->laddr == NULL) {
        sal_free(di);
        return NULL;
    }
    return di;
}

/*
 * Free a DMA memory block.
 */
static void
dma_info_free(int unit, dma_info_t *di)
{
    if (di->laddr && bcmdrd_dev_exists(unit)) {
        bcmdrd_hal_dma_free(unit, di->size, di->laddr, di->paddr);
    }
    sal_free(di);
}

/*
 * Display the information of a DMA memory block.
 */
static void
dma_info_show(dma_info_t *di, bool header)
{
    shr_pb_t *pb;
    uint64_t laddr64;

    pb = shr_pb_create();

    if (header) {
        cli_out("%15s   %16s  %8s  %8s  %s\n",
                "Logical address", "Physical address", "Size",
                 "Coherent", "Name");
        cli_out("----------------------------------------"
                "---------------------------------------\n");
    }
    laddr64 = (uintptr_t)di->laddr;
    shr_pb_format_uint64(pb, NULL, &laddr64, 1, 0);
    cli_out("%15s   ", shr_pb_str(pb));
    shr_pb_reset(pb);
    shr_pb_format_uint64(pb, NULL, &di->paddr, 1, 0);
    cli_out("%16s  ", shr_pb_str(pb));
    cli_out("%8d  ", (int)di->size);
    cli_out("%8s  ", di->cache_coherent ? "yes  " : "no  ");
    cli_out("%s\n", di->name[0] == '\0' ? "-" : di->name);

    shr_pb_destroy(pb);
}

/*
 * Add a DMA memory block to the head of the list.
 */
static int
dma_info_list_add(int unit, dma_info_t *di)
{
    assert(di->next == NULL);
    if (dma_info_list[unit] == NULL) {
        dma_info_list[unit] = di;
    } else {
        di->next = dma_info_list[unit];
        dma_info_list[unit] = di;
    }
    return 0;
}

/*
 * Remove a DMA memory block from the list.
 */
static int
dma_info_list_del(int unit, dma_info_t *di)
{
    dma_info_t *di_cur = dma_info_list[unit];
    dma_info_t *di_prev = NULL;

    while (di_cur) {
        if (di_cur == di) {
            if (di_prev) {
                di_prev->next = di_cur->next;
            } else {
                dma_info_list[unit] = di_cur->next;
            }
            return 0;
        }
        di_prev = di_cur;
        di_cur = di_cur->next;
    }
    return -1;
}

/*
 * Find a DMA memory block by logical address or name.
 * If the searched string \c arg can be parsed as a scalar, it would be
 * taken as the logical address value to search, otherwise it would be
 * taken as the name to compare.
 * A valid logical address is a value in the range of the logical address of
 * a DMA memory block plus the memory block size.
 * If \c laddr is not NULL, it should be set to the parsed logical address
 * if a DMA memory block is found by a logical address string, otherwise it
 * should be set to 0.
 */
static dma_info_t *
dma_info_list_find(int unit, const char *arg,
                   uint64_t *laddr, bool *indexed_by_laddr)
{
    uint64_t addr_min, addr_max, parsed_addr = 0;
    bool find_by_laddr = false;
    dma_info_t *di = dma_info_list[unit];
    char *str;

    str = sal_strdup(arg);
    if (str == NULL) {
        cli_out("%sFailed to duplicate string %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return NULL;
    }

    if (bcma_cli_parse_array_uint64(str, 1, &parsed_addr) == 1) {
        find_by_laddr = true;
        if (indexed_by_laddr) {
            *indexed_by_laddr = find_by_laddr;
        }
    }
    sal_free(str);

    if (find_by_laddr) {
        /* Find the DMA memory block by logical address */
        while (di) {
            addr_min = (uintptr_t)di->laddr;
            addr_max = addr_min + di->size;
            if (parsed_addr >= addr_min && parsed_addr < addr_max) {
                if (laddr) {
                    *laddr = parsed_addr;
                }
                return di;
            }
            di = di->next;
        }
    } else {
        /* Find the DMA memory block by name */
        while (di) {
            if (sal_strcmp(di->name, arg) == 0) {
                if (laddr) {
                    *laddr = (uintptr_t)di->laddr;
                }
                return di;
            }
            di = di->next;
        }
    }

    return NULL;
}

/*
 * Show the DMA memory blocks information for a specified unit and type.
 */
static void
dma_info_list_show(int unit)
{
    dma_info_t *di = dma_info_list[unit];
    int cnt = 0;
    bool header = true;

    while (di) {
        dma_info_show(di, header);
        header = false;
        cnt++;
        di = di->next;
    }
    if (cnt == 0) {
        cli_out("No allocated DMA memory blocks\n");
    }
}

/*
 * Free all DMA memory blocks for a given device unit.
 */
static void
dma_info_list_free(int unit)
{
    dma_info_t *di;

    while (dma_info_list[unit]) {
        di = dma_info_list[unit];
        dma_info_list[unit] = di->next;
        dma_info_free(unit, di);
    }
}

/*
 * Write value to the specified DMA logical address based on the memory unit.
 */
static void
dma_mem_addr_write(uintptr_t laddr, int elsz, uint64_t val)
{
    if (elsz == 1) {
        *((uint8_t *)laddr) = val;
    } else if (elsz == 2) {
        *((uint16_t *)laddr) = val;
    } else {
        *((uint32_t *)laddr) = val;
    }
}

/*
 * Check whether the address is aligned with the memory unit.
 */
static bool
dma_mem_addr_aligned(uint64_t laddr, int elsz)
{
    if ((laddr & (elsz - 1)) != 0) {
        cli_out("%sAddress unaligned for %d-byte access\n",
                BCMA_CLI_CONFIG_ERROR_STR, elsz);
        return false;
    }
    return true;
}

/*
 * Check whether the memory address and count exceed the memory region.
 */
static bool
dma_mem_addr_range(uint64_t laddr, int elsz, int count, uint64_t laddr_max)
{
    uint64_t laddr_end;

    laddr_end = laddr + elsz * count;
    if (laddr_end > laddr_max) {
        cli_out("%sAddress out of range (0x%"PRIx64")\n",
                BCMA_CLI_CONFIG_ERROR_STR, laddr_max);
        return false;
    }
    return true;
}

/*
 * Parse the memory access type from the CLI input argument if it is specified.
 * Return the element size of memory access type.
 * If no memory access type is specified, the CLI argument will be restored and
 * return 1 (byte size) as the default memory access type.
 */
static int
dma_mem_acc_type_parse(bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        if (bcma_cli_parse_cmp("Byte", arg, 0)) {
            return 1;
        } else if (bcma_cli_parse_cmp("Halfword", arg, 0)) {
            return 2;
        } else if (bcma_cli_parse_cmp("Word", arg, 0)) {
            return 4;
        }
        /* Restore the argument for the next token parsing */
        BCMA_CLI_ARG_PREV(args);
    }
    /* Default element size is byte size */
    return 1;
}

/*
 * Parse the count of a specified memory unit.
 * Note that the argument should be placed at the end of the command syntax
 * since if the count is omitted, it would be calculated as the number to
 * the end of the memory block.
 */
static int
dma_mem_unit_count_parse(bcma_cli_args_t *args, dma_info_t *di,
                         uint64_t laddr, int elsz)
{
    const char *arg;
    uint64_t laddr_max;
    int cnt = 0;

    laddr_max = (uintptr_t)di->laddr + di->size;

    if ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        if (bcma_cli_parse_int(arg, &cnt) < 0 || cnt <= 0) {
            cli_out("%sInvalid value for the elements count (%s)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return -1;
        }
        if (!dma_mem_addr_range(laddr, elsz, cnt, laddr_max)) {
            return -1;
        }
    } else {
        cnt = (laddr_max - laddr) / elsz;
    }
    return cnt;
}

/*
 * Find a DMA memory block by logical address or name.
 * The verbose option can be used to control the display of error message
 * if failed to find a valid DMA memory block.
 */
static dma_info_t *
dma_mem_block_parse(int unit, const char *arg, int verbose, uint64_t *laddr)
{
    dma_info_t *di;
    bool indexed_by_laddr = false;

    di = dma_info_list_find(unit, arg, laddr, &indexed_by_laddr);
    if (di == NULL && verbose) {
        cli_out("%sFailed to find the DMA memory block %s %s\n",
                BCMA_CLI_CONFIG_ERROR_STR,
                indexed_by_laddr ? "with logical address" : "named", arg);
    }
    return di;
}

/*
 * Allocate cache-coherent or non cache-coherent DMA memory block,
 * add the allocated information to CLI variables and
 * add the block to dma_info_list.
 */
static int
dma_mem_alloc(bcma_cli_t *cli, bcma_cli_args_t *args, bool cache_coherent)
{
    int unit = cli->cmd_unit;
    const char *arg;
    int elsz, cnt, size;
    const char *mem_name = NULL;
    dma_info_t *di;
    shr_pb_t *pb;
    uint64_t laddr64;

    elsz = dma_mem_acc_type_parse(args);

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Parse the size of the allocated memory element */
    if (bcma_cli_parse_int(arg, &cnt) < 0 || cnt <= 0) {
        cli_out("%sFailed to parse the allocated elements count (%s)\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Check if the name of the DMA memory block is specified */
    if ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        /*
         * A scalar is not accepted as a DMA memory block name.
         * It would be ambiguous to a logical address.
         */
        if (bcma_cli_parse_is_int(arg)) {
            cli_out("%sInvalid DMA memory block name: %s (scalar)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return BCMA_CLI_CMD_FAIL;
        }
        if ((sal_strlen(arg) + 1) > MAX_DMA_INFO_NAME_SIZE) {
            cli_out("%sInvalid DMA memory block name: %s (too long)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return BCMA_CLI_CMD_FAIL;
        }
        /* Check if the specified name is already in the list */
        if ((di = dma_info_list_find(unit, arg, NULL, NULL)) != NULL) {
            cli_out("%sAlready allocated a DMA memory block named %s:\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            dma_info_show(di, true);
            return BCMA_CLI_CMD_FAIL;
        }
        mem_name = arg;
    }

    size = elsz * cnt;
    di = dma_info_alloc(unit, mem_name, size, cache_coherent);
    if (di == NULL) {
        cli_out("%sFailed to allocate %sDMA memory (%d bytes)\n",
                BCMA_CLI_CONFIG_ERROR_STR,
                cache_coherent ? "cache-coherent " : "", size);
        return BCMA_CLI_CMD_FAIL;
    }
    dma_info_list_add(unit, di);
    dma_info_var_add(cli, di);

    pb = shr_pb_create();
    laddr64 = (uintptr_t)di->laddr;
    cli_out("Allocated %d bytes of %sDMA memory at address %s\n",
            size, cache_coherent ? "cache-coherent " : "",
            shr_pb_format_uint64(pb, NULL, &laddr64, 1, 0));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

/*
 * Handler for command 'dma ccalloc'.
 */
static int
bcmdrdcmd_dma_alloc(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return dma_mem_alloc(cli, args, true);
}

/*
 * Handler for command 'dma free'.
 */
static int
bcmdrdcmd_dma_free(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    const char *arg;
    dma_info_t *di = NULL;
    uint64_t laddr = 0;

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcmp(arg, "all") == 0) {
        /* Free all DMA memory blocks. */
        dma_info_list_free(unit);
        return BCMA_CLI_CMD_OK;
    }

    di = dma_mem_block_parse(unit, arg, true, &laddr);
    if (di == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    /*
     * Only accept the to-be-freed address as the starting address of
     * the memory block.
     */
    if (laddr != (uint64_t)((uintptr_t)di->laddr)) {
        cli_out("%sInvalid logical address for the \'dma free\' command\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }
    dma_info_list_del(unit, di);
    dma_info_free(unit, di);
    return BCMA_CLI_CMD_OK;
}

/*
 * Handler for command 'dma load'.
 */
static int
bcmdrdcmd_dma_load(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    const char *arg;
    dma_info_t *di;
    int elsz;
    uint64_t laddr = 0, val = 0, laddr_max;

    if (BCMA_CLI_ARG_CNT(args) < 2) {
        return BCMA_CLI_CMD_USAGE;
    }
    /* Memory load address */
    arg =BCMA_CLI_ARG_GET(args);
    di = dma_mem_block_parse(unit, arg, true, &laddr);
    if (di == 0) {
        return BCMA_CLI_CMD_FAIL;
    }
    laddr_max = (uintptr_t)di->laddr + di->size;

    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_cmp("HEXdigit", arg, 0)) {
        if (BCMA_CLI_ARG_CNT(args) == 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        /* Load the HEX data per byte in LSB order */
        while ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
            while (*arg) {
                if (sal_strchr(" -,_", *arg)) {
                    arg++;
                } else {
                    if (!sal_isxdigit(*arg) || !sal_isxdigit(*(arg + 1))) {
                        cli_out("%sInvalid HEX data (%s)\n",
                                BCMA_CLI_CONFIG_ERROR_STR, arg);
                        return BCMA_CLI_CMD_FAIL;
                    }
                    if (laddr >= laddr_max) {
                        cli_out("Stopped loading at %s (out of range)\n", arg);
                        return BCMA_CLI_CMD_OK;
                    }
                    val = (xdigit2i(*arg) << 4) | xdigit2i(*(arg + 1));
                    dma_mem_addr_write(laddr, 1, val);
                    arg += 2;
                    laddr++;
                }
            }
        }
    } else {
        /* The data is specified per the supported unit */
        BCMA_CLI_ARG_PREV(args);
        /* Memory load element */
        elsz = dma_mem_acc_type_parse(args);
        if (!dma_mem_addr_aligned(laddr, elsz)) {
            return BCMA_CLI_CMD_FAIL;
        }
        if (BCMA_CLI_ARG_CNT(args) == 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        /*
         * Check whether the data is specified in one single value
         * (long word started with 0x or 0X)
         */
        if (elsz == 1 && BCMA_CLI_ARG_CNT(args) == 1) {
            char *str = BCMA_CLI_ARG_GET(args);
            uint8_t *arr;
            int arr_max, idx;
            int rv = BCMA_CLI_CMD_OK;

            arr_max = bcma_cli_parse_array_uint8(str, 0, NULL);
            if (arr_max <= 0) {
                cli_out("%sFailed to load data: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, str);
                return BCMA_CLI_CMD_FAIL;
            }
            arr = sal_alloc(arr_max, "bcmaCliDmaLoad");
            if (arr == NULL) {
                cli_out("%sFailed to allocate memory\n", BCMA_CLI_CONFIG_ERROR_STR);
                return BCMA_CLI_CMD_FAIL;
            }
            if (bcma_cli_parse_array_uint8(str, arr_max, arr) < 0) {
                cli_out("%sFailed to load data: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, str);
                rv = BCMA_CLI_CMD_FAIL;
            } else {
                for (idx = 0; idx < arr_max; idx++, laddr++) {
                    if (laddr >= laddr_max) {
                        cli_out("Stopped loading at byte 0x%"PRIx32
                                " (out of range)\n", arr[idx]);
                        break;
                    }
                    dma_mem_addr_write(laddr, 1, arr[idx]);
                }
            }
            sal_free(arr);
            return rv;
        } else {
            /* Load the data per data unit */
            while ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
                if (bcma_cli_parse_uint64(arg, &val) < 0) {
                    cli_out("%sInvalid value for memory load (%s)\n",
                            BCMA_CLI_CONFIG_ERROR_STR, arg);
                    return BCMA_CLI_CMD_FAIL;
                }
                if (val >= (1ULL << (elsz * 8))) {
                    cli_out("%sInvalid value for loading %d-byte data "
                            "(0x%"PRIx64")\n",
                            BCMA_CLI_CONFIG_ERROR_STR, elsz, val);
                    return BCMA_CLI_CMD_FAIL;
                }
                if (laddr >= laddr_max) {
                    cli_out("Stopped loading at %s (out of range)\n", arg);
                    return BCMA_CLI_CMD_OK;
                }
                dma_mem_addr_write(laddr, elsz, val);
                laddr += elsz;
            }
        }
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Handler for command 'dma fill'.
 */
static int
bcmdrdcmd_dma_fill(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    const char *arg;
    dma_info_t *di;
    int elsz, idx, cnt = 0;
    uint64_t laddr = 0, val = 0;

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    /* Memory fill address */
    di = dma_mem_block_parse(unit, arg, true, &laddr);
    if (di == 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Memory fill element */
    elsz = dma_mem_acc_type_parse(args);
    if (!dma_mem_addr_aligned(laddr, elsz)) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Value for memory fill */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    if (bcma_cli_parse_uint64(arg, &val) < 0) {
        cli_out("%sInvalid value for memory fill (%s)\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }
    if (val >= (1ULL << (elsz * 8))) {
        cli_out("%sInvalid value for filling %d-byte data (0x%"PRIx64")\n",
                BCMA_CLI_CONFIG_ERROR_STR, elsz, val);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Elements count for memory fill */
    cnt = dma_mem_unit_count_parse(args, di, laddr, elsz);
    if (cnt <= 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    for (idx = 0; idx < cnt; idx++) {
        dma_mem_addr_write(laddr, elsz, val);
        laddr += elsz;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Handler for command 'dma edit'.
 */
static int
bcmdrdcmd_dma_edit(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    const char *arg;
    dma_info_t *di;
    int elsz;
    uint64_t laddr = 0, laddr_min, laddr_max;
    uint64_t val;
    char buf[80];
    shr_pb_t *pb;

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    /* Memory edit address */
    di = dma_mem_block_parse(unit, arg, true, &laddr);
    if (di == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Memory edit element */
    elsz = dma_mem_acc_type_parse(args);
    if (!dma_mem_addr_aligned(laddr, elsz)) {
        return BCMA_CLI_CMD_FAIL;
    }

    laddr_min = (uintptr_t)di->laddr;
    laddr_max = laddr_min + di->size;

    pb = shr_pb_create();

    shr_pb_printf(pb, "Editing memory block %s(",
                  di->name[0] == '\0' ? "" : di->name);
    shr_pb_format_uint64(pb, NULL, &laddr_min, 1, 0);
    shr_pb_printf(pb, " ~ ");
    shr_pb_format_uint64(pb, NULL, &laddr_max, 1, 0);
    shr_pb_printf(pb, ")\n");
    shr_pb_printf(pb,
                  "Available commands:\n"
                  "    '.', 'q'      -- Exit\n"
                  "    '-'           -- Go to previous memory unit address\n"
                  "    '<Enter>      -- Go to next memory unit address\n"
                  "    'b', 'h', 'w' -- Edit in bytes, half-words, or words\n");
    cli_out("%s", shr_pb_str(pb));

    do {
        /* Get current address and data */
        shr_pb_reset(pb);
        shr_pb_format_uint64(pb, NULL, &laddr, 1, 0);
        shr_pb_printf(pb, " ");
        if (elsz == 1) {
            val = *(uint8_t *)((uintptr_t)laddr);
            shr_pb_printf(pb, "[0x%02"PRIx64"] ", val);
        } else if (elsz == 2) {
            val = *(uint16_t *)((uintptr_t)laddr);
            shr_pb_printf(pb, "[0x%04"PRIx64"] ", val);
        } else {
            val = *(uint32_t *)((uintptr_t)laddr);
            shr_pb_printf(pb, "[0x%08"PRIx64"] ", val);
        }
        /* Get CLI input for the modified data or edit commands */
        rv = bcma_cli_gets(cli, shr_pb_str(pb), sizeof(buf), buf);
        if (rv != BCMA_CLI_CMD_OK) {
            if (rv == BCMA_CLI_CMD_INTR) {
                break;
            }
            continue;
        }
        /* Got to the next memory unit for <Enter> */
        if (buf[0] == '\0') {
            laddr += elsz;
            if (laddr >= laddr_max) {
                laddr = laddr_max - elsz;
            }
            continue;
        }
        /* Check for available commands */
        if (sal_strlen(buf) == 1) {
            if (sal_strchr(".qQ", buf[0])) {
                /* Quit */
                break;
            }
            if (buf[0] == '-') {
                /* Previous address unit */
                laddr -= elsz;
                if (laddr < laddr_min) {
                    laddr = laddr_min;
                }
                continue;
            } else if (sal_strchr("whbWHB", buf[0])) {
                /* Address unit */
                if (buf[0] == 'w' || buf[0] == 'W') {
                    elsz = 4;
                } else if (buf[0] == 'h' || buf[0] == 'H') {
                    elsz = 2;
                } else {
                    elsz = 1;
                }
                /* Address alignment for unit size */
                laddr &= ~((uint64_t)elsz - 1);
                continue;
            }
        }
        /* Parse the input value */
        if (bcma_cli_parse_uint64(buf, &val) < 0) {
            cli_out("%sInvalid value for memory edit (%s)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, buf);
            continue;
        }
        if (val >= (1ULL << (elsz * 8))) {
            cli_out("%sInvalid value for editing %d-byte data (0x%"PRIx64")\n",
                    BCMA_CLI_CONFIG_ERROR_STR, elsz, val);
            continue;
        }
        /* Update the modified data */
        dma_mem_addr_write(laddr, elsz, val);
        laddr += elsz;
        if (laddr >= laddr_max) {
            laddr = laddr_max - elsz;
        }
    } while (1);

    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

/*
 * Handler for command 'dma dump'.
 */
static int
bcmdrdcmd_dma_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    const char *arg;
    dma_info_t *di;
    int elsz, idx, cnt = 0, idx_max;
    uint64_t laddr = 0, laddr_end;
    shr_pb_t *pb;

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    /* Memory dump address */
    di = dma_mem_block_parse(unit, arg, true, &laddr);
    if (di == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Memory dump element */
    elsz = dma_mem_acc_type_parse(args);
    if (!dma_mem_addr_aligned(laddr, elsz)) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Elements count for memory dump */
    cnt = dma_mem_unit_count_parse(args, di, laddr, elsz);
    if (cnt <= 0) {
        return BCMA_CLI_CMD_FAIL;
    }
    laddr_end = laddr + elsz * cnt;

    pb = shr_pb_create();

    if (elsz == 1 || elsz == 2) {
        idx_max = 7;
    } else {
        idx_max = 3;
    }
    idx = 0;

    while (laddr < laddr_end) {
        if (idx == 0) {
            shr_pb_reset(pb);
            shr_pb_format_uint64(pb, NULL, &laddr, 1, 0);
            shr_pb_printf(pb, ":  ");
        }
        if (elsz == 1) {
            shr_pb_printf(pb, "0x%02x", *(uint8_t *)((uintptr_t)laddr));
        } else if (elsz == 2) {
            shr_pb_printf(pb, "0x%04x", *(uint16_t *)((uintptr_t)laddr));
        } else {
            shr_pb_printf(pb, "0x%08x", *(uint32_t *)((uintptr_t)laddr));
        }
        idx = (idx + 1) & idx_max;
        if (idx > 0) {
            shr_pb_printf(pb, "  ");
        } else {
            cli_out("%s\n", shr_pb_str(pb));
        }
        laddr += elsz;
    }
    if (idx != 0) {
        cli_out("%s\n", shr_pb_str(pb));
    }

    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

/*
 * Handler for command 'dma info'.
 */
static int
bcmdrdcmd_dma_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;

    dma_info_list_show(unit);

    return BCMA_CLI_CMD_OK;
}

/* Sub-commands and dispatch functions for the CLI dma command */
static bcma_cli_command_t dma_cmds[] = {
    { "alloc",    bcmdrdcmd_dma_alloc   },
    { "free",     bcmdrdcmd_dma_free    },
    { "load",     bcmdrdcmd_dma_load    },
    { "fill",     bcmdrdcmd_dma_fill    },
    { "edit",     bcmdrdcmd_dma_edit    },
    { "dump",     bcmdrdcmd_dma_dump    },
    { "info",     bcmdrdcmd_dma_info    }
};

int
bcma_bcmdrdcmd_dma_cleanup(bcma_cli_t *cli)
{
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        dma_info_list_free(unit);
    }
    return 0;
}

int
bcma_bcmdrdcmd_dma(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int idx;
    const char *arg;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    for (idx = 0; idx < COUNTOF(dma_cmds); idx++) {
        if (bcma_cli_parse_cmp(dma_cmds[idx].name, arg, 0)) {
            return (*dma_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}
