/*! \file issu_ha_ptr.c
 *
 * ISSU Structure size upgrade (for HA memory).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmissu/issu_types.h>
#include <bcmissu/issu_internal.h>

/*******************************************************************************
 * Local definitions
 */
/* macros/defines */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_DLL

/* typedefs */
/*!
 * \brief HA pointer field record
 */
typedef struct ha_ptr_tracking_s {
    bcmissu_field_id_t fid;     /* The field ID of the HA pointer */
    bcmissu_comp_s_info_t *ptr_struct_ctrl; /* Pointer structure control */
    bcmissu_comp_s_info_t *dest_struct_ctrl; /* Destination structure control */
    /* The destination field ID pointed by the pointer */
    bcmissu_field_id_t dest_s_fid;
    bool regular_ptr;
    struct ha_ptr_tracking_s *next;
} ha_ptr_tracking_t;

/* Static variables */
static int ha_ptr_track_size;
static ha_ptr_tracking_t **ha_ptr_track;
static ha_ptr_tracking_t *ha_ptr_track_gen;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Create a record for an instance of HA pointer
 *
 * This function allocates a record
 *
 * \param [in] unit The unit associated with the HA pointer.
 * \param [in] s_ctrl The structure control information for the structure
 * containing the HA pointer field.
 * \param [in] fid The HA pointer field ID.
 * \param [in] ptr The value of the HA pointer.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY Failed to allocate record memory.
 * \retval SHR_E_INTERNAL Internal error.
 */
static int ha_ptr_record(int unit,
                         bcmissu_comp_s_info_t *s_ctrl,
                         bcmissu_field_id_t fid,
                         void *ptr,
                         bcmissu_ha_ptr_t ptr_type)
{
    ha_ptr_tracking_t *rec = NULL;
    uint8_t sub;
    uint32_t offset;
    const issu_struct_t *s;
    uint32_t j;
    shr_ha_ptr_t *ha_ptr;
    shr_ha_lptr_t *ha_lptr;
    sal_mutex_t mtx = NULL;

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);

    /* Allocate a new record */
    SHR_ALLOC(rec,  sizeof(ha_ptr_tracking_t),  "bcmissuPtr");
    SHR_NULL_CHECK(rec, SHR_E_MEMORY);

    rec->fid = fid;
    if (ptr_type == HA_PTR) {
        ha_ptr = (shr_ha_ptr_t *)ptr;
        sub = SHR_HA_PTR_SUB(*ha_ptr);
        offset = SHR_HA_PTR_OFFSET(*ha_ptr);
        rec->regular_ptr = true;
    } else {
        ha_lptr = (shr_ha_lptr_t *)ptr;
        sub = SHR_HA_LPTR_SUB(*ha_lptr);
        offset = SHR_HA_LPTR_OFFSET(*ha_lptr);
        rec->regular_ptr = false;
    }
    rec->ptr_struct_ctrl = s_ctrl;
    rec->dest_struct_ctrl = bcmissu_find_struct_using_offset(
                                                      unit,
                                                      s_ctrl->comp_id,
                                                      sub,
                                                      offset);
    SHR_NULL_CHECK(rec->dest_struct_ctrl, SHR_E_INTERNAL);

    /*
     * Find the oldest structure definition. Use the offset value to determine
     * which field we pointing to.
     */
    s = bcmissu_find_oldest_struct(rec->dest_struct_ctrl->struct_id);
    SHR_NULL_CHECK(s, SHR_E_INTERNAL);

    offset -= rec->dest_struct_ctrl->offset;
    if (offset == 0) {
        rec->dest_s_fid = ISSU_INVALID_FIELD_ID;
    } else {
        /* Find the field with matches offset */
        for (j = 0; j < s->field_count; j++) {
            if (s->offset_get_func(s->fields[j].fid) == offset) {
                rec->dest_s_fid = s->fields[j].fid;
                break;
            }
        }
        if (j == s->field_count) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }

    /* Attach the record to the link list of pointer records for this unit */
    if (unit == BCMISSU_NO_UNIT) {
        rec->next = ha_ptr_track_gen;
        ha_ptr_track_gen = rec;
    } else {
        /* Make sure that there is a slot for this unit? */
        if (unit >= ha_ptr_track_size) {
            ha_ptr_tracking_t **tmp;
            mtx = bcmissu_unit_sync_obj_get();
            SHR_NULL_CHECK(mtx, SHR_E_INTERNAL);
            sal_mutex_take(mtx, SAL_MUTEX_FOREVER);

            tmp = ha_ptr_track;
            ha_ptr_track = NULL;
            SHR_ALLOC(ha_ptr_track,
                      sizeof(ha_ptr_tracking_t *) * (unit + 2),
                      "bcmissuHaPtrTrack");
            SHR_NULL_CHECK(ha_ptr_track, SHR_E_MEMORY);
            sal_memset(ha_ptr_track,
                       0,
                       sizeof(ha_ptr_tracking_t *) * (unit + 2));
            if (tmp) {
                sal_memcpy(ha_ptr_track,
                           tmp,
                           sizeof(ha_ptr_tracking_t *) * ha_ptr_track_size);
            }
            ha_ptr_track_size = unit + 2;
        }
        rec->next = ha_ptr_track[unit];
        ha_ptr_track[unit] = rec;
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(rec);
    }
    if (mtx) {
        sal_mutex_give(mtx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Record an HA pointer field
 *
 * This function records an HA field of a given data structure. It finds all the
 * instances of this structure and for every instance it creates a record that
 * indicate the destination of the HA pointer.
 *
 * \param [in] unit The unit associated with the HA pointer.
 * \param [in] s_info The structure information that contains the HA pointer
 * field.
 * \param [in] sid The structure ID.
 * \param [in] fid The HA pointer field ID.
 * \param [in] ptr_type Indicates if it is regular or large pointer.
 *
 * \return SHR_E_NONE success, otherwise error code
 */
static int report_ha_field(int unit,
                           const issu_struct_t *s_info,
                           bcmissu_struct_id_t sid,
                           bcmissu_field_id_t fid,
                           bcmissu_ha_ptr_t ptr_type)
{
    bcmissu_comp_s_info_t *s_ctrl;
    void *hint;
    uint8_t *ha_blk;
    uint32_t len;
    uint32_t fld_offset;
    shr_ha_ptr_t *ha_ptr = NULL;
    shr_ha_lptr_t *ha_lptr = NULL;
    uint32_t j;
    uint8_t *ha_blk_base;

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);
    fld_offset = s_info->offset_get_func(fid);
    s_ctrl = bcmissu_find_first_s_info(unit, sid,  &hint);
    while (s_ctrl) {
        len = s_ctrl->data_size;
        if (unit == BCMISSU_NO_UNIT) {
            ha_blk_base = shr_ha_gen_mem_alloc(s_ctrl->comp_id, s_ctrl->sub_id,
                                               NULL, &len);
        } else {
            ha_blk_base = shr_ha_mem_alloc(unit,
                                           s_ctrl->comp_id, s_ctrl->sub_id,
                                           NULL, &len);
        }
        SHR_NULL_CHECK(ha_blk_base, SHR_E_INTERNAL);
        for (j = 0; j < s_ctrl->instances; j++) {
            ha_blk = ha_blk_base + s_ctrl->offset + fld_offset +
                j * s_ctrl->data_size;
            if (ptr_type == HA_PTR) {
                ha_ptr = (shr_ha_ptr_t *)ha_blk;
                if (!SHR_HA_PTR_IS_NULL(*ha_ptr)) {
                    ha_ptr_record(unit, s_ctrl,  fid, ha_ptr, ptr_type);
                }
            } else {
                ha_lptr = (shr_ha_lptr_t *)ha_blk;
                if (!SHR_HA_PTR_IS_NULL(*ha_lptr)) {
                    ha_ptr_record(unit, s_ctrl,  fid, ha_lptr, ptr_type);
                }
            }
        }
        s_ctrl = bcmissu_find_next_s_info(sid, s_ctrl, &hint);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up the HA pointers units array.
 *
 * This function cleans up the HA pointer array when the last unit had been
 * updated. The function desired that all the slots of the ha_ptr_track will be
 * empty as pre-condition for desolving the HA pointer array.
 *
 * \return None.
 */
static void issu_ha_ptr_done()
{
    int unit;

    if (ha_ptr_track) {
        for (unit = 0; unit < ha_ptr_track_size; unit++) {
            if (ha_ptr_track[unit]) {
                break;
            }
        }
        /* Check if everything is empty */
        if (unit == ha_ptr_track_size) {
            sal_free(ha_ptr_track);
            ha_ptr_track_size = 0;
        }
    }
}

/*******************************************************************************
 * Public functions local to ISSU
 */
int bcmissu_ha_ptr_record_all(int unit,
                              const char *from_ver,
                              const char *to_ver)
{
    uint32_t j, k;
    const issu_struct_db_t *s;
    const issu_field_t *flds;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);
    /*
     * Record all the HA pointers start from first version. There might be
     * more pointers in newer versions but we don't have any value for them
     * so we don't need to handle that.
     */
    s = issu_struct_dict[0].ver_db;
    for (j = 0; s[j].struct_info != NULL; j++) {
        flds = s[j].struct_info->fields;
        for (k = 0; k < s[j].struct_info->field_count; k++, flds++) {
            if (flds->ha_ptr != HA_PTR_NONE) {
                /*
                 * Find the oldest version of this structure that matches
                 * the content of the HA block.
                 */

                SHR_IF_ERR_EXIT(report_ha_field(unit, s[j].struct_info,
                                                s[j].id, flds->fid,
                                                flds->ha_ptr));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmissu_update_ha_ptr(int unit)
{
    ha_ptr_tracking_t *ptr_rec;
    ha_ptr_tracking_t *tmp;
    bcmissu_comp_s_info_t *s;
    uint8_t *ha_blk;
    uint32_t len;
    uint32_t offset;
    const issu_struct_db_t  *s_db = NULL;
    const issu_struct_db_t  *last_ver_db = NULL;
    const issu_struct_db_t  *dest_s_db = NULL;
    int j;
    shr_ha_ptr_t *ha_ptr;
    const issu_field_t *flds;
    const issu_version_list_t *issu_struct_dict = bcmissu_struct_db_get();

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);
    if (unit == BCMISSU_NO_UNIT) {
        ptr_rec = ha_ptr_track_gen;
    } else {
        if (unit >= ha_ptr_track_size) {
            SHR_EXIT(); /* Nothing to do */
        }
        ptr_rec = ha_ptr_track[unit];
    }

    /*
     * Find the latest data structure definition to find the location
     * of the pointer field.
     */
    for (j = 0; issu_struct_dict[j].ver_db; j++) {
        last_ver_db = issu_struct_dict[j].ver_db;
    }

    while (ptr_rec) {
        len = ptr_rec->dest_struct_ctrl->data_size;
        s = ptr_rec->ptr_struct_ctrl;
        if (unit == BCMISSU_NO_UNIT) {
            ha_blk = shr_ha_gen_mem_alloc(s->comp_id, s->sub_id,
                                          NULL, &len);
        } else {
            ha_blk = shr_ha_mem_alloc(unit, s->comp_id, s->sub_id,
                                      NULL, &len);
        }
        SHR_NULL_CHECK(ha_blk, SHR_E_INTERNAL);

        s_db = last_ver_db;
        dest_s_db = last_ver_db;
        /* Find the data structure containing the pointer */
        while (s_db) {
            if (s_db->id == ptr_rec->ptr_struct_ctrl->struct_id) {
                break;
            }
            s_db++;
        }
        /* Verify that the pointer field still exist within this structure */
        if (s_db) {
            flds = s_db->struct_info->fields;
            for (j = 0; j < s_db->struct_info->field_count; j++, flds++) {
                if (flds->fid == ptr_rec->fid) {
                    break;
                }
            }
        }

        /* Verify that the destination structure is still defined */
        while (dest_s_db) {
            if (dest_s_db->id == ptr_rec->dest_struct_ctrl->struct_id) {
                break;
            }
            dest_s_db++;
        }

        if (s_db && dest_s_db && j < s_db->struct_info->field_count) {
            ha_blk += ptr_rec->ptr_struct_ctrl->offset;
            ha_blk += s_db->struct_info->offset_get_func(ptr_rec->fid);
            ha_ptr = (shr_ha_ptr_t *)ha_blk;
            offset =  ptr_rec->dest_struct_ctrl->offset;
            if (ptr_rec->dest_s_fid != ISSU_INVALID_FIELD_ID) {
                uint32_t fld_offset;

                fld_offset =
                  dest_s_db->struct_info->offset_get_func(ptr_rec->dest_s_fid);
                if (fld_offset != ISSU_INVALID_FLD_OFFSET) {
                    offset += fld_offset;
                }
            }
            if (ptr_rec->regular_ptr) {
                *ha_ptr = SHR_HA_PTR_OFFSET_SET(*ha_ptr, offset);
            }  else {
                *ha_ptr = SHR_HA_LPTR_OFFSET_SET(*ha_ptr, offset);
            }
        }
        tmp = ptr_rec;
        ptr_rec = ptr_rec->next;
        sal_free(tmp);
    }
    if (unit == BCMISSU_NO_UNIT) {
        ha_ptr_track_gen = NULL;
    } else {
        ha_ptr_track[unit] = NULL;
        issu_ha_ptr_done();
    }

exit:
    SHR_FUNC_EXIT();
}
