/*! \file bcm56780_a0_ina_2_4_13_rx.c
 *
 * BCM56780_A0 INA_2_4_13 RX driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/rx.h>
#include <bcm_int/ltsw/rx.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_RX


/*
 * \brief String map for TM_COS_Q_CPU_MAP_INFO_ID_T.
 */
static const
bcmint_rx_str_map_t tm_cos_q_cpu_map_info_id_type_map[] = {
    {
        .idx = BCMINT_TM_BASE_VIEW,
        .str = TM_BASE_VIEWs,
        .key0 = NOT_USEDs,
        .ifp_opaque_ctrl_id = ING_CMD_BUS_IFP_OPAQUE_CTRL_IDs,
        .int_pri = ING_CMD_BUS_INT_PRIs,
    },
};

/*!
 * \brief rx string map database.
 */
static const
bcmint_rx_str_map_db_t str_map_db[] = {
    {
        .hdl = BCMINT_TM_COS_Q_CPU_MAP_INFO_ID,
        .info = tm_cos_q_cpu_map_info_id_type_map,
        .num = 1
    },
};

static bcmint_rx_drop_event_convert_t rx_drop_event_to_code[] =
{
    {bcmPktDropEventIngressNoDrop,                           rxPktDropCodeNoDrop},
    {bcmPktDropEventIngressCmlFlagsDrop,                     rxPktDropCodeCmlFlagsDrop},
    {bcmPktDropEventIngressL2SrcStaticMove,                  rxPktDropCodeL2SrcStaticMove},
    {bcmPktDropEventIngressL2SrcDiscard,                     rxPktDropCodeL2SrcDiscard},
    {bcmPktDropEventIngressMacsaMulticast,                   rxPktDropCodeMacsaMulticast},
    {bcmPktDropEventIngressOuterTpidCheckFailed,             rxPktDropCodeOuterTpidCheckFailed},
    {bcmPktDropEventIngressIncomingPvlanCheckFailed,         rxPktDropCodeIncomingPvlanCheckFailed},
    {bcmPktDropEventIngressPktIntegrityCheckFailed,          rxPktDropCodePktIntegrityCheckFailed},
    {bcmPktDropEventIngressProtocolPktDrop,                  rxPktDropCodeProtocolPktDrop},
    {bcmPktDropEventIngressMembershipCheckFailed,            rxPktDropCodeMembershipCheckFailed},
    {bcmPktDropEventIngressSpanningTreeCheckFailed,          rxPktDropCodeSpanningTreeCheckFailed},
    {bcmPktDropEventIngressL2DstLookupMiss,                  rxPktDropCodeL2DstLookupMiss},
    {bcmPktDropEventIngressL2DstDiscard,                     rxPktDropCodeL2DstDiscard},
    {bcmPktDropEventIngressL3DstLookupMiss,                  rxPktDropCodeL3DstLookupMiss},
    {bcmPktDropEventIngressL3DstDiscard,                     rxPktDropCodeL3DstDiscard},
    {bcmPktDropEventIngressL3HdrError,                       rxPktDropCodeL3HdrError},
    {bcmPktDropEventIngressL3TtlError,                       rxPktDropCodeL3TtlError},
    {bcmPktDropEventIngressIpmcL3IifOrRpaIdCheckFailed,      rxPktDropCodeIpmcL3IifOrRpaIdCheckFailed},
    {bcmPktDropEventIngressTunnelTtlCheckFailed,             rxPktDropCodeTunnelTtlCheckFailed},
    {-1,                                                     rxPktDropCodeTunnelShimHdrError},
    {bcmPktDropEventIngressTunnelObjectValidationFailed,     rxPktDropCodeTunnelObjectValidationFailed},
    {bcmPktDropEventIngressTunnelAdaptDrop,                  rxPktDropCodeTunnelAdaptDrop},
    {bcmPktDropEventIngressPvlanDrop,                        rxPktDropCodePvlanDrop},
    {bcmPktDropEventIngressVfp,                              rxPktDropCodeVfp},
    {bcmPktDropEventIngressIfp,                              rxPktDropCodeIfp},
    {bcmPktDropEventIngressIfpMeter,                         rxPktDropCodeIfpMeter},
    {bcmPktDropEventIngressDstFp,                            rxPktDropCodeDstFp},
    {bcmPktDropEventIngressMplsProtectionDrop,               rxPktDropCodeMplsProtectionDrop},
    {bcmPktDropEventIngressMplsLabelActionInvalid,           rxPktDropCodeMplsLabelActionInvalid},
    {bcmPktDropEventIngressMplsTermPolicySelectTableDrop,    rxPktDropCodeMplsTermPolicySelectTableDrop},
    {bcmPktDropEventIngressMplsReservedLabelExposed,         rxPktDropCodeMplsReservedLabelExposed},
    {bcmPktDropEventIngressMplsTtlError,                     rxPktDropCodeMplsTtlError},
    {bcmPktDropEventIngressMplsEcnError,                     rxPktDropCodeMplsEcnError},
    {bcmPktDropEventIngressEmFt,                             rxPktDropCodeEmFt},
    {bcmPktDropEventIngressIvxlt,                            rxPktDropCodeIvxlt},
    {bcmPktDropEventIngressUrpfCheckFailed,                  rxPktDropCodeUrpfCheckFailed},
    {bcmPktDropEventIngressSrcPortKnockoutDrop,              rxPktDropCodeSrcPortKnockoutDrop},
    {bcmPktDropEventIngressLagFailoverPortDown,              rxPktDropCodeLagFailoverPortDown},
    {bcmPktDropEventIngressSplitHorizonCheckFailed,          rxPktDropCodeSplitHorizonCheckFailed},
    {bcmPktDropEventIngressDstLinkDown,                      rxPktDropCodeDstLinkDown},
    {bcmPktDropEventIngressBlockMaskDrop,                    rxPktDropCodeBlockMaskDrop},
    {bcmPktDropEventIngressL3MtuCheckFailed,                 rxPktDropCodeL3MtuCheckFailed},
    {bcmPktDropEventIngressSeqNumCheckFailed,                rxPktDropCodeSeqNumCheckFailed},
    {bcmPktDropEventIngressL3IifEqL3Oif,                     rxPktDropCodeL3IifEqL3Oif},
    {bcmPktDropEventIngressStormControlDrop,                 rxPktDropCodeStormControlDrop},
    {bcmPktDropEventIngressEgrMembershipCheckFailed,         rxPktDropCodeEgrMembershipCheckFailed},
    {bcmPktDropEventIngressEgrSpanningTreeCheckFailed,       rxPktDropCodeEgrSpanningTreeCheckFailed},
    {bcmPktDropEventIngressDstPbmZero,                       rxPktDropCodeDstPbmZero},
    {bcmPktDropEventIngressMplsCtrlPktDrop,                  rxPktDropCodeMplsCtrlPktDrop},
};

/*
 * Drop Event definitions which are available for copy to cpu usage.
 */

static bcmint_rx_trace_event_convert_t rx_trace_event_to_code[] =
{
    {-1,                                                    rxPktTraceCodeNoCopyToCpu},
    {bcmPktTraceEventIngressCmlFlags,                       rxPktTraceCodeCmlFlags},
    {bcmPktTraceEventIngressL2SrcStaticMove,                rxPktTraceCodeL2SrcStaticMove},
    {bcmPktTraceEventIngressL2SrcDiscard,                   rxPktTraceCodeL2SrcDiscard},
    {bcmPktTraceEventIngressMacsaMulticast,                 rxPktTraceCodeMacsaMulticast},
    {bcmPktTraceEventIngressPktIntegrityCheckFailed,        rxPktTraceCodePktIntegrityCheckFailed},
    {bcmPktTraceEventIngressProtocolPkt,                    rxPktTraceCodeProtocolPkt},
    {bcmPktTraceEventIngressMembershipCheckFailed,          rxPktTraceCodeMembershipCheckFailed},
    {bcmPktTraceEventIngressSpanningTreeCheckFailed,        rxPktTraceCodeSpanningTreeCheckFailed},
    {bcmPktTraceEventIngressL2DstLookupMiss,                rxPktTraceCodeL2DstLookupMiss},
    {bcmPktTraceEventIngressL2DstLookup,                    rxPktTraceCodeL2DstLookup},
    {bcmPktTraceEventIngressL3DstLookupMiss,                rxPktTraceCodeL3DstLookup_miss},
    {bcmPktTraceEventIngressL3DstLookup,                    rxPktTraceCodeL3DstLookup},
    {bcmPktTraceEventIngressL3HdrError,                     rxPktTraceCodeL3HdrError},
    {bcmPktTraceEventIngressL3TtlError,                     rxPktTraceCodeL3TtlError},
    {bcmPktTraceEventIngressIpmcL3IifOrRpaIdCheckFailed,    rxPktTraceCodeIpmcL3IifOrRpaIdCheckFailed},
    {bcmPktTraceEventIngressLearnCacheFull,                 rxPktTraceCodeLearnCacheFull},
    {bcmPktTraceEventIngressVfp,                            rxPktTraceCodeVfp},
    {bcmPktTraceEventIngressIfp,                            rxPktTraceCodeIfp},
    {bcmPktTraceEventIngressIfpMeter,                       rxPktTraceCodeIfpMeter},
    {bcmPktTraceEventIngressDstFp,                          rxPktTraceCodeDstFp},
    {bcmPktTraceEventIngressSvp,                            rxPktTraceCodeSvp},
    {bcmPktTraceEventIngressEmFt,                           rxPktTraceCodeEmFt},
    {bcmPktTraceEventIngressIvxlt,                          rxPktTraceCodeIvxlt},
    {bcmPktTraceEventIngressMirrorSamplerSampled,           rxPktTraceCodeMirrorSamplerSampled},
    {bcmPktTraceEventIngressMirrorSamplerEgrSampled,        rxPktTraceCodeMirrorSamplerEgrSampled},
    {bcmPktTraceEventIngressSerDrop,                        rxPktTraceCodeSerDrop},
    {bcmPktTraceEventIngressUrpfCheckFailed,                rxPktTraceCodeUrpfCheckFailed},
    {bcmPktTraceEventIngressL3IifEqL3Oif,                   rxPktTraceCodeL3IifEqL3Oif},
    {bcmPktTraceEventIngressDlbEcmpMonitorEnOrMemberReassined, rxPktTraceCodeDlbEcmpMonitorEnOrMemberReassined},
    {bcmPktTraceEventIngressDlbLagMonitorEnOrMemberReassined,  rxPktTraceCodeDlbLagMonitorEnOrMemberEeassined},
    {bcmPktTraceEventIngressMplsCtrlPktToCpu,               rxPktTraceCodeMplsCtrlPktToCpu},
    {bcmPktTraceEventIngressMatchedRuleBit0,                rxPktTraceCodeMatchedRuleBit0},
    {bcmPktTraceEventIngressMatchedRuleBit1,                rxPktTraceCodeMatchedRuleBit1},
    {bcmPktTraceEventIngressMatchedRuleBit2,                rxPktTraceCodeMatchedRuleBit2},
    {bcmPktTraceEventIngressMatchedRuleBit3,                rxPktTraceCodeMatchedRuleBit3},
    {bcmPktTraceEventIngressMatchedRuleBit4,                rxPktTraceCodeMatchedRuleBit4},
    {bcmPktTraceEventIngressMatchedRuleBit5,                rxPktTraceCodeMatchedRuleBit5},
    {bcmPktTraceEventIngressMatchedRuleBit6,                rxPktTraceCodeMatchedRuleBit6},
    {bcmPktTraceEventIngressMatchedRuleBit7,                rxPktTraceCodeMatchedRuleBit7},
    {-1,                                                    rxPktTraceCodeTraceDoNotMirror},
    {-1,                                                    rxPktTraceCodeTraceDoNotCopyToCpu},
    {bcmPktTraceEventIngressTraceDop,                       rxPktTraceCodeTraceDop},
};

/*!
 * \brief rx event database.
 */
static const
bcmint_rx_event_db_t rx_event_db[] = {
    {
        .hdl = BCMINT_RX_EVENT_INFO_ID,
        .drop_info = rx_drop_event_to_code,
        .drop_num = COUNTOF(rx_drop_event_to_code),
        .trace_info = rx_trace_event_to_code,
        .trace_num = COUNTOF(rx_trace_event_to_code),
    }
};

/*!
 * \brief rx map database.
 */
static const
bcmint_rx_map_db_t rx_map_db[] = {
    {
        .str_db = str_map_db,
        .str_num = 1,
        .event_db = rx_event_db,
        .event_num = 1,
    }
};

/******************************************************************************
 * Private functions
 */

/*
 * \brief read TM_COS_Q_CPU_MAP_INFO.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_tm_cos_q_cpu_map_info_get(
    int unit,
    const char *info_id,
    bcmint_tm_cos_q_cpu_map_info_t *info)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/ {TM_COS_Q_CPU_MAP_INFO_IDs,   BCMI_LT_FIELD_F_SET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {KEY0s,                       BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2*/ {KEY1s,                       BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*3*/ {KEY2s,                       BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.sym_val = info_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_COS_Q_CPU_MAP_INFOs,
                           &lt_entry, NULL, NULL));

    info->key0 = fields[1].u.sym_val;
    info->key1 = fields[2].u.sym_val;
    info->key2 = fields[3].u.sym_val;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize rx map database.
 *
 * \param [in] unit Unit Number.
 * \param [in|out] map_db rx map database pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_rx_map_db_init(int unit,
                                          const bcmint_rx_map_db_t **map_db)
{
    *map_db = rx_map_db;

    return SHR_E_NONE;
}

/*!
 * \brief RX driver function variable for bcm56780_a0 INA_2_4_13 device.
 */
static mbcm_ltsw_rx_drv_t bcm56780_a0_ina_2_4_13_ltsw_rx_drv = {
    .rx_map_db_init = bcm56780_a0_ina_2_4_13_ltsw_rx_map_db_init,
    .rx_tm_cos_q_cpu_map_info_get = bcm56780_a0_ina_2_4_13_ltsw_tm_cos_q_cpu_map_info_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_rx_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_rx_drv));

exit:
    SHR_FUNC_EXIT();
}

