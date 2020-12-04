/*! \file bcma_bslcmd_debug.c
 *
 * CLI BSL shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bsl/bsl_names.h>

#include <sal/sal_libc.h>

#include <shr/shr_bitop.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bsl/bcma_bslmgmt.h>
#include <bcma/bsl/bcma_bslenable.h>
#include <bcma/bsl/bcma_bslsink.h>
#include <bcma/bsl/bcma_bslcons.h>

#include <bcma/bsl/bcma_bslcmd_debug.h>


typedef enum {
    CMD_OPT_SET,
    CMD_OPT_SET_ALL,
    CMD_OPT_CLR_ALL,
    CMD_OPT_RST,
    CMD_OPT_TOGGLE,
    CMD_OPT_SHOW,
    CMD_OPT_COUNT
} cmd_option_t;

typedef enum {
    OFMT_LAYER,
    OFMT_SOURCE,
    OFMT_SEVERITY,
    OFMT_SEVERITY_END,
    OFMT_COUNT
} output_format_t;

typedef struct {
    uint8_t layer_list[BSL_LAY_COUNT];
    uint8_t layer_count;
} layer_list_t;

typedef struct {
    uint8_t source_list[BSL_LAY_COUNT][BSL_SRC_COUNT];
    uint8_t source_count[BSL_LAY_COUNT];
} source_list_t;

/*
 * Function:
 *      sh_bsl_list_next
 * Purpose:
 *      Find the bsl next list item.
 * Parameters:
 *      context - list context
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_NOT_FOUND if item not found.
 * Notes:
 */
static int
sh_bsl_list_next(char **context)
{
    char *delimiter;

    if ((delimiter = sal_strchr(*context, ',')) != NULL) {
        *context = delimiter + 1;
        return BCMA_CLI_CMD_OK;
    }

    return BCMA_CLI_CMD_NOT_FOUND;
}

/*
 * Function:
 *      sh_bsl_parse_option
 * Purpose:
 *      Parse the option from the arguments.
 * Parameters:
 *      arg - arguments
 *      option - bsl option.
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_NOT_FOUND if option not found.
 * Notes:
 */
static int
sh_bsl_parse_option(char *arg, cmd_option_t *option)
{
    if (sal_strcasecmp(arg, "+") == 0) {
        *option = CMD_OPT_SET_ALL;
    } else if (sal_strcasecmp(arg, "-") == 0) {
        *option = CMD_OPT_CLR_ALL;
    } else if (sal_strcasecmp(arg, "=") == 0) {
        *option = CMD_OPT_RST;
    } else {
        return BCMA_CLI_CMD_NOT_FOUND;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function:
 *      sh_bsl_parse_severity
 * Purpose:
 *      Parse the severity from the arguments.
 * Parameters:
 *      arg - arguments
 *      option - bsl severity.
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_NOT_FOUND if severity not found.
 * Notes:
 */
static int
sh_bsl_parse_severity(char *arg, bsl_severity_t *severity)
{
    int i;
        const char *name;

    for (i = 0; i < BSL_SEV_COUNT; i++) {
        name = bsl_severity2str(i);
        if (bcma_cli_parse_cmp(name, arg, '\0')) {
            *severity = i;
            return BCMA_CLI_CMD_OK;
        }
    }

    return BCMA_CLI_CMD_NOT_FOUND;
}

/*
 * Function:
 *      sh_bsl_parse_layer
 * Purpose:
 *      Parse the layer from the arguments.
 * Parameters:
 *      arg - arguments
 *      option - bsl layer.
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_NOT_FOUND if layer not found.
 * Notes:
 */
static int
sh_bsl_parse_layer(char *arg, layer_list_t *layer)
{
    int i;
    int is_found;
    const char *name;

    is_found = FALSE;
    do {
        for (i = 0; i < BSL_LAY_COUNT; i++) {
            name = bsl_layer2str(i);
            if (bcma_cli_parse_cmp(name, arg, ',') ||
                bcma_cli_parse_cmp(name, arg, '\0')) {
                is_found = TRUE;
                layer->layer_list[(layer->layer_count)++] = i;
                break;
            }
        }
    } while (sh_bsl_list_next(&arg) == BCMA_CLI_CMD_OK);

    /* check whether the layer is found */
    if (!is_found) {
        return BCMA_CLI_CMD_NOT_FOUND;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function:
 *      sh_bsl_parse_source
 * Purpose:
 *      Parse the source from the arguments.
 * Parameters:
 *      arg - arguments
 *      option - bsl source.
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_NOT_FOUND if source not found.
 * Notes:
 */
static int
sh_bsl_parse_source(char *arg, layer_list_t *layer, source_list_t *source)
{
    int i, j;
    int is_found;
    char *arg_cmp;
    bsl_layer_t lay;
    const char *name;

    is_found = FALSE;
    for (i = 0; i < layer->layer_count; i++) {
        lay = layer->layer_list[i];
        arg_cmp = arg;
        do {
            for (j = 0; j < BSL_SRC_COUNT; j++) {
                if (!bcma_bslenable_source_valid(lay, j)) {
                    continue;
                }
                name = bsl_source2str(j);
                if (bcma_cli_parse_cmp(name, arg_cmp, ',') ||
                    bcma_cli_parse_cmp(name, arg_cmp, '\0')) {
                    is_found = TRUE;
                    source->source_list[lay][source->source_count[lay]] = j;
                    (source->source_count[lay])++;
                    break;
                }
            }
        } while (sh_bsl_list_next(&arg_cmp) == BCMA_CLI_CMD_OK);
    }

    /* check whether the source is found */
    if (!is_found) {
        return BCMA_CLI_CMD_NOT_FOUND;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function:
 *      sh_bsl_enable_source_all
 * Purpose:
 *      Enable all source from the layer list.
 * Parameters:
 *      layer - layer list
 *      source - source list.
 * Returns:
 *      BCMA_CLI_CMD_OK.
 * Notes:
 */
static int
sh_bsl_enable_source_all(layer_list_t *layer, source_list_t *source)
{
    int i, j;
    bsl_layer_t lay;

    for (i = 0; i < layer->layer_count; i++) {
        lay = layer->layer_list[i];
        for (j = 0; j < BSL_SRC_COUNT; j++) {
            if (!bcma_bslenable_source_valid(lay, j)) {
                continue;
            }
            source->source_list[lay][source->source_count[lay]] = j;
            (source->source_count[lay])++;
        }
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function:
 *      sh_bsl_enable_layer_all
 * Purpose:
 *      Enable all layer.
 * Parameters:
 *      layer - layer list
 * Returns:
 *      BCMA_CLI_CMD_OK.
 * Notes:
 */
static int
sh_bsl_enable_layer_all(layer_list_t *layer)
{
    int i;

    for (i = 0; i < BSL_LAY_COUNT; i++) {
        layer->layer_list[(layer->layer_count)++] = i;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function:
 *      sh_bsl_enable_layer_source_all
 * Purpose:
 *      Enable all source for all layer.
 * Parameters:
 *      layer - layer list
 *      source - source list.
 * Returns:
 *      BCMA_CLI_CMD_OK.
 * Notes:
 */
static int
sh_bsl_enable_layer_source_all(layer_list_t *layer, source_list_t *source)
{
    sh_bsl_enable_layer_all(layer);
    sh_bsl_enable_source_all(layer, source);

    return BCMA_CLI_CMD_OK;
}

/*
 * Function:
 *      sh_bsl_parse
 * Purpose:
 *      Parse the bsl command and save it in option, severity, layer list and source list.
 * Parameters:
 *      arg1 - first argument
 *      agr2 - second argument
 *      agr3 - third argument
 *      option - bsl option
 *      layer - bsl layer list
 *      source - bsl source list
 *      severity -bsl severity.
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_USAGE if option/layer/source/severity not found.
 * Notes:
 */
static int
sh_bsl_parse(char *arg1, char *arg2, char *arg3, cmd_option_t *option,
             layer_list_t *layer, source_list_t *source,
             bsl_severity_t *severity)
{
    if (arg1 == NULL) {
        sh_bsl_enable_layer_source_all(layer, source);
        return BCMA_CLI_CMD_OK;
    }
    /* Check whether is bsl show */
    if (*option == CMD_OPT_SHOW) {
        if (sh_bsl_parse_layer(arg1, layer) == BCMA_CLI_CMD_OK) {
            /* Check whether is:  bsl layer  */
            if (arg2 == NULL) {
                sh_bsl_enable_source_all(layer, source);
                return BCMA_CLI_CMD_OK;
            } else if (sh_bsl_parse_source(arg2, layer, source) == BCMA_CLI_CMD_OK) {
                /* Check whether is:  bsl layer source */
                return BCMA_CLI_CMD_OK;
            } else {
                return BCMA_CLI_CMD_USAGE;
            }
        } else {
            sh_bsl_enable_layer_all(layer);
            if (sh_bsl_parse_source(arg1, layer, source) == BCMA_CLI_CMD_OK) {
                /* Check whether is:  bsl source */
                if (arg2 == NULL) {
                    return BCMA_CLI_CMD_OK;
                } else {
                    return BCMA_CLI_CMD_USAGE;
                }
            } else {
                return BCMA_CLI_CMD_USAGE;
            }
        }
    } else {
        if ((sh_bsl_parse_option(arg1, option) == BCMA_CLI_CMD_OK) ||
            (sh_bsl_parse_severity(arg1, severity) == BCMA_CLI_CMD_OK)) {
            /* Check whether is:  bsl <severity|+|-|=>  */
            if ((arg2 != NULL) || (arg3 != NULL)) {
                return BCMA_CLI_CMD_USAGE;
            }
            sh_bsl_enable_layer_source_all(layer, source);
            return BCMA_CLI_CMD_OK;
        } else if (sh_bsl_parse_layer(arg1, layer) == BCMA_CLI_CMD_OK) {
            /* Check whether is:  bsl layer  */
            if (arg2 == NULL) {
                /* Show current settings if only layer is specified */
                *option = CMD_OPT_SHOW;
                sh_bsl_enable_source_all(layer, source);
                return BCMA_CLI_CMD_OK;
            } else if ((sh_bsl_parse_option(arg2, option) == BCMA_CLI_CMD_OK) ||
                (sh_bsl_parse_severity(arg2, severity) == BCMA_CLI_CMD_OK)) {
                /* Check whether is:  bsl layer <+|-|=> */
                if (arg3 != NULL) {
                    return BCMA_CLI_CMD_USAGE;
                }
                sh_bsl_enable_source_all(layer, source);
                return BCMA_CLI_CMD_OK;
            } else if (sh_bsl_parse_source(arg2, layer, source) == BCMA_CLI_CMD_OK) {
                /* Check whether is:  bsl layer source */
                if (arg3 == NULL) {
                    *option = CMD_OPT_TOGGLE;
                    return BCMA_CLI_CMD_OK;
                } else if ((sh_bsl_parse_option(arg3, option) == BCMA_CLI_CMD_OK) ||
                    (sh_bsl_parse_severity(arg3, severity) == BCMA_CLI_CMD_OK)) {
                    /* Check whether is:  bsl layer source <severity|+|-|=> */
                    return BCMA_CLI_CMD_OK;
                } else {
                    return BCMA_CLI_CMD_USAGE;
                }
            } else {
                return BCMA_CLI_CMD_USAGE;
            }
        } else {
            sh_bsl_enable_layer_all(layer);
            if (sh_bsl_parse_source(arg1, layer, source) == BCMA_CLI_CMD_OK) {
                /* Check whether is:  bsl source */
                if (arg2 == NULL) {
                    *option = CMD_OPT_TOGGLE;
                    return BCMA_CLI_CMD_OK;
                } else if ((sh_bsl_parse_option(arg2, option) == BCMA_CLI_CMD_OK) ||
                    (sh_bsl_parse_severity(arg2, severity) == BCMA_CLI_CMD_OK)) {
                    /* Check whether is:  bsl source <severity|+|-|=>  */
                    if (arg3 != NULL) {
                        return BCMA_CLI_CMD_USAGE;
                    }
                    return BCMA_CLI_CMD_OK;
                } else {
                    return BCMA_CLI_CMD_USAGE;
                }
            }
        }
    }

    return BCMA_CLI_CMD_USAGE;
}

/*
 * Function:
 *      sh_bsl_config
 * Purpose:
 *      config the bsl.
 * Parameters:
 *      unit - unit number
 *      option - bsl option
 *      option - bsl layer
 *      option - bsl source
 *      option - bsl severity
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_NOT_FOUND if option not valid.
 * Notes:
 */
static int
sh_bsl_config(int unit, cmd_option_t option, int layer, int source, int severity)
{
    int rv = BCMA_CLI_CMD_OK;

    switch(option) {
    case CMD_OPT_SET:
        bcma_bslenable_set(layer, source, severity);
        break;

    case CMD_OPT_SET_ALL:
        severity = BSL_SEV_VERBOSE;
        bcma_bslenable_set(layer, source, severity);
        break;

    case CMD_OPT_CLR_ALL:
        severity = BSL_SEV_OFF;
        bcma_bslenable_set(layer, source, severity);
        break;

    case CMD_OPT_RST:
        bcma_bslenable_reset(layer, source);
        break;

    case CMD_OPT_TOGGLE:
        if (bcma_bslenable_get(layer, source) >= BSL_SEV_VERBOSE) {
            bcma_bslenable_reset(layer, source);
        } else {
            severity = BSL_SEV_VERBOSE;
            bcma_bslenable_set(layer, source, severity);
        }
        break;

    default:
        rv = BCMA_CLI_CMD_NOT_FOUND;
        break;
    }

    return rv;
}

/*
 * Function:
 *      sh_bsl_print
 * Purpose:
 *      Print the bsl configuration.
 * Parameters:
 *      unit - unit number
 *      out - bsl output type
 *      option - bsl layer
 *      option - bsl source
 *      option - bsl severity
 *      count - source count.
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_NOT_FOUND if output not valid.
 * Notes:
 */
static int
sh_bsl_print(int unit, output_format_t out,
             int layer, int source, int severity, int count)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *name;
    char buf[16];

    switch(out) {
    case OFMT_LAYER:
        name = bsl_layer2str(layer);
        cli_out("* %s\n", name);
        break;

    case OFMT_SOURCE:
        if (count && (count % 8) == 0) {
            cli_out("\n    %8s ", " ");
        }
        name = bsl_source2str(source);
        cli_out("%s ", name);
        break;

    case OFMT_SEVERITY:
        severity = bcma_bslenable_get(layer, source);
        name = bsl_severity2str(severity);
        sal_snprintf(buf, sizeof(buf), "%s:", name);
        buf[sizeof(buf) - 1] = 0;
        cli_out("    %-8s ", buf);
        break;

    case OFMT_SEVERITY_END:
        cli_out("\n");
        break;

    default:
        rv = BCMA_CLI_CMD_NOT_FOUND;
        break;
    }

    return rv;
}

/*
 * Function:
 *      sh_bsl_cfg
 * Purpose:
 *      Config the bsl by the layer list and source list.
 * Parameters:
 *      unit - unit number
 *      option - bsl option
 *      option - bsl layer list
 *      option - bsl source list
 *      option - bsl severity
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_FAIL if configuration failed.
 * Notes:
 */
static int
sh_bsl_cfg(int unit, cmd_option_t option, layer_list_t *layer,
           source_list_t *source, bsl_severity_t severity)
{
    int rv = 0;
    int lay;
    int src;
    int found_lay;
    int found_src;

    for (lay = 0; lay < layer->layer_count; lay++) {
        found_lay = layer->layer_list[lay];
        for (src = 0; src < source->source_count[found_lay]; src++) {
            found_src = source->source_list[found_lay][src];
            rv = sh_bsl_config(unit, option, found_lay, found_src, severity);
            if (rv < 0) {
                return BCMA_CLI_CMD_FAIL;
            }
        }
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function:
 *      sh_bsl_show
 * Purpose:
 *      Show the bsl configuration by the layer list and source list.
 * Parameters:
 *      unit - unit number
 *      option - bsl layer
 *      option - bsl source
 *      option - bsl severity.
 * Returns:
 *      BCMA_CLI_CMD_OK.
 * Notes:
 */
static int
sh_bsl_show(int unit, layer_list_t *layer, source_list_t *source,
            bsl_severity_t severity)
{
    int lay;
    int src;
    int sev;
    int found_lay;
    int found_src;
    int count;
    int found_severity;

    for (lay = 0; lay < layer->layer_count; lay++) {
        found_lay = layer->layer_list[lay];
        if (source->source_count[found_lay]) {
            sh_bsl_print(unit, OFMT_LAYER, found_lay, -1, -1, -1);

            for (sev = 0; sev < BSL_SEV_COUNT; sev++) {
                count = 0;
                found_severity = FALSE;

                for (src = 0; src < source->source_count[found_lay]; src++) {
                    found_src = source->source_list[found_lay][src];
                    if ((int)bcma_bslenable_get(found_lay, found_src) == sev) {
                        if (!found_severity) {
                            found_severity = TRUE;
                            sh_bsl_print(unit, OFMT_SEVERITY, found_lay,
                                         found_src, sev, -1);
                        }
                        sh_bsl_print(unit, OFMT_SOURCE, found_lay,
                                     found_src, -1, count++);
                    }
                }
                if (found_severity) {
                    sh_bsl_print(unit, OFMT_SEVERITY_END, -1, -1, -1, -1);
                }
            }
        }
    }

    return BCMA_CLI_CMD_OK;
}

static int
parse_severity_str(int u, const char *sev_str, bsl_severity_t *severity)
{
    bsl_severity_t sev = BSL_SEV_COUNT;

    if (sev_str != NULL && *sev_str) {
        for (sev = 0; sev < BSL_SEV_COUNT; sev++) {
            if (bcma_cli_parse_cmp(bsl_severity2str(sev), sev_str, 0)) {
                *severity = sev;
                break;
            }
        }
        if (sev == BSL_SEV_COUNT) {
            return -1;
        }
    }
    return 0;
}

static int
parse_bit_range(int u, const char *bstr,
                SHR_BITDCL *bit_range, int num_bits)
{
    int bitnum = 0;
    int bstart = -1;
    char ch;
    const char *cptr = bstr;;

    if (bstr == NULL || *bstr == 0) {
        return 0;
    }
    if (sal_strcmp(bstr, "all") == 0) {
        SHR_BITSET_RANGE(bit_range, 0, num_bits);
        return 0;
    }
    SHR_BITCLR_RANGE(bit_range, 0, num_bits);
    do {
        ch = *cptr++;
        if (ch >= '0' && ch <= '9') {
            bitnum = (bitnum * 10) + (ch - '0');
        } else {
            if (bstart >= 0) {
                while (bstart < bitnum) {
                    if (bstart >= num_bits) {
                        return -1;
                    }
                    SHR_BITSET(bit_range, bstart);
                    bstart++;
                }
                bstart = -1;
            }
            if (ch == ',' || ch == 0) {
                if (bitnum >= num_bits) {
                    return -1;
                }
                SHR_BITSET(bit_range, bitnum);
                bitnum = 0;
            } else if (ch == '-') {
                bstart = bitnum;
                bitnum = 0;
            } else {
                return -1;
            }
        }
    } while (ch != 0);

    return 0;
}

static void
sh_bsl_show_bit_range(int u, const char *prefix,
                      SHR_BITDCL *bit_range, int num_bits)
{
    int idx, all;

    cli_out("%s", prefix);
    all = 1;
    for (idx = 0; idx < num_bits; idx++) {
        if (SHR_BITGET(bit_range, idx) == 0) {
            all = 0;
            break;
        }
    }
    if (all) {
        cli_out("all\n");
    } else {
        for (idx = 0; idx < num_bits; idx++) {
            if (idx < (num_bits - 1) &&
                SHR_BITGET(bit_range, idx) &&
                SHR_BITGET(bit_range, idx + 1)) {
                cli_out("%d-", idx);
                idx++;
                while (idx < (num_bits - 1) &&
                       SHR_BITGET(bit_range, idx) &&
                       SHR_BITGET(bit_range, idx + 1)) {
                    idx++;
                }
            }
            if (SHR_BITGET(bit_range, idx)) {
                cli_out("%d ", idx);
            }
        }
        cli_out("\n");
    }
}

int
bcma_bslcmd_debug(bcma_cli_t *cli, bcma_cli_args_t *a)
/*
 * Function:
 *      sh_debug
 * Purpose:
 *      Set shell debug parameters.
 * Parameters:
 *      u - unit number
 *      a - arguments.
 * Returns:
 *      BCMA_CLI_CMD_OK or BCMA_CLI_CMD_USAGE if no match found.
 * Notes:
 */
{
    int rv = BCMA_CLI_CMD_OK;
    int u = -1;
    int idx;
    cmd_option_t option = CMD_OPT_SET;
    char *sub_cmd;
    char *arg1 = NULL;
    char *arg2 = NULL;
    char *arg3 = NULL;
    layer_list_t layer_list;
    source_list_t source_list;
    bsl_severity_t severity;

    if (cli) {
        u = cli->cmd_unit;
    }

    /* Check for sub-commands */
    if ((sub_cmd = BCMA_CLI_ARG_GET(a)) != NULL) {
        if (sal_strcasecmp(sub_cmd, "show") == 0) {
            option = CMD_OPT_SHOW;
        } else if (sal_strcasecmp(sub_cmd, "sink") == 0) {
            bcma_cli_parse_table_t pt;
            bcma_bslsink_sink_t tmp_sink, *cur_sink, *sink;
            char *en_min, *en_max, *pre_min, *pre_max, *pre_cfg;
            char *unit_str, *port_str;
            int sink_id = 0;

            if ((arg1 = BCMA_CLI_ARG_GET(a)) == NULL) {
                cli_out("Id   Name\n");
                do {
                    sink = bcma_bslsink_sink_find_by_id(sink_id++);
                    if (sink != NULL) {
                        cli_out("%-4d %-12s\n",
                                sink->sink_id, sink->name);
                    }
                } while (sink != NULL);
                return BCMA_CLI_CMD_OK;
            }
            if (bcma_cli_parse_int(arg1, &sink_id) == 0) {
                sink = bcma_bslsink_sink_find_by_id(sink_id);
            } else {
                sink = bcma_bslsink_sink_find(arg1);
            }
            if (sink == NULL) {
                cli_out("%s: Sink not found: %s\n", BCMA_CLI_ARG_CMD(a), arg1);
                return BCMA_CLI_CMD_FAIL;
            }

            /* Show sink settings if no additional parameters specified */
            if (BCMA_CLI_ARG_CUR(a) == NULL) {
                char range_str[32];
                bcma_bslsink_severity_range_t *range;

                cli_out("%-12s %d\n", "Id:", sink->sink_id);
                cli_out("%-12s %s\n", "Name:", sink->name);
                range = &sink->enable_range;
                if (range->min == range->max) {
                    sal_sprintf(range_str, "%s",
                                bsl_severity2str(range->min));
                } else {
                    sal_sprintf(range_str, "%s..%s",
                                bsl_severity2str(range->min),
                                bsl_severity2str(range->max));
                }
                cli_out("%-12s %s\n", "Enable:", range_str);
                range = &sink->prefix_range;
                if (range->min == range->max) {
                    sal_sprintf(range_str, "%s",
                                bsl_severity2str(range->min));
                } else {
                    sal_sprintf(range_str, "%s..%s",
                                bsl_severity2str(range->min),
                                bsl_severity2str(range->max));
                }
                cli_out("%-12s %s, \"%s\"\n", "Prefix:", range_str,
                        sink->prefix_format);
                sal_sprintf(range_str, "%-12s ", "Units:");
                sh_bsl_show_bit_range(u, range_str, sink->units,
                                      BCMA_BSLSINK_MAX_NUM_UNITS);
                sal_sprintf(range_str, "%-12s ", "Ports:");
                sh_bsl_show_bit_range(u, range_str, sink->ports,
                                      BCMA_BSLSINK_MAX_NUM_PORTS);
                cli_out("%-12s %d\n", "Parameter:", sink->xtra);
                return BCMA_CLI_CMD_OK;
            }

            /* Make a local copy of current sink */
            cur_sink = sink;
            sink = &tmp_sink;
            sal_memcpy(sink, cur_sink, sizeof(*sink));

            /* Initialize string variables */
            en_min = NULL;
            en_max = NULL;
            unit_str = NULL;
            port_str = NULL;
            pre_min = NULL;
            pre_max = NULL;
            pre_cfg = NULL;

            bcma_cli_parse_table_init(cli, &pt);
            bcma_cli_parse_table_add(&pt, "EnableMIN", "str", &en_min, NULL);
            bcma_cli_parse_table_add(&pt, "EnableMAX", "str", &en_max, NULL);
            bcma_cli_parse_table_add(&pt, "Units", "str", &unit_str, NULL);
            bcma_cli_parse_table_add(&pt, "Ports", "str", &port_str, NULL);
            bcma_cli_parse_table_add(&pt, "eXtra", "int", &sink->xtra, NULL);
            bcma_cli_parse_table_add(&pt, "PrefixMIN", "str", &pre_min, NULL);
            bcma_cli_parse_table_add(&pt, "PrefixMAX", "str", &pre_max, NULL);
            bcma_cli_parse_table_add(&pt, "PrefixFormat", "str", &pre_cfg, NULL);

            if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
                cli_out("%s: Invalid option: %s\n", BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            rv = parse_severity_str(u, en_min, &sink->enable_range.min);
            if (rv != 0) {
                cli_out("Invalid severity: %s\n", en_min);
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            rv = parse_severity_str(u, en_max, &sink->enable_range.max);
            if (rv != 0) {
                cli_out("Invalid severity: %s\n", en_max);
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            rv = parse_bit_range(u, unit_str, sink->units,
                                 BCMA_BSLSINK_MAX_NUM_UNITS);
            if (rv != 0) {
                cli_out("Invalid units: %s\n", unit_str);
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            rv = parse_bit_range(u, port_str, sink->ports,
                                 BCMA_BSLSINK_MAX_NUM_PORTS);
            if (rv != 0) {
                cli_out("Invalid ports: %s\n", port_str);
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            rv = parse_severity_str(u, pre_min, &sink->prefix_range.min);
            if (rv != 0) {
                cli_out("Invalid severity: %s\n", pre_min);
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            rv = parse_severity_str(u, pre_max, &sink->prefix_range.max);
            if (rv != 0) {
                cli_out("Invalid severity: %s\n", pre_max);
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            if (pre_cfg != NULL && *pre_cfg) {
                if (sal_strlen(pre_cfg) >= sizeof(sink->prefix_format)) {
                    cli_out("Prefix format string exceeds %d characters.\n",
                            (int)sizeof(sink->prefix_format) - 1);
                    bcma_cli_parse_table_done(&pt);
                    return BCMA_CLI_CMD_FAIL;
                }
                sal_strcpy(sink->prefix_format, pre_cfg);
            }
            bcma_cli_parse_table_done(&pt);
            /* Make sure ranges are sane */
            if (sink->enable_range.min > sink->enable_range.max) {
                sink->enable_range.min = sink->enable_range.max;
            }
            if (sink->prefix_range.min > sink->prefix_range.max) {
                sink->prefix_range.min = sink->prefix_range.max;
            }
            /* Update sink from local copy */
            sal_memcpy(cur_sink, sink, sizeof(*cur_sink));
            return BCMA_CLI_CMD_OK;
        } else {
            BCMA_CLI_ARG_PREV(a);
        }
    } else {
        cli_out("Available layers:\n");
        for (idx = 0; idx < BSL_LAY_COUNT; idx++) {
            cli_out("%s ", bsl_layer2str(idx));
        }
        cli_out("\n");
        return BCMA_CLI_CMD_OK;
    }

    if (BCMA_CLI_ARG_CNT(a) != 0 ) {
        arg1 = BCMA_CLI_ARG_GET(a);
    }

    if (BCMA_CLI_ARG_CNT(a) != 0 ) {
        arg2  = BCMA_CLI_ARG_GET(a);
    }

    if (BCMA_CLI_ARG_CNT(a) != 0 ) {
        arg3  = BCMA_CLI_ARG_GET(a);
    }

    if (BCMA_CLI_ARG_CNT(a) != 0 ) {
        return BCMA_CLI_CMD_USAGE;
    }

    sal_memset(&layer_list, 0, sizeof(layer_list));
    sal_memset(&source_list, 0, sizeof(source_list));
    rv = sh_bsl_parse(arg1, arg2, arg3,
                      &option, &layer_list, &source_list, &severity);
    if (rv < 0) {
        return rv;
    }

    if (option == CMD_OPT_SHOW) {
        rv = sh_bsl_show(u, &layer_list, &source_list, severity);
    } else {
        rv = sh_bsl_cfg(u, option, &layer_list, &source_list, severity);
    }

    return rv;
}
