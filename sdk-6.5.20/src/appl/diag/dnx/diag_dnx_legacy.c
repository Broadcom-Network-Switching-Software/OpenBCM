/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_legacy.c
 *
 * Purpose: List of commands specific for SOC_IS_DNX devices
 */

#include <appl/diag/sand/diag_sand_framework.h>

/* *INDENT-OFF* */
sh_sand_legacy_cmd_t sh_dnx_legacy_commands[] = {
    {
        .command = "DIAG field last",
        .substitute = "field last info"
    },
    {
        .command = "DIAG COSq local_to_sys",
        .message = "Command shell name changed to 'tm ingress local_to_sys'",
        .substitute = "tm ingress local_to_sys"
    },
    {
        .command = "DIAG cosq credit_watchdog",
        .message = "Credit watchdog attributes can be read by command shell 'tm queue info'"
    },
    {
        .command = "DIAG cosq print_mapping",
        .message = "Command shell name changed to 'tm ingress queue mapping'"
    },
    {
        .command = "DIAG cosq autocredit",
        .message = "Command shell name changed to 'tm ingress queue autocredit'"
    },
    {
        .command = "DIAG reassembly",
        .message = "Command shell name changed to 'tm ingress reassembly'"
    },
    {
        .command = "DIAG ing_congestion",
        .message = "Command shell name changed to 'tm ingress congestion'"
    },
    {
        .command = "DIAG egr_calendars",
        .message = "Command shell name changed TO 'tm egress spr'"
    },
    {
        .command = "DIAG egr_congestion",
        .message = "Command shell name changed to 'tm egress congestion all'"
    },
    {
        .command = "COSQ flush",
        .message = "Command shell name changed 'TBD'"
    },
    {
        .command = "DIAG COSq egq_port_shaper",
        .message = "Command shell name changed to 'TBD'"
    },
    {
        .command = "DIAG COSq qpair egq",
        .message = "Command shell name changed to 'tm egress shaper'",
        .substitute = "tm egress shaper"
    },
    {
        .command = "DIAG COSq print_flow_and_up",
        .message = "Command shell name changed to 'tm scheduler print_flow_and_up'",
        .substitute = "tm scheduler print_flow_and_up"
    },
    {
        .command = "DIAG COSq p",
        .message = "Command shell name changed to 'tm scheduler print_flow_and_up'",
        .substitute = "tm scheduler print_flow_and_up"
    },
    {
        .command = "DIAG COSq qpair e2e",
        .message = "Command shell name changed to 'tm scheduler port_info'",
        .substitute = "tm scheduler portinfo"
    },
    {
        .command = "GPORT",
        .message = "Command shell name changed to 'tm sch gport'"
    },
    {
        .command = "COSQ comp egress",
        .message = "Command shell name changed to 'TBD'"
    },
    {
        .command = "COSQ CONNection INGress",
        .message = "Command shell name changed to 'tm scheduler connection ingress'",
        .substitute = "tm scheduler connection ingress"
    },
    {
        .command = "COSQ CONNection EGRess",
        .message = "Command shell name changed to 'tm scheduler connection egress'",
        .substitute = "tm scheduler connection egress"
    },
    {
        .command = "CTRP",
        .message = "Command is not supported. Use 'crps' to see supported counter processor commands"
    },
    {
        .command = "DIAG pp eplcr",
        .message = "Command is not supported. Use 'meter' to see supported meter commands"
    },
    {
        .command = "DIAG pp ageplcr",
        .message = "Command is not supported. Use 'meter' to see supported meter commands"
    },
    {
        .command = "DIAG pp",
        .message = "Command shell name has changed to 'pp vis <options>'",
    },
    {
        .command = "DIAG pp pi",
        .message = "Command shell name has changed to 'pp vis ppi'",
        .substitute = "pp vis ppi"
    },
    {
        .command = "DIAG pp rpi",
        .message = "Command shell name has changed to 'pp vis ppi'",
        .substitute = "pp vis ppi"
    },
    {
        .command = "DIAG pp pkttm",
        .message = "Command shell name has changed to 'pp vis pkttm'",
        .substitute = "pp vis pkttm"
    },
    {
        .command = "DIAG pp fdt",
        .message = "Command shell name has changed to 'pp vis fdt'",
        .substitute = "pp vis fdt"
    },
    {
        .command = "DIAG pp fec last_fec",
        .message = "Command shell name has changed to 'pp vis fec'",
        .substitute = "pp vis fec"
    },
    {
        .command = "DIAG pp fec all",
        .message = "Command shell name has changed to 'pp info fec'",
        .substitute = "pp info fec"
    },
    {
        .command = "DIAG pp termi",
        .message = "Command shell name has changed to 'pp vis term'",
        .substitute = "pp vis term"
    },
    {
        .command = "DIAG pp ive",
        .message = "Command shell name has changed to 'pp vis ive'",
        .substitute = "pp vis ive"
    },
    {
        .command = "DIAG pp eve",
        .message = "Command shell name has changed to 'pp vis eve'",
        .substitute = "pp vis eve"
    },
    {
        .command = "DIAG pp last",
        .message = "Show last packet traverse information from IRPP blocks",
        .substitute = "pp vis last"
    },
    {
        .command = "DIAG dbal lp",
        .message = "Command shell name has changed to 'pp vis ikleap'",
        .substitute = "pp vis ikleap"
    },
    {
        .command = "DIAG pp kbp last",
        .message = "Command shell name has changed to 'pp vis ikleap phydb=kbp'",
        .substitute = "pp vis ikleap phydb=kbp"
    },
    {
        .command = "DIAG pp kbp print",
        .message = "Command information is now present in following: 'kbp opcode dump'",
        .substitute = "kbp opcode dump"
    },
    {
        .command = "DIAG last",
        .message = "Please use 'pp vis ppi'",
    },
    {
        .command = "DIAG pp dblif",
        .message = "Command shell name has changed to 'pp vis ikleap stage=vtt<1..5>'"
    },
    {
        .command = "DIAG pp trapsi",
        .message = "trap last info"
    },
    {
        .command = "DIAG pp encap",
        .message = "Command shell name has changed to 'pp vis ekleap'",
        .substitute = "pp vis ekleap"
    },
    {
        .command = "DIAG pp fli",
        .message = "Command shell name has changed to 'pp vis ikleap stage=fwd<1,2>'"
    },
    {
        .command = "DIAG pp flpm",
        .message = "Command shell name has changed to 'pp vis ikleap phydb=kaps<1,2>'"
    },
    {
        .command = "kbp kaps_show",
        .message = "TBD"
    },
    {
        .command = "kbp kaps_db_stats",
        .message = "TBD"
    },
    {
        .command = "kbp mdio_read_16",
        .message = "Command shell name has changed to 'PHY RAW C45 phy_id=<val> devad=<val> regad=<val>'"
    },
    {
        .command = "kbp mdio_read_64",
        .message = "Command shell name has changed to 'PHY RAW C45 phy_id=<val> devad=<val> regad=<val>'"
    },
    {
        .command = "kbp mdio_write_16",
        .message = "Command shell name has changed to 'PHY RAW C45 phy_id=<val> devad=<val> regad=<val> data=<val>'"
    },
    {
        .command = "kbp mdio_write_64",
        .message = "Command shell name has changed to 'PHY RAW C45 phy_id=<val> devad=<val> regad=<val> data=<val>'"
    },
    {
        .command = "DIAG pp dblem",
        .message = "Command shell name has changed to 'pp vis ikleap phydb=lem'"
    },
    {
        .command = "DIAG pp modeg",
        .message = "Command is not supported"
    },
    {
        .command = "DIAG pp modes",
        .message = "Command is not supported"
    },
    {
        .command = "DIAG pp pkttrc",
        .message = "Command is not supported"
    },
    {
        .command = "DIAG prge_info",
        .message = "Command is not supported"
    },
    {
        .command = "DIAG prge_last",
        .message = "Command is not supported"
    },
    {
        .command = "DIAG oam ep",
        .message = "Command information is now present in following: 'oam oamp_pe' and 'oam bfdEndPoint'"
    },
    {
        .command = "DIAG oam lu",
        .message = "Command shell name has changed to 'oam last_lookUp"
    },
    {
        .command = "DIAG oam count",
        .message = "Command shell name has changed to 'oam coUnter'"
    },
    {
        .command = "DIAG template all",
        .message = "Command information is now partially present in following: swstate <module> <template> dump."
    },
    {
        .command = "DIAG template type",
        .message = "Command information is now partially present in following: swstate <module> <template> dump."
    },
    {
        .command = "DIAG pp vtt",
        .message = "Command shell name has changed to 'ikleap info context'"
    },
    {
        .command = "DIAG pp ipv4_mc",
        .message = "TBD"
    },
    {
        .command = "DIAG pp flp",
        .message = "Command shell name has changed to 'ikleap info context'"
    },
    {
        .command = "DIAG pp sig",
        .message = "Command shell name has changed to 'sig get'"
    },
    {
        .command = "DIAG alloc",
        .message = "Command information is now partially present in following: swstate <module> <resource> dump."
    },
    {
        .command = "PortMod",
        .message = "TDB"
    },
    {
        .command = "DIAG pp gp",
        .message = "Command shell name has changed to 'gpm hwres'"
    },
    {
        .command = "DIAG pp cc",
        .message = "Command is not supported"
    },
    {
        .command = "DIAG pp occ",
        .message = "Command shell name has changed, check the commands under 'pp vis occ'",
    },
    {
        .command = "DIAG pp cos",
        .message = "Command shell name has changed to 'pp vis cos'",
        .substitute = "pp vis cos"
    },
    {
        .command = "DIAG pp lif",
        .message = "Command shell name has changed to 'gpm lif'",
        .substitute = "gpm lif"
    },
    {
        .command = "DIAG pp load_balancing",
        .message = "TBD"
    },
    {
        .command = "DIAG pp FTMH_Header_Change_Show",
        .message = "Command shell name has changed to 'pp vis sysh'"
    },
    {
        .command = "DIAG headerdiff",
        .message = "TDB"
    },
    {
        .command = "DIAG mmu",
        .message = "TDB"
    },
    {
        .command = "DDRPhyCDR",
        .message = "TDB"
    },
    {
        .command = "TDM edit show",
        .message = "TDB"
    },
    {
        .command = "NEGEV",
        .message = "TDB"
    },
    {
        .command = "DIAG multicast",
        .message = "TDB"
    },
    {
        .command = "INIT",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "DEInit",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "DETach",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "PROBE",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "REINIT",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },

    {
        .command = "SwitchControl",
        .message = "Command not supported. Use API bcm_switch_control_set()/get() instead"
    },
    {
        .command = "STKMode",
        .message = "Command not supported. Use API bcm_switch_control_set()/get() instead"
    },
    {
        .command = "PortStat",
        .message = "Command is not supported. Use 'port status <pbmp>' instead"
    },
    {
        .command = "DIAG nif n",
        .message = "Command is not supported. Use 'nif status <No_Rates>' instead"
    },
    {
        .command = "DIAG pp mtr",
        .message = "Command is not supported. Use 'meter' to see supported meter commands"
    },
    {
        .command = "DIAG pp eplcr",
        .message = "Command is not supported. Use 'meter' to see supported meter commands"
    },
    {
        .command = "fabric property",
        .message = "Command is not supported. Use 'data dump property *' instead"
    },
    {
        .command = "DIAG COSq non",
        .message = "Command is not supported. Use 'tm ingress queue non_empty' instead",
        .substitute = "tm ingress queue non_empty"
    },
    {
        .command = "DIAG COSq voq",
        .message = "Command is not supported. Use 'tm ingress queue info' instead",
        .substitute = "tm ingress queue info"
    },
    {
        .command = "DIAG rates",
        .message = "Command is not supported. Use 'tm sch rate *', 'tm ing rate *' and 'tm egr rate *' instead",
    },
    {
        .command = "DIAG pp cc",
        .message = "Command shell name has changed to 'pp info cct'",
        .substitute = "pp info cct"
    },
    {NULL} /* This line should always stay as last one */
};
/* *INDENT-ON* */
