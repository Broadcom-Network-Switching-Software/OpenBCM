/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag CLI IPFIX command
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/ipfix.h>
#include <bcm_int/esw/ipfix.h>

char cmd_ipfix_usage[] =
    "Parameters: [report +/-<values>] [set <values>] [stop|start]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\t  start  - Start the IPFIX deamon\n"
    "\t  stop   - Stop the IPFIX deamon\n"
    "\t  report - Configure reporting to the console\n"
    "\t      count   - print export entry # only\n"
    "\t      decode  - dump decoded export entry\n"
    "\t      Prepend + to set, - to clear\n"
    "\t  set    - Configure IPFIX for a port\n"
    "\t     Port=<port> [Stage=<stage>] [EnableL2=<1/0>] [EnableIp4=<1/0>]\n"
    "\t     [EnableIp6=<1/0>] [RecordNonDiscard=<1/0>] [RecordDiscard=<1/0>]\n"
    "\t     [CheckFlowEnd=<1/0>] [UseL2ForIp4=<1/0>] [UseL2ForIp6=<1/0>]\n"
    "\t     [KeySrcIp=<1/0>] [KeyIpProt=<1/0>] [KeyIpDscp=<1/0>] [KeyIpEcn=<1/0>]\n"
    "\t     [KeyDstIp=<1/0>] [KeyIpEcn=<1/0>] [KeyL4SrcPort=<1/0>] [KeyL4DstPort=<1/0>]\n"
    "\t     [KeyIp6Flow=<1/0>] [KeyIcmpType=<1/0>] [KeyIcmpCode=<1/0>] [KeyMacDa=<1/0>]\n"
    "\t     [KeyMacSa=<1/0>] [KeyVlanId=<1/0>] [KeyVlanPri=<1/0>] [KeyEtherType=<1/0>]\n"
    "\t     [KeyVlanTagged=<1/0>] [TunnelIp4SrcMask=<adr>]\n"
    "\t     [TunnelIp4DstMask=<adr>] [TunnelIp6SrcMask=<adr>]\n"
    "\t     [Ip4SrcMask=<adr>][Ip4DstMask=<adr>]\n"
    "\t     [Ip6SrcMask=<adr>][Ip6DstMask=<adr>]\n"
    "\t     [DscpMap=zero|identical][Limit=<val>]\n"
    "\t     [MaXTime=<val>][MiNTime=<val>]\n"
    "\t     [MaxIdletime=<val>][SampleRate=<val>]\n"
#endif
;

static int ipfix_report_count[BCM_MAX_NUM_UNITS] = { 0 };
static int ipfix_is_running[BCM_MAX_NUM_UNITS] = { 0 };
static uint32 ipfix_report[BCM_MAX_NUM_UNITS] = { 0 };

static const char *dscp_names[] = {
    "identical",
    "zero",
    NULL
};

#define REPORT_COUNT    0x01   /* Report export count */
#define REPORT_DECODE   0x02   /* Report Decoded export data */

static const parse_pm_t ipfix_report_table[] = {
    {"Count",   REPORT_COUNT},
    {"Decode",  REPORT_DECODE},
    {"@ALL",    ~0},
    {"@*",      ~0},
    {NULL,      0}
};

void
_ipfix_callback(int unit, bcm_ipfix_data_t *info, void *userdata)
{
    ipfix_report_count[unit]++;

    if (ipfix_report[unit] & REPORT_COUNT) {
        cli_out("Export entry #%d.\n", ipfix_report_count[unit]);
    }
    if (ipfix_report[unit] & REPORT_DECODE) {
        bcm_ipfix_dump_export_entry(unit, info);
    }
}

cmd_result_t
cmd_ipfix(int unit, args_t *arg)
{
    char *subcmd;
    parse_table_t pt;
    bcm_ipfix_config_t config;
    int rv, i;
    int port = 0;
    bcm_ip_t src_ip4_mask = 0, dst_ip4_mask = 0;
    bcm_ip_t tunnel_src_ip4_mask = 0, tunnel_dst_ip4_mask = 0;
    bcm_ip6_t src_ip6_mask, dst_ip6_mask;
    bcm_ip6_t tunnel_src_ip6_mask, tunnel_dst_ip6_mask;
    int enable_l2 = 0, enable_ip4 = 0, enable_ip6 = 0;
    int record_non_discard = 1, record_discard = 0, check_flow_end = 0;
    int use_l2_for_ip4 = 0, use_l2_for_ip6 = 0;
    int key_src_ip = 0, key_dst_ip = 0; 
    int  key_ip_prot = 0, key_ip_dscp = 0, key_ip_ecn = 0;
    int key_l4_src_port = 0, key_l4_dst_port = 0, key_ip6_flow = 0;
    int key_icmp_type = 0, key_icmp_code = 0, key_macda = 0, key_macsa = 0;
    int key_vlan_id = 0, key_vlan_pri = 0, key_ether_type = 0; 
    int key_vlan_tagged = 0, key_source_port = 0;
    int stage = 0, dscp_idx = 0, entry_limit = 8191;
    int min_time = 0, max_time = 0;
    int max_idle_time = 32767, sample_rate = 1;
    char str[IP6ADDR_STR_LEN];

    if (!ARG_CNT(arg)) {                  /* Nothing passed */
        cli_out("IPFIX Deamon Status: %s.\n",
                (ipfix_is_running[unit]) ? "Running" : "Not Running");

        cli_out("Reporting is enabled for: ");
        parse_mask_format(80, ipfix_report_table, ipfix_report[unit]);
        cli_out("Reporting is disabled for: ");
        parse_mask_format(80, ipfix_report_table, ipfix_report[unit] ^ ~0);

        cli_out("Number of records received: %d\n", ipfix_report_count[unit]);
        return(CMD_OK);
    }

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    rv = BCM_E_NONE;
    sal_memset(src_ip6_mask, 0, sizeof(bcm_ip6_t));
    sal_memset(dst_ip6_mask, 0, sizeof(bcm_ip6_t));
    sal_memset(tunnel_src_ip6_mask, 0, sizeof(bcm_ip6_t));
    sal_memset(tunnel_dst_ip6_mask, 0, sizeof(bcm_ip6_t));

    if (!sal_strcasecmp(subcmd, "start")) {
        if (ipfix_is_running[unit]) {
            cli_out("%s: IPFIX thread already running.\n", ARG_CMD(arg));
            return CMD_OK;
        }
        ipfix_report_count[unit] = 0;
        ipfix_is_running[unit] = 1;
        rv = bcm_ipfix_export_fifo_control(unit, 1000000);
        if (rv < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcm_ipfix_register(unit, _ipfix_callback, NULL);
    } else if (!sal_strcasecmp(subcmd, "stop")) {
        if (!ipfix_is_running[unit]) {
            cli_out("%s: ERROR: IPFIX thread already stopped.\n", ARG_CMD(arg));
            return CMD_FAIL;
        }
        ipfix_is_running[unit] = 0;
        rv = bcm_ipfix_export_fifo_control(unit, 0);
        if (rv < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcm_ipfix_unregister(unit, _ipfix_callback, NULL);
    } else if (!sal_strcasecmp(subcmd, "set")) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_INT, (void *)-1, &port, 0);
        parse_table_add(&pt, "Stage", PQ_DFL | PQ_INT, (void *)0, &stage, 0);
        parse_table_add(&pt, "EnableL2", PQ_DFL | PQ_INT, (void *)0,
                        &enable_l2, 0);
        parse_table_add(&pt, "EnableIp4", PQ_DFL | PQ_INT, (void *)0,
                        &enable_ip4, 0);
        parse_table_add(&pt, "EnableIp6", PQ_DFL | PQ_INT, (void *)0,
                        &enable_ip6, 0);
        parse_table_add(&pt, "RecordNonDiscard", PQ_DFL | PQ_INT, (void *)0,
                        &record_non_discard, 0);
        parse_table_add(&pt, "RecordDiscard", PQ_DFL | PQ_INT, (void *)0,
                        &record_discard, 0);
        parse_table_add(&pt, "CheckFlowEnd", PQ_DFL | PQ_INT, (void *)0,
                        &check_flow_end, 0);
        parse_table_add(&pt, "UseL2ForIp4", PQ_DFL | PQ_INT, (void *)0,
                        &use_l2_for_ip4, 0);
        parse_table_add(&pt, "UseL2ForIp6", PQ_DFL | PQ_INT, (void *)0,
                        &use_l2_for_ip6, 0);
        parse_table_add(&pt, "KeySrcIp", PQ_DFL | PQ_INT, (void *)0,
                        &key_src_ip, 0);
        parse_table_add(&pt, "KeyIpProt", PQ_DFL | PQ_INT, (void *)0,
                        &key_ip_prot, 0);
        parse_table_add(&pt, "KeyIpDscp", PQ_DFL | PQ_INT, (void *)0,
                        &key_ip_dscp, 0);
        parse_table_add(&pt, "KeyIpEcn", PQ_DFL | PQ_INT, (void *)0,
                        &key_ip_ecn, 0);
        parse_table_add(&pt, "KeyDstIp", PQ_DFL | PQ_INT, (void *)0,
                        &key_dst_ip, 0);
        parse_table_add(&pt, "KeyIpEcn", PQ_DFL | PQ_INT, (void *)0,
                        &key_ip_ecn, 0);
        parse_table_add(&pt, "KeyL4SrcPort", PQ_DFL | PQ_INT, (void *)0,
                        &key_l4_src_port, 0);
        parse_table_add(&pt, "KeyL4DstPort", PQ_DFL | PQ_INT, (void *)0,
                        &key_l4_dst_port, 0);
        parse_table_add(&pt, "KeyIp6Flow", PQ_DFL | PQ_INT, (void *)0,
                        &key_ip6_flow, 0);
        parse_table_add(&pt, "KeyIcmpType", PQ_DFL | PQ_INT, (void *)0,
                        &key_icmp_type, 0);
        parse_table_add(&pt, "KeyIcmpCode", PQ_DFL | PQ_INT, (void *)0,
                        &key_icmp_code, 0);
        parse_table_add(&pt, "KeyMacDa", PQ_DFL | PQ_INT, (void *)0,
                        &key_macda, 0);
        parse_table_add(&pt, "KeyMacSa", PQ_DFL | PQ_INT, (void *)0,
                        &key_macsa, 0);
        parse_table_add(&pt, "KeyVlanId", PQ_DFL | PQ_INT, (void *)0,
                        &key_vlan_id, 0);
        parse_table_add(&pt, "KeyVlanPri", PQ_DFL | PQ_INT, (void *)0,
                        &key_vlan_pri, 0);
        parse_table_add(&pt, "KeyEtherType", PQ_DFL | PQ_INT, (void *)0,
                        &key_ether_type, 0);
        parse_table_add(&pt, "KeyVlanTagged", PQ_DFL | PQ_INT, (void *)0,
                        &key_vlan_tagged, 0);
        parse_table_add(&pt, "KeySourcePort", PQ_DFL | PQ_INT, (void *)0,
                        &key_source_port, 0);
        parse_table_add(&pt, "Ip4SrcMask", PQ_DFL | PQ_IP, 0, &src_ip4_mask,
                        0);
        parse_table_add(&pt, "Ip4DstMask", PQ_DFL | PQ_IP, 0, &dst_ip4_mask,
                        0);
        parse_table_add(&pt, "TunnelIp4SrcMask", PQ_DFL | PQ_IP, 0,
                        &tunnel_src_ip4_mask, 0);
        parse_table_add(&pt, "TunnelIp4DstMask", PQ_DFL | PQ_IP, 0,
                        &tunnel_dst_ip4_mask, 0);
        parse_table_add(&pt, "Ip6SrcMask", PQ_DFL | PQ_IP6, 0, &src_ip6_mask,
                        0);
        parse_table_add(&pt, "Ip6DstMask", PQ_DFL | PQ_IP6, 0, &dst_ip6_mask,
                        0);
        parse_table_add(&pt, "TunnelIp6SrcMask", PQ_DFL | PQ_IP6, 0,
                        &tunnel_src_ip6_mask, 0);
        parse_table_add(&pt, "TunnelIp6DstMask", PQ_DFL | PQ_IP6, 0,
                        &tunnel_dst_ip6_mask, 0);
        parse_table_add(&pt, "DscpMap", PQ_DFL | PQ_MULTI, 0, &dscp_idx,
                        dscp_names);
        parse_table_add(&pt, "Limit", PQ_DFL | PQ_INT, (void *)0,
                        &entry_limit, 0);
        parse_table_add(&pt, "MinTime", PQ_DFL | PQ_INT, (void *)0,
                        &min_time, 0);
        parse_table_add(&pt, "MaxTime", PQ_DFL | PQ_INT, (void *)0,
                        &max_time, 0);
        parse_table_add(&pt, "MaxIdleTime", PQ_DFL | PQ_INT, (void *)0,
                        &max_idle_time, 0);
        parse_table_add(&pt, "SampleRate", PQ_DFL | PQ_INT, (void *)0,
                        &sample_rate, 0);

        if (parse_arg_eq(arg, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        sal_memset(&config, 0, sizeof(config));
        config.flags |= enable_l2 ? BCM_IPFIX_CONFIG_ENABLE_NON_IP : 0;
        config.flags |= enable_ip4 ? BCM_IPFIX_CONFIG_ENABLE_IP4 : 0;
        config.flags |= enable_ip6 ? BCM_IPFIX_CONFIG_ENABLE_IP6 : 0;
        config.flags |= check_flow_end ? BCM_IPFIX_CONFIG_TCP_END_DETECT : 0;
        config.flags |= record_non_discard ?
            BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT : 0;
        config.flags |= record_discard ?
            BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT : 0;
        config.flags |= use_l2_for_ip4 ? BCM_IPFIX_CONFIG_KEY_IP4_USE_L2 : 0;
        config.flags |= use_l2_for_ip6 ? BCM_IPFIX_CONFIG_KEY_IP6_USE_L2 : 0;
        config.flags |= key_src_ip ? BCM_IPFIX_CONFIG_KEY_SRC_IP : 0;
        config.flags |= key_dst_ip ? BCM_IPFIX_CONFIG_KEY_DST_IP : 0;
        config.flags |= key_ip_prot ? BCM_IPFIX_CONFIG_KEY_IP_PROT : 0;
        config.flags |= key_ip_dscp ? BCM_IPFIX_CONFIG_KEY_IP_DSCP : 0;
        config.flags |= key_ip_ecn ? BCM_IPFIX_CONFIG_KEY_IP_ECN : 0;
        config.flags |= key_l4_src_port ? BCM_IPFIX_CONFIG_KEY_L4_SRC_PORT : 0;
        config.flags |= key_l4_dst_port ? BCM_IPFIX_CONFIG_KEY_L4_DST_PORT : 0;
        config.flags |= key_ip6_flow ? BCM_IPFIX_CONFIG_KEY_IP6_FLOW : 0;
        config.flags |= key_icmp_type ? BCM_IPFIX_CONFIG_KEY_ICMP_TYPE : 0;
        config.flags |= key_icmp_code ? BCM_IPFIX_CONFIG_KEY_ICMP_CODE : 0;
        config.flags |= key_macda ? BCM_IPFIX_CONFIG_KEY_MACDA : 0;
        config.flags |= key_macsa ? BCM_IPFIX_CONFIG_KEY_MACSA : 0;
        config.flags |= key_vlan_id ? BCM_IPFIX_CONFIG_KEY_VLAN_ID : 0;
        config.flags |= key_vlan_pri ? BCM_IPFIX_CONFIG_KEY_VLAN_PRI : 0;
        config.flags |= key_ether_type ? BCM_IPFIX_CONFIG_KEY_ETHER_TYPE : 0;
        config.flags |= key_vlan_tagged ? BCM_IPFIX_CONFIG_KEY_VLAN_TAGGED : 0;
        config.flags |= key_source_port ?
            BCM_IPFIX_CONFIG_KEY_SOURCE_PORT_OR_INTERFACE : 0;

        if (dscp_idx == 0) {
            for (i = 0; i < 64; i++) {
                config.dscp_mask[i] = i;
            }
        } else {
            for (i = 0; i < 64; i++) {
                config.dscp_mask[i] = 0;
            }
        }
        config.src_ip4_mask = src_ip4_mask;
        config.dst_ip4_mask = dst_ip4_mask;
        config.tunnel_src_ip4_mask = tunnel_src_ip4_mask;
        config.tunnel_dst_ip4_mask = tunnel_dst_ip4_mask;
        SAL_IP6_ADDR_TO_UINT32(src_ip6_mask, (uint32 *)config.src_ip6_mask);
        SAL_IP6_ADDR_TO_UINT32(dst_ip6_mask, (uint32 *)config.dst_ip6_mask);
        SAL_IP6_ADDR_TO_UINT32(tunnel_src_ip6_mask,
                               (uint32 *)config.tunnel_src_ip6_mask);
        SAL_IP6_ADDR_TO_UINT32(tunnel_dst_ip6_mask,
                               (uint32 *)config.tunnel_dst_ip6_mask);
        config.entry_limit = entry_limit;
        config.min_time = min_time;
        config.max_time = max_time;
        config.max_idle_time = max_idle_time;
        config.sample_rate = sample_rate;
        rv = bcm_ipfix_config_set(unit, stage, port, &config);
    } else if (!sal_strcasecmp(subcmd, "get")) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_INT, (void *)-1, &port, 0);
        parse_table_add(&pt, "Stage", PQ_DFL | PQ_INT, (void *)0, &stage, 0);
        if (parse_arg_eq(arg, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        rv = bcm_ipfix_config_get(unit, stage, port, &config);
        if (rv >= 0) {
            if (config.flags & BCM_IPFIX_CONFIG_ENABLE_NON_IP) {
                cli_out("BCM_IPFIX_CONFIG_ENABLE_NON_IP\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_ENABLE_IP4) {
                cli_out("BCM_IPFIX_CONFIG_ENABLE_IP4\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_ENABLE_IP6) {
                cli_out("BCM_IPFIX_CONFIG_ENABLE_IP6\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_TCP_END_DETECT) {
                cli_out("BCM_IPFIX_CONFIG_TCP_END_DETECT\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT) {
                cli_out("BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT) {
                cli_out("BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_IP4_USE_L2) {
                cli_out("BCM_IPFIX_CONFIG_KEY_IP4_USE_L2\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_IP6_USE_L2) {
                cli_out("BCM_IPFIX_CONFIG_KEY_IP6_USE_L2\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_SRC_IP) {
                cli_out("BCM_IPFIX_CONFIG_KEY_SRC_IP\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_DST_IP) {
                cli_out("BCM_IPFIX_CONFIG_KEY_DST_IP\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_IP_PROT) {
                cli_out("BCM_IPFIX_CONFIG_KEY_IP_PROT\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_IP_DSCP) {
                cli_out("BCM_IPFIX_CONFIG_KEY_IP_DSCP\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_IP_ECN) {
                cli_out("BCM_IPFIX_CONFIG_KEY_IP_ECN\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_L4_SRC_PORT) {
                cli_out("BCM_IPFIX_CONFIG_KEY_L4_SRC_PORT\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_L4_DST_PORT) {
                cli_out("BCM_IPFIX_CONFIG_KEY_L4_DST_PORT\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_IP6_FLOW) {
                cli_out("BCM_IPFIX_CONFIG_KEY_IP6_FLOW\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_ICMP_TYPE) {
                cli_out("BCM_IPFIX_CONFIG_KEY_ICMP_TYPE\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_ICMP_CODE) {
                cli_out("BCM_IPFIX_CONFIG_KEY_ICMP_CODE\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_MACDA) {
                cli_out("BCM_IPFIX_CONFIG_KEY_MACDA\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_MACSA) {
                cli_out("BCM_IPFIX_CONFIG_KEY_MACSA\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_VLAN_ID) {
                cli_out("BCM_IPFIX_CONFIG_KEY_VLAN_ID\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_VLAN_PRI) {
                cli_out("BCM_IPFIX_CONFIG_KEY_VLAN_PRI\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_ETHER_TYPE) {
                cli_out("BCM_IPFIX_CONFIG_KEY_ETHER_TYPE\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_KEY_VLAN_TAGGED) {
                cli_out("BCM_IPFIX_CONFIG_KEY_VLAN_TAGGED\n");
            }
            if (config.flags & BCM_IPFIX_CONFIG_TCP_FLAGS_LAST) {
                cli_out("BCM_IPFIX_CONFIG_TCP_FLAGS_LAST\n");
            }
            cli_out("entry_limit=%d min_time=%d max_time=%d\n",
                    config.entry_limit, config.min_time, config.max_time);
            cli_out("max_idle_time=%d sample_rate=%d\n",
                    config.max_idle_time, config.sample_rate);
            format_ipaddr(str, config.src_ip4_mask);
            cli_out("src_ip4_mask %s\n", str);
            format_ipaddr(str, config.dst_ip4_mask);
            cli_out("dst_ip4_mask %s\n", str);
            format_ip6addr(str, config.src_ip6_mask);
            cli_out("src_ip6_mask %s\n", str);
            format_ip6addr(str, config.dst_ip6_mask);
            cli_out("dst_ip6_mask %s\n", str);
        }
    } else if (!sal_strcasecmp(subcmd, "report")) {
        if (ARG_CNT(arg)) {
            while ((subcmd = ARG_CUR(arg)) != NULL &&
                   !parse_mask(subcmd, ipfix_report_table, &ipfix_report[unit])) {
                ARG_NEXT(arg);        /* Bump arg if matched report */
            }
        } else {                    /* Print values */
            cli_out("IPFIX Reporting on for: ");
            parse_mask_format(50, ipfix_report_table, ipfix_report[unit]);
            cli_out("IPFIX Reporting off for: ");
            parse_mask_format(50, ipfix_report_table, ~ipfix_report[unit]);
        }
    } else {
        return CMD_USAGE;
    }

    if (rv < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}
