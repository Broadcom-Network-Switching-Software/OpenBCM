/*! \file bcma_bcmpktcmd_pmddecode.c
 *
 * PMD decode commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pmddecode.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_base_t.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_extension_0_t.h>
#include <bcmpkt/bcmpkt_rxpmd.h>

#include "bcma_bcmpktcmd_internal.h"

/*! This sequence should be same as bcmdrd_cm_dev_type_t */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, BCMDRD_DEV_T_##_bd},
static const shr_enum_map_t device_list[] = {
    {"device_none", BCMDRD_DEV_T_NONE},
#include <bcmdrd/bcmdrd_devlist.h>
    {"device_count", BCMDRD_DEV_T_COUNT}
};

/* Array of device variant specific data */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    {#_bd, #_ve, BCMLRD_VARIANT_##_bu##_##_ve},

static struct {
    char *dev_name;
    char *var_name;
    uint32_t val;
} variant_list[] = {
    {"device_none", "variant_none", 0}, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    {"device_count", "variant_count", (BCMLRD_VARIANT_MAX + 1)} /* end-of-list */
};

#define FLEX_PROFILE_INVALID 0xffffffff

static int
dev_type_get(const char *str, uint32_t *val)
{
    int i;
    uint32_t type = 0;

    if ((!str) || (!val)) {
        return SHR_E_PARAM;
    }

    for (i = BCMDRD_DEV_T_NONE + 1; i < BCMDRD_DEV_T_COUNT; i++) {
        if (!sal_strcasecmp(str, device_list[i].name)) {
            type = device_list[i].val;
            break;
        }
    }
    if (!type) {
        return SHR_E_FAIL;
    }
    *val = type;

    return SHR_E_NONE;
}

static char *
dev_type_name_get(uint32_t val)
{
    int i;

    for (i = BCMDRD_DEV_T_NONE + 1; i < BCMDRD_DEV_T_COUNT; i++) {
        if (val == (uint32_t)device_list[i].val) {
            return device_list[i].name;
        }
    }
    return NULL;
}

static int
variant_get(const char *dev_str, const char *var_str, uint32_t *val)
{
    int i;
    uint32_t type = 0;

    if ((!dev_str) || (!var_str) || (!val)) {
        return SHR_E_PARAM;
    }

    for (i = 1; i <= BCMLRD_VARIANT_MAX; i++) {
        if (!sal_strcasecmp(dev_str, variant_list[i].dev_name) &&
            !sal_strcasecmp(var_str, variant_list[i].var_name)) {
            type = variant_list[i].val;
            break;
        }
    }
    if (!type) {
        return SHR_E_FAIL;
    }
    *val = type;

    return SHR_E_NONE;
}

static int
pmddecode_args_process(bcma_cli_t *cli,
                               bcma_cli_args_t *args,
                               uint32_t *dev_type,
                               uint32_t *variant,
                               uint32_t *flex_profile)
{
    int rv = SHR_E_NONE;
    int unit = cli->cmd_unit;
    char *dev_str = NULL;
    char *var_str = NULL;
    uint32_t dev_val = 0;
    uint32_t var_val = 0;
    int profile_val = FLEX_PROFILE_INVALID;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Device", "str", &(dev_str), NULL);
    bcma_cli_parse_table_add(&pt, "Variant", "str", &(var_str), NULL);
    bcma_cli_parse_table_add(&pt, "FlexProfile", "int", &profile_val, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if (dev_str) {
        /* Get device type by input. */
        rv = dev_type_get(dev_str, &dev_val);
        if (SHR_FAILURE(rv)) {
            cli_out("%sDevice %s is not found.\n",
                    BCMA_CLI_CONFIG_ERROR_STR, dev_str);
            return BCMA_CLI_CMD_FAIL;
        }
    } else {
        /* Get device type of current device. */
        rv = bcmpkt_dev_type_get(unit, &dev_val);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFailed to get device type.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
    }
    *dev_type = dev_val;

    if (var_str) {
        if (!dev_str) {
            /* Dev name is not specified by user. Get the name of current device. */
            dev_str = dev_type_name_get(dev_val);
            if (!dev_str) {
                cli_out("%sFailed to get device name.\n",
                        BCMA_CLI_CONFIG_ERROR_STR);
                return BCMA_CLI_CMD_FAIL;
            }
        }
        /* Get variant by input. */
        rv = variant_get(dev_str, var_str, &var_val);
        if (SHR_FAILURE(rv)) {
            cli_out("%sVariant %s is not found on device %s.\n",
                    BCMA_CLI_CONFIG_ERROR_STR, var_str, dev_str);
            return BCMA_CLI_CMD_FAIL;
        }
    } else {
        if (!dev_str) {
            /* Both dev and variant are not specified by user. Get current variant. */
            var_val = bcmlrd_variant_get(unit);
            if (var_val == BCMLRD_VARIANT_T_NONE) {
                cli_out("%sFailed to get variant.\n",
                        BCMA_CLI_CONFIG_ERROR_STR);
                return BCMA_CLI_CMD_FAIL;
            }
        } else {
            /* Dev name is specified by user but not the variant name. Get base variant. */
            rv = variant_get(dev_str, "base", &var_val);
            if (SHR_FAILURE(rv)) {
                cli_out("%sBase variant is not found on device %s.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, dev_str);
                return BCMA_CLI_CMD_FAIL;
            }
        }
    }
    *variant = var_val;

    *flex_profile = profile_val;
    bcma_cli_parse_table_done(&pt);

    return BCMA_CLI_CMD_OK;
}

static int
txpmd_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = SHR_E_NONE;
    uint32_t val;
    char *arg;
    uint32_t txpmd[BCMPKT_TXPMD_SIZE_WORDS];
    int i;
    shr_pb_t *pb;
    uint32_t dev_type = 0;
    uint32_t variant = 0;
    uint32_t profile = 0;

    if (pmddecode_args_process(cli, args, &dev_type, &variant, &profile) <  0) {
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(&txpmd, 0, BCMPKT_TXPMD_SIZE_WORDS * sizeof(uint32_t));
    for (i = 0; i < BCMPKT_TXPMD_SIZE_WORDS; i++) {
        if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
            break;
        }

        if (bcma_cli_parse_uint32(arg, &val) < 0) {
            cli_out("%sFailed to parse data.\n", BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        txpmd[i] = val;
    }

    pb = shr_pb_create();
    rv = bcmpkt_txpmd_dump(dev_type, txpmd, BCMPKT_TXPMD_DUMP_F_ALL, pb);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to dump TXPMD.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
rxpmd_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = SHR_E_NONE;
    uint32_t val;
    char *arg;
    uint32_t rxpmd[BCMPKT_RXPMD_SIZE_WORDS];
    int i;
    shr_pb_t *pb;
    uint32_t dev_type = 0;
    uint32_t variant = 0;
    uint32_t *flexdata = NULL;
    uint32_t flexdata_len = 0;
    uint32_t flex_profile = 0;
    uint32_t profile = 0;

    if (pmddecode_args_process(cli, args, &dev_type, &variant, &profile) <  0) {
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(&rxpmd, 0, BCMPKT_RXPMD_SIZE_WORDS * sizeof(uint32_t));
    for (i = 0; i < BCMPKT_RXPMD_SIZE_WORDS; i++) {
        if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
            break;
        }

        if (bcma_cli_parse_uint32(arg, &val) < 0) {
            cli_out("%sFailed to parse data.\n", BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        rxpmd[i] = val;
    }


    rv = bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd, &flexdata, &flexdata_len);
    if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
        cli_out("%sFailed to detect RXPMD_FLEX.\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "[RXPMD information]\n");
    rv = bcmpkt_rxpmd_dump(dev_type, rxpmd, BCMPKT_RXPMD_DUMP_F_ALL, pb);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to dump RXPMD.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    if (flexdata_len) {
        bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                               BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE, &flex_profile);
        shr_pb_printf(pb, "[FLEX fields]\n");
        rv = bcmpkt_rxpmd_flex_dump(variant, flexdata, flex_profile,
                                    BCMPKT_RXPMD_FLEX_DUMP_F_ALL, pb);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFailed to dump RXPMD flex data.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    shr_pb_printf(pb, "[RX reasons]\n");
    if (flexdata_len) {
        rv = bcmpkt_rxpmd_flex_reason_dump(variant, flexdata, pb);
    } else {
        rv = bcmpkt_rx_reason_dump(dev_type, rxpmd, pb);
    }
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to dump RX reasons.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
rxpmdflex_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = SHR_E_NONE;
    uint32_t val;
    char *arg;
    uint32_t rxpmdflex[BCMPKT_RXPMD_SIZE_WORDS];
    uint32_t i;
    shr_pb_t *pb;
    uint32_t dev_type = 0;
    uint32_t variant = 0;
    uint32_t profile = 0;
    uint32_t flexdata_len_words;
    uint32_t *flexdata_addr;

    if (pmddecode_args_process(cli, args, &dev_type, &variant, &profile) <  0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (profile == FLEX_PROFILE_INVALID) {
        cli_out("%sFlexProfile is required for decoding flex data.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmpkt_rxpmd_flexdata_get(dev_type, rxpmdflex,
                                   &flexdata_addr, &flexdata_len_words);
   if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to get flex data length.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(&rxpmdflex, 0, BCMPKT_RXPMD_SIZE_WORDS * sizeof(uint32_t));
    for (i = 0; i < flexdata_len_words; i++) {
        if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
            break;
        }

        if (bcma_cli_parse_uint32(arg, &val) < 0) {
            cli_out("%sFailed to parse data.\n", BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        rxpmdflex[i] = val;
    }


    pb = shr_pb_create();
    shr_pb_printf(pb, "[RXPMD flex data information]\n");
    rv = bcmpkt_rxpmd_flex_dump(variant, rxpmdflex, profile,
                                BCMPKT_RXPMD_FLEX_DUMP_F_ALL, pb);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            cli_out("%sDevice does not support RXPMD flex data.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to dump RXPMD flex data.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        }
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }

    shr_pb_printf(pb, "[RX reasons]\n");
    rv = bcmpkt_rxpmd_flex_reason_dump(variant, rxpmdflex, pb);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to dump RX reasons.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}
static int
hg3_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = SHR_E_NONE;
    uint32_t val;
    char *arg;
    bool base_hdr_support;
    bool ext0_hdr_support;
    uint32_t higig3_base[BCMPKT_HG3_BASE_HEADER_SIZE_WORDS];
    uint32_t higig3_ext0[BCMPKT_HG3_EXT0_HEADER_SIZE_WORDS];
    int i;
    shr_pb_t *pb;
    uint32_t dev_type = 0;
    uint32_t variant = 0;
    uint32_t profile = 0;

    if (pmddecode_args_process(cli, args, &dev_type, &variant, &profile) <  0) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmpkt_hg3_base_t_is_supported(variant, &base_hdr_support);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to get Higig3 support status.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmpkt_hg3_extension_0_t_is_supported(variant,
                                               &ext0_hdr_support);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to get Higig3 support status.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    if ((!base_hdr_support) || (!ext0_hdr_support)) {
        cli_out("%sDevice does not support Higig3.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(&higig3_base, 0,
               BCMPKT_HG3_BASE_HEADER_SIZE_WORDS * sizeof(uint32_t));
    sal_memset(&higig3_ext0, 0,
               BCMPKT_HG3_EXT0_HEADER_SIZE_WORDS * sizeof(uint32_t));
    for (i = 0; i < BCMPKT_HG3_BASE_HEADER_SIZE_WORDS; i++) {
        if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
            break;
        }

        if (bcma_cli_parse_uint32(arg, &val) < 0) {
            cli_out("%sFailed to parse data.\n", BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        higig3_base[i] = val;
    }

    if (i == BCMPKT_HG3_BASE_HEADER_SIZE_WORDS) {
        /* At least base header are all filled. Decode extension header if present. */
        if ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
            if (bcma_cli_parse_uint32(arg, &val) < 0) {
                cli_out("%sFailed to parse data.\n", BCMA_CLI_CONFIG_ERROR_STR);
                return BCMA_CLI_CMD_FAIL;
            }
            higig3_ext0[0] = val;
        }
        if ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
            if (bcma_cli_parse_uint32(arg, &val) < 0) {
                cli_out("%sFailed to parse data.\n", BCMA_CLI_CONFIG_ERROR_STR);
                return BCMA_CLI_CMD_FAIL;
            }
            higig3_ext0[1] = val;
        }
    }

    pb = shr_pb_create();
    rv = bcmpkt_hg3_base_t_dump(variant, higig3_base,
                                BCMPKT_HG3_BASE_T_DUMP_F_ALL, pb);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to dump HG3 base data.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    rv = bcmpkt_hg3_extension_0_t_dump(variant, higig3_ext0,
                                       BCMPKT_HG3_EXTENSION_0_T_DUMP_F_ALL, pb);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to dump HG3 extension 0 data.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_pmddecode(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Txpmd", cmd, ' ')) {
        return txpmd_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("Rxpmd", cmd, ' ')) {
        return rxpmd_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("RxpmdFlex", cmd, ' ')) {
        return rxpmdflex_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("Hg3", cmd, ' ')) {
        return hg3_cmds(cli, args);
    }

    cli_out("%sUnknown PMDDECODE command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

