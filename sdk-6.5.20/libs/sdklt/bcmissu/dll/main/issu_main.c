/*! \file issu_main.c
 *
 * ISSU main DLL API module
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmissu/issu_types.h>
#include <bcmissu/issu_internal.h>

/*******************************************************************************
 * Local definitions
 */
/* macros/defines */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_DLL

typedef struct resized_struct_s {
    bcmissu_struct_id_t id;
    struct resized_struct_s *next;
} resized_struct_t;

static const char *start_from_ver;
static const char *current_ver;
static bool native_env;

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Retrieve the index of given version.
 *
 * This function searches the issu_struct_version for the version DB that
 * has matching version to the one provided as input.
 *
 * \param [in] start_ver The version to search for.
 * \param [out] idx1 The index in issu_struct_version where the DB of the
 * requested version can be found.
 * \param [out] idx2 The index in issu_struct_version of the next version.
 *
 * \return SHR_E_NONE on success
 * \return SHR_E_FAIL If the version was not found.
 * \return SHR_E_NOT_FOUND when the next version can't be found.
 */
static int retrieve_versions_struct(const char *start_ver,
                                    uint32_t *idx1,
                                    uint32_t *idx2)
{
    int len = sal_strlen(start_ver);
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    for (*idx1 = 0; issu_struct_dict[*idx1].ver_db != NULL; (*idx1)++) {
        if (sal_strncmp(start_ver,
                        issu_struct_dict[*idx1].version,
                        len) == 0) {
            break;
        }
    }
    if (!issu_struct_dict[*idx1].ver_db) {
        return SHR_E_FAIL;
    }

    /*
     * The assumption is that the entries are sorted in issu_struct_version
     * in accending order where the lowest version is at index 0.
     */
    if (issu_struct_dict[*idx1+1].ver_db) {
        *idx2 = *idx1 + 1;
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

/*!
 *\brief Invoke component callback.
 *
 * This function receives an array of component action infomation,
 * which includes a callback function. It then
 * invokes each callback function for the components in the list.
 *
 * \param [in] unit The unit associated with the upgrade.
 * \param [in] ca_info A structure of component action information.
 * \param [in] phase The phase of the upgrade.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int component_upgrade(int unit,
                             const bcmissu_comp_action_hlr_t *ca_info_list,
                             bcmissu_phases_t phase)
{
    int rv;
    shr_sysm_categories_t cat;
    int j;

    cat = (unit == BCMISSU_NO_UNIT ? SHR_SYSM_CAT_GENERIC : SHR_SYSM_CAT_UNIT);

    for (j = 0; ca_info_list[j].func; j++) {
        rv = ca_info_list[j].func(unit, cat, phase);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ISSU component %s upgrade failed: %s\n"),
                       ca_info_list[j].component_name, shr_errmsg(rv)));
            return rv;
        }
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions local to ISSU
 */
const issu_struct_t *bcmissu_find_oldest_struct(bcmissu_struct_id_t id)
{
    uint32_t start_idx, next_idx;
    const char *ver = start_from_ver;
    int rv;
    int j;
    const issu_struct_db_t *from;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    rv = retrieve_versions_struct(ver,  &start_idx,  &next_idx);
    while (rv == SHR_E_NONE) {
        from = issu_struct_dict[start_idx].ver_db;
        for (j = 0; from[j].struct_info != NULL; j++) {
            if (from[j].id == id) {
                return from[j].struct_info;
            }
        }
        ver = issu_struct_dict[next_idx].version;
        rv = retrieve_versions_struct(ver,  &start_idx,  &next_idx);
    }
    return NULL;
}

/*******************************************************************************
 * Public functions ISSU DLL
 */
int bcmissu_version_update(const char *from_ver,
                           const char *to_ver,
                           bool native)
{
    int j;
    bool first_version = false;
    bool last_version = false;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    /*
     * Validate that these versions exist within the DB. Also make sure that
     * the to_ver matches the last version of the DB.
     */
    for (j = 0; issu_struct_dict[j].ver_db != NULL; j++) {
        if (!first_version &&
            sal_strcmp(from_ver, issu_struct_dict[j].version) == 0) {
            first_version = true;
            continue;
        }

        if (!last_version) {
            if (sal_strcmp(to_ver, issu_struct_dict[j].version) == 0) {
                last_version = true;
            }
        } else { /* last version was set so the for loop must exit */
            return SHR_E_PARAM;
        }
    }

    if (!first_version || !last_version) {
        return SHR_E_PARAM;
    }

    start_from_ver = from_ver;
    current_ver = to_ver;
    native_env = native;

    return SHR_E_NONE;
}

int bcmissu_data_resize(int unit)
{
    uint32_t start_idx, next_idx;
    int rv;
    const char *ver = start_from_ver;
    uint32_t j;
    const issu_struct_db_t *from;
    bcmissu_ha_blk_inc_t *ha_blk_list = NULL;
    bcmissu_ha_blk_inc_t *tmp_ha_blk;
    resized_struct_t *visited_struct = NULL;
    resized_struct_t *struct_it;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);
    SHR_IF_ERR_EXIT(
        bcmissu_ha_ptr_record_all(unit, start_from_ver, current_ver));

    rv = retrieve_versions_struct(ver,  &start_idx,  &next_idx);
    /* This loop will go through all the versions except the last */
    while (rv == SHR_E_NONE) {
        from = issu_struct_dict[start_idx].ver_db;
        for (j = 0; from[j].struct_info != NULL; j++) {
            for (struct_it = visited_struct;
                 struct_it && struct_it->id != from[j].id;
                 struct_it = struct_it->next);

            if (!struct_it) {
                SHR_ALLOC(struct_it, sizeof(*struct_it), "bcmissuStTrack");
                SHR_NULL_CHECK(struct_it, SHR_E_MEMORY);
                struct_it->next = visited_struct;
                struct_it->id = from[j].id;
                visited_struct = struct_it;
                bcmissu_size_update(unit, &from[j], start_idx, &ha_blk_list);
            }
        }
        ver = issu_struct_dict[next_idx].version;
        rv = retrieve_versions_struct(ver,  &start_idx,  &next_idx);
    }

    /* Now resize all the HA blocks on the list */
    while (ha_blk_list) {
        bcmissu_ha_block_extend(unit, ha_blk_list);
        tmp_ha_blk = ha_blk_list;
        ha_blk_list = ha_blk_list->next;
        sal_free(tmp_ha_blk);
    }
exit:
    while (visited_struct) {
        struct_it = visited_struct;
        visited_struct = visited_struct->next;
        SHR_FREE(struct_it);
    }
    SHR_FUNC_EXIT();
}

int bcmissu_data_upgrade(int unit)
{
    uint32_t start_idx, next_idx;
    int rv;
    const char *ver = start_from_ver;
    int j;
    const issu_struct_db_t *from;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);

    /*
     * Go version by version and update the modified structure of that version.
     */
    rv = retrieve_versions_struct(ver,  &start_idx,  &next_idx);
    while (rv == SHR_E_NONE) {
        from = issu_struct_dict[start_idx].ver_db;
        for (j = 0; from[j].struct_info != NULL; j++) {
            SHR_IF_ERR_EXIT(bcmissu_struct_upgrade(unit, &from[j], start_idx));
        }
        ver = issu_struct_dict[next_idx].version;
        rv = retrieve_versions_struct(ver,  &start_idx,  &next_idx);
    }

    /*
     * Compress structure arrays that had reduced in size.
     */
    from = issu_struct_dict[start_idx].ver_db;
    for (j = 0; from[j].struct_info != NULL; j++) {
        SHR_IF_ERR_EXIT(bcmissu_struct_array_comp(unit, &from[j]));
    }

    /* Fix all the pointers values */
    SHR_IF_ERR_EXIT(bcmissu_update_ha_ptr(unit));

    if (unit != BCMISSU_NO_UNIT && !native_env) {
        /* Fix all the ltIDs */
        j = 0;
        while (issu_struct_dict[j].ver_db) {
            j++;
        }
        from = issu_struct_dict[j - 1].ver_db;
        SHR_IF_ERR_EXIT(bcmissu_update_ltids(unit, from,
                                             start_from_ver, current_ver));
        SHR_IF_ERR_EXIT(bcmissu_update_enums(unit, from, j - 1));
    }

    if (!native_env) {
        SHR_IF_ERR_EXIT(component_upgrade(unit,
                                          bcmissu_pre_upgrade_cb_get(),
                                          BCMISSU_PRE_UPGRADE));
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmissu_comp_upgrade(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (component_upgrade(unit,
                           bcmissu_post_upgrade_cb_get(),
                           BCMISSU_POST_UPGRADE));

    if (unit != BCMISSU_NO_UNIT) {
        SHR_IF_ERR_EXIT(bcmissu_apply_patches(unit, bcmissu_patch_list_get()));
    }

exit:
    SHR_FUNC_EXIT();
}
