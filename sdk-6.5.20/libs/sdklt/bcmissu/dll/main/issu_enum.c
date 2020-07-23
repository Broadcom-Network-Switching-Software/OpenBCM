/*! \file issu_enum.c
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
#include <bcmlrd/bcmlrd_enum.h>
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
 *\brief Obtain the variant/device enum dictionaries.
 *
 * This function uses the unit to obtain the enum dictionaries associated with
 * the unit. The dictionaries are the generic and variant based dictionary.
 *
 * \param [in] unit The unit number.
 * \param [out] gen_dict Generic dictionary.
 * \param [out] variant_dict Variant dictionary.
 *
 * \return LTID dictionary - success.
 * \return NULL - failure.
 */
static int find_device_dicts(int unit,
                             const issu_enum_def_t **gen_dict,
                             const issu_enum_def_t **variant_dict)
{
    const char *dev_type_name = bcmdrd_dev_type_str(unit);
    const char *var_name = bcmlrd_variant_name(unit);
    const issu_variants_enum_map_t *root;
    const issu_variants_enums_t *root_enums;
    int rv = SHR_E_NOT_FOUND;

    if (!dev_type_name || dev_type_name[0] == '\0') {
        return SHR_E_NOT_FOUND;
    }

    root = bcmissu_variants_enum_dict_get(dev_type_name);
    if (!root) {
        return SHR_E_INTERNAL;
    }
    if (variant_dict) {
        *variant_dict = NULL;
        if (var_name && var_name[0] != '\0') {
            root_enums = root->variants_enum;
            while (root_enums->device) {
                if ((sal_strncasecmp(root_enums->device, dev_type_name,
                                     sal_strlen(dev_type_name)) == 0) &&
                    (sal_strncasecmp(root_enums->variant, var_name,
                                     sal_strlen(var_name)) == 0)){
                    *variant_dict = root_enums->enums;
                    rv = SHR_E_NONE;
                    break;
                }
                root_enums++;
            }
        }
    }

    /* Now find the generic enum section */
    if (gen_dict) {
        *gen_dict = NULL;
        /* Search by base device name */
        root_enums = root->gen_enums;
        while (root_enums->device) {
            if (!sal_strncasecmp(root_enums->device, dev_type_name,
                                 sal_strlen(dev_type_name))) {
                *gen_dict = root_enums->enums;
                rv = SHR_E_NONE;
                break;
            }
            root_enums++;
        }
    }
    return rv;
}

/*!
 * \brief Find the enum structure in a given dictionary.
 *
 * This function searches the given dictionary for an entry that matches the
 * given enum type name.
 *
 * \param[in] dict Dictionary of enum type names.
 * \param [in] enum_type_name Enum type name to search in the dictionary.
 *
 * \return Pointer to the enum record - Success.
 * \return NULL - Failure.
 */
static const issu_enum_def_t *find_enum_in_dict(
        const issu_enum_def_t *dict,
        const char *enum_type_name)
{
    const issu_enum_def_t *it = dict;
    size_t inp_len = strlen(enum_type_name);

    while (it->enum_type_name != NULL) {
        if ((inp_len == strlen(it->enum_type_name)) &&
            !sal_strcmp(it->enum_type_name, enum_type_name)) {
            return it;
        }
        it++;
    }
    return NULL;
}

/*!
 * \brief Search for enum record.
 *
 * This function search for an enum record with the specified type name.
 *
 * \param [in] unit The unit associated with the enum.
 * \param [in] enum_type_name Enumerator type name to search for.
 *
 * \return Pointer to the enum record - Success.
 * \return NULL - Failure.
 */
static const issu_enum_def_t *find_enum_record(int unit,
                                               const char *enum_type_name)
{
    const issu_enum_def_t *gen_dict = NULL;
    const issu_enum_def_t *var_dict = NULL;
    const issu_enum_def_t *dict = NULL;

    if (find_device_dicts(unit, &gen_dict, &var_dict) != SHR_E_NONE) {
        return NULL;
    }

    /* Search at the variant first. If not found then resolve at the generic */
    if (var_dict) {
        dict = find_enum_in_dict(var_dict, enum_type_name);
    }
    /* If not found at the variant search the generic dictionary */
    if (!dict && gen_dict) {
        dict = find_enum_in_dict(gen_dict, enum_type_name);
    }
    return dict;
}

/*!
 * \brief Determine if a local enum has changed.
 *
 * This function searches the enum definition in every DB version that is prior
 * to the current version. If found, then the enum definition has changed.
 *
 * \param [in] enum_id The unique enum ID.
 * \param [in] current_ver_idx The index of the current version within the
 * ISSU DB.
 *
 * \return true if the enum definition was found in version prior to the
 * current version.
 */
static bool local_enum_changed(bcmissu_struct_id_t enum_id,
                               uint32_t current_ver_idx)
{
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();
    const issu_enum_db_t *from;
    int k;

    /* Find the first version containing the enum definition */
    for (k = 0; k < current_ver_idx; k++) {
        from = issu_struct_dict[k].enum_db;
        /* Search the enum definition in the from */
        while (from && from->enum_info) {
            if (from->id == enum_id) {
                return true;
            }
            from++;
        }
    }

    return false;
}
/*!
 * \brief Find if a structure contains an enum field that was changed.
 *
 * This function search a given data structure to find if the structure has
 * a field of enum type and if that enum had changed since the start version.
 *
 * \param [in] unit The unit associated with the enum.
 * \param [in] st The structure definition.
 * \param [in] cur_ver The current version data structure database. This
 * parameter is being used when a particular field of the structure is of type
 * structure.
 * \param [in] current_ver_idx The index of the current version within the
 * ISSU DB.
 *
 * \return true The structure contains an enum type field that had changed.
 * \return false Otherwise.
 */
static bool struct_contains_changed_enum(int unit,
                                         const issu_struct_t *st,
                                         const issu_struct_db_t *cur_ver,
                                         uint32_t current_ver_idx)
{
    const issu_field_t *fld;
    uint32_t j;

    for (j = 0, fld = st->fields; j < st->field_count; j++, fld++) {
        if (fld->enum_type_name != NULL &&
            bcmissu_dll_enum_has_changed(unit, fld->enum_type_name)) {
            return true;
        }
        if (fld->is_local_enum &&
            local_enum_changed(fld->struct_id, current_ver_idx)) {
            return true;
        }
        if (fld->struct_id != ISSU_INVALID_STRUCT_ID && (!fld->is_local_enum)) {
            const issu_struct_db_t *it = cur_ver;  /* Iterator */
            while (it->struct_info) {
                if (it->id == fld->struct_id) {
                    if (struct_contains_changed_enum(unit,
                                                     it->struct_info,
                                                     cur_ver,
                                                     current_ver_idx)) {
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
 * \brief Update the value of an enum field with a current value.
 *
 * This function obtains the current enum value pointed by \c ptr and
 * replaces it with the equivalent enum value that matches the current
 * version.
 *
 * \param [in] unit Associated with the data structure.
 * \param [in] ptr The memory location of the enum value.
 * \param [in] fld Definition of the enum field within the structure.
 * \param [in] gen_dict Generic (non variant dependent) enum dictionary.
 * \param [in] var_dict Variant dependent enum dictionary.
 *
 * \return SHR_E_NONE - Success.
 * \return SHR_E_INTERNAL - The enum record was not found within the start
 * version.
 * \return SHR_E_FAIL - The enum field width is unsupported.
 * \return NOT_FOUND - The enum value was not found within the source enum DB.
 */
static int update_field_enum_value(int unit,
                                   uint8_t *ptr,
                                   const issu_field_t *fld,
                                   const issu_enum_def_t *gen_dict,
                                   const issu_enum_def_t *var_dict)
{
    uint32_t current_val;
    uint64_t new_val;
    const issu_enum_def_t *enum_dict = NULL;
    const issu_enum_symbol_t *enum_desc;
    int rv;

    /* Prefer the variant value over the generic */
    if (var_dict) {
        enum_dict = find_enum_in_dict(var_dict, fld->enum_type_name);
    }
    if (!enum_dict && gen_dict) {
        enum_dict = find_enum_in_dict(gen_dict, fld->enum_type_name);
    }
    if (!enum_dict) {
        return SHR_E_NONE; /* Enum had not changed */
    }
    if (fld->width == sizeof(uint32_t)) {
        current_val = *(uint32_t *)ptr;
    } else if (fld->width == sizeof(uint16_t)) {
        current_val = (*(uint16_t *)ptr) & 0xFFFF;
    } else if (fld->width == sizeof(uint8_t)) {
        current_val = (*(uint8_t *)ptr) & 0xFF;
    } else {
        return SHR_E_FAIL;
    }

    enum_desc = enum_dict->enum_def;
    while (enum_desc->enum_fname) {
        if (enum_desc->val == current_val) {
            break;
        }
        enum_desc++;
    }
    if (!enum_desc->enum_fname) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                          "ENUM value %u was not found in enum type %s\n"),
                   current_val, fld->enum_type_name));
        return SHR_E_NONE;  /* Leave the enum value as is. It was illegal */
    }

    rv = bcmlrd_enum_symbol_to_scalar_by_type(unit, fld->enum_type_name,
                                              enum_desc->enum_fname, &new_val);
    /*
     * If enum value is no longer exist set the value to -1. This will
     * indicates to the component that the value is not available anymore.
     */
    if (rv != SHR_E_NONE) {
        rv = SHR_E_NOT_FOUND;
        new_val = BCMISSU_INVALID_ENUM;
    }

    if (fld->width == sizeof(uint32_t)) {
        *(uint32_t *)ptr = new_val;
    } else if (fld->width == sizeof(uint16_t)) {
        *(uint16_t *)ptr = (uint16_t)new_val;
    } else {
        *(uint8_t *)ptr = (uint8_t)new_val;
    }
    return rv;
}

/*!
 *\brief Migrate the local enum field values from one version of the enum to
 *\brief newer one.
 *
 * This function migrates all the instances of the local enum type from one
 * version into a newer version of the same enum type. This is only applicable
 * for enum type that is defined in the component ISSU YAML file (local enum).
 * It is not applicable for LT enum types.
 *
 * \param [in] from The current structure definition.
 * \param [in] to The destination structure definition.
 * \param [in,out] src The field value(s) memory.
 * \param [in] instances Indicates the number of instances of the
 * same structure (i.e. if it is an array of structures the number of instances
 * might be > 1).
 * \param [in] width The width of the enum field in bytes.
 *
 * \return SHR_E_NONE Success, error code otherwise.
 */
static int update_local_enum_values(const issu_enum_db_t *from,
                                    const issu_enum_db_t *to,
                                    uint8_t *src,
                                    uint64_t instances,
                                    uint32_t width)
{
    uint32_t l;
    uint32_t val;
    const issu_enum_symbol_t *src_enum_info;
    const issu_enum_symbol_t *to_enum_info;
    size_t inp_len;

    if ((width > sizeof(uint32_t)) || (width < sizeof(uint16_t))) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META("Unsupported enum size\n")));

        return SHR_E_INTERNAL;
    }
    for (l = 0; l < instances; l++) {
        if (width == sizeof(uint32_t)) {
            val = *(uint32_t *)src;
        } else {
            val = (uint32_t)(*(uint16_t *)src);
        }
        /*
         * Find the new val by finding the enum string associated with the
         * old val.
         */
        for (src_enum_info = from->enum_info;
             src_enum_info && src_enum_info->enum_fname;
             src_enum_info++) {
            if (src_enum_info->val == val) {
                break;
            }
        }
        if ((!src_enum_info) || (!src_enum_info->enum_fname)) {
            /* Source enum value was not found, used illegal value */
            LOG_WARN(BSL_LOG_MODULE,
                      (BSL_META("ENUM value %u was not found in enum type %s\n"),
                       val, from->enum_name));
            val = BCMISSU_INVALID_ENUM;
        } else {
            /* Search for the new val using the enum string */
            inp_len = strlen(src_enum_info->enum_fname);

            for (to_enum_info = to->enum_info;
                 to_enum_info && to_enum_info->enum_fname;
                 to_enum_info++) {
                if ((inp_len == strlen(to_enum_info->enum_fname)) &&
                    sal_strcmp(to_enum_info->enum_fname,
                               src_enum_info->enum_fname) == 0) {
                    break;
                }
            }

            /*
             * If the string was not found in the new version it has been
             * deleted.
             */
            if ((!to_enum_info) || (!to_enum_info->enum_fname)) {
                val = BCMISSU_INVALID_ENUM;
            } else {
                val = to_enum_info->val;
            }
        }

        /* Set the new value */
        if (width == sizeof(uint32_t)) {
            *(uint32_t *)src = val;
        } else {
            *(uint16_t *)src = (uint16_t)val;
        }

        /* Update the structure base point to the next instance */
        src += width;
    }
    return SHR_E_NONE;
}

/*!
 *\brief Update enum field within.
 *
 * This function updates an enum value that is a single field of another
 * structure. The function checks to see if the enum definition has changed
 * between the versions and updates it accordingly if it has.
 * Otherwise, the data is simply copied onto the destination memory.
 *
 * \param [in] fld Is the field in the larger structure.
 * \param [in] src Is the memory location of the enum in the
 * original version.
 * \param [in] fld_cnt Is the number of instances of the structure (i.e. array
 * size or 1 for a single instance).
 * \param [in] current_ver_idx The index of the current version. That is where
 * the destination enum definition ('to') can be found.
 *
 * \return SHR_E_NONE Success, error code otherwise.
 */
static int update_local_enum_field(const issu_field_t *fld,
                                   uint8_t *src,
                                   uint64_t fld_cnt,
                                   uint32_t current_ver_idx)
{
    const issu_enum_db_t *from = NULL;
    const issu_enum_db_t *to;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();
    int k;

    /* Find the first version containing the enum definition */
    for (k = 0; k < current_ver_idx; k++) {
        from = issu_struct_dict[k].enum_db;
        /* Search the enum definition in the from */
        while (from && from->enum_info) {
            if (from->id == fld->struct_id) {
                break;
            }
            from++;
        }
        if (from && from->enum_info) {
            break;
        }
    }

    if (!from || !from->enum_info) {
        return SHR_E_NONE;  /* This enum has not changed */
    }
    /* Now find the enum definition in the current version */
    to = issu_struct_dict[current_ver_idx].enum_db;
    while (to && to->enum_info) {
        if (to->id == fld->struct_id) {
            break;
        }
        to++;
    }
    if (!to || (!to->enum_info)) {
        return SHR_E_INTERNAL;  /* This should not happen */
    }
    return update_local_enum_values(from, to, src, fld_cnt, fld->width);
}

/*!
 * \brief Update enum values of a structure.
 *
 * This function scans all the fields of a structure in search for enum type
 * field. If found it updates the value of the field. Note that the actual
 * enum value can be different within different instances of the same
 * structure.
 * If a field is of type structure the function will also examine that
 * structure.
 *
 * \param [in] unit Associated with the data structure.
 * \param [in] st The data structure definition.
 * \param [in] ptr The address of the data structure.
 * \param [in] cur_ver The structure database of the current version.
 * \param [in] gen_dict Generic (non variant dependent) enum dictionary.
 * \param [in] var_dict Variant dependent enum dictionary.
 *
 * \return SHR_E_NONE - Success, error code otherwise.
 */
static int update_structure_enum(int unit,
                                 const issu_struct_db_t *st,
                                 uint8_t *ptr,
                                 const issu_struct_db_t *cur_ver,
                                 const issu_enum_def_t *gen_dict,
                                 const issu_enum_def_t *var_dict,
                                 uint32_t current_ver_idx)
{
    const issu_field_t *fld;
    uint32_t j, k;
    uint32_t offset;
    int rv;
    uint64_t fld_cnt;
    const issu_struct_t *st_info = st->struct_info;

    /* Go through all the fields and correct any enum that needs correction. */
    for (j = 0, fld = st_info->fields; j < st_info->field_count; j++, fld++) {
        if (fld->enum_type_name != NULL) {   /* Field is ENUM */
            offset = st_info->offset_get_func(fld->fid);
            fld_cnt = bcmissu_get_fld_ins(fld, st, ptr, 0);
            for (k = 0; k < fld_cnt; k++, offset += fld->width) {
                rv = update_field_enum_value(unit, ptr + offset,
                                             fld, gen_dict, var_dict);
                if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
                    return rv;
                }
            }
        }

        if (fld->struct_id != ISSU_INVALID_STRUCT_ID && (!fld->is_local_enum)) {
            const issu_struct_db_t *it = cur_ver;  /* Iterator */
            offset = st_info->offset_get_func(fld->fid);
            while (it->struct_info) {
                if (it->id == fld->struct_id) {
                    fld_cnt = bcmissu_get_fld_ins(fld, st, ptr, 0);
                    for (k = 0; k < fld_cnt; k++, offset += fld->width) {
                        rv = update_structure_enum(unit,
                                                   it,
                                                   ptr + offset,
                                                   cur_ver,
                                                   gen_dict,
                                                   var_dict,
                                                   current_ver_idx);
                        if (rv != SHR_E_NONE) {
                            return rv;
                        }
                    }
                    break;
                }
                it++;
            }
        }
        if (fld->is_local_enum) {
            offset = st_info->offset_get_func(fld->fid);
            fld_cnt = bcmissu_get_fld_ins(fld, st, ptr, 0);
            rv = update_local_enum_field(fld, ptr + offset,
                                         fld_cnt, current_ver_idx);
            if (rv != SHR_E_NONE) {
                return rv;
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Update all the instances of a structure that contains enum field.
 *
 * This function go through all the instances of a particular data structure
 * that contains one or mode fields of enum type. The function updates the
 * value of the field if the enum definition had changed.
 *
 * \param [in] unit Associated with the data structure.
 * \param [in] st_db The data structure definition.
 * \param [in] cur_ver_db The structure database of the current version.
 * \param [in] gen_dict Generic (non variant dependent) enum dictionary.
 * \param [in] var_dict Variant dependent enum dictionary.
 *
 * \return SHR_E_NONE - Success.
 * \return SHR_E_MEMORY - Failed to allocate the HA memory block associated
 * with a particular instance of the data structure.
 */
static int update_structure_inst_enum(int unit,
                                      const issu_struct_db_t *st_db,
                                      const issu_struct_db_t *cur_ver_db,
                                      const issu_enum_def_t *gen_dict,
                                      const issu_enum_def_t *var_dict,
                                      uint32_t current_ver_idx)
{
    bcmissu_comp_s_info_t *s_info;
    void *blk_hint;
    uint8_t *ha_blk;
    uint32_t len;
    int rv;
    uint32_t j;

    s_info = bcmissu_find_first_s_info(unit, st_db->id,  &blk_hint);
    while (s_info) {
        len = s_info->data_size;    /* Minimal block length */
        /* For LT ID we must have valid unit. */
        ha_blk = shr_ha_mem_alloc(unit, s_info->comp_id,
                                  s_info->sub_id,
                                  NULL, &len);
        if (!ha_blk || len < s_info->data_size) {
            return SHR_E_MEMORY;
        }

        ha_blk += s_info->offset;
        len = s_info->data_size;
        for (j = 0; j < s_info->instances; j++) {
            rv = update_structure_enum(unit, st_db,
                                       ha_blk, cur_ver_db,
                                       gen_dict, var_dict, current_ver_idx);
            if (rv != SHR_E_NONE) {
                LOG_WARN(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                  "Failed to upgrade :ENUM for struct id 0x%" PRIx64 "\n"),
                           st_db->id));
            }
            ha_blk += len;
        }

        s_info = bcmissu_find_next_s_info(st_db->id, s_info, &blk_hint);
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions local to ISSU
 */
int bcmissu_update_enums(int unit,
                         const issu_struct_db_t *cur_ver_db,
                         uint32_t current_ver_idx)
{
    const issu_struct_db_t *it = cur_ver_db;  /* Iterator */
    const issu_enum_def_t *variant_dict;
    const issu_enum_def_t *gen_dict;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = find_device_dicts(unit, &gen_dict, &variant_dict);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "Cannot find ISSU device dictionary\n")));
        return rv;
    }
    while (it->struct_info) {
        if (struct_contains_changed_enum(unit, it->struct_info,
                                         cur_ver_db, current_ver_idx)) {
            SHR_IF_ERR_EXIT(update_structure_inst_enum(unit,
                                                       it,
                                                       cur_ver_db,
                                                       gen_dict,
                                                       variant_dict,
                                                       current_ver_idx));
        }
        it++;
    }
exit:
    SHR_FUNC_EXIT();
}

/* This function is non-reentrant */
bool bcmissu_dll_enum_has_changed(int unit, const char *enum_type_name)
{
    if (!enum_type_name) {
        return false;
    }
    if (find_enum_record(unit, enum_type_name)) {
        return true;
    } else {
        return false;
    }
}

/* This function is non-reentrant */
int bcmissu_dll_enum_val_to_current(int unit,
                                    const char *enum_type_name,
                                    uint32_t old_val,
                                    uint32_t *current_val)
{
    const issu_enum_def_t *enum_entry;
    const issu_enum_symbol_t *enum_desc;
    int rv;
    uint64_t val;

    if (!current_val || !enum_type_name) {
        return SHR_E_PARAM;
    }
    enum_entry = find_enum_record(unit, enum_type_name);
    if (!enum_entry) {
        *current_val = old_val;
        return SHR_E_NONE;
    }
    enum_desc = enum_entry->enum_def;
    while (enum_desc->enum_fname) {
        if (enum_desc->val == old_val) {
            break;
        }
        enum_desc++;
    }
    if (!enum_desc->enum_fname) {
        /* Enum name not found in old version. */
        return SHR_E_PARAM;
    }

    /* Find in the LRD the current enum value */
    rv = bcmlrd_enum_symbol_to_scalar_by_type(unit, enum_type_name,
                                              enum_desc->enum_fname, &val);
    if (rv != SHR_E_NONE) {
        return SHR_E_NOT_FOUND;
    }
    *current_val = (uint32_t)val;
    return SHR_E_NONE;
}
