/*! \file bcmcth_fd_imm.c
 *
 * FlexDigest interfaces to IMM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmcth/bcmcth_flex_digest_drv.h>
/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEXDIGEST

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief FLEX_DIGEST_LKUP IMM table change callback function for staging.
 *
 * Handle FLEX_DIGEST_LKUP IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL error.
 */
static int
fd_lkup_cth(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t entry_event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context,
                     bcmltd_fields_t *output_fields)
{

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(context);

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (entry_event) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT(bcmcth_flex_digest_lentry_process(unit,
                     sid,
                     trans_id,
                     entry_event,
                     key,
                     data,
                     context,
                     output_fields));
        break;

    case BCMIMM_ENTRY_LOOKUP:
        SHR_EXIT();
        break;

    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FLEX_DIGEST_LKUP_PRESEL IMM table opcode validate.
 *
 * Validate FLEX_DIGEST_LKUP_PRESEL IMM table opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE no error.
 * \retval SHR_E_UNAVAIL not available.
 */

static int
fd_lkup_presel_validate(int unit,
                       bcmltd_sid_t sid,
                       bcmimm_entry_event_t entry_event,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(key);
    COMPILER_REFERENCE(data);
    COMPILER_REFERENCE(context);

    switch (entry_event) {
    case BCMIMM_ENTRY_DELETE:
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
       SHR_IF_ERR_EXIT(bcmcth_flex_digest_lentry_validate(unit,
                     sid,
                     entry_event,
                     key,
                     data,
                     context));
       break;

    case BCMIMM_ENTRY_LOOKUP:
       SHR_EXIT();
       break;

    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FLEX_DIGEST_LKUP IMM table opcode validate.
 *
 * Validate FLEX_DIGEST_LKUP IMM table opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE no error.
 * \retval SHR_E_UNAVAIL error.
 */
static int
fd_lkup_validate(int unit,
                       bcmltd_sid_t sid,
                       bcmimm_entry_event_t entry_event,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(key);
    COMPILER_REFERENCE(data);
    COMPILER_REFERENCE(context);

    switch (entry_event) {
    case BCMIMM_ENTRY_DELETE:
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
       SHR_IF_ERR_EXIT(bcmcth_flex_digest_lentry_validate(unit,
                     sid,
                     entry_event,
                     key,
                     data,
                     context));
       break;

    case BCMIMM_ENTRY_LOOKUP:
        SHR_EXIT();
        break;

    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief FLEX_DIGEST_LKUP In-memory event callback structure.
 *
 * This structure contains callback functions that is corresponding
 * to FLEX_DIGEST_LKUP logical table entry commit stages.
 */
static bcmimm_lt_cb_t fd_lkup_imm_callback = {

    /*! Validate function. */
    .validate = fd_lkup_validate,

    /*! Staging function. */
    .stage = fd_lkup_cth,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief FLEX_DIGEST_LKUP_PRESEL IMM table change callback function.
 *
 * Handle FLEX_DIGEST_LKUP_PRESEL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL error.
 */
static int
fd_lkup_presel_cth(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t entry_event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(context);

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (entry_event) {
       case BCMIMM_ENTRY_INSERT:
       case BCMIMM_ENTRY_UPDATE:
       case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT(bcmcth_flex_digest_lentry_process(unit,
                     sid,
                     trans_id,
                     entry_event,
                     key,
                     data,
                     context,
                     output_fields));
             break;
    default:
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FLEX_DIGEST_HASH_SALT IMM table change callback function.
 *
 * Handle FLEX_DIGEST_HASH_SALT IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL error.
 */
static int
fd_hash_salt_cth(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t entry_event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(context);

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (entry_event) {
       case BCMIMM_ENTRY_INSERT:
       case BCMIMM_ENTRY_UPDATE:
       case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT(bcmcth_flex_digest_lentry_process(unit,
                     sid,
                     trans_id,
                     entry_event,
                     key,
                     data,
                     context,
                     output_fields));
             break;
    default:
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FLEX_DIGEST_LKUP_PRESEL In-memory event callback structure.
 *
 * This structure contains callback functions that is conresponding
 * to FLEX_DIGEST_LKUP_PRESEL logical table entry commit stages.
 */
static bcmimm_lt_cb_t fd_lkup_presel_imm_callback = {

    /*! Validate function. */
    .validate = fd_lkup_presel_validate,

    /*! Staging function. */
    .stage = fd_lkup_presel_cth,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief FLEX_DIGEST_HASH_SALT In-memory event callback structure.
 *
 * This structure contains callback functions that is conresponding
 * to FLEX_DIGEST_HASH_SALT logical table entry commit stages.
 */
static bcmimm_lt_cb_t fd_hash_salt_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = fd_hash_salt_cth,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_flex_digest_imm_register(int unit, void *context)
{
    const bcmlrd_map_t *map = NULL;
    int rv = SHR_E_NONE;
    bcmcth_flex_digest_variant_field_id_t *desc;

    SHR_FUNC_ENTER(unit);
    desc = (bcmcth_flex_digest_variant_field_id_t *)context;

    /*
     * To register callback for Flex Digest LTs here: FLEX_DIGEST_LKUPt.
     */
    rv = bcmlrd_map_get(unit, desc->lkup_sid, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 desc->lkup_sid,
                                 &fd_lkup_imm_callback,
                                 context));
    }

    /* FLEX_DIGEST_LKUP_PRESELt */
    rv = bcmlrd_map_get(unit, desc->presel_sid, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 desc->presel_sid,
                                 &fd_lkup_presel_imm_callback,
                                 context));
    }

    rv = bcmlrd_map_get(unit, FLEX_DIGEST_HASH_SALTt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 FLEX_DIGEST_HASH_SALTt,
                                 &fd_hash_salt_imm_callback,
                                 context));
    }

exit:
    SHR_FUNC_EXIT();
}
