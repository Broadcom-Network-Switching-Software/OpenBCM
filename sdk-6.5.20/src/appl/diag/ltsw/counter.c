/*! \file counter.c
 *
 * LTSW Counter command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(BCM_LTSW_SUPPORT)

#include <bcm/cosq.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <appl/diag/system.h>
#include <shared/bsl.h>
#include <shr/shr_error.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/stat.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_client.h>

/******************************************************************************
* Local definitions
 */

/*!Custom stats definitions. */
#define CSTAT_COL_SZ         0x05

/*!Custom stats counter types. */
#define CSTAT_RDBGC          0x01
#define CSTAT_TDBGC          0x02
#define CSTAT_BOTH           (CSTAT_RDBGC | CSTAT_TDBGC)

/*!Custom stats command types. */
#define CSTAT_SET_FLAGS      0x01
#define CSTAT_LIST_FLAGS     0x02
#define CSTAT_SHOW_FLAGS     0x03
#define CSTAT_SHOW_COUNTERS  0x04
#define CSTAT_SHOW_TABLE     0x05

/*!Custom stats trigger actions. */
#define CSTAT_TRIGGER_ADD    0x01
#define CSTAT_TRIGGER_DEL    0x02
#define CSTAT_TRIGGER_TOGGLE 0x05

#define CSTAT_ERROR_SET_FLAGS(_err, cmd_type) \
        if (cmd_type == CSTAT_SET_FLAGS) {    \
            cli_out("Error: " _err);          \
            return CMD_FAIL;                  \
        }

/*! Event stats command types. */
 typedef enum estat_cmd_e {
    ESTAT_CMD_INVALID = -1,
    ESTAT_CMD_SHOW,
    ESTAT_CMD_CLEAR
} estat_cmd_t;

/*! Event stats directions. */
 typedef enum estat_dir_e {
    ESTAT_DIR_INVALID = -1,
    ESTAT_DIR_INGRESS,
    ESTAT_DIR_EGRESS,
    ESTAT_DIR_BOTH
} estat_dir_t;

/*! Event stats event types. */
 typedef enum estat_type_e {
    ESTAT_TYPE_INVALID = -1,
    ESTAT_TYPE_DROP,
    ESTAT_TYPE_TRACE
} estat_type_t;

/******************************************************************************
 * Private functions
 */

/*! \brief Counter collection configuration structure.
 *
 * This structure defines counter collection configuration parameters.
 */
typedef struct ltsw_counter_s {

    /*! Enable counter collection. */
    uint64_t enable;

    /*! Base time interval for collection in microseconds. */
    uint64_t usec;

    /*! Port bitmap on which counters are collected. */
    bcm_pbmp_t pbmp;

} ltsw_counter_t;

static uint64_t *counter_val[BCM_MAX_NUM_UNITS];

/*!
 * \brief Custom stats display tabular format.
 */
static char *cstat_head =
    "\n+------------------Programmable Statistics Counters[Port %4s]-------\
-----------+";
static char *cstat_menu =
    "\n| Type | No. |       Value       |                Enabled For       \
            |";
static char *cstat_line =
    "\n+-------------------------------------------------------------------\
------------+";
static char *cstat_trigger_s = "\n|      |     |                   | ";

static char *cstat_stat_names[] = BCM_STAT_NAME_INITIALIZER;

/*!
 * \brief Custom stats trigger names structure.
 */
typedef struct ltsw_cstat_trigger_name_s {
    char *name;
    char *about;
} ltsw_cstat_trigger_name_t;

ltsw_cstat_trigger_name_t cstat_trigger_names[] = {
    {"RIPD4", "Rx IPv4 L3 discard packets"},
    {"RIPC4", "Good rx L3 (V4 packets) includes tunneled"},
    {"RIPHE4", "Rx IPv4 header error packets"},
    {"IMRP4", "Routed IPv4 multicast packets"},
    {"RIPD6", "Rx IPv6 L3 discard packet"},
    {"RIPC6", "Good rx L3 (V6 packets) includes tunneled"},
    {"RIPHE6", "Rx IPv6 header error packets"},
    {"IMRP6", "Routed IPv6 multicast packets"},
    {"RDISC", "IBP discard and CBP full"},
    {"RUC", "Good rx unicast (L2+L3) packets"},
    {"RPORTD", "Packets droppped when ingress port is not in forwarding \
state"},
    {"PDISC", "Rx policy discard - DST_DISCARD, SRC_DISCARD, RATE_CONTROL"},
    {"IMBP", "Bridged multicast packets"},
    {"RFILDR", "Packets dropped by FP"},
    {"RIMDR", "Multicast (L2+L3) packets that are dropped"},
    {"RDROP", "Port bitmap zero drop condition"},
    {"IRPSE", "HiGig IPIC pause rx"},
    {"IRHOL", "HiGig End-to-End HOL rx packets"},
    {"IRIBP", "HiGig End-to-End IBP rx packets"},
    {"DSL3HE", "DOS L3 header error packets"},
    {"IUNKHDR", "Unknown HiGig header type packet"},
    {"DSL4HE", "DOS L4 header error packets"},
    {"IMIRROR", "HiGig mirror packet"},
    {"DSICMP", "DOS ICMP error packets"},
    {"DSFRAG", "DOS fragment error packets"},
    {"MTUERR", "Packets trapped to CPU due to egress L3 MTU violation"},
    {"RTUN", "Number of tunnel packets received"},
    {"RTUNE", "Rx tunnel error packets"},
    {"VLANDR", "Rx VLAN drops"},
    {"RHGUC", "Rx HiGig lookup UC cases"},
    {"RHGMC", "Rx HiGig lookup MC cases"},
    {"MPLS", "Received MPLS Packets"},
    {"MACLMT", "packets dropped due to MAC Limit is hit"},
    {"MPLSERR", "Received MPLS Packets with Error"},
    {"URPFERR", "L3 src URPF check Fail"},
    {"HGHDRE", "Higig Header error packets"},
    {"MCIDXE", "Multicast Index error packets"},
    {"LAGLUP", "LAG failover loopback packets"},
    {"LAGLUPD", "LAG backup port down"},
    {"PARITYD", "Parity error packets"},
    {"VFPDR", "VLAN FP drop case"},
    {"URPF", "Unicast RPF drop case"},
    {"DSTDISCARDROP", "L2/L3 lookup DST_DISCARD drop"},
    {"CLASSBASEDMOVEDROP", "Class based station movement drop"},
    {"MACLMT_NODROP", "Mac limit exceeded and packet not dropped"},
    {"MACSAEQUALMACDA", "Dos Attack L2 Packets"},
    {"MACLMT_DROP", "Mac limit exceeded and packet dropped"},
    {"TGIP4", "Tx Good IPv4 L3 UC packets"},
    {"TIPD4", "Tx IPv4 L3 UC Aged and Drop packets"},
    {"TGIPMC4", "Tx Good IPv4 IPMC packets"},
    {"TIPMCD4", "Tx IPv4 IPMC Aged and Drop packets"},
    {"TGIP6", "Tx Good IPv6 L3 UC packets"},
    {"TIPD6", "Tx IPv6 L3 UC Aged and Drop Packets"},
    {"TGIPMC6", "Tx Good IPv6 IPMC packets"},
    {"TIPMCD6", "Tx IPv6 IPMC Aged and Drop packets"},
    {"TTNL", "Tx Tunnel packets"},
    {"TTNLE", "Tx Tunnel error packets"},
    {"TTTLD", "Pkts dropped due to TTL threshold counter"},
    {"TCFID", "Pkts dropped when CFI set & pkt is untagged or L3 switched for \
IPMC"},
    {"TVLAN", "Tx VLAN tagged packets"},
    {"TVLAND", "Pkts dropped due to invalid VLAN counter"},
    {"TVXLTMD", "Pkts dropped due to miss in VXLT table counter"},
    {"TSTGD", "Pkts dropped due to Spanning Tree State not in forwarding \
state"},
    {"TAGED", "Pkts dropped due to packet aged counter"},
    {"TL2MCD", "L2 MC packet drop counter"},
    {"TPKTD", "Pkts dropped due to any condition"},
    {"TMIRR", "mirroring flag"},
    {"TSIPL", "SIP Link Local Drop flag"},
    {"THGUC", "Higig Lookedup L3UC Pkts"},
    {"THGMC", "Higig Lookedup L3MC Pkts"},
    {"THIGIG2", "Unknown HiGig2 Drop"},
    {"THGI", "Unknown HiGig drop"},
    {"TL2_MTU", "L2 MTU fail drop"},
    {"TPARITY_ERR", "Parity Error drop"},
    {"TIP_LEN_FAIL", "IP Length check fail drop"},
    {"TMTUD", "EBAD_MTU_DROP"},
    {"TSLLD", "ESIP_LINK_LOCAL"},
    {"TL2MPLS", "L2_MPLS_ENCAP_TX"},
    {"TL3MPLS", "L3_MPLS_ENCAP_TX"},
    {"TMPLS", "MPLS_TX"},
    {"MODIDTOOLARGEDROP", "MODID greater than 31 drop counter"},
    {"PKTMODTOOLARGEDROP", "Byte additions too large drop counter"},
    {"RX_SYMBOL", "Fabric I/F => Rx Errors (e.g.8B10B)"},
    {"TIME_ALIGNMENT_EVEN", "Fabric I/F => Time Alignment (even)"},
    {"TIME_ALIGNMENT_ODD", "Fabric I/F => Time Alignment (odd)"},
    {"BIT_INTERLEAVED_PARITY_EVEN", "Fabric I/F => BIP (even)"},
    {"BIT_INTERLEAVED_PARITY_ODD", "Fabric I/F => BIP (odd)"},
    {"FcmPortClass3RxFrames", "FCOE L3 RX frames"},
    {"FcmPortClass3RxDiscards", "FCOE L3 RX discarded frames "},
    {"FcmPortClass3TxFrames", "FCOE L3 TX frames "},
    {"FcmPortClass2RxFrames", "FCOE L2 RX frames "},
    {"FcmPortClass2RxDiscards", "FCOE L2 RX discarded frames "},
    {"FcmPortClass2TxFrames", "FCOE L2 TX frames "},
    {"NonHGoE", "Non HGoE Rx frames on an HGoE port"},
    {"HGoE", "HGoE Rx frames"},
    {"PROTSWITCHINGDROP", "Pkts Dropped due to Protection switching in EPIPE"},
    {"VNTAG_ERROR", "Rx VNTAG error packets"},
    {"NIV_FORWARDING_DROP", "Rx NIV/PE packets dropped due to forwarding \
error"},
    {"IFP_PFC_DROP", "Original ingress packets dropped due to IFP PfcTx \
action"},
    {"TECN_TABLE_DROP", "Tx Egress packets dropped due to ECN error"},
    {"TNIV_PRUNE_DROP", "Tx Egress NIV/PE packets dropped due to pruning \
check"},
    {"RxHigigLoopbackDrop", "HiGig loopback packets dropped"},
    {"RxHigigUnknownOpcodeDrop", "HiGig unknown opcode"},
    {"RxIpInIpDrop", "IPinIP tunnel process failure"},
    {"RxMimDrop", "MiM tunnel process failure"},
    {"RxTunnelInterfaceError", "IPMC tunnel incoming interface is incorrect"},
    {"RxMplsControlWordInvalid", "MPLS invalid control word"},
    {"RxMplsGalNotBosDrop", "MPLS Generic Label is not BOS"},
    {"RxMplsPayloadInvalid", "MPLS invalid payload"},
    {"RxMplsEntropyDrop", "MPLS Entropy label in unallowed range"},
    {"RxMplsLabelLookupMiss", "MPLS label lookup miss"},
    {"RxMplsActionInvalid", "MPLS invalid action"},
    {"RxTunnelTtlError", "Tunnel TTL check failure"},
    {"RxTunnelShimError", "Tunnel shim header error"},
    {"RxTunnelObjectValidationFail", "Tunnel termination failure"},
    {"RxVlanMismatch", "VLAN ID invalid"},
    {"RxVlanMemberMismatch", "Ingress port not in VLAN membership"},
    {"RxTpidMismatch", "VLAN TPID mismatch"},
    {"RxPrivateVlanMismatch", "PVLAN VID mismatch"},
    {"RxMacIpBindError", "MAC to IP bind check failure"},
    {"RxVlanTranslate2LookupMiss", "VLAN Translate2 lookup miss"},
    {"RxL3TunnelLookupMiss", "L3 Tunnel lookup miss"},
    {"RxMplsLookupMiss", "MPLS lookup miss"},
    {"RxVlanTranslate1LookupMiss", "VLAN Translate1 lookup miss"},
    {"RxFcoeVftDrop", "FVT header error"},
    {"RxFcoeSrcBindError", "FCOE source bind check failure"},
    {"RxFcoeSrcFpmaError", "Source FPMA prefix check failure"},
    {"RxVfiP2pDrop", "Packets arrived from a VP that is not programmed in \
PT2PT connection"},
    {"RxStgDrop", "STP not in forwarding state"},
    {"RxCompositeError", "HW related error like lookup table with parity \
error"},
    {"RxBpduDrop", "BPUD packets dropped"},
    {"RxProtocolDrop", "Protocol packets dropped"},
    {"RxUnknownMacSaDrop", "Unknown MACSA packets dropped "},
    {"RxSrcRouteDrop", "MACSA is multicast(bit 40 == 1)"},
    {"RxL2SrcDiscardDrop", "L2 SRC_DISCARD drop"},
    {"RxL2SrcStaticMoveDrop", "Port movement of static L2 addr"},
    {"RxL2DstDiscardDrop", "L2 DST_DISCARD drop"},
    {"RxL3DisableDrop", "V4/V6 L3_ENABLE unset drop"},
    {"RxMacSaEqual0Drop", "Packets dropped due to MACSA == 0"},
    {"RxVlanCrossConnectOrNonUnicastDrop", "L2 forwarding lookup miss or PBT \
non unicast packets drop"},
    {"RxTimeSyncDrop", "Network time sync packets dropped"},
    {"RxIpmcDrop", "IPMC process failure"},
    {"RxMyStationDrop", "MY_STATION.DISCARD drop"},
    {"RxPrivateVlanVpFilterDrop", "Packets dropped based on src/dst VP type"},
    {"RxL3DstDiscardDrop", "L3 DST_DISCARD drop"},
    {"RxTunnelDecapEcnError", "Tunnel decap ECN error"},
    {"RxL3SrcDiscardDrop", "L3 SRC_DISCARD drop"},
    {"RxFcoeZoneLookupMiss", "FCOE_ZONE lookup miss"},
    {"RxL3TtlError", "L3 TTL check failure"},
    {"RxL3HeaderError", "L3 header error"},
    {"RxL2HeaderError", "L2 header error"},
    {"RxL3DstLookupDrop", "IPMC processing drop, or L3_DESTINATION == 0 \
for L3UC packets"},
    {"RxL2TtlError", "L2 ZONE TTL check failure"},
    {"RxL2RpfError", "Incoming port/lag/svp check failure"},
    {"RxTagUntagDiscardDrop", "PORT_DIS_UNTAG/PORT_DIS_TAG drop"},
    {"RxStormControlDrop", "Storm control metering drop"},
    {"RxFcoeZoneError", "FCOE_ZONE check failure"},
    {"RxFieldChangeL2NoRedirectDrop", "Change L2 fields without deploying \
redirect action drop"},
    {"RxNextHopDrop", "NEXT_HOP drop"},
    {"RxNatDrop", "NAT process failure"},
    {"IngressProtectionDataDrop", "Protection data drop"},
    {"RxSrcKnockoutDrop", "SGPP == DGPP or SVP == DVP, etc"},
    {"RxMplsSeqNumberError", "MPLS control word sequence number error"},
    {"RxBlockMaskDrop", "Bitmap of ports blocked"},
    {"RxDlbRhResolutionError", "DLB or RH resolution error"},
    {"TxFwdDomainNotFoundDrop", "Forwarding domain not found"},
    {"TxNotFwdDomainMemberDrop", "Not forwarding domain member"},
    {"TxIpmcL2SrcPortPruneDrop", "IPMC L2 self port drop"},
    {"TxNonUnicastPruneDrop", "Non unicast pruning"},
    {"TxSvpRemoveDrop", "Virtual port pruning"},
    {"TxVplagPruneDrop", "VPLAG pruning"},
    {"TxSplitHorizonDrop", "Split horizon check failure"},
    {"TxMmuPurgeDrop", "Packets dropped due to MMU purge"},
    {"TxStgDisableDrop", "Packets dropped due to STG disabled"},
    {"TxEgressPortDrop", "EGR_LPORT_PROFILE.DROP"},
    {"TxEgressFilterDrop", "Packets dropped by EFP"},
    {"TxVisibilityDrop", "Visibility packets dropped"},
    {"RxMacControlDrop", "Packets dropped due to MAC control frame"},
    {"RxProtocolErrorIP4", "Packets dropped due to IPv4 protocol error"},
    {"RxProtocolErrorIP6", "Packets dropped due to IPv6 protocol error"},
    {"RxLAGFailLoopback", "LAG fail loopback packets in packet discard stage"},
    {"RxEcmpResolutionError", "Packets dropped due to ECMP resolution error"},
    {"RxPfmDrop", "Packets dropped due to PFM"},
    {"RxTunnelError", "Packets dropped due to tunnel errors"},
    {"RxBFDUnknownAchError", "BFD packets with non-zero version in the ACH \
header or the ACH channel type is unknown"},
    {"RxBFDUnknownControlFrames", "Unrecognized BFD control packets received \
from the PW VCCM type 1/2/3 or MPLS-TP control channel"},
    {"RxBFDUnknownVersionDiscards", "Unknown BFD version or discard dropped"},
    {"RxDSL2HE", "Dos Attack L2 Packets"},
    {"RxEgressMaskDrop", "Packets dropped due to egress mask"},
    {"RxFieldRedirectMaskDrop", "Packets dropped due to FP redirection mask"},
    {"RxFieldDelayDrop", "Packets dropped due to ingress FP delayed action"},
    {"RxEgressMaskSrcPortDrop", "Packets dropped due to block traffic from \
egress based on the ingress port"},
    {"RxMacBlockMaskDrop", "Packets dropped due to MAC block mask"},
    {"RxMCError", "Packets dropped due to multicast errors"},
    {"RxNonUnicastMaskDrop", "Packets dropped due to unknown unicast, \
unknown multicast, known multicast, and broadcast block masks"},
    {"RxNonUnicastLAGMaskDrop", "Packets dropped due to multicast and \
broadcast trunk block mask"},
    {"RxStgMaskDrop", "Packets dropped due to spanning tree group state"},
    {"RxVlanBlockMaskDrop", "Packets dropped due to VLAN blocked ports"},
    {"RxEgressVlanMemberMismatch", "Packets dropped due to mismatched egress \
port for the VLAN"},
    {"TxCellTooSmall", "Multi-cell packets whose SOP cell size is smaller \
than 64 bytes after decap"},
    {"TxLoopbackToLoopbackDrop", "Packets dropped due to loopback packet \
destination is also loopback port"},
    {"RxINTDataplaneProbeDrop", "INT (Inband Network Telemetry) dataplane \
probe packets dropped due to exception, including turn around and hop limit \
exceeded"},
    {"RxINTVectorMismatch", "INT (Inband Network Telemetry) packets dropped \
due to request vector match failure"},
    {"RxVfiAdaptLookupMiss", "Packets dropped due to the first lookup miss \
drop or both the first and the second lookup miss drop during ingress VFI \
adaption"},
    {"RxNextHopOrL3EifRangeError", "Packets dropped due to overlay and \
underlay next hop or L3 egress interface ID range error"},
    {"RxVxlanDecapSrcIpLookupMiss", "VXLAN packet dropped due to source IP \
lookup miss during tunnel decapsulation"}
};

static char *
ltsw_strncpy(char *dest, const char *src, size_t cnt)
{
    char *ptr = dest;

    while (*src && (*src !=  '.') && (*src !=  'r') && (cnt > 0) ) {
        *ptr++ = *src++;
        cnt--;
    }

    while (cnt > 0) {
        *ptr++ = 0;
        cnt--;
    }

    return dest;
}

static void
ltsw_counter_val_set(int unit, int port, int entries_perport, int base_index,
                     int offset, uint64_t val)
{
    int rv;
    int index;
    int alloc_size;
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        return;
    }

    alloc_size = ltsw_ctr_ctrl.total_counters * sizeof (uint64_t);
    if (counter_val[unit] == NULL) {
        counter_val[unit] = sal_alloc(alloc_size, "bcmDiagLtswCounterVal");
        if (counter_val[unit] == NULL) {
            return;
        }
        sal_memset(counter_val[unit], 0, alloc_size);
    }

    if (offset < 0) {
        for (offset = 0; offset < entries_perport; offset++) {
            ltsw_counter_val_set(unit, port, entries_perport, base_index,
                                 offset, val);
        }
        return;
    }

    index = base_index + port * entries_perport + offset;
    if (index >= 0 && index < ltsw_ctr_ctrl.total_counters) {
        counter_val[unit][index] = val;
    }
}

static void
ltsw_counter_val_get(int unit, int port, int entries_perport, int base_index,
                     int offset, uint64_t *val)
{
    int rv;
    int index;
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        return;
    }

    if (counter_val[unit] == NULL) {
        *val = 0;
    } else {
        index = base_index + port * entries_perport + offset;
        if (index >= 0 && index < ltsw_ctr_ctrl.total_counters) {
            *val = counter_val[unit][index];
        }
    }
}

static void
ltsw_counter_display(int unit, int port, const char *cname, uint64_t val,
                     uint64_t diff, bool changed, bool error_cnt, int flags)
{
    char line[120];
    char buf_cname[40];
    char buf_val[32];
    char buf_diff[32];

    if ((( changed && (flags & SHOW_CTR_CHANGED)) ||
          (!changed && (flags & SHOW_CTR_SAME))) &&
         (( val == 0 && (flags & SHOW_CTR_Z)) ||
           (val !=0 && (flags & SHOW_CTR_NZ))) &&
         (((!(flags & SHOW_CTR_ED)) ||
          (error_cnt && (flags & SHOW_CTR_ED))))) {
        sal_sprintf(buf_cname, "%s.%s", cname, bcmi_ltsw_port_name(unit, port));
        format_uint64_decimal(buf_val, val, ',');
        buf_diff[0] = '+';
        format_uint64_decimal(buf_diff + 1, diff, ',');
        sal_sprintf(line, "%-38s:%22s%20s", buf_cname, buf_val, buf_diff);
        cli_out("%s\n", line);
    }
}

static int
counter_control_parse_args(int unit, args_t *a, int *usec, bcm_pbmp_t *pbmp)
{
    parse_table_t pt;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_DFL|PQ_INT, (void *)( 0), usec, NULL);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM, (void *)(0),
                    pbmp, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return -1;
    }
    parse_arg_eq_done(&pt);

    return 0;
}

static void
counter_control_print_status(int unit, ltsw_counter_t cfg)
{
    char pbmp_str[FORMAT_PBMP_MAX];
    char pfmt[SOC_PBMP_FMT_LEN];

    sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));
    sal_memset(pfmt, 0, (sizeof(char) * SOC_PBMP_FMT_LEN));
    format_pbmp(unit, pbmp_str, sizeof(pbmp_str), cfg.pbmp);
    if (cfg.enable) {
        cli_out("    Interval=%u\n", (uint32_t)cfg.usec);
    } else {
        cli_out("    Interval=0\n");
    }
    cli_out("    PortBitMap=%s %s\n", SOC_PBMP_FMT(cfg.pbmp, pfmt), pbmp_str);

}

static int
counter_control_get(int unit, ltsw_counter_t *cfg)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_field_def_t field_def;
    uint32_t num_array;
    int port;
    uint64_t port_id[BCM_PBMP_PORT_MAX] = {0};

    rv = bcmlt_entry_allocate(dunit, CTR_CONTROLs, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_get(entry_hdl, COLLECTION_ENABLEs, &cfg->enable);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_get(entry_hdl, INTERVALs, &cfg->usec);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmi_lt_field_def_get(unit, CTR_CONTROLs, PORT_IDs, &field_def);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_array_get(entry_hdl, PORT_IDs, 0, port_id,
                                     field_def.elements, &num_array);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    BCM_PBMP_CLEAR(cfg->pbmp);
    for (port = 0; port < BCM_PBMP_PORT_MAX; port++) {
        if (port_id[port] > 0) {
            BCM_PBMP_PORT_ADD(cfg->pbmp, port);
        }
    }

    rv = bcmlt_entry_free(entry_hdl);
    return rv;

error:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    return rv;
}

static int
counter_control_update(int unit, ltsw_counter_t cfg)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int port;
    uint64_t port_id[BCM_PBMP_PORT_MAX] = {0};
    bcmlt_field_def_t field_def;

    rv = bcmlt_entry_allocate(dunit, CTR_CONTROLs, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_add(entry_hdl, COLLECTION_ENABLEs, cfg.enable);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    BCM_PBMP_ITER(cfg.pbmp, port) {
        port_id[port] = 1;
    }
    rv = bcmi_lt_field_def_get(unit, CTR_CONTROLs, PORT_IDs, &field_def);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    rv = bcmlt_entry_field_array_add(entry_hdl, PORT_IDs, 0, port_id,
                                     field_def.elements);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_add(entry_hdl, INTERVALs, cfg.usec);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_free(entry_hdl);
    return rv;

error:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    return rv;
}

static int
ltsw_mac_counter_show(int unit, const char *table, int port, int flags)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_field_def_t *table_fields = NULL;
    bcmlrd_pt_map_info_t *pt_maps = NULL;
    uint32_t idx, num_fields;
    char cname[32];
    uint32_t alloc_size, base_index, entries_perport;
    uint32_t num_entries;
    uint64_t field_val, prev_val, diff;
    bool changed = FALSE, error_cnt = FALSE;
    uint32_t table_id, field_id;
    const char *pt_name = NULL;
    uint32_t ret_entries;

    if (!(bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_PORT) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT))) {
        return 0;
    }

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    if(table == CTR_MACs) {
        base_index = ltsw_ctr_ctrl.base_index0;
        entries_perport = ltsw_ctr_ctrl.entries_perport0;
    } else if (table == CTR_MAC_ERRs) {
        error_cnt = TRUE;
        base_index = ltsw_ctr_ctrl.base_index1;
        entries_perport = ltsw_ctr_ctrl.entries_perport1;
    } else if (table == CTR_L3s) {
        error_cnt = TRUE;
        base_index = ltsw_ctr_ctrl.l3_base_index;
        entries_perport = ltsw_ctr_ctrl.l3_entries_perport;
    } else if (table == CTR_ECNs) {
        error_cnt = TRUE;
        base_index = ltsw_ctr_ctrl.ecn_base_index;
        entries_perport = ltsw_ctr_ctrl.ecn_entries_perport;
    } else if (table == CTR_ING_DEBUGs) {
        error_cnt = TRUE;
        base_index = ltsw_ctr_ctrl.rx_dbg_base_index;
        entries_perport = ltsw_ctr_ctrl.rx_dbg_entries_perport;
    } else if (table == CTR_EGR_DEBUGs) {
        error_cnt = TRUE;
        base_index = ltsw_ctr_ctrl.tx_dbg_base_index;
        entries_perport = ltsw_ctr_ctrl.tx_dbg_entries_perport;
    } else if (table == CTR_MIRROR_ING_PORT_SFLOWs) {
        error_cnt = TRUE;
        base_index = ltsw_ctr_ctrl.sflow_base_index;
        entries_perport = ltsw_ctr_ctrl.sflow_entries_perport;
    } else {
        rv = SHR_E_PARAM;
        goto error;
    }

    /* Get the default LT fields info. */
    rv = bcmlt_table_field_defs_get(dunit, table, 0, NULL, &num_fields);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    alloc_size = sizeof(bcmlt_field_def_t) * num_fields;
    table_fields = sal_alloc(alloc_size, "bcmDiagLtswCounterFieldInfo");
    if (table_fields == NULL) {
        rv = SHR_E_MEMORY;
        goto error;
    }
    sal_memset(table_fields, 0, alloc_size);
    rv = bcmlt_table_field_defs_get(dunit, table, num_fields, table_fields,
                                    &num_fields);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_add(entry_hdl, PORT_IDs, port);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    for (idx = 0; idx < num_fields; idx++) {
        if (table_fields[idx].key || table_fields[idx].symbol) {
            continue;
        }
        rv = bcmlt_entry_field_get(entry_hdl, table_fields[idx].name, &field_val);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        ltsw_counter_val_get(unit, port, entries_perport, base_index, idx,
                             &prev_val);
        if (prev_val > field_val) {
            prev_val = 0;
        }
        diff = field_val - prev_val;
        if (diff > 0) {
            changed = TRUE;
            ltsw_counter_val_set(unit, port, entries_perport, base_index, idx,
                                 field_val);
        } else {
            changed = FALSE;
        }

        /* Get the PT field mapping info. */
        rv = bcmlt_table_field_id_get_by_name(unit, table, table_fields[idx].name,
                                              &table_id, &field_id);
        if (SHR_FAILURE(rv)) {
            goto error;
        }

        rv = bcmlrd_field_pt_map_info_num_get(unit, table_id, field_id, &num_entries);
        if (SHR_FAILURE(rv) || num_entries == 0) {
            goto error;
        }
        /* In case LT field has multiple PT mappings. */
        if (num_entries > 1) {
            continue;
        }

        pt_maps = sal_alloc(sizeof(bcmlrd_pt_map_info_t) * num_entries,
                            "bcmDiagLtswCounterPtMap");
        if (pt_maps == NULL) {
            rv = SHR_E_MEMORY;
            goto error;
        }
        sal_memset(pt_maps, 0x0, sizeof(bcmlrd_pt_map_info_t) * num_entries);

        rv = bcmlrd_field_pt_map_info_get(unit, table_id, field_id, num_entries,
                                          &ret_entries, pt_maps);
        if (SHR_FAILURE(rv)) {
            goto error;
        }

        pt_name = bcmdrd_pt_sid_to_name(unit, pt_maps->pt_id);
        if (pt_name == NULL) {
            sal_free(pt_maps);
            continue;
        }
        ltsw_strncpy(cname, pt_name, 31);
        ltsw_counter_display(unit, port, cname, field_val, diff, changed,
                             error_cnt, flags);
        sal_free(pt_maps);
    }

    return bcmlt_entry_free(entry_hdl);

error:
    if (pt_maps) {
        sal_free(pt_maps);
    }
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (table_fields != NULL) {
        sal_free(table_fields);
        table_fields = NULL;
    }
    return rv;
}

static int
ltsw_mac_counter_resync(int unit, const char *table, int port)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_field_def_t *table_fields = NULL;
    uint32_t idx, num_fields;
    uint32_t alloc_size, base_index, entries_perport;
    uint64_t field_val;

    if (!(bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_PORT) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT))) {
        return 0;
    }

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    if(table == CTR_MACs) {
        base_index = ltsw_ctr_ctrl.base_index0;
        entries_perport = ltsw_ctr_ctrl.entries_perport0;
    } else if (table == CTR_MAC_ERRs) {
        base_index = ltsw_ctr_ctrl.base_index1;
        entries_perport = ltsw_ctr_ctrl.entries_perport1;
    } else if (table == CTR_L3s) {
        base_index = ltsw_ctr_ctrl.l3_base_index;
        entries_perport = ltsw_ctr_ctrl.l3_entries_perport;
    } else if (table == CTR_ECNs) {
        base_index = ltsw_ctr_ctrl.ecn_base_index;
        entries_perport = ltsw_ctr_ctrl.ecn_entries_perport;
    } else if (table == CTR_ING_DEBUGs) {
        base_index = ltsw_ctr_ctrl.rx_dbg_base_index;
        entries_perport = ltsw_ctr_ctrl.rx_dbg_entries_perport;
    } else if (table == CTR_EGR_DEBUGs) {
        base_index = ltsw_ctr_ctrl.tx_dbg_base_index;
        entries_perport = ltsw_ctr_ctrl.tx_dbg_entries_perport;
    } else if (table == CTR_MIRROR_ING_PORT_SFLOWs) {
        base_index = ltsw_ctr_ctrl.sflow_base_index;
        entries_perport = ltsw_ctr_ctrl.sflow_entries_perport;
    } else {
        rv = SHR_E_PARAM;
        goto error;
    }

    /* Get the LT fields info. */
    rv = bcmlt_table_field_defs_get(dunit, table, 0, NULL, &num_fields);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    alloc_size = sizeof(bcmlt_field_def_t) * num_fields;
    table_fields = sal_alloc(alloc_size, "bcmDiagLtswCounterFieldInfo");
    if (table_fields == NULL) {
        rv = SHR_E_MEMORY;
        goto error;
    }
    sal_memset(table_fields, 0, alloc_size);
    rv = bcmlt_table_field_defs_get(dunit, table, num_fields, table_fields,
                                    &num_fields);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_add(entry_hdl, PORT_IDs, port);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    for (idx = 0; idx < num_fields; idx++) {
        if (table_fields[idx].key || table_fields[idx].symbol) {
            continue;
        }
        rv = bcmlt_entry_field_get(entry_hdl, table_fields[idx].name, &field_val);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        ltsw_counter_val_set(unit, port, entries_perport, base_index, idx,
                             field_val);
    }

    sal_free(table_fields);
    table_fields = NULL;

    return bcmlt_entry_free(entry_hdl);

error:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (table_fields != NULL) {
        sal_free(table_fields);
        table_fields = NULL;
    }
    return rv;
}

static int
ltsw_mac_counter_clear(int unit, const char *table, int port)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_field_def_t *table_fields = NULL;
    uint32_t idx, num_fields;
    uint32_t alloc_size;

    if (!(bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_PORT) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT))) {
        return 0;
    }

    /* Get the LT fields info. */
    rv = bcmlt_table_field_defs_get(dunit, table, 0, NULL, &num_fields);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    alloc_size = sizeof(bcmlt_field_def_t) * num_fields;
    table_fields = sal_alloc(alloc_size, "bcmDiagLtswCounterFieldInfo");
    if (table_fields == NULL) {
        rv = SHR_E_MEMORY;
        goto error;
    }
    sal_memset(table_fields, 0, alloc_size);
    rv = bcmlt_table_field_defs_get(dunit, table, num_fields, table_fields,
                                    &num_fields);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    rv = bcmlt_entry_field_add(entry_hdl, PORT_IDs, port);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    for (idx = 0; idx < num_fields; idx++) {
        if (table_fields[idx].key || table_fields[idx].symbol) {
            continue;
        }
        rv = bcmlt_entry_field_add(entry_hdl, table_fields[idx].name, 0);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
    }

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    sal_free(table_fields);
    table_fields = NULL;
    return bcmlt_entry_free(entry_hdl);

error:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (table_fields != NULL) {
        sal_free(table_fields);
        table_fields = NULL;
    }
    return rv;
}

static int
ltsw_queue_counter_show(int unit, int port, int non_dma_id, int flags)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;
    bcmi_ltsw_stat_counter_non_dma_t *non_dma = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t entry_info;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    char cname[32];
    uint32_t idx, num_queues;
    uint64_t field_val, prev_val, diff;
    bool changed = FALSE, error_cnt = FALSE;

    if (!(bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_PORT) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT))) {
        return 0;
    }

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        rv = SHR_E_INIT;
        goto error;
    }
    non_dma = &ltsw_ctr_ctrl.counter_non_dma[non_dma_id];
    if(non_dma == NULL || non_dma->id != non_dma_id) {
        rv = SHR_E_INIT;
        goto error;
    }

    /* CTR_EGR_UC_Q LT is not supported on CPU port. */
    if ((sal_strcmp(non_dma->table, CTR_EGR_UC_Qs) == 0) &&
        bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU)) {
        return 0;
    }

    rv = bcmi_ltsw_cosq_port_map_info_get(unit, port, &port_info);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    if (sal_strcmp(non_dma->table, CTR_EGR_UC_Qs) == 0) {
        num_queues = port_info.num_uc_q;
    } else {
        num_queues = port_info.num_mc_q;
    }

    if (num_queues == 0) {
        goto error;
    }

    rv = bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    for (idx = 0; idx < num_queues; idx++) {
        rv = bcmlt_entry_allocate(dunit, non_dma->table, &entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_add(entry_hdl, PORT_IDs, port);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_add(entry_hdl, non_dma->key, idx);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_LOOKUP,
                                         entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        entry_hdl = BCMLT_INVALID_HDL;
    }

    rv = bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        cli_out("Warning: Invalid trans_info.status(%s) for port=%d.\n",
                shr_errmsg(rv), port);
        goto error;
    }

    for (idx = 0; idx < num_queues; idx++) {
        rv = bcmlt_transaction_entry_num_get(trans_hdl, idx, &entry_info);
        if (SHR_FAILURE(rv)) {
            goto error;
        }

        entry_hdl = entry_info.entry_hdl;
        rv = bcmlt_entry_field_get(entry_hdl, PKTs, &field_val);
        if (SHR_FAILURE(rv)) {
            goto error;
        }

        prev_val = 0;
        ltsw_counter_val_get(unit, port, non_dma->entries_perport,
                             non_dma->base_index, idx, &prev_val);
        if (prev_val > field_val) {
            prev_val = 0;
        }
        diff = field_val - prev_val;
        if (diff > 0) {
            changed = TRUE;
            ltsw_counter_val_set(unit, port, non_dma->entries_perport,
                                 non_dma->base_index, idx, field_val);
        } else {
            changed = FALSE;
        }

        sal_sprintf(&cname[0], "%s(%d)", non_dma->cname_pkt, idx);
        ltsw_counter_display(unit, port, cname, field_val, diff, changed,
                             error_cnt, flags);
    }

    for (idx = 0; idx < num_queues; idx++) {
        rv = bcmlt_transaction_entry_num_get(trans_hdl, idx, &entry_info);
        if (SHR_FAILURE(rv)) {
            goto error;
        }

        entry_hdl = entry_info.entry_hdl;
        rv = bcmlt_entry_field_get(entry_hdl, BYTESs, &field_val);
        if (SHR_FAILURE(rv)) {
            goto error;
        }

        prev_val = 0;
        ltsw_counter_val_get(unit, port, non_dma->entries_perport,
                             non_dma->base_index, idx + num_queues, &prev_val);
        diff = field_val - prev_val;
        if (diff > 0) {
            changed = TRUE;
            ltsw_counter_val_set(unit, port, non_dma->entries_perport,
                                 non_dma->base_index, idx + num_queues, field_val);
        } else {
            changed = FALSE;
        }

        sal_sprintf(&cname[0], "%s(%d)", non_dma->cname_byte, idx);
        ltsw_counter_display(unit, port, cname, field_val, diff, changed,
                             error_cnt, flags);
    }

    return bcmlt_transaction_free(trans_hdl);

error:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    return rv;
}

static int
ltsw_queue_counter_resync(int unit, int port, int non_dma_id)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;
    bcmi_ltsw_stat_counter_non_dma_t *non_dma = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t entry_info;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    uint32_t idx, num_queues;
    uint64_t field_val;

    if (!(bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_PORT) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT))) {
        return 0;
    }

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        rv = SHR_E_INIT;
        goto error;
    }
    non_dma = &ltsw_ctr_ctrl.counter_non_dma[non_dma_id];
    if(non_dma == NULL || non_dma->id != non_dma_id) {
        rv = SHR_E_INIT;
        goto error;
    }

    /* CTR_EGR_UC_Q LT is not supported on CPU port. */
    if ((sal_strcmp(non_dma->table, CTR_EGR_UC_Qs) == 0) &&
        bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU)) {
        return 0;
    }

    rv = bcmi_ltsw_cosq_port_map_info_get(unit, port, &port_info);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    if (sal_strcmp(non_dma->table, CTR_EGR_UC_Qs) == 0) {
        num_queues = port_info.num_uc_q;
    } else {
        num_queues = port_info.num_mc_q;
    }

    if (num_queues == 0) {
        goto error;
    }

    rv = bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }

    for (idx = 0; idx < num_queues; idx++) {
        rv = bcmlt_entry_allocate(dunit, non_dma->table, &entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_add(entry_hdl, PORT_IDs, port);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_add(entry_hdl, non_dma->key, idx);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_LOOKUP, entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        entry_hdl = BCMLT_INVALID_HDL;
    }

    rv = bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        cli_out("Warning: Invalid trans_info.status(%s) for port=%d.\n",
                shr_errmsg(rv), port);
        goto error;
    }

    for (idx = 0; idx < num_queues; idx++) {
        rv = bcmlt_transaction_entry_num_get(trans_hdl, idx, &entry_info);
        if (SHR_FAILURE(rv)) {
            goto error;
        }

        entry_hdl = entry_info.entry_hdl;
        rv = bcmlt_entry_field_get(entry_hdl, PKTs, &field_val);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        ltsw_counter_val_set(unit, port, non_dma->entries_perport,
                             non_dma->base_index, idx, field_val);

        rv = bcmlt_entry_field_get(entry_hdl, BYTESs, &field_val);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        ltsw_counter_val_set(unit, port, non_dma->entries_perport,
                             non_dma->base_index, idx + num_queues, field_val);

    }

    return bcmlt_transaction_free(trans_hdl);

error:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    return rv;
}

static int
ltsw_resync_counters(int unit, bcm_pbmp_t pbmp)
{
    int rv = SHR_E_NONE;
    int port;
    int non_dma_id;
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INIT;
    }

    BCM_PBMP_ITER(pbmp, port) {
        rv = ltsw_mac_counter_resync(unit, CTR_MACs, port);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: Resync MAC counters for port=%d failed(%s).\n",
                    port, shr_errmsg(rv));
        }

        rv = ltsw_mac_counter_resync(unit, CTR_MAC_ERRs, port);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: Resync MAC error counters for port=%d failed(%s).\n",
                    port, shr_errmsg(rv));
        }

        if (ltsw_ctr_ctrl.counter_non_dma != NULL) {
            for (non_dma_id = 0;
                 non_dma_id < BCMI_LTSW_COUNTER_NON_DMA_COUNT; non_dma_id++) {
                rv = ltsw_queue_counter_resync(unit, port, non_dma_id);
                if (SHR_FAILURE(rv)) {
                    cli_out("Error: Resync non-dma counters for "
                            "port=%d non_dma_id=%d failed(%s).\n",
                            port, non_dma_id, shr_errmsg(rv));
                }
            }
        }

        if (ltsw_ctr_ctrl.l3_entries_perport != 0) {
            rv = ltsw_mac_counter_resync(unit, CTR_L3s, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Resync L3 counters for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.ecn_entries_perport != 0) {
            rv = ltsw_mac_counter_resync(unit, CTR_ECNs, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Resync ECN counters for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.rx_dbg_entries_perport != 0) {
            rv = ltsw_mac_counter_resync(unit, CTR_ING_DEBUGs, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Resync ingress debug counters for port=%d "
                        "failed(%s).\n", port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.tx_dbg_entries_perport != 0) {
            rv = ltsw_mac_counter_resync(unit, CTR_EGR_DEBUGs, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Resync egress debug counters for port=%d "
                        "failed(%s).\n", port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.sflow_entries_perport != 0) {
            rv = ltsw_mac_counter_resync(unit, CTR_MIRROR_ING_PORT_SFLOWs,
                                         port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Resync Mirror SFLOW counters for port=%d "
                        "failed(%s).\n", port, shr_errmsg(rv));
            }
        }
    }

    return rv;
}

/*!
 * \brief Get numeric debug counter value from input string.
 *
 * \param [in] c Input string.
 * \param [in] rx_tx Debug counter type (CSTAT_RDBGC/CSTAT_TDBGC).
 * \param [in] counter Debug counter number to be returned.
 *
 * \retval CMD_OK   Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static int
cstat_get_counter(char *c, int rx_tx, int *counter)
{
    int i = 0;
    int n = 0;

    while (isdigit((unsigned) c[i])) {
        n = n * 10 + c[i++] - '0';
    }

    if (n < 0 || c[i]) {
        return CMD_FAIL;
    }

    if (rx_tx == CSTAT_RDBGC) {
        if (n > (snmpBcmCustomReceive8 - snmpBcmCustomReceive0)) {
            /* n = 9 ~ 15 */
            n = n - 9;
            n += snmpBcmCustomReceive9;
        } else {
            /* n = 0 ~ 8 */
            n += snmpBcmCustomReceive0;
        }
        if ((n < snmpBcmCustomReceive0) ||
            (n >= snmpBcmCustomTransmit0 && n < snmpBcmCustomReceive9) ||
            (n >= (snmpBcmCustomReceive15 + 1))) {
            return CMD_FAIL;
        }
    } else if (rx_tx == CSTAT_TDBGC) {
        n += snmpBcmCustomTransmit0;
        if (n < snmpBcmCustomTransmit0 || n >= (snmpBcmCustomTransmit14 + 1)) {
            return CMD_FAIL;
        }
    }

    *counter = n;

    return CMD_OK;
}

/*!
 * \brief Set triggers for given debug counter.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] counter Debug counter number.
 * \param [in] a Arg structure for parsing arguments.
 */
static void
cstat_set_triggers(int unit, int port, uint32_t counter, args_t *a)
{
    char *c;
    char act = CSTAT_TRIGGER_TOGGLE;
    int cur_config = 0;
    int rv = 0;
    uint32_t trig_typ = 0;

    while ((c = ARG_GET(a)) != NULL) {
        if (c[0] == '+') {
            act = CSTAT_TRIGGER_ADD; /* Add */
            c++;
        } else if (c[0] == '-') {
            act = CSTAT_TRIGGER_DEL; /* Remove */
            c++;
        }

        for (trig_typ = 0; trig_typ < bcmDbgCntNum; trig_typ++) {
            if (parse_cmp(cstat_trigger_names[trig_typ].name, c, '\0')) {
                break;
            }
        }

        if (trig_typ == bcmDbgCntNum) {
            cli_out("%s Not available: see CustomSTAT LS\n", c);
            continue;
        }

        switch (act) {
            case CSTAT_TRIGGER_ADD:
                rv = bcm_stat_custom_add(unit, port, counter, trig_typ);
                break;
            case CSTAT_TRIGGER_DEL:
                rv = bcm_stat_custom_delete(unit, port, counter, trig_typ);
                break;
            default: /* CSTAT_TRIGGER_TOGGLE. */
                rv = bcm_stat_custom_check(unit, port, counter,
                                           trig_typ, &cur_config);
                if (BCM_SUCCESS(rv)) {
                    if (cur_config == 1) {
                        rv = bcm_stat_custom_delete(unit, port, counter,
                                                    trig_typ);
                    } else {
                        rv = bcm_stat_custom_add(unit, port, counter,
                                                 trig_typ);
                    }
                }
        }

        if (BCM_FAILURE(rv)) {
            cli_out("\t%18s\t%s %s (stat %d, port %d): %s\n", "-",
                    cstat_stat_names[counter],
                    cstat_trigger_names[trig_typ].name,
                    counter, port, bcm_errmsg(rv));
        }
    }

    return;
}

/*!
 * \brief Show programmed triggers for given debug counter.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 * \param [in] rx_tx Debug counter type (CSTAT_RDBGC/CSTAT_TDBGC).
 * \param [in] counter Debug counter number (-1 means all).
 */
static void
cstat_flags_show(int unit, bcm_pbmp_t pbmp, int rx_tx, int counter)
{
    uint32_t count_index;
    uint32_t start;
    uint32_t end;
    int port;
    int title;
    int i = 0;
    int rv = 0;
    int trig_typ = 0;
    int cur_config = 0;
    bool counter_is_all = false;

    BCM_PBMP_ITER(pbmp, port) {
        break;
    }

    /* Debug counter type is RX. */
    if (rx_tx == CSTAT_RDBGC) {
        cli_out("Custom Receive stats are enabled for following triggers\n");
        cli_out("[Triggers are common for all ports]\n");

        if (counter == -1) {
            start = snmpBcmCustomReceive0;
            end = snmpBcmCustomTransmit0;
            counter_is_all = true;
        } else {
            start = counter;
            end = counter + 1;
        }

        for (count_index = start; count_index < end; count_index++) {
            title = 1;
            for (trig_typ = 0, i = 0; trig_typ < bcmDbgCntNum; trig_typ++) {
                cur_config = 0;
                rv = bcm_stat_custom_check(unit, port, count_index,
                                           trig_typ, &cur_config);
                if (BCM_SUCCESS(rv) && cur_config) {
                    if (title) {
                        cli_out("\n%23s-\n\t\t",
                                cstat_stat_names[count_index]);
                        title = 0;
                    }
                    cli_out("%s%s", cstat_trigger_names[trig_typ].name,
                            !((i + 1) % 5) ? "\n\t\t" : " ");
                    i++;
                } else {
                    continue;
                }
            }
        }
        if (counter_is_all) {
            start = snmpBcmCustomReceive9;
            end = (snmpBcmCustomReceive15 + 1);
            for (count_index = start; count_index < end; count_index++) {
                title = 1;
                for (trig_typ = 0, i = 0;
                     trig_typ < bcmDbgCntNum; trig_typ++) {
                    cur_config = 0;
                    rv = bcm_stat_custom_check(unit, port, count_index,
                                               trig_typ, &cur_config);
                    if (BCM_SUCCESS(rv) && cur_config) {
                        if (title) {
                            cli_out("\n%23s-\n\t\t",
                                    cstat_stat_names[count_index]);
                            title = 0;
                        }
                        cli_out("%s%s", cstat_trigger_names[trig_typ].name,
                                !((i + 1) % 5) ? "\n\t\t" : " ");
                        i++;
                    } else {
                        continue;
                    }
                }
            }
        }
        cli_out("\n");

        return;
    }

    /* Debug counter type is TX. */
    if (rx_tx == CSTAT_TDBGC) {
        cli_out("Custom Transmit stats are enabled for following triggers\n");
        cli_out("[Triggers are common for all ports]\n");

        start = (counter == -1) ? snmpBcmCustomTransmit0 : counter;
        end = (counter == -1) ? (snmpBcmCustomTransmit14 + 1) : counter + 1;

        for (count_index = start; count_index < end; count_index++) {
            title = 1;
            for (trig_typ = 0, i = 0; trig_typ < bcmDbgCntNum; trig_typ++) {
                cur_config = 0;
                rv = bcm_stat_custom_check(unit, port, count_index,
                                           trig_typ, &cur_config);
                if (BCM_SUCCESS(rv) && cur_config) {
                    if (title) {
                        cli_out("\n%23s-\n\t\t",
                                cstat_stat_names[count_index]);
                        title = 0;
                    }
                    cli_out("%s%s", cstat_trigger_names[trig_typ].name,
                            !((i + 1) % 5) ? "\n\t\t" : " ");
                    i++;
                } else {
                    continue;
                }
            }
        }
        cli_out("\n");

        return;
    }

}

/*!
 * \brief Show custom statistics for given debug counter.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 * \param [in] rx_tx Debug counter type (CSTAT_RDBGC/CSTAT_TDBGC).
 * \param [in] counter Debug counter number (-1 means all).
 *
 * \retval CMD_OK   Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static int
cstat_stats_show(int unit, bcm_pbmp_t pbmp, int rx_tx, int counter)
{
    uint32_t count_index;
    uint32_t start;
    uint32_t end;
    uint32_t title;
    uint64_t value;
    int port;
    int rv = 0;
    bool counter_is_all = false;

    cli_out("%s Custom Statistics :\n", rx_tx == CSTAT_RDBGC ? "RX" : "TX");

    /* coverity[overrun-local] */
    BCM_PBMP_ITER(pbmp, port) {
        switch (rx_tx) {
            case CSTAT_RDBGC:
                if (counter == -1) {
                    start = snmpBcmCustomReceive0;
                    end = snmpBcmCustomTransmit0;
                    counter_is_all = true;
                } else {
                    start = counter;
                    end = counter + 1;
                }

                title = 1;
                for (count_index = start; count_index < end; count_index++) {
                    COMPILER_64_ZERO(value);
                    rv = bcm_stat_get(unit, port, count_index, &value);
                    if (BCM_FAILURE(rv)) {
                        if ((rv != SHR_E_UNAVAIL) || (counter != -1)) {
                            cli_out("\t%18s\t%s (stat %d): %s\n", "-",
                                    cstat_stat_names[count_index], count_index,
                                    bcm_errmsg(rv));
                            continue;
                        }
                    }

                    if (COMPILER_64_IS_ZERO(value)) {
                        continue;
                    }

                    if (title) {
                        cli_out(" %s:\n", BCM_PORT_NAME(unit, port));
                        title = 0;
                    }

                    if (COMPILER_64_HI(value) == 0) {
                        cli_out("    %18u\t%s (stat %d)\n",
                                COMPILER_64_LO(value),
                                cstat_stat_names[count_index], count_index);
                    } else {
                        cli_out("    0x%08x%08x\t%s (stat %d)\n",
                                COMPILER_64_HI(value),
                                COMPILER_64_LO(value),
                                cstat_stat_names[count_index], count_index);
                    }
                }
                if (counter_is_all) {
                    title = 1;
                    start = snmpBcmCustomReceive9;
                    end = (snmpBcmCustomReceive15 + 1);
                    for (count_index = start;
                         count_index < end; count_index++) {
                        COMPILER_64_ZERO(value);
                        rv = bcm_stat_get(unit, port, count_index, &value);
                        if (BCM_FAILURE(rv)) {
                            if ((rv != SHR_E_UNAVAIL) || (counter != -1)) {
                                cli_out("\t%18s\t%s (stat %d): %s\n", "-",
                                        cstat_stat_names[count_index],
                                        count_index,
                                        bcm_errmsg(rv));
                                continue;
                            }
                        }

                        if (COMPILER_64_IS_ZERO(value)) {
                            continue;
                        }

                        if (title) {
                            cli_out(" %s:\n", BCM_PORT_NAME(unit, port));
                            title = 0;
                        }

                        if (COMPILER_64_HI(value) == 0) {
                            cli_out("    %18u\t%s (stat %d)\n",
                                    COMPILER_64_LO(value),
                                    cstat_stat_names[count_index],
                                    count_index);
                        } else {
                            cli_out("    0x%08x%08x\t%s (stat %d)\n",
                                    COMPILER_64_HI(value),
                                    COMPILER_64_LO(value),
                                    cstat_stat_names[count_index],
                                    count_index);
                        }
                    }
                }
                break;
            case CSTAT_TDBGC:
                start = (counter == -1) ? snmpBcmCustomTransmit0 : counter;
                end = (counter == -1) ? (snmpBcmCustomTransmit14 + 1) :
                                        counter + 1;

                title = 1;
                for (count_index = start; count_index < end; count_index++) {
                    COMPILER_64_ZERO(value);
                    rv = (bcm_stat_get(unit, port, count_index, &value));
                    if (BCM_FAILURE(rv)) {
                        if ((rv != SHR_E_UNAVAIL) || (counter != -1)) {
                            cli_out("\t%18s\t%s (stat %d): %s\n", "-",
                                    cstat_stat_names[count_index], count_index,
                                    bcm_errmsg(rv));
                            continue;
                        }
                    }

                    if (COMPILER_64_IS_ZERO(value)) {
                        continue;
                    }

                    if (title) {
                        cli_out(" %s:\n", BCM_PORT_NAME(unit, port));
                        title = 0;
                    }

                    if (COMPILER_64_HI(value) == 0) {
                        cli_out("    %18u\t%s (stat %d)\n",
                                COMPILER_64_LO(value),
                                cstat_stat_names[count_index], count_index);
                    } else {
                        cli_out("    0x%08x%08x\t%s (stat %d)\n",
                                COMPILER_64_HI(value),
                                COMPILER_64_LO(value),
                                cstat_stat_names[count_index], count_index);
                    }
                }
                break;
            default:
                cli_out("Invalid custom stats: valid values [RX,TX]\n");
                return CMD_FAIL;
        }
    }
    cli_out("\n");

    return CMD_OK;
}

/*!
 * \brief Set the triggers for given debug counter.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 * \param [in] rx_tx Debug counter type (CSTAT_RDBGC/CSTAT_TDBGC).
 * \param [in] counter Debug counter number to be programmed.
 * \param [in] a Arg structure for parsing arguments.
 *
 * \retval CMD_OK   Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static int
cstat_flags_set(int unit, bcm_pbmp_t pbmp, int rx_tx, int counter, args_t *a)
{
    int flags_index;
    bcm_port_t port;
    uint32_t count_index;
    uint32_t start;
    uint32_t end;
    bool counter_is_all = false;

    if (!ARG_CUR(a)) {
        cli_out("Error: No input triggers\n");
        return CMD_FAIL;
    }

    /* Save index of first flag. */
    flags_index = a->a_arg;

    /* coverity[overrun-local] */
    BCM_PBMP_ITER(pbmp, port) {
        switch (rx_tx) {
            case CSTAT_RDBGC:
                if (counter == -1) {
                    start = snmpBcmCustomReceive3;
                    end = snmpBcmCustomTransmit0;
                    counter_is_all = true;
                } else {
                    start = counter;
                    end = counter + 1;
                }

                for (count_index = start; count_index < end; count_index++) {
                    a->a_arg = flags_index;
                    cstat_set_triggers(unit, port, count_index, a);
                }

                if (counter_is_all) {
                    start = snmpBcmCustomReceive9;
                    end = (snmpBcmCustomReceive15 + 1);
                    for (count_index = start;
                         count_index < end; count_index++) {
                        a->a_arg = flags_index;
                        cstat_set_triggers(unit, port, count_index, a);
                    }
                }
                break;
            case CSTAT_TDBGC:
                start = (counter == -1) ? snmpBcmCustomTransmit6 : counter;
                end = (counter == -1) ? snmpBcmCustomTransmit12 : counter + 1;
                for (count_index = start; count_index < end; count_index++) {
                    a->a_arg = flags_index;
                    cstat_set_triggers(unit, port, count_index, a);
                }
                break;
            default:
                 cli_out("Error: Invalid custom stats: valid values [RX,TX]\n");
                return CMD_FAIL;
        }
        break;
    }
    return CMD_OK;
}

/*!
 * \brief Display the triggers available for current device.
 *
 * \param [in] unit Unit number.
 * \param [in] about (0/1) Whether to display short description of each trigger.
 * \param [in] rx_tx Debug counter type (CSTAT_RDBGC/CSTAT_TDBGC).
 */
static void
cstat_list_flags(int unit, int about, int rx_tx)
{
    int trig_typ;
    int res;
    int loop;
    int dummy_port = bcmi_ltsw_port_cpu(unit);

    cli_out("Custom stat\n");
    if (rx_tx == CSTAT_RDBGC || rx_tx == CSTAT_BOTH) {
        cli_out(" Available Rx trigger flags :\n  ");
        for (trig_typ = 0, loop = 0; trig_typ < bcmDbgCntNum; trig_typ++) {
            if (BCM_SUCCESS(bcm_stat_custom_check(unit, dummy_port,
                                                  snmpBcmCustomReceive0,
                                                  trig_typ, &res))) {
                if (!about) {
                    cli_out("%-35s%s", cstat_trigger_names[trig_typ].name,
                            !((loop + 1) % 3) ? "\n  " : "");
                } else {
                    cli_out("%-35s%s\n  ", cstat_trigger_names[trig_typ].name,
                            cstat_trigger_names[trig_typ].about);
                }
                loop++;
            }
        }
        cli_out("\n");
    }

    if (rx_tx == CSTAT_TDBGC || rx_tx == CSTAT_BOTH) {
        cli_out(" Available Tx trigger flags :\n  ");
        for (trig_typ = 0, loop = 0; trig_typ < bcmDbgCntNum; trig_typ++) {
            if (BCM_SUCCESS(bcm_stat_custom_check(unit, dummy_port,
                                                  snmpBcmCustomTransmit0,
                                                  trig_typ, &res))) {
                if (!about) {
                    cli_out("%-35s%s", cstat_trigger_names[trig_typ].name,
                            !((loop + 1) % 3) ? "\n  " : "");
                } else {
                    cli_out("%-35s%s\n  ", cstat_trigger_names[trig_typ].name,
                            cstat_trigger_names[trig_typ].about);
                }
                loop++;
            }
        }
        cli_out("\n");
    }
}

/*!
 * \brief Display value of debug counter.
 *
 * \param [in] val Value to be displayed.
 * \param [in] count_index Index of debug counter.
 */
static void
cstat_display_val(uint64_t val, int count_index)
{
    char *typ_s;
    char *no_s;

    typ_s = "  ";
    no_s = "   ";

    if (count_index >= snmpBcmCustomTransmit0 &&
        count_index <= snmpBcmCustomTransmit14) {
        if (count_index == snmpBcmCustomTransmit0) {
            typ_s = "TX";
        }
        if (count_index < snmpBcmCustomTransmit6) {
            no_s = "(R)";
        }
        count_index -= snmpBcmCustomTransmit0;
    } else {
        if (count_index == snmpBcmCustomReceive0) {
            typ_s = "RX";
        }
        if (count_index < snmpBcmCustomReceive3) {
            no_s = "(R)";
        }
        if (count_index >= snmpBcmCustomReceive9) {
            count_index -= snmpBcmCustomReceive9;
            count_index += (snmpBcmCustomReceive8 - snmpBcmCustomReceive0) + 1;
        } else {
            count_index -= snmpBcmCustomReceive0;
        }
    }

    cli_out("\n|  %s  |%2d%s|", typ_s, count_index, no_s);

    if (COMPILER_64_HI(val) == 0) {
        cli_out("%19u| ", COMPILER_64_LO((val)));
    } else {
        cli_out(" 0x%08x%08x| ", COMPILER_64_HI(val), COMPILER_64_LO(val));
    }
}

/*!
 * \brief Display the debug counters and triggers in tabular format.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 * \param [in] show_zero (0/1) Whether to include counters which are zero.
 */
static void
cstat_list_table(int unit, bcm_pbmp_t pbmp, int show_zero)
{
    int n;
    int i;
    int j;
    int rv;
    int trig;
    int port = 0;
    int conf;
    int val_f;
    int title;
    int mid;
    int end;
    char *blnk = "                                             ";
    uint64_t value;

    /* coverity[overrun-local] */
    BCM_PBMP_ITER(pbmp, port) {
        title = 1;
        mid = 1;
        end = 0;
        for (n = snmpBcmCustomReceive0; n <= snmpBcmCustomReceive8; n++) {
            rv = bcm_stat_get(unit, port, n, &value);
            if (BCM_FAILURE(rv)) {
                continue;
            }

            if (!show_zero && (COMPILER_64_IS_ZERO(value))) {
                continue;
            }

            if (title) {
                /* coverity[non_const_printf_format_string] */
                cli_out(cstat_head, BCM_PORT_NAME(unit, (port)));
                cli_out("%s%s", cstat_menu, cstat_line);
                title = 0;
                end = 1;
            }

            val_f = 1;
            j = 0;
            for (trig = 0, i = 0; trig < bcmDbgCntNum; trig++) {
                conf = 0;
                rv = bcm_stat_custom_check(unit, port, n, trig, &conf);
                if (BCM_SUCCESS(rv) && conf) {
                    if (val_f) {
                        cstat_display_val(value, n);
                        val_f = 0;
                    }

                    if (!((i + 1) % CSTAT_COL_SZ)) {
                        cli_out("%.*s|%s", (int) (sal_strlen(blnk) - j),
                                blnk, cstat_trigger_s);
                        j = 0;
                    }
                    cli_out("%s ", cstat_trigger_names[trig].name);
                    j = j + sal_strlen(cstat_trigger_names[trig].name) + 1;
                    i++;
                }
            }

            if (j) {
                cli_out("%.*s|", (int) (sal_strlen(blnk) - j), blnk);
            }
        }

        for (n = snmpBcmCustomReceive9; n <= snmpBcmCustomReceive15; n++) {
            rv = bcm_stat_get(unit, port, n, &value);
            if (BCM_FAILURE(rv)) {
                continue;
            }

            if (!show_zero && (COMPILER_64_IS_ZERO(value))) {
                continue;
            }

            if (title) {
                /* coverity[non_const_printf_format_string] */
                cli_out(cstat_head, BCM_PORT_NAME(unit, (port)));
                cli_out("%s%s", cstat_menu, cstat_line);
                title = 0;
                end = 1;
            }

            val_f = 1;
            j = 0;
            for (trig = 0, i = 0; trig < bcmDbgCntNum; trig++) {
                conf = 0;
                rv = bcm_stat_custom_check(unit, port, n, trig, &conf);
                if (BCM_SUCCESS(rv) && conf) {
                    if (val_f) {
                        cstat_display_val(value, n);
                        val_f = 0;
                    }

                    if (!((i + 1) % CSTAT_COL_SZ)) {
                        cli_out("%.*s|%s", (int) (sal_strlen(blnk) - j),
                                blnk, cstat_trigger_s);
                        j = 0;
                    }
                    cli_out("%s ", cstat_trigger_names[trig].name);
                    j = j + sal_strlen(cstat_trigger_names[trig].name) + 1;
                    i++;
                }
            }

            if (j) {
                cli_out("%.*s|", (int) (sal_strlen(blnk) - j), blnk);
            }
        }

        for (n = snmpBcmCustomTransmit0; n <= snmpBcmCustomTransmit14; n++) {
            rv = bcm_stat_get(unit, port, n, &value);
            if (BCM_FAILURE(rv)) {
                continue;
            }

            if (!show_zero && (COMPILER_64_IS_ZERO(value))) {
                continue;
            }

            if (title) {
                /* coverity[non_const_printf_format_string] */
                cli_out(cstat_head, BCM_PORT_NAME(unit, (port)));
                cli_out("%s%s", cstat_menu, cstat_line);
                title = 0;
                end = 1;
            }

            if (n >= snmpBcmCustomTransmit0 && mid) {
                cli_out("%s", cstat_line);
                mid = 0;
            }

            val_f = 1;
            j = 0;
            for (trig = 0, i = 0; trig < bcmDbgCntNum; trig++) {
                conf = 0;
                rv = bcm_stat_custom_check(unit, port, n, trig, &conf);
                if (BCM_SUCCESS(rv) && conf) {
                    if (val_f) {
                        cstat_display_val(value, n);
                        val_f = 0;
                    }

                    if (!((i + 1) % CSTAT_COL_SZ)) {
                        cli_out("%.*s|%s", (int) (sal_strlen(blnk) - j),
                                blnk, cstat_trigger_s);
                        j = 0;
                    }
                    cli_out("%s ", cstat_trigger_names[trig].name);
                    j = j + sal_strlen(cstat_trigger_names[trig].name) + 1;
                    i++;
                }
            }

            if (j) {
                cli_out("%.*s|", (int) (sal_strlen(blnk) - j), blnk);
            }
        }

        if (end) {
            cli_out("%s", cstat_line);
            end = 0;
            cli_out("\n\n");
        }
    }
}

static int
ltsw_estat_show(int unit, int show_zero, int event_type, int direction,
                const char *event_id)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char *table = NULL;
    const char *key_field = NULL;
    const char *key_sym = NULL;
    const char *cnt_field = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t entry_info;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    const char **sstrs = NULL;
    uint32_t num_keys = 0;
    uint32_t alloc_size = 0;
    uint64_t value = 0;
    bool found = false;
    int idx = 0;
    int num_entries = 0;
    char buf_val[32] = {0};

    if (event_type == ESTAT_TYPE_DROP) {
        if (direction == ESTAT_DIR_INGRESS) {
            table = CTR_ING_DROP_EVENTs;
            key_field = CTR_ING_DROP_EVENT_IDs;
        } else {
            table = CTR_EGR_DROP_EVENTs;
            key_field = CTR_EGR_DROP_EVENT_IDs;
        }
        cnt_field = DROP_CNTs;
    } else {
        if (direction == ESTAT_DIR_INGRESS) {
            table = CTR_ING_TRACE_EVENTs;
            key_field = CTR_ING_TRACE_EVENT_IDs;
        } else {
            table = CTR_EGR_TRACE_EVENTs;
            key_field = CTR_EGR_TRACE_EVENT_IDs;
        }
        cnt_field = TRACE_CNTs;
    }

    rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    /* Get all valid event IDs. */
    rv = bcmlt_entry_field_symbol_info_get(entry_hdl, key_field, 0, NULL,
                                           &num_keys);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    alloc_size = sizeof(const char *) * num_keys;
    sstrs = sal_alloc(alloc_size, "bcmDiagLtswCounterEventIDs");
    if (sstrs == NULL) {
        rv = SHR_E_MEMORY;
        goto error;
    }
    rv = bcmlt_entry_field_symbol_info_get(entry_hdl, key_field, num_keys, sstrs,
                                           &num_keys);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    (void)bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    if (event_id != NULL) {
        for (idx = 0; idx < num_keys; idx++) {
            if (sal_strcasecmp(event_id, sstrs[idx]) == 0) {
                event_id = sstrs[idx];
                found = true;
                break;
            }
        }
        if (!found) {
            cli_out("Error: Invalid event ID %s\n", event_id);
        }
    }

    rv = bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    if (event_id == NULL) {
        num_entries = 0;
        for (idx = 0; idx < num_keys; idx++) {
            rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            rv = bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            rv = bcmlt_entry_field_symbol_add(entry_hdl, key_field, sstrs[idx]);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            rv = bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_LOOKUP,
                                             entry_hdl);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            entry_hdl = BCMLT_INVALID_HDL;
            num_entries++;
        }
    } else {
        rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_symbol_add(entry_hdl, key_field, event_id);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_LOOKUP,
                                         entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        entry_hdl = BCMLT_INVALID_HDL;
        num_entries = 1;
    }

    rv = bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv) && (rv != SHR_E_PARTIAL)) {
        goto error;
    }

    cli_out("%s %s Event Counters :\n",
            direction == ESTAT_DIR_INGRESS ? "RX" : "TX",
            event_type == ESTAT_TYPE_DROP ? "Drop" : "Trace");

    for (idx = 0; idx < num_entries; idx++) {
        rv = bcmlt_transaction_entry_num_get(trans_hdl, idx, &entry_info);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        if (entry_info.status == SHR_E_NOT_FOUND) {
            continue;
        }
        if (entry_info.status != SHR_E_NONE) {
            rv = entry_info.status;
            break;
        }
        entry_hdl = entry_info.entry_hdl;
        rv = bcmlt_entry_field_get(entry_hdl, cnt_field, &value);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_symbol_get(entry_hdl, key_field, &key_sym);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        if (show_zero || (event_id != NULL) || ((show_zero == 0) && (value > 0))) {
            format_uint64_decimal(buf_val, value, ',');
            cli_out("    %-44s: %20s\n", key_sym, buf_val);
        }
    }
    cli_out("\n");

error:
    if (sstrs != NULL) {
        sal_free(sstrs);
    }
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    return rv;
}

static int
ltsw_estat_clear(int unit, int event_type, int direction, const char *event_id)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char *table = NULL;
    const char *key_field = NULL;
    const char *cnt_field = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t entry_info;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    const char **sstrs = NULL;
    uint32_t num_keys = 0;
    uint32_t alloc_size = 0;
    bool found = false;
    int idx = 0;
    int num_entries = 0;

    if (event_type == ESTAT_TYPE_DROP) {
        if (direction == ESTAT_DIR_INGRESS) {
            table = CTR_ING_DROP_EVENTs;
            key_field = CTR_ING_DROP_EVENT_IDs;
        } else {
            table = CTR_EGR_DROP_EVENTs;
            key_field = CTR_EGR_DROP_EVENT_IDs;
        }
        cnt_field = DROP_CNTs;
    } else {
        if (direction == ESTAT_DIR_INGRESS) {
            table = CTR_ING_TRACE_EVENTs;
            key_field = CTR_ING_TRACE_EVENT_IDs;
        } else {
            table = CTR_EGR_TRACE_EVENTs;
            key_field = CTR_EGR_TRACE_EVENT_IDs;
        }
        cnt_field = TRACE_CNTs;
    }

    rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    /* Get all valid event IDs. */
    rv = bcmlt_entry_field_symbol_info_get(entry_hdl, key_field, 0, NULL,
                                           &num_keys);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    alloc_size = sizeof(const char *) * num_keys;
    sstrs = sal_alloc(alloc_size, "bcmDiagLtswCounterEventIDs");
    if (sstrs == NULL) {
        rv = SHR_E_MEMORY;
        goto error;
    }
    rv = bcmlt_entry_field_symbol_info_get(entry_hdl, key_field, num_keys, sstrs,
                                           &num_keys);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    (void)bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    if (event_id != NULL) {
        for (idx = 0; idx < num_keys; idx++) {
            if (sal_strcasecmp(event_id, sstrs[idx]) == 0) {
                event_id = sstrs[idx];
                found = true;
                break;
            }
        }
        if (!found) {
            cli_out("Error: Invalid event ID %s\n", event_id);
        }
    }

    rv = bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl);
    if (SHR_FAILURE(rv)) {
        goto error;
    }
    if (event_id == NULL) {
        num_entries = 0;
        for (idx = 0; idx < num_keys; idx++) {
            rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            rv = bcmlt_entry_field_symbol_add(entry_hdl, key_field, sstrs[idx]);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            rv = bcmlt_entry_field_add(entry_hdl, cnt_field, 0);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            rv = bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_UPDATE,
                                             entry_hdl);
            if (SHR_FAILURE(rv)) {
                goto error;
            }
            entry_hdl = BCMLT_INVALID_HDL;
            num_entries++;
        }
    } else {
        rv = bcmlt_entry_allocate(dunit, table, &entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_symbol_add(entry_hdl, key_field, event_id);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_entry_field_add(entry_hdl, cnt_field, 0);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        rv = bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_UPDATE,
                                         entry_hdl);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        entry_hdl = BCMLT_INVALID_HDL;
        num_entries = 1;
    }

    rv = bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv) && (rv != SHR_E_PARTIAL)) {
        goto error;
    }

    for (idx = 0; idx < num_entries; idx++) {
        rv = bcmlt_transaction_entry_num_get(trans_hdl, idx, &entry_info);
        if (SHR_FAILURE(rv)) {
            goto error;
        }
        if (entry_info.status == SHR_E_NOT_FOUND) {
            continue;
        }
        if (entry_info.status != SHR_E_NONE) {
            rv = entry_info.status;
            break;
        }
    }

error:
    if (sstrs != NULL) {
        sal_free(sstrs);
    }
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    return rv;
}

/******************************************************************************
 * Public Functions
 */

int
ltsw_show_counters_proc(int unit, bcm_pbmp_t pbmp, int flags)
{
    int rv = SHR_E_NONE;
    int port;
    int non_dma_id;
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INIT;
    }

    BCM_PBMP_ITER(pbmp, port) {
        rv = ltsw_mac_counter_show(unit, CTR_MACs, port, flags);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: Show MAC counters for port=%d failed(%s).\n",
                    port, shr_errmsg(rv));
        }

        rv = ltsw_mac_counter_show(unit, CTR_MAC_ERRs, port, flags);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: Show MAC error counters for port=%d failed(%s).\n",
                    port, shr_errmsg(rv));
        }

        if (ltsw_ctr_ctrl.counter_non_dma != NULL) {
            for (non_dma_id = 0;
                 non_dma_id < BCMI_LTSW_COUNTER_NON_DMA_COUNT; non_dma_id++) {
                rv = ltsw_queue_counter_show(unit, port, non_dma_id, flags);
                if (SHR_FAILURE(rv)) {
                    cli_out("Error: Show non-dma counters for "
                            "port=%d non_dma_id=%d failed(%s).\n",
                            port, non_dma_id, shr_errmsg(rv));
                }
            }
        }

        if (ltsw_ctr_ctrl.l3_entries_perport != 0) {
            rv = ltsw_mac_counter_show(unit, CTR_L3s, port, flags);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Show L3 counters for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.ecn_entries_perport != 0) {
            rv = ltsw_mac_counter_show(unit, CTR_ECNs, port, flags);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Show L3 counters for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.rx_dbg_entries_perport != 0) {
            rv = ltsw_mac_counter_show(unit, CTR_ING_DEBUGs, port, flags);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Show L3 counters for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.tx_dbg_entries_perport != 0) {
            rv = ltsw_mac_counter_show(unit, CTR_EGR_DEBUGs, port, flags);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Show L3 counters for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }
        }

        if (ltsw_ctr_ctrl.sflow_entries_perport != 0) {
            rv = ltsw_mac_counter_show(unit, CTR_MIRROR_ING_PORT_SFLOWs,
                                       port, flags);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Show Mirror SFLOW counters for port=%d "
                        "failed(%s).\n", port, shr_errmsg(rv));
            }
        }
    }

    return rv;
}

int
ltsw_clear_counters_proc(int unit, bcm_pbmp_t pbmp)
{
    int rv = SHR_E_NONE;
    bcmi_ltsw_stat_counter_control_t ltsw_ctr_ctrl;
    bcmi_ltsw_stat_counter_non_dma_t *non_dma = NULL;
    int port;
    bcm_gport_t gport;
    int non_dma_id;

    rv = bcmi_ltsw_stat_counter_control_get(unit, &ltsw_ctr_ctrl);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INIT;
    }

    BCM_PBMP_ITER(pbmp, port) {
        /* Clear MAC counters. */
        rv = ltsw_mac_counter_clear(unit, CTR_MACs, port);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: Clear MAC counter for port=%d failed(%s).\n",
                    port, shr_errmsg(rv));
        }

        /* Clear the software MAC counters. */
        ltsw_counter_val_set(unit, port, ltsw_ctr_ctrl.entries_perport0,
                             ltsw_ctr_ctrl.base_index0, -1, 0);

        /* Clear MAC error counters. */
        rv = ltsw_mac_counter_clear(unit, CTR_MAC_ERRs, port);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: Clear MAC counter for port=%d failed(%s).\n",
                    port, shr_errmsg(rv));
        }

        /* Clear the software MAC error counters. */
        ltsw_counter_val_set(unit, port, ltsw_ctr_ctrl.entries_perport1,
                             ltsw_ctr_ctrl.base_index1, -1, 0);

        /* Clear non-dma counter. */
        rv = bcm_port_gport_get(unit, port, &gport);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: bcm_port_gport_get failed(%s).\n", shr_errmsg(rv));
        }

        if (ltsw_ctr_ctrl.counter_non_dma != NULL) {
            for (non_dma_id = 0;
                 non_dma_id < BCMI_LTSW_COUNTER_NON_DMA_COUNT; non_dma_id++) {
                rv = bcm_cosq_stat_set(unit, gport,
                                       -1, bcmCosqStatOutPackets, 0);
                if (SHR_FAILURE(rv)) {
                    cli_out("Error: Clear non-dam packets counter for "
                            "port=%d non_dma_id=%d failed(%s).\n",
                            non_dma_id, port, shr_errmsg(rv));
                }
                rv = bcm_cosq_stat_set(unit, gport, -1, bcmCosqStatOutBytes, 0);
                if (SHR_FAILURE(rv)) {
                    cli_out("Error: Clear non-dam bytes counter for "
                            "port=%d non_dma_id=%d failed(%s).\n",
                            non_dma_id, port, shr_errmsg(rv));
                }
                non_dma = &ltsw_ctr_ctrl.counter_non_dma[non_dma_id];
                ltsw_counter_val_set(unit, port, non_dma->entries_perport,
                                     non_dma->base_index, -1, 0);
            }
        }

        if (ltsw_ctr_ctrl.l3_entries_perport != 0) {
            /* Clear L3 counters. */
            rv = ltsw_mac_counter_clear(unit, CTR_L3s, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Clear L3 counter for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }

            /* Clear the software L3 counters. */
            ltsw_counter_val_set(unit, port, ltsw_ctr_ctrl.l3_entries_perport,
                                 ltsw_ctr_ctrl.l3_base_index, -1, 0);
        }

        if (ltsw_ctr_ctrl.ecn_entries_perport != 0) {
            /* Clear ECN counters. */
            rv = ltsw_mac_counter_clear(unit, CTR_ECNs, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Clear ECN counter for port=%d failed(%s).\n",
                        port, shr_errmsg(rv));
            }

            /* Clear the software ECN counters. */
            ltsw_counter_val_set(unit, port, ltsw_ctr_ctrl.ecn_entries_perport,
                                 ltsw_ctr_ctrl.ecn_base_index, -1, 0);
        }

        if (ltsw_ctr_ctrl.rx_dbg_entries_perport != 0) {
            /* Clear ingress debug counters. */
            rv = ltsw_mac_counter_clear(unit, CTR_ING_DEBUGs, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Clear ingress debug counter for port=%d "
                        "failed(%s).\n", port, shr_errmsg(rv));
            }

            /* Clear the software ingress debug counters. */
            ltsw_counter_val_set(unit, port,
                                 ltsw_ctr_ctrl.rx_dbg_entries_perport,
                                 ltsw_ctr_ctrl.rx_dbg_base_index, -1, 0);
        }

        if (ltsw_ctr_ctrl.tx_dbg_entries_perport != 0) {
            /* Clear egress debug counters. */
            rv = ltsw_mac_counter_clear(unit, CTR_EGR_DEBUGs, port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Clear egress debug counter for port=%d "
                        "failed(%s).\n", port, shr_errmsg(rv));
            }

            /* Clear the software egress debug counters. */
            ltsw_counter_val_set(unit, port,
                                 ltsw_ctr_ctrl.tx_dbg_entries_perport,
                                 ltsw_ctr_ctrl.tx_dbg_base_index, -1, 0);
        }

        if (ltsw_ctr_ctrl.sflow_entries_perport != 0) {
            /* Clear Mirror SFLOW counters. */
            rv = ltsw_mac_counter_clear(unit, CTR_MIRROR_ING_PORT_SFLOWs,
                                        port);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: Clear Mirror SFLOW counter for port=%d "
                        "failed(%s).\n", port, shr_errmsg(rv));
            }

            /* Clear the software Mirror SFLOW counters. */
            ltsw_counter_val_set(unit, port,
                                 ltsw_ctr_ctrl.sflow_entries_perport,
                                 ltsw_ctr_ctrl.sflow_base_index, -1, 0);
        }
    }

    return rv;
}

int
ltsw_do_resync_counters(int unit, bcm_pbmp_t pbmp)
{
    return ltsw_resync_counters(unit, pbmp);
}

/*!
 * \brief Counter collection configuration.
 *
 * \param [in] unit Unit number
 * \param [in] a Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
cmd_result_t
cmd_ltsw_counter(int unit, args_t *a)
{
    int rv = SHR_E_NONE;
    ltsw_counter_t cfg;
    int usec;
    bool sync = FALSE;

    rv = counter_control_get(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        cli_out("Error: counter_get() failed(%s).\n", shr_errmsg(rv));
        return CMD_FAIL;
    }

    /* Display settings */
    if (!ARG_CNT(a)) {
        counter_control_print_status(unit, cfg);
        return(CMD_OK);
    }

    usec = cfg.usec;
    rv = counter_control_parse_args(unit, a, &usec, &cfg.pbmp);
    if (SHR_FAILURE(rv)) {
        return CMD_FAIL;
    }

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "on")) {
        cfg.enable = 1;
        ARG_NEXT(a);
    }
    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "off")) {
        cfg.enable = 0;
        ARG_NEXT(a);
    }
    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "sync")) {
        ARG_NEXT(a);
        sync = TRUE;
    }

    if (sync) {
        rv = ltsw_resync_counters(unit, cfg.pbmp);
        if (SHR_FAILURE(rv)) {
            cli_out("Error: ltsw_resync_counters() failed(%s).\n", shr_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (usec == 0) {
        cfg.enable = 0;
    } else {
        cfg.usec = usec;
    }
    rv = counter_control_update(unit, cfg);
    if (SHR_FAILURE(rv)) {
        cli_out("Error: counter_update() failed(%s).\n", shr_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief Main function to process CustomSTAT command.
 *
 * \param [in] unit Unit number.
 * \param [in] a Arg structure for parsing arguments.
 *
 * \retval CMD_OK   Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
cmd_result_t
cmd_ltsw_custom_stat(int unit, args_t *a)
{
    bcm_pbmp_t pbmp;
    bcm_port_t first_port = 0;
    uint32_t port_type;
    uint32_t cmd_type; /* show/list/set flags, show counters/table. */
    char *c = NULL;
    int rx_tx;
    int rv = 0;
    int counter; /* selected DBGC map, input. */
    int display_zero;
    int display_info;

    port_type = BCMI_LTSW_PORT_TYPE_PORT | BCMI_LTSW_PORT_TYPE_MGMT;
    rv = bcmi_ltsw_port_bitmap(unit, port_type, &pbmp);
    if (SHR_FAILURE(rv)) {
        cli_out("Error: bcmi_ltsw_port_bitmap() failed(%s).\n",
                shr_errmsg(rv));
        return CMD_FAIL;
    }

    BCM_PBMP_ITER(pbmp, first_port) {
        break;
    }
    if (first_port >= BCM_PBMP_PORT_MAX) {
        /* To avoid that there is no member port in pbmp */
        first_port = 0;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, first_port); /* Dummy port. */

    counter = -1;
    cmd_type = CSTAT_SHOW_TABLE;
    rx_tx = CSTAT_BOTH;
    display_zero = FALSE;
    display_info = FALSE;

    if (!(c = ARG_GET(a))) {
        goto do_stat_command;
    } else {
        if (!sal_strcasecmp(c, "ls")) {
            cmd_type = CSTAT_LIST_FLAGS;
        } else if (!sal_strcasecmp(c, "info")) {
            cmd_type = CSTAT_LIST_FLAGS;
            display_info = TRUE;
        } else if (!sal_strcasecmp(c, "set")) {
            cmd_type = CSTAT_SET_FLAGS;
            c = ARG_GET(a);
        } else if (!sal_strcasecmp(c, "get")) {
            cmd_type = CSTAT_SHOW_FLAGS;
            c = ARG_GET(a);
        } else if (!sal_strcasecmp(c, "raw")) {
            cmd_type = CSTAT_SHOW_COUNTERS;
            c = ARG_GET(a);
        }
    }

    /* No input port bitmap. */
    if (!c) {
        CSTAT_ERROR_SET_FLAGS("No input port\n", cmd_type);
        goto do_stat_command;
    } else if (cmd_type != CSTAT_LIST_FLAGS) {
        if (parse_pbmp(unit, c, &pbmp) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n",
                    ARG_CMD(a), c);
            return CMD_FAIL;
        }
    }

    /* Check for RX or TX. */
    if (!(c = ARG_GET(a))) {
        CSTAT_ERROR_SET_FLAGS("No input type[rx|tx]\n", cmd_type);
        goto do_stat_command;
    } else {
        if (!sal_strcasecmp(c, "rx")) {
            rx_tx = CSTAT_RDBGC;
        } else if (!sal_strcasecmp(c, "tx")) {
            rx_tx = CSTAT_TDBGC;
        } else if (sal_toupper((int)c[0]) == 'Z') {
            display_zero = TRUE;
            goto do_stat_command;
        } else {
            cli_out("Invalid input '%s'\n", c);
            return CMD_FAIL;
        }

        /* coverity[new_values] */
        if (cmd_type == CSTAT_LIST_FLAGS) {
            goto do_stat_command;
        }
    }

    /* Counter bitmap. */
    if (!(c = ARG_GET(a))) {
        CSTAT_ERROR_SET_FLAGS("No input counter\n", cmd_type);
        goto do_stat_command;
    } else {
        if (!sal_strcasecmp(c, "all")) {
            goto do_stat_command;
        } else if (!cstat_get_counter(c, rx_tx, &counter)) {
            goto do_stat_command;
        } else {
            cli_out("Invalid counter\n");
            return CMD_FAIL;
        }
    }

do_stat_command:
    switch (cmd_type) {
        case CSTAT_SHOW_FLAGS:
            if (rx_tx == CSTAT_BOTH) {
                cstat_flags_show(unit, pbmp, CSTAT_RDBGC, counter);
                cstat_flags_show(unit, pbmp, CSTAT_TDBGC, counter);
            } else {
                cstat_flags_show(unit, pbmp, rx_tx, counter);
            }
            break;
        case CSTAT_SHOW_COUNTERS:
            if (rx_tx == CSTAT_BOTH) {
                rv = cstat_stats_show(unit, pbmp, CSTAT_RDBGC, counter);
                rv = cstat_stats_show(unit, pbmp, CSTAT_TDBGC, counter);
            } else {
                rv = cstat_stats_show(unit, pbmp, rx_tx, counter);
            }
            break;
        case CSTAT_SHOW_TABLE:
            cstat_list_table(unit, pbmp, display_zero);
            break;
        case CSTAT_SET_FLAGS:
            rv = cstat_flags_set(unit, pbmp, rx_tx, counter, a);
            break;
        case CSTAT_LIST_FLAGS:
            cstat_list_flags(unit, display_info, rx_tx);
            break;
        /* coverity[dead_error_begin] */
        default:
            /*
             * should not reach here
             */
            return CMD_FAIL;
    }

    if (rv < 0) {
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief Main function to process EventSTAT command.
 *
 * \param [in] unit Unit number.
 * \param [in] a Arg structure for parsing arguments.
 *
 * \retval CMD_OK   Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
cmd_result_t
cmd_ltsw_event_stat(int unit, args_t *a)
{
    int rv = SHR_E_NONE;
    char *subcmd = NULL;
    bool show_zero = false;
    estat_cmd_t cmd_type = ESTAT_CMD_INVALID;
    estat_dir_t direction = ESTAT_DIR_INVALID;
    estat_type_t event_type = ESTAT_TYPE_INVALID;
    const char *event_id = NULL;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        if (((subcmd = ARG_GET(a)) != NULL) && (sal_toupper((int)subcmd[0]) == 'Z')) {
            subcmd = ARG_GET(a);
            show_zero = true;
        }
        cmd_type = ESTAT_CMD_SHOW;
    } else if (sal_strcasecmp(subcmd, "clear") == 0) {
        subcmd = ARG_GET(a);
        cmd_type = ESTAT_CMD_CLEAR;
    } else {
        cli_out("%s: Error: Invalid option %s\n", ARG_CMD(a), subcmd);
        return CMD_FAIL;
    }

    if (subcmd == NULL) {
        cli_out("%s ERROR: Must specify Drop or Trace subcommand\n", ARG_CMD(a));
        return CMD_FAIL;
    }
    if (sal_toupper((int)subcmd[0]) == 'D') {
        event_type = ESTAT_TYPE_DROP;
    } else if ((sal_toupper((int)subcmd[0]) == 'T')) {
        event_type = ESTAT_TYPE_TRACE;
    } else {
        cli_out("%s: Error: Invalid option %s\n", ARG_CMD(a), subcmd);
        return CMD_FAIL;
    }

    direction = ESTAT_DIR_BOTH;
    if ((subcmd = ARG_GET(a)) != NULL) {
        if (sal_strcasecmp(subcmd, "rx") == 0) {
            direction = ESTAT_DIR_INGRESS;
        } else if (sal_strcasecmp(subcmd, "tx") == 0) {
            direction = ESTAT_DIR_EGRESS;
        } else {
           cli_out("%s: Error: Invalid option %s\n", ARG_CMD(a), subcmd);
           return CMD_FAIL;
        }
    }

    event_id = ARG_GET(a);
    if (cmd_type == ESTAT_CMD_SHOW) {
        if (direction == ESTAT_DIR_BOTH) {
            rv = ltsw_estat_show(unit, show_zero, event_type, ESTAT_DIR_INGRESS,
                                 event_id);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: RX ltsw_estat_show() failed(%s).\n",
                        shr_errmsg(rv));
                return CMD_FAIL;
            }
            rv = ltsw_estat_show(unit, show_zero, event_type, ESTAT_DIR_EGRESS,
                                 event_id);
            if (SHR_FAILURE(rv)) {
            cli_out("Error: TX ltsw_estat_show() failed(%s).\n",
                    shr_errmsg(rv));
                return CMD_FAIL;
            }
        } else {
            rv = ltsw_estat_show(unit, show_zero, event_type, direction,
                                 event_id);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: %s ltsw_estat_show() failed(%s).\n",
                        direction == ESTAT_DIR_INGRESS ? "RX" : "TX",
                        shr_errmsg(rv));
                return CMD_FAIL;
            }
        }
    } else {
        if (direction == ESTAT_DIR_BOTH) {
            rv = ltsw_estat_clear(unit, event_type, ESTAT_DIR_INGRESS, event_id);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: RX ltsw_estat_clear() failed(%s).\n",
                        shr_errmsg(rv));
                return CMD_FAIL;
            }
            rv = ltsw_estat_clear(unit, event_type, ESTAT_DIR_EGRESS, event_id);
            if (SHR_FAILURE(rv)) {
                cli_out("Error: TX ltsw_estat_clear() failed(%s).\n",
                        shr_errmsg(rv));
                return CMD_FAIL;
            }
        } else {
            rv = ltsw_estat_clear(unit, event_type, direction, event_id);
            if (SHR_FAILURE(rv)) {
                cli_out("%s Error: ltsw_estat_clear() failed(%s).\n",
                        direction == ESTAT_DIR_INGRESS ? "RX" : "TX",
                        shr_errmsg(rv));
                return CMD_FAIL;
            }
        }
    }

    return CMD_OK;
}

#endif /* BCM_LTSW_SUPPORT */
