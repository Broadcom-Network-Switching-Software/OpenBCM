/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	dma.c
 * Purpose: 	Commands for low-level DMA-able memory control
 *
 * code taken from esw
 */

#include <shared/bsl.h>

#include <sal/types.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/error.h>

#include <appl/diag/parse.h>
#include <appl/diag/system.h>

#define CMD_SAND_DMA_LADDR      "dma_laddr"
#define CMD_SAND_DMA_PADDR      "dma_paddr"
#define CMD_SAND_DMA_SIZE       "dma_size"
#define CMD_SAND_DMA_COUNT      "dma_count"
#define CMD_SAND_DMA_BYTES      "dma_bytes"
#define CMD_SAND_DMA_SHORTS     "dma_shorts"
#define CMD_SAND_DMA_WORDS      "dma_words"
#define CMD_SAND_DMA_MAX_LEN    16

char cmd_sand_dma_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: dma <option> [args...]\n"
#else
    "Usages:\n\t"
    "\n\t"
    "  dma alloc [b|h|w] <count> [name]\n\t"
    "            Allocate DMA-able memory (you can add region name)\n\t"
    "  dma free  [<laddr>]\n\t"
    "            Free DMA-able memory\n\t"
    "  dma l2p   [<laddr>]\n\t"
    "            Convert logical (CPU) address into a physical (bus) address\n\t"
    "  dma p2l   [<paddr>]\n\t"
    "            Convert physical (bus) address into a logical (CPU) address\n\t"
    "  dma flush [b|h|w] [<laddr> [<count>]]\n\t"
    "            Flush the cache corresponding to the DMA-able memory region\n\t"
    "  dma inval [b|h|w] [<laddr> [<count>]]\n\t"
    "            Invalidate the cache corresponding to the DMA-able memory region\n\t"
    "  dma load  <laddr> <hex data>\n\t"
    "            Fill DMA-able memory with provided hex data\n\t"
    "  dma fill  [b|h|w] <laddr> <count> <value>\n\t"
    "            Fill DMA-able memory with provided value\n\t"
    "  dma edit  [b|h|w] [<laddr>]\n\t"
    "            Interactive memory editing\n\t"
    "\nNote:\n\t"
    "The following variables contain the results of the subcommands and\n\t"
    "be used as default arguments:\n\t\t"
    "$" CMD_SAND_DMA_LADDR "\t -- the logical  address of DMA-able memory\n\t\t"
    "$" CMD_SAND_DMA_PADDR "\t -- the physical address of DMA-able memory\n\t\t"
    "$" CMD_SAND_DMA_SIZE "\t -- the size of the element (b, h, or w)\n\t\t"
    "$" CMD_SAND_DMA_COUNT "\t -- the number of elements (bytes, shorts, words)\n\t\t"
    "$" CMD_SAND_DMA_BYTES "\t -- number of bytes in the allocated region\n\t\t"
    "$" CMD_SAND_DMA_SHORTS "\t -- number of shorts in the allocated region\n\t\t"
    "$" CMD_SAND_DMA_WORDS "\t -- number of words in the allocated region\n\t\t" "\n"
#endif
    ;

STATIC cmd_result_t
cmd_sand_dma_get_size(
    int unit,
    args_t * a,
    int *size)
{
    char *sz;

    if (size == NULL)
    {
        return CMD_FAIL;
    }

    if ((sz = ARG_GET(a)) != NULL)
    {
        switch (sz[0])
        {
            case 'b':
            case 'B':
                *size = 1;
                return CMD_OK;
            case 'h':
            case 'H':
                *size = 2;
                return CMD_OK;
            case 'w':
            case 'W':
                *size = 4;
                return CMD_OK;
            default:
                ARG_PREV(a);
        }
    }

    if ((sz = var_get(CMD_SAND_DMA_SIZE)) == NULL)
    {
        *size = 1;      /* Default are bytes */
        return CMD_OK;
    }
    else
    {
        switch (sz[0])
        {
            case 'b':
            case 'B':
                *size = 1;
                return CMD_OK;
            case 'h':
            case 'H':
                *size = 2;
                return CMD_OK;
            case 'w':
            case 'W':
                *size = 4;
                return CMD_OK;
            default:
                cli_out("Incorrect size specification <%s>\n", sz);
                return CMD_FAIL;
        }
    }

    return CMD_FAIL;
}

STATIC cmd_result_t
cmd_sand_dma_get_count(
    int unit,
    args_t * a,
    int *count)
{
    char *arg;

    if (count == NULL)
    {
        return CMD_FAIL;
    }

    /*
     * Parse the number of bytes to allocate 
     */
    if ((arg = ARG_GET(a)) == NULL && (arg = var_get(CMD_SAND_DMA_COUNT)) == NULL)
    {
        return CMD_FAIL;
    }

    *count = parse_integer(arg);

    return CMD_OK;
}

STATIC void
cmd_sand_dma_set_count_size(
    uint32 count,
    uint32 size)
{

    var_unset(CMD_SAND_DMA_BYTES, TRUE, FALSE, FALSE);
    var_unset(CMD_SAND_DMA_SHORTS, TRUE, FALSE, FALSE);
    var_unset(CMD_SAND_DMA_WORDS, TRUE, FALSE, FALSE);

    switch (size)
    {
        case 1:
            var_set(CMD_SAND_DMA_SIZE, "b", TRUE, FALSE);
            var_set_integer(CMD_SAND_DMA_BYTES, count, TRUE, FALSE);
            break;
        case 2:
            var_set(CMD_SAND_DMA_SIZE, "h", TRUE, FALSE);
            var_set_integer(CMD_SAND_DMA_SHORTS, count, TRUE, FALSE);
            break;
        case 4:
            var_set(CMD_SAND_DMA_SIZE, "w", TRUE, FALSE);
            var_set_integer(CMD_SAND_DMA_WORDS, count, TRUE, FALSE);
            break;
        default:
            var_set(CMD_SAND_DMA_SIZE, "?", TRUE, FALSE);
            break;
    }

}

STATIC cmd_result_t
cmd_sand_dma_get_laddr(
    int unit,
    args_t * a,
    void **laddr)
{
    char *arg;

    if (laddr == NULL)
    {
        return CMD_FAIL;
    }

    /*
     * Parse the logical address 
     */
    if ((arg = ARG_GET(a)) == NULL && (arg = var_get(CMD_SAND_DMA_LADDR)) == NULL)
    {
        return CMD_FAIL;
    }

    *laddr = (void *) parse_address(arg);

    return CMD_OK;
}

STATIC void
cmd_sand_dma_set_laddr(
    void *laddr)
{
    char buf[20];

    sal_snprintf(buf, CMD_SAND_DMA_MAX_LEN, "%p", laddr);
    var_set(CMD_SAND_DMA_LADDR, buf, TRUE, FALSE);
}

STATIC cmd_result_t
cmd_sand_dma_get_paddr(
    int unit,
    args_t * a,
    uint32 *paddr)
{
    char *arg;

    if (paddr == NULL)
    {
        return CMD_FAIL;
    }

    /*
     * Parse the logical address 
     */
    if ((arg = ARG_GET(a)) == NULL && (arg = var_get(CMD_SAND_DMA_PADDR)) == NULL)
    {
        return CMD_FAIL;
    }

    *paddr = parse_address(arg);

    return CMD_OK;
}

STATIC void
cmd_sand_dma_set_paddr(
    uint32 paddr)
{
    char buf[20];

    sal_snprintf(buf, CMD_SAND_DMA_MAX_LEN, "0x%08x", paddr);
    var_set(CMD_SAND_DMA_PADDR, buf, TRUE, FALSE);
}

/*
 * Subcommands 
 */

STATIC cmd_result_t
cmd_sand_dma_alloc(
    int unit,
    args_t * a)
{
    char *arg;
    int size;
    int count;
    int alloc_count;
    void *laddr;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (cmd_sand_dma_get_size(unit, a, &size) != CMD_OK)
    {
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_count(unit, a, &count) != CMD_OK || count == 0)
    {
        return CMD_USAGE;
    }

    /*
     * Get the region name if provided 
     */
    arg = ARG_GET(a);

    /*
     * Allocate DMA-able memory 
     */
    alloc_count = size * count;
    laddr = soc_cm_salloc(unit, alloc_count, arg);
    if (laddr == NULL)
    {
        cli_out("Failed to allocate %d bytes of DMA-able memory\n", alloc_count);
        return CMD_FAIL;
    }

    /*
     * Print the results and set the variable for others to reference 
     */
    cli_out("Allocated %d bytes of DMA-able memory at address %p\n", alloc_count, laddr);

    cmd_sand_dma_set_laddr(laddr);
    cmd_sand_dma_set_count_size(count, size);

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_free(
    int unit,
    args_t * a)
{
    void *laddr;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (cmd_sand_dma_get_laddr(unit, a, &laddr) != CMD_OK)
        if (laddr == NULL)
        {
            return CMD_FAIL;
        }

    soc_cm_sfree(unit, laddr);

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_l2p(
    int unit,
    args_t * a)
{
    void *laddr;
    uint32 paddr;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (cmd_sand_dma_get_laddr(unit, a, &laddr) != CMD_OK)
    {
        return CMD_USAGE;
    }

    paddr = soc_cm_l2p(unit, laddr);

    cli_out("l2p(%p) = 0x%08x\n", laddr, paddr);

    cmd_sand_dma_set_paddr(paddr);

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_p2l(
    int unit,
    args_t * a)
{
    uint32 paddr;
    void *laddr;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (cmd_sand_dma_get_paddr(unit, a, &paddr) != CMD_OK)
    {
        return CMD_USAGE;
    }

    laddr = soc_cm_p2l(unit, paddr);

    cli_out("p2l(0x%08x) = %p\n", paddr, laddr);

    cmd_sand_dma_set_laddr(laddr);

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_flush(
    int unit,
    args_t * a)
{
    int size;
    int count;
    void *laddr;
    int flush_count;
    int rv;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (cmd_sand_dma_get_size(unit, a, &size) != CMD_OK)
    {
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_laddr(unit, a, &laddr) != CMD_OK)
    {
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_count(unit, a, &count) != CMD_OK)
    {
        return CMD_USAGE;
    }

    flush_count = size * count;

    rv = soc_cm_sflush(unit, laddr, flush_count);
    cli_out("Flushing %d bytes starting at %p: %s\n", flush_count, laddr, soc_errmsg(rv));
    if (SOC_FAILURE(rv))
    {
        return CMD_FAIL;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_inval(
    int unit,
    args_t * a)
{
    int size;
    int count;
    void *laddr;
    int inval_count;
    int rv;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (cmd_sand_dma_get_size(unit, a, &size) != CMD_OK)
    {
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_laddr(unit, a, &laddr) != CMD_OK)
    {
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_count(unit, a, &count) != CMD_OK)
    {
        return CMD_USAGE;
    }

    inval_count = size * count;

    rv = soc_cm_sinval(unit, laddr, inval_count);
    cli_out("Invalidating %d bytes starting at %p: %s\n", inval_count, laddr, soc_errmsg(rv));
    if (SOC_FAILURE(rv))
    {
        return CMD_FAIL;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_load(
    int unit,
    args_t * a)
{
    void *laddr_v;
    uint8 *laddr;
    char *cp;

    if (cmd_sand_dma_get_laddr(unit, a, &laddr_v) != CMD_OK)
    {
        return CMD_USAGE;
    }

    laddr = (uint8 *) laddr_v;

    while ((cp = ARG_GET(a)) != NULL)
    {
        while (*cp)
        {
            if (isspace((int) (*cp)))
            {
                cp++;
            }
            else
            {
                if (!isxdigit((unsigned) *cp) || !isxdigit((unsigned) *(cp + 1)))
                {
                    cli_out("%s: Invalid character\n", ARG_CMD(a));
                    return (CMD_FAIL);
                }

                *laddr++ = (xdigit2i(*cp) << 4) | xdigit2i(*(cp + 1));
                cp += 2;
            }
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_fill(
    int unit,
    args_t * a)
{
    char *arg;
    int size;
    void *laddr;
    int count;
    uint32 val;
    int i;

    if (cmd_sand_dma_get_size(unit, a, &size) != CMD_OK)
    {
        cli_out("Incorrect size specification\n");
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_laddr(unit, a, &laddr) != CMD_OK)
    {
        cli_out("Invalid logical address specified\n");
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_count(unit, a, &count) != CMD_OK)
    {
        cli_out("Invalid count specified\n");
        return CMD_USAGE;
    }

    if ((arg = ARG_GET(a)) == NULL)
    {
        cli_out("No fill value specified\n");
        return CMD_USAGE;
    }

    val = parse_integer(arg);

    switch (size)
    {
        case 1:
            sal_memset(laddr, val, count);
            break;
        case 2:
            for (i = 0; i < count; i++)
            {
                ((uint16 *) laddr)[i] = (uint16) val;
            }
            break;
        case 4:
            for (i = 0; i < count; i++)
            {
                ((uint32 *) laddr)[i] = val;
            }
            break;
        default:
            return CMD_USAGE;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sand_dma_edit(
    int unit,
    args_t * a)
{
    void *laddr_v;
    uint8 *laddr;
    int size;
    uint32 val;
    char prompt[40];
    char dfl_str[40];

    if (cmd_sand_dma_get_size(unit, a, &size) != CMD_OK)
    {
        return CMD_USAGE;
    }

    if (cmd_sand_dma_get_laddr(unit, a, &laddr_v) != CMD_OK)
    {
        return CMD_USAGE;
    }

    cli_out("Editing memory starting at %p\n", laddr_v);
    cli_out("Available commands:\n"
            "\t'.', 'q' -- Exit\n"
            "\t'-'      -- Go to the previous byte/word/halfword\n"
            "\t'<ENTER> -- Go to the next byte/word/halfword\n"
            "\t'b'      -- Edit bytes\n" "\t'h'      == Edit half-words\n" "\t'w'      -- Edit words\n\n");

    laddr = laddr_v;

    for (;;)
    {
        sal_snprintf(prompt, CMD_SAND_DMA_MAX_LEN, "0x%08x ", PTR_TO_INT(laddr));
        switch (size)
        {
            case 1:
                val = *(uint8 *) laddr;
                sal_snprintf(dfl_str, CMD_SAND_DMA_MAX_LEN, "0x%02x", val);
                break;
            case 2:
                val = *(uint16 *) laddr;
                sal_snprintf(dfl_str, CMD_SAND_DMA_MAX_LEN, "0x%04x", val);
                break;
            case 4:
                val = *(uint32 *) laddr;
                sal_snprintf(dfl_str, CMD_SAND_DMA_MAX_LEN, "0x%08x", val);
                break;
        }

        if (sal_readline(prompt, prompt, sizeof(prompt), dfl_str) == 0 || prompt[0] == 0)
        {
            cli_out("Aborted\n");
            break;
        }

        if (prompt[0] == '.' || prompt[0] == 'Q' || prompt[0] == 'q')
        {
            break;
        }

        switch (prompt[0])
        {
            case '-':
                laddr -= size;
                continue;
                break;
            case 'W':
            case 'w':
                size = 4;
                continue;
                break;
            case 'H':
            case 'h':
                size = 2;
                continue;
                break;
            case 'B':
            case 'b':
                size = 1;
                continue;
                break;
            default:
                val = parse_integer(prompt);
                break;
        }

        switch (size)
        {
            case 1:
                *(uint8 *) laddr = val;
                break;
            case 2:
                *(uint16 *) laddr = val;
                break;
            case 4:
                *(uint32 *) laddr = val;
                break;
        }

        laddr += size;
    }

    return CMD_OK;
}

cmd_result_t
cmd_sand_dma(
    int unit,
    args_t * a)
{
    char *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL)
    {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "alloc") == 0)
    {
        return cmd_sand_dma_alloc(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "free") == 0)
    {
        return cmd_sand_dma_free(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "l2p") == 0)
    {
        return cmd_sand_dma_l2p(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "p2l") == 0)
    {
        return cmd_sand_dma_p2l(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "flush") == 0)
    {
        return cmd_sand_dma_flush(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "inval") == 0)
    {
        return cmd_sand_dma_inval(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "fill") == 0)
    {
        return cmd_sand_dma_fill(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "load") == 0)
    {
        return cmd_sand_dma_load(unit, a);
    }
    else if (sal_strcasecmp(subcmd, "edit") == 0)
    {
        return cmd_sand_dma_edit(unit, a);
    }

    cli_out("Unrecognized subcommand <%s>\n", subcmd);

    return CMD_USAGE;
}
