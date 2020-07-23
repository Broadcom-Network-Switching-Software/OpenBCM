/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef BCM_INSTRUMENTATION_SUPPORT

#include <bcm/pkt.h>
#include <bcm/trunk.h>
#include <bcm/stg.h>

#include <appl/diag/system.h>
#include <appl/diag/visibility.h>

/* hex dump without offset header
 * blocks of 8 bytes, 16 bytes per line
 *
 * 0000000000000000 0000000000000000
 */
static void hexdump_8x2(uint8 *raw_data, int len)
{
    int  ri;
    uint8 data[8] = {0};

    for (ri = 0; ri + 16 <= len; ) {        
        sal_memcpy(&data, &raw_data[ri], 8);
        cli_out("%02x%02x%02x%02x%02x%02x%02x%02x ", 
                data[0], data[1], data[2], data[3], 
                data[4], data[5], data[6], data[7]);
        ri += 8;
        sal_memcpy(&data, &raw_data[ri], 8);
        cli_out("%02x%02x%02x%02x%02x%02x%02x%02x\n", 
                data[0], data[1], data[2], data[3], 
                data[4], data[5], data[6], data[7]);
        ri += 8; 
    }
}

/* hex dump with 2 bytes offset header
 * blocks of 4 bytes, 32 bytes per line
 *
 * 0000  00000000 11222222 33333333 44444444  55555555 66666666 77777777
 */
static void hexdump_offset_4x8(uint8 *raw_data, int len)
{
    int  ri;

    for (ri = 0; ri < len; ri++) {
        if (!(ri & 0x1f)) {
            cli_out("\n%04x  ", ri);
        }
        cli_out("%s%s%02x",
                (!(ri & 0x03)) ? " " : "",
                (!(ri & 0x0f)) ? " " : "",
                raw_data[ri]);
    }
    cli_out("\n");
}

typedef struct {
    bcm_switch_pkt_trace_resolution_t resolution;
    char *name;
} abs_resolution_t;

static void abs_resolution_print(int unit, bcm_switch_pkt_trace_resolution_t resolution)
{
    abs_resolution_t abs_resolution_table[] = {
        { bcmSwitchPktTraceResolutionUnkown, "UnknownPkt" },
        { bcmSwitchPktTraceResolutionControlPkt, "ControlPkt" },
        { bcmSwitchPktTraceResolutionOamPkt, "OamPkt" },
        { bcmSwitchPktTraceResolutionBfdPkt, "BfdPkt" },
        { bcmSwitchPktTraceResolutionBpduPkt, "BpduPkt" },
        { bcmSwitchPktTraceResolution1588Pkt, "1588Pkt" },
        { bcmSwitchPktTraceResolutionKnownL2UcPkt, "KnownL2UcPkt" },
        { bcmSwitchPktTraceResolutionUnknownL2UcPkt, "UnknownL2UcPkt" },
        { bcmSwitchPktTraceResolutionKnownL2McPkt, "KnownL2McPkt" },
        { bcmSwitchPktTraceResolutionUnknownL2McPkt, "UnknownL2McPkt" },
        { bcmSwitchPktTraceResolutionL2BcPkt, "L2BcPkt" },
        { bcmSwitchPktTraceResolutionKnownL3UcPkt, "KnownL3UcPkt" },
        { bcmSwitchPktTraceResolutionUnknownL3UcPkt, "UnknownL3UcPkt" },
        { bcmSwitchPktTraceResolutionKnownIpmcPkt, "KnownIpmcPkt" },
        { bcmSwitchPktTraceResolutionUnknownIpmcPkt, "UnknownIpmcPkt" },
        { bcmSwitchPktTraceResolutionKnownMplsL2Pkt, "KnownMplsL2Pkt" },
        { bcmSwitchPktTraceResolutionUnknownMplsPkt, "UnknownMplsPkt" },
        { bcmSwitchPktTraceResolutionKnownMplsL3Pkt, "KnownMplsL3Pkt" },
        { bcmSwitchPktTraceResolutionKnownMplsPkt, "KnownMplsPkt" },
        { bcmSwitchPktTraceResolutionKnownMimPkt, "KnownMimPkt" },
        { bcmSwitchPktTraceResolutionUnknownMimPkt, "UnknownMimPkt" },
        { bcmSwitchPktTraceResolutionKnownTrillPkt, "KnownTrillPkt" },
        { bcmSwitchPktTraceResolutionUnknownTrillPkt, "UnknownTrillPkt" },
        { bcmSwitchPktTraceResolutionKnownNivPkt, "KnownNivPkt" },
        { bcmSwitchPktTraceResolutionUnknownNivPkt, "UnknownNivPkt" },
        { bcmSwitchPktTraceResolutionKnownL2GrePkt, "KnownL2GrePkt" },
        { bcmSwitchPktTraceResolutionKnownVxlanPkt, "KnownVxlanPkt" },
        { bcmSwitchPktTraceResolutionKnownFCOEPkt, "KnownFCOEPkt" },
        { bcmSwitchPktTraceResolutionUnknownFCOEPkt, "UnknownFCOEPkt" },
        { 0, NULL }
    };

    abs_resolution_t *artp = &(abs_resolution_table[0]);
    cli_out("Packet Resolution Result:\n");
    while (1) {
        if (! artp->name) {
            break;
        }
        
        if (resolution == artp->resolution) {
            cli_out("   %s\n", artp->name);
        }
        artp++;
    }
}

typedef struct {
    bcm_switch_pkt_trace_drop_reason_t bit;
    char *name;
} abs_drop_reasons_t;

static void abs_drop_reasons_print(int unit, bcm_switch_pkt_trace_drop_reason_t *dr)
{
    uint32 i = 0;
    abs_drop_reasons_t abs_drop_reasons_table[] = {
        { bcmSwitchPktTraceNoDrop,
          "NoDrop" },
        { bcmSwitchPktTraceDropReasonInternal,
          "Internal" },
        {bcmSwitchPktTraceDropReasonMplsLabelLookupMiss,
          "MplsLabelLookupMiss"},
        {bcmSwitchPktTraceDropReasonMplsInvalidAction,
          "MplsInvalidAction"},
        {bcmSwitchPktTraceDropReasonMplsInvalidPayload,
          "MplsInvalidPayload"},
        {bcmSwitchPktTraceDropReasonMplsTtlCheckFail,
          "MplsTtlCheckFail"},
        {bcmSwitchPktTraceDropReasonMplsInvalidControlWord,
          "MplsInvalidControlWord"},
        {bcmSwitchPktTraceDropReasonL2greSipLookupMiss,
          "L2greSipLookupMiss"},
        {bcmSwitchPktTraceDropReasonL2greVpnLookupMiss,
          "L2greVpnLookupMiss"},
        {bcmSwitchPktTraceDropReasonL2greTunnelError,
          "L2greTunnelError"},
        {bcmSwitchPktTraceDropReasonVxlanSipLookupMiss,
          "VxlanSipLookupMiss"},
        {bcmSwitchPktTraceDropReasonVxlanVnidLookupMiss,
          "VxlanVnidLookupMiss"},
        {bcmSwitchPktTraceDropReasonVxlanTunnelError,
          "VxlanTunnelError"},
        {bcmSwitchPktTraceDropReasonVlanNotValid,
          "VlanNotValid"},
        {bcmSwitchPktTraceDropReasonIngressPortNotInVlanMember,
          "IngressPortNotInVlanMember"},
        {bcmSwitchPktTraceDropReasonTpidMismatch,
          "TpidMismatch"},
        {bcmSwitchPktTraceDropReasonIpv4ProtocolError,
          "Ipv4ProtocolError"},
        {bcmSwitchPktTraceDropReasonHigigLoopback,
          "HigigLoopback"},
        {bcmSwitchPktTraceDropReasonHigigMirrorOnly,
          "HigigMirrorOnly"},
        {bcmSwitchPktTraceDropReasonHigigUnknownHeader,
          "HigigUnknownHeader"},
        {bcmSwitchPktTraceDropReasonHigigUnknownOpcode,
          "HigigUnknownOpcode"},
        {bcmSwitchPktTraceDropReasonLagFailLoopback,
          "LagFailLoopback"},
        {bcmSwitchPktTraceDropReasonL2SrcEqualL2Dst,
          "L2SrcEqualL2Dst"},
        {bcmSwitchPktTraceDropReasonIpv6ProtocolError,
          "Ipv6ProtocolError"},
        {bcmSwitchPktTraceDropReasonNivVntagPresent,
          "NivVntagPresent"},
        {bcmSwitchPktTraceDropReasonNivVntagNotPresent,
          "NivVntagNotPresent"},
        {bcmSwitchPktTraceDropReasonNivVntagFormat,
          "NivVntagFormat"},
        {bcmSwitchPktTraceDropReasonTrillErrorFrame,
          "TrillErrorFrame"},
        {bcmSwitchPktTraceDropReasonBpdu,
          "Bpdu"},
        {bcmSwitchPktTraceDropReasonBadUdpChecksum,
          "BadUdpChecksum"},
        {bcmSwitchPktTraceDropReasonTunnlDecapEcnError,
          "TunnlDecapEcnError"},
        {bcmSwitchPktTraceDropReasonIpv4HeaderError,
          "Ipv4HeaderError"},
        {bcmSwitchPktTraceDropReasonIpv6HeaderError,
          "Ipv6HeaderError"},
        {bcmSwitchPktTraceDropReasonParityError,
          "ParityError"},
        {bcmSwitchPktTraceDropReasonRpfCheckFail,
          "RpfCheckFail"},
        {bcmSwitchPktTraceDropReasonStageLookup,
          "StageLookup"},
        {bcmSwitchPktTraceDropReasonStageIngress,
          "StageIngress"},
        {bcmSwitchPktTraceDropReasonTunnelObjectValidationFail,
          "TunnelObjectValidationFail"},
        {bcmSwitchPktTraceDropReasonTunnelShimHeaderError,
          "TunnelShimHeaderError"},
        {bcmSwitchPktTraceDropReasonTunnelTTLError,
          "TunnelTTLError"},
        {bcmSwitchPktTraceDropReasonTunnelInterfaceCheckFail,
          "TunnelInterfaceCheckFail"},
        {bcmSwitchPktTraceDropReasonTunnelError,
          "TunnelError"},
        {bcmSwitchPktTraceDropReasonTunnelAdapt1LookupMiss,
          "TunnelAdapt1LookupMiss"},
        {bcmSwitchPktTraceDropReasonTunnelAdapt2LookupMiss,
          "TunnelAdapt2LookupMiss"},
        {bcmSwitchPktTraceDropReasonTunnelAdapt3LookupMiss,
          "TunnelAdapt3LookupMiss"},
        {bcmSwitchPktTraceDropReasonTunnelAdapt4LookupMiss,
          "TunnelAdapt4LookupMiss"},
        {bcmSwitchPktTraceDropReasonCount,
          NULL }

    };
    abs_drop_reasons_t *adrtp = &(abs_drop_reasons_table[0]);

    cli_out("Drop Reasons:\n");
    while (1) {
        if (!adrtp->name) {
            break;
        }
        i = (adrtp->bit / BYTES2BITS(sizeof(uint32)));
        if (dr[i] & (1 << (adrtp->bit - (32 * i)))) {
            cli_out("   %s\n", adrtp->name);
        }
        adrtp++;
    }
}

typedef struct {
    bcm_switch_pkt_trace_lookup_t bit;
    char *name;
} abs_lookup_results_t;

static void abs_lookup_results_print(int unit, bcm_switch_pkt_trace_lookup_result_t *lr)
{
    abs_lookup_results_t abs_lookup_results_table[] = {
        { bcmSwitchPktTraceLookupFirstVlanTranslationHit,
          "FirstVlanTranslationHit" },
        { bcmSwitchPktTraceLookupSecondVlanTranslationHit,
          "SecondVlanTranslationHit" },
        { bcmSwitchPktTraceLookupForwardingVlanValid,
          "ForwardingVlanValid" },
        { bcmSwitchPktTraceLookupL2SrcHit,
          "L2SrcHit" },
        { bcmSwitchPktTraceLookupL2SrcStatic,
          "L2SrcStatic" },
        { bcmSwitchPktTraceLookupL2DstHit,
          "L2DstHit" },
        { bcmSwitchPktTraceLookupL2CacheHit,
          "L2CacheHit" },
        { bcmSwitchPktTraceLookupL3SrcHostHit,
          "L3SrcHostHit" },
        { bcmSwitchPktTraceLookupL3DestHostHit,
          "L3DestHostHit" },
        { bcmSwitchPktTraceLookupL3DestRouteHit,
          "L3DestRouteHit" },
        { bcmSwitchPktTraceLookupL2SrcMiss,
          "L2SrcMiss" },
        { bcmSwitchPktTraceLookupDosAttack,
          "DosAttack" },
        { bcmSwitchPktTraceLookupIpTunnelHit,
          "IpTunnelHit" },
        { bcmSwitchPktTraceLookupMplsLabel1Hit,
          "MplsLabel1Hit" },
        { bcmSwitchPktTraceLookupMplsLabel2Hit,
          "MplsLabel2Hit" },
        { bcmSwitchPktTraceLookupMplsTerminated,
          "MplsTerminated" },
        { bcmSwitchPktTraceLookupMystationHit,
          "MystationHit" },
        { bcmSwitchPktTraceLookupInvalid,
          NULL }
    };
    abs_lookup_results_t *alrtp = &(abs_lookup_results_table[0]);

    cli_out("Lookup Status Results:\n");
    while (1) {
        if (! alrtp->name) {
            break;
        }
        
        if (lr->pkt_trace_status_bitmap[0] & (1 << alrtp->bit )) {
            cli_out("   %s\n", alrtp->name);
        }
        alrtp++;
    }
}
 

static void gport_print(bcm_port_t gp)
{
    if (BCM_GPORT_IS_SET(gp)) {
        if (BCM_GPORT_IS_LOCAL(gp)) 
            cli_out("local(%d)\n", BCM_GPORT_LOCAL_GET(gp));
        else if (BCM_GPORT_IS_MODPORT(gp))
            cli_out("modport(%d, %d)\n", 
                   BCM_GPORT_MODPORT_MODID_GET(gp), 
                   BCM_GPORT_MODPORT_PORT_GET(gp));
        else if (BCM_GPORT_IS_TRUNK(gp))
            cli_out("trunk(%d)\n", BCM_GPORT_TRUNK_GET(gp));
        else if (BCM_GPORT_IS_BLACK_HOLE(gp))
            cli_out("blackhole\n");
        else if (BCM_GPORT_IS_LOCAL_CPU(gp))
            cli_out("cpu\n");
    } else if (gp == BCM_GPORT_INVALID) {
        cli_out("Invalid GPORT\n");
    } else {
        cli_out("%d\n", gp);
    }
}

static void fp_trunk_print(int unit, bcm_trunk_t tid)
{
    int rv;
    int i;
    bcm_trunk_info_t tinfo;
    bcm_trunk_member_t member_array[BCM_TRUNK_MAX_PORTCNT];
    int member_count;

    rv = bcm_trunk_get(unit,
                       tid,
                       &tinfo,
                       BCM_TRUNK_MAX_PORTCNT,
                       member_array, 
                       &member_count);

    if (BCM_FAILURE(rv)) {
        cli_out("bcm_trunk_get(unit=%d,tid=%d): rv=%d, %s\n",
                unit,
                tid,
                rv,
                bcm_errmsg(rv));
        return;
    }
    cli_out("   Trunk id: %d\n", tid);
    cli_out("     Rtag: %d\n", tinfo.psc);
    cli_out("     Members: %d ports\n", member_count);
    for (i = 0; i < member_count; i++) {
        cli_out("       "); gport_print(member_array[i].gport);
    }
}

static void abs_hashing_info_print(int unit, bcm_switch_pkt_trace_hashing_info_t *hi)
{
    /* Hashing resolution? */
    if (hi->flags == 0) {
        return;
    }
    if (hi->flags & BCM_SWITCH_PKT_TRACE_ECMP_1) {
        cli_out("Hash resolution: ECMP1 (MULTIPATH)\n");
        cli_out("  ecmp_1_group %d\n", hi->ecmp_1_group);
        cli_out("  > l3 multipath show %d\n", hi->ecmp_1_group);

        cli_out("Egress multipath interface:\n");
        cli_out("  ecmp_1_egress %d\n", hi->ecmp_1_egress);
        cli_out("  > l3 egress show %d\n", hi->ecmp_1_egress);
    }
    if (hi->flags & BCM_SWITCH_PKT_TRACE_ECMP_2) {
        cli_out("Hash resolution: ECMP2 (MULTIPATH)\n");
        cli_out("   ecmp_2_group %d\n", hi->ecmp_2_group);
        cli_out("   > l3 multipath show %d\n", hi->ecmp_2_group);
        cli_out("   ecmp2_egress %d\n", hi->ecmp_2_egress);
    }
    if (hi->flags & BCM_SWITCH_PKT_TRACE_TRUNK) {
        cli_out("Hash resolution: Trunk\n");
        fp_trunk_print(unit, hi->trunk);
        cli_out("     Trunk egress member: "); gport_print(hi->trunk_member);
    }
    if (hi->flags & BCM_SWITCH_PKT_TRACE_FABRIC_TRUNK) {
        cli_out("Hash resolution: Fabric trunk\n");
        cli_out("   Fabric trunk: "); gport_print(hi->fabric_trunk);
        cli_out("   Fabric trunk egress member: "); gport_print(hi->fabric_trunk_member);
    }
}

static void abs_fwd_dst_info_print(int unit,  bcm_switch_pkt_trace_fwd_dst_info_t *fd)
{
    /* Forward destination information*/
    if (fd->flags == 0) {
        return;
    }
    if (fd->flags & BCM_SWITCH_PKT_TRACE_FWD_DST_DGPP) {
        cli_out(" Destination port %x\n", fd->port);
    }
    if (fd->flags & BCM_SWITCH_PKT_TRACE_FWD_DST_DVP) {
        cli_out(" DVP %x\n", fd->dvp);
    }
    if (fd->flags & BCM_SWITCH_PKT_TRACE_FWD_DST_DVP) {
        cli_out(" DVP %d\n", fd->dvp);
    }
    if (fd->flags & BCM_SWITCH_PKT_TRACE_FWD_DST_VLAN) {
        cli_out(" Forwarding VlAN  %d\n", fd->fwd_vlan);
    }
    if (fd->flags & BCM_SWITCH_PKT_TRACE_FWD_DST_L2MC) {
        cli_out(" l2 multicast group %d\n", fd->mc_group);
    }
    if (fd->flags & BCM_SWITCH_PKT_TRACE_FWD_DST_IPMC) {
        cli_out(" l3 multicast group %d\n", fd->mc_group);
    }
    if (fd->flags & BCM_SWITCH_PKT_TRACE_FWD_DST_NH) {
        cli_out("  > l3 egress show %d\n", fd->egress_intf);
    } 
}

static void abs_stp_state_print(int unit, bcm_stg_stp_t stp_state)
{
    char *stp_state_names[] =  {
        /* In Tomahawk, there is no learning */
        "Disable", "Block", "Listen", "Forward", "Forward"
    };

    cli_out("Source port STP state: ");
    if (stp_state > BCM_STG_STP_FORWARD) {
        cli_out(" Invalid STP state\n");
    } else {
        cli_out(" %s\n", stp_state_names[stp_state]);
    }
}

void appl_pkt_trace_info_print(int unit, bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    abs_lookup_results_print(unit, &pkt_trace_info->pkt_trace_lookup_status);
    abs_resolution_print(unit, pkt_trace_info->pkt_trace_resolution);
    abs_stp_state_print(unit, pkt_trace_info->pkt_trace_stp_state);
    abs_hashing_info_print(unit, &pkt_trace_info->pkt_trace_hash_info);
    abs_fwd_dst_info_print(unit, &pkt_trace_info->pkt_trace_fwd_dst_info);
    abs_drop_reasons_print(unit, pkt_trace_info->pkt_trace_drop_reason);
}


int appl_visibility_trace(int unit, int vis_options, int vis_sport, bcm_pkt_t *pkt_info)
{
    int rv;
    uint16 i = 0;
    uint16 print_raw_data = 0;
    bcm_switch_pkt_trace_info_t pkt_trace_info;

    rv = bcm_switch_pkt_trace_info_get(unit,
                                       vis_options,
                                       vis_sport,
                                       pkt_info->pkt_data->len,
                                       pkt_info->pkt_data->data,
                                       &pkt_trace_info);
    cli_out("\n=====PACKET TRACE source_port=%s, %s\n",
            BCM_PORT_NAME(unit, vis_sport),
            bcm_errmsg(rv));
    if (rv == BCM_E_NONE) {
        cli_out("\n=====RAW PACKET");
        hexdump_offset_4x8(pkt_info->pkt_data->data,
                           pkt_info->pkt_data->len);
        cli_out("\n=====RESULTS\n");
        cli_out("Ingress pipeline = %d\n", 
                pkt_trace_info.dest_pipe_num);
        appl_pkt_trace_info_print(unit, &pkt_trace_info);

        for (i = 0; i < BCM_SWITCH_PKT_TRACE_RAW_DATA_MAX; i++) {
           if (pkt_trace_info.raw_data[i]) {
               print_raw_data = 1;
               break;
           }
        }
        if (print_raw_data) {
            cli_out("\n=====RAW DATA\n");
            hexdump_8x2(pkt_trace_info.raw_data,
                    sizeof(pkt_trace_info.raw_data));
        }
    }

    return rv;
}
#endif /* BCM_INSTRUMENTATION_SUPPORT */
