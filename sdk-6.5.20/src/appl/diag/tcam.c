/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag CLI "tcam" command (5660x devices only)
 */

#include <soc/defs.h>
#if defined(BCM_TRIUMPH_SUPPORT)
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <bcm/error.h>
#include <soc/er_tcam.h>
#include <soc/triumph.h>
#include <soc/triumph3.h>

char cmd_tcam_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "\t  tcam <subcmd> [options]\n"
#else
    "\t  tcam info\n"
    "\t       - Show tcam configuration\n"
    "\t  tcam write Partition=<part> Index=<index> Mask=<mask> Data=<data>\n"
    "\t             [Count=<count>][Valid=<v>]\n"
    "\t  tcam dump Partition=<part> Index=<index> [Count=<count>]\n"
    "\t            [Display=<boolean>]\n"
    "\t  tcam search Partition=<part> [Partition2=<part>][Upper] Data=<data>\n"
    "\t  tcam ib Addr=address Data=opcode\n"
    "\t  tcam ima Addr=address [Data=<data>]\n"
    "\t  tcam dbreg Addr=address [Data=<data>]\n"
    "\t  tcam calc Partition=<part> Index=<index> Table=<table>\n"
    "\t  tcam prbs <set>|<get>|<clear> [poly={0|1|2|3}] <seed> \n"
    "\t       -For ex: \n" 
    "\t        tcam prbs set poly=3 seed=0xaaaaaaaa\n" 
    "\t  tcam latency\n"
    "\t       - Calculate external tcam request response latency\n"
    "\t \n"
#endif
;

static const char *partition_names[] = {
  "RAW",
  "ACL",
  "LPM",
  "FWD_L2",
  "FWD_L2_WIDE",
  "FWD_IP4_UCAST",
  "FWD_IP4_UCAST_WIDE",
  "FWD_IP4",
  "FWD_IP6U",
  "FWD_IP6_UCAST",
  "FWD_IP6_UCAST_WIDE",
  "FWD_IP6",
  "ACL_L2",
  "ACL_IP4",
  "ACL_IP6S",
  "ACL_IP6F",
  "ACL_L2C",
  "ACL_IP4C",
  "ACL_IP6C",
  "ACL_L2IP4",
  "ACL_L2IP6",
  "DEV0_TBL72",
  "DEV0_TBL144",
  "DEV1_TBL72",
  "DEV1_TBL144",
  "ACL_80",
  "ACL_160",
  "ACL_320",
  "ACL_480",
  "FWD_L2_DUPLICATE",
  "FWD_IP4_DUPLICATE",
  "FWD_IP6U_DUPLICATE",
  "FWD_IP6_DUPLICATE",
  NULL
};

static const char *partition_descs[] = {
  "",
  "",
  "",
  "L2 forwarding table",
  "L2 forwarding table (wide)",
  "IPv4 host table",
  "IPv4 host table(wide)",
  "IPv4 forwarding table",
  "IPv6 64-bit forwarding table",
  "IPv6 128-bit host forwarding table",
  "IPv6 128-bit host forwarding table(wide)",
  "IPv6 128-bit forwarding table",
  "L2 288-bit ACL table",
  "IPv4 288-bit ACL table",
  "IPv6 360-bit ACL table",
  "IPv6 432-bit ACL table",
  "L2 144-bit ACL table",
  "IPv4 144-bit ACL table",
  "IPv6 144-bit ACL table",
  "L2 + IPv4 432-bit ACL table",
  "L2 + IPv6 432-bit ACL table",
  "Device 0 72-bit test table",
  "Device 0 144-bit test table",
  "Device 1 72-bit test table",
  "Device 1 144-bit test table",
  "ACL 80",
  "ACL 160",
  "ACL 320",
  "ACL 480",
  "Duplicated L2 forwarding table",
  "Duplicated IPv4 forwarding table",
  "Duplicated IPv6 64 forwarding table",
  "Duplicated IPv6 128 forwarding table",
};

/*
 * convert hex string to multiple groups of 72-bit binary
 * each 72-bit binary group is padded and stored in a 128-bit unit
 * for multiple 72-bit group, LS bit is in highest offset unit
 * for example: convert following 128-bit string to 144-bit binary
 * hex: [0x]1234567890abcdef1357924680acebdf
 * bin: 00000000 00000000 00123456 7890abcd 00000000 000000ef 13579246 80acebdf
 */
STATIC int
hex_to_72bit_unit_bin(char *hex_str, uint32 *bin_str, int num_unit)
{
    int len, len_per_unit, len_per_word, unit_idx, word_idx, nibble_idx;
    uint32 value;
    char hex_nibble;

    if (hex_str[0] == '0' && (hex_str[1] | 0x20) == 'x') {
        hex_str += 2;
    }

    len = strlen(hex_str);
    for (unit_idx = num_unit - 1; unit_idx >= 0; unit_idx--) {
        len_per_unit = len > 18 ? 18 : len;
        len -= len_per_unit;
        for (word_idx = 3; word_idx >= 0; word_idx--) {
            len_per_word = len_per_unit > 8 ? 8 : len_per_unit;
            len_per_unit -= len_per_word;
            value = 0;
            for (nibble_idx = 0; nibble_idx < len_per_word; nibble_idx++) {
                value <<= 4;
                hex_nibble = hex_str[len + len_per_unit + nibble_idx];
                if (hex_nibble >= '0' && hex_nibble <= '9') {
                    value |= hex_nibble - '0';
                } else {
                    hex_nibble |= 0x20;
                    if (hex_nibble >= 'a' && hex_nibble <= 'f') {
                        value |= hex_nibble - 'a' + 0x0a;
                    } else {
                        return CMD_FAIL;
                    }
                }
            }
            bin_str[unit_idx * 4 + word_idx] = value;
        }
    }

    return CMD_OK;
}

STATIC int
hex_to_80bit_unit_bin(char *hex_str, uint32 *bin_str, int num_unit)
{
    int len, len_per_unit, len_per_word, unit_idx, word_idx, nibble_idx;
    uint32 value;
    char hex_nibble;

    if (hex_str[0] == '0' && (hex_str[1] | 0x20) == 'x') {
        hex_str += 2;
    }

    len = strlen(hex_str);
    for (unit_idx = num_unit - 1; unit_idx >= 0; unit_idx--) {
        len_per_unit = len > 20 ? 20 : len;
        len -= len_per_unit;
        for (word_idx = 3; word_idx >= 0; word_idx--) {
            len_per_word = len_per_unit > 8 ? 8 : len_per_unit;
            len_per_unit -= len_per_word;
            value = 0;
            for (nibble_idx = 0; nibble_idx < len_per_word; nibble_idx++) {
                value <<= 4;
                hex_nibble = hex_str[len + len_per_unit + nibble_idx];
                if (hex_nibble >= '0' && hex_nibble <= '9') {
                    value |= hex_nibble - '0';
                } else {
                    hex_nibble |= 0x20;
                    if (hex_nibble >= 'a' && hex_nibble <= 'f') {
                        value |= hex_nibble - 'a' + 0x0a;
                    } else {
                        return CMD_FAIL;
                    }
                }
            }
            bin_str[unit_idx * 4 + word_idx] = value;
        }
    }

    return CMD_OK;
}

cmd_result_t
cmd_tcam(int u, args_t * a)
{
    soc_tcam_partition_t *partition;
    char * subcmd;
    parse_table_t pt;
    int rv, part, index, addr, valid, count, display, i, j;
    char *mask_str, *data_str, *table_str;
    uint32 mask[32], data[32];
    soc_tcam_info_t *tcam_info;
    int type, subtype, width;
#ifdef BCM_TRIUMPH3_SUPPORT
    int idx;
#endif

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (!SOC_IS_TRIUMPH3(u)) {
#endif
        rv = soc_tcam_get_info(u, &type, &subtype, NULL, &tcam_info);
        if (rv >= 0 &&
            (!type || !tcam_info->partitions[TCAM_PARTITION_RAW].num_entries)) {
            rv = SOC_E_INIT;
        }
        if (rv < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
#ifdef BCM_TRIUMPH3_SUPPORT
    } else {
        rv = soc_tcam_get_info(u, &type, &subtype, NULL, &tcam_info);
        type=0; subtype=0;
        if ((rv < 0) || (tcam_info == NULL)) {
            cli_out("TCAM info not found. Not initialized\n");
            return CMD_FAIL;
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    rv = SOC_E_NONE;
    if (sal_strcasecmp(subcmd, "init") == 0) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(u)) {
            if (soc_feature(u, soc_feature_esm_support)) {
                SOC_IF_ERROR_RETURN(soc_tr3_esm_init_read_config(u));
                if (SOC_CONTROL(u)->tcam_info) {
                    SOC_IF_ERROR_RETURN(soc_tr3_esmif_init(u));
                }
            }
            soc_tcam_init(u);
            soc_triumph3_esm_init(u);
            cli_out("esm init done\n");
        } else {
#endif
            soc_tcam_init(u);
#if defined(BCM_TRIUMPH3_SUPPORT)
        }
#endif
    } else if (sal_strcasecmp(subcmd, "info") == 0) {
        cli_out("tcam type %d subtype %d", type, subtype);
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(u)) {
            for (idx = 0; idx < tr3_tcam_partition_order_num_entries; idx++) {
                i = tr3_tcam_partition_order[idx];
                if (i == 0) {
                    continue;
                }
                partition = &tcam_info->partitions[i];
                if (partition->num_entries) {
                    cli_out("\n%-10s %s",
                            partition_names[i], partition_descs[i]);
                    cli_out("\n           %d entries",
                            partition->num_entries);
                    if (partition->num_entries_include_pad !=
                        partition->num_entries) {
                        cli_out(" plus %d padded entries",
                                partition->num_entries_include_pad -
                                partition->num_entries);
                    }
                    cli_out("\n           tcam_base:0x%x width:%d-bit",
                            partition->tcam_base,
                            80 << partition->tcam_width_shift);
                    cli_out("\n           ad_ba:%d ad_ra_base:%d ad_ca:%d",
                            partition->ad_info.ad_ba,
                            partition->ad_info.ad_ra_base,
                            partition->ad_info.ad_ca);
                    cli_out("\n           ad_width:%d et_width:%d",
                            35 * (partition->ad_info.ad_width + 1), 
                            80 << partition->ad_info.et_width);
                    if (partition->ad_info.hit_bit_enable) {
                        cli_out("\n           hit_bit_pa_base:%d",
                                partition->ad_info.hit_bit_pa_base); 
                    }
                }
            }
        } else {
#endif
            for (i = TCAM_PARTITION_RAW + 1; i < TCAM_PARTITION_COUNT; i++) {
                partition = &tcam_info->partitions[i];
                if (partition->num_entries) {
                    cli_out("\n%-10s %s",
                            partition_names[i], partition_descs[i]);
                    cli_out("\n           %d entries",
                            partition->num_entries);
                    if (partition->num_entries_include_pad !=
                        partition->num_entries) {
                        cli_out(" plus %d padded entries",
                                partition->num_entries_include_pad -
                                partition->num_entries);
                    }
                    cli_out("\n           tcam_base:0x%x width:%d-bit",
                            partition->tcam_base,
                            72 << partition->tcam_width_shift);
                    if (partition->sram_base >= 0) {
                        cli_out("\n           sram_base:0x%x-0x%x width:%d-bit in",
                                partition->sram_base,
                                partition->sram_base +
                                ((partition->num_entries_include_pad - 1) <<
                                partition->sram_width_shift),
                                SOC_IS_TRIUMPH3(u) ? 40 << partition->sram_width_shift :
                                36 << partition->sram_width_shift);
                        if (partition->flags &
                            TCAM_PARTITION_FLAGS_AD_IN_SRAM0) {
                            cli_out(" sram0");
                        }
                        if (partition->flags &
                            TCAM_PARTITION_FLAGS_AD_IN_SRAM1) {
                            cli_out(" sram1");
                        }
                    }
                    if (partition->counter_base >= 0) {
                        cli_out("\n           counter_base:0x%x in %s",
                                partition->counter_base, partition->flags &
                                TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0 ?
                                "sram0" : "sram1");
                    }
                    if (partition->hbit_base >= 0) {
                        cli_out("\n           hbit_base:0x%x",
                                partition->hbit_base);
                    }
                }
            }
#ifdef BCM_TRIUMPH3_SUPPORT
        } /* SOC_IS_TRIUMPH3 */
#endif
        cli_out("\n");
    } else if (sal_strcasecmp(subcmd, "write") == 0) {
        uint32 mask_r[32], data_r[32], pattern;
        int valid_r, readback, search;

        parse_table_init(u, &pt);
        parse_table_add(&pt, "Partition", PQ_MULTI, 0, &part, partition_names);
        parse_table_add(&pt, "Index", PQ_INT, (void *)-1, &index, 0);
        parse_table_add(&pt, "Mask", PQ_STRING, 0, &mask_str, 0);
        parse_table_add(&pt, "Data", PQ_STRING, 0, &data_str, 0);
        parse_table_add(&pt, "Pattern", PQ_INT, (void *)0, &pattern, 0);
        parse_table_add(&pt, "Valid", PQ_INT, (void *)-1, &valid, 0);
        parse_table_add(&pt, "Count", PQ_INT, (void *)1, &count, 0);
        parse_table_add(&pt, "Readback", PQ_INT, (void *)0, &readback, 0);
        parse_table_add(&pt, "Search", PQ_INT, (void *)0, &search, 0);

        if (parse_arg_eq(a, &pt) < 0 || index < 0 ||
            !mask_str[0] || !data_str[0]) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        width = 1 << tcam_info->partitions[part].tcam_width_shift;
        SOC_IS_TRIUMPH3(u) ? hex_to_80bit_unit_bin(mask_str, mask, width) :
                             hex_to_72bit_unit_bin(mask_str, mask, width);

        SOC_IS_TRIUMPH3(u) ? hex_to_80bit_unit_bin(data_str, data, width) :
                             hex_to_72bit_unit_bin(data_str, data, width);


        parse_arg_eq_done(&pt);

        for (i = 0; i < count; i++) {
            rv = soc_tcam_write_entry(u, part, index + i, mask, data);
            if (rv < 0) {
                break;
            }
            if (valid != -1) {
                rv = soc_tcam_set_valid(u, part, index + i, valid);
                if (rv < 0) {
                    break;
                }
            }
            if (readback) {
                rv = soc_tcam_read_entry(u, part, index + i, mask_r, data_r,
                                         &valid_r);
                if (rv < 0) {
                    break;
                }
                if ((data[1] ^ data_r[1]) & 0xff ||
                    (mask[1] ^ mask_r[1]) & 0xff ||
                    data[2] != data_r[2] || data[3] != data_r[3]) {
                    cli_out("Mismatched readback: index %d\n", index + i);
                }
                if (width == 2) {
                    if ((data[5] ^ data_r[5]) & 0xff ||
                        (mask[5] ^ mask_r[5]) & 0xff ||
                        data[6] != data_r[6] || data[7] != data_r[7]) {
                        cli_out("Mismatched readback: index %d\n", index + i);
                    }
                }
            }
            data[width * 4 - 1] += 0x100;
        }
    } else if (sal_strcasecmp(subcmd, "dump") == 0) {
        parse_table_init(u, &pt);
        parse_table_add(&pt, "Partition", PQ_MULTI, 0, &part, partition_names);
        parse_table_add(&pt, "Index", PQ_INT, (void *)-1, &index, 0);
        parse_table_add(&pt, "Count", PQ_INT, (void *)1, &count, 0);
        parse_table_add(&pt, "Display", PQ_INT, (void *)1, &display, 0);

        if (parse_arg_eq(a, &pt) < 0 || index < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        width = 1 << tcam_info->partitions[part].tcam_width_shift;
        if (SOC_IS_TRIUMPH3(u)) {
            if (width == 8) width = 6;
        }
        for (i = 0; i < count; i++) {
            rv = soc_tcam_read_entry(u, part, index + i, mask, data, &valid);
            if (rv < 0) {
                break;
            }
            if (!display) {
                continue;
            }
            cli_out("%d: V=%d ", index + i, valid);
            cli_out("MASK=");
            for (j = 0; j < width; j++) {
                if (SOC_IS_TRIUMPH3(u)) {
                    cli_out("%4.4x%8.8x%8.8x",
                            mask[j * 4 + 1], mask[j * 4 + 2], mask[j * 4 + 3]);
                } else {
                    cli_out("%2.2x%8.8x%8.8x",
                            mask[j * 4 + 1], mask[j * 4 + 2], mask[j * 4 + 3]);
                }
            }
            cli_out(" DATA=");
            for (j = 0; j < width; j++) {
                if (SOC_IS_TRIUMPH3(u)) {
                    cli_out("%4.4x%8.8x%8.8x",
                            data[j * 4 + 1], data[j * 4 + 2], data[j * 4 + 3]);
                } else {
                    cli_out("%2.2x%8.8x%8.8x",
                            data[j * 4 + 1], data[j * 4 + 2], data[j * 4 + 3]);
                }
            }
            cli_out("\n");
        }
    } else if (sal_strcasecmp(subcmd, "search") == 0) {
        int upper, part2, index2, bist;

        sal_memset(data, 0, sizeof(data));

        parse_table_init(u, &pt);
        parse_table_add(&pt, "Partition", PQ_MULTI, 0, &part, partition_names);
        parse_table_add(&pt, "Partition2", PQ_MULTI, 0, &part2,
                        partition_names);
        parse_table_add(&pt, "Data", PQ_STRING, 0, &data_str, 0);
        parse_table_add(&pt, "Upper", PQ_INT, (void *)0, &upper, 0);
        parse_table_add(&pt, "Bist", PQ_INT, (void *)0, &bist, 0);
        parse_table_add(&pt, "Index", PQ_INT, (void *)0, &index, 0);
        parse_table_add(&pt, "Index2", PQ_INT, (void *)0, &index2, 0);

        if (parse_arg_eq(a, &pt) < 0 || !data_str[0]) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        width = tcam_info->partitions[part].tcam_width_shift >
            tcam_info->partitions[part2].tcam_width_shift ?
            1 << tcam_info->partitions[part].tcam_width_shift :
            1 << tcam_info->partitions[part2].tcam_width_shift;
        SOC_IS_TRIUMPH3(u) ? hex_to_80bit_unit_bin(data_str, data, width) :
                             hex_to_72bit_unit_bin(data_str, data, width);
        parse_arg_eq_done(&pt);

        if (soc_feature(u, soc_feature_esm_support)) {
#if defined(BCM_TRIUMPH_SUPPORT)
            if (bist) {
                rv = soc_triumph_tcam_search_bist(u, part, part2, data, index,
                                                  index2, 10000);
            } else
#endif /* BCM_TRIUMPH_SUPPORT */
            {
                rv = soc_tcam_search_entry(u, part, part2, data, &index,
                                           &index2);
                if(SOC_IS_TRIUMPH3(u)) {
                    if (rv >= 0) {
                        if(!(index & 0x40000000)) {
                            cli_out("NOT FOUND\n");
                        } else {
                            /* for single search, dbus[1] bits 23-0 have result */
                            index = index & 0x7FFFFF;
                            cli_out("Index=0x%x 0x%x\n", index, index2);
                        }
                    } else if (rv == SOC_E_NOT_FOUND) {
                        cli_out("NOT FOUND\n");
                        rv = SOC_E_NONE;
                    }
                } else {
                    if (rv >= 0) {
                        cli_out("Index=0x%x 0x%x\n", index, index2);
                    } else if (rv == SOC_E_NOT_FOUND) {
                        cli_out("NOT FOUND\n");
                        rv = SOC_E_NONE;
                    }
                }
            }
        }
    } else if (sal_strcasecmp(subcmd, "ib") == 0) {
        parse_table_init(u, &pt);
        parse_table_add(&pt, "Addr", PQ_INT, (void *) -1, &addr, 0);
        parse_table_add(&pt, "Data", PQ_STRING, 0, &data_str, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        hex_to_72bit_unit_bin(data_str, data, 1);
        parse_arg_eq_done(&pt);

        rv = soc_tcam_write_ib(u, addr, data[3]);
    } else if (sal_strcasecmp(subcmd, "ima") == 0) {
        parse_table_init(u, &pt);
        parse_table_add(&pt, "Addr", PQ_INT, (void *)-1, &addr, 0);
        parse_table_add(&pt, "Data", PQ_STRING, 0, &data_str, 0);

        if (parse_arg_eq(a, &pt) < 0 || addr < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        display = !data_str[0];
        SOC_IS_TRIUMPH3(u) ? hex_to_80bit_unit_bin(data_str, data, 1) :
                             hex_to_72bit_unit_bin(data_str, data, 1);
        parse_arg_eq_done(&pt);

        if (display) {
            rv = soc_tcam_read_ima(u, addr, &data[2], &data[3]);
            if (rv >= 0) {
                cli_out("Read from addr 0x%08x: 0x%08x %08x\n",
                        addr, data[2], data[3]);
            }
        } else {
            rv = soc_tcam_write_ima(u, addr, data[2], data[3]);
        }
    } else if (sal_strcasecmp(subcmd, "dbreg") == 0) {
        parse_table_init(u, &pt);
        parse_table_add(&pt, "Addr", PQ_INT, (void *)-1, &addr, 0);
        parse_table_add(&pt, "Data", PQ_STRING, 0, &data_str, 0);

        if (parse_arg_eq(a, &pt) < 0 || addr < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        display = !data_str[0];
        SOC_IS_TRIUMPH3(u) ? hex_to_80bit_unit_bin(data_str, data, 1) :
                             hex_to_72bit_unit_bin(data_str, data, 1);
        parse_arg_eq_done(&pt);

        if (display) {
            rv = soc_tcam_read_dbreg(u, addr, &data[1], &data[2], &data[3]);
            if (rv >= 0) {
                cli_out("Read from addr 0x%08x: 0x%08x 0x%08x %08x\n",
                        addr, data[1], data[2], data[3]);
            }
        } else {
            rv = soc_tcam_write_dbreg(u, addr, data[1], data[2], data[3]);
        }
    } else if (sal_strcasecmp(subcmd, "calc") == 0) {
        int copyno, mem_index, raw_index;
        soc_mem_t mem, real_mem;

        parse_table_init(u, &pt);
        parse_table_add(&pt, "Partition", PQ_MULTI, 0, &part, partition_names);
        parse_table_add(&pt, "Index", PQ_INT, (void *)-1, &index, 0);
        parse_table_add(&pt, "Table", PQ_STRING, 0, &table_str, 0);

        if (parse_arg_eq(a, &pt) < 0 || index < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (parse_memory_name(u, &mem, table_str, &copyno, 0) < 0) {
            cli_out("%s: ERROR: unknown table %s\n", ARG_CMD(a), table_str);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);

	rv = soc_tcam_part_index_to_mem_index(u, part, index, mem, &mem_index);
        if (rv >= 0) {
	    rv = soc_tcam_mem_index_to_raw_index(u, mem, mem_index,
                                                 &real_mem, &raw_index);
            if (rv >= 0) {
                cli_out("logical index 0x%x raw index 0x%x\n",
                        mem_index, raw_index);
            }
        }
    } else if (sal_strcasecmp(subcmd, "prbs") == 0) {
#ifdef BCM_TRIUMPH3_SUPPORT
        char *str = NULL;
        uint32 seed = 0x0, poly = 0;
        uint32 enable = 0;
        uint16 cmd = 0;
        uint32 status;
        uint16 max_lane = 0, num_tcams = 0, nl_dev_id = 0, lane = 0;
        uint16 crx_lanes = 0, ctx_lanes = 0, mdio_portid = 0, prbs_pass = 1;
        enum { _PHY_PRBS_SET_CMD, _PHY_PRBS_GET_CMD, _PHY_PRBS_CLEAR_CMD };

        if ((str = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        if (sal_strcasecmp(str, "set") == 0) {
            cmd = _PHY_PRBS_SET_CMD;
            enable = 1;
        } else if (sal_strcasecmp(str, "get") == 0) {
            cmd = _PHY_PRBS_GET_CMD;
            enable = 0;
        } else if (sal_strcasecmp(str, "clear") == 0) {
            cmd = _PHY_PRBS_CLEAR_CMD;
            enable = 0;
        } else return CMD_USAGE;

        parse_table_init(u, &pt);
        parse_table_add(&pt, "poly", PQ_DFL|PQ_INT,
                (void *)(0), &poly, NULL);
        parse_table_add(&pt, "seed", PQ_DFL|PQ_INT,
                (void *)(0), &seed, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        /* Now free allocated strings */
        parse_arg_eq_done(&pt);

        num_tcams = ((soc_tcam_info_t *)SOC_CONTROL(u)->tcam_info)->num_tcams;
        max_lane = 23;

        
        if(num_tcams >1) {
            max_lane = 15;
        }

        if((cmd == _PHY_PRBS_SET_CMD) || 
                (cmd == _PHY_PRBS_CLEAR_CMD)) {

            /* Enable prbs generation from TR3 */ 
            for(lane=0; lane <= max_lane; ++lane) {
                if ((poly <= 3) && (poly !=1)) {
                    rv = wcmod_esm_serdes_control_set(u, 
                            lane, 
                            SOC_PHY_CONTROL_PRBS_POLYNOMIAL,   
                            &poly);
                    if( rv != BCM_E_NONE) {
                        cli_out("Setting prbs polynomial failed on ESM SerDes\n");
                        return CMD_FAIL;
                    }
                    /* Tx enable */
                    rv = wcmod_esm_serdes_control_set(u, 
                            lane, 
                            SOC_PHY_CONTROL_PRBS_TX_ENABLE,   
                            &enable);
                    if( rv != BCM_E_NONE) {
                        cli_out("Setting prbs tx enable failed\n");
                        return CMD_FAIL;
                    }
                } else {
                    cli_out("Polynomial must be a value of 0,2,3, inclusive.\n");
                    return CMD_FAIL;
                }

            }
            /* Enable prbs checkers on all the tcams */
            for(nl_dev_id=0; nl_dev_id < num_tcams; ++nl_dev_id) {
                mdio_portid = mdio_portid_get(u, nl_dev_id);
                /* CRX lanes active for all tcams except the first
                   TCAM in the chain directly connected to request \
                   line(tx) of serdes (nl_dev_id = 0)*/
                crx_lanes = nl_dev_id? 1 : 0;
                rv = nl_mdio_prbs_chk(u, mdio_portid, poly, seed, crx_lanes, enable);
                if(rv < 0) {
                    cli_out("Enabling prbs checker on tcam(mdio_portid %d) failed \n",mdio_portid);
                    return CMD_FAIL;
                }
            }

            /* Enable prbs generators in NL */
            for(nl_dev_id=0; nl_dev_id < num_tcams; ++nl_dev_id) {
                mdio_portid = mdio_portid_get(u, nl_dev_id);
                /* CTX lanes active for all tcams except the last
                   TCAM in the chain directly connected to response 
                   line(rx) of serdes */
                ctx_lanes = (nl_dev_id == (num_tcams-1))? 0 : 1;
                rv = nl_mdio_prbs_gen(u, mdio_portid, poly, seed, ctx_lanes, enable);
                if(rv < 0) {
                    cli_out("Enabling prbs generator on tcam(mdio_portid %d) failed \n",mdio_portid);
                    return CMD_FAIL;
                }
            }


            max_lane = 23; 
            /* Configure prbs checkers in TR3 for upper 12 rx lanes */
            for(lane=12; lane <= max_lane; ++lane) {
                if (poly <= 3) {
                    rv = wcmod_esm_serdes_control_set(u, 
                            lane, 
                            SOC_PHY_CONTROL_PRBS_POLYNOMIAL,   
                            &poly);
                    if( rv != BCM_E_NONE) {
                        cli_out("Enabling prbs checker on ESM SerDes failed\n");
                        return CMD_FAIL;
                    }
                    /* Tx enable */
                    rv = wcmod_esm_serdes_control_set(u, 
                            lane, 
                            SOC_PHY_CONTROL_PRBS_TX_ENABLE,   
                            &enable);
                    if( rv != BCM_E_NONE) {
                        cli_out("Setting prbs tx enable failed\n");
                        return CMD_FAIL;
                    }
                } else {
                    cli_out("Polynomial must be between 0 & 3, inclusive.\n");        
                    return CMD_FAIL;
                }
            }
        } else if(cmd == _PHY_PRBS_GET_CMD) {
            /* Check for errors in NL prbs checkers, TR3 prbs checkers */
            for(nl_dev_id=0; nl_dev_id < num_tcams; ++nl_dev_id) {
                mdio_portid = mdio_portid_get(u, nl_dev_id);
                /* CRX lanes active for all tcams except the first
                   TCAM in the chain directly connected to request \
                   line(tx) of serdes (nl_dev_id = 0)*/
                crx_lanes = nl_dev_id? 1 : 0;
                cli_out("CHECKING PRBS: NL Tcam (%d)\n", mdio_portid);
                rv = nl_mdio_prbs_chk_err(u, mdio_portid, crx_lanes);
                if(rv < 0) {
                    cli_out("Getting prbs status on tcam(mdio_portid %d) failed \n",mdio_portid);
                    return CMD_FAIL;
                }

            }

            /* Check for errors in TR3 prbs checkers */
            cli_out("CHECKING PRBS: TR3 ESM SerDes\n");
            max_lane = 23;
            for(lane=12; lane <= max_lane; ++lane) {
                rv = wcmod_esm_serdes_control_get(u, 
                        lane, 
                        SOC_PHY_CONTROL_PRBS_RX_STATUS,   
                        &status);
                if( rv != BCM_E_NONE) {
                    cli_out("ESM SerDes: Getting prbs status failed (lane:%d) \n", lane);
                    return CMD_FAIL;
                }
                switch (status) {
                    case 0:
                        /*cli_out("(%d):  WC PRBS OK!\n", lane);*/
                        break;
                    case 0xffffffff: /* -1 */ 
                        cli_out("\tPRBS not locked on lane %d of RX interface\n", lane - 12);
                        prbs_pass = 0;
                        break;
                    case 0xfffffffe: /* -2 */ 
                        cli_out("\tPRBS lost lock on lane %d of RX interface\n", lane - 12);
                        prbs_pass = 0;
                        break;
                    default:
                        cli_out("\t%d PRBS errors seen on lane %d of RX interface\n", status, lane - 12);
                        prbs_pass = 0;
                        break;
                }
            }         
            if(prbs_pass) {
                cli_out("\tPRBS OK\n");
            }
        }

#else /*BCM_TRIUMPH3_SUPPORT*/
            return CMD_NOTIMPL;
#endif /*BCM_TRIUMPH3_SUPPORT*/
    } else if (sal_strcasecmp(subcmd, "latency") == 0) { 
#if defined(BCM_TRIUMPH3_SUPPORT)
        int ptr_fme_req;
        unsigned latency_ns;
        if (SOC_IS_TRIUMPH3(u)) {
            if (soc_feature(u, soc_feature_esm_support) && SOC_CONTROL(u)->tcam_info) {
                ptr_fme_req = soc_tr3_esm_get_worst_fme(u);
                cli_out("Calculating, please wait...");
                rv = hwtl_find_idle_req_rsp_latency(u, ptr_fme_req, 30, &latency_ns);
                
                if( rv != BCM_E_NONE) {
                    cli_out("Get external tcam request response latency failed\n");
                    return CMD_FAIL;
                } else {
                    cli_out("\nExternal tcam request response latency:%d\n", latency_ns);
                }
            }
        }
#else /*BCM_TRIUMPH3_SUPPORT*/
        return CMD_NOTIMPL;
#endif /*BCM_TRIUMPH3_SUPPORT*/
    } else {
        return CMD_USAGE;
    }

    if (rv < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

#endif /* BCM_TRIUMPH_SUPPORT */
