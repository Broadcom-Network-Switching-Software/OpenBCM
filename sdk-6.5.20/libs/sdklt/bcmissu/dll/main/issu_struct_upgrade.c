/*! \file issu_struct_upgrade.c
 *
 * ISSU Structure data upgrade (for HA memory).
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
#include <bcmissu/issu_types.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_internal.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCMISSU_DLL

static void update_fields(uint32_t db_idx,
                          const issu_struct_db_t *from,
                          const issu_struct_db_t *to,
                          uint8_t *s_copy,
                          uint32_t instances,
                          uint8_t *blk);

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Find the structure definition within a newer DB version.
 *
 * This function searches the issu_struct_version for a structure definition.
 * The search starts at a given index of the issu_struct_version data
 * structure. If the structure was found in the given index it returns the
 * structure definition. Otherwise the index increments until the structure is
 * found or the DB had been exhausted.
 *
 * \param [in] id The structure ID.
 * \param [out] next_s The newer structure definition.
 * \param [in,out] idx The index of the DB to start the search from.
 *
 * \return SHR_E_NONE on success
 * \return SHR_E_NOT_FOUND when the structure ID is not part of any future DB.
 */
static int find_newer_struct_def(bcmissu_struct_id_t id,
                                 const issu_struct_db_t **next_s,
                                 uint32_t *idx)
{
    const issu_struct_db_t *ver_db;
    int rv = SHR_E_NOT_FOUND;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    while (issu_struct_dict[*idx].ver_db) {
        ver_db = issu_struct_dict[*idx].ver_db;
        while (ver_db && ver_db->struct_info) {
            if (ver_db->id == id) {
                *next_s = ver_db;
                rv = SHR_E_NONE;
                break;
            }
            ver_db++;
        }
        if (rv == SHR_E_NONE) {
            break;
        }
        (*idx)++;
    }
    return rv;
}

/*!
 *\brief Write value into a field within a data structure.
 *
 * This function writes a field value into specific field in a data structure.
 *
 * \param [in] val The value to write.
 * \param [in] dest The location to write the value to.
 * \param [in] fld_len The destination field size in bytes.
 *
 * \return None.
 */
static void write_fld_val(uint64_t val, uint8_t *dest, uint32_t fld_len)
{
    switch (fld_len) {
    case 1:
        *dest = val & 0xFF;
        break;
    case 2:
        *(uint16_t *)dest = val & 0xFFFF;
        break;
    case 4:
        *(uint32_t *)dest = val & 0xFFFFFFFF;
        break;
    case 8:
        *(uint64_t *)dest = val;
        break;
    default:
        assert(0);
    }
}

/*!
 *\brief Update the value of a field that changes its type.
 *
 * This function updates the value of a field that changed its type between
 * two versions of software.
 *
 * \param [in] src The field to get the value from.
 * \param [in] dest The field where the value should be written to.
 * \param [in] fld_cnt The number of fields to handle. In case of array this
 * number might be > 1.
 * \param [in] fld The definition of the source field.
 * \param [in] dest_fld The definition of the destination field.
 *
 * \return None.
 */
static void update_mixed_type(uint8_t *src,
                              uint8_t *dest,
                              uint64_t fld_cnt,
                              const issu_field_t *fld,
                              const issu_field_t *dest_fld)
{
    uint32_t j;
    uint64_t val;

    for (j = 0; j < fld_cnt; j++) {
        val = bcmissu_obtain_fld_val(src,  fld->width);
        src += fld->width;
        write_fld_val(val, dest, dest_fld->width);
        dest += dest_fld->width;
    }
}

/*!
 *\brief Update structure that is a field within another structure
 *
 * This function updates a structure that is a single field of another
 * structure. The function checks to see if the structure had changed between
 * the versions and update it accordingly if it did.
 * Otherwise, the data is simply copied onto the destination memory.
 *
 * \param [in] fld Is the field in the larger structure.
 * \param [in] from_db_idx The DB index associated with the base version of
 * which the upgrade being started from.
 * \param [in] src Is the memory location of the structure in the
 * original version.
 * \param [in] dest Is the memory location of the structure in the target
 * version.
 * \param [in] fld_cnt Is the number of instances of the structure (i.e. array
 * size or 1 for a single instance).
 *
 * \return None.
 */
static void update_nested_struct(const issu_field_t *fld,
                                 uint32_t from_db_idx,
                                 uint8_t *src,
                                 uint8_t *dest,
                                 uint64_t fld_cnt)
{
    const issu_struct_db_t *from;
    const issu_struct_db_t *to;
    int rv;
    uint32_t to_idx;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    /* Search if this structure included in the source version */
    from = issu_struct_dict[from_db_idx].ver_db;
    while (from && from->struct_info) {
        if (from->id == fld->struct_id) {
            break;
        }
        from++;
    }
    if (!from || !from->struct_info) {
        /* The data didn't change in the original version */
        sal_memcpy(dest, src, fld_cnt * fld->width);
        return;
    }

    /*
     * If we are here it means that the data structure was changed after the
     * base version. Now find the new definition of this structure.
     */
    to_idx = from_db_idx + 1;
    rv = find_newer_struct_def(from->id, &to, &to_idx);
    /*
     * The new version must be found, unless the data structure is no longer
     * in use
     */
    if (rv != SHR_E_NONE) {
        return;
    }
    update_fields(from_db_idx, from, to, src, fld_cnt & 0xFFFFFFFF, dest);
}

/*!
 *\brief Migrate the field values from one version of the structure to newer
 * one.
 *
 * This function migrates all the fields of a structure from one version into
 * a newer version of the same structure.
 *
 * \param [in] from_db_idx The base index of the structure DB. This index
 * indicates how the data is currently being stored in memory.
 * \param [in] from The current structure definition.
 * \param [in] to The destination structure definition.
 * \param [in] s_copy The copy of the source memory. This matches the \c from
 * data structure.
 * \param [in] instances Indicate the number of instances of the
 * same structure (i.e. if it is an array of structures the number of instances
 * might be > 1).
 * \param [out] blk The destination memory into which the structure within its new
 * format should be copied.
 *
 * \return None.
 */
static void update_fields(uint32_t from_db_idx,
                          const issu_struct_db_t *from,
                          const issu_struct_db_t *to,
                          uint8_t *s_copy,
                          uint32_t instances,
                          uint8_t *blk)
{
    uint32_t j, k, l;
    uint8_t *src, *dest;
    uint32_t offset;
    uint64_t fld_cnt;
    uint64_t dest_fld_cnt;
    const issu_field_t *fld;
    const issu_field_t *dest_fld;
    uint32_t src_variable_size;
    uint32_t dest_variable_size;

    for (l = 0; l < instances; l++) {
        src_variable_size = 0;
        dest_variable_size = 0;
        /* Now relocate every field to its destination */
        for (j = 0, fld = from->struct_info->fields;
             j < from->struct_info->field_count; j++, fld++) {
            /* Find the location of the source */
            offset =  from->struct_info->offset_get_func(fld->fid);
            src = s_copy + offset;

            /* Find the destination location */
            offset = to->struct_info->offset_get_func(fld->fid);
            if (offset == ISSU_INVALID_FLD_OFFSET) {
                continue;   /* Field doesn't exist in the new structure */
            }
            dest = blk + offset;

            /* Now find the field size */
            fld_cnt = bcmissu_get_fld_ins(fld, from, s_copy, 0);

            /* Determine the size of the field if variable size field */
            if (fld->var_size_id != ISSU_INVALID_STRUCT_ID) {
                src_variable_size += fld_cnt * fld->width;
                for (k = 0, dest_fld = to->struct_info->fields;
                     k < to->struct_info->field_count;
                     k++, dest_fld++) {
                    if (dest_fld->fid == fld->fid) {
                        dest_variable_size += fld_cnt * dest_fld->width;
                        break;
                    }
                }
            }

            if (fld->struct_id != ISSU_INVALID_STRUCT_ID &&
                (!fld->is_local_enum)) {
                update_nested_struct(fld, from_db_idx, src, dest, fld_cnt);
            } else {
                /*
                 * Find the destination field size.
                 * If larger need to take care of endianess.
                 */
                for (k = 0, dest_fld = to->struct_info->fields;
                     k < to->struct_info->field_count &&
                     dest_fld->fid != fld->fid;
                     k++, dest_fld++);
                if (k >= to->struct_info->field_count) {
                    continue; /* The field doesn't exist anymore */
                }

                dest_fld_cnt = bcmissu_get_fld_ins(dest_fld, to, NULL, fld_cnt);
                if (fld_cnt > dest_fld_cnt) {
                    fld_cnt = dest_fld_cnt;
                }
                if (dest_fld->width != fld->width) {
                    update_mixed_type(src, dest, fld_cnt, fld, dest_fld);
                } else {
                    /* Assuming that arrays will be packed */
                    sal_memcpy(dest, src, fld_cnt * fld->width);
                }
            }
        }
        /* Update the structure base point to the next instance */
        s_copy += from->struct_info->generic_size + src_variable_size;
        blk += to->struct_info->generic_size + dest_variable_size;
    }
}

/*!
 *\brief Update specific data structure from one version to the next.
 *
 * This function upgrade a specific data structure from a given definition into
 * a new definition. The function also updates the structure control entry with
 * the new entry size.
 *
 * \param [in] unit The unit to upgrade this structure.
 * \param [in] db_idx Indicates the index of the issu_struct_version where the
 * current structure is defined at.
 * \param [in] from The definition of the source data structure.
 * \param [in] to The destination data structure definition.
 * \param [in] s_info The structure control entry of this structure.
 *
 * \return None.
 */
static int update_struct(int unit,
                         uint32_t db_idx,
                         const issu_struct_db_t *from,
                         const issu_struct_db_t *to,
                         bcmissu_comp_s_info_t *s_info)
{
    uint8_t *s_copy;
    uint8_t *blk;
    uint32_t len = s_info->data_size * s_info->instances;
    uint32_t j;

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);

    /* Start by allocating memory to copy the old data into */
    s_copy = sal_alloc(s_info->data_size,  "bcmissuStruct");
    SHR_NULL_CHECK(s_copy,  SHR_E_MEMORY);

    if (unit == BCMISSU_NO_UNIT) {
        blk = shr_ha_gen_mem_alloc(s_info->comp_id, s_info->sub_id,
                                   NULL, &len);
    } else {
        blk = shr_ha_mem_alloc(unit,
                               s_info->comp_id,  s_info->sub_id,
                               NULL, &len);
    }
    SHR_NULL_CHECK(blk,  SHR_E_MEMORY);
    if (len < s_info->data_size * s_info->instances + s_info->offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    blk += s_info->offset;
    for (j = 0; j < s_info->instances; j++) {
        /* Copy the original content into regular memory */
        sal_memcpy(s_copy, blk, s_info->data_size);

        /*
         * Clean up the destination structure. Note that all the structures within
         * the block already moved to accommodate for the maximal structure size.
         */
        sal_memset(blk, 0, s_info->max_data_size);

        update_fields(db_idx, from, to, s_copy, 1, blk);
        /*
         * The update occurs after the block was resized and every structure
         * occupies its new size.
         */
        blk += s_info->max_data_size;
    }
    /*
     * The data size here is only taken for the benefit of the structure data
     * copy. The actual structure update is done based on the structure
     * definition within the DB.
     */
    s_info->data_size = s_info->max_data_size;

exit:
    if (s_copy) {
        sal_free(s_copy);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions local to ISSU
 */

uint64_t bcmissu_get_fld_ins(const issu_field_t *fld,
                             const issu_struct_db_t *s_db,
                             uint8_t *s_loc,
                             uint64_t orig_cnt)
{
    uint64_t fld_cnt;
    uint32_t fld_len;
    uint32_t offset;

    if (fld->is_array) {
        if (fld->var_size_id != ISSU_INVALID_FIELD_ID) {
            if (!s_loc) {
                fld_cnt = orig_cnt;
            } else {
                /* Find the location and original value of the size field */
                offset = bcmissu_fld_offset(s_db, fld->var_size_id, &fld_len);
                fld_cnt = bcmissu_obtain_fld_val(s_loc + offset, fld_len);
            }
        } else {
            fld_cnt = fld->size;    /* Fixed array */
        }
    } else {
        fld_cnt = 1;
    }
    return fld_cnt;
}

/*
 * a. Find the next version where the structure is defined. This function
 * will upgrade the structure from the 'from' DB to the next version it is
 * defined.
 * b. Use the ISSU structure report to find all the instances of this
 * structure. Update the structure for all its instances.
 * c. Note that it is possible that a data structure will not be found in
 * the HA structure report. That is in case that the structure is not an
 * independent structure but is part of another structure. The other
 * structure will be reported.
 * d. idx Is the index of the from version.
 */
int bcmissu_struct_upgrade(int unit,
                           const issu_struct_db_t *from,
                           uint32_t idx)
{
    uint32_t next_idx = idx + 1;
    const issu_struct_db_t *next_db;
    int rv;
    bcmissu_comp_s_info_t *s_info;
    void *blk;

    rv = find_newer_struct_def(from->id, &next_db, &next_idx);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    s_info = bcmissu_find_first_s_info(unit, from->id,  &blk);

    while (s_info) {
        update_struct(unit, idx, from, next_db, s_info);

        s_info = bcmissu_find_next_s_info(from->id, s_info, &blk);
    }

    return SHR_E_NONE;
}

int bcmissu_struct_array_comp(int unit, const issu_struct_db_t *from)
{
    void *hint;
    bcmissu_comp_s_info_t *s_info;
    uint8_t *ha_blk;
    uint32_t len;
    uint8_t *src, *dest;
    uint32_t j;

    s_info = bcmissu_find_first_s_info(unit, from->id,  &hint);

    while (s_info) {
        if (s_info->instances > 1 &&
            s_info->last_ver_data_size < s_info->data_size) {
            len = s_info->data_size * s_info->instances;
            if (unit == BCMISSU_NO_UNIT) {
                ha_blk = shr_ha_gen_mem_alloc(s_info->comp_id, s_info->sub_id,
                                              NULL, &len);
            } else {
                ha_blk = shr_ha_mem_alloc(unit,
                                           s_info->comp_id,  s_info->sub_id,
                                           NULL, &len);
            }
            if (!ha_blk) {
                return SHR_E_MEMORY;
            }
            src = ha_blk + s_info->offset + s_info->data_size;
            dest = ha_blk + s_info->offset + s_info->last_ver_data_size;
            for (j = 1; j < s_info->instances; j++) {
                sal_memmove(dest, src, s_info->last_ver_data_size);
                dest += s_info->last_ver_data_size;
                src += s_info->data_size;
            }
            s_info->data_size = s_info->last_ver_data_size;
        }

        s_info = bcmissu_find_next_s_info(from->id, s_info, &hint);
    }

    return SHR_E_NONE;
}
