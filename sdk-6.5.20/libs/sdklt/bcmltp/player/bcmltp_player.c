/*! \file bcmltp_player.c
 *
 * BCMLTP Player
 *
 * The LTP player "plays back" logical and physical table
 * configurations recorded by BCMCFG. This is not intended as an SDK
 * application interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_player.h>
#include <bcmcfg/comp/bcmcfg_cfg_error_control.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtrm/trm_api.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltp/bcmltp_player.h>

/*******************************************************************************
 * Local definitions
 */

/*
 * Many applications cannot tolerate initialization errors caused by
 * errors in playback, but still want some visual indication of the
 * error.  For those applications that prefer initialization to fail
 * if there is a TRM operation error, set the following flag to 1.
 */
#ifndef RETURN_ENTRY_STATUS_ERROR
#define RETURN_ENTRY_STATUS_ERROR 0
#endif

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/*!
 * \brief Player function.
 *
 * Callback function to play back logical or physical table data.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     user_data    User data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmltp_play_f)(int unit,
                             const char *locus,
                             const bcmcfg_tbl_list_t *tlist,
                             void *user_data);

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check field lists.
 *
 * Check input field list.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical/Physical Table ID.
 * \param [in]     field_in     Field list.
 *
 * \retval 0      OK
 * \retval !0     ERROR
 */
static int
bcmltp_entry_field_check(int unit,
                         uint32_t sid,
                         shr_fmm_t *field_in)
{
    int rv;
    uint32_t error = 0;

    SHR_FUNC_ENTER(unit);

    while (field_in) {
        bool unmapped = false;

        rv = bcmlrd_field_is_unmapped(unit,
                                      sid,
                                      field_in->id,
                                      &unmapped);
        if (SHR_FAILURE(rv)) {
            error++;
            break;
        }

        if (unmapped) {
            const bcmltd_table_rep_t *tbl = NULL;
            tbl = bcmltd_table_get(sid);
            if (tbl) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Unmapped field %s in table %s "
                                    "is used for entry playback.\n"),
                          tbl->field[field_in->id].name, tbl->name));
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Failed to get information for table %u\n"),
                          sid));
            }
            error++;
        }
        /* Next input field. */
        field_in = field_in->next;
    }

    if (error) {
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a TRM entry for playback
 *
 * Allocate a TRM entry and copy input fields for playback via TRM.
 * Note that the TRM interface assumes that field data is allocated
 * via shr_fmm_alloc(), and will be freed by bcmtrm_entry_free().
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical/Physical Table ID.
 * \param [in]     name         Logical table name.
 * \param [in]     interactive  True if table is interactive.
 * \param [in]     field_in     Playback field list.
 * \param [out]    entry_p      Pointer to TRM entry.
 *
 * \retval 0      OK
 * \retval !0     ERROR
 */
static int
bcmltp_entry_alloc(int unit,
                   uint32_t sid,
                   const char *name,
                   bool interactive,
                   shr_fmm_t *field_in,
                   bcmtrm_entry_t **entry_p)
{
    bcmtrm_entry_t *entry;
    SHR_FUNC_ENTER(unit);

    do {
        shr_fmm_t *field;

        entry = bcmtrm_entry_alloc(unit, sid, interactive, false, NULL, name);
        if (entry == NULL) {
            break;
        }

        while (field_in) {
            /* Allocate field data. */
            field = shr_fmm_alloc();
            if (field == NULL) {
                int rv;

                /* Handle field allocation error - error in
                   bcmtrm_entry_free() */
                rv = bcmtrm_entry_free(entry);
                entry = NULL;
                if (SHR_FAILURE(rv)) {
                    SHR_IF_ERR_CONT(rv);
                } else {
                    SHR_IF_ERR_CONT(SHR_E_MEMORY);
                }
                break;
            }
            /* Set field data. */
            field->id = field_in->id;
            field->flags = field_in->flags;
            field->data = field_in->data;
            field->idx = field_in->idx;
            field->next = entry->l_field;

            /* Link into entry. */
            entry->l_field = field;

            /* Next input field. */
            field_in = field_in->next;
        }

    } while (0);

    *entry_p = entry;

    SHR_FUNC_EXIT();
}



/*!
 * \brief Format LT TRM entry.
 *
 * Return an allocated string with a formatted LT TRM entry.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     notice       Notice string.
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     entry        TRM entry.
 *
 * \retval !NULL string
 * \retval NULL  ERROR
 */
static char *
bcmltp_lt_entry_str(int unit,
                    const char *notice,
                    const char *locus,
                    const bcmcfg_tbl_list_t *tlist,
                    bcmtrm_entry_t *entry)
{
    size_t len = 32768;
    char *str = sal_alloc(len, "bcmltpEntryStr");
    const bcmlrd_table_rep_t *tbl = bcmlrd_table_get(entry->table_id);
    if (str != NULL) {
        if (tbl != NULL) {
            shr_fmm_t *field = entry->l_field;
            char *p = str;
            int n;
            *p = 0;
            /* Error locus. */
            n = sal_snprintf(p, len-1, "%s:%d:%d: %s\n",
                             locus, tlist->line, tlist->column, notice);
            p += n;
            len -= n;
            /* opcode entry->opcode.lt_opcode */
            n = sal_snprintf(p, len-1, "op:%d\n", entry->opcode.lt_opcode);
            p += n;
            len -= n;
            /* name */
            n = sal_snprintf(p, len-1, "tbl:%s\n", entry->info.table_name);
            p += n;
            len -= n;
            /* fields entry->l_field  */
            while (field != NULL) {
                n = sal_snprintf(p, len-1, "   %s[%u] %"PRIu64"\n",
                                 tbl->field[field->id].name,
                                 field->idx,
                                 field->data);
                p += n;
                len -= n;
                field = field->next;
            }
        } else {
            sal_snprintf(str, len-1, "%s:%d:%d: %s cannot resolve LT %d\n",
                         locus, tlist->line, tlist->column, notice,
                         entry->table_id);
        }
    }
    return str;
}

/*!
 * \brief Format PT TRM entry.
 *
 * Return an PT field name string or BCMCFG special field target.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          DRD symbol ID.
 * \param [in]     fid          DRD field ID.
 *
 * \retval !NULL string
 * \retval NULL  ERROR
 */
static const char *
bcmltp_pt_field_name(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    const char *name = "???"; /* In case name is unresolvable. */
    bcmdrd_sym_field_info_t finfo;

    switch (fid) {
    case BCMCFG_PT_FIELD_KEY_INDEX:
        name = "__INDEX";
        break;
    case BCMCFG_PT_FIELD_KEY_PORT:
        name = "__PORT";
        break;
    default:
        rv = bcmdrd_pt_field_info_get(unit,
                                      sid,
                                      fid,
                                      &finfo);
        if (SHR_SUCCESS(rv)) {
            name = finfo.name;
        }
        break;
    }

    return name;
}

/*!
 * \brief Format PT TRM entry.
 *
 * Return an allocated string with a formatted PT TRM entry.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     notice       Notice string.
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     entry        TRM entry.
 *
 * \retval !NULL string
 * \retval NULL  ERROR
 */
static char *
bcmltp_pt_entry_str(int unit,
                    const char *notice,
                    const char *locus,
                    const bcmcfg_tbl_list_t *tlist,
                    bcmtrm_entry_t *entry)
{
    size_t len = 32768;
    char *str = sal_alloc(len, "bcmltpEntryStr");
    const char *name = bcmdrd_pt_sid_to_name(unit, entry->table_id);

    if (str != NULL) {
        if (name != NULL) {
            shr_fmm_t *field = entry->l_field;
            char *p = str;
            int n;
            *p = 0;
            /* Error locus. */
            n = sal_snprintf(p, len-1, "%s:%d:%d: %s\n",
                             locus, tlist->line, tlist->column, notice);
            p += n;
            len -= n;
            /* opcode entry->opcode.lt_opcode */
            n = sal_snprintf(p, len-1, "op:%d\n", entry->opcode.lt_opcode);
            p += n;
            len -= n;
            /* name */
            n = sal_snprintf(p, len-1, "pt:%s\n", entry->info.table_name);
            p += n;
            len -= n;
            /* fields entry->l_field  */
            while (field != NULL) {
                n = sal_snprintf(p, len-1, "   %s[%u] %"PRIu64"\n",
                                 bcmltp_pt_field_name(unit,
                                                      entry->table_id,
                                                      field->id),
                                 field->idx,
                                 field->data);
                p += n;
                len -= n;
                field = field->next;
            }
        } else {
            sal_snprintf(str, len-1, "%s:%d:%d: %s cannot resolve PT %d\n",
                         locus, tlist->line, tlist->column, notice,
                         entry->table_id);
        }
    }
    return str;
}


/*!
 * \brief Show entry
 *
 * Show the TRM entry and configuration locus.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     entry        TRM entry.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_entry_show(int unit,
                  const char *locus,
                  const bcmcfg_tbl_list_t *tlist,
                  bcmtrm_entry_t *entry)
{
    char *str;

    SHR_FUNC_ENTER(unit);

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        if (entry->pt) {
            str = bcmltp_pt_entry_str(unit, "show", locus, tlist, entry);
        } else {
            str = bcmltp_lt_entry_str(unit, "show", locus, tlist, entry);
        }
        if (str) {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s"), str));
            sal_free(str);
        } else {
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Check bcmltp playback error control.
 *
 * Return true if playback needs to stop on TRM error,
 * otherwise return false.
 *
 * \retval 1  Needs to return error.
 * \retval 0  No need to return error.
 */
static uint32_t
bcmltp_stop_playback_on_error(void)
{
    bcmcfg_cfg_error_control_config_t ctrl;
    uint32_t stop = RETURN_ENTRY_STATUS_ERROR;
    int rv;

    rv = bcmcfg_cfg_error_control_config_get(&ctrl);
    if (SHR_SUCCESS(rv)) {
        stop = ctrl.stop_playback_on_error;
    }

    return stop;
}

/*!
 * \brief Play back LT data
 *
 * Play back BCMCFG Logical table data. Data is written by first
 * trying an insert, and if the insert says the entry exists, then try
 * an update. This avoid duplicating reference data in playback.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     user_data    Player user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_lt_play(int unit,
               const char *locus,
               const bcmcfg_tbl_list_t *tlist,
               void *user_data)
{
    bcmtrm_entry_t *entry;
    int rv;
    int rv1;
    uint32_t sid = tlist->sid;
    const char *name = tlist->name;
    shr_fmm_t *field = tlist->field;
    bool interactive = false;
    const bcmlrd_map_t *map;
    bool update_only = false;
    char *str;

    SHR_FUNC_ENTER(unit);

    do {
        rv = bcmlrd_map_get(unit, sid, &map);

        /* Sanity check map data. */
        if (SHR_FAILURE(rv)) {
            /* Not mapped.*/
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Table not mapped.\n")));
            SHR_IF_ERR_CONT(rv);
            break;
        }

        if (map == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Mapped table is NULL.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        rv = bcmltp_entry_field_check(unit, sid, field);
        if (SHR_FAILURE(rv)) {
            if (bcmltp_stop_playback_on_error()) {
                SHR_IF_ERR_CONT(rv);
            }
            break;
        }

        rv = bcmlrd_table_interactive_get(unit, sid, &interactive);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        rv = bcmltp_entry_alloc(unit, sid, name, interactive, field, &entry);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        entry->opcode.lt_opcode = BCMLT_OPCODE_INSERT;

        if (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_INSERT) {
            rv = bcmtrm_entry_req(entry);
        } else if (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_UPDATE) {
            update_only = true;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Table does not allow insert or update.\n")));
            /* This should have been caught during parsing. */
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        if ((SHR_SUCCESS(rv) && entry->info.status == SHR_E_EXISTS) ||
            update_only) {
            /* Either assume an entry has been inserted, so just
               update, or if it is update_only, then update. */
            entry->opcode.lt_opcode = BCMLT_OPCODE_UPDATE;
            rv = bcmtrm_entry_req(entry);
        }

        if (SHR_FAILURE(rv) || SHR_FAILURE(entry->info.status)) {
            str = bcmltp_lt_entry_str(unit, "error", locus, tlist, entry);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Cannot %s entry.\n%s\n"),
                       (entry->opcode.lt_opcode == BCMLT_OPCODE_UPDATE)?
                       "update":"insert", str));
            if (bcmltp_stop_playback_on_error() &&
                SHR_SUCCESS(rv)) {
                /* Must have been status error. */
                SHR_IF_ERR_CONT(entry->info.status);
            }
            sal_free(str);
        } else {
            rv = bcmltp_entry_show(unit, locus, tlist, entry);
        }

        SHR_IF_ERR_CONT(rv);
        rv1 = bcmtrm_entry_free(entry);
        if (SHR_SUCCESS(rv)) {
            /* Don't let TRM success cover up LT play failure. */
            SHR_IF_ERR_CONT(rv1);
        }
    } while (0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Play back PT data
 *
 * Play back BCMCFG Physical table data. No user data needed.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     user_data    Player user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_pt_play(int unit,
               const char *locus,
               const bcmcfg_tbl_list_t *tlist,
               void *user_data)
{
    bcmtrm_entry_t *entry;
    uint32_t sid = tlist->sid;
    const char *name = tlist->name;
    shr_fmm_t *field = tlist->field;
    int rv;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(user_data);

    do {
        rv = bcmltp_entry_alloc(unit, sid, name, true, field, &entry);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        entry->opcode.pt_opcode = BCMLT_PT_OPCODE_SET;
        entry->pt = true;

        rv = bcmtrm_entry_req(entry);
        if (SHR_FAILURE(rv)) {
            char *str = bcmltp_pt_entry_str(unit, "error", locus, tlist, entry);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Cannot set entry.\n%s\n"), str));
            sal_free(str);
        } else {
            rv = bcmltp_entry_show(unit, locus, tlist, entry);
        }
        SHR_IF_ERR_CONT(rv);

        rv = bcmtrm_entry_free(entry);
        SHR_IF_ERR_CONT(rv);
    } while (0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Play back BCMCFG table data
 *
 * Play back BCMCFG table data for the given player.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     player       Player function.
 * \param [in]     user_data    Player user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_tlist_play(int unit,
                  const char *locus,
                  const bcmcfg_tbl_list_t *tlist,
                  bcmltp_play_f player,
                  void *user_data)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (tlist) {
        rv = player(unit, locus, tlist, user_data);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
        tlist = tlist->next;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Play back BCMCFG data
 *
 * Play back BCMCFG data for the given player.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     list         Playback list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_list_play(int unit, const bcmcfg_playback_list_t *list)
{
    int rv;
    const char *locus;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (list) {
        locus = list->locus ? list->locus : "<unknown>";

        rv = bcmltp_tlist_play(unit, locus, list->lt, bcmltp_lt_play, NULL);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        rv = bcmltp_tlist_play(unit, list->locus, list->pt, bcmltp_pt_play, NULL);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
        list = list->next;
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

/* Play back BCMCFG data. */
int bcmltp_play(int unit)
{
    int rv;
    const bcmcfg_playback_list_t *list = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        rv = bcmcfg_playback_get(unit, &list);
        if (rv == SHR_E_UNAVAIL) {
            /* Unavailability is a verbose message, not warning. */
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            /* No break here because static analysis says it is unreachable. */
        }
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
        rv = bcmltp_list_play(unit, list);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
    } while(0);

 exit:
    SHR_FUNC_EXIT();
}
