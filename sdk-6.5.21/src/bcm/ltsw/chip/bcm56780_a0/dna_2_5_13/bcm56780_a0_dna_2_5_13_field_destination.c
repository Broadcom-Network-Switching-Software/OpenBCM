/*! \file bcm56780_a0_dna_2_5_13_field_destination.c
 *
 * BCM56780_A0 dna_2_5_13 Field Destination driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/field_destination_int.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FP

/* DESTINATION_FP_TABLE LT fields. */
static const bcmint_field_destination_fld_t
destination_fp_table_flds[BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_FLD_CNT] = {
    [BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_DROP_CODE] = {
        DROP_CODEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_DROP_CODE_MASK] = {
        DROP_CODE_MASKs,
        NULL,
        NULL
    }
};

static const bcmint_field_destination_lt_t
bcm56780_a0_dna_2_5_13_field_dest_lt[] = {
    [BCMINT_LTSW_FIELD_DEST_LT_DESTINATION_FP_TABLE] = {
        .name = DESTINATION_FP_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_DROP_CODE) |
            (1 << BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_DROP_CODE_MASK),
        .flds = destination_fp_table_flds
    }
};

/* Field destination drop reason map. */
static const bcmint_field_destination_map_t
bcm56780_a0_dna_2_5_13_field_dest_drop_reason_map[bcmFieldDropReasonLastCount] = {
    [bcmFieldDropReasonNoDrop] =
        {0, bcmFieldDropReasonNoDrop},
    [bcmFieldDropReasonCMLFlags] =
        {8, bcmFieldDropReasonCMLFlags},
    [bcmFieldDropReasonL2SrcStaticMove] =
        {9, bcmFieldDropReasonL2SrcStaticMove},
    [bcmFieldDropReasonL2SrcDiscard] =
        {10, bcmFieldDropReasonL2SrcDiscard},
    [bcmFieldDropReasonMacSaMulticast] =
        {11, bcmFieldDropReasonMacSaMulticast},
    [bcmFieldDropReasonOuterTpidCheckFailed] =
        {12, bcmFieldDropReasonOuterTpidCheckFailed},
    [bcmFieldDropReasonIncomingPvlanCheckFailed] =
        {13, bcmFieldDropReasonIncomingPvlanCheckFailed},
    [bcmFieldDropReasonPktIntegrityCheckFailed] =
        {14, bcmFieldDropReasonPktIntegrityCheckFailed},
    [bcmFieldDropReasonProtocolPktDrop] =
        {15, bcmFieldDropReasonProtocolPktDrop},
    [bcmFieldDropReasonMembershipCheckFailed] =
        {16, bcmFieldDropReasonMembershipCheckFailed},
    [bcmFieldDropReasonSpanningTreeCheckFailed] =
        {17, bcmFieldDropReasonSpanningTreeCheckFailed},
    [bcmFieldDropReasonL2DstLookupMiss] =
        {18, bcmFieldDropReasonL2DstLookupMiss},
    [bcmFieldDropReasonL2DstDiscard] =
        {19, bcmFieldDropReasonL2DstDiscard},
    [bcmFieldDropReasonL3DstLookupMiss] =
        {20, bcmFieldDropReasonL3DstLookupMiss},
    [bcmFieldDropReasonL3DstDiscard] =
        {21, bcmFieldDropReasonL3DstDiscard},
    [bcmFieldDropReasonL3HdrError] =
        {22, bcmFieldDropReasonL3HdrError},
    [bcmFieldDropReasonL3TTLError] =
        {23, bcmFieldDropReasonL3TTLError},
    [bcmFieldDropReasonIPMCL3IIFOrRPAIDCheckFailed] =
        {24, bcmFieldDropReasonIPMCL3IIFOrRPAIDCheckFailed},
    [bcmFieldDropReasonTunnelTTLCheckFailed] =
        {25, bcmFieldDropReasonTunnelTTLCheckFailed},
    [bcmFieldDropReasonTunnelShimHdrError] =
        {26, bcmFieldDropReasonTunnelShimHdrError},
    [bcmFieldDropReasonTunnelObjectValidationFailed] =
        {27, bcmFieldDropReasonTunnelObjectValidationFailed},
    [bcmFieldDropReasonTunnelAdaptDrop] =
        {28, bcmFieldDropReasonTunnelAdaptDrop},
    [bcmFieldDropReasonPVLANDrop] =
        {29, bcmFieldDropReasonPVLANDrop},
    [bcmFieldDropReasonVFP] =
        {30, bcmFieldDropReasonVFP},
    [bcmFieldDropReasonIFP] =
        {31, bcmFieldDropReasonIFP},
    [bcmFieldDropReasonIFPMeter] =
        {32, bcmFieldDropReasonIFPMeter},
    [bcmFieldDropReasonDSTFP] =
        {33, bcmFieldDropReasonDSTFP},
    [bcmFieldDropReasonMplsProtectionDrop] =
        {34, bcmFieldDropReasonMplsProtectionDrop},
    [bcmFieldDropReasonMplsLabelActionInvalid] =
        {35, bcmFieldDropReasonMplsLabelActionInvalid},
    [bcmFieldDropReasonMplsTermPolicySelectTableDrop] =
        {36, bcmFieldDropReasonMplsTermPolicySelectTableDrop},
    [bcmFieldDropReasonMPLSReservedLabelExposed] =
        {37, bcmFieldDropReasonMPLSReservedLabelExposed},
    [bcmFieldDropReasonMplsTTLError] =
        {38, bcmFieldDropReasonMplsTTLError},
    [bcmFieldDropReasonMplsEcnError] =
        {39, bcmFieldDropReasonMplsEcnError},
    [bcmFieldDropReasonEMFT] =
        {40, bcmFieldDropReasonEMFT},
    [bcmFieldDropReasonIVXLT] =
        {41, bcmFieldDropReasonIVXLT},
    [bcmFieldDropReasonUrpfCheckFailed] =
        {97, bcmFieldDropReasonUrpfCheckFailed},
    [bcmFieldDropReasonSrcPortKnockoutDrop] =
        {98, bcmFieldDropReasonSrcPortKnockoutDrop},
    [bcmFieldDropReasonLagFailoverPortDown] =
        {99, bcmFieldDropReasonLagFailoverPortDown},
    [bcmFieldDropReasonSplitHorizonCheckFailed] =
        {100, bcmFieldDropReasonSplitHorizonCheckFailed},
    [bcmFieldDropReasonDstLinkDown] =
        {101, bcmFieldDropReasonDstLinkDown},
    [bcmFieldDropReasonBlockMaskDrop] =
        {102, bcmFieldDropReasonBlockMaskDrop},
    [bcmFieldDropReasonL3MtuCheckFailed] =
        {103, bcmFieldDropReasonL3MtuCheckFailed},
    [bcmFieldDropReasonSeqNumCheckFailed] =
        {104, bcmFieldDropReasonSeqNumCheckFailed},
    [bcmFieldDropReasonL3IIFEqL3OIF] =
        {105, bcmFieldDropReasonL3IIFEqL3OIF},
    [bcmFieldDropReasonStormControlDrop] =
        {106, bcmFieldDropReasonStormControlDrop},
    [bcmFieldDropReasonEgrMembershipCheckFailed] =
        {107, bcmFieldDropReasonEgrMembershipCheckFailed},
    [bcmFieldDropReasonEgrSpanningTreeCheckFailed] =
        {108, bcmFieldDropReasonEgrSpanningTreeCheckFailed},
    [bcmFieldDropReasonDstPBMZero] =
        {109, bcmFieldDropReasonDstPBMZero},
    [bcmFieldDropReasonMplsCtrlPktDrop] =
        {110, bcmFieldDropReasonMplsCtrlPktDrop},
    [bcmFieldDropReasonIngRecircleHdrDrop] =
        {42, bcmFieldDropReasonIngRecircleHdrDrop},
    [bcmFieldDropReasonForteIpv4Gateway] =
        {43, bcmFieldDropReasonForteIpv4Gateway}
};

/******************************************************************************
 * Private functions
 */
static int
bcm56780_a0_dna_2_5_13_field_dest_lt_db_get(
    int unit,
    bcmint_field_destination_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    lt_db->lt_bmp = (1 << BCMINT_LTSW_FIELD_DEST_LT_DESTINATION_FP_TABLE);

    lt_db->lts = bcm56780_a0_dna_2_5_13_field_dest_lt;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_dna_2_5_13_field_destination_drop_reason_to_code(
    int unit,
    bcm_field_drop_reason_t reason,
    int *code)
{
    const bcmint_field_destination_map_t *map;
    SHR_FUNC_ENTER(unit);

    if (reason >= bcmFieldDropReasonLastCount) {
        LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Error: drop reason %d not supported.\n"),
                     reason));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    map = &bcm56780_a0_dna_2_5_13_field_dest_drop_reason_map[reason];
    if (map->type != reason) {
        LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Error: drop reason %d not supported.\n"),
                     reason));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *code = map->value;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_dna_2_5_13_field_destination_drop_code_to_reason(
    int unit,
    int code,
    bcm_field_drop_reason_t *reason)
{
    bcm_field_drop_reason_t reason_tmp;
    const bcmint_field_destination_map_t *map;

    SHR_FUNC_ENTER(unit);

    for (reason_tmp = 0; reason_tmp < bcmFieldDropReasonLastCount; reason_tmp++) {
        map = &bcm56780_a0_dna_2_5_13_field_dest_drop_reason_map[reason_tmp];
        if (map->value == code) {
            break;
        }
    }

    if (reason_tmp == bcmFieldDropReasonLastCount) {
        LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Error: drop reason %d not found.\n"),
                     code));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *reason = reason_tmp;

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_field_drv_t bcm56780_a0_dna_2_5_13_ltsw_field_dest_drv = {
    .field_destination_lt_db_get = bcm56780_a0_dna_2_5_13_field_dest_lt_db_get,
    .field_destination_drop_reason_to_code =
         bcm56780_a0_dna_2_5_13_field_destination_drop_reason_to_code,
    .field_destination_drop_code_to_reason =
         bcm56780_a0_dna_2_5_13_field_destination_drop_code_to_reason
};

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_dna_2_5_13_ltsw_field_destination_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv_set(unit,
                                 &bcm56780_a0_dna_2_5_13_ltsw_field_dest_drv));

exit:
    SHR_FUNC_EXIT();
}
