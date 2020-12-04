/*! \file bcm56880_a0_dna_4_7_11_field_destination.c
 *
 * BCM56880_A0 dna_4_7_4 Field Destination driver.
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
bcm56880_a0_dna_4_7_11_field_dest_lt[] = {
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
bcm56880_a0_dna_4_7_11_field_dest_drop_reason_map[bcmFieldDropReasonLastCount] = {
    [bcmFieldDropReasonNoDrop] =
        {0, bcmFieldDropReasonNoDrop},
    [bcmFieldDropReasonCMLFlags] =
        {1, bcmFieldDropReasonCMLFlags},
    [bcmFieldDropReasonL2SrcStaticMove] =
        {2, bcmFieldDropReasonL2SrcStaticMove},
    [bcmFieldDropReasonL2SrcDiscard] =
        {3, bcmFieldDropReasonL2SrcDiscard},
    [bcmFieldDropReasonMacSaMulticast] =
        {4, bcmFieldDropReasonMacSaMulticast},
    [bcmFieldDropReasonOuterTpidCheckFailed] =
        {5, bcmFieldDropReasonOuterTpidCheckFailed},
    [bcmFieldDropReasonIncomingPvlanCheckFailed] =
        {6, bcmFieldDropReasonIncomingPvlanCheckFailed},
    [bcmFieldDropReasonPktIntegrityCheckFailed] =
        {7, bcmFieldDropReasonPktIntegrityCheckFailed},
    [bcmFieldDropReasonProtocolPktDrop] =
        {8, bcmFieldDropReasonProtocolPktDrop},
    [bcmFieldDropReasonMembershipCheckFailed] =
        {9, bcmFieldDropReasonMembershipCheckFailed},
    [bcmFieldDropReasonSpanningTreeCheckFailed] =
        {10, bcmFieldDropReasonSpanningTreeCheckFailed},
    [bcmFieldDropReasonL2DstLookupMiss] =
        {11, bcmFieldDropReasonL2DstLookupMiss},
    [bcmFieldDropReasonL2DstDiscard] =
        {12, bcmFieldDropReasonL2DstDiscard},
    [bcmFieldDropReasonL3DstLookupMiss] =
        {13, bcmFieldDropReasonL3DstLookupMiss},
    [bcmFieldDropReasonL3DstDiscard] =
        {14, bcmFieldDropReasonL3DstDiscard},
    [bcmFieldDropReasonL3HdrError] =
        {15, bcmFieldDropReasonL3HdrError},
    [bcmFieldDropReasonL3TTLError] =
        {16, bcmFieldDropReasonL3TTLError},
    [bcmFieldDropReasonIPMCL3IIFOrRPAIDCheckFailed] =
        {17, bcmFieldDropReasonIPMCL3IIFOrRPAIDCheckFailed},
    [bcmFieldDropReasonTunnelTTLCheckFailed] =
        {18, bcmFieldDropReasonTunnelTTLCheckFailed},
    [bcmFieldDropReasonTunnelShimHdrError] =
        {19, bcmFieldDropReasonTunnelShimHdrError},
    [bcmFieldDropReasonTunnelObjectValidationFailed] =
        {20, bcmFieldDropReasonTunnelObjectValidationFailed},
    [bcmFieldDropReasonTunnelAdaptDrop] =
        {21, bcmFieldDropReasonTunnelAdaptDrop},
    [bcmFieldDropReasonPVLANDrop] =
        {22, bcmFieldDropReasonPVLANDrop},
    [bcmFieldDropReasonVFP] =
        {23, bcmFieldDropReasonVFP},
    [bcmFieldDropReasonIFP] =
        {24, bcmFieldDropReasonIFP},
    [bcmFieldDropReasonIFPMeter] =
        {25, bcmFieldDropReasonIFPMeter},
    [bcmFieldDropReasonDSTFP] =
        {26, bcmFieldDropReasonDSTFP},
    [bcmFieldDropReasonMplsProtectionDrop] =
        {27, bcmFieldDropReasonMplsProtectionDrop},
    [bcmFieldDropReasonMplsLabelActionInvalid] =
        {28, bcmFieldDropReasonMplsLabelActionInvalid},
    [bcmFieldDropReasonMplsTermPolicySelectTableDrop] =
        {29, bcmFieldDropReasonMplsTermPolicySelectTableDrop},
    [bcmFieldDropReasonMPLSReservedLabelExposed] =
        {30, bcmFieldDropReasonMPLSReservedLabelExposed},
    [bcmFieldDropReasonMplsTTLError] =
        {31, bcmFieldDropReasonMplsTTLError},
    [bcmFieldDropReasonMplsEcnError] =
        {32, bcmFieldDropReasonMplsEcnError},
    [bcmFieldDropReasonEMFT] =
        {33, bcmFieldDropReasonEMFT},
    [bcmFieldDropReasonIVXLT] =
        {34, bcmFieldDropReasonIVXLT},
    [bcmFieldDropReasonUrpfCheckFailed] =
        {90, bcmFieldDropReasonUrpfCheckFailed},
    [bcmFieldDropReasonSrcPortKnockoutDrop] =
        {91, bcmFieldDropReasonSrcPortKnockoutDrop},
    [bcmFieldDropReasonLagFailoverPortDown] =
        {92, bcmFieldDropReasonLagFailoverPortDown},
    [bcmFieldDropReasonSplitHorizonCheckFailed] =
        {93, bcmFieldDropReasonSplitHorizonCheckFailed},
    [bcmFieldDropReasonDstLinkDown] =
        {94, bcmFieldDropReasonDstLinkDown},
    [bcmFieldDropReasonBlockMaskDrop] =
        {95, bcmFieldDropReasonBlockMaskDrop},
    [bcmFieldDropReasonL3MtuCheckFailed] =
        {96, bcmFieldDropReasonL3MtuCheckFailed},
    [bcmFieldDropReasonSeqNumCheckFailed] =
        {97, bcmFieldDropReasonSeqNumCheckFailed},
    [bcmFieldDropReasonL3IIFEqL3OIF] =
        {98, bcmFieldDropReasonL3IIFEqL3OIF},
    [bcmFieldDropReasonStormControlDrop] =
        {99, bcmFieldDropReasonStormControlDrop},
    [bcmFieldDropReasonEgrMembershipCheckFailed] =
        {100, bcmFieldDropReasonEgrMembershipCheckFailed},
    [bcmFieldDropReasonEgrSpanningTreeCheckFailed] =
        {101, bcmFieldDropReasonEgrSpanningTreeCheckFailed},
    [bcmFieldDropReasonDstPBMZero] =
        {102, bcmFieldDropReasonDstPBMZero},
    [bcmFieldDropReasonMplsCtrlPktDrop] =
        {103, bcmFieldDropReasonMplsCtrlPktDrop}
};

/******************************************************************************
 * Private functions
 */
static int
bcm56880_a0_dna_4_7_11_field_dest_lt_db_get(
    int unit,
    bcmint_field_destination_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    lt_db->lt_bmp = (1 << BCMINT_LTSW_FIELD_DEST_LT_DESTINATION_FP_TABLE);

    lt_db->lts = bcm56880_a0_dna_4_7_11_field_dest_lt;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_dna_4_7_11_field_destination_drop_reason_to_code(
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

    map = &bcm56880_a0_dna_4_7_11_field_dest_drop_reason_map[reason];
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
bcm56880_a0_dna_4_7_11_field_destination_drop_code_to_reason(
    int unit,
    int code,
    bcm_field_drop_reason_t *reason)
{
    bcm_field_drop_reason_t reason_tmp;
    const bcmint_field_destination_map_t *map;

    SHR_FUNC_ENTER(unit);

    for (reason_tmp = 0; reason_tmp < bcmFieldDropReasonLastCount; reason_tmp++) {
        map = &bcm56880_a0_dna_4_7_11_field_dest_drop_reason_map[reason_tmp];
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

static mbcm_ltsw_field_drv_t bcm56880_a0_dna_4_7_11_ltsw_field_dest_drv = {
    .field_destination_lt_db_get = bcm56880_a0_dna_4_7_11_field_dest_lt_db_get,
    .field_destination_drop_reason_to_code =
         bcm56880_a0_dna_4_7_11_field_destination_drop_reason_to_code,
    .field_destination_drop_code_to_reason =
         bcm56880_a0_dna_4_7_11_field_destination_drop_code_to_reason
};

/******************************************************************************
 * Public functions
 */
int
bcm56880_a0_dna_4_7_11_ltsw_field_destination_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv_set(unit,
                                 &bcm56880_a0_dna_4_7_11_ltsw_field_dest_drv));

exit:
    SHR_FUNC_EXIT();
}

