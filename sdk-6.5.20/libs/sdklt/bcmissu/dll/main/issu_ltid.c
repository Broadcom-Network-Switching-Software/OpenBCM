/*! \file issu_ltid.c
 *
 * ISSU LTID upgrade
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmissu/issu_types.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_DLL

/*******************************************************************************
 * Private functions
 */

/*!
 *\brief Finds the current table ID using table name.
 *
 * This function finds the LTID based on the table name and unit.
 *
 * \param [in] unit The unit associated with this DB.
 * \param [in] name The logical table name.
 * \param [out] ltid The LT ID corresponding to the LT name or
 * BCMLTD_SID_INVALID if the table no longer exists.
 *
 * \return SHR_E_NONE always.
 */
static int find_ltid_ent_by_name(int unit, const char *name, uint32_t *ltid)
{
    int rv;
    bcmlrd_table_attrib_t lattrib;

    rv = bcmlrd_table_attributes_get(unit, name, &lattrib);
    if (rv != SHR_E_NONE) {
        *ltid = BCMLTD_SID_INVALID;
        LOG_INFO(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                          "LT %s not exist anymore\n"),
                   name));
        return SHR_E_UNAVAIL;
    } else {
        *ltid = lattrib.id;
        return SHR_E_NONE;
    }
}

/*!
 *\brief Finds the current field ID
 *
 * This function searches for the new field ID based on the LT and field names.
 * The search is done on the LRD DB.
 *
 * \param [in] unit The unit associated with this DB.
 * \param [in] tbl_ent The portion of the DB that matches the LT.
 * \param [in] fld_ent The portion of the DB that matches the field.
 * \param [out] fid The new field ID.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int find_flid_by_name(int unit,
                             const issu_ltid_dict_t *tbl_ent,
                             const issu_ltid_f_dict_t *fld_ent,
                             uint32_t *fid)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info = NULL;
    bcmlrd_table_attrib_t t_attrib;
    size_t j;
    int rv;
    size_t inp_len;

    rv = bcmlrd_table_attributes_get(unit, tbl_ent->lt_name, &t_attrib);
    if (SHR_FAILURE(rv)) {
        *fid = BCMLTD_SID_INVALID;
        return rv;
    }
    num_fid = t_attrib.number_of_fields;
    f_info = sal_alloc(sizeof(bcmlrd_client_field_info_t) * num_fid,
                       "bcmissuFlidByName");
    if (!f_info) {
        return SHR_E_MEMORY;
    }
    do {
        rv = bcmlrd_table_fields_def_get(unit, tbl_ent->lt_name,
                                         num_fid, f_info, &num_fid);
        if (SHR_FAILURE(rv)) {
            *fid = BCMLTD_SID_INVALID;
            break;
        }

        *fid = BCMLTD_SID_INVALID;
        inp_len = strlen(fld_ent->f_name);
        for (j = 0; j < num_fid; j++) {
            if ((inp_len == strlen(f_info[j].name)) &&
                sal_strcmp(fld_ent->f_name, f_info[j].name) == 0)  {
                *fid = f_info[j].id;
                break;
            }
        }
    } while (0);

    sal_free(f_info);
    return rv;
}

/*!
 *\brief Finds the LT entry in the dictionary that matches the LTID.
 *
 * This function searches the dictionary for an entry that has matches LTID
 * to the input ID.
 *
 * \param [in] id Searched value.
 * \param [in] gen_dict Generic dictionary to search.
 * \param [in] var_dict Variant dictionary to search.
 *
 * \return Pointer to the desired entry on success and NULL on failure.
 */
static const issu_ltid_dict_t *find_ltid_ent_by_id(
    uint32_t id,
    const issu_variant_info_t *gen_dict,
    const issu_variant_info_t *var_dict)
{
    const issu_variant_info_t *dict = var_dict;
    const issu_ltid_dict_t *it;
    int j;
    int low_idx, high_idx;
    int idx;

    /* Search with preference for the variance dictionary first */
    for (j = 0; j < 2; j++, dict = gen_dict) {
        if (!dict) {
            continue;
        }
        /* Each dictionary is sorted by ID so do binary search */
        low_idx = 0;
        high_idx = (int)(dict->length - 1);
        while (low_idx <= high_idx) {
            idx = (low_idx + high_idx) / 2;
            it = &dict->ltid_dict[idx];
            if (it->id == id) {
                return it;
            }
            if (it->id > id) {
                high_idx = idx - 1;
            } else {
                low_idx = idx + 1;
            }
        }
   }
    return NULL;
}

/*!
 *\brief Update specific LTID value.
 *
 * This function updates specific LTID to its new value.
 *
 * \param [in] unit The unit associated with the structure instances.
 * \param [in] ptr The address to read the old LTID and replace it with
 * the current value.
 * \param [in] fld Contains the field information.
 * \param [in] gen_dict LTID generic dictionary of the source version.
 * \param [in] var_dict LTID variant dictionary of the source version.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_FAIL The field width is too narrow.
 * \return SHR_E_BADID The LT name was not found in the current version.
 * \return SHR_R_NOT_FOUND The field ID was not found in the LTID dictionary.
 */
static int update_field_ltid_value(int unit,
                                   uint8_t *ptr,
                                   const issu_field_t *fld,
                                   const issu_variant_info_t *gen_dict,
                                   const issu_variant_info_t *var_dict)
{
    uint32_t current_val;
    const issu_ltid_dict_t *old_ent;
    uint32_t new_val;
    int rv;

    if (fld->width == sizeof(uint32_t)) {
        current_val = *(uint32_t *)ptr;
        if (current_val == BCMLTD_SID_INVALID) {
            return SHR_E_NOT_FOUND;
        }
    } else if (fld->width == sizeof(uint16_t)) {
        current_val = (*(uint16_t *)ptr) & 0xFFFF;
        if (current_val == (BCMLTD_SID_INVALID & 0xFFFF)) {
            return SHR_E_NOT_FOUND;
        }
    } else {
        return SHR_E_FAIL;
    }

    old_ent = find_ltid_ent_by_id(current_val, gen_dict, var_dict);
    if (!old_ent) {
        return SHR_E_FAIL;
    }
    rv = find_ltid_ent_by_name(unit, old_ent->lt_name, &new_val);
    if (rv != SHR_E_NONE && rv != SHR_E_UNAVAIL) {
        return SHR_E_NOT_FOUND;
    }

    if (fld->width == sizeof(uint32_t)) {
        *(uint32_t *)ptr = new_val;
    } else {
        *(uint16_t *)ptr = (uint16_t)new_val;
    }
    return SHR_E_NONE;
}

/*!
 *\brief Obtain the variant specific LT dictionary.
 *
 * This function uses the unit to obtain the device ID and the variant ID.
 * Using these two IDs, the function retrieves the LTID dictionary associated
 * with this variant.
 *
 * \param [in] unit The unit number.
 * \param [in] root_dict Global dictionary.
 *
 * \return LTID dictionary - success.
 * \return NULL - failure.
 */
static const issu_variant_info_t *find_variant_dict(
        int unit,
        const issu_ltid_dict_entry_t *root_dict)
{
    const char *dev_type_name = bcmdrd_dev_type_str(unit);
    const char *var_name = bcmlrd_variant_name(unit);
    const issu_device_info_t **dev_it = root_dict->devices;
    const issu_variant_info_t **variant_it;

    if (!dev_type_name || dev_type_name[0] == '\0' ||
        !var_name || var_name[0] == '\0') {
        return NULL;
    }
    /* Search by base device type */
    while (*dev_it && (*dev_it)->dev_name &&
           sal_strncasecmp((*dev_it)->dev_name, dev_type_name,
                           strlen(dev_type_name))) {
        dev_it++;
    }
    if (!(*dev_it) || !(*dev_it)->dev_name) {
        return NULL;
    }

    variant_it = (*dev_it)->variants;
    while (*variant_it && (*variant_it)->variant_id &&
            sal_strncasecmp((*variant_it)->variant_id, var_name,
                            strlen(var_name))) {
        variant_it++;
    }
    if (!(*variant_it) || !(*variant_it)->variant_id) {
        return NULL;
    }
    return (*variant_it);
}

/*!
 *\brief Update the structure field IDs.
 *
 * This function go through all the fields of a structure and updates the
 * field ID of those fields that contain field ID attribute (i.e. their value
 * is a field ID).
 *
 * \param [in] unit The unit number.
 * \param [in] st Structure information.
 * \param [in] ptr The address of the structure in (HA) memory.
 *
 * \return SHR_E_NONE - success.
 * \return SHR_E_UNAVAIL - Field ID was not found.
 * \return SHR_E_FAIL - The width of the field is not 32 or 16 bits.
 */
static int update_structure_fid(int unit,
                                const issu_struct_db_t *st,
                                uint8_t *ptr)
{
    const issu_field_t *fld;
    uint32_t j, k;
    uint32_t fid_offset;
    uint32_t ltid_offset;
    uint32_t ltid, flid;
    uint64_t fld_cnt;
    int rv;
    const issu_struct_t *st_info = st->struct_info;

    for (j = 0, fld = st_info->fields; j < st_info->field_count; j++, fld++) {
        if (fld->lt_attrib == HA_LT_FID) {  /* Field is FID */
            /*
             * Get the ltid offset. For array of field IDs there is only one
             * LTID.
             */
            ltid_offset = st_info->offset_get_func(fld->ltid_var_for_fid);
            ltid = *(uint32_t *)(ptr + ltid_offset);
            fid_offset = st_info->offset_get_func(fld->fid);
            fld_cnt = bcmissu_get_fld_ins(fld, st, ptr, 0);
            for (k = 0; k < fld_cnt; k++, fid_offset += fld->width) {
                if (fld->width == sizeof(uint32_t)) {
                    flid = *(uint32_t *)(ptr + fid_offset);
                } else if (fld->width == sizeof(uint16_t)) {
                    flid = (*(uint16_t *)(ptr + fid_offset)) & 0xFFFF;
                } else {
                    LOG_WARN(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                      "unexpected field width = %u\n"),
                               fld->width));
                    return SHR_E_FAIL;
                }
                rv = bcmissu_dll_fid_to_current(unit, ltid, flid, &flid);
                if (rv != SHR_E_NONE) {
                    LOG_WARN(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                      "Failed to convert fid = %u\n"),
                               flid));
                    flid = BCMLTD_SID_INVALID;
                }
                if (fld->width == sizeof(uint32_t)) {
                    *(uint32_t *)(ptr + fid_offset) = flid;
                } else {
                    *(uint16_t *)(ptr + fid_offset) = (uint16_t)flid;
                }
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Update all fields of a structure that are LTID.
 *
 * This function updates all the fields of a structure that has the LTID
 * attribute.
 *
 * \param [in] unit The unit associated with the structure instances.
 * \param [in] st_db Structure DB to update.
 * \param [in] ptr HA memory block where the structure starts. This
 * memory used to read the original value and update it.
 * \param [in] cur_ver Structure DB.
 * \param [in] gen_dict LTID generic dictionary of the source version.
 * \param [in] var_dict LTID variant dictionary of the source version.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_FAIL The LTID conversion failed.
 */
static int update_structure_ltid(int unit,
                                 const issu_struct_db_t *st,
                                 uint8_t *ptr,
                                 const issu_struct_db_t *cur_ver,
                                 const issu_variant_info_t *gen_dict,
                                 const issu_variant_info_t *var_dict)
{
    const issu_field_t *fld;
    uint32_t j, k;
    uint32_t offset;
    int rv;
    uint64_t fld_cnt;
    const issu_struct_t *st_info = st->struct_info;

    /*
     * Start by upgrading all the field ID values. The old value of the LT ID
     * is require to do the conversion.
     */
    rv = update_structure_fid(unit, st, ptr);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    for (j = 0, fld = st_info->fields; j < st_info->field_count; j++, fld++) {
        if (fld->lt_attrib == HA_LT_LTID) {   /* Field is LTID */
            offset = st_info->offset_get_func(fld->fid);
            fld_cnt = bcmissu_get_fld_ins(fld, st, ptr, 0);
            for (k = 0; k < fld_cnt; k++, offset += fld->width) {
                rv = update_field_ltid_value(unit, ptr + offset,
                                             fld, gen_dict, var_dict);
                if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
                    LOG_WARN(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                      "Failed to upgrade LTID = %u, struct id  0x%" PRIx64 "\n"),
                               *(uint32_t *)(ptr + offset), st->id));
                    return rv;
                }
            }
        }

        if (fld->struct_id != ISSU_INVALID_STRUCT_ID) {
            const issu_struct_db_t *it = cur_ver;  /* Iterator */
            offset = st_info->offset_get_func(fld->fid);
            while (it->struct_info) {
                if (it->id == fld->struct_id) {
                    fld_cnt = bcmissu_get_fld_ins(fld, st, ptr, 0);
                    for (k = 0; k < fld_cnt; k++, offset += fld->width) {
                        rv = update_structure_ltid(unit,
                                                   it,
                                                   ptr + offset,
                                                   cur_ver,
                                                   gen_dict,
                                                   var_dict);
                        if (rv != SHR_E_NONE) {
                            return rv;
                        }
                    }
                    break;
                }
                it++;
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Update LTIDs for all the instances of a structure.
 *
 * This function searches for all the instances of a given structure ID.
 * For each instance it updates the value of the field in this structure that
 * are LT IDs.
 *
 * \param [in] unit The unit associated with the structure instances.
 * \param [in] st_db Structure DB to update.
 * \param [in] cur_ver_db IStructure DB.
 * \param [in] gen_dict LTID generic dictionary of the source version.
 * \param [in] var_dict LTID variant dictionary of the source version.
 *
 * \return SHR_E_NONE Success, error code otherwise.
 */
static int update_structure_inst_ltid(int unit,
                                      const issu_struct_db_t *st_db,
                                      const issu_struct_db_t *cur_ver_db,
                                      const issu_variant_info_t *gen_dict,
                                      const issu_variant_info_t *var_dict)
{
    bcmissu_comp_s_info_t *s_info;
    void *blk;
    uint8_t *ha_blk;
    uint32_t len;
    uint32_t j;

    s_info = bcmissu_find_first_s_info(unit, st_db->id,  &blk);
    while (s_info) {
        len = s_info->data_size;    /* Minimal block length */
        /* For LT ID we must have valid unit. */
        ha_blk = shr_ha_mem_alloc(unit,
                                  s_info->comp_id, s_info->sub_id,
                                  NULL, &len);
        if (!ha_blk || len < s_info->data_size) {
            return SHR_E_MEMORY;
        }

        ha_blk += s_info->offset;
        len = s_info->data_size;
        for (j = 0; j < s_info->instances; j++) {
            update_structure_ltid(unit, st_db,
                                  ha_blk, cur_ver_db,
                                  gen_dict, var_dict);
            ha_blk += len;
        }

        s_info = bcmissu_find_next_s_info(st_db->id, s_info, &blk);
    }

    return SHR_E_NONE;
}

/*!
 *\brief Check if a structure contains an LT ID field.
 *
 * This function traverse the field list is seach for a fields with the
 * field ID attributre. Note that the function can search into sub structure
 * as well.
 *
 * \param [in] st Structure to search in.
 * \param [in] cur_ver Structure DB.
 *
 * \return  true is a fields in the structure has LT IDs attribute.
 */
static bool struct_contains_ltid(const issu_struct_t *st,
                                 const issu_struct_db_t *cur_ver)
{
    const issu_field_t *fld;
    uint32_t j;

    for (j = 0, fld = st->fields; j < st->field_count; j++, fld++) {
        if (fld->lt_attrib == HA_LT_LTID) {
            return true;
        }
        if (fld->struct_id != ISSU_INVALID_STRUCT_ID) {
            const issu_struct_db_t *it = cur_ver;  /* Iterator */
            while (it->struct_info) {
                if (it->id == fld->struct_id) {
                    if (struct_contains_ltid(it->struct_info, cur_ver)) {
                        return true;
                    }
                    break;
                }
                it++;
            }
        }
    }
    return false;
}

/*!
 *\brief Check if an LT had changed.
 *
 * This function checks if any field changed its ID from before the upgrade.
 *
 * \param [in] unit The unit associated with this DB.
 * \param [in] it Pointing to the LT DB prior to the change.
 *
 * \return true us a field ID was changed between the versions, false otherwise.
 */
static bool lt_fid_changed(int unit, const issu_ltid_dict_t *lt_info)
{
    bcmlrd_table_attrib_t t_attrib;
    size_t actual_fields;
    bcmlrd_client_field_info_t *field_array = NULL;
    const issu_ltid_f_dict_t *it;
    int rv;
    size_t j;
    bool rc = false;
    size_t inp_len;

    do {
        if (bcmlrd_table_attributes_get(unit,
                                        lt_info->lt_name,
                                        &t_attrib) != SHR_E_NONE) {
            rc = false;
            break;
        }
        field_array = sal_alloc(sizeof(bcmlrd_client_field_info_t) *
                                t_attrib.number_of_fields,
                                "bcmimmIssu");
        if (!field_array) {
            rc = false;
            break;
        }

        rv = bcmlrd_table_fields_def_get(unit,
                                         lt_info->lt_name,
                                         t_attrib.number_of_fields,
                                         field_array,
                                         &actual_fields);

        if (rv != SHR_E_NONE || t_attrib.number_of_fields != actual_fields) {
            rc = false;
            break;
        }

        it = lt_info->lt_fields;
        actual_fields = 0;
        /* Count the number of fields in the ISSU DB */
        for (; it->f_name; it++) {
            actual_fields++;
        }
        /* Number of fields had changed? */
        if (actual_fields != t_attrib.number_of_fields) {
            rc = true;
            break;
        }

        rc = false;
        for (j = 0; j < actual_fields; j++) {
            inp_len = strlen(field_array[j].name);
            /* Find the field in the source DB */
            for (it = lt_info->lt_fields; it->f_name; it++) {
                if (it->id == field_array[j].id) {
                    if ((inp_len != strlen(it->f_name)) ||
                        sal_strcmp(it->f_name, field_array[j].name)) {
                        /* Field ID match by field name is different */
                        rc = true;
                        break;
                    }
                    break; /* Field was found and matched */
                }
            }
            if (!it->f_name || rc) {
                break;
            }
        }
    } while (0);

    SHR_FREE(field_array);
    return rc;
}
/*******************************************************************************
 * Public functions local to ISSU
 */
int bcmissu_update_ltids(int unit,
                         const issu_struct_db_t *cur_ver_db,
                         const char *start_ver,
                         const char *current_ver)
{
    const issu_struct_db_t *it = cur_ver_db;  /* Iterator */
    const issu_variant_info_t *dict;
    const issu_variant_info_t *gen_dict;
    const issu_ltid_dict_entry_t *root_dict = bcmissu_ltid_dict_get();

    SHR_FUNC_ENTER(unit);

    /* Get the LT ID DB associated with this unit */
    dict = find_variant_dict(unit, root_dict);
    gen_dict = root_dict->gen_variant;

    while (it->struct_info) {
        if (struct_contains_ltid(it->struct_info, cur_ver_db)) {
            SHR_IF_ERR_EXIT(update_structure_inst_ltid(unit,
                                                       it,
                                                       cur_ver_db,
                                                       gen_dict,
                                                       dict));
        }
        it++;
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmissu_dll_ltid_to_current(int unit,
                                uint32_t old_ltid,
                                uint32_t *current_ltid)
{
    const issu_variant_info_t *dict;
    const issu_ltid_dict_t *old_ent;
    const issu_variant_info_t *gen_dict;
    const issu_ltid_dict_entry_t *root_dict = bcmissu_ltid_dict_get();

    SHR_FUNC_ENTER(unit);

    if (unit < 0 || unit >= BCMDRD_CONFIG_MAX_UNITS || !current_ltid) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    dict = find_variant_dict(unit, root_dict);

    gen_dict = root_dict->gen_variant;
    old_ent = find_ltid_ent_by_id(old_ltid, gen_dict, dict);
    if (!old_ent) {
        return SHR_E_NOT_FOUND;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (find_ltid_ent_by_name(unit, old_ent->lt_name, current_ltid));

exit:
    SHR_FUNC_EXIT();
}

int bcmissu_dll_fid_to_current(int unit,
                               uint32_t old_ltid,
                               uint32_t old_flid,
                               uint32_t *current_flid)
{
    const issu_variant_info_t *dict;
    const issu_variant_info_t *gen_dict;
    const issu_ltid_dict_t *old_ent;
    const issu_ltid_f_dict_t *it;
    const issu_ltid_dict_entry_t *root_dict = bcmissu_ltid_dict_get();

    SHR_FUNC_ENTER(unit);

    if (unit < 0 || unit >= BCMDRD_CONFIG_MAX_UNITS || !current_flid) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    dict = find_variant_dict(unit, root_dict);
    gen_dict = root_dict->gen_variant;

    /*
     * Search in the device dictionary for the table entry using the old
     * table ID. The search is done on the source dictionary (the dictionary
     * is only available for the source version).
     */
    old_ent = find_ltid_ent_by_id(old_ltid, gen_dict, dict);
    if (!old_ent) {
        return SHR_E_NOT_FOUND;
    }

    /*
     * The field dictionary is organized in the same order as the LT
     * dictionary. That means that we can use the same index where the
     * LT was found to refer to the field DB.
     */
    it = old_ent->lt_fields;
    /* Search for a matching field ID */
    for (; it->f_name; it++) {
        if (it->id == old_flid) {
            break;
        }
    }
    if (!it || !it->f_name) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT(find_flid_by_name(unit, old_ent, it, current_flid));

exit:
    SHR_FUNC_EXIT();
}

bool bcmissu_dll_lt_has_changed(int unit, uint32_t ltid)
{
    const issu_variant_info_t *dict;
    const issu_variant_info_t *gen_dict;
    const issu_ltid_dict_t *old_ent;
    const issu_ltid_dict_entry_t *root_dict = bcmissu_ltid_dict_get();

    if (unit < 0 || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return false;
    }

    dict = find_variant_dict(unit, root_dict);
    gen_dict = root_dict->gen_variant;

    old_ent = find_ltid_ent_by_id(ltid, gen_dict, dict);
    if (!old_ent) {
        return false;
    }
    return lt_fid_changed(unit, old_ent);
}

