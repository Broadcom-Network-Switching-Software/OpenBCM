/*! \file bcmltm_lta_intf.c
 *
 * LTA/LTM interface accessors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlta/bcmlta_ltm_intf_conf.h>
#include <bcmptm/bcmptm_table_intf.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_info_internal.h>


#include <bcmptm/bcmptm_itx.h>

/*******************************************************************************
 * Local definitions
 */

/*
 * PTM handler for all direct mapped modeled keyed LTs.
 */
static const bcmltd_table_entry_limit_handler_t
lta_ptm_intf_model_keyed_table_entry_limit_handler = {
    .entry_limit_get = bcmptm_mreq_keyed_table_entry_limit_get,
    .arg      = NULL
};

/*
 * Pointer to per-device configuration. This may be overridden for
 * test purposes.
 */
const bcmltm_lta_device_conf_t bcmltm_default_lta_device_conf =
{
    .ltm_lta_intf_config = bcmltm_default_lta_conf,
};

bcmltm_lta_device_conf_t const *bcmltm_lta_device_conf = &bcmltm_default_lta_device_conf;

/*
 * Pointer to per-device LTM configuration. This may be overridden for
 * test purposes.
 */
bcmltd_ltm_intf_t const **bcmltm_lta_conf = bcmltm_default_lta_conf;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Return pointer to LTM interfaces.
 *
 * Return the LTM interface structure for the given unit. Every supported
 * unit will have such a structure, but pointers to specific interfaces may
 * be NULL if there are no such interfaces for the given unit.
 *
 * Note that LTA dispatch is on DRD device-type, not device variant.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval pointer to LTM interface structure
 */
static const bcmltd_ltm_intf_t *
bcmltm_lta_intf_conf_get(int unit)
{
    bcmlrd_variant_t variant;

    variant = bcmlrd_variant_get(unit);
    return bcmltm_lta_conf[variant];
}

/*!
 * \brief Check if table is a modeled keyed LT.
 *
 * This routine checks if given table ID is a modeled keyed LT.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 *
 * \retval TRUE Table is a modeled keyed LT.
 * \retval FALSE Table is a not a modeled keyed LT or failure.
 */
static bool
table_is_model_keyed(int unit, bcmlrd_sid_t sid)
{
    int rv;
    bcmltm_table_mode_t mode;
    bcmltm_table_type_t type;

    /* Get table operating mode */
    rv = bcmltm_info_table_mode_get(unit, sid, &mode);
    if (SHR_FAILURE(rv) || !BCMLTM_TABLE_MODE_IS_MODELED(mode)) {
        /* Error or not a modeled LT */
        return FALSE;
    }

    /* Get table type */
    rv = bcmltm_info_table_type_get(unit, sid, &type);
    if (SHR_FAILURE(rv) || !BCMLTM_TABLE_TYPE_IS_KEYED(type)) {
        /* Error or not a keyed LT */
        return FALSE;
    }

    /* This is a modeled keyed table */
    return TRUE;
}

/*!
 * \brief Get the custom table handler by table ID.
 *
 * This routine returns the custom table handler by logical table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 *
 * \retval Handler.
 * \retval NULL ERROR.
 */
static const bcmltd_table_handler_t *
lta_intf_table_handler_get_by_sid(int unit,
                                  uint32_t sid)
{
    const bcmltd_table_handler_t *handler = NULL;
    uint32_t handler_id;
    int rv;

    /* Get CTH handler ID */
    rv = bcmltm_info_table_cth_handler_id_get(unit, sid, &handler_id);

    if (SHR_SUCCESS(rv)) {
        /* Get CTH handler */
        handler = bcmltm_lta_intf_table_handler_get(unit, handler_id);
    }

    return handler;
}

/*!
 * \brief Get the table entry limit handler defined in map attribute.
 *
 * This routine returns the table entry limit handler specified
 * by the table map attribute.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 *
 * \retval Handler.
 * \retval NULL No handler map attribute specified or ERROR.
 */
static const bcmltd_table_entry_limit_handler_t *
lta_intf_table_entry_limit_handler_attr_get(int unit,
                                            uint32_t sid)
{
    const bcmltd_table_entry_limit_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);
    uint32_t handler_id;
    int rv;

    /* Get table entry limit handler ID */
    rv = bcmltm_info_table_entry_limit_handler_id_get(unit, sid, &handler_id);

    if (SHR_SUCCESS(rv)) {
        if (conf != NULL && conf->table_entry_limit->handler != NULL) {
            if (handler_id < conf->table_entry_limit->count) {
                handler = conf->table_entry_limit->handler[handler_id];
            }
        }
    }

    return handler;
}

/*******************************************************************************
 * Public functions
 */

const bcmltd_xfrm_handler_t *
bcmltm_lta_intf_xfrm_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_xfrm_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->xfrm->handler != NULL) {
        if (handler_id < conf->xfrm->count) {
            handler = conf->xfrm->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_field_val_handler_t *
bcmltm_lta_intf_val_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_field_val_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->val->handler != NULL) {
        if (handler_id < conf->val->count) {
            handler = conf->val->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_table_commit_handler_t *
bcmltm_lta_intf_table_commit_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_table_commit_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->table_commit->handler != NULL) {
        if (handler_id < conf->table_commit->count) {
            handler = conf->table_commit->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_table_entry_limit_handler_t *
bcmltm_lta_intf_table_entry_limit_handler_get(int unit, uint32_t sid)
{
    const bcmltd_table_entry_limit_handler_t *handler = NULL;
    const bcmltd_table_handler_t *cth_handler = NULL;
    bcmltm_table_map_type_t map_type;
    int rv;

    /*
     * The table entry limit handler is retrieved using the following
     * logic:
     *   1) If map attribute table_entry_limit is present,
     *      use handler specified in attribute.
     *
     *   2) Else, handler is provided based on the table type
     *      as follows:
     *      - CTH :  LRD CTH handler data (if specified), NULL otherwise.
     *               (this is specified by the 'entry_limit' handler feature).
     *      - DM  :  Dedicated PTM handler for modeled keyed.
     *               NULL for index and non-modeled.
     *      - LTM :  NULL.
     *
     * Additional notes:
     * - In the case where a table has possibly two handlers (1) and (2),
     *   the handler defined by (1) takes precedence.
     * - The map attribute (1) is specified per LT, so it offers
     *   more granularity.
     * - The CTH feature 'entry_limit' (2) is specified per handler,
     *   so it is shared by all LTs that use that CTH handler.
     */

    /* Try to get table map attribute handler first */
    handler = lta_intf_table_entry_limit_handler_attr_get(unit, sid);
    if (handler != NULL) {
        return handler;
    }

    /* Get handler based on table type */
    rv = bcmltm_info_table_map_type_get(unit, sid, &map_type);
    if (SHR_FAILURE(rv)) {
        return handler;
    }

    if (BCMLTM_TABLE_MAP_IS_CTH(map_type)) {
        /* CTH */
        cth_handler = lta_intf_table_handler_get_by_sid(unit, sid);
        if (cth_handler != NULL) {
            handler = cth_handler->entry_limit;
        }

    } else if (BCMLTM_TABLE_MAP_IS_DM(map_type)) {
        /* Direct mapped */

        /* If modeled keyed LT, return PTM modeled keyed entry limit handler */
        if (table_is_model_keyed(unit, sid)) {
            handler = &lta_ptm_intf_model_keyed_table_entry_limit_handler;
        }
    }

    return handler;
}

const bcmltd_table_entry_usage_handler_t *
bcmltm_lta_intf_table_entry_usage_handler_get(int unit, uint32_t sid)
{
    const bcmltd_table_entry_usage_handler_t *handler = NULL;
    const bcmltd_table_handler_t *cth_handler = NULL;
    bcmltm_table_map_type_t map_type;
    int rv;

    /*
     * TABLE entry usage handler is retrieved as follows:
     * - CTH :  LRD CTH handler data (if specified), NULL otherwise.
     *          (this is specified by the 'entry_limit' handler feature).
     * - DM  :  NULL.
     * - LTM :  NULL.
     */
    rv = bcmltm_info_table_map_type_get(unit, sid, &map_type);
    if (SHR_FAILURE(rv)) {
        return handler;
    }

    if (BCMLTM_TABLE_MAP_IS_CTH(map_type)) {
        /* CTH */
        cth_handler = lta_intf_table_handler_get_by_sid(unit, sid);
        if (cth_handler != NULL) {
            handler = cth_handler->entry_usage;
        }
    }

    return handler;
}

const bcmltd_table_handler_t *
bcmltm_lta_intf_table_handler_get(int unit, uint32_t handler_id)
{
    const bcmltd_table_handler_t *handler = NULL;
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);

    if (conf != NULL && conf->table->handler != NULL) {
        if (handler_id < conf->table->count) {
            handler = conf->table->handler[handler_id];
        }
    }

    return handler;
}

const bcmltd_table_lifecycle_intf_t *
bcmltm_lta_intf_table_lifecycle_get(int unit)
{
    const bcmltd_ltm_intf_t *conf = bcmltm_lta_intf_conf_get(unit);
    const bcmltd_table_lifecycle_intf_t *intf = NULL;

    if (conf != NULL) {
        intf = conf->table_lc;
    }

    return intf;
}

const bcmltd_ltm_intf_t **
bcmltm_lta_intf_set(const bcmltd_ltm_intf_t **lta_conf)
{
    const bcmltd_ltm_intf_t **old = bcmltm_lta_conf;
    bcmltm_lta_conf = lta_conf;

    return old;
}


bool
table_is_itile_index(int unit, bcmlrd_sid_t sid)
{
    int rv;
    const bcmptm_tile_list_t *tile_list = NULL;
    bool itile = FALSE;
    uint32_t i;

    /* Get itile table list */
    rv = bcmptm_itx_tile_list_get(unit, &tile_list);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }

    if (tile_list != NULL) {
        for (i = 0; i < tile_list->tile_info_count; i++) {
            if (tile_list->tile_info[i].sid == sid) {
                itile = TRUE;
            }
        }
    }

    return itile;
}
