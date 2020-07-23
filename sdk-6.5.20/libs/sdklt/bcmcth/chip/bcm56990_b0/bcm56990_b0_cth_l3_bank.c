/*! \file bcm56990_b0_cth_l3_bank.c
 *
 * This file defines the chip-specific routines of
 * L3 bank driver for bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_l3_bank.h>
#include <bcmcth/bcmcth_l3_util.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_L3
#define BANK_NUM_PER_CHIP 8
#define NHOP_BANK_INDEX_OFFSET 12
#define EIF_BANK_INDEX_OFFSET 10

#define NHOP_BANK_LT_SID_COUNT 5
static bcmltd_sid_t bcm56990_b0_cth_l3_nhop_lt_sid[] = {
    L3_UC_NHOPt,
    L3_MC_NHOPt,
    TNL_MPLS_ENCAP_NHOPt,
    TNL_MPLS_TRANSIT_NHOPt,
    FP_ING_ACTION_NHOPt,
    L2_TAG_ACTION_NHOPt
};

#define EIF_BANK_LT_SID_COUNT 1
static bcmltd_sid_t bcm56990_b0_cth_l3_eif_lt_sid[] = {
    L3_EIFt
};
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief L3 bank encode.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  l3_bank       L3 banck struct ptr.
 *
 * \retval SHR_E_NONE         No errors.
 */
static int
bcm56990_b0_l3_bank_encode(int unit,
                           bcmcth_l3_bank_t *l3_bank)
{
    int off_set = 0;

    if (l3_bank->num_o_nhop_valid) {
        off_set = BANK_NUM_PER_CHIP - l3_bank->num_o_nhop;
        SHR_BITSET_RANGE(l3_bank->nhop_bank_bitmap,
                         off_set,
                         l3_bank->num_o_nhop);
    }

    if (l3_bank->num_o_eif_valid) {
        off_set = BANK_NUM_PER_CHIP - l3_bank->num_o_eif;
        SHR_BITSET_RANGE(l3_bank->eif_bank_bitmap,
                         off_set,
                         l3_bank->num_o_eif);
    }

    return SHR_E_NONE;
}

/*!
 * \brief L3 bank decode.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  l3_bank       L3 banck struct ptr.
 *
 * \retval SHR_E_NONE         No errors.
 */
static int
bcm56990_b0_l3_bank_decode(int unit,
                           bcmcth_l3_bank_t *l3_bank)
{
   int count = 0;

   if (l3_bank->num_o_nhop_valid) {
        SHR_BITCOUNT_RANGE(l3_bank->nhop_bank_bitmap,
                           count,
                           0,
                           BANK_NUM_PER_CHIP);
        l3_bank->num_o_nhop = count;
    }

    if (l3_bank->num_o_eif_valid) {
        SHR_BITCOUNT_RANGE(l3_bank->eif_bank_bitmap,
                           count,
                           0,
                           BANK_NUM_PER_CHIP);
        l3_bank->num_o_eif = count;
    }

    return SHR_E_NONE;

}

/*!
 * \brief L3 bank set.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  l3_bank       L3 banck struct info.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56990_b0_l3_bank_set(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t lt_id,
                        bcmcth_l3_bank_t l3_bank)
{
    ING_L3_NEXT_HOP_BANK_CONFIGr_t ing_nhop_buf;
    EGR_L3_NEXT_HOP_BANK_CONFIGr_t egr_nhop_buf;
    EGR_L3_INTF_BANK_CONFIGr_t eif_buf;

    SHR_FUNC_ENTER(unit);

    if (l3_bank.num_o_nhop_valid) {
        ING_L3_NEXT_HOP_BANK_CONFIGr_CLR(ing_nhop_buf);
        /* Only low 8 bits are used. */
        ING_L3_NEXT_HOP_BANK_CONFIGr_BITMAPf_SET
            (ing_nhop_buf, l3_bank.nhop_bank_bitmap[0]);
        SHR_IF_ERR_EXIT
            (bcmcth_l3_hw_op_write(unit, op_arg, lt_id,
                                   ING_L3_NEXT_HOP_BANK_CONFIGr,
                                   0, &ing_nhop_buf));

        EGR_L3_NEXT_HOP_BANK_CONFIGr_CLR(egr_nhop_buf);
        /* Only low 8 bits are used. */
        EGR_L3_NEXT_HOP_BANK_CONFIGr_BITMAPf_SET
            (egr_nhop_buf, l3_bank.nhop_bank_bitmap[0]);
        SHR_IF_ERR_EXIT
            (bcmcth_l3_hw_op_write(unit, op_arg, lt_id,
                                   EGR_L3_NEXT_HOP_BANK_CONFIGr,
                                   0, &egr_nhop_buf));
    }

    if (l3_bank.num_o_eif_valid) {
        EGR_L3_INTF_BANK_CONFIGr_CLR(eif_buf);
        /* Only low 8 bits are used. */
        EGR_L3_INTF_BANK_CONFIGr_BITMAPf_SET
            (eif_buf, l3_bank.eif_bank_bitmap[0]);
        SHR_IF_ERR_EXIT
            (bcmcth_l3_hw_op_write(unit, op_arg, lt_id,
                                   EGR_L3_INTF_BANK_CONFIGr,
                                   0, &eif_buf));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Tunnel MPLS protection enable get.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in/out]  l3_bank   pointer of l3_bank.
 * \param [in]  ireq          read via ireq PTM API.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56990_b0_l3_bank_get(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t lt_id,
                        bcmcth_l3_bank_t *l3_bank,
                        bool ireq)
{
    ING_L3_NEXT_HOP_BANK_CONFIGr_t ing_nhop_buf;
    EGR_L3_INTF_BANK_CONFIGr_t eif_buf;

    SHR_FUNC_ENTER(unit);

    ING_L3_NEXT_HOP_BANK_CONFIGr_CLR(ing_nhop_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_l3_hw_op_read(unit, op_arg, lt_id,
                              ING_L3_NEXT_HOP_BANK_CONFIGr,
                              0, ireq, &ing_nhop_buf));
    l3_bank->nhop_bank_bitmap[0] =
                        ING_L3_NEXT_HOP_BANK_CONFIGr_BITMAPf_GET(ing_nhop_buf);
    l3_bank->num_o_nhop_valid = 1;

    EGR_L3_INTF_BANK_CONFIGr_CLR(eif_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_l3_hw_op_read(unit, op_arg, lt_id,
                              EGR_L3_INTF_BANK_CONFIGr,
                              0, ireq, &eif_buf));
    l3_bank->eif_bank_bitmap[0] = EGR_L3_INTF_BANK_CONFIGr_BITMAPf_GET(eif_buf);
    l3_bank->num_o_eif_valid = 1;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/* Driver structure. */
static bcmcth_l3_bank_drv_t bcm56990_b0_cth_l3_bank_drv = {
    .nhop_lt_id_count = NHOP_BANK_LT_SID_COUNT,
    .nhop_lt_id_list = bcm56990_b0_cth_l3_nhop_lt_sid,
    .eif_lt_id_count = EIF_BANK_LT_SID_COUNT,
    .eif_lt_id_list = bcm56990_b0_cth_l3_eif_lt_sid,
    .nhop_bank_index_offset = NHOP_BANK_INDEX_OFFSET,
    .eif_bank_index_offset = EIF_BANK_INDEX_OFFSET,
    .l3_bank_encode = bcm56990_b0_l3_bank_encode,
    .l3_bank_decode = bcm56990_b0_l3_bank_decode,
    .l3_bank_set = bcm56990_b0_l3_bank_set,
    .l3_bank_get = bcm56990_b0_l3_bank_get,
};

bcmcth_l3_bank_drv_t *
bcm56990_b0_cth_l3_bank_drv_get(int unit)
{
    return &bcm56990_b0_cth_l3_bank_drv;
}
