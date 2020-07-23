/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        celldata.c
 * Purpose:     Diagnostic helper routines
 * Notes:       Perform various MCU DDR tuning tasks on Easyrider.
 */

#ifndef NO_MEMTUNE

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH_SUPPORT)
#include <soc/triumph.h>
#include <soc/error.h>
#include <soc/mem.h>
#include <soc/er_tcam.h>
#include <soc/memtune.h>

#include <appl/diag/cmdlist.h>
#include <appl/diag/system.h>
#include <sal/appl/config.h>

#include <bcm/pkt.h>
#include <bcm/stack.h>
#include <bcm/tx.h>
#include <bcm/vlan.h>

/*
 * convert hex string to multiple groups of 72-bit binary
 * each 72-bit binary group is padded and stored in a 128-bit unit
 * for multiple 72-bit group, LS bit is in highest offset unit
 * for example: convert following 128-bit string to 144-bit binary
 * hex: [0x]1234567890abcdef1357924680acebdf
 * bin: 00000000 00000000 00123456 7890abcd 00000000 000000ef 13579246 80acebdf
 */
STATIC int
_extt_hex_to_72bit_unit_bin(char *hex_str, uint32 *bin_str, int num_unit)
{
    int len, len_per_unit, len_per_word, unit_idx, word_idx, nibble_idx;
    uint32 value;
    char hex_nibble;

    if (hex_str[0] == '0' && (hex_str[1] | 0x20) == 'x') {
        hex_str += 2;
    }

    len = sal_strlen(hex_str);
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

/*
 * convert hex string to 2 groups of 18-bit binary
 * each 18-bit binary group is padded and stored in a 32-bit unit
 * hex: [0x]123456789
 * hi_18-bit: 000048d1
 * lo_18-bit: 00016789
 */
STATIC int
_extt_hex_to_18bit_unit_bin(char *hex_str, uint32 *lo_18bit, uint32 *hi_18bit)
{
    int len, len_per_word, word_idx, nibble_idx;
    uint32 value, bin_str[2];
    char hex_nibble;

    if (hex_str[0] == '0' && (hex_str[1] | 0x20) == 'x') {
        hex_str += 2;
    }

    len = sal_strlen(hex_str);
    for (word_idx = 1; word_idx >= 0; word_idx--) {
        len_per_word = len > 8 ? 8 : len;
        len -= len_per_word;
        value = 0;
        for (nibble_idx = 0; nibble_idx < len_per_word; nibble_idx++) {
            value <<= 4;
            hex_nibble = hex_str[len + nibble_idx];
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
        bin_str[word_idx] = value;
    }

    *hi_18bit = ((bin_str[0] & 0xf) << 14) | ((bin_str[1] & 0xfffc0000) >> 18);
    *lo_18bit = bin_str[1] & 0x3ffff;

    return CMD_OK;
}

#if defined(INCLUDE_L3) && defined(BCM_FIELD_SUPPORT)
STATIC void
_extt_do_checksum(unsigned char *stream, int length, int checksumOffset)
{
    unsigned short value;
    unsigned long checksum;
    int i;

    checksum = 0;
    for (i = 0; i < length; i += 2) {
        value = stream[i];
        if (i + 1 < length) {
            value |= stream[i + 1] << 8;
        }
        if (i == checksumOffset) {
            value = 0;
        }
        checksum += value;
    }
    checksum = (checksum & 0xffff) + (checksum >> 16);
    checksum ^= 0xffff;

    stream[checksumOffset] = checksum & 0xff;
    stream[checksumOffset + 1] = (checksum >> 8) & 0xff;
}
#endif /* INCLUDE_L3 && BCM_FIELD_SUPPORT */

STATIC int
_extt_l3_activities(int unit, int enable, int num_ge_pkt, int num_xe_pkt)
{
#if defined(INCLUDE_L3) && defined(BCM_FIELD_SUPPORT)
    static int l3_activities_running = FALSE;
    bcm_vlan_t vid = 2;
    bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    bcm_mac_t mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_if_t intf_id = 10;
    int pkt_size = 64;
    pbmp_t pbmp, ubmp;
    static int l3_egress_mode;
    static bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    static bcm_if_t egr_if_id;
    bcm_module_t my_modid;
    bcm_field_qset_t qset;
    static bcm_field_group_t group;
    static bcm_field_entry_t entry[SOC_MAX_NUM_PORTS];
    bcm_port_t port, prev_port;
    bcm_pkt_t *pkt;
    enet_hdr_t *enet_hdr;
    unsigned char *ipv4_hdr;
    int rv, i;

    vid = 2;
    intf_id = 10;

    if (enable) {
        if (l3_activities_running) {
            return CMD_OK;
        }

        /* Create a vlan */
        rv = bcm_vlan_create(unit, vid);
        if (rv < 0) {
            cli_out("fail to create Vlan %d : %s\n", vid, soc_errmsg(rv));
            return CMD_FAIL;
        }

        /* Add all ports to the vlan */
        SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        SOC_PBMP_OR(pbmp, PBMP_CMIC(unit));
        SOC_PBMP_ASSIGN(ubmp, PBMP_PORT_ALL(unit));
        rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
        if (rv < 0) {
            cli_out("fail to add ports to Vlan %d : %s\n",
                    vid, soc_errmsg(rv));
            return CMD_FAIL;
        }

        /* Save the current L3 mode then enable L3 */
        rv = bcm_switch_control_get(unit, bcmSwitchL3EgressMode,
                                    &l3_egress_mode);
        if (rv < 0) {
            cli_out("fail to get current L3 egress mode : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
        if (rv < 0) {
            cli_out("fail to enable L3 egress mode : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        /* Create L3 interface */
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_flags = BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
        l3_intf.l3a_intf_id = intf_id;
        sal_memcpy(&l3_intf.l3a_mac_addr, &router_mac, sizeof(router_mac));
        l3_intf.l3a_vid = vid;
        rv = bcm_l3_intf_create(unit, &l3_intf);
        if (rv < 0) {
            cli_out("fail to create L3 interface : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        rv = bcm_stk_my_modid_get(unit, &my_modid);
        if (rv < 0) {
            cli_out("fail to get my modid :%s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        /* Create L3 egress object, using router_mac as next hop mac address */
        bcm_l3_egress_t_init(&l3_egress);
        l3_egress.intf = intf_id;
        sal_memcpy(&l3_egress.mac_addr, &router_mac, sizeof(router_mac));
        l3_egress.module = my_modid;
        l3_egress.port = 0;
        rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_if_id);
        if (rv < 0) {
            cli_out("fail to create egress L3 object : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        /* Create field group to qualify ingress port on ingress stage */
        BCM_FIELD_QSET_INIT(qset);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
        rv = bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY,
                                    &group);
        if (rv < 0) {
            cli_out("fail to create field group : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        PBMP_E_ITER(unit, port) {
            /* Create field entry */
            rv = bcm_field_entry_create (unit, group, &entry[port]);
            if (rv < 0) {
                cli_out("fail to create field entry : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }
        }

        prev_port = SOC_PORT_MAX(unit, ge);
        PBMP_GE_ITER(unit, port) {
            /* Setup the ingress port qualifier */
            rv = bcm_field_qualify_InPort(unit, entry[port], prev_port,
                                          BCM_FIELD_EXACT_MATCH_MASK);
            if (rv < 0) {
                cli_out("fail to add field port qualifier : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }

            /* Setup the action to redirect the packet to the next GE port */
            rv = bcm_field_action_add(unit, entry[port],
                                      bcmFieldActionRedirect, my_modid, port);
            if (rv < 0) {
                cli_out("fail to add field action : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }
            prev_port = port;
        }

        prev_port = SOC_PORT_MAX(unit, xe);
        PBMP_XE_ITER(unit, port) {
            /* Setup the ingress port qualifier */
            rv = bcm_field_qualify_InPort(unit, entry[port], prev_port,
                                          BCM_FIELD_EXACT_MATCH_MASK);
            if (rv < 0) {
                cli_out("fail to add field port qualifier : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }

            /* Setup the action to redirect the packet to the next XE port */
            rv = bcm_field_action_add(unit, entry[port],
                                      bcmFieldActionRedirect, my_modid, port);
            if (rv < 0) {
                cli_out("fail to add field action : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }
            prev_port = port;
        }

        PBMP_E_ITER(unit, port) {
            /* Setup the action to not change TTL in IP header */
            rv = bcm_field_action_add(unit, entry[port],
                                      bcmFieldActionDoNotChangeTtl, 0, 0);
            if (rv < 0) {
                cli_out("fail to add field action : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }

            /* Install the FP entry to hardware */
            rv = bcm_field_entry_install(unit, entry[port]);
            if (rv < 0) {
                cli_out("fail to install field entry to hardware : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }
        }

        l3_activities_running = TRUE;

        PBMP_E_ITER(unit, port) {
            /* Put port to PHY loopback */
            rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY);
            if (rv < 0) {
                cli_out("fail to set port %d to MAC loopback : %s\n",
                        port, soc_errmsg(rv));
                return CMD_FAIL;
            }
        }

        /* Send 1 routable packet (MAC_DA == router_mac) to any port */
        rv = bcm_pkt_alloc(unit, pkt_size + 4, 0, &pkt);
        if (rv < 0) {
            cli_out("Error, bcm_pkt_alloc failed $rv\n");
            return rv;
        }
        if (pkt == NULL) {
            cli_out("fail to allocate packet : %s\n", soc_errmsg(rv));
            return CMD_FAIL;
        }

        pkt->unit = unit;
        pkt->flags = BCM_TX_CRC_APPEND;
        BCM_PKT_TX_LEN_SET(pkt, pkt_size);

        enet_hdr = (enet_hdr_t *)pkt->_pkt_data.data;
        ENET_SET_MACADDR(&enet_hdr->en_dhost, router_mac);
        enet_hdr->en_tag_tpid = soc_htons(0x0800); /* IP protocol */

        ipv4_hdr = &pkt->_pkt_data.data[ENET_UNTAGGED_HDR_LEN];
        ipv4_hdr[0] = 0x45; /* IPv4, length 5 x 32-bit words */
        ipv4_hdr[2] = 0x0; /* total length pkt_size - 18 bytes */
        ipv4_hdr[3] = pkt_size - 18;
        ipv4_hdr[8] = 0xff; /* time to live */
        ipv4_hdr[9] = 0;    /* L4 protocol */
        ipv4_hdr[12] = 10;  /* source IP address 10.1.1.1 */
        ipv4_hdr[13] = 1;
        ipv4_hdr[14] = 1;
        ipv4_hdr[15] = 1;
        ipv4_hdr[16] = 10;  /* destination IP address 10.2.1.1 */
        ipv4_hdr[17] = 2;
        ipv4_hdr[18] = 1;
        ipv4_hdr[19] = 1;
        _extt_do_checksum(ipv4_hdr, 20, 10); /* fill in header check sum */

        if (SOC_PORT_MIN(unit, ge) >= 0) {
            BCM_PKT_PORT_SET(pkt, SOC_PORT_MIN(unit, ge), TRUE, FALSE);
            mac[5] = 0x10;
            ENET_SET_MACADDR(&enet_hdr->en_shost, mac);
            for (i = 0; i < num_ge_pkt; i ++) {
                rv = bcm_tx(unit, pkt, NULL);
                if (rv < 0) {
                    cli_out("fail to send packet : %s\n", soc_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }

        if (SOC_PORT_MIN(unit, xe) >= 0) {
            BCM_PKT_PORT_SET(pkt, SOC_PORT_MIN(unit, xe), TRUE, FALSE);
            mac[5] = 0x11;
            ENET_SET_MACADDR(&enet_hdr->en_shost, mac);
            for (i = 0; i < num_xe_pkt; i ++) {
                rv = bcm_tx(unit, pkt, NULL);
                if (rv < 0) {
                    cli_out("fail to send packet : %s\n", soc_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }

        bcm_pkt_free(unit, pkt);
    } else {
        PBMP_E_ITER(unit, port) {
            rv = bcm_field_entry_remove(unit, entry[port]);
            if (rv < 0) {
                cli_out("fail to remove field entry from hardware : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }

            rv = bcm_field_entry_destroy(unit, entry[port]);
            if (rv < 0) {
                cli_out("fail to destroy field entry : %s\n",
                        soc_errmsg(rv));
                return CMD_FAIL;
            }
        }

        rv = bcm_field_group_destroy(unit, group);
        if (rv < 0) {
            cli_out("fail to destroy field group : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        rv = bcm_l3_egress_destroy(unit, egr_if_id);
        if (rv < 0) {
            cli_out("fail to destroy egress L3 object : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        rv = bcm_l3_intf_delete(unit, &l3_intf);
        if (rv < 0) {
            cli_out("fail to delete L3 interface : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode,
                                    l3_egress_mode);
        if (rv < 0) {
            cli_out("fail to restore L3 egress mode : %s\n",
                    soc_errmsg(rv));
            return CMD_FAIL;
        }

        rv = bcm_vlan_destroy(unit, vid);
        if (rv < 0) {
            cli_out("fail to destroy Vlan %d : %s\n",
                    vid, soc_errmsg(rv));
            return CMD_FAIL;
        }

        /* Remove all ports from MAC loopback */
        PBMP_E_ITER(unit, port) {
            rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE);
            if (rv < 0) {
                cli_out("fail to remove port %d from loopback : %s\n",
                        port, soc_errmsg(rv));
                return CMD_FAIL;
            }
        }

        l3_activities_running = FALSE;
    }
#endif /* INCLUDE_L3 && BCM_FIELD_SUPPORT */

    return CMD_OK;
}

const char *cmd_extt_parse_interface[] = {
    "RLDRAM.Ch0",
    "RLDRAM.Ch1",
    "SRAM",
    "QDR",
    "TCAM",
    "UnknownInterface",
    NULL
};

char cmd_extt_usage[] =
    "Parameters:\n\t"
    "[Interface=<extif>] [RandSeed=<rseed>] [InstallConfig=true|false]\n\t"
    "[TestCount=<num>] [SliceMask=<mask>] [FullTest=true|false] [Verbose=true|false]\n\t"
    "[DdrLatency=<num>] [EmLatency=<num>] [RxPhase=<num>] [TxPhase=<num>]\n\t"
    "\tPerforms a BIST analysis of common tuning parameters to find the\n\t"
    "\toptimal set.  Sets interface to successful parameters.\n\t"
    "\tIf FullTest is false, BIST testing will stop on first failure (for that setting).\n\t"
    "\tUse DdrLatency, EmLatency, RxPhase, TxPhase for manual settings.\n";
cmd_result_t
cmd_extt(int unit, args_t *a)
{
    parse_table_t      pt;
    soc_memtune_data_t mt_data;
    int                rv;
    soc_mem_interfaces interface;
    int                sub_interface;
    int                rseed;
    int                dump_config, make_config;
    int                summary;
    int                test_count;
    int                full_test;
    int                freq;
    char               *d0f_str, *d0r_str, *d1f_str, *d1r_str;
    char               *mask_str, *data_str, *bg_data_str[8];
    char               *bg_d0f_str, *bg_d0r_str, *bg_d1f_str, *bg_d1r_str;
    int                bg_l3, bg_l3_num_ge_pkt, bg_l3_num_xe_pkt;
    int                phase_sel, em_latency, ddr_latency;
    int                tx_offset, rx_offset, w2r_nops, r2w_nops;
    int                rbus_sync_dly, dpeo_sync_dly;
    int                fcd_dpeo, fcd_rbus, use_midl;
    int                invert_txclk, invert_rxclk;
    uint32             rval, fval;
    int                i, first_si, last_si;
    char               name_str[24], val_str[11], *prefix;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Interface", PQ_MULTI,
                    (void *)SOC_MEM_INTERFACE_COUNT, &interface,
                    cmd_extt_parse_interface);
    parse_table_add(&pt, "SubInterface", PQ_INT, (void *)-1, &sub_interface,
                    0);
    parse_table_add(&pt, "RandSeed", PQ_INT, (void *)0x12345678, &rseed, 0);
    parse_table_add(&pt, "InstallConfig", PQ_BOOL, (void *)TRUE,
                    &mt_data.config, 0);
    parse_table_add(&pt, "DumpConfig", PQ_BOOL, 0, &dump_config, 0);
    parse_table_add(&pt, "MakeConfig", PQ_BOOL, 0, &make_config, 0);
    parse_table_add(&pt, "Verbose", PQ_BOOL, 0, &mt_data.verbose, 0);
    parse_table_add(&pt, "SuppressFail", PQ_BOOL, (void *)TRUE,
                    &mt_data.suppress_fail, 0);
    parse_table_add(&pt, "SUmmary", PQ_BOOL, (void *)TRUE, &summary, 0);
    parse_table_add(&pt, "ShowMatrix", PQ_BOOL, 0, &mt_data.show_matrix, 0);
    parse_table_add(&pt, "ShowProgres", PQ_BOOL, 0, &mt_data.show_progress, 0);
    parse_table_add(&pt, "TestCount", PQ_INT, (void *)10, &test_count, 0);
    parse_table_add(&pt, "FullTest", PQ_BOOL, (void *)TRUE, &full_test, 0);
    parse_table_add(&pt, "DoPvtComp", PQ_BOOL, 0, &mt_data.do_pvt_comp, 0);
    parse_table_add(&pt, "FreQuency", PQ_INT, (void *)-1, &freq, 0);
    parse_table_add(&pt, "PhaseSelOvrd", PQ_BOOL, (void *)TRUE,
                    &mt_data.phase_sel_ovrd, 0);
    parse_table_add(&pt, "TestAllLatency", PQ_BOOL,
                    soc_feature(unit, soc_feature_esm_support) ?
                    (void *)TRUE : (void *)FALSE,
                    &mt_data.test_all_latency, 0);
    parse_table_add(&pt, "DoTxRxFirst", PQ_BOOL,
                    soc_feature(unit, soc_feature_esm_rxfifo_resync) ?
                    (void *)TRUE : (void *)FALSE, &mt_data.do_txrx_first, 0);
    parse_table_add(&pt, "D0F", PQ_STRING, 0, &d0f_str, 0);
    parse_table_add(&pt, "D0R", PQ_STRING, 0, &d0r_str, 0);
    parse_table_add(&pt, "D1F", PQ_STRING, 0, &d1f_str, 0);
    parse_table_add(&pt, "D1R", PQ_STRING, 0, &d1r_str, 0);
    parse_table_add(&pt, "Adr0", PQ_INT, (void *)-1, &mt_data.adr0, 0);
    parse_table_add(&pt, "Adr1", PQ_INT, (void *)-1, &mt_data.adr1, 0);
    parse_table_add(&pt, "AdrMode", PQ_INT, (void *)-1, &mt_data.adr_mode, 0);
    parse_table_add(&pt, "Mask", PQ_STRING, 0, &mask_str, 0);
    parse_table_add(&pt, "Data", PQ_STRING, 0, &data_str, 0);
    parse_table_add(&pt, "TcamLoopCount", PQ_INT, (void *)100,
                    &mt_data.tcam_loop_count, 0);

    parse_table_add(&pt, "BGSramBist", PQ_BOOL, 0, &mt_data.bg_sram_bist, 0);
    parse_table_add(&pt, "BGSramD0F", PQ_STRING, 0, &bg_d0f_str, 0);
    parse_table_add(&pt, "BGSramD0R", PQ_STRING, 0, &bg_d0r_str, 0);
    parse_table_add(&pt, "BGSramD1F", PQ_STRING, 0, &bg_d1f_str, 0);
    parse_table_add(&pt, "BGSramD1R", PQ_STRING, 0, &bg_d1r_str, 0);
    parse_table_add(&pt, "BGSramAdr0", PQ_INT, (void *)-1, &mt_data.bg_adr0,
                    0);
    parse_table_add(&pt, "BGSramAdr1", PQ_INT, (void *)-1, &mt_data.bg_adr1,
                    0);
    parse_table_add(&pt, "BGSramTestMode", PQ_INT, (void *)3,
                    &mt_data.bg_loop_mode, 0);
    parse_table_add(&pt, "BGSramAdrMode", PQ_INT, (void *)-1,
                    &mt_data.bg_adr_mode, 0);
    parse_table_add(&pt, "BGTcamBist", PQ_BOOL, 0, &mt_data.bg_tcam_bist, 0);
    parse_table_add(&pt, "BGTcamLoopCount", PQ_INT, (void *)1000,
                    &mt_data.bg_tcam_loop_count, 0);
    parse_table_add(&pt, "BGTcamData0", PQ_STRING, 0, &bg_data_str[0], 0);
    parse_table_add(&pt, "BGTcamData1", PQ_STRING, 0, &bg_data_str[1], 0);
    parse_table_add(&pt, "BGTcamData2", PQ_STRING, 0, &bg_data_str[2], 0);
    parse_table_add(&pt, "BGTcamData3", PQ_STRING, 0, &bg_data_str[3], 0);
    parse_table_add(&pt, "BGTcamData4", PQ_STRING, 0, &bg_data_str[4], 0);
    parse_table_add(&pt, "BGTcamData5", PQ_STRING, 0, &bg_data_str[5], 0);
    parse_table_add(&pt, "BGTcamData6", PQ_STRING, 0, &bg_data_str[6], 0);
    parse_table_add(&pt, "BGTcamData7", PQ_STRING, 0, &bg_data_str[7], 0);
    parse_table_add(&pt, "BGTcamOeMap", PQ_INT, 0, &mt_data.bg_tcam_oemap, 0);
    parse_table_add(&pt, "BGL3", PQ_BOOL, 0, &bg_l3, 0);
    parse_table_add(&pt, "BGL3NumGePkt", PQ_INT, (void *)300,
                    &bg_l3_num_ge_pkt, 0);
    parse_table_add(&pt, "BGL3NumXePkt", PQ_INT, (void *)80,
                    &bg_l3_num_xe_pkt, 0);

    parse_table_add(&pt, "PhaseSel", PQ_INT, (void *) -1, &phase_sel, 0);
    parse_table_add(&pt, "EmLatency", PQ_INT, (void *)-1, &em_latency, 0);
    parse_table_add(&pt, "DdrLatency", PQ_INT, (void *)-1, &ddr_latency, 0);
    parse_table_add(&pt, "TxPhase", PQ_INT, (void *)-1, &tx_offset, 0);
    parse_table_add(&pt, "RxPhase", PQ_INT, (void *)-1, &rx_offset, 0);
    parse_table_add(&pt, "W2RNops", PQ_INT, (void *)-1, &w2r_nops, 0);
    parse_table_add(&pt, "R2WNops", PQ_INT, (void *)-1, &r2w_nops, 0);
    parse_table_add(&pt, "RbusSyncDly", PQ_INT, (void *)-1, &rbus_sync_dly, 0);
    parse_table_add(&pt, "DpeoSyncDly", PQ_INT, (void *)-1, &dpeo_sync_dly, 0);
    parse_table_add(&pt, "FcdDpeo", PQ_INT, (void *)-1, &fcd_dpeo, 0);
    parse_table_add(&pt, "FcdRbus", PQ_INT, (void *)-1, &fcd_rbus, 0);
    parse_table_add(&pt, "UseMidl", PQ_INT, (void *)-1, &use_midl, 0);
    parse_table_add(&pt, "InvertTxclk", PQ_INT, (void *)-1, &invert_txclk, 0);
    parse_table_add(&pt, "InvertRxclk", PQ_INT, (void *)-1, &invert_rxclk, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }

    mt_data.d0f_0 = mt_data.d0f_1 = 0xffffffff;
    mt_data.d0r_0 = mt_data.d0r_1 = 0xffffffff;
    mt_data.d1f_0 = mt_data.d1f_1 = 0xffffffff;
    mt_data.d1r_0 = mt_data.d1r_1 = 0xffffffff;
    if (d0f_str[0] && d0r_str[0] && d1f_str[0] && d1r_str[0]) {
        _extt_hex_to_18bit_unit_bin(d0f_str, &mt_data.d0f_0, &mt_data.d0f_1);
        _extt_hex_to_18bit_unit_bin(d0r_str, &mt_data.d0r_0, &mt_data.d0r_1);
        _extt_hex_to_18bit_unit_bin(d1f_str, &mt_data.d1f_0, &mt_data.d1f_1);
        _extt_hex_to_18bit_unit_bin(d1r_str, &mt_data.d1r_0, &mt_data.d1r_1);
    }

    mt_data.bg_d0f_0 = mt_data.bg_d0f_1 = 0xffffffff;
    mt_data.bg_d0r_0 = mt_data.bg_d0r_1 = 0xffffffff;
    mt_data.bg_d1f_0 = mt_data.bg_d1f_1 = 0xffffffff;
    mt_data.bg_d1r_0 = mt_data.bg_d1r_1 = 0xffffffff;
    if (bg_d0f_str[0] && bg_d0r_str[0] && bg_d1f_str[0] && bg_d1r_str[0]) {
        _extt_hex_to_18bit_unit_bin(bg_d0f_str, &mt_data.bg_d0f_0,
                                    &mt_data.bg_d0f_1);
        _extt_hex_to_18bit_unit_bin(bg_d0r_str, &mt_data.bg_d0r_0,
                                    &mt_data.bg_d0r_1);
        _extt_hex_to_18bit_unit_bin(bg_d1f_str, &mt_data.bg_d1f_0,
                                    &mt_data.bg_d1f_1);
        _extt_hex_to_18bit_unit_bin(bg_d1r_str, &mt_data.bg_d1r_0,
                                    &mt_data.bg_d1r_1);
    }

    sal_memset(mt_data.mask, 0xff, sizeof(mt_data.mask));
    sal_memset(mt_data.data, 0, sizeof(mt_data.data));
    if (mask_str[0] && data_str[0]) {
        _extt_hex_to_72bit_unit_bin(mask_str, mt_data.mask, 2);
        _extt_hex_to_72bit_unit_bin(data_str, mt_data.data, 2);
    }

    sal_memset(mt_data.bg_tcam_data, 0, sizeof(mt_data.bg_tcam_data));
    for (i = 0; i < 8; i++) {
        if (bg_data_str[i]) {
            _extt_hex_to_72bit_unit_bin(bg_data_str[i],
                                        &mt_data.bg_tcam_data[i * 4], 1);
            mt_data.bg_tcam_data[i * 4] = mt_data.bg_tcam_data[i * 4 + 3];
            mt_data.bg_tcam_data[i * 4 + 3] = mt_data.bg_tcam_data[i * 4 + 1];
            mt_data.bg_tcam_data[i * 4 + 1] = mt_data.bg_tcam_data[i * 4 + 2];
            mt_data.bg_tcam_data[i * 4 + 2] = mt_data.bg_tcam_data[i * 4 + 3];
            mt_data.bg_tcam_data[i * 4 + 3] = 0;
        }
    }

    /* Don't need strings anymore */
    parse_arg_eq_done(&pt);

    if (dump_config) {
        if (soc_feature(unit, soc_feature_esm_support)) {
            for (i = 0; i < 2; i++) { /* ES0 and ES1 */
                sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING, i);
                rval = soc_property_get(unit, name_str, 0);
                cli_out("Tuning parameter (%s) for ES%d", name_str, i);
                if (!rval) {
                    cli_out(" is not in config\n");
                    continue;
                }
                prefix = i ? "SRAM1" : "SRAM0";
                cli_out("\n");
                cli_out("    %s, PhaseSelOvrd, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_OVRD_SM_SHIFT) &
                        SOC_TR_MEMTUNE_OVRD_SM_MASK);
                cli_out("    %s, PhaseSel, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_PHASE_SEL_SHIFT) &
                        SOC_TR_MEMTUNE_PHASE_SEL_MASK);
                fval = (rval >> SOC_TR_MEMTUNE_EM_LATENCY_SHIFT) &
                    SOC_TR_MEMTUNE_EM_LATENCY_MASK;
                cli_out("    %s, EmLatency, %d\n",
                        prefix, fval ? fval : 8);
                cli_out("    %s, DdrLatency, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT) &
                        SOC_TR_MEMTUNE_DDR_LATENCY_MASK);
                cli_out("    %s, TxPhase, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT) &
                        SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK);
                cli_out("    %s, RxPhase, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT) &
                        SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK);
                cli_out("    %s, W2RNops, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_W2R_NOPS_SHIFT) &
                        SOC_TR_MEMTUNE_W2R_NOPS_MASK);
                cli_out("    %s, R2WNops, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_R2W_NOPS_SHIFT) &
                        SOC_TR_MEMTUNE_R2W_NOPS_MASK);
                cli_out("    %s, FreQuency, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_DDR_FREQ_SHIFT) &
                        SOC_TR_MEMTUNE_DDR_FREQ_MASK);

                sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_PVT, i);
                rval = soc_property_get(unit, name_str, 0);
                if (rval) {
                    cli_out("    %s, OvrdOdtRes, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_OVRD_ODTRES_SHIFT) &
                            SOC_TR_MEMTUNE_OVRD_ODTRES_MASK));
                    cli_out("    %s, OdtRes, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_ODTRES_SHIFT) &
                            SOC_TR_MEMTUNE_ODTRES_MASK));
                    cli_out("    %s, OvrdDriver, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_OVRD_DRIVER_SHIFT) &
                            SOC_TR_MEMTUNE_OVRD_DRIVER_MASK));
                    cli_out("    %s, PDRive, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_PDRIVE_SHIFT) &
                            SOC_TR_MEMTUNE_PDRIVE_MASK));
                    cli_out("    %s, NDRive, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_NDRIVE_SHIFT) &
                            SOC_TR_MEMTUNE_NDRIVE_MASK));
                    cli_out("    %s, OvrdSLew, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_OVRD_SLEW_SHIFT) &
                            SOC_TR_MEMTUNE_OVRD_SLEW_MASK));
                    cli_out("    %s, SLew, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_SLEW_SHIFT) &
                            SOC_TR_MEMTUNE_SLEW_MASK));
                }

                sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING_STATS, i);
                rval = soc_property_get(unit, name_str, 0);
                if (rval) {
                    cli_out("    %s, (STATS) Area, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_WIDTH_MASK) *
                            ((rval >> SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_HEIGHT_MASK));
                    cli_out("    %s, (STATS) Width, %d\n",
                            prefix,
                            (rval >> SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_WIDTH_MASK);
                    cli_out("    %s, (STATS) Height, %d\n",
                            prefix,
                            (rval >> SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_HEIGHT_MASK);
                    cli_out("    %s, (STATS) FailCount, %d\n",
                            prefix,
                            (rval >> SOC_TR_MEMTUNE_STATS_FAIL_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_FAIL_MASK);
                }
                sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING2_STATS, i);
                rval = soc_property_get(unit, name_str, 0);
                if (rval) {
                    cli_out("    %s, (STATS2) Area, %d\n",
                            prefix,
                            ((rval >> SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_WIDTH_MASK) *
                            ((rval >> SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_HEIGHT_MASK));
                    cli_out("    %s, (STATS2) Width, %d\n",
                            prefix,
                            (rval >> SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_WIDTH_MASK);
                    cli_out("    %s, (STATS2) Height, %d\n",
                            prefix,
                            (rval >> SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_HEIGHT_MASK);
                    cli_out("    %s, (STATS2) FailCount, %d\n",
                            prefix,
                            (rval >> SOC_TR_MEMTUNE_STATS_FAIL_SHIFT) &
                            SOC_TR_MEMTUNE_STATS_FAIL_MASK);
                }
            }

            sal_sprintf(name_str, "%s0", spn_EXT_TCAM_TUNING);
            rval = soc_property_get(unit, name_str, 0);
            cli_out("Tuning parameter (%s) for TCAM (part 1)", name_str);
            if (!rval) {
                cli_out(" is not in config\n");
                return CMD_OK;
            }
            prefix = "TCAM";
            cli_out("\n");
            cli_out("    %s, RbusSyncDly, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT) &
                    SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK);
            cli_out("    %s, DpeoSyncDly, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT) &
                    SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK);
            cli_out("    %s, FcdDpeo, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_FCD_DPEO_SHIFT) &
                    SOC_TR_MEMTUNE_FCD_DPEO_MASK);
            cli_out("    %s, FcdRbus, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_FCD_RBUS_SHIFT) &
                    SOC_TR_MEMTUNE_FCD_RBUS_MASK);

            sal_sprintf(name_str, "%s1", spn_EXT_TCAM_TUNING);
            rval = soc_property_get(unit, name_str, 0);
            cli_out("Tuning parameter (%s) for TCAM (part 2)", name_str);
            if (!rval) {
                cli_out(" is not in config\n");
                return CMD_OK;
            }
            cli_out("\n");
            cli_out("    %s, TxPhase, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT) &
                    SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK);
            cli_out("    %s, RxPhase, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT) &
                    SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK);
            cli_out("    %s, FreQuency, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT) &
                    SOC_TR_MEMTUNE_TCAM_FREQ_MASK);
            cli_out("    %s, UseMidl, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_USE_MIDL_SHIFT) &
                    SOC_TR_MEMTUNE_USE_MIDL_MASK);
            cli_out("    %s, InvertTxclk, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_INVERT_TXCLK_SHIFT) &
                    SOC_TR_MEMTUNE_INVERT_TXCLK_MASK);
            cli_out("    %s, InvertRxclk, %d\n",
                    prefix,
                    (rval >> SOC_TR_MEMTUNE_INVERT_RXCLK_SHIFT) &
                    SOC_TR_MEMTUNE_INVERT_RXCLK_MASK);

            sal_sprintf(name_str, "%s", spn_EXT_TCAM_PVT);
            rval = soc_property_get(unit, name_str, 0);
            if (rval) {
                cli_out("    %s, OvrdOdtRes, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_OVRD_ODTRES_SHIFT) &
                        SOC_TR_MEMTUNE_OVRD_ODTRES_MASK));
                cli_out("    %s, OdtRes, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_ODTRES_SHIFT) &
                        SOC_TR_MEMTUNE_ODTRES_MASK));
                cli_out("    %s, OvrdDriver, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_OVRD_DRIVER_SHIFT) &
                        SOC_TR_MEMTUNE_OVRD_DRIVER_MASK));
                cli_out("    %s, PDRive, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_PDRIVE_SHIFT) &
                        SOC_TR_MEMTUNE_PDRIVE_MASK));
                cli_out("    %s, NDRive, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_NDRIVE_SHIFT) &
                        SOC_TR_MEMTUNE_NDRIVE_MASK));
                cli_out("    %s, OvrdSLew, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_OVRD_SLEW_SHIFT) &
                        SOC_TR_MEMTUNE_OVRD_SLEW_MASK));
                cli_out("    %s, SLew, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_SLEW_SHIFT) &
                        SOC_TR_MEMTUNE_SLEW_MASK));
            }

            sal_sprintf(name_str, "%s", spn_EXT_TCAM_TUNING_STATS);
            rval = soc_property_get(unit, name_str, 0);
            if (rval) {
                cli_out("    %s, (STATS) Ares, %d\n",
                        prefix,
                        ((rval >> SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) &
                        SOC_TR_MEMTUNE_STATS_WIDTH_MASK) *
                        ((rval >> SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) &
                        SOC_TR_MEMTUNE_STATS_HEIGHT_MASK));
                cli_out("    %s, (STATS) Width, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) &
                        SOC_TR_MEMTUNE_STATS_WIDTH_MASK);
                cli_out("    %s, (STATS) Height, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) &
                        SOC_TR_MEMTUNE_STATS_HEIGHT_MASK);
                cli_out("    %s, (STATS) FailCount, %d\n",
                        prefix,
                        (rval >> SOC_TR_MEMTUNE_STATS_FAIL_SHIFT) &
                        SOC_TR_MEMTUNE_STATS_FAIL_MASK);
            }
            return CMD_OK;
        }
        return CMD_FAIL;
    }
    if (make_config) {
        if (soc_feature(unit, soc_feature_esm_support)) {
            switch (interface) {
            case SOC_MEM_INTERFACE_SRAM:
                if (sub_interface == -1) {
                    first_si = 0;
                    last_si = 1;
                } else if (sub_interface == 0 || sub_interface == 1) {
                    first_si = last_si = sub_interface;
                } else {
                    cli_out("Invalid SubInterface\n");
                    return CMD_FAIL;
                }
                for (i = first_si; i <= last_si; i++) { /* ES0 and/or ES1 */
                    sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING, i);
                    rval = soc_property_get(unit, name_str, 0);
                    if (phase_sel != -1) {
                        rval |= (SOC_TR_MEMTUNE_OVRD_SM_MASK <<
                                 SOC_TR_MEMTUNE_OVRD_SM_SHIFT);
                        rval &= ~(SOC_TR_MEMTUNE_PHASE_SEL_MASK <<
                                  SOC_TR_MEMTUNE_PHASE_SEL_SHIFT);
                        rval |= ((phase_sel & SOC_TR_MEMTUNE_PHASE_SEL_MASK) <<
                                 SOC_TR_MEMTUNE_PHASE_SEL_SHIFT);
                    }
                    if (em_latency != -1) {
                        rval &= ~(SOC_TR_MEMTUNE_EM_LATENCY_MASK <<
                                  SOC_TR_MEMTUNE_EM_LATENCY_SHIFT);
                        rval |= ((em_latency &
                                  SOC_TR_MEMTUNE_EM_LATENCY_MASK) <<
                                 SOC_TR_MEMTUNE_EM_LATENCY_SHIFT);
                    }
                    if (ddr_latency != -1) {
                        rval &= ~(SOC_TR_MEMTUNE_DDR_LATENCY_MASK <<
                                  SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT);
                        rval |= ((ddr_latency &
                                  SOC_TR_MEMTUNE_DDR_LATENCY_MASK) <<
                                 SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT);
                    }
                    if (tx_offset != -1) {
                        rval &= ~(SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK <<
                                  SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT);
                        rval |= ((tx_offset &
                                  SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK) <<
                                 SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT);
                    }
                    if (rx_offset != -1) {
                        rval &= ~(SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK <<
                                  SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT);
                        rval |= ((rx_offset &
                                  SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK) <<
                                 SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT);
                    }
                    if (w2r_nops != -1) {
                        rval &= ~(SOC_TR_MEMTUNE_W2R_NOPS_MASK <<
                                  SOC_TR_MEMTUNE_W2R_NOPS_SHIFT);
                        rval |= ((w2r_nops & SOC_TR_MEMTUNE_W2R_NOPS_MASK) <<
                                 SOC_TR_MEMTUNE_W2R_NOPS_SHIFT);
                    }
                    if (r2w_nops != -1) {
                        rval &= ~(SOC_TR_MEMTUNE_R2W_NOPS_MASK <<
                                  SOC_TR_MEMTUNE_R2W_NOPS_SHIFT);
                        rval |= ((r2w_nops & SOC_TR_MEMTUNE_R2W_NOPS_MASK) <<
                                 SOC_TR_MEMTUNE_R2W_NOPS_SHIFT);
                    }
                    if (freq != -1) {
                        rval &= ~(SOC_TR_MEMTUNE_DDR_FREQ_MASK <<
                                  SOC_TR_MEMTUNE_DDR_FREQ_SHIFT);
                        rval |= ((freq & SOC_TR_MEMTUNE_DDR_FREQ_MASK) <<
                                 SOC_TR_MEMTUNE_DDR_FREQ_SHIFT);
                    }
                    sal_sprintf(val_str, "0x%08x", rval);
                    if (sal_config_set(name_str, val_str) < 0) {
                        cli_out("Write config fail");
                        return CMD_FAIL;
                    }
                }
                return CMD_OK;
            case SOC_MEM_INTERFACE_TCAM:
                sal_sprintf(name_str, "%s0", spn_EXT_TCAM_TUNING);
                rval = soc_property_get(unit, name_str, 0);
                if (rbus_sync_dly != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK <<
                              SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT);
                    rval |= ((rbus_sync_dly &
                              SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK) <<
                             SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT);
                }
                if (dpeo_sync_dly != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK <<
                              SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT);
                    rval |= ((dpeo_sync_dly &
                              SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK) <<
                             SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT);
                }
                if (fcd_dpeo != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_FCD_DPEO_MASK <<
                              SOC_TR_MEMTUNE_FCD_DPEO_SHIFT);
                    rval |= ((fcd_dpeo & SOC_TR_MEMTUNE_FCD_DPEO_MASK) <<
                             SOC_TR_MEMTUNE_FCD_DPEO_SHIFT);
                }
                if (fcd_rbus != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_FCD_RBUS_MASK <<
                              SOC_TR_MEMTUNE_FCD_RBUS_SHIFT);
                    rval |= ((fcd_rbus & SOC_TR_MEMTUNE_FCD_RBUS_MASK) <<
                             SOC_TR_MEMTUNE_FCD_RBUS_SHIFT);
                }
                sal_sprintf(val_str, "0x%08x", rval);
                if (sal_config_set(name_str, val_str) < 0) {
                    cli_out("Write config fail");
                    return CMD_FAIL;
                }
                sal_sprintf(name_str, "%s1", spn_EXT_TCAM_TUNING);
                rval = soc_property_get(unit, name_str, 0);
                if (tx_offset != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK <<
                              SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT);
                    rval |= ((tx_offset &
                              SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK) <<
                             SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT);
                }
                if (rx_offset != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK <<
                              SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT);
                    rval |= ((rx_offset &
                              SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK) <<
                             SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT);
                }
                if (freq != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_TCAM_FREQ_MASK <<
                              SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT);
                    rval |= ((freq & SOC_TR_MEMTUNE_TCAM_FREQ_MASK) <<
                             SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT);
                }
                if (use_midl != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_USE_MIDL_MASK <<
                              SOC_TR_MEMTUNE_USE_MIDL_SHIFT);
                    rval |= ((use_midl & SOC_TR_MEMTUNE_USE_MIDL_MASK) <<
                             SOC_TR_MEMTUNE_USE_MIDL_SHIFT);
                }
                if (invert_txclk != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_INVERT_TXCLK_MASK <<
                              SOC_TR_MEMTUNE_INVERT_TXCLK_SHIFT);
                    rval |= ((invert_txclk &
                              SOC_TR_MEMTUNE_INVERT_TXCLK_MASK) <<
                             SOC_TR_MEMTUNE_INVERT_TXCLK_SHIFT);
                }
                if (invert_rxclk != -1) {
                    rval &= ~(SOC_TR_MEMTUNE_INVERT_RXCLK_MASK <<
                              SOC_TR_MEMTUNE_INVERT_RXCLK_SHIFT);
                    rval |= ((invert_rxclk &
                              SOC_TR_MEMTUNE_INVERT_RXCLK_MASK) <<
                             SOC_TR_MEMTUNE_INVERT_RXCLK_SHIFT);
                }
                sal_sprintf(val_str, "0x%08x", rval);
                if (sal_config_set(name_str, val_str) < 0) {
                    cli_out("Write config fail");
                    return CMD_FAIL;
                }
                return CMD_OK;
            default:
                break;
            }
        }
        return CMD_FAIL;
    }

    mt_data.interface = interface;
    mt_data.sub_interface = sub_interface;
    mt_data.rseed = rseed;
    mt_data.summary = summary;
    mt_data.summary_header = summary;
    mt_data.test_count = test_count;
    mt_data.max_fail_count = full_test ? test_count : 1;
    mt_data.freq = freq;

    mt_data.manual_settings = 0;
    mt_data.man_em_latency = em_latency;
    mt_data.man_ddr_latency = ddr_latency;
    mt_data.man_tx_offset = tx_offset;
    mt_data.man_rx_offset = rx_offset;

    if (soc_feature(unit, soc_feature_esm_support)) {
        if (mt_data.interface == SOC_MEM_INTERFACE_SRAM) {
            mt_data.slice_mask = SOC_TR_SRAM_SLICE_MASK;
        }
    }

    if (soc_feature(unit, soc_feature_esm_support)) {
        if (mt_data.interface == SOC_MEM_INTERFACE_TCAM) {
            if (tx_offset != -1 && rx_offset != -1) {
                mt_data.verbose = TRUE;
                mt_data.suppress_fail = FALSE;
                mt_data.manual_settings = 1;
            }
        } else if (mt_data.interface == SOC_MEM_INTERFACE_SRAM) {
            if (ddr_latency != -1 &&em_latency != -1 &&
                rx_offset != -1 && tx_offset != -1) {
                mt_data.verbose = TRUE;
                mt_data.suppress_fail = FALSE;
                mt_data.manual_settings = 1;
                mt_data.man_phase_sel = phase_sel;
                mt_data.man_w2r_nops = w2r_nops;
                mt_data.man_r2w_nops = r2w_nops;
            }
        }
    } else if ((ddr_latency != -1) || (em_latency != -1) ||
        (rx_offset != -1) || (tx_offset != -1)) {

        if ((ddr_latency == -1) || (rx_offset == -1) || (tx_offset == -1)) {
            cli_out("%s: Must specify %s %s %s parameter(s).\n",
                    ARG_CMD(a),
                    (ddr_latency == -1) ? "DdrLatency" : "",
                    (rx_offset == -1) ? "RxPhase" : "",
                    (tx_offset == -1) ? "TxPhase" : "");
            goto fail;
        }

        mt_data.verbose = TRUE;
        mt_data.suppress_fail = FALSE;
        mt_data.manual_settings = 1;
        mt_data.man_phase_sel = phase_sel;
        mt_data.man_w2r_nops = w2r_nops;
        mt_data.man_r2w_nops = r2w_nops;
    }

    if (bg_l3) {
        _extt_l3_activities(unit, TRUE, bg_l3_num_ge_pkt, bg_l3_num_xe_pkt);
    }

    if ((rv = soc_mem_interface_tune(unit, &mt_data)) < 0) {
        cli_out("ERROR: unit %d, tuning %s: %s\n",
                unit, cmd_extt_parse_interface[interface], soc_errmsg(rv));
    }

    if (bg_l3) {
        _extt_l3_activities(unit, FALSE, 0, 0);
    }

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_support) &&
        mt_data.interface == SOC_MEM_INTERFACE_TCAM) {
        if (soc_triumph_esm_init_read_config(unit) < 0) {
            cli_out("ERROR: unit %d, restore TCAM config fail\n", unit);
            goto fail;
        }
        if (soc_tcam_init(unit) < 0) {
            cli_out("ERROR: unit %d, re-init TCAM fail\n", unit);
            goto fail;
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */
 
    return rv >= 0 ? CMD_OK : CMD_FAIL;

fail:
    parse_arg_eq_done(&pt);
    return CMD_FAIL;

}

char cmd_extts_usage[] =
    "Parameters:\n\t"
    "[TestCount=<num>] [InstallConfig=true|false]\n\t"
    "[PhaseSelOvrd=true|false]\n";
cmd_result_t
cmd_extts(int unit, args_t *a)
{
    int         test_count = SOC_ER_DDR_BIST_COUNT; 
    int         phase_sel_ovrd = TRUE; 
    int         config = FALSE;
    parse_table_t      pt;
    int                i, rv;
    soc_memtune_data_t mt_data;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "TestCount", PQ_INT | PQ_DFL, 0, &test_count, 0);
    parse_table_add(&pt, "InstallConfig", PQ_BOOL | PQ_DFL, 0, &config, 0);
    parse_table_add(&pt, "PhaseSelOvrd", PQ_BOOL | PQ_DFL, 0, &phase_sel_ovrd, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }

    /* Don't need strings anymore */
    parse_arg_eq_done(&pt);
    
    cli_out("Running external memory tuning on all interfaces. Test count = %d.\n",
            test_count);

    for (i=0; i < SOC_MEM_INTERFACE_COUNT; i++) {
        mt_data.interface = i;
        mt_data.rseed = 0x12345678;
        mt_data.config = config;
        mt_data.test_count = test_count;
        mt_data.max_fail_count = test_count;
        mt_data.slice_mask = (i == SOC_MEM_INTERFACE_QDR) ? 
                            SOC_ER_QDR_SLICE_MASK : SOC_ER_RLDRAM_SLICE_MASK;
        mt_data.verbose = 0;
        mt_data.suppress_fail = 0;
        mt_data.manual_settings = 0;
        mt_data.summary = TRUE;
        mt_data.summary_header = i == 0 ? TRUE : FALSE;
        mt_data.phase_sel_ovrd = phase_sel_ovrd;

        rv = soc_mem_interface_tune(unit, &mt_data);
        if (rv < 0 && rv != SOC_E_UNAVAIL) {
            cli_out("ERROR: unit %d, tuning %s: %s\n",
                    unit, cmd_extt_parse_interface[i], soc_errmsg(rv));
        }
    }

    return CMD_OK;
}

char cmd_extt2_usage[] =
    "Parameters:\n\t"
    "[InstallConfig=true|false] [Verbose=true|false] [SUmmary=true|false]\n\t"
    "[TestCount=<num>] [UseTestList=true|false] [TestListID=<num>]\n\t"
    "[DeLTa0=<num>][DeLTa1=<num>]\n";

#define EXTT2_START_ACCUMULATE    0x01
#define EXTT2_INSTALL_CONFIG      0x02
#define EXTT2_ENABLE_TCAM_ODT     0x04
#define EXTT2_DISABLE_TCAM_ODT    0x08
#define EXTT2_START_L3_TRAFFIC    0x10
#define EXTT2_STOP_L3_TRAFFIC     0x20

typedef struct extt2_test_list_s {
    char *test_name;
    int loop_mode[2];
    int alt_adr;
    int tcam_data_choice;
    int sram_data_choice;
    uint32 event;
} extt2_test_list_t;

/* Test list ID 0 : full test */
extt2_test_list_t extt2_full_test_list[] = {
    /* test_name       ,loop_mode,aa,  tdc,sdc, event */
    { "10"             , { 3, 3 }, 1,    1,  1, EXTT2_START_ACCUMULATE },
    { "10_SDC2"        , { 3, 3 }, 1,    1,  2, 0 },
    { "10_SDC3"        , { 3, 3 }, 1,    1,  3, 0 },
    { "10_SDC7"        , { 3, 3 }, 1,    1,  7, 0 },
    { "10_SDC8"        , { 3, 3 }, 1,    1,  8, 0 },
    { "10_SDC3_AA3"    , { 3, 3 }, 3,    1,  3, 0 },
    { "10_SDC7_AA3"    , { 3, 3 }, 3,    1,  7, 0 },
    { "10_SDC8_AA3"    , { 3, 3 }, 3,    1,  8, 0 },
    { "11"             , { 3, 4 }, 1,    1,  1, 0 },
    { "11_SDC2"        , { 3, 4 }, 1,    1,  2, 0 },
    { "11_SDC3"        , { 3, 4 }, 1,    1,  3, 0 },
    { "11_SDC7"        , { 3, 4 }, 1,    1,  7, 0 },
    { "11_SDC8"        , { 3, 4 }, 1,    1,  8, 0 },
    { "11_SDC3_AA3"    , { 3, 4 }, 3,    1,  3, 0 },
    { "11_SDC7_AA3"    , { 3, 4 }, 3,    1,  7, 0 },
    { "11_SDC8_AA3"    , { 3, 4 }, 3,    1,  8, 0 },
    { "12"             , { 4, 3 }, 1,    1,  1, 0 },
    { "12_SDC2"        , { 4, 3 }, 1,    1,  2, 0 },
    { "12_SDC3"        , { 4, 3 }, 1,    1,  3, 0 },
    { "12_SDC7"        , { 4, 3 }, 1,    1,  7, 0 },
    { "12_SDC8"        , { 4, 3 }, 1,    1,  8, 0 },
    { "12_SDC3_AA3"    , { 4, 3 }, 3,    1,  3, 0 },
    { "12_SDC7_AA3"    , { 4, 3 }, 3,    1,  7, 0 },
    { "12_SDC8_AA3"    , { 4, 3 }, 3,    1,  8, 0 },
    { "14_1"           , { 1, 1 }, 1,    6,  1, 0 },
    { "14_1_SDC2"      , { 1, 1 }, 1,    6,  2, 0 },
    { "14_1_SDC3"      , { 1, 1 }, 1,    6,  3, 0 },
    { "14_1_SDC7"      , { 1, 1 }, 1,    6,  7, 0 },
    { "14_1_SDC8"      , { 1, 1 }, 1,    6,  8, 0 },
    { "14_1_SDC3_AA3"  , { 1, 1 }, 3,    6,  3, 0 },
    { "14_1_SDC7_AA3"  , { 1, 1 }, 3,    6,  7, 0 },
    { "14_1_SDC8_AA3"  , { 1, 1 }, 3,    6,  8, 0 },
    { "140_1"          , { 1, 1 }, 1,    7,  1, 0 },
    { "140_1_SDC2"     , { 1, 1 }, 1,    7,  2, 0 },
    { "140_1_SDC3"     , { 1, 1 }, 1,    7,  3, 0 },
    { "140_1_SDC7"     , { 1, 1 }, 1,    7,  7, 0 },
    { "140_1_SDC8"     , { 1, 1 }, 1,    7,  8, 0 },
    { "140_1_SDC3_AA3" , { 1, 1 }, 3,    7,  3, 0 },
    { "140_1_SDC7_AA3" , { 1, 1 }, 3,    7,  7, 0 },
    { "140_1_SDC8_AA3" , { 1, 1 }, 3,    7,  8, 0 },
    { "13"             , { 3, 3 }, 1, 2000,  1, 0 },
    { "13_SDC2"        , { 3, 3 }, 1, 2000,  2, 0 },
    { "13_SDC3"        , { 3, 3 }, 1, 2000,  3, 0 },
    { "13_SDC7"        , { 3, 3 }, 1, 2000,  7, 0 },
    { "13_SDC8"        , { 3, 3 }, 1, 2000,  8, 0 },
    { "13_SDC3_AA3"    , { 3, 3 }, 3, 2000,  3, 0 },
    { "13_SDC7_AA3"    , { 3, 3 }, 3, 2000,  7, 0 },
    { "13_SDC8_AA3"    , { 3, 3 }, 3, 2000,  8, 0 },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_START_L3_TRAFFIC },
    { "13_1"           , { 3, 3 }, 1, 2000,  1, 0 },
    { "13_1_SDC2"      , { 3, 3 }, 1, 2000,  2, 0 },
    { "13_1_SDC3"      , { 3, 3 }, 1, 2000,  3, 0 },
    { "13_1_SDC7"      , { 3, 3 }, 1, 2000,  7, 0 },
    { "13_1_SDC8"      , { 3, 3 }, 1, 2000,  8, 0 },
    { "13_1_SDC3_AA3"  , { 3, 3 }, 3, 2000,  3, 0 },
    { "13_1_SDC7_AA3"  , { 3, 3 }, 3, 2000,  7, 0 },
    { "13_1_SDC8_AA3"  , { 3, 3 }, 3, 2000,  8, 0 },
    { "15_RR"          , { 1, 1 }, 1,    1,  1, 0 },
    { "15_S0_RR"       , { 1, 3 }, 1,    1,  1, 0 },
    { "15_S1_RR"       , { 3, 1 }, 1,    1,  1, 0 },
    { "15"             , { 3, 3 }, 1,    1,  1, 0 },
    { "15_SDC2"        , { 3, 3 }, 1,    1,  2, 0 },
    { "15_SDC3"        , { 3, 3 }, 1,    1,  3, 0 },
    { "15_SDC7"        , { 3, 3 }, 1,    1,  7, 0 },
    { "15_SDC8"        , { 3, 3 }, 1,    1,  8, 0 },
    { "15_SDC3_AA3"    , { 3, 3 }, 3,    1,  3, 0 },
    { "15_SDC7_AA3"    , { 3, 3 }, 3,    1,  7, 0 },
    { "15_SDC8_AA3"    , { 3, 3 }, 3,    1,  8, EXTT2_INSTALL_CONFIG },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_STOP_L3_TRAFFIC },
};

extt2_test_list_t extt2_old_full_test_list[] = {
    /* test_name       ,loop_mode,aa,  tdc,sdc, event */
    { "10"             , { 3, 3 }, 1,    1,  1, EXTT2_START_ACCUMULATE },
    { "10_SDC2"        , { 3, 3 }, 1,    1,  2, 0 },
    { "10_SDC3"        , { 3, 3 }, 1,    1,  3, 0 },
    { "10_SDC3_AA3"    , { 3, 3 }, 3,    1,  3, 0 },
    { "11"             , { 3, 4 }, 1,    1,  1, 0 },
    { "11_SDC2"        , { 3, 4 }, 1,    1,  2, 0 },
    { "11_SDC3"        , { 3, 4 }, 1,    1,  3, 0 },
    { "11_SDC3_AA3"    , { 3, 4 }, 3,    1,  3, 0 },
    { "12"             , { 4, 3 }, 1,    1,  1, 0 },
    { "12_SDC2"        , { 4, 3 }, 1,    1,  2, 0 },
    { "12_SDC3"        , { 4, 3 }, 1,    1,  3, 0 },
    { "12_SDC3_AA3"    , { 4, 3 }, 3,    1,  3, 0 },
    { "14_1"           , { 1, 1 }, 1,    6,  1, 0 },
    { "14_1_SDC2"      , { 1, 1 }, 1,    6,  2, 0 },
    { "14_1_SDC3"      , { 1, 1 }, 1,    6,  3, 0 },
    { "14_1_SDC3_AA3"  , { 1, 1 }, 3,    6,  3, 0 },
    { "140_1"          , { 1, 1 }, 1,    7,  1, 0 },
    { "140_1_SDC2"     , { 1, 1 }, 1,    7,  2, 0 },
    { "140_1_SDC3"     , { 1, 1 }, 1,    7,  3, 0 },
    { "140_1_SDC3_AA3" , { 1, 1 }, 3,    7,  3, 0 },
    { "13"             , { 3, 3 }, 1, 2000,  1, 0 },
    { "13_SDC2"        , { 3, 3 }, 1, 2000,  2, 0 },
    { "13_SDC3"        , { 3, 3 }, 1, 2000,  3, 0 },
    { "13_SDC3_AA3"    , { 3, 3 }, 3, 2000,  3, 0 },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_START_L3_TRAFFIC },
    { "13_1"           , { 3, 3 }, 1, 2000,  1, 0 },
    { "13_1_SDC2"      , { 3, 3 }, 1, 2000,  2, 0 },
    { "13_1_SDC3"      , { 3, 3 }, 1, 2000,  3, 0 },
    { "13_1_SDC3_AA3"  , { 3, 3 }, 3, 2000,  3, 0 },
    { "15_RR"          , { 1, 1 }, 1,    1,  1, 0 },
    { "15_S0_RR"       , { 1, 3 }, 1,    1,  1, 0 },
    { "15_S1_RR"       , { 3, 1 }, 1,    1,  1, 0 },
    { "15"             , { 3, 3 }, 1,    1,  1, 0 },
    { "15_SDC2"        , { 3, 3 }, 1,    1,  2, 0 },
    { "15_SDC3"        , { 3, 3 }, 1,    1,  3, 0 },
    { "15_SDC3_AA3"    , { 3, 3 }, 3,    1,  3, EXTT2_INSTALL_CONFIG },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_STOP_L3_TRAFFIC },
};

/* Test list ID 1 : short test */
extt2_test_list_t extt2_short_test_list[] = {
    /* test_name       ,loop_mode,aa,  tdc,sdc, event */
    { "14_1"           , { 1, 1 }, 1,    6,  1, EXTT2_START_ACCUMULATE },
    { "14_1_SDC2"      , { 1, 1 }, 1,    6,  2, 0 },
    { "14_1_SDC3"      , { 1, 1 }, 1,    6,  3, 0 },
    { "14_1_SDC3_AA3"  , { 1, 1 }, 3,    6,  3, 0 },
    { "140_1"          , { 1, 1 }, 1,    7,  1, 0 },
    { "140_1_SDC2"     , { 1, 1 }, 1,    7,  2, 0 },
    { "140_1_SDC3"     , { 1, 1 }, 1,    7,  3, 0 },
    { "140_1_SDC3_AA3" , { 1, 1 }, 3,    7,  3, 0 },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_START_L3_TRAFFIC },
    { "15_RR"          , { 1, 1 }, 1,    1,  1, 0 },
    { "15_S0_RR"       , { 1, 3 }, 1,    1,  1, 0 },
    { "15_S1_RR"       , { 3, 1 }, 1,    1,  1, 0 },
    { "15"             , { 3, 3 }, 1,    1,  1, 0 },
    { "15_SDC2"        , { 3, 3 }, 1,    1,  2, 0 },
    { "15_SDC3"        , { 3, 3 }, 1,    1,  3, 0 },
    { "15_SDC3_AA3"    , { 3, 3 }, 3,    1,  3, EXTT2_INSTALL_CONFIG },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_STOP_L3_TRAFFIC },
};

extt2_test_list_t extt2_full_test_list_sram0[] = {
    /* test_name       ,loop_mode,aa,  tdc,sdc, event */
    { "11"             , { 3, 4 }, 1,    1,  1, EXTT2_START_ACCUMULATE },
    { "11_SDC2"        , { 3, 4 }, 1,    1,  2, 0 },
    { "11_SDC3"        , { 3, 4 }, 1,    1,  3, 0 },
    { "11_SDC3_AA3"    , { 3, 4 }, 3,    1,  3, 0 },
    { "14_1"           , { 1, 4 }, 1,    6,  1, 0 },
    { "14_1_SDC2"      , { 1, 4 }, 1,    6,  2, 0 },
    { "14_1_SDC3"      , { 1, 4 }, 1,    6,  3, 0 },
    { "14_1_SDC3_AA3"  , { 1, 4 }, 3,    6,  3, 0 },
    { "140_1"          , { 1, 4 }, 1,    7,  1, 0 },
    { "140_1_SDC2"     , { 1, 4 }, 1,    7,  2, 0 },
    { "140_1_SDC3"     , { 1, 4 }, 1,    7,  3, 0 },
    { "140_1_SDC3_AA3" , { 1, 4 }, 3,    7,  3, 0 },
    { "13"             , { 3, 4 }, 1, 2000,  1, 0 },
    { "13_SDC2"        , { 3, 4 }, 1, 2000,  2, 0 },
    { "13_SDC3"        , { 3, 4 }, 1, 2000,  3, 0 },
    { "13_SDC3_AA3"    , { 3, 4 }, 3, 2000,  3, 0 },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_START_L3_TRAFFIC },
    { "13_1"           , { 3, 4 }, 1, 2000,  1, 0 },
    { "13_1_SDC2"      , { 3, 4 }, 1, 2000,  2, 0 },
    { "13_1_SDC3"      , { 3, 4 }, 1, 2000,  3, 0 },
    { "13_1_SDC3_AA3"  , { 3, 4 }, 3, 2000,  3, 0 },
    { "15_RR"          , { 1, 4 }, 1,    1,  1, 0 },
    { "15"             , { 3, 4 }, 1,    1,  1, 0 },
    { "15_SDC2"        , { 3, 4 }, 1,    1,  2, 0 },
    { "15_SDC3"        , { 3, 4 }, 1,    1,  3, 0 },
    { "15_SDC3_AA3"    , { 3, 4 }, 3,    1,  3, EXTT2_INSTALL_CONFIG },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_STOP_L3_TRAFFIC },
};

extt2_test_list_t extt2_full_test_list_sram1[] = {
    /* test_name       ,loop_mode,aa,  tdc,sdc, event */
    { "12"             , { 4, 3 }, 1,    1,  1, EXTT2_START_ACCUMULATE },
    { "12_SDC2"        , { 4, 3 }, 1,    1,  2, 0 },
    { "12_SDC3"        , { 4, 3 }, 1,    1,  3, 0 },
    { "12_SDC3_AA3"    , { 4, 3 }, 3,    1,  3, 0 },
    { "14_1"           , { 4, 1 }, 1,    6,  1, 0 },
    { "14_1_SDC2"      , { 4, 1 }, 1,    6,  2, 0 },
    { "14_1_SDC3"      , { 4, 1 }, 1,    6,  3, 0 },
    { "14_1_SDC3_AA3"  , { 4, 1 }, 3,    6,  3, 0 },
    { "140_1"          , { 4, 1 }, 1,    7,  1, 0 },
    { "140_1_SDC2"     , { 4, 1 }, 1,    7,  2, 0 },
    { "140_1_SDC3"     , { 4, 1 }, 1,    7,  3, 0 },
    { "140_1_SDC3_AA3" , { 4, 1 }, 3,    7,  3, 0 },
    { "13"             , { 4, 3 }, 1, 2000,  1, 0 },
    { "13_SDC2"        , { 4, 3 }, 1, 2000,  2, 0 },
    { "13_SDC3"        , { 4, 3 }, 1, 2000,  3, 0 },
    { "13_SDC3_AA3"    , { 4, 3 }, 3, 2000,  3, 0 },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_START_L3_TRAFFIC },
    { "13_1"           , { 4, 3 }, 1, 2000,  1, 0 },
    { "13_1_SDC2"      , { 4, 3 }, 1, 2000,  2, 0 },
    { "13_1_SDC3"      , { 4, 3 }, 1, 2000,  3, 0 },
    { "13_1_SDC3_AA3"  , { 4, 3 }, 3, 2000,  3, 0 },
    { "15_RR"          , { 4, 1 }, 1,    1,  1, 0 },
    { "15"             , { 4, 3 }, 1,    1,  1, 0 },
    { "15_SDC2"        , { 4, 3 }, 1,    1,  2, 0 },
    { "15_SDC3"        , { 4, 3 }, 1,    1,  3, 0 },
    { "15_SDC3_AA3"    , { 4, 3 }, 3,    1,  3, EXTT2_INSTALL_CONFIG },
    { NULL             , { 0, 0 }, 0,    0,  0, EXTT2_STOP_L3_TRAFFIC },
};

STATIC int
_extt2_do_test_list(int unit, soc_memtune_data_t *mt_data, int test_list_id,
                    int config, int show_matrix, int restore_txrx_after_test,
                    int delta0, int delta1,
                    int bg_l3_num_ge_pkt, int bg_l3_num_xe_pkt)
{
    extt2_test_list_t *list;
    int rv, list_len, alloc_size, i;
    uint32 rval;

    alloc_size = (SOC_TR_SRAM_TX_OFFSET_MAX - SOC_TR_SRAM_TX_OFFSET_MIN + 1) *
        (SOC_TR_SRAM_RX_OFFSET_MAX - SOC_TR_SRAM_RX_OFFSET_MIN + 1) *
        sizeof(int);

    switch (test_list_id) {
    case 0:
        list_len = sizeof(extt2_full_test_list) /
            sizeof(extt2_full_test_list[0]);
        list = extt2_full_test_list;
        break;
    case 1:
        list_len = sizeof(extt2_short_test_list) /
            sizeof(extt2_short_test_list[0]);
        list = extt2_short_test_list;
        break;
    case 3:
        list_len = sizeof(extt2_full_test_list_sram0) /
            sizeof(extt2_full_test_list_sram0[0]);
        list = extt2_full_test_list_sram0;
        break;
    case 4:
        list_len = sizeof(extt2_full_test_list_sram1) /
            sizeof(extt2_full_test_list_sram1[0]);
        list = extt2_full_test_list_sram1;
        break;
    case 5:
        list_len = sizeof(extt2_old_full_test_list) /
            sizeof(extt2_old_full_test_list[0]);
        list = extt2_old_full_test_list;
        break;
    default:
        cli_out("Invalid TestListID %d\n", test_list_id);
        return CMD_FAIL;
    }

    rv = CMD_OK;
    for (i = 0; i < list_len; i++) {
        if (list[i].test_name == NULL) {
            if (list[i].event & EXTT2_ENABLE_TCAM_ODT) {
                if (READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval) < 0) {
                    return CMD_FAIL;
                }
                soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                                  ENABLE_ODTf, 1);
                soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                                  ENABLE_ODT_CKf, 1);
                if (WRITE_ETU_DDR72_CONFIG_REG1_ISr(unit, rval) < 0) {
                    return CMD_FAIL;
                }
                cli_out("=== Enable ODT\n");
            }
            if (list[i].event & EXTT2_DISABLE_TCAM_ODT) {
                if (READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval) < 0) {
                    return CMD_FAIL;
                }
                soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                                  ENABLE_ODTf, 0);
                soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                                  ENABLE_ODT_CKf, 0);
                if (WRITE_ETU_DDR72_CONFIG_REG1_ISr(unit, rval) < 0) {
                    return CMD_FAIL;
                }
                cli_out("=== Disable ODT\n");
            }
            if (list[i].event & EXTT2_START_L3_TRAFFIC) {
                cli_out("=== Starting L3 and FP activities\n");
                if (_extt_l3_activities(unit, TRUE, bg_l3_num_ge_pkt,
                                        bg_l3_num_xe_pkt) < 0) {
                    return CMD_FAIL;
                }
            }
            if (list[i].event & EXTT2_STOP_L3_TRAFFIC) {
                cli_out("=== Stopping L3 and FP activities\n");
                if (_extt_l3_activities(unit, FALSE, 0, 0) < 0) {
                    return CMD_FAIL;
                }
            }
            continue;
        }
        mt_data->config =
            list[i].event & EXTT2_INSTALL_CONFIG ? config : FALSE;
        mt_data->show_matrix =
            list[i].event & EXTT2_INSTALL_CONFIG ? TRUE : show_matrix;
        mt_data->delta[0] = delta0;
        mt_data->delta[1] = delta1;
        mt_data->loop_mode[0] = list[i].loop_mode[0];
        mt_data->loop_mode[1] = list[i].loop_mode[1];
        mt_data->alt_adr = list[i].alt_adr;
        mt_data->tcam_data_choice = list[i].tcam_data_choice;
        mt_data->sram_data_choice = list[i].sram_data_choice;
        mt_data->restore_txrx_after_test =
            list[i].event & EXTT2_INSTALL_CONFIG ?
            restore_txrx_after_test : TRUE;
        if (list[i].event & EXTT2_START_ACCUMULATE) {
            sal_memset(mt_data->fail_array[0], 0, alloc_size);
            sal_memset(mt_data->fail_array[1], 0, alloc_size);
        }

        cli_out("TEST:%s LoopMode0=%d LoopMode1=%d AltAdr=%d "
                "TcamDataChoice=%d SramDataChoice=%d\n",
                list[i].test_name,
                list[i].loop_mode[0],
                list[i].loop_mode[1],
                list[i].alt_adr,
                list[i].tcam_data_choice,
                list[i].sram_data_choice);

        rv = soc_mem_interface_tune_lvl2(unit, mt_data);
        if (rv < 0) {
            cli_out("ERROR: unit %d, extt2: %s\n", unit, soc_errmsg(rv));
        }
    }

    return rv;
}

cmd_result_t
cmd_extt2(int unit, args_t *a)
{
    parse_table_t      pt;
    soc_memtune_data_t mt_data;
    int                rv;
    int                config;
    int                show_matrix;
    int                use_test_list, test_list_id;
    int                loop_mode0, loop_mode1, alt_adr;
    int                tcam_data_choice, sram_data_choice;
    int                delta0, delta1;
    int                tx_offset0, rx_offset0, tx_offset1,rx_offset1;
    int                ok_on_both_pass, restore_txrx_after_test;
    int                bg_l3_num_ge_pkt, bg_l3_num_xe_pkt;
    int                alloc_size;

    sal_memset(&mt_data, 0, sizeof(mt_data));

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "InstallConfig", PQ_BOOL, (void *)TRUE, &config, 0);
    parse_table_add(&pt, "Verbose", PQ_BOOL, 0, &mt_data.verbose, 0);
    parse_table_add(&pt, "SUmmary", PQ_BOOL, (void *)TRUE, &mt_data.summary,
                    0);
    parse_table_add(&pt, "ShowSetting", PQ_BOOL, 0, &mt_data.show_setting, 0);
    parse_table_add(&pt, "ShowMatrix", PQ_BOOL, 0, &show_matrix, 0);
    parse_table_add(&pt, "TestCount", PQ_INT, (void *)20, &mt_data.test_count,
                    0);
    parse_table_add(&pt, "MaxFailCount", PQ_INT, (void *)1,
                    &mt_data.max_fail_count, 0);
    parse_table_add(&pt, "TestAllLatency", PQ_BOOL,
                    soc_feature(unit, soc_feature_esm_rxfifo_resync) ?
                    (void *)TRUE : (void *)FALSE,
                    &mt_data.test_all_latency, 0);
    parse_table_add(&pt, "UseTestList", PQ_BOOL, (void *)TRUE, &use_test_list,
                    0);
    parse_table_add(&pt, "TestListID", PQ_INT, (void *)2, &test_list_id, 0);
    parse_table_add(&pt, "LoopMode0", PQ_INT, (void *)-1, &loop_mode0, 0);
    parse_table_add(&pt, "LoopMode1", PQ_INT, (void *)-1, &loop_mode1, 0);
    parse_table_add(&pt, "AltAdr", PQ_INT, (void *)-1, &alt_adr, 0);
    parse_table_add(&pt, "TcamDataChoice", PQ_INT, (void *)-1,
                    &tcam_data_choice, 0);
    parse_table_add(&pt, "SramDataChoice", PQ_INT, (void *)-1,
                    &sram_data_choice, 0);
    parse_table_add(&pt, "TcamLoopCount", PQ_INT, (void *)1000,
                    &mt_data.tcam_loop_count, 0);
    parse_table_add(&pt, "DeLTa0", PQ_INT, (void *)28, &delta0, 0);
    parse_table_add(&pt, "DeLTa1", PQ_INT, (void *)28, &delta1, 0);
    parse_table_add(&pt, "TxOffset0", PQ_INT, (void *)-1, &tx_offset0, 0);
    parse_table_add(&pt, "RxOffset0", PQ_INT, (void *)-1, &rx_offset0, 0);
    parse_table_add(&pt, "TxOffset1", PQ_INT, (void *)-1, &tx_offset1, 0);
    parse_table_add(&pt, "RxOffset1", PQ_INT, (void *)-1, &rx_offset1, 0);
    parse_table_add(&pt, "OkOnBothPass", PQ_INT, 0, &ok_on_both_pass, 0);
    parse_table_add(&pt, "RestoreTxRxAfterTest", PQ_BOOL, 0,
                    &restore_txrx_after_test, 0);
    parse_table_add(&pt, "BGL3NumGePkt", PQ_INT, (void *)300,
                    &bg_l3_num_ge_pkt, 0);
    parse_table_add(&pt, "BGL3NumXePkt", PQ_INT, (void *)80,
                    &bg_l3_num_xe_pkt, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    /* Don't need strings anymore */
    parse_arg_eq_done(&pt);

    alloc_size = (SOC_TR_SRAM_TX_OFFSET_MAX - SOC_TR_SRAM_TX_OFFSET_MIN + 1) *
        (SOC_TR_SRAM_RX_OFFSET_MAX - SOC_TR_SRAM_RX_OFFSET_MIN + 1) *
        sizeof(int);

    mt_data.fail_array[0] = sal_alloc(alloc_size, "memtune fail array");
    if (mt_data.fail_array[0] == NULL) {
        return SOC_E_MEMORY;
    }
    mt_data.fail_array[1] = sal_alloc(alloc_size, "memtune fail array");
    if (mt_data.fail_array[1] == NULL) {
        sal_free(mt_data.fail_array[0]);
        return SOC_E_MEMORY;
    }

    if (use_test_list) {
        mt_data.tx_offset[0] = -1;
        mt_data.rx_offset[0] = -1;
        mt_data.tx_offset[1] = -1;
        mt_data.rx_offset[1] = -1;
        mt_data.ok_on_both_pass = ok_on_both_pass;

        if (test_list_id == 2 || test_list_id == 6) {
            if (test_list_id == 2) {
                test_list_id = 0;
            } else if (test_list_id == 6) {
                test_list_id = 5;
            }
            mt_data.ok_on_both_pass = FALSE;
            rv = _extt2_do_test_list(unit, &mt_data, test_list_id, config,
                                     show_matrix, restore_txrx_after_test, 28,
                                     0, bg_l3_num_ge_pkt, bg_l3_num_xe_pkt);
            if (rv >= 0) {
                mt_data.ok_on_both_pass = TRUE;
                rv = _extt2_do_test_list(unit, &mt_data, test_list_id, config,
                                         show_matrix, restore_txrx_after_test,
                                         0, 28, bg_l3_num_ge_pkt,
                                         bg_l3_num_xe_pkt);
            }
        } else {
            rv = _extt2_do_test_list(unit, &mt_data, test_list_id, config,
                                     show_matrix, restore_txrx_after_test,
                                     delta0, delta1,
                                     bg_l3_num_ge_pkt, bg_l3_num_xe_pkt);
        }
    } else {
        mt_data.config = config;
        mt_data.show_matrix = show_matrix;
        mt_data.loop_mode[0] = loop_mode0;
        mt_data.loop_mode[1] = loop_mode1;
        mt_data.alt_adr = alt_adr;
        mt_data.tcam_data_choice = tcam_data_choice;
        mt_data.sram_data_choice = sram_data_choice;
        mt_data.delta[0] = delta0;
        mt_data.delta[1] = delta1;
        mt_data.tx_offset[0] = tx_offset0;
        mt_data.rx_offset[0] = rx_offset0;
        mt_data.tx_offset[1] = tx_offset1;
        mt_data.rx_offset[1] = rx_offset1;
        mt_data.ok_on_both_pass = ok_on_both_pass;
        mt_data.restore_txrx_after_test = restore_txrx_after_test;
        sal_memset(mt_data.fail_array[0], 0, alloc_size);
        sal_memset(mt_data.fail_array[1], 0, alloc_size);

        rv = soc_mem_interface_tune_lvl2(unit, &mt_data);
    }

    if (rv < 0) {
        cli_out("ERROR: unit %d, extt2: %s\n", unit, soc_errmsg(rv));
    }

    if (mt_data.fail_array[0] != NULL) {
        sal_free(mt_data.fail_array[0]);
    }
    if (mt_data.fail_array[1] != NULL) {
        sal_free(mt_data.fail_array[1]);
    }

    return CMD_OK;
}
#endif /* BCM_TRIUMPH_SUPPORT */

#endif /* NO_MEMTUNE */
