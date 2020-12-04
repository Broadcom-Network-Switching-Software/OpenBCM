/*! \file bcmcth_ctr_evict_imm.c
 *
 * BCMCTH CTR_EVICT IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_ctr_evict_drv.h>

/*******************************************************************************
 * Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREVICT

/* Internal use to identify no key LT case */
#define CTR_CONTROL_NO_KEY_LT      (0xFFFFFFFF)

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Fill CTR control structure for CTR_ING_FLEX_POOL_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_ing_flex_entry_set(int unit,
                               bcmltd_sid_t sid,
                               uint32_t fid,
                               uint64_t fval,
                               ctr_control_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    entry->pool = CTR_POOL_ING_FLEX;
    switch (fid) {
        case CTR_ING_FLEX_POOL_CONTROLt_CTR_ING_FLEX_POOL_IDf:
            entry->pool_id = fval;
            break;
        case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_MODEf:
            entry->evict_mode = fval;
            entry->update_flags |= CTR_UPDATE_F_E_MODE;
            break;
        case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf:
            entry->evict_thd_bytes = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
            break;
        case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf:
            entry->evict_thd_pkts = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
            break;
        case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_SEEDf:
            entry->evict_seed = fval;
            entry->update_flags |= CTR_UPDATE_F_E_SEED;
            break;
        case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Af:
            entry->evict_thd_ctr_a = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_A;
            break;
        case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Bf:
            entry->evict_thd_ctr_b = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_B;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure for CTR_EGR_FLEX_POOL_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_egr_flex_entry_set(int unit,
                               bcmltd_sid_t sid,
                               uint32_t fid,
                               uint64_t fval,
                               ctr_control_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    entry->pool = CTR_POOL_EGR_FLEX;
    switch (fid) {
        case CTR_EGR_FLEX_POOL_CONTROLt_CTR_EGR_FLEX_POOL_IDf:
            entry->pool_id = fval;
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_MODEf:
            entry->evict_mode = fval;
            entry->update_flags |= CTR_UPDATE_F_E_MODE;
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf:
            entry->evict_thd_bytes = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf:
            entry->evict_thd_pkts = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_SEEDf:
            entry->evict_seed = fval;
            entry->update_flags |= CTR_UPDATE_F_E_SEED;
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Af:
            entry->evict_thd_ctr_a = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_A;
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Bf:
            entry->evict_thd_ctr_b = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_B;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure for CTR_EGR_Q_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_egr_perq_entry_set(int unit,
                               bcmltd_sid_t sid,
                               uint32_t fid,
                               uint64_t fval,
                               ctr_control_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    entry->pool = CTR_POOL_EGR_PERQ;
    switch (fid) {
        case CTR_EGR_Q_CONTROLt_EVICTION_MODEf:
            entry->evict_mode = fval;
            entry->update_flags |= CTR_UPDATE_F_E_MODE;
            break;
        case CTR_EGR_Q_CONTROLt_EVICTION_THD_BYTESf:
            entry->evict_thd_bytes = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
            break;
        case CTR_EGR_Q_CONTROLt_EVICTION_THD_PKTSf:
            entry->evict_thd_pkts = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
            break;
        case CTR_EGR_Q_CONTROLt_EVICTION_SEEDf:
            entry->evict_seed = fval;
            entry->update_flags |= CTR_UPDATE_F_E_SEED;
            break;
        case CTR_CONTROL_NO_KEY_LT:
            /* For no key LT */
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure for CTR_EGR_FP_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_egr_fp_entry_set(int unit,
                             bcmltd_sid_t sid,
                             uint32_t fid,
                             uint64_t fval,
                             ctr_control_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    entry->pool = CTR_POOL_EGR_FP;
    switch (fid) {
        case CTR_EGR_FP_CONTROLt_EVICTION_MODEf:
            entry->evict_mode = fval;
            entry->update_flags |= CTR_UPDATE_F_E_MODE;
            break;
        case CTR_EGR_FP_CONTROLt_EVICTION_THD_BYTESf:
            entry->evict_thd_bytes = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
            break;
        case CTR_EGR_FP_CONTROLt_EVICTION_THD_PKTSf:
            entry->evict_thd_pkts = fval;
            entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
            break;
        case CTR_EGR_FP_CONTROLt_EVICTION_SEEDf:
            entry->evict_seed = fval;
            entry->update_flags |= CTR_UPDATE_F_E_SEED;
            break;
        case CTR_CONTROL_NO_KEY_LT:
            /* For no key LT */
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_entry_set(int unit,
                      bcmltd_sid_t sid,
                      uint32_t fid,
                      uint64_t fval,
                      ctr_control_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    switch (sid) {
        case CTR_ING_FLEX_POOL_CONTROLt:
            SHR_IF_ERR_EXIT
                (ctr_control_ing_flex_entry_set(unit, sid, fid, fval, entry));
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt:
            SHR_IF_ERR_EXIT
                (ctr_control_egr_flex_entry_set(unit, sid, fid, fval, entry));
            break;
        case CTR_EGR_Q_CONTROLt:
            SHR_IF_ERR_EXIT
                (ctr_control_egr_perq_entry_set(unit, sid, fid, fval, entry));
            break;
        case CTR_EGR_FP_CONTROLt:
            SHR_IF_ERR_EXIT
                (ctr_control_egr_fp_entry_set(unit, sid, fid, fval, entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure  as per.CTR_ING_FLEX_POOL_CONTROLt field default value.
 *
 * \param [in] update  evict update field.
 * \param [out] field ID.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_ing_fid_default_get(int unit,
                                ctr_evict_update_t update,
                                bcmltd_fid_t *fid)
{
    SHR_FUNC_ENTER(unit);

    switch (update) {
        case CTR_UPDATE_EVICT_MODE:
            *fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_MODEf;
            break;
        case CTR_UPDATE_EVICT_THD_BYTES:
            *fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf;
            break;
        case CTR_UPDATE_EVICT_THD_PKTS:
            *fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf;
            break;
        case CTR_UPDATE_EVICT_SEED:
            *fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_SEEDf;
            break;
        case CTR_UPDATE_EVICT_THD_CTR_A:
            *fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Af;
            break;
        case CTR_UPDATE_EVICT_THD_CTR_B:
            *fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Bf;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Fill CTR control structure  as per.CTR_EGR_FLEX_POOL_CONTROLt field default value.
 *
 * \param [in] update  evict update field.
 * \param [out] field ID.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_egr_fid_default_get(int unit,
                                ctr_evict_update_t update,
                                bcmltd_fid_t *fid)
{
    SHR_FUNC_ENTER(unit);

    switch (update) {
        case CTR_UPDATE_EVICT_MODE:
            *fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_MODEf;
            break;
        case CTR_UPDATE_EVICT_THD_BYTES:
            *fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf;
            break;
        case CTR_UPDATE_EVICT_THD_PKTS:
            *fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf;
            break;
        case CTR_UPDATE_EVICT_SEED:
            *fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_SEEDf;
            break;
        case CTR_UPDATE_EVICT_THD_CTR_A:
            *fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Af;
            break;
        case CTR_UPDATE_EVICT_THD_CTR_B:
            *fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_CTR_Bf;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure  as per.CTR_EGR_Q_CONTROLt field default value.
 *
 * \param [in] update  evict update field.
 * \param [out] field ID.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_perq_fid_default_get(int unit,
                                 ctr_evict_update_t update,
                                 bcmltd_fid_t *fid)
{

    SHR_FUNC_ENTER(unit);

    switch (update) {
        case CTR_UPDATE_EVICT_MODE:
            *fid = CTR_EGR_Q_CONTROLt_EVICTION_MODEf;
            break;
        case CTR_UPDATE_EVICT_THD_BYTES:
            *fid = CTR_EGR_Q_CONTROLt_EVICTION_THD_BYTESf;
            break;
        case CTR_UPDATE_EVICT_THD_PKTS:
            *fid = CTR_EGR_Q_CONTROLt_EVICTION_THD_PKTSf;
            break;
        case CTR_UPDATE_EVICT_SEED:
            *fid = CTR_EGR_Q_CONTROLt_EVICTION_SEEDf;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Fill CTR control structure  as per.CTR_EGR_FP_CONTROLt field default value.
 *
 * \param [in] update  evict update field.
 * \param [out] field ID.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_fp_fid_default_get(int unit,
                               ctr_evict_update_t update,
                               bcmltd_fid_t *fid)
{
    SHR_FUNC_ENTER(unit);

    switch (update) {
        case CTR_UPDATE_EVICT_MODE:
            *fid = CTR_EGR_FP_CONTROLt_EVICTION_MODEf;
            break;
        case CTR_UPDATE_EVICT_THD_BYTES:
            *fid = CTR_EGR_FP_CONTROLt_EVICTION_THD_BYTESf;
            break;
        case CTR_UPDATE_EVICT_THD_PKTS:
            *fid = CTR_EGR_FP_CONTROLt_EVICTION_THD_PKTSf;
            break;
        case CTR_UPDATE_EVICT_SEED:
            *fid = CTR_EGR_FP_CONTROLt_EVICTION_SEEDf;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure according to the given LT field default value.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] update  evict update field.
 * \param [out] def_val Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_fid_default_get(int unit,
                            bcmltd_sid_t sid,
                            ctr_evict_update_t update,
                            uint64_t *def_val)
{
    uint32_t num;
    bcmltd_fid_t fid = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(def_val, SHR_E_PARAM);

    switch (sid) {
       case CTR_ING_FLEX_POOL_CONTROLt:
            SHR_IF_ERR_VERBOSE_EXIT
                (ctr_control_ing_fid_default_get(unit, update, &fid));
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt:
            SHR_IF_ERR_VERBOSE_EXIT
                (ctr_control_egr_fid_default_get(unit, update, &fid));
            break;
        case CTR_EGR_Q_CONTROLt:
            SHR_IF_ERR_VERBOSE_EXIT
                (ctr_control_perq_fid_default_get(unit, update, &fid));
            break;
        case CTR_EGR_FP_CONTROLt:
            SHR_IF_ERR_VERBOSE_EXIT
                (ctr_control_fp_fid_default_get(unit, update, &fid));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_ERR_EXIT
        (bcmlrd_field_default_get(unit, sid, fid, 1, def_val, &num));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill non-updated CTR control parameter with default values
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_lt_field_fill_default(int unit,
                                  bcmltd_sid_t sid,
                                  ctr_control_entry_t *entry)
{
    uint64_t def_val;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if ((entry->update_flags & CTR_UPDATE_F_E_MODE) == 0) {
        rv = ctr_control_fid_default_get(unit, sid,
                                         CTR_UPDATE_EVICT_MODE,
                                         &def_val);
        if (SHR_SUCCESS(rv)) {
            entry->evict_mode = (def_val >> 32) & 0xFFFFFFFF;
            entry->update_flags |= CTR_UPDATE_F_E_MODE;
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_THD_BYTES) == 0) {
        rv = ctr_control_fid_default_get(unit, sid,
                                         CTR_UPDATE_EVICT_THD_BYTES,
                                         &def_val);
        if (SHR_SUCCESS(rv)) {
            entry->evict_thd_bytes = def_val;
            entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_THD_PKTS) == 0) {
        rv = ctr_control_fid_default_get(unit, sid,
                                         CTR_UPDATE_EVICT_THD_PKTS,
                                         &def_val);
        if (SHR_SUCCESS(rv)) {
            entry->evict_thd_pkts = (def_val >> 32) & 0xFFFFFFFF;
            entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_SEED) == 0) {
        rv = ctr_control_fid_default_get(unit, sid,
                                         CTR_UPDATE_EVICT_SEED,
                                         &def_val);
        if (SHR_SUCCESS(rv)) {
            entry->evict_seed = def_val;
            entry->update_flags |= CTR_UPDATE_F_E_SEED;
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_THD_CTR_A) == 0) {
        rv = ctr_control_fid_default_get(unit, sid,
                                         CTR_UPDATE_EVICT_THD_CTR_A,
                                         &def_val);
        if (SHR_SUCCESS(rv)) {
            entry->evict_thd_ctr_a = def_val;
            entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_A;
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_THD_CTR_B) == 0) {
        rv = ctr_control_fid_default_get(unit, sid,
                                         CTR_UPDATE_EVICT_THD_CTR_B,
                                         &def_val);
        if (SHR_SUCCESS(rv)) {
            entry->evict_thd_ctr_b = def_val;
            entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_B;
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure with default values
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_lt_field_restore_default(int unit,
                                     bcmltd_sid_t sid,
                                     ctr_control_entry_t *entry)
{
    uint64_t def_val;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    rv = ctr_control_fid_default_get(unit, sid,
                                     CTR_UPDATE_EVICT_MODE,
                                     &def_val);
    if (SHR_SUCCESS(rv)) {
        entry->evict_mode = (def_val >> 32) & 0xFFFFFFFF;
        entry->update_flags |= CTR_UPDATE_F_E_MODE;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    rv = ctr_control_fid_default_get(unit, sid,
                                     CTR_UPDATE_EVICT_THD_BYTES,
                                     &def_val);
    if (SHR_SUCCESS(rv)) {
        entry->evict_thd_bytes = def_val;
        entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    rv = ctr_control_fid_default_get(unit, sid,
                                     CTR_UPDATE_EVICT_THD_PKTS,
                                     &def_val);
    if (SHR_SUCCESS(rv)) {
        entry->evict_thd_pkts = (def_val >> 32) & 0xFFFFFFFF;
        entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    rv = ctr_control_fid_default_get(unit, sid,
                                     CTR_UPDATE_EVICT_SEED,
                                     &def_val);
    if (SHR_SUCCESS(rv)) {
        entry->evict_seed = def_val;
        entry->update_flags |= CTR_UPDATE_F_E_SEED;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    rv = ctr_control_fid_default_get(unit, sid,
                                     CTR_UPDATE_EVICT_THD_CTR_A,
                                     &def_val);
    if (SHR_SUCCESS(rv)) {
        entry->evict_thd_ctr_a = def_val;
        entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_A;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    rv = ctr_control_fid_default_get(unit, sid,
                                     CTR_UPDATE_EVICT_THD_CTR_B,
                                     &def_val);
    if (SHR_SUCCESS(rv)) {
        entry->evict_thd_ctr_b = def_val;
        entry->update_flags |= CTR_UPDATE_F_E_THD_CTR_B;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to ctr_pool_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry CTR control data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to convert \c in to \c pdata.
 */
static int
ctr_control_lt_fields_parse(int unit,
                            bcmltd_sid_t sid,
                            const bcmltd_field_t *key_flds,
                            const bcmltd_field_t *data_flds,
                            ctr_control_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    if (fld == 0) {
        /* For no key LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (ctr_control_entry_set(unit, sid, CTR_CONTROL_NO_KEY_LT, 0, entry));
    } else {
        while (fld) {
            fid = fld->id;
            fval = fld->data;

            SHR_IF_ERR_VERBOSE_EXIT
                (ctr_control_entry_set(unit, sid, fid, fval, entry));

            fld = fld->next;
        }
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (ctr_control_entry_set(unit, sid, fid, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_XXX_CONTROL IMM table change callback function for staging.
 *
 * Handle CTR_XXX_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] event BCMIMM entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context Pointer to the context given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
evict_control_imm_callback_stage(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 bcmimm_entry_event_t event,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 void *context,
                                 bcmltd_fields_t *output_fields)
{
    ctr_control_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ctr_control_entry_t));
    entry.pool = CTR_INVALID_POOL;
    SHR_IF_ERR_VERBOSE_EXIT
        (ctr_control_lt_fields_parse(unit, sid, key, data, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT
                (ctr_control_lt_field_fill_default(unit, sid, &entry));
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_evict_entry_update(unit, sid, trans_id, &entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_evict_entry_update(unit, sid, trans_id, &entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (ctr_control_lt_field_restore_default(unit, sid, &entry));
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_evict_entry_update(unit, sid, trans_id, &entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR Eviction In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to CTR_XXX_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t ctr_evict_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = evict_control_imm_callback_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static int
bcmcth_ctr_evict_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    bcmlrd_table_attrib_t t_attrib;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_ING_FLEX_POOL_CONTROL",
                                     &t_attrib);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, CTR_ING_FLEX_POOL_CONTROLt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit,
                                     CTR_ING_FLEX_POOL_CONTROLt,
                                     &ctr_evict_imm_callback,
                                     NULL));
        }
    } else {
        /* If CTR_ING_FLEX_POOL_CONTROL LT doesn't exist, exit */
        SHR_EXIT();
    }

    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_EGR_FLEX_POOL_CONTROL",
                                     &t_attrib);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, CTR_EGR_FLEX_POOL_CONTROLt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit,
                                     CTR_EGR_FLEX_POOL_CONTROLt,
                                     &ctr_evict_imm_callback,
                                     NULL));
        }
    } else {
        /* If CTR_EGR_FLEX_POOL_CONTROL LT doesn't exist, exit */
        SHR_EXIT();
    }

    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_EGR_Q_CONTROL",
                                     &t_attrib);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, CTR_EGR_Q_CONTROLt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit,
                                     CTR_EGR_Q_CONTROLt,
                                     &ctr_evict_imm_callback,
                                     NULL));
        }
    } else {
        /* If CTR_EGR_Q_CONTROL LT doesn't exist, exit */
        SHR_EXIT();
    }

    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_EGR_FP_CONTROL",
                                     &t_attrib);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlrd_map_get(unit, CTR_EGR_FP_CONTROLt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit,
                                     CTR_EGR_FP_CONTROLt,
                                     &ctr_evict_imm_callback,
                                     NULL));
        }
    } else {
        /* If CTR_EGR_FP_CONTROL LT doesn't exist, exit */
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ctr_evict_imm_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_ctr_evict_imm_register(unit));
exit:
    SHR_FUNC_EXIT();
}

