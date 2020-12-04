/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Tomahawk Instrumentation API
 */

#include <soc/drv.h>

#ifdef BCM_INSTRUMENTATION_SUPPORT

#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/loopback.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <bcm_int/esw/instrumentation.h>
#include <bcm/l2.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/common/multicast.h>
#include <shared/bsl.h>
#include <shared/types.h>
#include <shared/bitop.h>
/* when visibility packet is sent
   following datastructure will be set */
STATIC _bcm_switch_pkt_trace_port_info_t pkt_trace_port_info[BCM_MAX_NUM_UNITS];
STATIC uint8 cpu_pkt_profile_id[BCM_MAX_NUM_UNITS];

#define TH_MAX_PKT_TRACE_CPU_PROFILE_INDEX 0x7

static uint32 ivp_raw_data[BCM_MAX_NUM_UNITS][4];     /* packet trace ivp process result data */
static uint32 isw1_raw_data[BCM_MAX_NUM_UNITS][8];    /* packet trace isw1 process result data */
static uint32 isw2_raw_data[BCM_MAX_NUM_UNITS][16];   /* packet trace isw2 process result data */

/*
 * Function:
 *      bcm_th_pkt_trace_hw_reset
 * Purpose:
 *      clean PTR_RESULTS_BUFFER_IVP/ISW1/ISW2 registers
 *      call this functio before sending a 
 *      visibilty packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_hw_reset(int unit)
{
    uint32                          result_index;
    ptr_results_buffer_ivp_entry_t  ivp_entry;
    ptr_results_buffer_isw1_entry_t isw1_entry;
    ptr_results_buffer_isw2_entry_t isw2_entry;

    memset(&ivp_entry,  0x0, sizeof(ptr_results_buffer_ivp_entry_t));
    memset(&isw1_entry, 0x0, sizeof(ptr_results_buffer_isw1_entry_t));
    memset(&isw2_entry, 0x0, sizeof(ptr_results_buffer_isw2_entry_t));
     
    for (result_index = 0; result_index < TH_PTR_RESULTS_IVP_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_IVPm(unit, MEM_BLOCK_ALL, result_index, &ivp_entry);
    }

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW1_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_ISW1m(unit, MEM_BLOCK_ALL, result_index,&isw1_entry);
    }

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW2_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_ISW2m(unit, MEM_BLOCK_ALL, result_index,&isw2_entry);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_port_set 
 * Purpose:
 *      set the source port of 
 *      visiblity packet
 *      this must be called before visibilty packet 
 *      is sent to set the pipe to read the resuls 
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_src_port_set(int unit, uint32 logical_src_port) 
{
    soc_info_t  *si;
    si = &SOC_INFO(unit);

    pkt_trace_port_info[unit].pkt_trace_src_logical_port = logical_src_port;
    pkt_trace_port_info[unit].pkt_trace_src_pipe =  
                                      si->port_pipe[logical_src_port];

    return 0;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_port_get 
 * Purpose:
 *      get the source port of 
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_th_pkt_trace_src_port_get(int unit) 
{
    return pkt_trace_port_info[unit].pkt_trace_src_logical_port;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_pipe_get 
 * Purpose:
 *      get the ingress pipe of
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_src_pipe_get(int unit) 
{
    return pkt_trace_port_info[unit].pkt_trace_src_pipe;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_stp_state_get
 * Purpose:
 *      change bit mapping of stp state 
 *      and assign to sw stp state 
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      stp_state (INOUT) stp state output.
 *      hw_stp_date (IN). hw stp state bits
 :* Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_th_pkt_trace_stp_state_get(int unit,
                                uint32 *stp_state,
                                uint32  hw_stp_data)
{
    /* Input parameters check. */
    if ((NULL == stp_state)) {
        return (BCM_E_PARAM);
    }

    switch (hw_stp_data) {
      case 0:
          *stp_state = BCM_STG_STP_DISABLE;
          break;
      case 1:
          *stp_state = BCM_STG_STP_BLOCK;
          break;
      case 2:
          *stp_state = BCM_STG_STP_LEARN;
          break;
      case 3:
          *stp_state = BCM_STG_STP_FORWARD;
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th_pkt_trace_drop_reason_abstract
 * Purpose:
 *      abstract visibility packet drop reason,
 * Parameters:
 *      unit - (IN) Unit number.
 *      drop_reason - (OUT) packet drop reason.
 * Returns:
 *      BCM_E_XXX
 */
static
int _bcm_th_pkt_trace_drop_reason_abstract(int unit,
                                           uint32* drop_reason)
{
    typedef struct drop_reason_map_s {
        int32  reason;
        int32  offset;
    } drop_reason_map_t;

    static drop_reason_map_t drop_reason_map[] = {
        {bcmSwitchPktTraceDropReasonVlanNotValid,                 27},
        {bcmSwitchPktTraceDropReasonIngressPortNotInVlanMember,   28},
        {bcmSwitchPktTraceDropReasonTpidMismatch,                 29},
        {bcmSwitchPktTraceDropReasonControlFrame,                 30},
        {bcmSwitchPktTraceDropReasonIpv4ProtocolError,            31},
        {bcmSwitchPktTraceDropReasonHigigLoopback,                32},
        {bcmSwitchPktTraceDropReasonHigigMirrorOnly,              33},
        {bcmSwitchPktTraceDropReasonHigigUnknownHeader,           34},
        {bcmSwitchPktTraceDropReasonHigigUnknownOpcode,           35},
        {bcmSwitchPktTraceDropReasonLagFailLoopback,              37},
        {bcmSwitchPktTraceDropReasonL2SrcEqualL2Dst,              38},
        {bcmSwitchPktTraceDropReasonIpv6ProtocolError,            39},
        {bcmSwitchPktTraceDropReasonNivDot1p,                     42},
        {bcmSwitchPktTraceDropReasonNivVntagPresent,              43},
        {bcmSwitchPktTraceDropReasonNivVntagNotPresent,           44},
        {bcmSwitchPktTraceDropReasonNivVntagFormat,               45},
        {bcmSwitchPktTraceDropReasonTrillErrorFrame,              46},
        {bcmSwitchPktTraceDropReasonMplsLabelLookupMiss,          48},
        {bcmSwitchPktTraceDropReasonMplsInvalidAction,            49},
        {bcmSwitchPktTraceDropReasonMplsInvalidPayload,           50},
        {bcmSwitchPktTraceDropReasonMplsTtlCheckFail,             51},
        {bcmSwitchPktTraceDropReasonMplsInvalidControlWord,       52},
        {bcmSwitchPktTraceDropReasonMplsGalLabel,                 53},
        {bcmSwitchPktTraceDropReasonL2greSipLookupMiss,           54},
        {bcmSwitchPktTraceDropReasonL2greVpnLookupMiss,           55},
        {bcmSwitchPktTraceDropReasonL2greTunnelError,             56},
        {bcmSwitchPktTraceDropReasonVxlanSipLookupMiss,           57},
        {bcmSwitchPktTraceDropReasonVxlanVnidLookupMiss,          58},
        {bcmSwitchPktTraceDropReasonVxlanTunnelError,             59},
        {bcmSwitchPktTraceDropReasonBpdu,                         65},
        {bcmSwitchPktTraceDropReasonProtocolPkt,                  66},
        {bcmSwitchPktTraceDropReasonUnknownSrcMac,                67},
        {bcmSwitchPktTraceDropReasonSrcRoute,                     68},
        {bcmSwitchPktTraceDropReasonL2SrcDiscard,                 69},
        {bcmSwitchPktTraceDropReasonL2SrcStaticMove,              70},
        {bcmSwitchPktTraceDropReasonL2DstDiscard,                 71},
        {bcmSwitchPktTraceDropReasonHigigHdrError,                76},
        {bcmSwitchPktTraceDropReasonSrcMacZero,                   78},
        {bcmSwitchPktTraceDropReasonStageLookup,                  79},
        {bcmSwitchPktTraceDropReasonL2DstMissOrPbt,               80},
        {bcmSwitchPktTraceDropReasonClassBasedStationMove,        81},
        {bcmSwitchPktTraceDropReasonTimeSyncPkt,                  84},
        {bcmSwitchPktTraceDropReasonMyStation,                    88},
        {bcmSwitchPktTraceDropReasonBadUdpChecksum,               89},
        {bcmSwitchPktTraceDropReasonPVlanVpEgressFilter,          94},
        {bcmSwitchPktTraceDropReasonL3DstDiscard,                 106},
        {bcmSwitchPktTraceDropReasonTunnlDecapEcnError,           107},
        {bcmSwitchPktTraceDropReasonIpv4HeaderError,              130},
        {bcmSwitchPktTraceDropReasonIpv6HeaderError,              134},
        {bcmSwitchPktTraceDropReasonStpNotForwarding,             138},
        {bcmSwitchPktTraceDropReasonPortTagPresentError,          139},
        {bcmSwitchPktTraceDropReasonStageIngress,                 141},
        {bcmSwitchPktTraceDropReasonMcastLookupMiss,              142},
        {bcmSwitchPktTraceDropReasonPortBitmapZero,               143},
        {bcmSwitchPktTraceDropReasonMcastIndexError,              156},
        {bcmSwitchPktTraceDropReasonParityError,                  159},
        {bcmSwitchPktTraceDropReasonRpfCheckFail,                 162},
        {-1,                                                      -1},
    };
    uint32 drop_reason_tmp[6] = {0,0,0,0,0,0};
    uint32 index;

    if(drop_reason == NULL) {
        return BCM_E_PARAM;
    }

    SHR_BITCOPY_RANGE(&drop_reason_tmp[0], 27, &(isw1_raw_data[unit][6]), 27, 5);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[1], 0, &(isw1_raw_data[unit][7]), 0, 32);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[2], 0, &(isw2_raw_data[unit][12]), 0, 32);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[3], 0, &(isw2_raw_data[unit][13]), 0, 12);

    /* Definition of COUNTER_UPDATE_VECTOR in INSTR_BUFFER_ISW2_DW7
     * Bit0:   V4 L3 pkt discarded due to resource
     * Bit2:   IPv4 Header Error packets
     * Bit4:   V6 L3 pkt discarded due to resource
     * Bit6:   IPv6 Header Error packets
     * Bit10: pkts dropped as ingress port's SP state is not forwarding
     * Bit11: policy discard such as discard tag/untag pkts, disc. based on SA/DA lookup
     * Bit13: Pkts dropped by FP
     * Bit14: PFM = 2 and M/C Miss
     * Bit15: portbitmap zero drop condition
     * Bit20: DOS L4 header error pkts, applicable only to 10GE ingress ports
     * Bit23: pkts dropped due to MTU check fail
     * Bit25: Received Tunnel pkts with Error
     * Bit26: packets dropped due to VLAN related errors
     * Bit27: packets dropped due to HiGig Hdr errors
     * Bit28: packets dropped due to MC/IPMC index error
     * Bit31: Pkts dropped due to parity error
     * Bit34: packets dropped due to uRPF check failure
     * Bit36: packets dropped due to L2/L3 lookup DST_DISCARD bit
     * Bit45: drop Non-Trill frame on NW port
     * Bit46: drop Trill frame on access port
     * Bit55: ECMP not resolved
     */
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 0, &(isw2_raw_data[unit][14]), 0, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 2, &(isw2_raw_data[unit][14]), 2, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 4, &(isw2_raw_data[unit][14]), 4, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 6, &(isw2_raw_data[unit][14]), 6, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 10, &(isw2_raw_data[unit][14]), 10, 2);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 13, &(isw2_raw_data[unit][14]), 13, 3);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 20, &(isw2_raw_data[unit][14]), 20, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 23, &(isw2_raw_data[unit][14]), 23, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 25, &(isw2_raw_data[unit][14]), 25, 4);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 31, &(isw2_raw_data[unit][14]), 31, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[5], 2, &(isw2_raw_data[unit][15]), 2, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[5], 5, &(isw2_raw_data[unit][15]), 5, 1);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[5], 14, &(isw2_raw_data[unit][15]), 14, 2);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[5], 24, &(isw2_raw_data[unit][15]), 24, 1);

    SHR_BITSET(drop_reason, bcmSwitchPktTraceNoDrop);
    for(index = 0; ; index++) {
        if((drop_reason_map[index].offset == -1) || (drop_reason_map[index].reason == -1)) {
            break;
        }
        if(drop_reason_map[index].offset >= (32*6)) {
            continue;
        }
        if((drop_reason_map[index].reason <= bcmSwitchPktTraceDropReasonInternal) ||
           (drop_reason_map[index].reason >= bcmSwitchPktTraceDropReasonCount)) {
            continue;
        }
        if(SHR_BITGET(drop_reason_tmp, drop_reason_map[index].offset)) {
            SHR_BITSET(drop_reason, drop_reason_map[index].reason);
            SHR_BITCLR(drop_reason, bcmSwitchPktTraceNoDrop);
            SHR_BITCLR(drop_reason_tmp, drop_reason_map[index].offset);
        }
    }

    if(drop_reason_tmp[0] || drop_reason_tmp[1] || drop_reason_tmp[2] ||
       drop_reason_tmp[3] || drop_reason_tmp[4] || drop_reason_tmp[5]) {
        SHR_BITSET(drop_reason, bcmSwitchPktTraceDropReasonInternal);
        SHR_BITCLR(drop_reason, bcmSwitchPktTraceNoDrop);
    }

    return BCM_E_NONE;
}

#ifdef INCLUDE_L3
static
int _bcm_th_pkt_trace_fwd_dst_info_get(int unit,
                                  bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    bcm_port_t port = 0;
    bcm_module_t modid = 0;
    uint32 fwd_dst_type = 0;
    uint32 fwd_dst_field = 0;
    bcm_switch_pkt_trace_fwd_dst_info_t fwd_dst_info = {0};

    SHR_BITCOPY_RANGE(&fwd_dst_type, 0, &(isw2_raw_data[unit][0]), 19, 3);
    SHR_BITCOPY_RANGE(&fwd_dst_field, 0, &(isw2_raw_data[unit][0]), 1, 18);

    switch (fwd_dst_type) {
        case TH_FWD_DEST_TYPE_NH:
            fwd_dst_info.flags = BCM_SWITCH_PKT_TRACE_FWD_DST_NH;
            fwd_dst_info.egress_intf = fwd_dst_field + BCM_XGS3_EGRESS_IDX_MIN(unit);
            break;

        case TH_FWD_DEST_TYPE_L2MC:
            fwd_dst_info.flags = BCM_SWITCH_PKT_TRACE_FWD_DST_L2MC;
            _BCM_MULTICAST_GROUP_SET(fwd_dst_info.mc_group,
                _BCM_MULTICAST_TYPE_L2, fwd_dst_field);
            break;

        case TH_FWD_DEST_TYPE_IPMC:
            fwd_dst_info.flags = BCM_SWITCH_PKT_TRACE_FWD_DST_IPMC;
            _BCM_MULTICAST_GROUP_SET(fwd_dst_info.mc_group,
                _BCM_MULTICAST_TYPE_L3, fwd_dst_field);
            break;

        case TH_FWD_DEST_TYPE_DVP:
            fwd_dst_info.flags = BCM_SWITCH_PKT_TRACE_FWD_DST_DVP;
            _bcm_vp_encode_gport(unit, fwd_dst_field, &fwd_dst_info.dvp);
            break;

        case TH_FWD_DEST_TYPE_DGLP:
            port = fwd_dst_field & TH_FWD_DEST_PORT_MASK;
            modid = (fwd_dst_field >> TH_FWD_DEST_PORT_SHIFT) & TH_FWD_DEST_PORT_MASK;
            fwd_dst_info.flags = BCM_SWITCH_PKT_TRACE_FWD_DST_DGPP;
            BCM_GPORT_MODPORT_SET(fwd_dst_info.port, modid, port);
            break;

        default:
            break;
    }

    pkt_trace_info->pkt_trace_fwd_dst_info = fwd_dst_info;
    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_th_pkt_trace_info_abstract
 * Purpose: 
 *      abstract visibility packet process data from PTR_RESULTS_BUFFER_IVP,
 *      ISW1, and ISW2 stored in bcm_switch_pkt_trace_info_s.
 * Parameters:
 *      IN :  unit
 *      IN:   pkt_trace_info
 * Returns:
 *      BCM_E_XXX
 */
static
int _bcm_th_pkt_trace_info_abstract(int unit,
                                  bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
#define STP_BIT_POS 4
#define STP_BIT_NUM 2
    /* pkt lookup and resolution vars*/
    uint32  my_station_index[1] = {0}, my_station_hit[1] = {0},
            lookup_status[1] = {0}, pkt_rsl_vector[1] = {0},
            ing_stp_state[1] = {0};

#ifdef INCLUDE_L3
    /* hashing information vars */
    uint32  ecmp_rsl_done1[1] = {0}, ecmp_rsl_done2[1] = {0}, 
            ecmp_hash_val1[1] = {0}, ecmp_hash_val2[1] = {0},
            ecmp_group1[1]    = {0}, ecmp_group2[1]    = {0}, 
            ecmp_offset1[1]   = {0}, ecmp_offset2[1]   = {0};
    uint32  ecmp_index, ecmp_ptr;
    int     ecmp_id;
    ecmp_count_entry_t      ecmpc_entry;
    ecmp_entry_t            ecmp_entry;
#endif
    uint32  lag_rsl_done[1]   = {0}, lag_hash_val[1]   = {0},
            lag_id[1]         = {0},  
            hg_rsl_done[1]    = {0}, hg_trunk_id[1]    = {0}, 
            hg_port[1]        = {0}, hg_hash_val[1]    = {0},
            hg_offset[1]      = {0}, lag_offset[1]     = {0};
    /* hashing cal vars */
    int         tgid,  mod_is_local;
    uint32      trunk_base, trunk_group_size;
    uint32      trunk_index, trunk_member_table_index;
    bcm_module_t        mod_id;
    bcm_port_t          port_id;
    bcm_gport_t         dst_member_port;
    _bcm_gport_dest_t   dest;

    trunk_member_entry_t    trunk_member_entry;
    trunk_group_entry_t     tg_entry;
    hg_trunk_member_entry_t hg_trunk_member_entry;
    hg_trunk_group_entry_t  hg_tg_entry;

    bcm_trunk_chip_info_t chip_info;

    /* retrieve my station info */
    SHR_BITCOPY_RANGE(my_station_index, 0, &(ivp_raw_data[unit][0]), 17, 10);
    SHR_BITCOPY_RANGE(my_station_hit, 0, &(ivp_raw_data[unit][0]), 27, 1);

    /* retrieve lookup status */
    SHR_BITCOPY_RANGE(lookup_status, 0, &(isw1_raw_data[unit][6]), 7, 20);

    if (my_station_hit[0]) {
        SHR_BITSET(lookup_status, bcmSwitchPktTraceLookupMystationHit);
    }

    /* fill ingress stp enum*/
    SHR_BITCOPY_RANGE(ing_stp_state, 0, lookup_status, STP_BIT_POS, STP_BIT_NUM);
    BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_stp_state_get(unit, 
                              &(pkt_trace_info->pkt_trace_stp_state),
                                ing_stp_state[0]));

    /* clear stp bits from lookup status */
    SHR_BITCLR(lookup_status, STP_BIT_POS);
    SHR_BITCLR(lookup_status, STP_BIT_POS + STP_BIT_NUM - 1);

    SHR_BITCOPY_RANGE(
        (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap), 0,
         lookup_status, 0, 1);

    SHR_BITCOPY_RANGE(
        (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap), 2,
         lookup_status, 1, 1);

    SHR_BITCOPY_RANGE(
        (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap), 1,
         lookup_status, 2, 1);

    /* fill lookup status enum*/
    SHR_BITCOPY_RANGE(
        (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap), 3,
         lookup_status, 3, bcmSwitchPktTraceLookupCount-2);
  
    /* fill packet resolution vector */
    SHR_BITCOPY_RANGE(pkt_rsl_vector, 0, &(isw2_raw_data[unit][0]), 22, 6);
    pkt_trace_info->pkt_trace_resolution = pkt_rsl_vector[0];

#ifdef INCLUDE_L3
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        /*retrieve hashing information*/
         SHR_BITCOPY_RANGE(ecmp_offset2 , 0, &(isw2_raw_data[unit][2]), 2, 10);
         SHR_BITCOPY_RANGE(ecmp_group2 , 0, &(isw2_raw_data[unit][2]), 12 , 12);
         SHR_BITCOPY_RANGE(ecmp_rsl_done2 , 0, &(isw2_raw_data[unit][2]), 24, 1);

        /* ecmp_hash_val1 = isw2.dw1[26:41] */
         SHR_BITCOPY_RANGE(ecmp_hash_val1 , 0, &(isw2_raw_data[unit][2]), 25 , 7);
         SHR_BITCOPY_RANGE(ecmp_hash_val1 , 7, &(isw2_raw_data[unit][3]), 0, 9);
         SHR_BITCOPY_RANGE(ecmp_offset1, 0, &(isw2_raw_data[unit][3]), 9, 10);
         SHR_BITCOPY_RANGE(ecmp_group1 , 0, &(isw2_raw_data[unit][3]), 19, 12);
         SHR_BITCOPY_RANGE(ecmp_rsl_done1 , 0, &(isw2_raw_data[unit][3]), 31, 1);
         SHR_BITCOPY_RANGE(ecmp_hash_val2 , 0, &(isw2_raw_data[unit][5]), 16, 16);
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
    {
        /*retrieve hashing information*/
         SHR_BITCOPY_RANGE(ecmp_offset2 , 0, &(isw2_raw_data[unit][2]), 4, 10);
         SHR_BITCOPY_RANGE(ecmp_group2 , 0, &(isw2_raw_data[unit][2]), 14 , 11);
         SHR_BITCOPY_RANGE(ecmp_rsl_done2 , 0, &(isw2_raw_data[unit][2]), 25, 1);

        /* ecmp_hash_val1 = isw2.dw1[41:26] */
         SHR_BITCOPY_RANGE(ecmp_hash_val1 , 0, &(isw2_raw_data[unit][2]), 26 , 6);
         SHR_BITCOPY_RANGE(ecmp_hash_val1 , 6, &(isw2_raw_data[unit][3]), 0, 10);
         SHR_BITCOPY_RANGE(ecmp_offset1, 0, &(isw2_raw_data[unit][3]), 10, 10);
         SHR_BITCOPY_RANGE(ecmp_group1 , 0, &(isw2_raw_data[unit][3]), 20, 11);
         SHR_BITCOPY_RANGE(ecmp_rsl_done1 , 0, &(isw2_raw_data[unit][3]), 31, 1);
         SHR_BITCOPY_RANGE(ecmp_hash_val2 , 0, &(isw2_raw_data[unit][5]), 16, 16);
    }
#endif

    /* isw2.dw2[31:0] */
    SHR_BITCOPY_RANGE(lag_offset, 0, &(isw2_raw_data[unit][4]), 1, 10);
    SHR_BITCOPY_RANGE(lag_hash_val , 0, &(isw2_raw_data[unit][4]), 11, 10);
    SHR_BITCOPY_RANGE(lag_id , 0, &(isw2_raw_data[unit][4]) ,21, 10);
    SHR_BITCOPY_RANGE(lag_rsl_done , 0, &(isw2_raw_data[unit][4]), 31, 1);

    /* isw2.dw3[21:0] */
    SHR_BITCOPY_RANGE(hg_offset , 0, &(isw2_raw_data[unit][6]), 0, 5);
    SHR_BITCOPY_RANGE(hg_port , 0, &(isw2_raw_data[unit][6]), 6, 8);
    SHR_BITCOPY_RANGE(hg_trunk_id , 0, &(isw2_raw_data[unit][6]), 14, 7);
    SHR_BITCOPY_RANGE(hg_rsl_done , 0, &(isw2_raw_data[unit][6]),21 , 1);

    /* isw2.dw4[63:54] */
    SHR_BITCOPY_RANGE(hg_hash_val , 0, &(isw2_raw_data[unit][8]), 22, 10);

#ifdef INCLUDE_L3
    if (ecmp_rsl_done1[0]) {
        ecmp_id = ecmp_group1[0];

       SOC_IF_ERROR_RETURN
            (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_id, &ecmpc_entry));

        ecmp_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                  BASE_PTRf);
        ecmp_index = (ecmp_ptr + ecmp_offset1[0]) & (soc_mem_index_count(unit, L3_ECMPm) - 1);
    
        SOC_IF_ERROR_RETURN
            (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_index, &ecmp_entry));
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group   = ecmp_id; 
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group  += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress  = 
                    soc_L3_ECMPm_field32_get(unit, &ecmp_entry, NEXT_HOP_INDEXf); 
        if (ecmp_rsl_done2[0]) {
            pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        } else {
            pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress += BCM_XGS3_EGRESS_IDX_MIN(unit);
        }

        pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_ECMP_1;
    }

    if (ecmp_rsl_done2[0]) {
        ecmp_id = ecmp_group2[0];

       SOC_IF_ERROR_RETURN
            (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_id, &ecmpc_entry));

        ecmp_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                  BASE_PTRf);
        ecmp_index = (ecmp_ptr + ecmp_offset2[0]) & (soc_mem_index_count(unit, L3_ECMPm) - 1);
    
        SOC_IF_ERROR_RETURN
            (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_index, &ecmp_entry));

        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group   = ecmp_id; 
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group  += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress   = 
                    soc_L3_ECMPm_field32_get(unit, &ecmp_entry, NEXT_HOP_INDEXf); 
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress  += BCM_XGS3_EGRESS_IDX_MIN(unit);
        pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_ECMP_2;
    }

#endif        

    if (lag_rsl_done[0]) {
        tgid = lag_id[0]; 

        BCM_IF_ERROR_RETURN(READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tgid, &tg_entry));

        trunk_group_size    = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry, TG_SIZEf);
        trunk_base          = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry, BASE_PTRf);
        trunk_index         = lag_hash_val[0] % (trunk_group_size + 1);
        trunk_member_table_index = (trunk_base + trunk_index) & 0x7ff;

        BCM_IF_ERROR_RETURN(READ_TRUNK_MEMBERm(unit, MEM_BLOCK_ANY, 
                                 trunk_member_table_index, &trunk_member_entry));
        mod_id  = soc_TRUNK_MEMBERm_field32_get(unit, &trunk_member_entry,
                                                MODULE_IDf);
        port_id = soc_TRUNK_MEMBERm_field32_get(unit, &trunk_member_entry,
                                                PORT_NUMf);
       
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_id, port_id,
                                            &(dest.modid), &(dest.port)));        
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, dest.modid,
                                     &mod_is_local));
        if (mod_is_local) {
            if (IS_ST_PORT(unit, dest.port)) {
                dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
            }
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &dst_member_port));

        pkt_trace_info->pkt_trace_hash_info.trunk        = tgid; 
        pkt_trace_info->pkt_trace_hash_info.trunk_member = dst_member_port; 
        pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_TRUNK;

    } /* end lag resolution */

    if (hg_rsl_done[0]) {
        tgid = hg_trunk_id[0]; 

        BCM_IF_ERROR_RETURN(READ_HG_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tgid, &hg_tg_entry));

        trunk_group_size    = soc_HG_TRUNK_GROUPm_field32_get(unit, &hg_tg_entry, TG_SIZEf);
        trunk_base          = soc_HG_TRUNK_GROUPm_field32_get(unit, &hg_tg_entry, BASE_PTRf);
        trunk_index         = hg_hash_val[0] % (trunk_group_size + 1);
        trunk_member_table_index = (trunk_base + trunk_index) & 0xff;

        BCM_IF_ERROR_RETURN(READ_HG_TRUNK_MEMBERm(unit, MEM_BLOCK_ANY,
                            trunk_member_table_index, &hg_trunk_member_entry));
        port_id = soc_HG_TRUNK_MEMBERm_field32_get(unit, 
                            &hg_trunk_member_entry, PORT_NUMf);

        if (BCM_FAILURE(bcm_esw_stk_my_modid_get(unit, &mod_id))) {
            mod_id = 0;
        }
    
        BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_id, port_id,
                                             &(dest.modid), &(dest.port)));
        dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &dst_member_port));

        BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
        pkt_trace_info->pkt_trace_hash_info.fabric_trunk = 
                            tgid + chip_info.trunk_fabric_id_min;
        pkt_trace_info->pkt_trace_hash_info.fabric_trunk_member = 
                            dst_member_port;
        pkt_trace_info->pkt_trace_hash_info.flags += 
                            BCM_SWITCH_PKT_TRACE_FABRIC_TRUNK;

    } /* end lag resolution */

    BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_drop_reason_abstract(unit,
                        pkt_trace_info->pkt_trace_drop_reason));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_info_get
 * Purpose:
 *      read results ptr buffer for
 *      visibility packet process and
 *      store into pkt_trace_info
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_info_get(int unit,
                              bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    ptr_results_buffer_ivp_entry_t        ivp_entry;
    ptr_results_buffer_isw1_entry_t       isw1_entry; 
    ptr_results_buffer_isw2_entry_t       isw2_entry;

    uint8    result_index = 0, prev_index = 0, pipe = 0, trace_result_avail = 0;
    uint32   *dw_first, *dw_second;

    soc_mem_t mem;
    pipe =  pkt_trace_port_info[unit].pkt_trace_src_pipe;
    pkt_trace_info->dest_pipe_num = pipe;

    for (result_index = 0; result_index < TH_PTR_RESULTS_IVP_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IVPm) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IVPm)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &ivp_entry)); 

        dw_first = (uint32*)&ivp_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(ivp_raw_data[unit][result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(ivp_raw_data[unit][(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

        if ((*dw_first != 0) || (*dw_second != 0)) {
            trace_result_avail = 1;
        }  

        sal_memcpy(&(pkt_trace_info->raw_data[result_index*8]), 
                   (uint8*)&ivp_entry, (sizeof(ivp_entry)));

    }/*end of ivp_parsing*/
    prev_index += result_index;
 
    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW1_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW1m) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW1m)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &isw1_entry)); 

        dw_first = (uint32*)&isw1_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(isw1_raw_data[unit][result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(isw1_raw_data[unit][(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

        if ((*dw_first != 0) || (*dw_second != 0)) {
            trace_result_avail = 1;
        }
 
        sal_memcpy(&(pkt_trace_info->raw_data[(prev_index+result_index)*8]), 
                   (uint8*)&isw1_entry, (sizeof(isw1_entry)));
   
    }/*end of isw1_parsing*/
    prev_index += result_index;
 
    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW2_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW2m) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW2m)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &isw2_entry)); 

        dw_first = (uint32*)&isw2_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(isw2_raw_data[unit][result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(isw2_raw_data[unit][(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

        if ((*dw_first != 0) || (*dw_second != 0)) {
            trace_result_avail = 1;
        }

 
        sal_memcpy(&(pkt_trace_info->raw_data[(prev_index+result_index)*8]), 
                   (uint8*)&isw2_entry, (sizeof(isw2_entry)));
   
    }/*end of isw2_parsing*/
  
    if (trace_result_avail == 0) {
        return BCM_E_BUSY;
    }

    pkt_trace_info->raw_data_length = (prev_index+result_index)*8;
    /* abstraction */
    BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_info_abstract(unit, pkt_trace_info));

#ifdef INCLUDE_L3
    BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_fwd_dst_info_get(unit, pkt_trace_info));
#endif /* INCLUDE_L3 */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_cpu_profile_init 
 * Purpose:
 *      pre-configure CPU_PKT_PROFILE_1,2 Register according to
 *      index : drop | ifp  | no learn
 *       0       0      0      0          
 *       1       0      0      1
 *       2       0      1      0
 *       3       0      1      1
 *       4       1      0      0
 *       5       1      0      1
 *       6       1      1      0
 *       7       1      1      1
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_cpu_profile_init(int unit) {
    /* pkt_profile_1 contains control of IFP and Learning */
    uint32  pkt_profile_1_w = 0xCCAA;
    /* pkt_profile_2 contains control of forwarding */
    uint32  pkt_profile_2_w = 0xF0;

    if (SOC_REG_IS_VALID(unit, CPU_PKT_PROFILE_1r)) {
        WRITE_CPU_PKT_PROFILE_1r(unit, pkt_profile_1_w);
    }

    if (SOC_REG_IS_VALID(unit, CPU_PKT_PROFILE_2r)) {
        WRITE_CPU_PKT_PROFILE_2r(unit, pkt_profile_2_w);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_cpu_profile_set
 * Purpose:
 *      given visibility packet behavior flags
 *      setting the profile id for the next visiblity packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_cpu_profile_set(int unit,
                                     uint32 flags)
{
    uint32 negate_flags[1] = {0}, param_flags[1] = {0};
    uint32 max_profile_index[1] = {0}, profile_id[1] = {0};
    param_flags[0] = flags;
    max_profile_index[0] =  TH_MAX_PKT_TRACE_CPU_PROFILE_INDEX;
    SHR_BITNEGATE_RANGE(param_flags, 0, 3, negate_flags); 

    SHR_BITAND_RANGE(negate_flags, max_profile_index, 0, 3,
                     profile_id);    

    cpu_pkt_profile_id[unit] = profile_id[0];
          

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_cpu_profile_get
 * Purpose:
 *      retrieve cpu profile id to be used for
 *      the next visibilty packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_th_pkt_trace_cpu_profile_get(int unit, uint32* profile_id)
{
    if (profile_id == NULL) {
        return BCM_E_INTERNAL;
    }
    *profile_id = cpu_pkt_profile_id[unit];
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_int_lbport_set
 * Purpose:
 *       When SDK performs cpu masquerade, make sure
 *       the following 6 fields are consistent between
 *       lport_table and port_table.
 *       V6L3_ENABLEf, V4L3_ENABLEf, IEEE_802_1AS_ENABLEf,
 *       V4IPMC_ENABLEf, L2GRE_TERMINATION_ALLOWEDf,
 *       VXLAN_TERMINATION_ALLOWEDf.
 *
 * Parameters:
 *      unit    - (IN)  bcm device.
 *      port    - (IN)  port number.
 *      enable   - (IN)  1: enable the sw workaround;
 *                              0: disable the sw workaround.
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_th_pkt_trace_int_lbport_set(int unit, uint8 port, int enable, int *old_values)
{
    int pipe_num, i;
    int loopback_port;
    uint32 values[6];
    int rv = BCM_E_NONE;
    soc_field_t  fields[6] = {
        V6L3_ENABLEf, V4L3_ENABLEf, IEEE_802_1AS_ENABLEf,
        V4IPMC_ENABLEf, L2GRE_TERMINATION_ALLOWEDf, VXLAN_TERMINATION_ALLOWEDf,
    };
    SOC_IF_ERROR_RETURN(
        soc_port_pipe_get(unit, port, &pipe_num));
    loopback_port = soc_loopback_lbport_num_get(unit, pipe_num);
    if (loopback_port == -1) {
        return BCM_E_PARAM;
    }
    if (enable) {
        PORT_LOCK(unit);
        rv = bcm_esw_port_lport_fields_get(unit, port,
                                           LPORT_PROFILE_LPORT_TAB, 6,
                                           fields, values);
        PORT_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        for (i = 0; i < 6; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_get(unit, loopback_port,
                                      fields[i], &old_values[i]));
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, loopback_port, 0,
                                      fields[i], values[i]));
        }
    } else {
        for (i = 0; i < 6; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, loopback_port, 0,
                                      fields[i], old_values[i]));
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_INSTRUMENTATION_SUPPORT */
