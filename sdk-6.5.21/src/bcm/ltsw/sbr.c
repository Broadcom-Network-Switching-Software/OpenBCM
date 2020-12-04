/*! \file sbr.c
 *
 * Management of strength profile table and strength field for strength
 * based-resolution (SBR).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcm/types.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/mbcm/sbr.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/sbr_int.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/*!
 * \brief Strength profile entry information.
 */
typedef struct sbr_profile_ent_s {

    /*! Entry index. */
    int index;

    /*! Entry type. */
    bcmi_ltsw_sbr_profile_ent_type_t ent_type;
} sbr_profile_ent_t;

/*!
 * \brief Strength profile table information.
 */
typedef struct sbr_profile_tbl_s {

    /*! Entry information array. */
    sbr_profile_ent_t *ent_info;

    /*! Number of entry array. */
    int ent_num;

} sbr_profile_tbl_t;

/* SBR module informaiton. */
typedef struct sbr_info_s {
    /* Initialized flag. */
    int inited;

    /* SBR database per variant, which can be shared by multiple units. */
    bcmint_sbr_db_t sbr_db;

    /* Strength profile table information per unit. */
    sbr_profile_tbl_t *profile_tbl_info;
} sbr_info_t;

static sbr_info_t sbr_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/******************************************************************************
* Private functions
 */

/*!
 * \brief Configure the maximum number of table entries for a SBR table.
 *
 * \param [in] unit        Unit number.
 * \param [in] table_id    Logical table ID.
 * \param [in] max_entries Maximum number of table entries.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
sbr_table_max_entries_set(int unit, const char *table_id, uint64_t max_entries)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_CONTROLs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TABLE_IDs, table_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MAX_ENTRIESs, max_entries));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the table size for a SBR table.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Logical table ID.
 * \param [in] entry_limit The greatest number of table entries.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
sbr_table_size_get(int unit, const char *table_id, uint64_t *table_size)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_INFOs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TABLE_IDs, table_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, ENTRY_LIMITs, table_size));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the max entries of all strength profile tables.
 *
 * \param [in] unit      Unit Number.
 * \param [in] sbr_db    SBR database.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
sbr_profile_tbl_max_entries_udpate (int unit, bcmint_sbr_db_t *sbr_db)
{
    const bcmint_sbr_profile_tbl_t *tbl_info = NULL;
    uint64_t max_entries = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    /* Get strength profile database. */
    tbl_info = sbr_db->profile_tbl_db;

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {
        if (!SHR_BITGET(sbr_db->profile_tbl_bmp, i)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (sbr_table_size_get(unit, tbl_info[i].name, &max_entries));

        SHR_IF_ERR_VERBOSE_EXIT
            (sbr_table_max_entries_set(unit, tbl_info[i].name, max_entries));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all strength profile tables.
 *
 * \param [in] unit      Unit Number.
 * \param [in] sbr_db    SBR database.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
sbr_profile_tbl_clear(int unit, bcmint_sbr_db_t *sbr_db)
{
    const bcmint_sbr_profile_tbl_t *tbl_info = NULL;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    /* Get strength profile database. */
    tbl_info = sbr_db->profile_tbl_db;

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {
        if (!SHR_BITGET(sbr_db->profile_tbl_bmp, i)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, tbl_info[i].name));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup strength profile table information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static void
sbr_profile_tbl_info_cleanup(int unit)
{
    sbr_info_t *si = &sbr_info[unit];
    int i = 0;

    if (si->profile_tbl_info) {

        for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {

            if (si->sbr_db.profile_tbl_bmp &&
                !SHR_BITGET(si->sbr_db.profile_tbl_bmp, i)) {
                continue;
            }

            SHR_FREE(si->profile_tbl_info[i].ent_info);
        }

        SHR_FREE(si->profile_tbl_info);
    }

    return;
}

/*!
 * \brief Initialize strength profile table information.
 *
 * \param [in] unit      Unit Number.
 * \param [in] sbr_db    SBR database.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
sbr_profile_tbl_info_init(int unit)
{
    sbr_info_t *si = &sbr_info[unit];
    const bcmint_sbr_profile_tbl_t *db_tbl_info = NULL;
    sbr_profile_tbl_t *tbl_info = NULL;
    uint32_t alloc_size = 0;
    int i = 0, j = 0;

    SHR_FUNC_ENTER(unit);

    alloc_size = sizeof(sbr_profile_tbl_t) * BCMI_LTSW_SBR_PTH_COUNT;
    SHR_ALLOC(si->profile_tbl_info,
              alloc_size, "bcmLtswProfTblInfo");
    SHR_NULL_CHECK(si->profile_tbl_info, SHR_E_MEMORY);
    sal_memset(si->profile_tbl_info, 0, alloc_size);

    db_tbl_info = si->sbr_db.profile_tbl_db;
    tbl_info = si->profile_tbl_info;

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {

        if (!SHR_BITGET(si->sbr_db.profile_tbl_bmp, i)) {
            continue;
        }

        alloc_size = sizeof(sbr_profile_ent_t) * db_tbl_info[i].ent_num;
        SHR_ALLOC(tbl_info[i].ent_info,
                  alloc_size, "bcmLtswProfEntInfo");
        SHR_NULL_CHECK(tbl_info[i].ent_info, SHR_E_MEMORY);

        tbl_info[i].ent_num = db_tbl_info[i].ent_num;
        for (j = 0; j < tbl_info[i].ent_num; j++) {
            tbl_info[i].ent_info[j].ent_type =
                db_tbl_info[i].ent_info[j].ent_type;
            tbl_info[i].ent_info[j].index =
                db_tbl_info[i].ent_info[j].index;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        sbr_profile_tbl_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add all default entries in strength profile table.
 *
 * \param [in] unit      Unit Number.
 * \param [in] sbr_db    SBR database.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
sbr_profile_tbl_add(int unit, bcmint_sbr_db_t *sbr_db)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const bcmint_sbr_profile_tbl_t *db_tbl_info = NULL;
    sbr_profile_tbl_t *tbl_info = NULL;
    bcmint_sbr_profile_ent_t *ent_info = NULL;
    const bcmint_sbr_profile_fld_t *fld_info = NULL;
    int i = 0, j = 0, k = 0;
    bcmlt_opcode_t opcode;
    int rv = SHR_E_NONE;
    uint64_t index = 0;
    int ent_num = 0;
    int fld_num = 0;
    int dunit = 0;
    uint32_t status = 0;

    SHR_FUNC_ENTER(unit);

    /* Get strength profile database. */
    db_tbl_info = sbr_db->profile_tbl_db;
    tbl_info = sbr_info[unit].profile_tbl_info;

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {

        if (!SHR_BITGET(sbr_db->profile_tbl_bmp, i)) {
            continue;
        }

        ent_info = db_tbl_info[i].ent_info;
        ent_num = db_tbl_info[i].ent_num;

        /* Add entries. */
        for (j = 0; j < ent_num; j++) {

            /* No LT operation for dummy shared entry. */
            if (ent_info[j].shr_ent != NULL) {
                continue;
            }

            /* No LT operation for disabled entry. */
            if (SHR_SUCCESS(
                    mbcm_ltsw_sbr_ent_status_get(
                    unit, i,  ent_info[j].ent_type, &status))) {
                if (status & BCMINT_SBR_ENT_STATUS_DISABLED) {
                    continue;
                }
            }

            fld_info = ent_info[j].fld_info;
            fld_num = ent_info[j].fld_num;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, db_tbl_info[i].name, &ent_hdl));

            /* No key index for the static entry or idx_allocate LT. */
            if (ent_info[j].index != -1) {
                opcode = BCMLT_OPCODE_INSERT;

                /*
                 * Insert with specified key for some strength profile
                 * tables even SDKLT can allocate index dynamically.
                 */
                if (ent_info[j].flags & BCMINT_SBR_CTRL_FIXED_ENT_IDX) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(ent_hdl,
                                               ent_info[j].key_name,
                                               ent_info[j].index));
                }
            } else {
                opcode = BCMLT_OPCODE_INSERT;
            }

            /* Add fields. */
            for (k = 0; k < fld_num; k++) {
                rv = bcmlt_entry_field_add(ent_hdl,
                                           fld_info[k].name,
                                           fld_info[k].val);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
            }

            rv = bcmi_lt_entry_commit(unit, ent_hdl, opcode,
                                      BCMLT_PRIORITY_NORMAL);
            if (SHR_SUCCESS(rv)) {
                /* Allocate index dynamically. */
                if (ent_info[j].index != -1) {

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(ent_hdl,
                                               ent_info[j].key_name,
                                               &index));
                    /* Set index into dynamic information per unit. */
                    tbl_info[i].ent_info[j].index = (int)index;
                }
            } else {
                /* Associated LT is not mapped. */
                if ((rv != SHR_E_CONFIG) && (rv != SHR_E_UNAVAIL)) {
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
            }

            (void) bcmlt_entry_free(ent_hdl);
            ent_hdl = BCMLT_INVALID_HDL;
        }
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all default entries in strength profile table.
 *
 * \param [in] unit      Unit Number.
 * \param [in] sbr_db    SBR database.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
sbr_profile_tbl_delete(int unit, bcmint_sbr_db_t *sbr_db)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const bcmint_sbr_profile_ent_t *db_ent_info = NULL;
    const bcmint_sbr_profile_tbl_t *db_tbl_info = NULL;
    sbr_profile_tbl_t *tbl_info = NULL;
    uint32_t use_cnt = 0;
    int i = 0, j = 0;
    int ent_num = 0;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Get strength profile database. */
    db_tbl_info = sbr_db->profile_tbl_db;
    tbl_info = sbr_info[unit].profile_tbl_info;

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {

        if (!SHR_BITGET(sbr_db->profile_tbl_bmp, i)) {
            continue;
        }

        /* All entries have been cleared before SBR de-init. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_inuse_cnt_get(unit, db_tbl_info[i].name, &use_cnt));

        if (use_cnt == 0) {
            continue;
        }

        db_ent_info = db_tbl_info[i].ent_info;
        ent_num = db_tbl_info[i].ent_num;

        for (j = 0; j < ent_num; j++) {

            /* No LT operation for dummy shared entry. */
            if (db_ent_info[j].shr_ent != NULL) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, db_tbl_info[i].name, &ent_hdl));

            /* No key index for the LT which size is just 1 entry. */
            if (db_ent_info[j].index != -1) {

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(ent_hdl,
                                           db_ent_info[j].key_name,
                                           tbl_info[i].ent_info[j].index));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL));

            (void) bcmlt_entry_free(ent_hdl);
            ent_hdl = BCMLT_INVALID_HDL;

        }
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the index of dummy entry (sharing entry with another LT).
 *
 * \param [in] unit      Unit Number.
 * \param [in] sbr_db    SBR database.
 *
 * \retval none
 */
static void
sbr_profile_shr_ent_index_update(int unit, bcmint_sbr_db_t *sbr_db)
{
    const bcmint_sbr_profile_tbl_t *db_tbl_info = NULL;
    bcmint_sbr_profile_ent_t *ent_info = NULL;
    bcmi_ltsw_sbr_profile_ent_type_t shr_ent_type;
    bcmi_ltsw_sbr_profile_tbl_hdl_t shr_tbl_hdl;
    sbr_profile_tbl_t *tbl_info = NULL;
    int i = 0, j = 0, k = 0;
    int ent_num = 0;
    int shr_ent_num = 0;


    /* Get strength profile database. */
    db_tbl_info = sbr_db->profile_tbl_db;
    tbl_info = sbr_info[unit].profile_tbl_info;

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {

        if (!SHR_BITGET(sbr_db->profile_tbl_bmp, i)) {
            continue;
        }

        ent_info = db_tbl_info[i].ent_info;
        ent_num = db_tbl_info[i].ent_num;

        for (j = 0; j < ent_num; j++) {

            if (ent_info[j].shr_ent == NULL) {
                continue;
            }

            shr_ent_type = ent_info[j].shr_ent->ent_type;
            shr_tbl_hdl = ent_info[j].shr_hdl;
            shr_ent_num = db_tbl_info[shr_tbl_hdl].ent_num;

            for (k = 0; k < shr_ent_num; k++) {
                if (shr_ent_type ==
                    db_tbl_info[shr_tbl_hdl].ent_info[k].ent_type) {
                    tbl_info[i].ent_info[j].index =
                        tbl_info[shr_tbl_hdl].ent_info[k].index;
                    break;
                }
            }
        }
    }

    return;
}

/*!
 * \brief Recover SBR index which is allocated dynamically.
 *
 * \param [in] unit      Unit Number.
 * \param [in] sbr_db    SBR database.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
sbr_profile_ent_index_recover(int unit, bcmint_sbr_db_t *sbr_db)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const bcmint_sbr_profile_tbl_t *db_tbl_info = NULL;
    bcmint_sbr_profile_ent_t *ent_info = NULL;
    const bcmint_sbr_profile_fld_t *fld_info = NULL;
    sbr_profile_tbl_t *tbl_info = NULL;
    int i = 0, j = 0, k = 0;
    int rv = SHR_E_NONE;
    uint64_t value = 0;
    int ent_num = 0;
    int fld_num = 0;
    int dunit = 0;
    int ent_found = 0;
    uint32_t inuse_cnt = 0;
    uint32_t status = 0;

    SHR_FUNC_ENTER(unit);

    /* Get strength profile database. */
    db_tbl_info = sbr_db->profile_tbl_db;
    tbl_info = sbr_info[unit].profile_tbl_info;

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {

        if (!SHR_BITGET(sbr_db->profile_tbl_bmp, i)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_inuse_cnt_get(unit,
                                         db_tbl_info[i].name, &inuse_cnt));
        if (!inuse_cnt) {
            continue;
        }

        ent_info = db_tbl_info[i].ent_info;
        ent_num = db_tbl_info[i].ent_num;

        /* Traverse LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, db_tbl_info[i].name, &ent_hdl));

        while ((rv = bcmi_lt_entry_commit(
                         unit, ent_hdl,
                         BCMLT_OPCODE_TRAVERSE,
                         BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

            for (j = 0; j < ent_num; j++) {

                /*
                 * No comparison on a dummy entry or a static SBR entry
                 * or a known entry.
                 */
                if ((ent_info[j].shr_ent != NULL) ||
                    (ent_info[j].index == -1) ||
                    (tbl_info[i].ent_info[j].index > 0)) {
                    continue;
                }

                /* No comparison on a disabled entry. */
                if (SHR_SUCCESS(
                        mbcm_ltsw_sbr_ent_status_get(
                        unit, i, ent_info[j].ent_type, &status))) {
                    if (status & BCMINT_SBR_ENT_STATUS_DISABLED) {
                        continue;
                    }
                }

                fld_info = ent_info[j].fld_info;
                fld_num = ent_info[j].fld_num;

                ent_found = 1;

                /* Get fields. */
                for (k = 0; k < fld_num; k++) {

                    rv = bcmlt_entry_field_get(ent_hdl,
                                               fld_info[k].name,
                                               &value);
                    if (rv == SHR_E_NOT_FOUND) {
                        continue;
                    } else {
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                    }

                    if (fld_info[k].val != value) {

                        ent_found = 0;
                    }
                }

                /* Get entry index. */
                if (ent_found) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(ent_hdl,
                                               ent_info[j].key_name,
                                               &value));

                    /* Set index into dynamic information per unit. */
                    tbl_info[i].ent_info[j].index = (int)value;
                }
            }
        }

        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        (void) bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }

    /* Update the index of dummy entry (sharing entry with another LT). */
    sbr_profile_shr_ent_index_update(unit, sbr_db);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();

}

/*!
 * \brief Cleanup the database of SBR module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval None
 */
static void
sbr_db_cleanup(int unit)
{
    sbr_info_t *si = &sbr_info[unit];
    bcmint_sbr_db_t *sbr_db = NULL;

    sbr_db = &(si->sbr_db);

    /* Reset dynamic strength profile entry information per unit. */
    sbr_profile_tbl_info_cleanup(unit);

    SHR_FREE(sbr_db->profile_tbl_bmp);
    SHR_FREE(sbr_db->tbl_bmp);

    sal_memset(sbr_db, 0, sizeof(bcmint_sbr_db_t));

    return;
}

/*!
 * \brief Initialize the database of SBR module.
 *
 * \param [in] unit     Unit Number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
static int
sbr_db_init(int unit)
{
    sbr_info_t *si = &sbr_info[unit];
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    if (si->inited) {
        sbr_db_cleanup(unit);
    }

    alloc_size = SHR_BITALLOCSIZE(BCMI_LTSW_SBR_PTH_COUNT);
    SHR_ALLOC(si->sbr_db.profile_tbl_bmp,
              alloc_size, "bcmLtswProfTblBmp");
    SHR_NULL_CHECK(si->sbr_db.profile_tbl_bmp, SHR_E_MEMORY);
    sal_memset(si->sbr_db.profile_tbl_bmp, 0, alloc_size);

    alloc_size = SHR_BITALLOCSIZE(BCMI_LTSW_SBR_TH_COUNT);
    SHR_ALLOC(si->sbr_db.tbl_bmp,
              alloc_size, "bcmLtswTblBmp");
    SHR_NULL_CHECK(si->sbr_db.tbl_bmp, SHR_E_MEMORY);
    sal_memset(si->sbr_db.tbl_bmp, 0, alloc_size);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize all default strength information.
 *
 * \param [in] unit       Unit Number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
sbr_info_init(int unit)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sbr_db_init(unit));

    /* Get SBR database. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_sbr_db_get(unit, &(si->sbr_db)));

    /* Initialize dynamic strength profile table information per unit. */
    SHR_IF_ERR_VERBOSE_EXIT
        (sbr_profile_tbl_info_init(unit));

    /* Add all default strength profile entries into SDKLT. */
    if (!bcmi_warmboot_get(unit)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (sbr_profile_tbl_max_entries_udpate(unit, &(si->sbr_db)));

        SHR_IF_ERR_VERBOSE_EXIT
            (sbr_profile_tbl_clear(unit, &(si->sbr_db)));

        SHR_IF_ERR_VERBOSE_EXIT
            (sbr_profile_tbl_add(unit, &(si->sbr_db)));

        /* Update the index of dummy entry sharing with another LT. */
        sbr_profile_shr_ent_index_update(unit, &(si->sbr_db));
    } else {

        /* Recover SBR entry index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (sbr_profile_ent_index_recover(unit, &(si->sbr_db)));
    }

exit:
    if (SHR_FUNC_ERR()) {
        sbr_db_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize all default strength information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
sbr_info_deinit(int unit)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    /* Delete all default strength profile entries from SDKLT. */
    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (sbr_profile_tbl_delete(unit, &(si->sbr_db)));
    }

    sbr_db_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get entry index per strength profile table handle and entry type.
 *
 * \param [in] unit       Unit Number.
 * \param [in] tbl_hdl    Handle to strength profile table.
 * \param [in] ent_type   Strength profile entry type.
 * \param [out]index      Strength profile entry index.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
sbr_profile_ent_index_get(int unit,
                          bcmi_ltsw_sbr_profile_tbl_hdl_t tbl_hdl,
                          bcmi_ltsw_sbr_profile_ent_type_t ent_type,
                          int *index)
{
    sbr_profile_tbl_t *tbl_info = NULL;
    sbr_profile_ent_t *ent_info = NULL;
    sbr_info_t *si = &sbr_info[unit];
    int ent_num = 0;
    int ent_idx = -1;
    int i = 0;
    uint32_t status = 0;

    SHR_FUNC_ENTER(unit);

    if (tbl_hdl == BCMI_LTSW_SBR_PTH_COUNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Get strength profile information. */
    tbl_info = si->profile_tbl_info;

    /* Check bitmap of SBR LTs. */
    if (!SHR_BITGET(si->sbr_db.profile_tbl_bmp, tbl_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    ent_info = tbl_info[tbl_hdl].ent_info;
    ent_num = tbl_info[tbl_hdl].ent_num;

    for (i = 0; i < ent_num; i++) {
        if (SHR_SUCCESS(
                mbcm_ltsw_sbr_ent_status_get(
                unit, tbl_hdl, ent_info[i].ent_type, &status))) {
            if (status & BCMINT_SBR_ENT_STATUS_DISABLED) {
                continue;
            }
        }
        if (ent_info[i].ent_type == ent_type) {
            ent_idx = ent_info[i].index;
            break;
        }
    }

    if (ent_idx != -1) {
        *index = ent_idx;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get entry type per strength profile table handle and entry index.
 *
 * \param [in] unit       Unit Number.
 * \param [in] tbl_hdl    Handle to strength profile table.
 * \param [in] index      Strength profile entry index.
 * \param [out]ent_type   Strength profile entry type.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
sbr_profile_ent_type_get(int unit,
                         bcmi_ltsw_sbr_profile_tbl_hdl_t tbl_hdl,
                         int index,
                         bcmi_ltsw_sbr_profile_ent_type_t *ent_type)
{
    sbr_profile_tbl_t *tbl_info = NULL;
    sbr_profile_ent_t *ent_info = NULL;

    sbr_info_t *si = &sbr_info[unit];
    int ent_num = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if (tbl_hdl == BCMI_LTSW_SBR_PTH_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get strength profile information. */
    tbl_info = si->profile_tbl_info;

    /* Check bitmap of SBR LTs. */
    if (!SHR_BITGET(si->sbr_db.profile_tbl_bmp, tbl_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    ent_info = tbl_info[tbl_hdl].ent_info;
    ent_num = tbl_info[tbl_hdl].ent_num;

    /* Find the profile entry type. */
    for (i = 0; i < ent_num; i++) {
        if (ent_info[i].index == index) {
            *ent_type = ent_info[i].ent_type;
            break;
        }
    }

    if (i == ent_num) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get strength field value per table handle and field type.
 *
 * \param [in] unit     Unit Number.
 * \param [in] sbr_db   SBR database.
 * \param [in] tbl_hdl  Handle to table which has strength field.
 * \param [in] fld_type Field type.
 * \param [out] value   Strength value.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
static int
sbr_fld_value_get(int unit,
                  bcmint_sbr_db_t *sbr_db,
                  bcmi_ltsw_sbr_tbl_hdl_t tbl_hdl,
                  bcmi_ltsw_sbr_fld_type_t fld_type,
                  int *value)
{
    const bcmint_sbr_tbl_t *tbl_info = NULL;
    const bcmint_sbr_fld_t *fld_info = NULL;
    sbr_info_t *si = &sbr_info[unit];
    int fld_num = 0;
    int val = -1;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if (tbl_hdl == BCMI_LTSW_SBR_TH_COUNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Get the database to tables which have strength field. */
    tbl_info = sbr_db->tbl_db;

    /* Check bitmap of LTs which have strength field. */
    if (!SHR_BITGET(si->sbr_db.tbl_bmp, tbl_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    fld_info = tbl_info[tbl_hdl].fld_info;
    fld_num = tbl_info[tbl_hdl].fld_num;

    /* Find the profile entry. */
    for (i = 0; i < fld_num; i++) {
        if (fld_info[i].type == fld_type) {
            val = fld_info[i].val;
            break;
        }
    }

    if (val != -1) {
        *value = val;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get strength field type per table handle and field value.
 *
 * \param [in] unit     Unit Number.
 * \param [in] sbr_db   SBR database.
 * \param [in] tbl_hdl  Handle to table which has strength field.
 * \param [in] value    Strength value.
 * \param [out]fld_type Field type.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
static int
sbr_fld_type_get(int unit,
                 bcmint_sbr_db_t *sbr_db,
                 bcmi_ltsw_sbr_tbl_hdl_t tbl_hdl,
                 int value,
                 bcmi_ltsw_sbr_fld_type_t *fld_type)
{
    const bcmint_sbr_tbl_t *tbl_info = NULL;
    const bcmint_sbr_fld_t *fld_info = NULL;
    sbr_info_t *si = &sbr_info[unit];
    int fld_num = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if (tbl_hdl == BCMI_LTSW_SBR_TH_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the database to tables which have strength field. */
    tbl_info = sbr_db->tbl_db;

    /* Check bitmap of LTs which have strength field. */
    if (!SHR_BITGET(si->sbr_db.tbl_bmp, tbl_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    fld_info = tbl_info[tbl_hdl].fld_info;
    fld_num = tbl_info[tbl_hdl].fld_num;

    /* Find the field type. */
    for (i = 0; i < fld_num; i++) {
        if (fld_info[i].val == value) {
            *fld_type = fld_info[i].type;
            break;
        }
    }

    if (i == fld_num) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public functions
 */

/*!
 * \brief De-initialize the resource of SBR module.
 *
 * \param [in] unit     Unit Number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcmi_ltsw_sbr_detach(int unit)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    si->inited = 0;

    SHR_IF_ERR_CONT
        (sbr_info_deinit(unit));

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the resource of SBR module.
 *
 * \param [in] unit     Unit Number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcmi_ltsw_sbr_init(int unit)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (sbr_info_init(unit));

    si->inited = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Display SBR software structure information.
 *
 * \retval None
 */
void
bcmi_ltsw_sbr_sw_dump(int unit)
{
    const char *pth_str[BCMI_LTSW_SBR_PTH_COUNT] = BCMI_LTSW_SBR_PTH_STR;
    const char *pet_str[BCMI_LTSW_SBR_PET_COUNT] = BCMI_LTSW_SBR_PET_STR;
    sbr_profile_tbl_t *tbl_info = NULL;
    sbr_profile_ent_t *ent_info = NULL;
    sbr_info_t *si = &sbr_info[unit];
    int i = 0, j = 0, ent_num = 0;

    if (!si->inited) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information SBR - Unit %d\n"), unit));

    /* Get strength profile information. */
    tbl_info = si->profile_tbl_info;

    for (i = 0; i < BCMI_LTSW_SBR_PTH_COUNT; i++) {

        if (!SHR_BITGET(si->sbr_db.profile_tbl_bmp, i)) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit,
                            "\n  Table - %s\n"), pth_str[i]));

        ent_info = tbl_info[i].ent_info;
        ent_num = tbl_info[i].ent_num;

        for (j = 0; j < ent_num; j++) {
            LOG_CLI((BSL_META_U(unit,
                                "    Entry: Index = %2d, Type = %s\n"),
                                ent_info[j].index,
                                pet_str[ent_info[j].ent_type]));
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n")));

    return;
}

/*!
 * \brief Get entry index per strength profile table handle and entry type.
 *
 * This function is used to get strength profile entry index for specified
 * strength table handle and entry type. This function can be called in case
 * that the LT table points to a strength profile.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to strength profile table.
 * \param [in] ent_type Strength profile entry type.
 * \param [out] index   Strength profile entry index.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcmi_ltsw_sbr_profile_ent_index_get(int unit,
                                    bcmi_ltsw_sbr_profile_tbl_hdl_t tbl_hdl,
                                    bcmi_ltsw_sbr_profile_ent_type_t ent_type,
                                    int *index)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!si->inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(index, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (sbr_profile_ent_index_get(unit, tbl_hdl, ent_type, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get entry type per strength profile table handle and entry index.
 *
 * This function is used to get strength profile entry type for specified
 * strength table handle and entry index. This function can be called in case
 * that the LT table points to a strength profile.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to strength profile table.
 * \param [in] index    Strength profile entry index.
 * \param [out]ent_type Strength profile entry type.

 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcmi_ltsw_sbr_profile_ent_type_get(int unit,
                                   bcmi_ltsw_sbr_profile_tbl_hdl_t tbl_hdl,
                                   int index,
                                   bcmi_ltsw_sbr_profile_ent_type_t *ent_type)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!si->inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ent_type, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (sbr_profile_ent_type_get(unit, tbl_hdl, index, ent_type));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get strength field value per table handle and field type.
 *
 * This function is used to get strength field value per table handle and
 * field type. This function can be called on the configuration of LT table
 * which has strength field.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to table which has strength field.
 * \param [in] fld_type Field type.
 * \param [out] value   Strength value.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcmi_ltsw_sbr_fld_value_get(int unit,
                            bcmi_ltsw_sbr_tbl_hdl_t tbl_hdl,
                            bcmi_ltsw_sbr_fld_type_t fld_type,
                            int *value)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!si->inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (sbr_fld_value_get(unit, &(si->sbr_db), tbl_hdl, fld_type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get strength field type per table handle and field value.
 *
 * This function is used to get strength field type per table handle and
 * field index. This function can be called on the configuration of LT table
 * which has strength field.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to table which has strength field.
 * \param [in] value    Strength value.
 * \param [out] fld_type Field type.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
int
bcmi_ltsw_sbr_fld_type_get(int unit,
                           bcmi_ltsw_sbr_tbl_hdl_t tbl_hdl,
                           int value,
                           bcmi_ltsw_sbr_fld_type_t *fld_type)
{
    sbr_info_t *si = &sbr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!si->inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(fld_type, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (sbr_fld_type_get(unit, &(si->sbr_db), tbl_hdl, value, fld_type));

exit:
    SHR_FUNC_EXIT();
}
