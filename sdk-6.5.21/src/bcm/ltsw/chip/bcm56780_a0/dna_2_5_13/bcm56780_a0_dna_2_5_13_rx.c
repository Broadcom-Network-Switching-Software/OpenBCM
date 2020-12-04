/*! \file bcm56780_a0_dna_2_5_13_rx.c
 *
 * BCM56780_A0 DNA_2_5_13 RX driver.
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
    {bcmPktDropEventIngressNoDrop,                           0},
    {bcmPktDropEventIngressCmlFlagsDrop,                     8},
    {bcmPktDropEventIngressL2SrcStaticMove,                  9},
    {bcmPktDropEventIngressL2SrcDiscard,                    10},
    {bcmPktDropEventIngressMacsaMulticast,                  11},
    {bcmPktDropEventIngressOuterTpidCheckFailed,            12},
    {bcmPktDropEventIngressIncomingPvlanCheckFailed,        13},
    {bcmPktDropEventIngressPktIntegrityCheckFailed,         14},
    {bcmPktDropEventIngressProtocolPktDrop,                 15},
    {bcmPktDropEventIngressMembershipCheckFailed,           16},
    {bcmPktDropEventIngressSpanningTreeCheckFailed,         17},
    {bcmPktDropEventIngressL2DstLookupMiss,                 18},
    {bcmPktDropEventIngressL2DstDiscard,                    19},
    {bcmPktDropEventIngressL3DstLookupMiss,                 20},
    {bcmPktDropEventIngressL3DstDiscard,                    21},
    {bcmPktDropEventIngressL3HdrError,                      22},
    {bcmPktDropEventIngressL3TtlError,                      23},
    {bcmPktDropEventIngressIpmcL3IifOrRpaIdCheckFailed,     24},
    {bcmPktDropEventIngressTunnelTtlCheckFailed,            25},
    {-1,                                                    26},
    {bcmPktDropEventIngressTunnelObjectValidationFailed,    27},
    {bcmPktDropEventIngressTunnelAdaptDrop,                 28},
    {bcmPktDropEventIngressPvlanDrop,                       29},
    {bcmPktDropEventIngressVfp,                             30},
    {bcmPktDropEventIngressIfp,                             31},
    {bcmPktDropEventIngressIfpMeter,                        32},
    {bcmPktDropEventIngressDstFp,                           33},
    {bcmPktDropEventIngressMplsProtectionDrop,              34},
    {bcmPktDropEventIngressMplsLabelActionInvalid,          35},
    {bcmPktDropEventIngressMplsTermPolicySelectTableDrop,   36},
    {bcmPktDropEventIngressMplsReservedLabelExposed,        37},
    {bcmPktDropEventIngressMplsTtlError,                    38},
    {bcmPktDropEventIngressMplsEcnError,                    39},
    {bcmPktDropEventIngressEmFt,                            40},
    {bcmPktDropEventIngressIvxlt,                           41},
    {bcmPktDropEventEgressEpRecircDrop,                     42},
    {bcmPktDropEventIngressUrpfCheckFailed,                 97},
    {bcmPktDropEventIngressSrcPortKnockoutDrop,             98},
    {bcmPktDropEventIngressLagFailoverPortDown,             99},
    {bcmPktDropEventIngressSplitHorizonCheckFailed,        100},
    {bcmPktDropEventIngressDstLinkDown,                    101},
    {bcmPktDropEventIngressBlockMaskDrop,                  102},
    {bcmPktDropEventIngressL3MtuCheckFailed,               103},
    {bcmPktDropEventIngressSeqNumCheckFailed,              104},
    {bcmPktDropEventIngressL3IifEqL3Oif,                   105},
    {bcmPktDropEventIngressStormControlDrop,               106},
    {bcmPktDropEventIngressEgrMembershipCheckFailed,       107},
    {bcmPktDropEventIngressEgrSpanningTreeCheckFailed,     108},
    {bcmPktDropEventIngressDstPbmZero,                     109},
    {bcmPktDropEventIngressMplsCtrlPktDrop,                110},
};

/*
 * Drop Event definitions which are available for copy to cpu usage.
 */

static bcmint_rx_trace_event_convert_t rx_trace_event_to_code[] =
{
    {-1,                                                    0},
    {bcmPktTraceEventIngressCmlFlags,                       1},
    {bcmPktTraceEventIngressL2SrcStaticMove,                2},
    {bcmPktTraceEventIngressL2SrcDiscard,                   3},
    {bcmPktTraceEventIngressMacsaMulticast,                 4},
    {bcmPktTraceEventIngressPktIntegrityCheckFailed,        5},
    {bcmPktTraceEventIngressProtocolPkt,                    6},
    {bcmPktTraceEventIngressMembershipCheckFailed,          7},
    {bcmPktTraceEventIngressSpanningTreeCheckFailed,        8},
    {bcmPktTraceEventIngressL2DstLookupMiss,                9},
    {bcmPktTraceEventIngressL2DstLookup,                    10},
    {bcmPktTraceEventIngressL3DstLookupMiss,                11},
    {bcmPktTraceEventIngressL3DstLookup,                    12},
    {bcmPktTraceEventIngressL3HdrError,                     13},
    {bcmPktTraceEventIngressL3TtlError,                     14},
    {bcmPktTraceEventIngressIpmcL3IifOrRpaIdCheckFailed,    15},
    {bcmPktTraceEventIngressLearnCacheFull,                 16},
    {bcmPktTraceEventIngressVfp,                            17},
    {bcmPktTraceEventIngressIfp,                            18},
    {bcmPktTraceEventIngressIfpMeter,                       19},
    {bcmPktTraceEventIngressDstFp,                          20},
    {bcmPktTraceEventIngressSvp,                            21},
    {bcmPktTraceEventIngressEmFt,                           22},
    {bcmPktTraceEventIngressIvxlt,                          23},
    {bcmPktTraceEventIngressMirrorSamplerSampled,           24},
    {bcmPktTraceEventIngressMirrorSamplerEgrSampled,        25},
    {bcmPktTraceEventIngressSerDrop,                        26},
    {bcmPktTraceEventIngressUrpfCheckFailed,                27},
    {bcmPktTraceEventIngressL3IifEqL3Oif,                   28},
    {bcmPktTraceEventIngressDlbEcmpMonitorEnOrMemberReassined, 29},
    {bcmPktTraceEventIngressDlbLagMonitorEnOrMemberReassined,  30},
    {bcmPktTraceEventIngressMplsCtrlPktToCpu,               31},
    {bcmPktTraceEventIngressMatchedRuleBit0,                32},
    {bcmPktTraceEventIngressMatchedRuleBit1,                33},
    {bcmPktTraceEventIngressMatchedRuleBit2,                34},
    {bcmPktTraceEventIngressMatchedRuleBit3,                35},
    {bcmPktTraceEventIngressMatchedRuleBit4,                36},
    {bcmPktTraceEventIngressMatchedRuleBit5,                37},
    {bcmPktTraceEventIngressMatchedRuleBit6,                38},
    {bcmPktTraceEventIngressMatchedRuleBit7,                39},
    {-1,                                                    44},
    {-1,                                                    45},
    {bcmPktTraceEventIngressTraceDop,                       47},
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
bcm56780_a0_dna_2_5_13_ltsw_tm_cos_q_cpu_map_info_get(
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
bcm56780_a0_dna_2_5_13_ltsw_rx_map_db_init(int unit,
                                          const bcmint_rx_map_db_t **map_db)
{
    *map_db = rx_map_db;

    return SHR_E_NONE;
}

/*!
 * \brief RX driver function variable for bcm56780_a0 DNA_2_5_13 device.
 */
static mbcm_ltsw_rx_drv_t bcm56780_a0_dna_2_5_13_ltsw_rx_drv = {
    .rx_map_db_init = bcm56780_a0_dna_2_5_13_ltsw_rx_map_db_init,
    .rx_tm_cos_q_cpu_map_info_get = bcm56780_a0_dna_2_5_13_ltsw_tm_cos_q_cpu_map_info_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_5_13_ltsw_rx_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_drv_set(unit, &bcm56780_a0_dna_2_5_13_ltsw_rx_drv));

exit:
    SHR_FUNC_EXIT();
}

