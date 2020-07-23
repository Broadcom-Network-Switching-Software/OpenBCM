/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    instrumentation.c
 * Purpose: Tomahawk3 Instrumentation code
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/drv.h>

#if defined(BCM_INSTRUMENTATION_SUPPORT)
#include <soc/mem.h>
#include <soc/format.h>
#include <soc/tomahawk3.h>
#include <bcm/switch.h>
#include <bcm_int/esw/instrumentation.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/stg.h>
#include <shared/bitop.h>

#define TH3_PTR_RESULTS_IADAPT_MAX_INDEX    1
#define TH3_PTR_RESULTS_IPARS_MAX_INDEX     1
#define TH3_PTR_RESULTS_IFWD_MAX_INDEX      4
#define TH3_PTR_RESULTS_ISW_MAX_INDEX       7

#define TH3_IP_STAGE_PER_ENTRY_SIZE  8

#define TH3_IADAPT_RAW_DATA_BYTES   \
            (TH3_PTR_RESULTS_IADAPT_MAX_INDEX * TH3_IP_STAGE_PER_ENTRY_SIZE)
#define TH3_IPARS_RAW_DATA_BYTES    \
            (TH3_PTR_RESULTS_IPARS_MAX_INDEX * TH3_IP_STAGE_PER_ENTRY_SIZE)
#define TH3_IFWD_RAW_DATA_BYTES     \
            (TH3_PTR_RESULTS_IFWD_MAX_INDEX * TH3_IP_STAGE_PER_ENTRY_SIZE)
#define TH3_ISW_RAW_DATA_BYTES      \
            (TH3_PTR_RESULTS_ISW_MAX_INDEX * TH3_IP_STAGE_PER_ENTRY_SIZE)

#define TH3_IPARS_RAW_DATA_OFFSET   0
#define TH3_IADAPT_RAW_DATA_OFFSET  \
        (TH3_IPARS_RAW_DATA_OFFSET + TH3_IPARS_RAW_DATA_BYTES)
#define TH3_IFWD_RAW_DATA_OFFSET    \
        (TH3_IADAPT_RAW_DATA_OFFSET + TH3_IADAPT_RAW_DATA_BYTES)
#define TH3_ISW_RAW_DATA_OFFSET     \
        (TH3_IFWD_RAW_DATA_OFFSET + TH3_IFWD_RAW_DATA_BYTES)

/*
 * Function:
 *     _bcm_th3_pkt_trace_hw_reset
 *
 * Purpose:
 *       clean PTR_RESULTS_BUFFER_IADAPT/IPARS/IFWD/ISW memories
 *       call this function before sending a visibilty packet
 *
 * Parameters:
 *      unit - (IN)  Device Number
 *      pipe - (IN)  Ingress Pipeline Number
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      If 'pipe' parameter is -1, memories for all pipes will
 *      get reset else memory for only that pipe gets reset
 */
int _bcm_th3_pkt_trace_hw_reset(int unit, int pipe)
{
    int         rv = BCM_E_NONE;
    uint32      active_pipes_bmap = 0x0;
    soc_mem_t   mem;

    if (-1 == pipe) {
        SOC_IF_ERROR_RETURN(
            soc_mem_clear(unit, PTR_RESULTS_BUFFER_IPARSm, MEM_BLOCK_ALL, 0));
        SOC_IF_ERROR_RETURN(
            soc_mem_clear(unit, PTR_RESULTS_BUFFER_IADAPTm, MEM_BLOCK_ALL, 0));
        SOC_IF_ERROR_RETURN(
            soc_mem_clear(unit, PTR_RESULTS_BUFFER_IFWDm, MEM_BLOCK_ALL, 0));
        SOC_IF_ERROR_RETURN(
            soc_mem_clear(unit, PTR_RESULTS_BUFFER_ISWm, MEM_BLOCK_ALL, 0));

    } else if ((pipe > -1) && (pipe < SOC_MAX_NUM_PIPES)) {

        /* Get pipe bitmap for active pipes in the system */
        soc_tomahawk3_pipe_map_get(unit, &active_pipes_bmap);

        /* Check if the given pipe is active on the device */
        if (active_pipes_bmap & (1 << pipe)) {
            mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IPARSm)[pipe];
            SOC_IF_ERROR_RETURN(soc_mem_clear(unit, mem, MEM_BLOCK_ALL, 0));

            mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IADAPTm)[pipe];
            SOC_IF_ERROR_RETURN(soc_mem_clear(unit, mem, MEM_BLOCK_ALL, 0));

            mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IFWDm)[pipe];
            SOC_IF_ERROR_RETURN(soc_mem_clear(unit, mem, MEM_BLOCK_ALL, 0));

            mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISWm)[pipe];
            SOC_IF_ERROR_RETURN(soc_mem_clear(unit, mem, MEM_BLOCK_ALL, 0));
        } else {
            /* If the given pipe is not active then return error */
            rv = BCM_E_PARAM;
        }
    } else {
        rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_th3_pkt_trace_drop_reason_decode
 *
 * Purpose:
 *      decode the raw drop reason bits into Application
 *      understandable format
 *
 * Parameters:
 *      unit - (IN)  Device Number
 *      pkt_trace_info - (IN/OUT) User given packet trace structure.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Parameter 'pkt_trace_info' will be used as input to this function
 *      for member 'raw_data' and will be used as an output with respect
 *      to member 'pkt_trace_drop_reason'
 */
static
int _bcm_th3_pkt_trace_drop_reason_decode(int unit,
                                    bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    int     fld_idx;
    int     bit_set_cnt = 0;
    uint32  pre_coun_v = 0;
    uint32  pkt_dp_v[2] = {0};
    uint32  disc_dp_v = 0;
    uint32  rsel_dp_v = 0;
    uint32  tl_dp_v = 0;
    uint32  vlan_dp_v = 0;
    uint32  pre_dp_v = 0;
    uint32  *isw_raw_data =
                (uint32 *)(pkt_trace_info->raw_data + TH3_ISW_RAW_DATA_OFFSET);
    static const soc_field_t dp_flds[] = {
                                           RIPD4f,
                                           RIPHE4f,
                                           RIPD6f,
                                           RIPHE6f,
                                           BLOCK_MASK_DROPf,
                                           MTU_CHECK_FAILf,
                                           PARITYDf,
                                           PDISCf,
                                           RDROPf,
                                           RIMDRf,
                                           RPORTDf,
                                           RTUNEf
                                       };

    typedef struct {
        soc_field_t field;
        uint8       idx_fmt_map;
        uint8       reason;
    } drop_reason_map_t;

    /* Drop reasons TH3 cares to provide to Applications */
    const drop_reason_map_t drop_reason_map[] = {
        { INVALID_VLANf,            0 /* VLAN_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonVlanNotValid              },
        { ENIFILTERf,               0 /* VLAN_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonIngressPortNotInVlanMember},
        { INVALID_TPIDf,            0 /* VLAN_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonTpidMismatch              },
        { IFP_DROPf,                1 /* RSEL_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonStageIngress              },
        { TUNNEL_ERRORf,            1 /* RSEL_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonTunnelError               },
        { BPDUf,                    2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonBpdu                      },
        { TUNNEL_DECAP_ECNf,        2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonTunnlDecapEcnError        },
        { VFPf,                     2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonStageLookup               },
        { CMLf,                     2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonUnknownSrcMac             },
        { L2DST_DISCARDf,           2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonL2DstDiscard              },
        { L2SRC_DISCARDf,           2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonL2SrcDiscard              },
        { L2SRC_STATIC_MOVEf,       2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonL2SrcStaticMove           },
        { L3DST_DISCARDf,           2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonL3DstDiscard              },
        { MACSA0f,                  2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonSrcMacZero                },
        { MULTICAST_INDEX_ERRORf,   2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMcastIndexError           },
        { MY_STATIONf,              2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMyStation                 },
        { PFMf,                     2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMcastLookupMiss           },
        { PROTCOL_PKTf,             2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonProtocolPkt               },
        { SRC_ROUTEf,               2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonSrcRoute                  },
        { TAG_UNTAG_DISCARDf,       2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonPortTagPresentError       },
        { TIME_SYNC_PKTf,           2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonTimeSyncPkt               },
        { VLAN_CC_OR_PBTf,          2 /* PKT_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonL2DstMissOrPbt            },
        { MPLS_LABEL_MISSf,         3 /* TUNNEL_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMplsLabelLookupMiss       },
        { MPLS_INVALID_ACTIONf,     3 /* TUNNEL_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMplsInvalidAction         },
        { MPLS_INVALID_PAYLOADf,    3 /* TUNNEL_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMplsInvalidPayload        },
        { MPLS_INVALID_CWf,         3 /* TUNNEL_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMplsInvalidControlWord    },
        { MPLS_TTL_CHECK_FAILf,     3 /* TUNNEL_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMplsTtlCheckFail          },
        { MPLS_GAL_LABELf,          3 /* TUNNEL_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonMplsGalLabel              },
        { CONTROL_FRAMEf,           4 /* DISCARD_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonControlFrame              },
        { IPV4_PROTOCOL_ERRORf,     4 /* DISCARD_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonIpv4ProtocolError         },
        { IPV6_PROTOCOL_ERRORf,     4 /* DISCARD_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonIpv6ProtocolError         },
        { LAG_FAIL_LOOPBACKf,       4 /* DISCARD_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonLagFailLoopback           },
        { MACSA_EQUALS_DAf,         4 /* DISCARD_PROC_DROP_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonL2SrcEqualL2Dst           },
        { RIPHE4f,                  5 /* PRE_COUNTER_UPDATE_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonIpv4HeaderError           },
        { RIPHE6f,                  5 /* PRE_COUNTER_UPDATE_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonIpv6HeaderError           },
        { RDROPf,                   5 /* PRE_COUNTER_UPDATE_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonPortBitmapZero            },
        { RPORTDf,                  5 /* PRE_COUNTER_UPDATE_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonStpNotForwarding          },
        { PARITYDf,                 5 /* PRE_COUNTER_UPDATE_VECTORfmt */,
            (uint8)bcmSwitchPktTraceDropReasonParityError               }
    };

    typedef struct {
        soc_format_t    fmt;
        void *          fmt_entry;
    } format_map_t;

    format_map_t fmt_map[] = {
        {   VLAN_DROP_VECTORfmt,           NULL },
        {   RSEL_DROP_VECTORfmt,           NULL },
        {   PKT_PROC_DROP_VECTORfmt,       NULL },
        {   TUNNEL_PROC_DROP_VECTORfmt,    NULL },
        {   DISCARD_PROC_DROP_VECTORfmt,   NULL },
        {   PRE_COUNTER_UPDATE_VECTORfmt,  NULL }
    };

    fmt_map[0].fmt_entry = (void *)&vlan_dp_v;
    fmt_map[1].fmt_entry = (void *)&rsel_dp_v;
    fmt_map[2].fmt_entry = (void *)&pkt_dp_v;
    fmt_map[3].fmt_entry = (void *)&tl_dp_v;
    fmt_map[4].fmt_entry = (void *)&disc_dp_v;
    fmt_map[5].fmt_entry = (void *)&pre_dp_v;

    /* Extract the PKT_PROC_DROP_VECTOR from the ISW.DW5 raw data */
    soc_format_field_get(unit,
                         INGRESS_COUNTER_UPDATE_VECTORfmt,
                         isw_raw_data + 10,
                         PKT_PROC_DROP_VECTORf,
                         pkt_dp_v);

    /* Extract the DISCARD_PROC_DROP_VECTOR from PKT_PROC_DROP_VECTOR
     * field and then clear the DISCARD_PROC_DROP_VECTOR from the pkt_dp_v
     */
    disc_dp_v = soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
                                       pkt_dp_v, DISCARD_PROC_DROP_VECTORf);
    soc_format_field32_set(unit, PKT_PROC_DROP_VECTORfmt,
                           pkt_dp_v, DISCARD_PROC_DROP_VECTORf, 0x0);

    /* Extract the RSEL_DROP_VECTOR from PKT_PROC_DROP_VECTOR
     * field and then clear the RSEL_DROP_VECTOR from the pkt_dp_v
     */
    rsel_dp_v = soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
                                       pkt_dp_v, RSEL_DROP_VECTORf);
    soc_format_field32_set(unit, PKT_PROC_DROP_VECTORfmt,
                           pkt_dp_v, RSEL_DROP_VECTORf, 0x0);

    /* Extract the TUNNEL_PROC_DROP_VECTOR from PKT_PROC_DROP_VECTOR
     * field and then clear the TUNNEL_PROC_DROP_VECTOR from the pkt_dp_v
     */
    tl_dp_v = soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
                                     pkt_dp_v, TUNNEL_PROC_DROP_VECTORf);
    soc_format_field32_set(unit, PKT_PROC_DROP_VECTORfmt,
                           pkt_dp_v, TUNNEL_PROC_DROP_VECTORf, 0x0);

    /* Extract the VLAN_DROP_VECTOR from PKT_PROC_DROP_VECTOR
     * field and then clear the VLAN_DROP_VECTOR from the pkt_dp_v
     */
    vlan_dp_v = soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
                                       pkt_dp_v, VLAN_DROP_VECTORf);
    soc_format_field32_set(unit, PKT_PROC_DROP_VECTORfmt,
                           pkt_dp_v, VLAN_DROP_VECTORf, 0x0);

    /* Extract the PRE_COUNTER_UPDATE_VECTORf from ISW Stage buffer */
    pre_coun_v = soc_format_field32_get(unit,
                     INSTR_BUFFER_ISW_DW6fmt,
                     isw_raw_data + 12,
                     PRE_COUNTER_UPDATE_VECTORf);

    /* Extract required drop vector from pre counter vector
     * RIPD4, RIPHE4, RIPD6, RIPHE6, BLOCK_MASK_DROP,
     * MTU_CHECK_FAIL, PARITYD, PDISC, RDROP, RIMDR, RPORTD, RTUNE
     */
    for (fld_idx=0 ; fld_idx < COUNTOF(dp_flds) ; fld_idx++) {
        if(soc_format_field32_get(unit,
           PRE_COUNTER_UPDATE_VECTORfmt, &pre_coun_v, dp_flds[fld_idx])) {
              soc_format_field32_set(unit,
                PRE_COUNTER_UPDATE_VECTORfmt, &pre_dp_v, dp_flds[fld_idx], 0x1);
        }
    }

    /* Clear the drop reason bitmap begin-with */
    sal_memset(pkt_trace_info->pkt_trace_drop_reason,
                0, sizeof(pkt_trace_info->pkt_trace_drop_reason));

    /* Capture the necessary drop reasons and clear the corresponding bit
     * in the source bitmap to indicate that the drop reason has been processed
     */
    for (fld_idx=0 ; fld_idx < COUNTOF(drop_reason_map) ; fld_idx++) {
        if (soc_format_field32_get(unit,
                fmt_map[drop_reason_map[fld_idx].idx_fmt_map].fmt,
                fmt_map[drop_reason_map[fld_idx].idx_fmt_map].fmt_entry,
                drop_reason_map[fld_idx].field))
        {
            SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                       drop_reason_map[fld_idx].reason);
            soc_format_field32_set(unit,
                    fmt_map[drop_reason_map[fld_idx].idx_fmt_map].fmt,
                    fmt_map[drop_reason_map[fld_idx].idx_fmt_map].fmt_entry,
                    drop_reason_map[fld_idx].field,
                    0x0);
        }
    }

    /* If there are more drop reasons which are not yet captured/decoded
     * individually then mark the Internal drop reason too in the bitmap.
     */
    if (tl_dp_v || disc_dp_v || vlan_dp_v ||
         pre_dp_v || pkt_dp_v[0] || pkt_dp_v[1] || rsel_dp_v) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                    bcmSwitchPktTraceDropReasonInternal);
    }

    /* See if the drop reason bitmap has any drop bit set */
    SHR_BITCOUNT_RANGE(pkt_trace_info->pkt_trace_drop_reason,
                       bit_set_cnt, 0, bcmSwitchPktTraceDropReasonCount);

    /* If no error or drop bit is set then mark the bit for No Drop */
    if (0 == bit_set_cnt) {
        SHR_BITSET(
            pkt_trace_info->pkt_trace_drop_reason, bcmSwitchPktTraceNoDrop);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_th3_pkt_trace_raw_data_decode
 *
 * Purpose:
 *     decode the raw data from PTR_RESULTS_BUFFER_IADAPT/IFWD/ISW memories
 *     The memories are not read instead the data is interpreted from the
 *     'raw_data' buffer which already has been filled-in with the entries
 *     from the above mentioned memories.
 *
 * Parameters:
 *     unit - (IN)  Device Number
 *     pkt_trace_info - (IN/OUT) User given packet trace structure.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Parameter 'pkt_trace_info' will be used as input to this function
 *      for member 'raw_data' and will be used as an output with respect
 *      to other members.
 */
static
int _bcm_th3_pkt_trace_raw_data_decode(int unit,
                                  bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    int                     rv = BCM_E_NONE;

    /* pkt lookup and resolution vars*/
    int                     l2x_index = -1;
    uint32                  lookup_status;
    uint32                  ing_stp_state;
    l2x_entry_t             l2x_entry;

    /* Trunk/LAG related vars */
    int                     tgid;
    uint32                  lag_hash_val;
    uint32                  trunk_group_size;
    uint32                  trunk_member_index;
    _bcm_gport_dest_t       dest;
    fast_trunk_size_entry_t trunk_size_entry;

#ifdef INCLUDE_L3
    /* ECMP hashing information vars */
    int                     ecmp_id;
    uint32                  ecmp_ptr;
    uint32                  ecmp_index;
    ecmp_entry_t            ecmp_entry;
    ecmp_count_entry_t      ecmpc_entry;
    uint32                  ecmp_offset;
    uint32                  ecmp_rsl_done1;
    uint32                  ecmp_rsl_done2;
#endif

    /* get hold of the respective pipeline stage data */
    uint32 *offset_isw_data;
    uint32 *iadapt_raw_data =
        (uint32 *)(pkt_trace_info->raw_data + TH3_IADAPT_RAW_DATA_OFFSET);
    uint32 *ifwd_raw_data =
        (uint32 *)(pkt_trace_info->raw_data + TH3_IFWD_RAW_DATA_OFFSET);
    uint32 *isw_raw_data =
        (uint32 *)(pkt_trace_info->raw_data + TH3_ISW_RAW_DATA_OFFSET);

    /* retrieve lookup status */
    lookup_status =
        soc_format_field32_get(
            unit, INSTR_BUFFER_IFWD_DW3fmt, ifwd_raw_data + 6, LOOKUP_STATUSf);

    /* fill ingress stp enum*/
    ing_stp_state =
        soc_format_field32_get(unit,
            IFP_LOOKUP_STATUS_VECTORfmt, &lookup_status, INGRESS_STG_STATEf);
    BCM_IF_ERROR_RETURN(_bcm_stg_pvp_translate(
        unit, ing_stp_state, (int *)&(pkt_trace_info->pkt_trace_stp_state)));

    /* fill in the lookup status bit vector */
    if (soc_format_field32_get(
         unit, IFP_LOOKUP_STATUS_VECTORfmt, &lookup_status, VALID_VLAN_IDf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
           pkt_trace_status_bitmap, bcmSwitchPktTraceLookupForwardingVlanValid);
    }

    if (soc_format_field32_get(
         unit, INSTR_BUFFER_IFWD_DW0fmt, ifwd_raw_data, L2LU_SRC_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
           pkt_trace_status_bitmap, bcmSwitchPktTraceLookupL2SrcHit);

        /* Extract the index into L2X table for later use */
        l2x_index = soc_format_field32_get(
                unit, INSTR_BUFFER_IFWD_DW0fmt, ifwd_raw_data, L2LU_SRC_INDEXf);
    }

    if (-1 != l2x_index) {
        soc_mem_lock(unit, L2Xm);
        rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, l2x_index, &l2x_entry);
        soc_mem_unlock(unit, L2Xm);

        SOC_IF_ERROR_RETURN(rv);

        if(soc_L2Xm_field32_get(unit, &l2x_entry, BASE_VALIDf)) {
            if(soc_L2Xm_field32_get(unit, &l2x_entry, STATIC_BITf)) {
                SHR_BITSET(pkt_trace_info->
                    pkt_trace_lookup_status.pkt_trace_status_bitmap,
                    bcmSwitchPktTraceLookupL2SrcStatic);
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    }

    if (soc_format_field32_get(
         unit, INSTR_BUFFER_IFWD_DW0fmt, ifwd_raw_data, L2LU_DST_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
           pkt_trace_status_bitmap, bcmSwitchPktTraceLookupL2DstHit);
    }

    /* bcmSwitchPktTraceLookupL2CacheHit: Not available in TH3 */

    if (soc_format_field32_get(unit,
         INSTR_BUFFER_IFWD_DW1fmt, ifwd_raw_data + 2, L3_ENTRY_SRC_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
           pkt_trace_status_bitmap, bcmSwitchPktTraceLookupL3SrcHostHit);
    }

    if (soc_format_field32_get(unit,
         INSTR_BUFFER_IFWD_DW1fmt, ifwd_raw_data + 2, L3_ENTRY_DST_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
           pkt_trace_status_bitmap, bcmSwitchPktTraceLookupL3DestHostHit);
    }

    if (soc_format_field32_get(
         unit, INSTR_BUFFER_IFWD_DW2fmt, ifwd_raw_data + 4, LPM_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
           pkt_trace_status_bitmap, bcmSwitchPktTraceLookupL3DestRouteHit);
    }

    /* bcmSwitchPktTraceLookupL2SrcMiss: Not available in TH3 */

    if (soc_format_field32_get(
         unit, IFP_LOOKUP_STATUS_VECTORfmt, &lookup_status, DOS_ATTACKf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
                   pkt_trace_status_bitmap, bcmSwitchPktTraceLookupDosAttack);
    }

    if (soc_format_field32_get(
         unit, IFP_LOOKUP_STATUS_VECTORfmt, &lookup_status, L3_TUNNEL_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
                  pkt_trace_status_bitmap, bcmSwitchPktTraceLookupIpTunnelHit);
    }

    if (soc_format_field32_get(unit, IFP_LOOKUP_STATUS_VECTORfmt,
                    &lookup_status, MPLS_ENTRY_TABLE_LOOKUP_0_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
                 pkt_trace_status_bitmap, bcmSwitchPktTraceLookupMplsLabel1Hit);
    }

    if (soc_format_field32_get(unit, IFP_LOOKUP_STATUS_VECTORfmt,
                    &lookup_status, MPLS_ENTRY_TABLE_LOOKUP_1_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
                 pkt_trace_status_bitmap, bcmSwitchPktTraceLookupMplsLabel2Hit);
    }

    if (soc_format_field32_get(unit,
         IFP_LOOKUP_STATUS_VECTORfmt, &lookup_status, MPLS_BOS_TERMINATEDf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
                pkt_trace_status_bitmap, bcmSwitchPktTraceLookupMplsTerminated);
    }

    if (soc_format_field32_get(
         unit, INSTR_BUFFER_IADAPT_DW0fmt, iadapt_raw_data, MY_STATION_HITf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_lookup_status.
                pkt_trace_status_bitmap, bcmSwitchPktTraceLookupMystationHit);
    }

    /* fill packet resolution vector */
    pkt_trace_info->pkt_trace_resolution =
        soc_format_field32_get(unit, INSTR_BUFFER_ISW_DW0fmt,
                                isw_raw_data, PKT_RESOLUTION_VECTORf);

#ifdef INCLUDE_L3
    /* Retrieve the ECMP member resolution details */
    offset_isw_data = isw_raw_data + 2;
    ecmp_rsl_done1 = soc_format_field32_get(unit,
            INSTR_BUFFER_ISW_DW1fmt, offset_isw_data, ECMP_RESOLUTION_DONE1f);
    ecmp_rsl_done2 = soc_format_field32_get(unit,
            INSTR_BUFFER_ISW_DW1fmt, offset_isw_data, ECMP_RESOLUTION_DONE2f);
    if (ecmp_rsl_done1) {
        ecmp_id = soc_format_field32_get(unit,
                    INSTR_BUFFER_ISW_DW1fmt, offset_isw_data, ECMP_GROUP1f);

        SOC_IF_ERROR_RETURN
            (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_id, &ecmpc_entry));

        ecmp_ptr =
            soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry, BASE_PTRf);

        ecmp_offset = soc_format_field32_get(unit,
                    INSTR_BUFFER_ISW_DW1fmt, offset_isw_data, ECMP_OFFSET1f);

        ecmp_index = (ecmp_ptr + ecmp_offset) &
                        (soc_mem_index_count(unit, L3_ECMPm) - 1);

        SOC_IF_ERROR_RETURN
            (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_index, &ecmp_entry));
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group = ecmp_id;
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group +=
                                                BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

        pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress =
                soc_L3_ECMPm_field32_get(unit, &ecmp_entry, NEXT_HOP_INDEXf);
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress +=
            ecmp_rsl_done2 ?
                BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit) : BCM_XGS3_EGRESS_IDX_MIN(unit);

        pkt_trace_info->pkt_trace_hash_info.flags +=BCM_SWITCH_PKT_TRACE_ECMP_1;
    }

    if (ecmp_rsl_done2) {
        ecmp_id = soc_format_field32_get(unit,
                    INSTR_BUFFER_ISW_DW1fmt, offset_isw_data, ECMP_GROUP2f);

        SOC_IF_ERROR_RETURN
            (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_id, &ecmpc_entry));

        ecmp_ptr =
            soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry, BASE_PTRf);

        offset_isw_data = isw_raw_data + 4;
        ecmp_offset = soc_format_field32_get(unit,
                    INSTR_BUFFER_ISW_DW2fmt, offset_isw_data, ECMP_OFFSET2f);

        ecmp_index = (ecmp_ptr + ecmp_offset) &
                        (soc_mem_index_count(unit, L3_ECMPm) - 1);

        SOC_IF_ERROR_RETURN
            (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_index, &ecmp_entry));

        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group = ecmp_id;
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group +=
                                                BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

        pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress =
                soc_L3_ECMPm_field32_get(unit, &ecmp_entry, NEXT_HOP_INDEXf);
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress +=
                                                        BCM_XGS3_EGRESS_IDX_MIN(unit);
        pkt_trace_info->pkt_trace_hash_info.flags +=BCM_SWITCH_PKT_TRACE_ECMP_2;
    }
#endif

    /* Decode LAG related fields */
    offset_isw_data = isw_raw_data + 4;
    if (soc_format_field32_get(unit,
         INSTR_BUFFER_ISW_DW2fmt, offset_isw_data, LAG_RESOLUTION_DONEf)) {

        /* Get the LAG ID */
        tgid = soc_format_field32_get(
                    unit, INSTR_BUFFER_ISW_DW2fmt, offset_isw_data, LAG_IDf);

        /* Get the number of Trunk members */
        BCM_IF_ERROR_RETURN(
           READ_FAST_TRUNK_SIZEm(unit, MEM_BLOCK_ANY, tgid, &trunk_size_entry));
        trunk_group_size    =
            soc_FAST_TRUNK_SIZEm_field32_get(unit, &trunk_size_entry, TG_SIZEf);

        /* Extract the member determination hash */
        lag_hash_val = soc_format_field32_get(unit,
                    INSTR_BUFFER_ISW_DW2fmt, offset_isw_data, HASH_VALUE_LAGf);

        /* Derive the Trunk member from Hash and Trunk group size */
        trunk_member_index  = (lag_hash_val % (trunk_group_size + 1)) & 0x3F;

        /* Determine the Port number from the Trunk member index */
        if (trunk_member_index < 32) {
            fast_trunk_ports_1_entry_t  trunk_entry;
             static const soc_field_t _th3_fast_port_fld[32] = {
                TRUNK_MEMBER_0f,  TRUNK_MEMBER_1f,  TRUNK_MEMBER_2f,
                TRUNK_MEMBER_3f,  TRUNK_MEMBER_4f,  TRUNK_MEMBER_5f,
                TRUNK_MEMBER_6f,  TRUNK_MEMBER_7f,  TRUNK_MEMBER_8f,
                TRUNK_MEMBER_9f,  TRUNK_MEMBER_10f, TRUNK_MEMBER_11f,
                TRUNK_MEMBER_12f, TRUNK_MEMBER_13f, TRUNK_MEMBER_14f,
                TRUNK_MEMBER_15f, TRUNK_MEMBER_16f, TRUNK_MEMBER_17f,
                TRUNK_MEMBER_18f, TRUNK_MEMBER_19f, TRUNK_MEMBER_20f,
                TRUNK_MEMBER_21f, TRUNK_MEMBER_22f, TRUNK_MEMBER_23f,
                TRUNK_MEMBER_24f, TRUNK_MEMBER_25f, TRUNK_MEMBER_26f,
                TRUNK_MEMBER_27f, TRUNK_MEMBER_28f, TRUNK_MEMBER_29f,
                TRUNK_MEMBER_30f, TRUNK_MEMBER_31f
            };
            BCM_IF_ERROR_RETURN(
             READ_FAST_TRUNK_PORTS_1m(unit, MEM_BLOCK_ANY, tgid, &trunk_entry));
            dest.port = soc_FAST_TRUNK_PORTS_1m_field32_get(
                    unit, &trunk_entry, _th3_fast_port_fld[trunk_member_index]);
        } else {
            fast_trunk_ports_2_entry_t  trunk_entry;
            static const soc_field_t _th3_fast_port_fld[32] = {
                TRUNK_MEMBER_32f, TRUNK_MEMBER_33f, TRUNK_MEMBER_34f,
                TRUNK_MEMBER_35f, TRUNK_MEMBER_36f, TRUNK_MEMBER_37f,
                TRUNK_MEMBER_38f, TRUNK_MEMBER_39f, TRUNK_MEMBER_40f,
                TRUNK_MEMBER_41f, TRUNK_MEMBER_42f, TRUNK_MEMBER_43f,
                TRUNK_MEMBER_44f, TRUNK_MEMBER_45f, TRUNK_MEMBER_46f,
                TRUNK_MEMBER_47f, TRUNK_MEMBER_48f, TRUNK_MEMBER_49f,
                TRUNK_MEMBER_50f, TRUNK_MEMBER_51f, TRUNK_MEMBER_52f,
                TRUNK_MEMBER_53f, TRUNK_MEMBER_54f, TRUNK_MEMBER_55f,
                TRUNK_MEMBER_56f, TRUNK_MEMBER_57f, TRUNK_MEMBER_58f,
                TRUNK_MEMBER_59f, TRUNK_MEMBER_60f, TRUNK_MEMBER_61f,
                TRUNK_MEMBER_62f, TRUNK_MEMBER_63f
            };
            BCM_IF_ERROR_RETURN(
             READ_FAST_TRUNK_PORTS_2m(unit, MEM_BLOCK_ANY, tgid, &trunk_entry));
            dest.port = soc_FAST_TRUNK_PORTS_2m_field32_get(
                unit, &trunk_entry, _th3_fast_port_fld[trunk_member_index-32]);
        }

        /* Create a MODPORT GPORT from the local port numberii and save it  */
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &(dest.modid)));
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_construct(unit,
                &dest, &(pkt_trace_info->pkt_trace_hash_info.trunk_member)));

        /* Save the other Trunk information */
        pkt_trace_info->pkt_trace_hash_info.trunk = tgid;
        pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_TRUNK;

    } /* end lag resolution */

    /* Decode the drop reason raw bits */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_pkt_trace_drop_reason_decode(unit, pkt_trace_info));
    return BCM_E_NONE;
}

/*
 * Function:
 *     __bcm_th3_pkt_trace_hw_buffer_read
 *
 * Purpose:
 *     Read the raw data from PTR_RESULTS_BUFFER_IADAPT/IFWD/ISW memories.
 *     Store the read data in its raw format into the given buffer. Also
 *     store the number of bytes written to the buffer if 'bytes_read'
 *     parameter is provided byt the caller.
 *
 * Parameters:
 *     unit - (IN)  Device Number
 *     pipe - (IN)  Ingress pipeline number
 *     buffer - (OUT) buffer where the captured data would be dumped.
 *     bytes_read - (OUT) Stores the number of bytes written to the buffer
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 *      Assumption is that all parameters are valid.
 *      Hence no addtional checks performed on them here.
 */
static int __bcm_th3_pkt_trace_hw_buffer_read(
    int unit, int pipe, uint8 *buffer, uint32 *bytes_read)
{

    uint8       mem_idx;
    uint8       entry_idx;
    uint32      byte_cnt;
    soc_mem_t   mem;

    const struct ptr_results_mem_info_s {
        soc_mem_t   mem;            /* Buffer Memory */
        uint8       max_idx;        /* Max number of entries */
    } mem_info_arr[] = {
        {   PTR_RESULTS_BUFFER_IPARSm,  TH3_PTR_RESULTS_IPARS_MAX_INDEX     },
        {   PTR_RESULTS_BUFFER_IADAPTm, TH3_PTR_RESULTS_IADAPT_MAX_INDEX    },
        {   PTR_RESULTS_BUFFER_IFWDm,   TH3_PTR_RESULTS_IFWD_MAX_INDEX      },
        {   PTR_RESULTS_BUFFER_ISWm,    TH3_PTR_RESULTS_ISW_MAX_INDEX       }
    };

    /* Initialize the bytes read count to 0 at first */
    if (NULL != bytes_read) {
        *bytes_read = 0;
    }

    /* Loop through all the entries of all the Ing. Pipe Stage Buffers */
    for (mem_idx=0, byte_cnt=0 ; mem_idx < COUNTOF(mem_info_arr) ; mem_idx++)
    {
        /* Retrieve the pipe specific memory instance */
        if (NULL == SOC_MEM_UNIQUE_ACC(unit, mem_info_arr[mem_idx].mem)) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, mem_info_arr[mem_idx].mem)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        /* Read the entry from HW tables entry by entry */
        for (entry_idx = 0 ;
             entry_idx < mem_info_arr[mem_idx].max_idx ; entry_idx++) {
            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                             entry_idx, buffer + byte_cnt));

            /* Adjust byte count with the number of bytes already read */
            byte_cnt += TH3_IP_STAGE_PER_ENTRY_SIZE;
        }
    }

    /* Populate the length of the data stored to the buffer */
    if (NULL != bytes_read) {
        *bytes_read = byte_cnt;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_pkt_trace_info_retrieve_data
 *
 * Purpose:
 *     Read the data from PTR_RESULTS_BUFFER_IPARS/IADAPT/IFWD/ISW memories
 *     for the given pipe instance. Store the data in its raw format into the
 *     'raw_data' buffer in given data structure. Further it calls the decode
 *     function to decode some of the raw data.
 *
 *     If pipe parameter is passed as '-1' then all pipe instances for
 *     PTR_RESULTS_BUFFER_XXX memories would be polled for. The first
 *     available valid captured data would be given back to the user.
 *
 * Parameters:
 *     unit - (IN)  Device Number
 *     pipe - (IN)  Ingress pipeline number
 *     pkt_trace_info - (OUT) User given packet trace structure.
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 *   'pipe' parameter shoule be either -1 or range between 0 to 7 (inclusive).
 */
int _bcm_th3_pkt_trace_info_retrieve_data(int unit, int pipe,
                              bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    int     pipe_idx;
    int     min_pipe = pipe;
    int     max_pipe = pipe;
    uint32  byte_cnt = 0;
    uint32  byte_idx = 0;
    uint32  active_pipes_bmap = 0x0;

    /* Check for input parameters validity */
    if ((NULL == pkt_trace_info) || (pipe < -1) || (pipe >= SOC_MAX_NUM_PIPES)) {
        return BCM_E_PARAM;
    }

    /* Get pipe bitmap for the active pipes */
    soc_tomahawk3_pipe_map_get(unit, &active_pipes_bmap);

    /* If pipe is passed in as -1 then poll for data from all pipes */
    if (-1 == pipe) {
        min_pipe = 0;
        max_pipe = SOC_MAX_NUM_PIPES - 1;
    } else if (!(active_pipes_bmap & (1 << pipe))) {
        /* If a pipe is a valid number but it is inactive then return error */
        return BCM_E_PARAM;
    }

    /* Run the loop for the number of pipes */
    for (pipe_idx = min_pipe ; pipe_idx <= max_pipe ; pipe_idx++) {

        /* If the pipe is inactive, skip it */
        if (!(active_pipes_bmap & (1 << pipe_idx))) {
            continue;
        }

        /* Call routine to read the HW PktTrace capture memories for the pipe */
        BCM_IF_ERROR_RETURN(
            __bcm_th3_pkt_trace_hw_buffer_read(
                        unit, pipe_idx, pkt_trace_info->raw_data, &byte_cnt));

        /* See if we got valid data */
        for (byte_idx = 0 ; byte_idx < byte_cnt ; byte_idx += sizeof(uint32) ) {
            /* No need to worry about dword boundary as is is dword-aligned */
            if (*((uint32 *)(pkt_trace_info->raw_data + byte_idx))) {
                break;
            }
        }

        /* If there is valid data then break out the loop to interpret it */
        if (byte_idx < byte_cnt) {
            break;
        }
    }

    /* If no valid data found then mark so */
    if (pipe_idx > max_pipe) {
        return BCM_E_BUSY;
    }

    /* Populate the lenght of the data stored to the buffer */
    pkt_trace_info->raw_data_length = byte_cnt;

    /* Store the pipe information on which the pkt trace has been captured */
    pkt_trace_info->dest_pipe_num = pipe_idx;

    /* Decode the raw data */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_pkt_trace_raw_data_decode(unit, pkt_trace_info));

    return BCM_E_NONE;
}

#endif /* BCM_INSTRUMENTATION_SUPPORT */
#endif /* BCM_TOMAHAWK3_SUPPORT */
