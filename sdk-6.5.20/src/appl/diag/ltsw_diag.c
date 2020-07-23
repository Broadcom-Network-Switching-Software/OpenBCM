/*! \file ltsw_diag.c
 *
 * LTSW diag command list.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "appl/diag/diag.h"

#if defined(BCM_LTSW_SUPPORT)

char cmd_ltsw_clear_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  show <args>\n"
#else
    "clear counters [PBMP] \n\t"
    "counters - zeroes all or some internal packet counters\n"
#endif
    ;

extern cmd_result_t cmd_ltsw_clear(int unit, args_t *a);

char cmd_ltsw_cos_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "cos <option> [args...]\n"
#else
    "cos clear                  - Reset COS configuration to default\n\t"
    "cos map [Pri=#] [Queue=#]  - Map a priority 0-7 to a COS queue\n\t"
    "cos strict                 - Set strict queue scheduling mode\n\t"
    "cos simple                 - Set round robin queue scheduling mode\n\t"
    "cos weight [W0=#] [W1=#] ... - Set weighted queue scheduling mode\n\t"
    "                             with the specified weights per queue\n\t"
    "cos fair [W0=#] [W1=#] ... - Set weighted fair queue scheduling mode\n\t"
    "                             with the specified weights per queue\n\t"
    "cos bounded [W0=#] [W1=#] ... [Delay=<usec>]\n\t"
    "                           - Set bounded queue scheduling with delay\n\t"
    "cos drr [W0=#] [W1=#] ...  - Set Deficit Round Robin scheduling mode\n\t"
    "                             with the specified weights per queue\n\t"
    "cos discard [Enable=true|false] [CapAvg=true|false]\n\t"
    "                           - Set discard (WRED) config\n\t"
    "cos discard_show [Gport=<GPORT ID>] [Queue=#]\n\t"
    "                           - Show current discard (WRED) config\n\t"
    "cos discard_gport [Enable=true|false] [CapAvg=true|false]\n\t"
    "                  [Gport=<GPORT ID>] [Queue=#] [Color=green|yellow|red]\n\t"
    "                  [DropSTart=#] [DropEnd=#] [DropProbability=#] [GAin=#]\n\t"
    "                  [Mode=byte|packet]\n\t"
    "                           - Set GPORT discard (WRED) config\n\t"
    "cos bandwidth [PortBitMap=<pbmp>] [Queue=#] [KbpsMIn=#]\n\t"
    "              [KbpsMAx=#] [Flags=#]\n\t"
    "                           - Set cos bandwidth\n\t"
    "cos bandwidth_show         - Show current COS bandwidth config\n\t"
    "cos show                   - Show current COS config\n"
    "\n\t"
    "cos port config [PortBitMap=<pbmp>] [Numcos=#] [Gport=<GPORT ID>]\n\t"
    "                           - Add and attach a COSQ GPORT\n\t"
    "cos port map [PortBitMap=<pbmp>] [Pri=#] [Queue=#]\n\t"
    "                           - Map a port's priority 0-7 to a COS queue\n\t"
    "cos port strict            - Set strict queue scheduling mode\n\t"
    "cos port simple            - Set round robin queue scheduling mode\n\t"
    "cos port weight [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set weighted queue scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port fair [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set weighted fair queue scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port bounded [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set bounded queue scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port drr [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set Deficit Round Robin scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port bandwidth [PortBitMap=<pbmp>] [Queue=#] [KbpsMIn=#]\n\t"
    "                   [KbpsMAx=#] [Flags=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set port cos bandwidth\n\t"
    "cos port show [PortBitMap=<pbmp>]\n\t"
    "                           - Show current port COS config\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_cos(int unit, args_t *a);

char cmd_ltsw_counter_usage[] =
    "\nParameters:  [on] [off] [sync] [Interval=<usec>]\n\t"
    "\t[PortBitMap=<pbm>] \n\t"
    "Starts the counter collection task running every <usec>\n\t"
    "microseconds.  The task tallies software counters based on hardware\n\t"
    "values and must run often enough to avoid counter overflow.\n\t"
    "If <interval> is 0, stops the task.  If <interval> is omitted, prints\n\t"
    "the current INTERVAL.  sync reads in all the counters to synchronize\n\t"
    "'show counters' for the first time, and must be used alone.\n";

extern cmd_result_t cmd_ltsw_counter(int unit, args_t *a);

char cmd_ltsw_custom_stat_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "CustomSTAT <option> [args...]\n"
#else
    "  cstat <pbmp> [z]\n\t"
    "        - Display all non-zero custom counters and their triggers\n\t"
    "            z  - include counters that are zero\n\t"
    "  cstat raw <pbmp> [rx|tx] <counter>\n\t"
    "        - Display value of counter in raw mode\n\t"
    "  cstat set <pbmp> [rx|tx] <counter> <triggers>\n\t"
    "        - Set triggers for given counter\n\t"
    "  cstat get <pbmp> [rx|tx] <counter>\n\t"
    "        - Get currently enabled triggers of counter\n\t"
    "  cstat ls [rx|tx]\n\t"
    "        - List available triggers\n\t"
    "  cstat info [rx|tx]\n\t"
    "        - List available triggers with short description\n\t"
    "        Note: To specify all counters use 'all'\n\t"
    "Displays and configures programmable statistic counters\n\t"
    "RDBGC0,TDBGC0 etc.\n"
#endif
;

extern cmd_result_t cmd_ltsw_custom_stat(int unit, args_t *a);

char cmd_ltsw_event_stat_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "EventSTAT <option> [args...]\n"
#else
    "  estat show [Z] {Drop|Trace} [rx [<event ID>]|tx [<event ID>]]\n\t"
    "        - Display drop|trace event counters\n\t"
    "        Z  - include counters that are zero\n\t"
    "        Note: If event ID is not specified, will display all or rx/tx counters\n\t"
    "  estat clear {Drop|Trace} [rx [<event ID>]|tx [<event ID>]]\n\t"
    "        - Clear drop|trace event counters\n\t"
    "        Note: If event ID is not specified, will clear all or rx/tx counters\n"
#endif
;
extern cmd_result_t cmd_ltsw_event_stat(int unit, args_t *a);

char cmd_ltsw_l2_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  l2 <options> <args>\n"
#else
    "  l2 add [Module=<mod>] [Port=<port>] [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "         [Trunk=true|false] [TrunkGroupId=<id>]\n\t"
    "         [STatic=true|false] [SourceCosPriority=true|false]\n\t"
    "         [DiscardSource=true|false] [DiscardDest]=true|false]\n\t"
    "         [HIT=true|false] [CPUmirror=true|false] [Group=<group>]\n\t"
    "        - Add L2 address associated with port\n\t"
    "  l2 del [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "        - Delete L2 address with given mac and vlanid\n\t"
    "  l2 get [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "        - Look up L2 address with given mac and vlanid\n\t"
    "  l2 show \n\t"
    "        - Show all L2 addresses\n\t"
    "  l2 count \n\t"
    "        - Show current L2 address count\n\t"
    "  l2 clear [Module=<n>] [Port=<port>] [MACaddress=<mac>]\n\t"
    "           [Vlanid=<id>] [TrunkGroupID=<id>] [Static=true|false]\n\t"
    "        - Remove all L2 entries on the given module, module/port,\n\t"
    "           VLAN, or trunk group ID\n\t"
    "  l2 replace [Module=<n>] [Port=<port>] [MACaddress=<mac>]\n\t"
    "             [Vlanid=<id>] [Trunk=true|false] [TrunkGroupId=<id>]\n\t"
    "             [STatic=true|false] [NewModule=<n>] [NewPort=<port>]\n\t"
    "             [NewTrunkGroupId=<id>]\n\t"
    "        - Replace all L2 entries matching given module/port or mac\n\t"
    "          vlanid, trunk with new destination\n\t"
    "  l2 age [Timer=<n>|on|off]\n\t"
    "        - Setup L2 age timer in seconds\n\t"
    "          If not specify timer value, then display setting\n\t"
    "        - On|Off\n\t"
    "          Off is to stop L2 age without breaking current interval\n\t"
    "          On is to resume L2 age with current interval\n\t"
    "  l2 station add [Priority=<val>] [ID=<Sid>]\n\t"
    "         [MACaddress=<mac>] [MACaddressMask=<mask>]\n\t"
    "         [Vlanid=<id>] [VlanidMask=<mask>]\n\t"
    "         [Ivid=<id>] [IvidMask=<mask>]\n\t"
    "         [Vfi=<id>] [VfiMask=<mask>]\n\t"
    "         [SourcePort=<id>] [SourcePortMask=<mask>] \n\t"
    "         [IPv4=true|false] [IPv6=true|false] [ArpRarp=true|false]\n\t"
    "         [MPLS=true|false] [MiM=true|false] [TRILL=true|false]\n\t"
    "         [FCoE=true|false] [OAM=true|false] [CPUmirror=true|false]\n\t"
    "         [Replace=true|false] [Discard=true|false]\n\t"
    "        - Add L2 station entry\n\t"
    "  l2 station delete ID=<sid>\n\t"
    "        - Delete L2 station entry\n\t"
    "  l2 station clear\n\t"
    "        - Delete all L2 station entries\n\t"
    "  l2 station show [ID=<sid>] \n\t"
    "        - Show L2 station entry, if not specify sid, then display all\n\t"
    "  l2 init\n\t"
    "        - Init L2 module\n\t"
    "  l2 detach\n\t"
    "        - Detach L2 module\n\t"
    "  l2 learn [on|off]\n\t"
    "        - Enable/Disable global L2 learning\n\t"
    "          On is to enable, off is to disable\n\t"
    "          If no specify on or off, display current L2 learning setting\n"
#endif
    ;

extern cmd_result_t cmd_ltsw_l2(int unit, args_t *arg);

char cmd_ltsw_ser_usage[] =
        "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
        "SER <options> <args>\t"
#else
        " SER LOG [ID=<id>]\n"
        "\tid argument is used to show specified SER log\n"
        " SER INJECT PT=<PT name> [Index=<Index>] [Inst=<Inst>] [Xor=true|false]\n"
        "\tInst can be pipe number for EP/IP memory or register table.\n"
        "\tInst can be inst number for MMU memory or register table.\n"
        "\tInst can be port number for Port memory or register table.\n"
        "\tValidation true: SER error is validated during command.\n"
        "\tXor_bank argument is used to insert error into XOR ram of XOR hash tables.\n"
        "\tWith no optional parameters, Index will default to 0, and Inst to"
        " 0\n"
        " SER CONFIG [SquashDuplicatedInterval=<Interval>]\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_ser(int unit, args_t *arg);

char cmd_ltsw_sram_scan_usage[] =
        "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
        "SramSCAN [option] [args]\t"
#else
        " SramSCAN [Interval=<USEC>] [Rate=<ENTRIES>] [on] [off]\n"
        "\tInterval specifies how often to run (0 to stop).\n"
        "\tMax value: 100000000us, Min value 1000000us.\n"
        "\tRate specifies the number of entries to process per interval.\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_sram_scan(int unit, args_t *arg);

char cmd_ltsw_mem_scan_usage[] =
        "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
        "MemSCAN [option] [args]\t"
#else
        " MemSCAN [on] [off]\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_mem_scan(int unit, args_t *arg);

char cmd_ltsw_l3_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "l3 <option> [args...]\n"
#else
    "l3 init    - Init L3 function.\n\t"
    "l3 detach  - Detach L3 function.\n\n\t"

    "l3 ing_intf add\n\t"
    "   [INTF=<val>] VRF=<val> [UrfpMode=<val>] [INtfClass=<val>]\n\t"
    "   [QOSmapID=<val>] [DSCPTrust=<val>] [UrpfDefaultRouteCheck=<val>]\n\t"
    "   [PIM_BIDIR=<val>\n\t"
    "l3 ing_intf delete INTF=<val>\n\t"
    "l3 ing_intf get INTF=<val>\n\t"
    "l3 ing_intf clear\n\t"
    "l3 ing_intf show\n\n\t"

    "l3 intf add\n\t"
    "   [INTF=<val>] Vlan=<val> MAC=<val> [UnderLay=<val>]\n\t"
    "l3 intf delete INTF=<val>\n\t"
    "l3 intf get INTF=<val>\n\t"
    "l3 intf clear\n\t"
    "l3 intf show\n\n\t"

    "l3 egress add\n\t"
    "   [EgrID=<val>] MAC=<val> Port=<val>  INTF=<val> [MOdule=<val>]\n\t"
    "   [UnderLay=<val>] [Trunk=<val>] [L2tocpu=<val>]\n\t"
    "l3 egress delete EgrID=<val>\n\t"
    "l3 egress get EgrID=<val>\n\t"
    "l3 egress clear\n\t"
    "l3 egress show\n\n\t"

    "l3 ecmp add\n\t"
    "   [GRouPID=<val>] [MaxPaths=<val>] [DynamicMode=<val>] [UnderLay=<val>]\n\t"
    "   OL_INTF0=<val> OL_INTF1=<val> ... OL_INTF31=<val>\n\t"
    "   UL_INTF0=<val> UL_INTF1=<val> ... UL_INTF31=<val>\n\t"
    "l3 ecmp delete GRouPID=<val>\n\t"
    "l3 ecmp get GRouPID=<val>\n\t"
    "l3 ecmp member insert GRouPID=<val> OL_INTF=<val> UL_INTF=<val>\n\t"
    "l3 ecmp member remove GRouPID=<val> OL_INTF=<val> UL_INTF=<val>\n\t"
    "l3 ecmp clear\n\t"
    "l3 ecmp show\n\n\t"

    "l3 host add\n\t"
    "   {ip=<val> | ip6=<val>} [vrf=<val>] intf=<val> [ul_intf=<val>] \n\t"
    "   [ecmp=<bool>] \n\t"
    "l3 host delete\n\t"
    "   {ip=<val> | ip6=<val>} [vrf=<val>]\n\t"
    "l3 host get\n\t"
    "   {ip=<val> | ip6=<val>} [vrf=<val>]\n\t"
    "l3 host clear [v6=<bool>]\n\t"
    "l3 host show [v6=<bool>] [start=<val>] [end=<val>]\n\n\t"

    "l3 route add\n\t"
    "   {ip=<val> | ip6=<val>} masklen=<val> [vrf=<val>] [ecmp=<bool>]\n\t"
    "   intf=<val> [ul_intf=<val>] [DstDiscard=<bool>] [<Replace>=<bool>]\n\t"
    "l3 route delete\n\t"
    "   {ip=<val> | ip6=<val>} masklen=<val> [vrf=<val>]\n\t"
    "l3 route get\n\t"
    "   {ip=<val> | ip6=<val>} masklen=<val> [vrf=<val>]\n\t"
    "l3 route clear [v6=<bool>]\n\t"
    "l3 route show [v6=<bool>] [start=<val>] [end=<val>]\n\t"
    "l3 route perf v4v6=<val> OPs=<val> [VRF_OPs=<val>] [loops=<val>]\n\t"
    "              [nsets=<val>] [UseApi=<bool>]\n\t"
    "       - Test L3 route operation performance.\n\t"
    "       - v4v6: 1 - IPv4; 2 - IPv6; 3 - IPv4 + IPv6; 4 - IPv6 + IPv4.\n\t"
    "       - OPs: B0 - Add, B1 - Update, B2 - Lookup, B3 - Delete.\n\t"
    "       - VRFs: B0 - VRF, B1 - GLOBAL, B2 - OVERRIDE.\n\t"
    "               Default: 3 for parallel, 1 for combined.\n\t"
    "       - loops: Number of runs. Forced to 1 if no delete operation.\n\t"
    "       - nsets: Number of sets.\n\t"
    "       - UseApi: 1 - Use BCM API.\n\n\t"

    "l3 tunnel_init create [<TnlId>=<val>] <TYpe>=<val> <TTL>=<val>\n\t"
    "   <DIP>=<val> <SIP>=<val> <DSCPEcnSel>=<val> <DSCPV>=<val>\n\t"
    "   <DFSEL4>=<val> <DFSEL6>=<val> <SIP6>=<val> <DIP6>=<val>\n\t"
    "   <DSCPEcnMap>=<val>\n\t"
    "l3 tunnel_init get <TnlId>=<val>\n\t"
    "l3 tunnel_init destroy <TnlId>=<val>\n\t"

    "l3 tunnel_init set <INtf>=<val> <TYpe>=<val> <TTL>=<val>\n\t"
    "   <Mac>=<val> <DIP>=<val> <SIP>=<val> <DSCPSel>=<val> <DSCPV>=<val>\n\t"
    "   <DFSEL4>=<val> <DFSEL6>=<val> <SIP6>=<val> <DIP6>=<val>\n\t"
    "l3 tunnel_init show <INtf>=<val>\n\t"
    "l3 tunnel_init clear <INtf>=<val>\n\t"

    "l3 tunnel_term add <TYPE>=<val> <DIP>=<val> <SIP>=<val>\n\t"
    "   <DipMaskLen>=<val> <SipMaskLen>=<val> <DIP6>=<val> <SIP6>=<val>\n\t"
    "   <OuterTTL>=<bool> \n\t"
    "   <OuterDSCP>=<val> \n\t"
    "   <VLanid>=<val> \n\t"
    "   <Replace>=<bool> <EM>=<bool>\n\t"
    "l3 tunnel_term delete <TYPE>=<val> <DIP>=<val> <SIP>=<val> <EM>=<bool>\n\t"
    "   <DipMaskLen>=<val> <SipMaskLen>=<val> <DIP6>=<val> <SIP6>=<val>\n\t"
    "l3 tunnel_term show <TYPE>=<val> <DIP>=<val> <SIP>=<val> <EM>=<bool>\n\t"
    "   <DipMaskLen>=<val> <SipMaskLen>=<val> <DIP6>=<val> <SIP6>=<val>\n\n\t"

    "l3 nat_egress add <NAPT>=<val> <SAT>=<val> <DNAT>=<val>\n\t"
    "   <SNAT_ID>=<val> <DNAT_ID>=<val>\n\t"
    "   <SIP>=<val> <DIP>=<val>\n\t"
    "   <SPORT>=<val> <DPORT>=<val>\n\t"
    "l3 nat_egress get <NAPT>=<val> <SAT>=<val> <DNAT>=<val>\n\t"
    "   <SNAT_ID>=<val> <DNAT_ID>=<val> \n\t"
    "l3 nat_egress destroy <NAPT>=<val> <SAT>=<val> <DNAT>=<val>\n\t"
    "   <SNAT_ID>=<val> <DNAT_ID>=<val>\n\t"
    "l3 nat_egress show <NAPT>=<val> <SAT>=<val> <DNAT>=<val>\n\n\t"

    "l3 nat_ingress add <NAPT>=<val> <DST_DISCARD>=<val>\n\t"
    "   <IP>=<val> <VRF>=<val> <PROTO>=<val> <L4_PORT>=<val>\n\t"
    "   <NAT_ID>=<val> <NEXTHOP>=<val>\n\t"
    "   <SREALM_ID>=<val> <CLASS_ID>=<val>\n\t"
    "l3 nat_ingress get <NAPT>=<val> <DST_DISCARD>=<val>\n\t"
    "   <IP>=<val> <VRF>=<val> <PROTO>=<val> <L4_PORT>=<val>\n\t"
    "l3 nat_ingress delete <NAPT>=<val> <DST_DISCARD>=<val> \n\t"
    "   <IP>=<val> <VRF>=<val> <PROTO>=<val> <L4_PORT>=<val>\n\t"
    "l3 nat_ingress show <NAPT>=<val> <DST_DISCARD>=<val>\n"
#endif
    ;

char cmd_ltsw_mirror_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  mirror <options> <args>\n"
#else
    "\tshow                 - display current mirror configuration\n"
    "Switch Device Options:\n"
    "\tDest                 - Set mirror destination or attach it to port\n"
    "\tInit                 - Init Mirror module\n"
    "\tPort                 - Set mirror configuration for a port\n"
    "Advanced Options: (subcmd dest) to use mirror destination features\n"
    "\tcreate          (cmd) - creates a new mirror destination\n"
    "\tdestroy         (cmd) - destroys mirror destination\n"
    "\tadd             (cmd) - add created mirror destination to a port\n"
    "\tdelete          (cmd) - delete mirror destination from a port\n"
    "\tshow            (cmd) - display currently defined mirror destinations\n"
    "\tid=<id>         (arg) - mirror destination id\n"
    "\tdestport=<p>    (arg) - mirror destination gport\n"
    "\tsrcport=<p>     (arg) - source mirror port/gport\n"
    "\tmode=<OFF|Ingress|Egress|EgressTrue|IngressEgress> (arg) - mirroring mode\n"
    "\ttunnel=<L2|GRE> (arg) - mirroring tunnel\n"
    "\tsrcIP=<IPv4>    (arg) - source IPv4 address for IP GRE tunneling\n"
    "\tdestIP=<IPv4>   (arg) - destination IPv4 address for IP GRE tunneling\n"
    "\tsrcIP6=<IPv6>   (arg) - source IPv6 address for IP GRE tunneling\n"
    "\tdestIP6=<IPv6>  (arg) - destination IPv6 address for IP GRE tunneling\n"
    "\tsrcMAC=<MAC>    (arg) - source MAC address for L2/IP GRE tunneling\n"
    "\tdestMAC=<MAC>   (arg) - destination MAC address for L2/IP GRE tunneling\n"
    "\tvlan=<VLAN-ID>  (arg) - VLAN ID for L2/IP GRE tunneling\n"
    "\ttpid=<TPID>     (arg) - TPID for L2/IP GRE tunneling\n"
    "\tversion=<IP-Version> (arg) - Version field for IP GRE tunneling\n"
    "\tttl=<TTL>       (arg) - TTL field for IP GRE tunneling\n"
    "\ttos=<TOD>       (arg) - TOS field for IP GRE tunneling\n"
    "\tFlowLable=<FlowLable> (arg) - IPv6 header flow label field for IP GRE tunneling\n"
    "\tNoVlan=<T|F>    (arg) - Strip VLAN tag from mirrored packet\n"
    "\n"
    "Advanced Options: (subcmd port) to set mirror configuration for a port\n"
    "Parameters: <ports> [SHow] [Mode=<Off|On|Ingress|Egress|All>]\n"
    "\t[DestPort=<port>] [DestModule=<modid>]\n"
#endif
    ;

extern cmd_result_t cmd_ltsw_mirror(int unit, args_t *arg);


char if_ltsw_linkscan_usage[] =
    "Parameters: [SwPortBitMap=<pbmp>] [HwPortBitMap=<pbmp>]\n\t"
    "[OverridePortBitMap=<pbmp>] [OverrideLinkUp=<pbmp>] [Interval=<usec>]\n\t"
    "[FORCE=<pbmp>] [on] [off]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tWith no arguments, displays the linkscan status for all ports.\n\t"
    "Enables software linkscan on ports specified by SwPortBitMap.\n\t"
    "Enables hardware linkscan on ports specified by HwPortBitMap.\n\t"
    "Enables override linkscan on ports specified by OverridePortBitMap.\n\t"
    "Disables linkscan on all other ports.\n\t"
    "Set override link state to up on ports specified by OverrideLinkUp.\n\t"
    "Interval specifies non-default linkscan interval for software.\n\t"
    "Note: With linkscan disabled, autonegotiated ports will NOT have\n\t"
    "the MACs updated with speed/duplex..\n\t"
    "FORCE=<pbmp> requests linkscan to update ports once even if link\n\t"
    "status did not change.\n"
#endif
    ;

extern cmd_result_t if_ltsw_linkscan(int unit, args_t *a);


char cmd_ltsw_port_usage[] =
    "Parameters: <pbmp> [[ENCap=IEEE|HIGIG|B5632|HIGIG2]\n\t"
    "[AutoNeg=on|off] [ADVert=<portmode>]\n\t"
    "[LinkScan=none|sw|hw|ovr] [SPeed=10|100|1000] [FullDuplex=true|false]\n\t"
    "[TxPAUse=on|off] [RxPAUse=on|off] [STationADdr=<macaddr>]\n\t"
    "[LeaRN=<learnmode>] [DIScard=none|untag|all] [VlanFilter=<value>]\n\t"
    "[PRIOrity=<0-7>] [PortFilterMode=<value>]\n\t"
    "[PHymaster=<Master|Slave|Auto|None>] [Enable=<true|false>]\n\t"
    "[FrameMax=<value>]\n\t"
    "[MDIX=Auto|ForcedAuto|ForcedNormal|ForcedXover]\n\t"
    "[LoopBack=NONE|MAC|PHY]\n\t"
    "[MEdium=None|Copper|Fiber]]\n\t"
    "| [[EEE] [ENable=<enable|disable>|TxIDleTime=<0-2560>|TxWakeTime=<0-2560>|STats=<get>]]\n\t"
    "[ASF(in case of multimode asf)=<0-3>]\n\t"
    "[ASF(in case regular asf)=<0-1>]\n\t"
    "If only <ports> is specified, characteristics for that port are\n\t"
    "displayed. <ports> is a standard port bitmap (see \"help pbmp\").\n\t"
    "If AutoNeg is on, SPeed and DUPlex are the ADVERTISED MAX values.\n\t"
    "If AutoNeg is off, SPeed and DUPlex are the FORCED values.\n\t"
    "SPeed of zero indicates maximum speed.\n\t"
    "LinkScan enables automatic scanning for link changes with updating\n\t"
    "of MAC registers, and EPC_LINK (or equivalent)\n\t"
    "PAUse enables send/receive of pause frames in full duplex mode.\n\t"
    "<learnmode> is a numeric value controlling source lookup failure\n\t"
    "packets; it may include bit 0 to enable hardware L2 learn, bit 1\n\t"
    "to copy SLF packets to CPU, bit 2 to forward SLF packets.\n\t"
    "VlanFilter drops input packets that not tagged with a valid VLAN\n\t"
    "that contains the port. For XGS3, VlanFilter takes a value 0/1/2/3 where\n\t"
    "bit 0 turns on/off ingress filter and bit 1 turns on/off egress filter.\n\t"
    "PRIOrity sets the priority for untagged packets coming on this port.\n\t"
    "PortFilterMode takes a value 0/1/2 for mode A/B/C (see register manual).\n\t"
    "EEE: Energy Efficient Ethernet.\n\t"
    "ASF: Alternative Store and Forward (cut-thru mode).\n\t"
    "ASF Modes(in case of multimode asf): [0]SAF [1]Same Speed [2]Slow to Fast [3]Fast to Slow\n\t"
    "ASF Modes(in case regular asf): [0]Disable [1]Enable\r\n"
    ;

extern cmd_result_t cmd_ltsw_port(int unit, args_t *arg);

char cmd_ltsw_port_stat_usage[] =
    "Display info about port status in table format.\n"
    "    Link scan modes:\n"
    "        SW = software\n"
    "        HW = hardware\n"
    "    Learn operations (source lookup failure control):\n"
    "        F = SLF packets are forwarded\n"
    "        C = SLF packets are sent to the CPU\n"
    "        A = SLF packets are learned in L2 table\n"
    "        D = SLF packets are discarded.\n"
    "    Pause:\n"
    "        TX = Switch will transmit pause packets\n"
    "        RX = Switch will obey pause packets\n"
    ;

extern cmd_result_t cmd_ltsw_port_stat(int unit, args_t *arg);

char cmd_ltsw_pbmp_usage[] =
    "Parameters: <pbmp>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tConverts a pbmp string into a hardware port bitmap.  A pbmp string\n\t"
    "is a single port, or a group of ports specified in a list using ','\n\t"
    "to separate them and '-' for ranges, e.g. 1-8,25,cpu.  Ports may be\n\t"
    "specified using the one-based port number (1-29) or port type and\n\t"
    "zero-based number (fe0-fe23,ge0-ge7).  'cpu' is the CPU port,\n\t"
    "'fe' is all FE ports, 'ge' is all GE ports, 'e' is all ethernet\n\t"
    "ports, 'all' is all ports, and 'none' is no ports (0x0).\n\t"
    "A '~' may be used to exclude port previously given (e.g. e,~fe19)\n\t"
    "Acceptable strings and values also depend on the chip being used.\n\t"
    "A pbmp may also be given as a raw hex (0x) number, e.g. 0xbffffff.\n"
#endif
    ;

extern cmd_result_t cmd_ltsw_pbmp(int unit, args_t *arg);

char cmd_ltsw_flexport_usage[] =
    "Usage: FLEXport PM<id>[=<pm_mode>[:<port_numbers>]]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "Parameters:\n"
    "  PM<id>           - Port Macro to be flexed.\n"
    "  <pm_mode>        - Port Macro mode:\n"
    "                     Single Mode:\n"
    "                       1x400G | 2x200G | 2x100G | 2x40G\n"
    "                     Dual Mode:\n"
    "                       4x100G | 4x50G | 4x40G\n"
    "                     Quad Mode\n"
    "                       8x50G | 8x25G | 8x10G\n"
    "                     Clear ports on a Port Macro:\n"
    "                       None\n"
    "  [<port_numbers>] - New port numbers on a specific Port Macro.\n"
    "                     If no port_number configured, new port numbers will\n"
    "                     be picked from un-used ports.\n"
    "Note: All the other modes not listed above are not\n"
    "      supported on BCM Shell, even they may be\n"
    "      supported via API configuration.\n"
    "                                      \n"
    "Example: flexport pm0=2x200G pm1=4x100G\n"
    "Example: flexport pm0=none pm2=8x50g\n"
    "Example: flexport pm0=4x100G:5-8\n"
    "Example: flexport pm0=2x40G:1,5\n"
    "Flex Port Macro(s) into a specific mode.\n"
    "Remove all ports in a Port Macro if the <pm_mode> is configured as None.\n"
    "If only PM<id> is specified, characteristics for that Port Macro is displayed.\n";
#endif
    ;

extern cmd_result_t cmd_ltsw_flexport(int unit, args_t *arg);

char if_ltsw_asf_usage[] =
    "ASF Configurations Summary & Diagnostics.\n"
    "Parameters: [show | diag] <pbmp>\n"
    "Example: asf show [ce0 | hg1 | ce,hg]\n"
    "Example: asf diag [ce0 | hg1 | ce,hg]\n"
    "<pbmp> is a standard port bitmap (see \"help pbmp\").\n"
    "To configure ASF on port(s), use 'port <pbmp> asf <mode>' command\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: asf <option> [args...]\n"
#endif
    ;

extern cmd_result_t if_ltsw_asf(int unit, args_t *a);

char cmd_ltsw_pvlan_usage[] =
    "Usages:\n\t"
    "  pvlan show <pbmp>\n\t"
    "        - Show PVLAN info for these ports.\n\t"
    "  pvlan set <pbmp> <vid>\n\t"
    "        - Set default VLAN tag for port(s)\n\t"
    "          Port bitmaps are read from the VTABLE entry for the VID.\n\t"
    "          <vid> must have been created and all ports in <pbmp> must\n\t"
    "          belong to that VLAN.\n";
extern cmd_result_t cmd_ltsw_pvlan(int unit, args_t *a);

char cmd_ltsw_show_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  show <args>\n"
#else
    "  show Counters [Changed] [Same] [Z] [NZ] [ErDisc] [ALL] [<pbmp>]\n"
    "\tDisplay all counters, or only specified regs and/or ports\n"
    "\t  Changed - include counters that changed\n"
    "\t  Same    - include counters that did not change\n"
    "\t  Z       - include counters that are zero\n"
    "\t  Nz      - include counters that are not zero\n"
    "\t  All     - same as: Changed Same Z Nz\n"
    "\t  ErDisc  - Only show those counters marked with Error/Discard\n"
    "\tNOTES: If neither Changed or Same is specified, Change is used.\n"
    "\t       Use All to display counters regardless of value.\n"
    "  show features [all] - Show enabled (or all) features for this unit\n"
    "  show CONFig - Show configuration in yaml format.\n"
    "  show Threads - Show threads information.\n"
#endif
    ;

extern cmd_result_t cmd_ltsw_show(int unit, args_t *a);

char cmd_ltsw_stg_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  stg <option> [args...]\n"
#else
    "  stg create [<id>]            - Create a STG; optionally specify ID\n\t"
    "  stg destroy <id>             - Destroy a STG\n\t"
    "  stg show [<id>]              - List STG(s)\n\t"
    "  stg add <id> <vlan_id> [...] - Add VLAN(s) to a STG\n\t"
    "  stg remove <id> <vlan_id> [...]\n\t"
    "                               - Remove VLAN(s) from a STG\n\t"
    "  stg stp                      - Get span tree state, all ports/STGs\n\t"
    "  stg stp <id>                 - Get span tree state of ports in STG\n\t"
    "  stg stp <id> <pbmp> <state>  - Set span tree state of ports in STG\n\t"
    "                                 (disable/block/listen/learn/forward)\n\t"
    "  stg default [<id>]           - Show or set the default STG\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_stg(int unit, args_t *a);

extern cmd_result_t cmd_ltsw_init(int unit, args_t *arg);

char cmd_ltsw_init_usage[] =
    "Parameters: [soc|bcm]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tPerforms various initializations (default is soc):\n\t"
    "  soc - resets the device\n\t"
    "  bcm - initializes BCM driver\n"
    "Additional selective initialization (components of 'init bcm'):\n\t"
    "port, l2, vlan, trunk, cosq, multicast, linkscan, stat, filter, diffserv\n\t"
    "mirror, packet, l3, stack, ipmc, stg, mbcm, stp, tx, auth.\n"
#endif
    ;

char cmd_ltsw_vlan_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  vlan <option> [args...]\n"
#else
    "  vlan create <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>]\n\t"
    "                                       - Create a VLAN\n\t"
    "  vlan destroy <id>                    - Destroy a VLAN\n\t"
    "  vlan clear                           - Destroy all VLANs\n\t"
    "  vlan add <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>\n\t"
    "                                       - Add port(s) to a VLAN\n\t"
    "  vlan remove <id> [PortBitMap=<pbmp>] - Remove ports from a VLAN\n\t"
    "  vlan show                            - Display all VLANs\n\t"
    "  vlan <id> <name>=<vlaue>             - Set per VLAN property\n\t"
    "  vlan gport add <id> [PortID=<gport_id>\n\t"
    "                                       - Add gport to a VLAN\n\t"
    "  vlan gport delete <id> [PortID=<gport_id>\n\t"
    "                                       - Delete gport from a VLAN\n\t"
    "  vlan gport get <id> [PortID=<gport_id>\n\t"
    "                                       - Check if gport belongs to a VLAN\n\t"
    "  vlan gport clear <id>\n\t"
    "                                       - Delete all ports from a VLAN\n\t"
    "  vlan gport show <id>\n\t"
    "                                       - Show all ports in a VLAN\n\t"
    "  vlan control [<name> [<value>]]\n\t"
    "                                       - Set/get VLAN-specific chip options\n\t"
    "  vlan stat <id> Attach=<true|false> StatId=<sid>\n\t"
    "                                       - Attach/detach a flex counter to a VLAN\n\t"
    "  vlan default [<id>]                  - Show or set the default VLAN\n\t"
    "  vlan translate [On|Off]              - Enable/disable Ing&Egr VLAN translation\n\t"
    "  vlan port [<port>|all [<name> [<value>]]]\n\t"
    "                                       - Set/get port-specific VLAN options\n\t"
    "  vlan action port default add Port=<port> OuterVlan=<vlanid>\n\t"
    "        InnerVlan=<vlanid> Prio=<prio> DtOuter=<action>\n\t"
    "        DtOuterPrio=<action> DtInner=<action> DtInnerPrio=<action>\n\t"
    "        OtOuter=<action> OtOuterPrio=<action> OtInner=<action>\n\t"
    "        OtOuter=<action> ItOuter=<action> ItInner=<action>\n\t"
    "        ItInnerPrio=<action> UtOuter=<action> UtInner=<action>\n\t"
    "        ForwardingDomain=<fwd_domain> OuterTag=<action> InnerTag=<action>\n\t"
    "        - Add port default VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action port default get Port=<port>\n\t"
    "        - Get port default VLAN tag actions\n\t"
    "  vlan action translate add Port=<port> KeyType=<key> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid> DtOuter=<action>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        ForwardingDomain=<fwd_domain> OuterTag=<action> InnerTag=<action>\n\t"
    "        - Add VLAN tag translation with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "          Valid key types are { Double, Outer, Inner, OuterTag, \n\t"
    "          InnerTag, PortDouble, PortOuter, Policer, \n\t"
    "          PortGroupDouble, PortGroupOuter, PortGroupInner}\n\t"
    "  vlan action translate delete Port=<port> KeyType=<key> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Delete VLAN tag translation actions.\n\t"
    "  vlan action translate get Port=<port> KeyType=<key> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Get VLAN tag translation actions.\n\t"
    "  vlan action translate show\n\t"
    "        - Show all VLAN tag translation actions.\n\t"
    "  vlan action translate range add Port=<port> \n\t"
    "        OuterVlanLo=<vlanid> OuterVlanHi=<vlanid> \n\t"
    "        InnerVlanLo=<vlanid> InnerVlanHi=<vlanid>\n\t"
    "        - Add VLAN tag range translation.\n\t"
    "  vlan action translate range delete Port=<port> \n\t"
    "        OuterVlanLo=<vlanid> OuterVlanHi=<vlanid> \n\t"
    "        InnerVlanLo=<vlanid> InnerVlanHi=<vlanid>\n\t"
    "        - Delete VLAN tag translation range.\n\t"
    "  vlan action translate range clear\n\t"
    "        - Delete all VLAN tag translation range.\n\t"
    "  vlan action translate range show\n\t"
    "        - Show all VLAN tag translation range.\n\t"
    "  vlan action translate egress add PortClass=<class> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid> DtOuter=<action>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        UtInner=<action>\n\t"
    "        ForwardingDomain=<fwd_domain> OuterTag=<action> InnerTag=<action>\n\t"
    "        - Add VLAN tag egress translation with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action translate egress delete PortClass=<class> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Delete VLAN tag egress translation actions.\n\t"
    "  vlan action translate egress get PortClass=<class> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Get VLAN tag egress translation actions.\n\t"
    "  vlan action translate egress show\n"
#endif
    ;

extern cmd_result_t cmd_ltsw_vlan(int unit, args_t *arg);

/*! Help for CLI Trunk command. */
char if_ltsw_trunk_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  trunk <option> [args...]\n"
#else
    "  trunk init\n\t"
    "        - Initialize trunking function\n\t"
    "  trunk deinit\n\t"
    "        - Deinitialize trunking function\n\t"
    "  trunk add <Id=val> <Rtag=val> <Pbmp=val>\n\t"
    "        - Add ports to a trunk\n\t"
    "  trunk remove <Id=val> <Pbmp=val>\n\t"
    "        - Remove ports from a trunk\n\t"
    "  trunk show [<Id=val>]\n\t"
    "        - Display trunk information\n\t"
    "  trunk psc <Id=val> <Rtag=val>\n\t"
    "        - Change Rtag (for testing ONLY)\n"
#endif
    ;

/*!
 * \brief CLI 'trunk' command implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] a Argument list
 *
 * \return CMD_xxx return values.
 */
extern cmd_result_t if_ltsw_trunk(int unit, args_t *a);

char cmd_ltsw_rate_usage[] =
    "Parameters: [PortBitMap=<pbm>] [Limit=<limit>] [Bcast=true|false]\n\t"
    "  [Mcast=true|false] [Dlf=true|false]\n\t"
#ifndef COMPILER_STRING_CONST_LIMIT
    "Enables the specified packet rate controls.\n\t"
    "  pbm       port(s) to set up or display\n\t"
    "  limit     packets per second\n\t"
    "  bcast     Enable broadcast rate control\n\t"
    "  mcast     Enable multicast rate control\n\t"
    "  dlf       Enable DLF flooding rate control\n\t"
    "If no flags are given or only <pbm> is given, displays the current\n\t"
    "rate settings.\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
    ;

extern cmd_result_t cmd_ltsw_rate(int unit, args_t *a);

char cmd_ltsw_ratebw_usage[] =
    "Set/Display port bandwidth rate limiting.\n"
    "Parameters: <pbm> flags rate max_burst\n\t"
    "    flag is one of the bcast | mcast | dlf\n\t"
    "    rate and max_burst are values in kilobits (1000 bits) per second\n";

extern cmd_result_t cmd_ltsw_ratebw(int unit, args_t *a);


/*! Help for CLI SC command. */
char cmd_ltsw_switch_control_usage[] =
    "\n\tSwitchControl [PortBitMap=<pbmp>] [<name> | <name>=<value>]\n";

/*!
 * \brief CLI 'SwitchControl' command implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] a Argument list
 *
 * \return CMD_xxx return values.
 */
extern cmd_result_t cmd_ltsw_switch_control(int unit, args_t *a);

/*! Help for CLI flowdb command. */
char cmd_ltsw_flow_db_usage [] =
    "Parameters: [flow | flow_option | help] [show]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\t flow show        - Displays available flow handles.\n"\
    "\t flow_option show - Displays available flow options.\n"\
    "\t help             - Displays this usage page.\n"\
    "\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
    ;

char cmd_ltsw_dump_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "DUMP [options]\n"
#else
    "DUMP [File=<name>] [Append=true|false]\n\t"
    "DUMP SW [all|<MODULE>]   (Dump the software bookkeeping)\n\t"
#endif
    ;
extern cmd_result_t cmd_ltsw_dump(int unit, args_t *a);

char cmd_ltsw_txn_usage[] =
    "Parameters: <Count> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  Transmit the specified number of packets, if the contents of the\n"
    "  packets is important, they may be specified using options.\n"
    "  Supported options are:\n"
    "      Untagged[yes/no]    - Specify packet should be sent untagged(XGS3)\n"
    "      PortBitMap=<pbmp>   - Specify port bitmap packet is sent to.\n"
    "      File=<filename>     - Load hex packet data from file and ignore\n"
    "                            various other pattern parameters below.\n"
    "      DATA=<packet>       - Parse string given by user as packet payload\n"
    "      Length=<value>      - Specify the total length of the packet,\n"
    "                            including header, possible tag, and CRC.\n"
    "      VLantag=<value>     - Specify the VLAN tag used, only the low\n"
    "                            order 16-bits are used (VLANID=0 for none)\n"
    "      VlanPrio            - VLAN Priority.\n"
    "      PrioInt             - Internal Priority.\n"
    "      COSqueue            - Tx pmd cos.\n"
    "      Pattern=<value>     - Specify 32-bit data pattern used.\n"
    "      PatternInc=<value>  - Value by which each word of the data\n"
    "                            pattern is incremented\n"
    "      PatternRandom[yes/no] - Use Random data pattern\n"
    "      PerPortSrcMac=[0|1] - Associate specific (different) src macs\n"
    "                            with each source port.\n"
    "      SourceMac=<value>   - Source MAC address in packet\n"
    "      SourceMacInc=<val>  - Source MAC increment\n"
    "      DestMac=<value>     - Destination MAC address in packet.\n"
    "      DestMacInc=<value>  - Destination MAC increment.\n"
    "      McQType=[yes/no]    - Send packet to MC queue(XGS3)\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_txn(int unit, args_t *a);

char cmd_ltsw_trace_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "   trace <option> [args...]\n"
#else
    "   trace tx PortBitMap=<pbmp> DATA=<packet>\n\t"
    "                           - Transmit one trace packet.\n\t"
    "   trace parse PortBitMap=<pbmp> DOP=<dop_id> [DATA=<parse_data>]\n\t"
    "                           - Parse trace data.\n\t"
    "\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_mpls(int unit, args_t *a);

char       cmd_ltsw_mpls_usage[] =
    "  mpls init\n\t"
    "        - Init MPLS software system.\n\t"
    "  mpls cleanup\n\t"
    "        - Detach MPLS software system.\n\t"
    "  mpls vpn create vpws VPN=<vpn_id>\n\t"
    "        - Create VPWS vpn with given id.\n\t"
    "  mpls vpn create vpls VPN=<vpn_id> BroadcastGroup=<bcast_id>\n\t"
    "                    [Policer=<policer_id>]\n\t"
    "        - Create VPLS vpn with given id  and broadcast group.\n\t"
    "  mpls vpn port add VPN=<vpn_id> [MplsPortId=<mpls_port_id>]\n\t"
    "                    [Port=<port>] [PortGroup=<port_group>]\n\t"
    "                    PortMatch=<pm>\n\t"
    "                    [IntfFlaGs=<flags>] [VCInitLabel=<vcinit_label>]\n\t"
    "                    [EgrLabelOptions=<labelopt>]\n\t"
    "                    [VCTermLabel=<vcterm_label>] [Vlan=<vlan>]\n\t"
    "                    [TunnelId=<tunnel_id>]\n\t"
    "                    [EgrTTL=<egress_ttl>] [QoSMapID=<qos_map_id>]\n\t"
    "                    [ServiceVID=<vlan>] [ServiceTPID=<tpid>]\n\t"
    "                    [IntfCLass=<val>] [EXPMapPtr=<exp_ptr>] [IntPRI=<pri>]\n\t"
    "                    [InnerVLAN=<vid>] [MTU=<val>] [EgrObj=<egress_obj_id>]\n\t"
    "                    [EncapId=<encap_id>] [TunnelId=<tunnel_id>]\n\t"
    "        - Add a port to VPN.\n\t"
    "  mpls vpn port delete VPN=<vpn_id> MplsPort=<mpls_port_id>\n\t"
    "        - Delete an MPLS port from vpn.\n\t"
    "  mpls vpn destroy VPN=<vpn_id>\n\t"
    "        - Destroy a vpn.\n\t"
    "  mpls vpn show VPN=<vpn_id>\n\t"
    "        - Show VPN info.\n\t"
    "  mpls tunnel init set EgrTunnelLabel=<egr_tun_label>\n\t"
    "                       EgrLabelOptions=<egr_label_opt>\n\t"
    "                       EgrTTL=<egre_ttl> INtf=<intf_id>\n\t"
    "        - Initiate tunnel.\n\t"
    "  mpls tunnel init clear INtf=<intf_id>\n\t"
    "        - Clear tunnel.\n\t"
    "  mpls tunnel switch add SwitchOptions=<label_switch_opt>\n\t"
    "                         ACTion=<act> Port=<gport>\n\t"
    "                         EXPMapPtr=<exp_ptr> IntPRI=<pri>\n\t"
    "                         IngLabel=<label> IngInnerLabel=<inner_label>\n\t"
    "                         IngIntf=<ingress_interface>\n\t"
    "                         EgrLabel=<egr_label>\n\t"
    "                         EgrLabelOptions=<labelopt> EgrLabelTTL=<ttl>\n\t"
    "                         EgrLabelEXP=<exp>\n\t"
    "                         EgrObject=<egr_object_id> VPN=<vpn_id>\n\t"
    "        - Add ILM for ingress label.\n\t"
    "  mpls tunnel switch del IngLabel=<label>\n\t"
    "        - Delete ILM.\n\t"
    "  mpls tunnel encap create EncapId=<encap_id>\n\t"
    "                           [EncapOptions=<encapopt>]\n\t"
    "                           EgrTunnelLabel=<label> EgrTTL=<ttl>\n\t"
    "                           EgrLabelOptions=<labelopt>\n\t"
    "        - Create MPLS tunnel encap.\n\t"
    "  mpls tunnel encap show EncapId=<encap_id>\n\t"
    "        - Show MPLS tunnel encap.\n\t"
    "  mpls tunnel encap delete EncapId=<encap_id>\n\t"
    "        - Delete MPLS tunnel encap.\n\t"
    "  mpls options Type=[PORT|PortMatch|MplsSwitch|MplsEgressLabel|MplsTunnelEncap]\n\t"
    "        - List various options/flags used in mpls commands.\n\t" "\n"
    ;
extern cmd_result_t cmd_ltsw_trace(int unit, args_t *a);

char cmd_ltsw_pcktwatch_usage[] =
    "Parameters: [report +/-<values>] [stop|start] [count]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tlog # - specifies # packets to remember\n"
    "\tdump [# | options] - # > 0 dumps # packets from start of log\n"
    "\t                     # < 0 dumps last # packets\n"
    "\t                     nothing - dumps all logs\n"
    "\t                     options - count   - print packet # only\n"
    "\t                               decode  - attempt to decode packet\n"
    "\t                               raw     - dump raw byte stream (host order)\n"
    "\t                               pmd     - dump rx metadata and reason\n"
    "\t                               Prepend + to set, - to clear, nothing to toggle\n"
    "\treport - count   - print packet # only\n"
    "\t         decode  - attempt to decode packet\n"
    "\t         raw     - dump raw byte stream (host order)\n"
    "\t         pmd     - dump rx metadata and reason\n"
    "\t         Prepend + to set, - to clear, nothing to toggle\n"
    "\tpmd - nonzero - print non-zero pmd only\n"
    "\t    - all     - print all pmd\n"
    "\tquiet - disable all reporting\n"
    "\tstart - start the PW-daemon\n"
    "\tstop  - stop the PW-daemon and deallocate log space\n"
    "\tcount - print # packets received.\n"
    "\treset - reinitialize configuration\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_pcktwatch(int unit, args_t *a);

/*! Help for CLI flexctr command. */
char cmd_ltsw_flexctr_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  flexctr <option> [args...]\n"
#else
    "  flexctr quantization add <Object>=<Val> <NumRange>=<Val>\n\t"
    "                           [<Val>-<Val>] [<Val>-<Val>] ...\n\t"
    "                           - Create flexctr quantization\n\t"
    "  flexctr quantization update <QuantId>=<Val>\n\t"
    "                              <Object>=<Val> <NumRange>=<Val>\n\t"
    "                              [<Val>=<Val>] [<Val>=<Val>] ...\n\t"
    "                           - Update flexctr quantization\n\t"
    "  flexctr quantization del [<QuantId>=<Val>]\n\t"
    "                           - Delete flexctr quantization\n\t"
    "  flexctr quantization show [<QuantId>=<Val>]\n\t"
    "                           - Show flexctr quantizaiton\n\t"
    "  flexctr index set <Object0>=<Val> <Object1>=<Val>\n\t"
    "                    <BitNum0>=<Val> <BitNum1>=<Val>\n\t"
    "                           - Setup action index for creation\n\t"
    "  flexctr valueA set  <SELect>=<Val> <Type>=<Val>\n\t"
    "                      [<Object0>=<Val>] [<Object1>=<Val>]\n\t"
    "                           - Setup action value A for creation\n\t"
    "  flexctr valueB set  <SELect>=<Val> <Type>=<Val>\n\t"
    "                      [<Object0>=<Val>] [<Object1>=<Val>]\n\t"
    "                           - Setup action value B for creation\n\t"
    "  flexctr trigger set <Type>=<Val> [<SCale>=<Val>] [<Period>=<Val>]\n\t"
    "                      [<Object>=<Val>] [<START>=<Val>] [<STOP>=<Val>]\n\t"
    "                           - Setup action trigger for creation\n\t"
    "  flexctr trigger update <StatId>=<Val> <Enable>=<true|false>\n\t"
    "                           - Enable/Disable action trigger\n\t"
    "  flexctr action add <Source>=<Val> <Hint>=<Val>\n\t"
    "                     <Mode>=<Val> <Trigger>=<Val>\n\t"
    "                           - Create flexctr action\n\t"
    "  flexctr action del [<StatId>=<Val>]\n\t"
    "                           - Delete flexctr action\n\t"
    "  flexctr action show [<StatId>=<Val>]\n\t"
    "                           - Show flexctr action\n\t"
    "  flexctr stat show <StatId>=<Val> [<Val>-<Val>]\n\t"
    "                           - Get flexctr stat\n\t"
    "  flexctr stat clear <StatId>=<Val> [<Val>-<Val>]\n\t"
    "                           - Clear flexctr stat\n\t"
    "\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_flexctr(int unit, args_t *a);

char cmd_ltsw_multicast_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  multicast <options> [arg...]\n\t"
#else
    "  multicast create Type=L2|L3|VPLS|FLOW [Group=<num>]\n\t"
    "            - Create a mulitcast group.\n\t"
    "  multicast destroy Group=<num>\n\t"
    "            - Destory a mulitcast group.\n\t"
    "  multicast encap create EncapType=L3|L3TNL|L2TNL|L2TNLACC UlEgressIf=<num> L3Intf=<num> PortID=<num>\n\t"
    "            - Create a mulitcast encapsulation ID.\n\t"
    "  multicast encap destroy EncapID=<num>\n\t"
    "            - Destroy a mulitcast encapsulation ID.\n\t"
    "  multicast encap show EncapID=<num>\n\t"
    "            - Show encapsulation.\n\t"
    "  multicast add Group=<num> Port=<num> EncapId=<num>\n\t\t"
    "            - Add port member to a multicast group.\n\t"
    "  multicast delete Group=<num> [Port=<num> EncapId=<num>]\n\t"
    "            - Delete port member from a multicast group.\n\t"
    "  multicast show [Group=<num>]\n"
#endif
    ;
extern cmd_result_t cmd_ltsw_multicast(int unit, args_t *arg);

char cmd_ltsw_ipmc_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  ipmc <options> [arg...]\n"
#else
    "  ipmc init\n\t"
    "       - Init ipmc function.\n\t"
    "  ipmc detach\n\t"
    "       - Detach ipmc function.\n\t"
    "  ipmc table add Src_IP=<ipaddr> Mc_IP=<grpaddr>\n\t"
    "         Mc_IPMaskLen=<grpaddr> VlanID=<vid>\n\t"
    "         Group=<val> [GroupL2=<val>] [src_Port=<port>] [COS=<cosl>]\n\t"
    "         [TS=<tsl>] [Valid=<bool>] [NoCHECK=<bool>] [IngIntf=<val>]\n\t"
    "         [Replace=<bool>] [VRF=<vrf>] [LookupClass=<val>] [Hit=<bool>]\n\t"
    "       - Add an entry into ipmc table.\n\t"
    "  ipmc table del Src_IP=<ipaddr> Mc_IP=<macaddr> VlanID=<vid>\n\t"
    "       - Destroy an entry in ipmc table.\n\t"
    "  ipmc table clear\n\t"
    "       - Delete all ipmc table.\n\t"
    "  ipmc table show [entry]\n\t"
    "       - Display all ipmc table or a single one.\n\t"
    "  ipmc ip6table add Src_IP=<ipaddr> Mc_IP=<grpaddr>\n\t"
    "         Mc_IPMaskLen=<grpaddr> VlanID=<vid>\n\t"
    "         Group=<val> [src_Port=<port>] [COS=<cosl>]\n\t"
    "         [NoCHECK=<bool>] [TS=<tsl>] [IngIntf=<val>]\n\t"
    "         [Valid=<bool>] [LookupClass=<val>] [VRF=<vrf>] [Hit=<bool>]\n\t"
    "        - Add an entry into ipmc table.\n\t"
    "  ipmc ip6table del Src_IP=<ipaddr> Mc_IP=<macaddr> VlanID=<vid> [VRF=<vrf>]\n\t"
    "       - Destroy an entry in ipmc table\n\t"
    "  ipmc ip6table clear\n\t"
    "       - Delete all ipmc table.\n\t"
    "  ipmc ip6table show [entry]\n\t"
    "       - Display all ipmc table or a single one.\n"
#endif
    ;

char cmd_ltsw_stk_port_set_usage[] =
    "\nStackPortSet Port=<n> [Unit=<n>] [Capable=<T|F>] [Enable=<T|F>]\n"
    "    [Inactive=<T|F>] [Simplex=<T|F>] [Duplex=<T|F>] [Internal=<T|F>]\n"
    "    Add a stack port to the system with the indicated flags\n";

extern cmd_result_t cmd_ltsw_stk_port_set(int unit, args_t *args);

char cmd_ltsw_stk_port_get_usage[] =
    "\nStackPortGet [Port=<n>] [Unit=<n>]\n"
    "    Get stack port information.  If no port is specified, get all\n"
    "    information for the current or specified device\n"
    "    If unit < 0 is specified, get all info for all local devices.\n";

extern cmd_result_t cmd_ltsw_stk_port_get(int unit, args_t *args);

char cmd_ltsw_stkmode_usage[] =
    "Parameters:\n"
    "    Modid=<m>            Set the module identifier\n"
    "    ModPortClear=[T|F]   Clear modport table\n"
    "If no parameters, show current state\n";

extern cmd_result_t cmd_ltsw_stkmode(int unit, args_t *args);

char cmd_ltsw_flexdigest_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\taction <add|get|remove> <eid> [act] [p0] [p1]\n"
"\tdetach\n"
"\tentry create <gid> [<eid>]\n"
"\tentry install|reinstall|remove|destroy <eid>\n"
"\tentry prio <eid> <priority>\n"
"\tgroup create <pri> [<gid>]\n"
"\tgroup destroy|get <gid>\n"
"\thash profile create|destroy [<profile_id>]\n"
"\thash profile set|get <profile_id> <TYPE> [<value>]\n"
"\thash salt set|get <BinA|BinB|BinC> [<HHHH,HHHH,HHHH,HHHH,...>]\n"
"\tinit\n"
"\tmatch create\n"
"\tmatch destroy [<match_id>]\n"
"\tmatch add|delete <match_id> <MATCH>\n"
"\tmatch clear|show <match_id>\n"
"\tnorm seed profile create|destroy [<profile_id>]\n"
"\tnorm seed profile set|get <profile_id> <TYPE> [<value>]\n"
"\tnorm profile create|destroy [<profile_id>]\n"
"\tnorm profile set|get <profile_id> <TYPE> [<value>]\n"
"\tqset add|clear|set|show [qualifier]\n"
"\tqual <eid> <QUAL> data mask \n"
"\tqual <eid> delete <QUAL>\n"
"\tqual <eid> clear\n"
"\tshow [group|entry|qset] [id]\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_ltsw_flexdigest(int unit, args_t *a);

/*! Help for CLI HA command. */
char cmd_ltsw_ha_usage[] =
    "\n"
    "\tHA alloc status [<component> ...]\n"
    "\tHA file info\n"
    "\tHA compare <ha path>\n"
    ;
extern cmd_result_t cmd_ltsw_ha(int unit, args_t *args);

char if_ltsw_field_proc_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\tshow [group|entry|presel] [id] [brief]\n"
"\tlist actions/qualifiers <stage>\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;

extern cmd_result_t if_ltsw_field_proc(int unit, args_t *a);

char cmd_ltsw_alpm_usage[] =
    "\n\talpm usage show"
    "\n\talpm projection show [v4v6_ratio x100]\n";

#endif /* BCM_LTSW_SUPPORT */
