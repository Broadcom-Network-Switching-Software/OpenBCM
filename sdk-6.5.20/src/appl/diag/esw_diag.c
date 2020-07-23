/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW socdiag command list
 */
 
#include "appl/diag/diag.h"
       
char cmd_age_esw_usage[] =
    "Parameters:  [<seconds>]\n\t"
    "   Set the hardware age timer to the indicated number of seconds.\n\t"
    "   With no parameter, displays current value.\n\t"
    "   Setting to 0 disables hardware aging\n";

extern cmd_result_t cmd_esw_age(int unit, args_t *a);
char auth_esw_cmd_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "auth <option> [args...]\n"
#else
    "auth init\n\t"
    "       - Init auth function\n\t"
    "auth detach\n\t"
    "       - Detach auth function\n\t"
    "auth mac init\n\t"
    "       - Add switch mac address from config to all ports\n\t"
    "auth mac add  [PortBitMap = <pbmp>] [MACaddress=<address>]\n\t"
    "       - Add switch mac address\n\t"
    "auth mac del  [PortBitMap = <pbmp>] [MACaddress=<address>]\n\t"
    "       - Delete switch mac address\n\t"
    "auth mac clear  [PortBitMap = <pbmp>]\n\t"
    "       - Clear all switch mac addresses\n\t"
    "auth block [PortBitMap = <pbmp>] [IngressOnly=true|false]\n\t"
    "       - Block traffic for all directions or ingress direction only\n\t"
    "auth unblock [PortBitMap = <pbmp>]\n\t"
    "       - All traffic allowed, ports in uncontrolled state\n\t"
    "auth enable [PortBitMap = <pbmp>] [LearnEnable=true|false]\n\t"
    "            [IgnorLink=true|false]\n\t"
    "       - Authorized ports to allow traffic\n\t"
    "auth disable [PortBitMap = <pbmp>]\n\t"
    "       - Put ports back in block state\n\t"
    "auth show\n\t"
    "       - Show ports access state\n"
#endif
    ;
extern cmd_result_t cmd_esw_auth(int unit, args_t *a);

char if_esw_bpdu_usage[] =
    "Usages:\n\t"
    "  bpdu add [Index=<n>] [MACaddress=<mac>]\n\t"
    "        - Add BPDU addresses by index\n\t"
    "  bpdu del [Index=<n>]\n\t"
    "        - Delete BPDU address by index\n\t"
    "  bpdu show \n\t"
    "        - Show BPDU addresses and index range\n";

extern cmd_result_t if_esw_bpdu(int unit, args_t *a);

char cmd_esw_cablediag_usage[] =
	"Run Cable Diagnostics on a set of ports.\n"
	"Parameter: <portbitmap>\n";
extern cmd_result_t cmd_esw_cablediag(int unit, args_t *a);

char mem_esw_cache_usage[] =
    "Parameters: [+<TABLE>[.COPYNO] | -<TABLE>[.COPYNO]] ...\n\t"
    "If no parameters are given, displays caching status.  Otherwise,\n\t"
    "turns software caching on (+) or off (-) for specified TABLE(s).\n\t"
    "Use \"listmem\" for a list of tables and cachability.\n";

extern cmd_result_t mem_esw_cache(int unit, args_t *a);

char cmd_esw_clear_usage[] =
    "clear counters [PBMP] | [ALL] <TABLE> ... | DEV | SA | stats [PBMP]\n\t"
    "counters - zeroes all or some internal packet counters\n\t"
    "<TABLE> - clears specified memory table\n\t"
    "ALL - when clearing a sorted table, overwrite all entries instead\n\t"
    "      of just occupied ones (i.e. 1 through software-entry-count).\n\t"
    "DEV - Call bcm_clear on the unit to reset to known state\n\t"
    "SA - clears software ARL shadow table\n";

extern cmd_result_t cmd_esw_clear(int unit, args_t *a);

char if_esw_combo_usage[] =
    "Parameters: <ports> [copper|fiber [<option>=<value>]]\n"
    "            <ports> watch [on|off]\n\t"
    "Display or update operating parameters of copper/fiber combo PHY.\n\t"
    "Note: the 'port' command operates on the currently active medium.\n\t"
    "Example: combo ge1 fiber autoneg_enable=1 autoneg_advert=1000,pause\n\t"
    "Watch subcommand enables/disables media change notifications\n";

extern cmd_result_t if_esw_combo(int unit, args_t *a);

char cmd_esw_cos_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "cos <option> [args...]\n"
#else
    "cos clear                  - Reset COS configuration to default\n\t"
    "cos config [Numcos=#]      - Set number of active COS queues\n\t"
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
    "cos discard_port [PortBitMap=<pbmp>] [Queue=#] [Color=green|yellow|red]\n\t"
    "                 [DropSTart=#] [DropSLope=#] [AvgTime=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set port discard (WRED) config\n\t"
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
extern cmd_result_t cmd_esw_cos(int unit, args_t *a);

char cmd_esw_counter_usage[] =
    "\nParameters:  [on] [off] [sync] [Interval=<usec>]\n\t"
    "\t[PortBitMap=<pbm>] [DMA=<true|false>]\n\t"
    "Starts the counter collection task and/or DMA running every <usec>\n\t"
    "microseconds.  The task tallies software counters based on hardware\n\t"
    "values and must run often enough to avoid counter overflow.\n\t"
    "If <interval> is 0, stops the task.  If <interval> is omitted, prints\n\t"
    "the current INTERVAL.  sync reads in all the counters to synchronize\n\t"
    "'show counters' for the first time, and must be used alone.\n";

extern cmd_result_t cmd_esw_counter(int unit, args_t *a);

char cmd_esw_custom_stat_usage[] =
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

extern cmd_result_t cmd_esw_custom_stat(int unit, args_t *a);


char if_esw_dscp_usage[] =
    "Usages:\n"
    "  dscp pbmp [Mode=<mode>][source [mapped [prio [cng]]]]\n"
    "        - map source dscp to mapped\n";

extern cmd_result_t if_esw_dscp(int unit, args_t *a);

char if_esw_dtag_usage[] =
	"Usage:\n"
	"\tdtag show <pbmp>\n"
	"\tdtag mode <pbmp> none|internal|external [addInnerTag|removeInnerTag]\n"
	"\tdtag tpid <pbmp> hex-value\n"
	"\tdtag addTpid <pbmp> hex-value\n"
	"\tdtag deleteTpid <pbmp> hex-value\n";

extern cmd_result_t if_esw_dtag(int unit, args_t *a);

char cmd_esw_dump_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "DUMP [options]\n"
#else
    "DUMP [File=<name>] [Append=true|false] [raw] [hex] [all] [chg]\n\t"
    "        <TABLE>[.<COPYNO>] [<INDEX>] [<COUNT>] [[field1]..[fieldN]]\n\t"
    "        [-filter <FIELD>=<VALUE>[,...]]\n\t"
    "      If raw is specified, show raw memory words instead of fields.\n\t"
    "      If hex is specified, show hex data only (for Expect parsing).\n\t"
    "      If all is specified, show even empty or invalid entries\n\t"
    "      If chg is specified, show only fields changed from defaults\n\t"
    "      If pipe_x is specified show data from pipe_x(chip has two pipes)\n\t"
    "      If pipe_y is specified show data from pipe_y(chip has two pipes)\n\t"
    "      If pipe0 is specified show data from pipe0(chip has four pipes)\n\t"
    "      If pipe1 is specified show data from pipe1(chip has four pipes)\n\t"
    "      If pipe2 is specified show data from pipe2(chip has four pipes)\n\t"
    "      If pipe3 is specified show data from pipe3(chip has four pipes)\n\t"
    "      If pipe4 is specified show data from pipe4(chip has four pipes)\n\t"
    "      If pipe5 is specified show data from pipe5(chip has four pipes)\n\t"
    "      If pipe6 is specified show data from pipe6(chip has four pipes)\n\t"
    "      If pipe7 is specified show data from pipe7(chip has four pipes)\n\t"
    "      If nocache is specified, show data from h/w tables, skip cache\n\t"
    "      If nocheck_mem_mode is specified, checking of unique_global mode, slice_mode will be skipped\n\t"
    "      (Use \"listmem\" command to show a list of valid tables)\n\t"
    "DUMP PCIC                     (PCI config space)\n\t"
    "DUMP PCIM [<START> [<COUNT>]] (CMIC PCI registers)\n\t"
    "DUMP MCS <START> [<COUNT>] (MicroController Subsystem)\n\t"
    "      For both Registers and Memories, (32 bits).\n\t"
    "DUMP SOC [ADDR | RVAL | DIFF] (All SOC registers)\n\t"
    "      ADDR shows only addresses, doesn't actually load.\n\t"
    "      RVAL shows reset defaults, doesn't actually load.\n\t"
    "      DIFF shows only regs not equal to their reset defaults.\n\t"
    "DUMP SOCMEM [DIFF] [EXT] (All SOC memories)\n\t"
    "      DIFF shows only memories not equal to their reset defaults.\n\t"
    "      EXT Extended Dump - also shows mmu memories\n\t"
    "DUMP MW [<START> [<COUNT>]]   (System memory, 32 bits)\n\t"
    "DUMP MH [<START> [<COUNT>]]   (System memory, 16 bits)\n\t"
    "DUMP MB [<START> [<COUNT>]]   (System memory, 8 bits)\n\t"
    "DUMP SA                       (ARL shadow table)\n\t"
    "DUMP DV ADDR                  (DMA vector)\n\t"
    "DUMP PHY [<PHYID>]            See also, the 'PHY' command.\n\t"
    "DUMP FORMAT [<name> <value>]  Display the format fields and values.\n"
#endif
    ;
extern cmd_result_t cmd_esw_dump(int unit, args_t *a);

char cmd_esw_reg_edit_usage[] =
    "Parameters: <REGISTER>\n\t"
    "Loads a register and displays each field, providing an opportunity\n\t"
    "to modify each field.\n";

extern cmd_result_t cmd_esw_reg_edit(int unit, args_t *a);


char if_esw_fcoe_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "  fcoe <cmd> <fc-port> <cna-port>\n"
    "  cmd = start/stop\n"
#else 
    "  fcoe <cmd> <fc-port> <cna-port>\n"
    "  cmd = start/stop\n"
#endif
    ;
extern cmd_result_t if_esw_fcoe(int unit, args_t *a);

char cmd_pemode_usage[] =
    "  pemode\n"
    "  Initializes the system for PE Mode\n"
    ;
extern cmd_result_t cmd_pemode(int unit, args_t *a);

char cmd_peapp_usage[] =
    "  peapp\n"
    "  Access to PE Lib commands\n"
    ;
extern cmd_result_t cmd_peapp(int unit, args_t *a);

char if_esw_field_proc_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\taction <add|get|remove> <eid> [act] [p0] [p1]\n"
"\taction ports <add|get> <eid> <act> [<pbmp>]\n"
"\taction mac <add|get> <eid> <act> [<mac>]\n"
"\taction oam_stat <add|delete [all] |get [all]> <eid>\n"
"\t       object=<StatObj> mode=<mode_id> action=<action>\n"
"\taset <add|delete|show|clear> <gid> [action|action list]\n"
"\tcontrol <ctrl_num> [<status>]\n"
"\tdetach\n"
"\tentry create <gid> [<eid>]\n"
"\tentry copy <src_eid> [<dst_eid>]\n"
"\tentry install|reinstall|remove|destroy <eid>\n"
"\tentry enable|disable <eid>\n"
"\tentry prio <eid> [highest|lowest|dontcare|default|<priority>]\n"
"\tentry oper <eid> [backup|restore|cleanup]\n"
"\tgroup create <pri> [<gid>] [mode] [pbmp|pipe0|pipe1|pipe2|pipe3] [size]\n"
"\tgroup destroy|get|set <gid>\n"
"\tgroup compress <gid>\n"
"\tinit\n"
"\tlist actions|qualifiers [stage]\n"
"\trange create [<rid>] [<flags>] [<min>] [<max>]\n"
"\trange group create [<rid>] [<flags>] [<min>] [<max>] [<group>]\n"
"\trange get|destroy <rid>\n"
"\tresync\n"
"\tpolicer create PolId=<polcerid> mode=<mode> cbs=<cbs> \n"
"\t        cir=<cir> ebs=<ebs> eir=<eir> ColorBlind=<bool> \n" 
"\t        ColorMergeOr=<bool> PacketBased=<bool>\n"
"\tpolicer set PolId=<polcerid> mode=<mode> cbs=<cbs> \n"
"\t        cir=<cir> ebs=<ebs> eir=<eir> ColorBlind=<bool> \n" 
"\t        ColorMergeOr=<bool> PacketBased=<bool>\n"
"\tpolicer destroy [all] or PolId=<polcerid>] \n"
"\tpolicer attach entry=<eid> level=<level> PolId=<polcerid> \n"
"\tpolicer detach entry=<eid> level=<level> \n"
"\tstat create group=<group> type0=<stat_type0> type1=<stat_type1>\n"
"\t        type2=<stat_type2> typeX=<stat_typeX>\n"
"\tstat destroy StatId=<statid> \n"
"\tstat attach entry=<eid> [StatId=<statid>]* \n"
"\t         (*if not specified, last created StatId is used) \n"
"\tstat detach entry=<eid> StatId=<statid> \n"
"\tstat set StatId=<statid> type=<stat_type> val=<value>\n"
"\tstat get StatId=<statid> type=<stat_type>\n"
"\tdata create OffsetBase=<offset_base> offset=<offset> length=<length>\n"
"\tdata destroy [all] or QualId=<qualid> \n"
"\tdata format add QualId=<qualid> RelativeOffset=<relative_offset> \n"
"\t     L2=<l2> VlanTag=<tag_format> OuterIp=<outer_ip> InnerIp=<inner_ip>\n"
"\t     Tunnel=<tunnel> mpls=<mpls> Higig=<higig> Vntag=<vntag> \n"
"\t     Cntag=<cntag> Etag=<etag> Icnm=<icnm>\n"
"\tdata format delete QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     L2=<l2> VlanTag=<tag_format> OuterIp=<outer_ip> InnerIp=<inner_ip> \n"
"\t     Tunnel=<tunnel> mpls=<mpls> Higig=<higig> Vntag=<vntag> \n"
"\t     Cntag=<cntag> Etag=<etag> Icnm=<icnm>\n"
"\tdata ethertype add QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     etype=<ethertype> VlanTag=<tag_format> L2=<l2> Higig=<higig> \n"
"\t     Vntag=<vntag> Cntag=<cntag> Etag=<etag> Icnm=<icnm>\n"
"\tdata ethertype delete QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     etype=<ethertype> VlanTag=<tag_format> L2=<l2> Higig=<higig> \n"
"\t     Vntag=<vntag> Cntag=<cntag> Etag=<etag> Icnm=<icnm>\n"
"\tdata ipproto add QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     protocol=<protocol> VlanTag=<tag_format> L2=<l2> IpVer=<ip_version>\n"
"\tdata ipproto delete QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     protocol=<protocol> VlanTag=<tag_format> L2=<l2> IpVer=<ip_version>\n"
"\tqual <eid> <QUAL> [<udf_id*>/<data_qual_id>] data mask \n"
"\t        (*required for QUAL=UserDefined or Data)\n"
"\tqual <eid> delete <QUAL>\n"
"\tqual <eid> clear\n"
"\tqset add|clear|set|show [qualifier|qualifier list]\n"
"\tshow [class] [group|entry] [id] [brief]\n"
"\tstatus\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t if_esw_field_proc(int unit, args_t *a);

char cmd_esw_reg_get_usage[] =
    "Parameters: [hex|raw|chg] [<REGTYPE>] <REGISTER> [[field1]..[fieldN]]\n\t"
    "If <REGTYPE> is not specified, it defaults to \"soc\".\n\t"
    "<REGISTER> designates offset, address, or symbolic name.\n\t"
    "If hex is specified, dumps only a hex value (for Expect parsing).\n\t"
    "If raw is specified, no field decoding is done.\n\t"
    "If chg is specified, show only fields/regs changed from defaults.\n\t"
    "For a list of register types, use \"dump\".\n";

extern cmd_result_t cmd_esw_reg_get(int unit, args_t *a);

char if_esw_ipg_usage[] =
    "Parameters: [PortBitMap=<pbmp>] [SPeed=10|100|1000|2500|10000]\n\t"
    "[FullDuplex=true|false] [Gap=<int>]\n"
    "Set the IPG register values to the specified values when the\n\t"
    "MAC is programmed after autonegotiation OR when the port is\n\t"
    "into the specific mode.  If no args are given, displays settings.\n";

extern cmd_result_t if_esw_ipg(int unit, args_t *a);

char if_esw_l2_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  l2 <option> [args...]\n"
#else
    "  l2 add [PortBitMap=<pbmp> or Port=<port>] [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "         [Trunk=true|false] [TrunkGroupId=<id>] [STatic=true|false]\n\t"
    "         [SourceCosPriority=true|false] [DiscardSource=true|false]\n\t"
    "         [DiscardDest]=true|false] [L3=true|false] [HIT=true|false]\n\t"
    "         [Replace=true|false] [CPUmirror=true|false] [MIRror=true|false]\n\t"
    "         [ReplacePriority=true|false] [RemoteLookup=true|false]\n\t"
    "         [MacBlockPortBitMap=<pbmp>] [Group=<group>]\n\t"
    "         [Mesh=true|false]\n\t"
    "        - Add incrementing L2 addresses associated with port(s)\n\t"
    "  l2_replace [Module=<n>] [Port=<port>] [MACaddress=<mac>]\n\t"
    "             [Vlanid=<id>] [Trunk=true|false] [TrunkGroupId=<id>]\n\t"
    "             [STatic=true|false] [Pending=true|false] \n\t"
    "             [NewModule=<n>] [NewPort=<port>] [NewTrunkGroupId=<id>]\n\t"
    "             [Mesh=true|false]\n\t"
    "  l2 del [MACaddress=<mac>] [Count=<value>] [Vlanid=<id>]\n\t"
    "        - Delete incrementing L2 address(s)\n\t"
    "  l2 show [PortBitMap=<pbmp]\n\t"
    "        - Show L2 addresses associated with port(s)\n\t"
    "  l2 count\n\t"
    "        - Show the L2 counts on different memories\n\t"
    "  l2 check [hit, valid, static]\n\t"
    "        - Check full L2 table for valid entries\n\t"
    "  l2 clear [Module=<n>] [Port=<port>] [MACaddress=<mac>]\n\t"
    "           [Vlanid=<id>] [TrunkGroupID=<id>] [Static=true|false]\n\t"
    "        - Remove all L2 entries on the given module, module/port,\n\t"
    "           VLAN, or trunk group ID\n\t"
    "  l2 hash [MACaddress=<mac>] [Vlanid=<id>] [Hash=<hash_type>]\n\t"
    "        - Calculate L2 hash index\n\t"
    "  l2 conflict [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "        - Dump all conflicting L2 entries (same hash bucket)\n\t"
    "  l2 watch [start | stop]\n\t"
    "        - Watch dynamic address insertions/deletions\n\t"
    "  l2 info\n\t"
    "        - Display the capacity and usage of the L2 resources.\n"
#ifdef BCM_XGS3_SWITCH_SUPPORT
    "\t"
    "  l2 cache add [CacheIndex=<index>]\n\t"
    "         [MACaddress=<mac>] [MACaddressMask=<mac>]\n\t"
    "         [Vlanid=<id>] [VlanidMask=<mask>] [LookupClass=<class>]\n\t"
    "         [SourcePort=<id>] [SourcePortMask=<mask>]\n\t"
    "         [Module=<modid>] [Port=<port>] \n\t"
    "         [Trunk=true|false] [TrunkGroupId=<id>]\n\t"
    "         [SetPriority=true|false] [PRIOrity=<prio>]\n\t"
    "         [DiscardDest]=true|false] [CPUmirror=true|false]\n\t"
    "         [MIRror=true|false] [BPDU=true|false] [L3=true|false]\n\t"
    "         [ReplacePriority=true|false] [LearnDisable=true|false]\n\t"
    "         [PROTocolPacKeT=true|false]\n\t"
    "        - Add L2 cache entry\n\t"
    "  l2 cache del CacheIndex=<index> [Count=<value>]\n\t"
    "        - Delete L2 cache entry\n\t"
    "  l2 cache show\n\t"
    "        - Show L2 cache entries\n\t"
    "  l2 cache clear [ReInit=true|false]\n\t"
    "        - Delete all L2 cache entries\n\t"
#if defined(BCM_TRIUMPH_SUPPORT)
    "  l2 station add [Priority=<val>] [ID=<Sid>]\n\t"
    "         [MACaddress=<mac>] [MACaddressMask=<mask>]\n\t"
    "         [Vlanid=<id>] [VlanidMask=<mask>]\n\t"
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
    "  l2 station show ID=<sid> \n\t"
    "        - Show L2 station entry\n"
#endif /* BCM*/
#endif /* BCM_XGS3_SWITCH_SUPPORT */
#endif
    ;
extern cmd_result_t if_esw_l2(int unit, args_t *a);

char if_esw_linkscan_usage[] =
    "Parameters: [SwPortBitMap=<pbmp>] [HwPortBitMap=<pbmp>]\n\t"
    "[Interval=<usec>] [FORCE=<pbmp>] [on] [off]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tWith no arguments, displays the linkscan status for all ports.\n\t"
    "Enables software linkscan on ports specified by SwPortBitMap.\n\t"
    "Enables hardware linkscan on ports specified by HwPortBitMap.\n\t"
    "Disables linkscan on all other ports.\n\t"
    "Interval specifies non-default linkscan interval for software.\n\t"
    "Note: With linkscan disabled, autonegotiated ports will NOT have\n\t"
    "the MACs updated with speed/duplex..\n\t"
    "FORCE=<pbmp> requests linkscan to update ports once even if link\n\t"
    "status did not change.\n"
#endif
    ;

extern cmd_result_t if_esw_linkscan(int unit, args_t *a);

#ifdef SW_AUTONEG_SUPPORT
char if_esw_swAutoneg_usage[]  = 
    "Parameters: [on] [off]\n";
extern cmd_result_t if_esw_swAutoneg(int unit, args_t *a);
#endif

char cmd_esw_mem_list_usage[] =
    "Parameters: [<TABLE> [<DATA> ...]]\n\t"
    "If no parameters are given, displays a reference list of all\n\t"
    "memories and their attributes.\n\t"
    "If TABLE is given, displays the entry fields for that table.\n\t"
    "If DATA is given, decodes the data into entry fields.\n";

extern cmd_result_t cmd_esw_mem_list(int unit, args_t *a);

char cmd_esw_reg_list_usage[] =
"Usage: listreg [options] regname [value]\n"
"Options:\n"
"	-alias/-a	display aliases\n"
"	-summary/-s	display single line summary for each reg\n"
"	-counters/-c	display counters\n"
"	-ed/-e		display error/discard counters\n"
"	-type/-t	display registers grouped by block type\n"
"If regname is '*' or does not match a register name, a substring\n"
"match is performed.  [] indicates register array.\n"
"If regname is a numeric address, the register that resides at that\n"
"address is displayed.\n";
extern cmd_result_t cmd_esw_reg_list(int unit, args_t *a);

char if_esw_mcast_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  mcast <option> [args...]\n"
#else
    "For Switch Devices:\n\t"
    "  mcast add MACaddress=<val> Vlanid=<val> Cos=<val>\n\t"
    "            PortBitMap=<val> UntagBitMap=<val> Index=<val>\n\t"
    "  mcast delete MACaddress=<val> Vlanid=<val>\n\t"
    "  mcast join MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "  mcast leave MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "  mcast padd MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "  mcast premove MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "For Fabric Devices:\n\t"
    "  mcast bitmap max\n\t"
    "  mcast bitmap set Port=<val> PortBitMap=<val> Index=<val>\n\t"
    "  mcast bitmap del Port=<val> PortBitMap=<val> Index=<val>\n\t"
    "  mcast bitmap get Port=<val> Index=<val>\n"
#endif
    ;
extern cmd_result_t if_esw_mcast(int unit, args_t *a);

char        if_esw_mirror_usage[] =
  "mirror [options...]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
  "\tshow                 - display current mirror configuration\n"
  "Switch Device Options:\n"
  "\tMode=<Off|L2|L3>     - set the mirroring mode\n"
  "\tPort=<p>             - set the mirror-to destination port\n"
  "\tIngressBitMap=<pbmp> - ports with ingress mirroring enabled\n"
  "\tEgressBitMap=<pbmp>  - ports with egress mirroring enabled\n"
  "\tPreserveForMat=<Off|On> - preserve format\n"
  "Fabric Device Options: (5670/5675)\n"
  "\tPort=<p>             - which port to set the bitmap\n"
  "\tBitMap=<pbmp>        - destination bitmap for mirroring\n"
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
#endif
  ;

extern cmd_result_t if_esw_mirror(int unit, args_t *a);

char regwatch_usage[] =
    "Parameters: [Reg=<Reg>] [write] [read] [off]\n\t"
    "Reg specifies which reg to watch\n\t"
    "Write - specify to start watching writing operations on register\n\t"
    "Read  - specify to start watching reading operations on register\n\t"
    "Off   - specify to stop watching all operations on register\n";

extern cmd_result_t reg_watch(int unit, args_t *a);

extern cmd_result_t cmd_esw_ser(int unit, args_t *a);

char memwatch_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "memwatch [options...]\n"
#else
    "All Memories Read/Write Counting Watch Options:\n\t"
    "memwatch start\n\t"
    "       - Start to counting read/write operations on all memory\n\t"
    "         It is turned-on by default\n\t"
    "memwatch stop [clear]\n\t"
    "       - Stop to counting read/write operations on all memory\n\t"
    "           -Clear - specify to clear memory counters\n\t"
    "memwatch show [ |read|write] [all]\n\t"
    "       -Blank - specify to show both memory rd/wr-count update\n\t"
    "       -Read  - specify to show memory rd-count update\n\t"
    "       -Write - specify to show memory wr-count update\n\t"
    "       -ALL   - specify to show memory rd/wr-count even if no update\n\t"
    "Specify which memory to watch:\n\t"
    "memwatch [Mem=<Memory>] [write] [read] [off]\n\t"
    "       -Mem   - memory to watch\n\t"
    "       -Write - specify to start watching writing operations on memory\n\t"
    "       -Read  - specify to start watching reading operations on memory\n\t"
    "       -Off   - specify to stop watching all operations on memory\n"
#endif
    ;
extern cmd_result_t mem_watch(int unit, args_t *a);

char memfirst_usage[] =
    "Parameters: [None]\n\t"
    "Displays first valid memories and Total Valid memories for chip \n\t"
    "Will set mem_name and mem_count for external use \n";

extern cmd_result_t mem_first(int unit, args_t *a);

char memnext_usage[] =
    "Parameters: [None]\n\t"
    "Displays next valid memories and Total Valid memories for chip \n\t"
    "Will set mem_name for external use \n";

extern cmd_result_t mem_next(int unit, args_t *a);

char cmd_esw_mem_modify_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        <FIELD>=<VALUE>[,...]\n\t"
    "If nocheck_mem_mode is specified, checking of unique_global mode, slice_mode will be skipped\n\t"
    "Read/modify/write field(s) of a table entry(s).\n";

extern cmd_result_t cmd_esw_mem_modify(int unit, args_t *a);

char cmd_esw_reg_mod_usage[] =
    "Parameters: [nocache] <REGISTER> <FIELD>=<VALUE>[,...]\n\t"
    "[nocache] if nocache is specified, just update H/W data, skip cache.\n\t"
    "<REGISTER> is SOC register symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "for example: L3_ENA=0,CPU_PRI=1.\n\t"
    "Fields not specified in the list are left unaffected.\n\t"
    "For a list of register types, use \"dump\".\n";

extern cmd_result_t cmd_esw_reg_mod(int unit, args_t *a);

char cmd_esw_pbmp_usage[] =
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
extern cmd_result_t cmd_esw_pbmp(int unit, args_t *a);

char if_esw_phy_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "  phy <option> [args...]\n"
    "Default: [int] <ports> [regnum [value/devad] [value]]\n"
    "Subcommands: raw, dumpall, info, control, firmware, eee,\n"
    "             power, longreach, prbs, clock, margin, oam\n"
#else 
    "Parameters: [int] <ports> [regnum [value/devad] [value]]\n"
    "            raw [c45|sbus|sim] <mii-addr> [devad[.lane]] <regnum> [<value>[/<mask>]]\n"
    "            dumpall <c22|c45> [start_phy_addr [end_phy_addr]]\n"
    "            info\n"
    "            control <ports> [phy_control_type=value]\n" 
    "            firmware <port> set=<filename> [-y]\n"
    "            eee <port> [mode=<native|auto|none>|latency=<variable|fixed>|\n"
    "                        stats=<get|clear>|idle_th=<0-7>\n"
    "            power <ports> [mode=mode_type [sleep_time=value] "
                               "[wake_time=value]]\n"
    "            longreach   <ports> [parameter=value]\n"
    "            prbs <ports> set|get|clear Mode=<hc|si> [Polynomial=<value>]\n"
    "                         [LoopBack=true|false]\n"
    "            clock <ports> [PRImary=<t|f>] [SECondary=<t|f>]\n"
    "                          [FRequency=<125000|195312|156250> for BCM8747]\n"
    "            margin <ports> maxget|valueget|valueset|set|clear [marginval=<value>]\n"
    "            oam <ports> [MODE={y1731|bhh|ietf}] [DIR={tx|rx}] \n"
    "                        [MacCheck={yes|no}] [ENTROPY={yes|no}] \n"
    "                        [TimeStampFormat={ntp|ptp}] [ETHerType=<value>] \n"
    "                        [MACIndex={1|2|3}] [MACAddrHi=<value[47:32]]\n"
    "                        [MACAddrLow=<value[31:0]>] \n"
    "                        [MACAddrHi1=<value[47:32]>] [MACAddrLow1=<value[31:0]>] \n"
    "                        [MACAddrHi2=<value[47:32]>] [MACAddrLow2=<value[31:0]>] \n"
    "                        [MACAddrHi3=<value[47:32]>] [MACAddrLow3=<value[31:0]>] \n"
#ifdef INCLUDE_PHY_SYM_DBG
    "            SymDebug <tcp port num>\n"
    "            SymDebugOff\n"
#endif
    "\tSet or display PHY registers.  If only <ports> is specified,\n\t"
    "then registers for those ports' PHYs are displayed. <ports> is a\n\t"
    "standard port bitmap, e.g. fe for all 10/100 ports, fe5-fe7 for\n\t"
    "three FE's, etc. (see \"help pbmp\").  If the int option is given,\n\t"
    "the intermediate PHY for the port is used instead of the outer PHY.\n\t"
    "In 'raw' mode, the direct mii-address can be specified. 'c45', 'sbus'\n\t"
    "and 'sim' raw modes require a devad parameter, and only 'sbus' and 'sim'\n\t"
    "raw modes support optional lane and mask.\n\t"
    "dumpall <c22|c45> - Do a clause 22 or clause 45 PHY scan of ID registers\n\t"
    "info    - display PHY device ID and attached PHY drivers\n\t" 
#ifdef BCM_DFE_SUPPORT
    "verbose - set PHY verbosity level. Example usage: phy verbose p=1 v=10\n\t" 
#endif
    "firmware - program the firmware given by the filename to the PHY \n\t"
    "           device's non-volatile storage. The file must be in binary\n\t"
    "           format.\n\t" 
    "eee      - set or display eee mode, latency and counters for PHYs devices\n\t"
    "           implementing EEE. Mode can be native or auto. Latency may be \n\t"
    "           variable or fixed. idle_th may be a value from 0 to 7.\n\t"
    "power    - set or display power mode for the PHY devices implemented\n\t"
    "           power control. mode_type has these values: auto_low,\n\t"
    "           auto_down,auto_off,low and full. sleep_time and wake_time \n\t"
    "           only applies to auto_down power mode\n\t" 
    "control - set or display PHY settings such as WAN, Preemphasis, etc.,\n\t"
    "longreach - set or display PHY longreach settings.\n\n\t"
    "diag - phy diagnostic commands.\n\t"
    "phy diag <pbm> <sub_cmd> [sub cmd parameters]\n\t"
    " All sub commands take two general parameters: unit and if. This identifies\n\t"
    " the instance the command targets to.\n\t"
    "unit = 0,1, ....\n\t"
    "   unit takes numeric values identifying the instance of the PHY devices\n\t"
    "   associated with the given port. A value 0 indicates the internal\n\t"
    "   PHY(serdes) the one directly connected to the MAC. A value 1 indicates\n\t"
    "   the first external PHY.\n\t"
    "if(interface) = [sys | line]\n\t"
    "   interface identifies the system side interface or line side interface of\n\t"
    "   PHY device.\n\t"
    "The list of sub commands:\n\t"
    "   dsc - display lane information, include sigdet, lock etc.\n\t"
    "          Example:\n\t"
    "            BCM.0> phy diag ce0 dsc debug u=0 if=line\n\t"
    "            BCM.0> phy diag ce0 dsc debug u=1 if=sys\n\t"
    "   veye - vertical eye margin mesurement. WC only. All eye margin functions\n\t"
    "          are used in conjunction with PRBS utility in the configed speed mode\n\t"
    "   heye_r - right horizontal eye margin mesurement. WC only\n\t"
    "   heye_l - right horizontal eye margin mesurement. WC only\n\t"
    "   for the WarpLite, there are three additional parameters, live link or not,\n\t"
    "   par1 for the target BER value for example: -18 stands for 10^(-18)\n\t"
    "   par2 will be percentage range for example, 5 means checxk for +-%5 of the target BER\n\t"
    "   loopback - put the device in the given loopback mode\n\t"
    "              parameter: mode=[remote | local | none]\n\t"
    "   prbs - perform various PRBS functions. Takes all parameters of the\n\t"
    "          \"phy prbs\" command except the mode parameter.\n\t"
    "          Example:\n\t"
    "            A port has a WC serdes and 84740 PHY connected. A typical usage\n\t"
    "            is to use the PRBS to check the link between WC and the system\n\t"
    "            side of the 84740. Use port xe0 as an example:\n\t"
    "            BCM.0> phy diag xe0 prbs set unit=0 p=3\n\t"
    "            BCM.0> phy diag xe0 prbs set unit=1 if=sys p=3\n\t"
    "            BCM.0> phy diag xe0 prbs get unit=1 if=sys\n\t"
    "            BCM.0> phy diag xe0 prbs get unit=0\n\t"
#ifdef BCM_TOMAHAWK3_SUPPORT
    "   prbsstat - periodically collect PRBS error counters and\n\t"
    "          compute BER based on the port configuration and\n\t"
    "          the observed error counters.\n\t"
    "          prbsstat assumes PRBS is already running on the ports\n\t"
    "          that prbsstat will be monitoring.\n\t"
    "          Parameters:\n\t"
    "             [STArt [Interval=<secs>]]\n\t"
    "             [STOp]\n\t"
    "             [Counters]\n\t"
    "             [Ber]\n\t"
    "             [Clear]\n\t"
    "          Example:\n\t"
    "            BCM.0> phy diag cd0-cd3 prbsstat ?\n\t"
    "            BCM.0> phy diag cd0-cd3 prbsstat\n\t"
    "            BCM.0> phy diag cd0-cd3 prbsstat start i=60\n\t"
    "            BCM.0> phy diag cd0-cd3 prbsstat counters\n\t"
    "            BCM.0> phy diag cd0-cd3 prbsstat ber\n\t"
    "            BCM.0> phy diag cd0-cd3 prbsstat clear\n\t"
    "            BCM.0> phy diag cd0-cd3 prbsstat stop\n\t"
    "   fecstat - periodically collect FEC status counters such as\n\t"
    "          corrected and uncorrected FEC codewords and corrected\n\t"
    "          symbols for a given port.\n\t"
    "          The command then uses the corrected symbol count to compute\n\t"
    "          the pre-FEC bit error rate for the channel.\n\t"
    "          Parameters:\n\t"
    "             [STArt [Interval=<secs>]]\n\t"
    "             [STOp]\n\t"
    "             [Counters]\n\t"
    "             [Ber]\n\t"
    "             [Clear]\n\t"
    "          Example:\n\t"
    "            BCM.0> phy diag cd0-cd3 fecstat ?\n\t"
    "            BCM.0> phy diag cd0-cd3 fecstat\n\t"
    "            BCM.0> phy diag cd0-cd3 fecstat start i=10\n\t"
    "            BCM.0> phy diag cd0-cd3 fecstat counters\n\t"
    "            BCM.0> phy diag cd0-cd3 fecstat ber\n\t"
    "            BCM.0> phy diag cd0-cd3 fecstat clear\n\t"
    "            BCM.0> phy diag cd0-cd3 fecstat stop\n\t"
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef PHYMOD_LINKCAT_BLACKHAWK_SUPPORT
    "   linkcat - Channel Analysis tool\n\t"
    "          Example:\n\t"
    "            BCM.0> phy diag xe0 linkcat lane=1\n\t"
    "            Mode default to lpbk\n\t"
    "            BCM.0> phy diag xe0 linkcat lane=1 mode=><tx/rx/lpbk/err>\n\t"
#endif
    "   linkmon - perform various PCS linkmonitor functions. Takes following parameters \n\t"
    "            unit, interface side(line/sys), lane(lane number if 0xf enable linkmon \n\t"
    "            on the lane of a port), mode (link mon mode: 0:LMM OFF, 1: LMM_PCS49_1X10G,\n\t"
    "            2: LMM_PCS82_4X10G, 3: LMM_PCS82_2X25G, 4: LMM_PCS82_4X25G, 5: LMM_FC4,\n\t"
    "            6: LMM_FC8, 7: LMM_FC16, 8: LMM_FC32,\n\t"
    "          Example:\n\t"
    "            A typical usage of link monitor to check PCS link between TSCF and  \n\t"
    "            the system side of the 82381. Use port ce0 as an example:\n\t"
    "            BCM.0> phy diag ce0 linkmon set u=1 if=line lane=1 mode=4\n\t"
    "            BCM.0> phy diag ce0 linkmon get u=1 if=line lane=1 \n\t"
    "            BCM.0> phy diag ce0 linkmon set u=1 if=line lane=1 mode=0\n\t"
    "   mfg - run manufacturing test on PHY BCM8483X\n\t"
    "              parameter: (t)est=num (d)ata=<val> (f)ile=filename\n\t"
    "              Test should be :\n\t"
    "              1 (HYB_CANC), needs (f)ile=filename\n\t"
    "              2 (DENC),     needs (f)ile=filename\n\t"
    "              3 (TX_ON)     needs (d)ata = bit map for turning TX off on pairs DCBA\n\t"
    "              0 (EXIT)\n\t"
    "  berproj - Post FEC BER projection. Takes following parameters \n\t"
    "            berproj [hist_errcnt_threshold=<value>] [sample_time=<value>] \n\t"
    "            Example:\n\t"
    "            BCM.0> phy diag cd0 berproj\n\t"
    "            BCM.0> phy diag cd0 berproj sample_time=80\n\t"
    "            BCM.0> phy diag cd0 berproj hist_errcnt_threshold=4\n\t"
#ifdef INCLUDE_PHY_SYM_DBG
    "SymDebug - Mode that lets to connect to PHY GUI (from PHY team) to\n\t"
    "           perform MDIO read/writes on PHYs\n\t"
    "SymDebugOff - Turn off the symbolic debug Mode\n\t"
#endif
    "prbs     - set, get, or clear internal PRBS generator/checker.\n"
#endif
    ;
extern cmd_result_t if_esw_phy(int unit, args_t *a);

char if_esw_port_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: port <option> [args...]\n"
#else
    "Parameters: <pbmp> [[ENCap=IEEE|HIGIG|B5632|HIGIG2|HGOE]\n\t"
    "[AutoNeg=on|off] [ADVert=<portmode>]\n\t"
    "[LinkScan=on|off|hw|sw] [SPeed=10|100|1000] [FullDuplex=true|false]\n\t"
    "[TxPAUse=on|off] [RxPAUse=on|off] [STationADdr=<macaddr>]\n\t"
    "[LeaRN=<learnmode>] [DIScard=none|untag|all] [VlanFilter=<value>]\n\t"
    "[PRIOrity=<0-7>] [PortFilterMode=<value>]\n\t"
    "[PHymaster=<Master|Slave|Auto|None>] [Enable=<true|false>]\n\t"
    "[FrameMax=<value>]\n\t"
    "[MDIX=Auto|ForcedAuto|ForcedNormal|ForcedXover]\n\t"
    "[LoopBack=NONE|MAC|PHY]\n\t"
    "[MEdium=None|Copper|Fiber]]\n\t"
    "| [[EEE] [ENable=<enable|disable>|TxIDleTime=<0-2560>|TxWakeTime=<0-2560>|STats=<get>]]\n\t"
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    "[ASF(in case of multimode asf)=<0-3>]\n\t"
    "[ASF(in case regular asf)=<0-1>]\n\t"
#endif
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
    "EEE: Energy Efficient Ethernet.\n"
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    "\t"
    "ASF: Alternative Store and Forward (cut-thru mode).\n\t"
    "ASF Modes(in case of multimode asf): [0]SAF [1]Same Speed [2]Slow to Fast [3]Fast to Slow\n\t"
    "ASF Modes(in case regular asf): [0]Disable [1]Enable\n"
#endif
#endif
    ;
extern cmd_result_t if_esw_port(int unit, args_t *a);

char if_esw_gport_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: port = <port> \n"
#else
    "Parses and construct GPORT from given parameters \n\t"
    "Parameters: <port=gporttype(modid,portnum)> where gporttype can be \n\t"
    "modport/mp - MODPORT_GPORT takes modid and port num or name \n\t"
    "local - LOCAL_GPORT takes only port num or name \n\t"
    "trunk - TRUNK_GPORT takes only trunk id \n\t"
    "devport - DEVPORT_GPORT takes port nuber or name \n\t"
    "localcpu/lc - LOCAL_CPU_GPORT no input required \n\t"
    "invalid - INVALID_GPORT no input required \n\t"
    "blackhole/bh - BLACKHOLE_GPORT no input required \n\t"
    "mpls - MPLS_GPORT takes mpls vpn \n\t"
    "mim - MIM_GPORT takes mim id \n\t"
    "wlan - WLAN_GPORT takes wlan vpn \n\t"
    "sub - SUBPORT_GPORT takes subport port id \n\t"
    "subg - SUBPORT_GROUP_GPORT takes subport group id \n\t"
    "vlan - VLAN_GPORT takes vlan port id \n\t"
    "vxlan - VXLAN_GPORT takes vxlan port id \n\t"
    "trill - TRILL_GPORT takes trill port id \n\t"
    "niv - NIV_GPORT takes niv port id \n\t"
    "l2gre - L2GRE_GPORT takes l2gre port id \n\t"
    "extender - EXTENDER_GPORT takes extender port id \n\t"
    "\n"
#endif
    ;

extern cmd_result_t if_esw_gport(int unit, args_t *a);

char if_esw_port_rate_usage[] =
    "Set/Display port rate metering characteristics.\n"
    "Parameters: <pbm> [ingress|egress|pause [arg1 arg2]]\n\t"
    "    For Ingress or Egress: arg1 is rate, arg2 is max_burst\n\t"
    "    For Pause: arg1 is pause_thresh, arg2 is resume_thresh\n\t"
    "    rate is in kilobits (1000 bits) per second\n\t"
    "    max_burst and xxx_thresh are in kilobits (1000 bits)\n";
extern cmd_result_t if_esw_port_rate(int unit, args_t *a);

char if_esw_port_stat_usage[] =
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
    "        RX = Switch will obey pause packets\n";
extern cmd_result_t if_esw_port_stat(int unit, args_t *a);
char cmd_esw_reg_set_usage[] =
    "1. Parameters: [<REGTYPE>] <REGISTER> <VALUE>\n\t"
    "If <REGTYPE> is not specified, it defaults to \"soc\".\n\t"
    "<REGISTER> is offset, address, or symbolic name.\n"
    "2. Parameters: <REGISTER> <FIELD>=<VALUE>[,...]\n\t"
    "<REGISTER> is SOC register offset or symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "for example: L3_ENA=0,CPU_PRI=1.\n\t"
    "Fields not specified in the list are set to zero.\n\t"
    "For a list of register types, use \"help dump\".\n";
extern cmd_result_t cmd_esw_reg_set(int unit, args_t *a);

char if_esw_pvlan_usage[] =
    "Usages:\n\t"
    "  pvlan show <pbmp>\n\t"
    "        - Show PVLAN info for these ports.\n\t"
    "  pvlan set <pbmp> <vid>\n\t"
    "        - Set default VLAN tag for port(s)\n\t"
    "          Port bitmaps are read from the VTABLE entry for the VID.\n\t"
    "          <vid> must have been created and all ports in <pbmp> must\n\t"
    "          belong to that VLAN.\n";
extern cmd_result_t if_esw_pvlan(int unit, args_t *a);

char cmd_esw_rate_usage[] =
    "Parameters: [PortBitMap=<pbm>] [Limit=<limit>] [Bcast=true|false]\n\t"
    "  [Mcast=true|false] [Dlf=true|false]\n\t"
    "Enables the specified packet rate controls.\n\t"
    "  pbm       port(s) to set up or display\n\t"
    "  limit     packets per second\n\t"
    "  bcast     Enable broadcast rate control\n\t"
    "  mcast     Enable multicast rate control\n\t"
    "  dlf       Enable DLF flooding rate control\n\t"
    "If no flags are given or only <pbm> is given, displays the current\n\t"
    "rate settings.\n";
extern cmd_result_t cmd_esw_rate(int unit, args_t *a);

char cmd_esw_rx_cfg_usage[] =
    "rxcfg [<chan>] [options...]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "    With no options, displays current configuration\n"
    "    Global options include:\n"
    "        SPPS=<n>            Set system-wide packet per second limit\n"
    "                            Other options combined this are ignored\n"
    "        GPPS=<n>            Set global (all COS) pkts per second limit\n"
    "        PKTSIZE=<n>         Set maximum receive packet size\n"
    "        PPC=<n>             Set the number of pkts/chain\n"
    "        BURST=<n>           Set global (all COS) packet burst size\n"
    "        COSPPS<n>=<r>       Set the per COS rate limiting\n"
    "        FREE=[T|F]          Should handler free buffers?\n"
    "    Channel specific options include:\n"
    "        CHAINS=<n>          Set the number of chains for the channel\n"
    "        PPS=<n>             Set packet per second for channel\n"
    "        COSBMP=<bmp>        COS bitmap to accept on channel\n"
    "    Global options can be given w/o a channel.  Channel options\n"
    "    require that a channel be specified.\n"
    "    The channel's burst rate is #chains * pkts/chain\n"
#endif
    ;
extern cmd_result_t cmd_esw_rx_cfg(int unit, args_t *a);

char cmd_esw_rx_init_usage[] =
    "RXInit <override-unit>\n"
    "    Call bcm_rx_init on the given override unit.  Ignores"
    "    the current unit.\n";
extern cmd_result_t cmd_esw_rx_init(int unit, args_t *a);

char cmd_esw_rx_mon_usage[] =
    "Parameters [init|start|stop|show|[-]enqueue [n]]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "With no parameters, show whether or not active.\n"
    "    init:           Initialize the RX API, but don't register handler\n"
    "    start:          Call RX start with local pkt dump routine\n"
    "                    Modify the configuration with the rxcfg command\n"
    "    stop:           Call RX stop\n"
    "    [-]enqueue [n]: Enqueue packets to be freed later in thread\n"
    "                    If n > 0, enqueue at least n pkts before freeing\n"
    "    stop:           Call RX stop\n"
    "    show:           Call RX show\n"
#endif
    ;
extern cmd_result_t cmd_esw_rx_mon(int unit, args_t *a);

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
char if_esw_port_policer_usage[] =
    "Display info about port status in table format.\n"
    "Usages:\n\t"
    "  PortPolicer Set <pbmp><policer_id>\n\t"
    "              - Set policer for Port\n\t"
    "  PortPolicer Get <pbmp>\n\t"
    "              - Get policer for Port\n\t";
extern cmd_result_t if_esw_port_policer(int unit, args_t *a);
extern cmd_result_t cmd_esw_policer_global_meter(int unit, args_t *a);
#endif

char cmd_esw_show_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  show <args>\n"
#else
    "  show Pci        - Probe and display function 0 of all busses/devices\n"
    "  show CHips      - Show all driver-supported device IDs\n"
    "  show Counters [Changed] [Same] [Z] [NZ] [Hex] [Raw] [<reg>][.][<pbmp>]\n"
    "\tDisplay all counters, or only specified regs and/or ports\n"
    "\t  Changed - include counters that changed\n"
    "\t  Same    - include counters that did not change\n"
    "\t  Z       - include counters that are zero\n"
    "\t  Nz      - include counters that are not zero\n"
    "\t  All     - same as: Changed Same Z Nz\n"
    "\t  Hex     - display counters as 64-bit hex values\n"
    "\t  Raw     - display raw 64-bit hex values, no register name(s)\n"
    "\t  ErDisc  - Only show those counters marked with Error/Discard\n"
    "\tNOTES: If neither Changed or Same is specified, Change is used.\n"
    "\t       If neither Z or Nz is specified, Nz is used.\n"
    "\t       Use All to display counters regardless of value.\n"
    "  show Statistics [pbm] [all] - SNMP accumulated statistics, all shows 0s\n"
    "  show <mem>             - memory info (\"listmem\" for list)\n"
    "  show sa                - software ARL table\n"
    "  show Errors            - logged error counts for certain errors\n"
    "  show Interrupts        - interrupt counts for most interrupt types\n"
    "  show params [<chip>]   - Chip parameters (chip id or current unit)\n"
    "  show unit [<unit>]     - Unit list or unit parameters\n"
    "  show features [all]    - Show enabled (or all) features for this unit\n"
#if defined(BCM_KATANA2_SUPPORT)
    "  show linkphy           - Show linkphy hw information for this unit\n"
#endif /* BCM_KATANA2_SUPPORT */
#if defined(VXWORKS)
    "  show ip                - IP statistics\n"
    "  show icmp              - ICMP statistics\n"
    "  show arp               - ARP statistics\n"
    "  show udp               - UDP statistics\n"
    "  show tcp               - TCP statistics\n"
    "  show mux               - MUX protocol stack backplane\n"
    "  show routes            - IP routing table\n"
    "  show hosts             - IP host table\n"
#endif /* VXWORKS */
    "  show icid              - ICID of chip\n"
    "  show Threads           - Show threads information.\n"
#endif
    ;
extern cmd_result_t cmd_esw_show(int unit, args_t *a);

char cmd_esw_soc_usage[] =
    "Parameters: [<unit #>] ... \n\t"
    "Print internal SOC driver control information IF compiled as a \n\t"
    "debug version. If not compiled as a debug version, a warning is\n\t"
    "printed and the command completes successfully with no further\n\t"
    "output\n";
extern cmd_result_t cmd_esw_soc(int unit, args_t *a);

char if_esw_stg_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  stg <option> [args...]\n"
#else
    "  stg create [<id>]            - Create a STG; optionally specify ID\n\t"
    "  stg destroy <id>             - Destroy a STG\n\t"
    "  stg show [<id>]              - List STG(s)\n\t"
    "  stg add <id> <vlan_id> [...]     - Add VLAN(s) to a STG\n\t"
    "  stg remove <id> <vlan_id> [...]  - Remove VLAN(s) from a STG\n\t"
    "  stg stp                      - Get span tree state, all ports/STGs\n\t"
    "  stg stp <id>                 - Get span tree state of ports in STG\n\t"
    "  stg stp <id> <pbmp> <state>  - Set span tree state of ports in STG\n\t"
    "                                 (disable/block/listen/learn/forward)\n\t"
    "  stg default [<id>]           - Show or set the default STG\n"
#endif
    ;
extern cmd_result_t if_esw_stg(int unit, args_t *a);

char if_esw_trunk_usage[] =
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
    "  trunk egress [<Id=val>] <Pbmp=val>\n\t"
    "        - Set egress ports for trunk\n\t"
    "  trunk mcast <Id=val> <Mac=val> <Vlan=val>\n\t"
    "        - Join multicast to a trunk\n\t"
#ifdef BCM_FIREBOLT_SUPPORT
    "  trunk hash set <Pbmp=val> <HashValue=val>\n\t"
    "        - Set ingress port hash value to select egress port of a trunk\n\t"
    "  trunk hash get <Pbmp=val>\n\t"
    "        - Get ingress port programmable hash value\n\t"
#endif /* BCM_FIREBOLT_SUPPORT */
    "  trunk psc <Id=val> <Rtag=val>\n\t"
    "        - Change Rtag (for testing ONLY)\n"
#endif
    ;
extern cmd_result_t if_esw_trunk(int unit, args_t *a);

char cmd_esw_tx_usage[] =
    "Parameters: <Count> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  Transmit the specified number of packets, if the contents of the\n"
    "  packets is important, they may be specified using options.\n"
    "  Supported options are:\n"
    "      Untagged[yes/no]    - Specify packet should be sent untagged(XGS3)\n"
    "      TXUnit=<val>        - Transmit unit number\n"
    "      PortBitMap=<pbmp>   - Specify port bitmap packet is sent to.\n"
    "      UntagBitMap=<pbmp>  - Specify untag bitmap used for DMA.\n"
    "      File=<filename>     - Load hex packet data from file and ignore\n"
    "                            various other pattern parameters below.\n"
    "      DATA=<packet>       - Parse string given by user as packet payload\n"
    "      Length=<value>      - Specify the total length of the packet,\n"
    "                            including header, possible tag, and CRC.\n"
    "      VLantag=<value>     - Specify the VLAN tag used, only the low\n"
    "                            order 16-bits are used (VLANID=0 for none)\n"
    "      VlanPrio            - VLAN Priority.\n"
    "      PrioInt             - Internal Priority.\n"
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
    "      PurGe[yes/no]       - Send packet with purge Indication(XGS3)\n"
    "      McQType=[yes/no]    - Send packet to MC queue(XGS3)\n"
#if defined(BCM_TOMAHAWK_SUPPORT)
    "      VisibilitySourcePort=<port> - Source port for visibility trace\n"
    "                                    Select 'any' to disable\n"
    "      VisibilityOptions=<value>   - Visibility options\n"
    "                                  - 0x1 BCM_PKT_TRACE_LEARN\n"
    "                                  - 0x2 BCM_PKT_TRACE_NO_IFP\n"
    "                                  - 0x4 BCM_PKT_TRACE_FORWARD\n"
#endif
#if defined(BCM_CPU_TX_PROC_SUPPORT)
    "      TxProcMhDestinationType=<opcode>   - Case/Destination type\n"
    "                                         - Choose from VLAN_FLOOD, L2MC, IPMC, DGPP\n"
    "                                           ECMP, ECMP_MEMBER and NHI\n"
    "      TxProcMhSourcePort=<port>          - Packet Processing port used to gauge loopback port\n"
    "                                           to send the packet to\n"
    "      TxProcMhDestination=<value>        - Destination Index/Value\n"
    "      TxProcMhDestinationPort=<port>     - Destination port\n"
    "      TxProcMhEcmpGroup=<value>          - ECMP group index value\n"
    "      TxProcMhEcmpMember=<value>         - ECMP member index value\n"
    "      TxProcMhRoutedPkt[Yes/No]          - L3 Routed packet\n"
    "      TxProcMhMcastLbIndexValid[Yes/No]  - Load balancing for Non Unicast Packets\n"
    "      TxProcMhMcastLbIndex=<value>       - Select entry in NONUCAST_TRUNK_BLOCK_MASK table\n"
    "                                           Used to control load balancing for non unicast packets\n"
    "      TxProcMhQosFieldsValid[Yes/No]     - Select QoS Processing\n"
    "      TxProcMhDropPrecedence<color>      - Drop Precendence, choose packet color (green/red/yellow)\n"
    "      TxProcMhInternalPriority=<value>   - Internal Priority\n"
    "      TxProcMhInternalCongestion=<value> - Internal Congestion\n"
#endif /* BCM_CPU_TX_PROC_SUPPORT */
    "\n"
    "  Header Format-specific options (see HeaderMode command):\n"
    "  - HiGig:\n"
    "      HGOpcode=<val>      - HiGig header opcode\n"
    "      HGSrcMod=<val>      - Source Module ID\n"
    "      HGSrcPort=<val>     - Source Port (numeric value only)\n"
    "      HGDestMod=<val>     - Destination Module ID\n"
    "      HGDestPort=<val>    - Destination Port (numeric value only)\n"
    "      PFM=<val>           - Port Filtering Mode (0, 1, 2)\n"
#ifdef BCM_HIGIG2_SUPPORT
    "  - HiGig2 only:\n"
    "      TrafficClass=<val>  - Traffic Class (0-15)\n"
    "      McastGroupID=<val>  - Multicast Group ID\n"
    "      LoadBalID=<value>   - Load Balancing ID\n"
    "      DropPrecedence=<c>  - Drop Precendence (green/red/yellow)\n"
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    "  - SOBH options: TRIUMPH3 only\n"
    "      RplType=<val>       - OAM replacement type \n"
    "      RplOffset=<val>     - OAM replacement offset\n"
    "      LmCtrIdx=<val>      - LM counter Index\n"
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
    " RQE num=<val>\n"
#endif
#endif
    ;
extern cmd_result_t cmd_esw_tx(int unit, args_t *a);

#if defined(INCLUDE_PKTIO)
char cmd_pktio_txn_usage[] =
    "Parameters: <Count> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  Transmit the specified number of packets, if the contents of the\n"
    "  packets is important, they may be specified using options.\n"
    "  Supported options are:\n"
    "      Untagged[yes/no]    - Specify packet should be sent untagged.\n"
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
    "      McQType=[yes/no]    - Send packet to MC queue.\n"
#endif
    ;
extern cmd_result_t cmd_pktio_txn(int unit, args_t *a);

char cmd_pktio_pcktwatch_usage[] =
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
    "\tinterval # - # us to wait after a packet arrives before next\n"
    "\t        DMA starts (SOC mode only)\n"
    "\treport- count   - print packet # only\n"
    "\t        decode  - attempt to decode packet\n"
    "\t        raw     - dump raw byte stream (host order)\n"
    "\t        pmd     - dump rx metadata and reason\n"
    "\t        Prepend + to set, - to clear, nothing to toggle\n"
    "\tquiet - disable all reporting\n"
    "\tstart - start the PW-daemon\n"
    "\tstop  - stop the PW-daemon and deallocate log space\n"
    "\tcount - print # packets received.\n"
    "\treset - reinitialize configuration\n"
#endif
    ;
extern cmd_result_t cmd_pktio_pcktwatch(int unit, args_t *a);
#endif /* INCLUDE_PKTIO */

char cmd_esw_tx_count_usage[] =
    "Parameters: None\n\t"
    "Print current request count and set count values for an active\n\t"
    "TXSTART command.\n";

extern cmd_result_t cmd_esw_tx_count(int unit, args_t *a);
extern cmd_result_t cmd_esw_tx_start(int unit, args_t *a);


char cmd_esw_tx_stop_usage[] =
    "Parameters: None\n\t"
    "Terminate a background TXSTART command that is currently running.\n\t"
    "This command only requests termination, the background thread\n\t"
    "looks for termination requests BETWEEN sending packets.\n";
extern cmd_result_t cmd_esw_tx_stop(int unit, args_t *a);


char if_esw_vlan_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  vlan <option> [args...]\n"
#else
    "  vlan create <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>]\n\t"
    "                                       - Create a VLAN\n\t"
    "  vlan destroy <id>                    - Destroy a VLAN\n\t"
    "  vlan clear                           - Destroy all VLANs\n\t"
    "  vlan add <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>]\n\t"
    "                                       - Add port(s) to a VLAN\n\t"
    "  vlan remove <id> [PortBitMap=<pbmp>] - Remove ports from a VLAN\n\t"
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
    "  vlan MulticastFlood <id>  [Mode]     - Multicast flood setting\n\t"
    "  vlan show                            - Display all VLANs\n\t"
    "  vlan default [<id>]                  - Show or set the default VLAN\n\t"
    "  vlan protocol add PortBitMap=<pbmp> Frame=<N> Ether=<N>\n\t"
    "        VLan=<vlanid> Prio=<prio>\n\t"
    "  vlan protocol delete PortBitMap=<pbmp> Frame=<N> Ether=<N>\n\t"
    "  vlan protocol clear PortBitMap=<pbmp>\n\t"
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "  vlan policer set <id> POLICER=<policer>\n\t"
    "  vlan policer get <id>\n\t"
#endif
    "  vlan mac add MACaddress=<address> VLan=<vlanid> Prio=<prio>\n\t"
    "        Cng=<cng>\n\t"
    "  vlan mac delete MACaddress=<address>\n\t"
    "  vlan mac clear\n\t"
    "  vlan translate [On|Off]\n\t"
    "  vlan translate add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate show              - Shows all vlan translations \n\t"
    "  vlan translate delete Port=<port> OldVLan=<vlanid>\n\t"
    "  vlan translate clear\n\t"
    "  vlan translate egress add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate egress get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate egress show  - Shows all vlan egress translations \n\t"
    "  vlan translate egress delete Port=<port> OldVLan=<vlanid>\n\t"
    "  vlan translate egress clear\n\t"
    "  vlan translate dtag add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate dtag get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate dtag show     - Shows all double tagged vlans \n\t"
    "  vlan translate range add Port=<port> OldVLanHi=<vlanid> \n\t"
    "        OldVlanLo=<vlanid> NewVLan=<vlanid> Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate range get Port=<port> OldVLanHi=<vlanid>\n\t"
    "        OldVlanLo=<vlanid>\n\t"
    "  vlan translate range delete Port=<port> OldVLanHi=<vlanid>\n\t"
    "        OldVlanLo=<vlanid>\n\t"
    "  vlan translate range clear\n\t"
    "  vlan translate range show\n\t"
    "  vlan ip4 add IPaddr=<ip-address> NetMask=<ip-addr> VLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan ip4 delete IPaddr=<ip-address> NetMask=<ip-addr>\n\t"
    "  vlan ip4 clear\n\t"
    "  vlan ip6 add IPaddr=<ip-address> prefiX=<prefix> VLan=<vlanid>\n\t"
    "        Prio=<prio>\n\t"
    "  vlan ip6 delete IPaddr=<ip-address> prefiX=<prefix>\n\t"
    "  vlan ip6 clear\n\t"
    "  vlan control [<name> [<value>]]\n\t"
    "  vlan port [<port>|all [<name> [<value>]]]\n\t"
    "  vlan innertag <port> Priority=<priority> CFI=<cfi> VLan=<vlanid>\n\t"
    "  vlan <id> [<name>=<value>]         - Set/Get per VLAN property\n\t"
    "  vlan action port default add Port=<port> OuterVlan=<vlanid>\n\t"
    "        InnerVlan=<vlanid> Prio=<prio> DtOuter=<action>\n\t"
    "        DtOuterPrio=<action> DtInner=<action> DtInnerPrio=<action>\n\t"
    "        OtOuter=<action> OtOuterPrio=<action> OtInner=<action>\n\t"
    "        OtOuter=<action> ItOuter=<action> ItInner=<action>\n\t"
    "        ItInnerPrio=<action> UtOuter=<action> UtInner=<action>\n\t"
    "        - Add port default VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action port default get Port=<port>\n\t"
    "        - Get port default VLAN tag actions\n\t"
    "  vlan action port egress default add Port=<port> OuterVlan=<vlanid>\n\t"
    "        InnerVlan=<vlanid> Prio=<prio> DtOuter=<action>\n\t"
    "        DtOuterPrio=<action> DtInner=<action> DtInnerPrio=<action>\n\t"
    "        OtOuter=<action> OtOuterPrio=<action> OtInner=<action>\n\t"
    "        OtOuter=<action> ItOuter=<action> ItInner=<action>\n\t"
    "        ItInnerPrio=<action> UtOuter=<action> UtInner=<action>\n\t"
    "        - Add port egress VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action port egress default get Port=<port>\n\t"
    "        - Get port egress VLAN tag actions\n\t"
    "  vlan action mac add MACaddress=<address> OuterVlan=<vlanid>\n\t"
    "        InnerVlan=<vlanid> Prio=<prio> DtOuter=<action>\n\t"
    "        DtOuterPrio=<action> DtInner=<action> DtInnerPrio=<action>\n\t"
    "        OtOuter=<action> OtOuterPrio=<action> OtInner=<action>\n\t"
    "        OtOuter=<action> ItOuter=<action> ItInner=<action>\n\t"
    "        ItInnerPrio=<action> UtOuter=<action> UtInner=<action>\n\t"
    "        - Add MAC-based VLAN tag with actions\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action mac get MACaddress=<address>\n\t"
    "        - Get MAC-based VLAN tag actions\n\t"
    "  vlan action mac delete MACaddress=<address>\n\t"
    "        - Delete MAC-based VLAN tag actions\n\t"
    "  vlan action mac clear\n\t"
    "        - Delete all MAC-based VLAN tag actions\n\t"
    "  vlan action protocol add PortBitMap=<pbmp> Frame=<N> Ether=<N>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action> \n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action> \n\t"
    "        UtInner=<action>\n\t"
    "        - Add protocol-based VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action ip4 add IPaddr=<ip-address> NetMask=<ip-addr>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action> \n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action> \n\t"
    "        UtInner=<action>\n\t"
    "        - Add IP4-based VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action ip6 add IPaddr=<ip-address> prefiX=<prefix>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action> \n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action> \n\t"
    "        UtInner=<action>\n\t"
    "        - Add IP6-based VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action translate add Port=<port> KeyType=<key> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid> DtOuter=<action>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        UtInner=<action> Policer=<policer>\n\t"
    "        - Add VLAN tag translation with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "          Valid key types are { Double, Outer, Inner, OuterTag, \n\t"
    "          InnerTag, PortDouble, PortOuter, Policer }\n\t"
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
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        UtInner=<action>\n\t"
    "        - Add VLAN tag range translation with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action translate range delete Port=<port> \n\t"
    "        OuterVlanLo=<vlanid> OuterVlanHi=<vlanid> \n\t"
    "        InnerVlanLo=<vlanid> InnerVlanHi=<vlanid>\n\t"
    "        - Delete VLAN tag translation range actions.\n\t"
    "  vlan action translate range get Port=<port> \n\t"
    "        OuterVlanLo=<vlanid> OuterVlanHi=<vlanid> \n\t"
    "        InnerVlanLo=<vlanid> InnerVlanHi=<vlanid>\n\t"
    "        - Get VLAN tag translation range actions.\n\t"
    "  vlan action translate range clear\n\t"
    "        - Delete all VLAN tag translation range actions.\n\t"
    "  vlan action translate range show\n\t"
    "        - Show all VLAN tag translation range actions.\n\t"
    "  vlan action translate egress add PortClass=<class> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid> DtOuter=<action>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        UtInner=<action>\n\t"
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
extern cmd_result_t if_esw_vlan(int unit, args_t *a);


char cmd_esw_mem_write_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "If nocheck_mem_mode is specified, checking of unique_global mode, slice_mode will be skipped\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "Writes entry(s) into table index(es).\n";
extern cmd_result_t cmd_esw_mem_write(int unit, args_t *a);

#if defined (BCM_FIRELIGHT_SUPPORT)
char cmd_esw_coupled_mem_write_usage[] =
    "\n\tParameters: <table1> <table2> <entry_start> <count>\n\t"
    "            <table1_field>=<value>[,...]\n\t"
    "            <table2_field>=<value>[,...]\n\t"
    "Write entries to two coupled memories at the same time.\n\t"
    "Example: cmw ISEC_SP_TCAM_KEY ISEC_SP_TCAM_MASK 2 3\n\t"
    "             PORT_MAP=0x2,FRAME_FORMAT=0x2\n\t"
    "             VALID=1,PORT_MAP_MASK=0x3,FRAME_FORMAT_MASK=0x3\n";
extern cmd_result_t cmd_esw_coupled_mem_write(int unit, args_t *a);
#endif

char cmd_esw_eav_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\tcontrol set|get <ctrl_type> <parameter>\n"
"\tmac get|set <mac value>\n"
"\ttypes. (Describe the types of Ethernet AV CLI commands)\n"
"\tinit\n"
"\tport enable|disable <port number>\n"
"\twatch start|stop\n"
"\tstatus\n"
"\ttimestamp <port number>\n"
"\ttx <pbmp> <vlanid>\n"
"\tsrp set|get <mac value> <ethertype>\n"
"\tbandwidth set|get <port number> <class> <bandwidth> <burstsize>\n"
"\tpcp set|get <class> <pcp> <remapped_pcp>\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_esw_eav(int unit, args_t *a);

#ifdef INCLUDE_CES
char cmd_esw_ces_usage[] = 
    "\n"
    "\tces <option> [args....] - Manage and debug the CES subsystem\n"
    "\t             Options are:\n"
    "\t                         init - Initialize CES services\n"
    "\t                         diag - Configure loopbacks and view basic diagnostic information\n"
    "\t                         cclk - Manage common clcok configuration\n"
    "\t                      service - Manage CES services\n"
    "\t                          tdm - Manage TDM ports used for CES\n"
    "\t                          liu - Configure and debug LIU settings\n"
    "\t                          rpc - Configure and display RPC PM counters\n"
    "\t                         test - Run framer PRBS tests\n"
    "\t                         help - Display usage information for a specific option\n"
    "\n\t           To display usage information for a specific option use:\n"
    "\t                         ces help <option>\n\n"
;
extern cmd_result_t cmd_esw_ces(int unit, args_t *a);
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
#ifdef COMPILER_STRING_CONST_LIMIT
    char if_esw_asf_usage[] =
    "Usage: asf <option> [args...]\n";
#else
    char if_esw_asf_usage[] =
    "ASF Configurations Summary & Diagnostics.\n"
    "Parameters: [show | diag] <pbmp>\n"
    "Example: asf show [ce0 | hg1 | ce,hg]\n"
    "Example: asf diag [ce0 | hg1 | ce,hg]\n"
    "<pbmp> is a standard port bitmap (see \"help pbmp\").\n"
    "To configure ASF on port(s), use 'port <pbmp> asf <mode>' command\n";
#endif

#ifdef COMPILER_STRING_CONST_LIMIT
    char if_esw_latency_usage[] =
    "Usage: latency <option> \n";
#else
    char if_esw_latency_usage[] =
    "Switch Latency Bypass Configurations Summary & Diagnostics.\n"
    "Parameters: [show | diag]\n"
    "Example: latency show\n"
    "Example: latency diag\n"
    "Switch latency bypass mode is configurable only during initialization\n"
    "To configure switch latency bypass mode, use switch_bypass_mode = [0 | 1 | 2 ] in config file\n";
#endif
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT
#ifdef COMPILER_STRING_CONST_LIMIT
char cmd_if_flexport_usage[] =
    "Usage: FLEXport PM<id>[=<pm_mode>[:<port_numbers>]]\n";
#else
char cmd_if_flexport_usage[] =
    "FLEXport PM<id>[=<pm_mode>[:<port_numbers>]]\n"
    "Parameters:\n"
    "  PM<id>           - Port Macro to be flexed.\n"
    "  <pm_mode>        - Port Macro mode:\n"
    "                     Single Mode:\n"
    "                       1x100G | 1x40G | 1xHG[106] | 1xHG[42] |\n"
    "                       1xHG[100]| 1xHG[40]\n"
    "                     Dual Mode:\n"
    "                       2x50G | 2x40G | 2x25G | 2x20G |\n"
    "                       1x40G+1x20G | 1x20G+1x40G |\n"
    "                       1x25G+1x50G | 1x50G+1x25G |\n"
    "                       2xHG[53] | 2xHG[42] | 2xHG[27] | 2xHG[21] |\n"
    "                       1xHG[42]+1xHG[21] | 1xHG[21]+1xHG[42] |\n"
    "                       1xHG[27]+1xHG[53] | 1xHG[53]+1xHG[27] |\n"
    "                       2xHG[50] | 2xHG[40] | 2xHG[20] | 2xHG[25] |\n"
    "                       1xHG[25]+1xHG[50] | 1xHG[50]+1xHG[25] |\n"
    "                       1xHG[40]+1xHG[20] | 1xHG[20]+1xHG[40]\n"
    "                     Tri Mode:\n"
    "                       1x50G+2x25G | 2x25G+1x50G |\n"
    "                       2x10G+1x40G | 1x40G+2x10G |\n"
    "                       2x10G+1x20G | 1x20G+2x10G |\n"
    "                       1xHG[53]+2xHG[27] | 2xHG[27]+1xHG[53] |\n"
    "                       2xHG[11]+1xHG[42] | 1xHG[42]+2xHG[11] |\n"
    "                       2xHG[11]+1xHG[21] | 1xHG[21]+2xHG[11] |\n"
    "                       1xHG[50]+2xHG[25] | 2xHG[25]+1xHG[50] |\n"
    "                       2xHG[10]+1xHG[40] | 1xHG[40]+2xHG[10] |\n"
    "                       2xHG[10]+1xHG[20] | 1xHG[20]+2xHG[10]\n"
    "                     Quad Mode\n"
    "                       4x25G | 4x10G | 4xHG[27] | 4xHG[11] |\n"
    "                       4xHG[25] | 4xHG[10] | 4x2.5G | 4x1G\n"
    "                     Clear ports on a Port Macro:\n"
    "                       None\n"
    "                     Note: All the other modes not listed above are not\n"
    "                           supported on BCM Shell, even they may be\n"
    "                           supported via API configuration.\n"
    "  [<port_numbers>] - New port numbers on a specific Port Macro.\n"
    "                     If no port_number configured, new port numbers will\n"
    "                     be picked from un-used ports.\n"
    "Example: flexport pm0=1x40G:1 pm1=4x25G:2-5\n"
    "Example: flexport pm0=none pm2=2x50g\n"
    "Flex Port Macro(s) into a specific mode.\n"
    "Remove all ports in a Port Macro if the <pm_mode> is configured as None.\n"
    "If only PM<id> is specified, characteristics for that Port Macro is displayed.\n";
#endif
#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */
#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT
#ifdef COMPILER_STRING_CONST_LIMIT
char cmd_if_flexport_usage_td3[] =
    "Usage: FLEXport PM<id>[=<pm_mode>]\n";
#else
char cmd_if_flexport_usage_td3[] =
    "FLEXport PM<id>[=<pm_mode>]\n"
    "Parameters:\n"
    "  PM<id>           - Port Macro to be flexed.\n"
    "  <pm_mode>        - Port Macro mode:\n"
    "                     Single Mode:\n"
    "                       1x100G | 1x40G | 1xHG[106] | 1xHG[42] |\n"
    "                       1xHG[100]| 1xHG[40]\n"
    "                     Dual Mode:\n"
    "                       2x50G | 2x40G | 2x20G |\n"
    "                       1x40G+1x20G | 1x20G+1x40G |\n"
    "                       2xHG[53] | 2xHG[42] | 2xHG[21] |\n"
    "                       1xHG[42]+1xHG[21] | 1xHG[21]+1xHG[42] |\n"
    "                       2xHG[50] | 2xHG[40] | 2xHG[20] |\n"
    "                       1xHG[40]+1xHG[20] | 1xHG[20]+1xHG[40]\n"
    "                     Tri Mode:\n"
    "                       1x50G+2x25G | 2x25G+1x50G |\n"
    "                       2x10G+1x40G | 1x40G+2x10G |\n"
    "                       2x10G+1x20G | 1x20G+2x10G |\n"
    "                       1xHG[53]+2xHG[27] | 2xHG[27]+1xHG[53] |\n"
    "                       2xHG[11]+1xHG[42] | 1xHG[42]+2xHG[11] |\n"
    "                       2xHG[11]+1xHG[21] | 1xHG[21]+2xHG[11] |\n"
    "                       1xHG[50]+2xHG[25] | 2xHG[25]+1xHG[50] |\n"
    "                       2xHG[10]+1xHG[40] | 1xHG[40]+2xHG[10] |\n"
    "                       2xHG[10]+1xHG[20] | 1xHG[20]+2xHG[10]\n"
    "                     Quad Mode\n"
    "                       4x25G | 4x10G | 4xHG[27] | 4xHG[11] |\n"
    "                       4xHG[25] | 4xHG[10] | 4x2.5G | 4x1G\n"
    "                     Clear ports on a Port Macro:\n"
    "                       None\n"
    "                     Note: All the other modes not listed above are not\n"
    "                           supported on BCM Shell, even they may be\n"
    "                           supported via API configuration.\n"
    "Example: flexport pm0=1x40G pm1=4x25G\n"
    "Example: flexport pm0=none pm2=2x50g\n"
    "Flex Port Macro(s) into a specific mode.\n"
    "Remove all ports in a Port Macro if the <pm_mode> is configured as None.\n"
    "If only PM<id> is specified, characteristics for that Port Macro is displayed.\n";
#endif
#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */
#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT
#ifdef COMPILER_STRING_CONST_LIMIT
char cmd_if_flexport_usage_th3[] =
    "Usage: FLEXport PM<id>[=<pm_mode>]\n";
#else
char cmd_if_flexport_usage_th3[] =
    "FLEXport PM<id>[=<pm_mode>]\n"
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
#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */

#ifdef INCLUDE_PKTIO
char cmd_esw_pktio_usage[] =
    "pktio [on|off]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\ton   - Enable TX/PacketWatcher command with streamlined PKTIO driver type.\n"
    "\toff  - Disable TX/PacketWatcher command with streamlined PKTIO driver type.\n"
    "NOTE: This command is used to switched different command parameter set according to PKTIO driver type.\n"
    "      User should deploy spn_PKTIO_driver_type config to specify driver type. \n"
#endif
    ;
#else
char cmd_esw_pktio_usage[] =
    "Available when PKTIO feature is enabled.\n";
#endif /* INCLUDE_PKTIO */
