/*! \file bcmltm_wb.c
 *
 * Logical Table Manager Working Buffer.
 *
 * This file contains routines to manage the working buffer space
 * required for table operations to assemble the data, such as
 * PTM format entry buffers, PTM memory parameters, field transforms input
 * and output, etc.
 *
 * Coordination of the working buffer space is required among different
 * binary tree function nodes called during the FA and EE stages
 * of a table operation.
 *
 * Working Buffer Handle:
 * The working buffer handle maintains the total working buffer
 * space required for a table.  It contains a list of working
 * buffer blocks for a given table.
 *
 * Working Buffer Block:
 * Working buffer blocks are added during the LTM metadata construction
 * based on the table needs.  Each block has a type with a well
 * defined buffer layout.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_wb_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Buffer size for field description string */
#define FIELD_STRING_SIZE          20

/* Buffer size for prefix string */
#define PREFIX_STRING_SIZE         30


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Display information for given PTM working buffer block.
 *
 * This routine displays the working buffer information for given PTM block.
 *
 * \param [in] block PTM working buffer block.
 * \param [in] type PTM block type.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
block_ptm_dump(const bcmltm_wb_block_ptm_t *block,
               bcmltm_wb_block_type_t type,
               char *prefix,
               shr_pb_t *pb)
{
    char *ptm_str;
    SHR_PB_LOCAL_DECL(pb);

    if (type == BCMLTM_WB_BLOCK_TYPE_PTM_INDEX) {
        ptm_str = "PTM_INDEX";
    } else if (type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU) {
        ptm_str = "PTM_KEYED_PTHRU";
    } else { /* BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT */
        ptm_str = "PTM_KEYED_LT";
    }

    shr_pb_printf(pb, "%s%s:\n", prefix, ptm_str);

    shr_pb_printf(pb, "%s  ptid=%d num_ops=%d\n",
                  prefix, block->ptid, block->num_ops);
    shr_pb_printf(pb, "%s  param_wsize=%d entry_wsize=%d op_wsize(1-op)=%d\n",
                  prefix, block->param_wsize, block->entry_wsize,
                  block->op_wsize);
    shr_pb_printf(pb, "%s  ofs={param=%-5d entry=%-5d}\n",
                  prefix, block->offsets.param, block->offsets.entry);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display information for given LT Private working buffer block.
 *
 * This routine displays the working buffer information for given
 * LT Private (internal elements) block.
 *
 * \param [in] block LT Private working buffer block.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
block_lt_pvt_dump(const bcmltm_wb_block_lt_pvt_t *block,
                  char *prefix,
                  shr_pb_t *pb)
{
    char *block_str;
    SHR_PB_LOCAL_DECL(pb);

    block_str = "LT_PVT";

    shr_pb_printf(pb, "%s%s:\n", prefix, block_str);

    shr_pb_printf(pb, "%s  ofs={track_index=%-5d index_absent=%-5d "
                  "pt_suppress=%-5d}\n",
                  prefix,
                  block->offsets.track_index,
                  block->offsets.index_absent,
                  block->offsets.pt_suppress);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display information for given LTA working buffer block.
 *
 * This routine displays the working buffer information for given LTA
 *  block.
 *
 * \param [in] block LTA working buffer block.
 * \param [in] type LTA block type.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
block_lta_dump(const bcmltm_wb_block_lta_t *block,
               bcmltm_wb_block_type_t type,
               char *prefix,
               shr_pb_t *pb)
{
    char *block_str;
    SHR_PB_LOCAL_DECL(pb);

    if (type == BCMLTM_WB_BLOCK_TYPE_LTA_CTH) {
        block_str = "LTA_CTH";
    } else if (type == BCMLTM_WB_BLOCK_TYPE_LTA_XFRM) {
        block_str = "LTA_XFRM";
    } else { /* BCMLTM_WB_BLOCK_TYPE_LTA_VAL */
        block_str = "LTA_VAL";
    }

    shr_pb_printf(pb, "%s%s:\n", prefix, block_str);

    shr_pb_printf(pb, "%s  in_num_fields=%d out_num_fields=%d\n",
                  prefix, block->in_num_fields, block->out_num_fields);
    shr_pb_printf(pb, "%s  in_ofs ={param=%-5d ptrs=%-5d field=%-5d}\n",
                  prefix, block->offsets.in.param, block->offsets.in.ptrs,
                  block->offsets.in.field);
    shr_pb_printf(pb, "%s  in_key_ofs ={param=%-5d ptrs=%-5d field=%-5d}\n",
                  prefix, block->offsets.in_key.param,
                  block->offsets.in_key.ptrs,
                  block->offsets.in_key.field);
    shr_pb_printf(pb, "%s  out_ofs={param=%-5d ptrs=%-5d field=%-5d}\n",
                  prefix, block->offsets.out.param, block->offsets.out.ptrs,
                  block->offsets.out.field);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display information for given LTM working buffer block.
 *
 * This routine displays the working buffer information for given
 * LTM block.
 *
 * \param [in] block LTM working buffer block.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
block_ltm_dump(const bcmltm_wb_block_ltm_t *block,
               char *prefix,
               shr_pb_t *pb)
{
    char *block_str;
    SHR_PB_LOCAL_DECL(pb);

    block_str = "LTM";

    shr_pb_printf(pb, "%s%s:\n", prefix, block_str);

    shr_pb_printf(pb, "%s  ofs={ltid=%-5d lt_field_id=%-5d select_group=%-5d "
                  "resource_info=%-5d}\n",
                  prefix,
                  block->offsets.key_value_offsets[BCMLTM_TABLE_KEYS_LTID],
                  block->offsets.key_value_offsets[BCMLTM_TABLE_KEYS_LT_FIELD_ID],
                  block->offsets.key_value_offsets[BCMLTM_TABLE_KEYS_SELECT_GROUP],
                  block->offsets.key_value_offsets[BCMLTM_TABLE_KEYS_RESOURCE_INFO]);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display information for given API Cache working buffer block.
 *
 * This routine displays the working buffer information for given
 * API Cache block.
 *
 * \param [in] block API Cache working buffer block.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
block_apic_dump(const bcmltm_wb_block_apic_t *block,
                char *prefix,
                shr_pb_t *pb)
{
    char *block_str;
    char field_str[FIELD_STRING_SIZE];
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    block_str = "APIC";

    shr_pb_printf(pb, "%s%s:\n", prefix, block_str);

    shr_pb_printf(pb, "%s  field_base_ofs=%-5d\n",
                  prefix, block->field_base_offset);
    shr_pb_printf(pb, "%s  num_field_elems=%d field_wsize=%d\n",
                  prefix, block->num_field_elems, block->field_elem_wsize);

    shr_pb_printf(pb, "%s  num_fields=%d: (field_id, field_idx_count)\n",
                  prefix, block->num_fields);
    for (i = 0; i < block->num_fields; i++) {
        if ((i%5) == 0) {
            if (i != 0) {
                shr_pb_printf(pb, "\n");
            }
            shr_pb_printf(pb, "%s    ", prefix);
        } else {
            shr_pb_printf(pb, " ");
        }
        sal_snprintf(field_str, sizeof(field_str), "(%d, %d)",
                    block->fields[i].fid,
                    block->fields[i].idx_count);
        shr_pb_printf(pb, "%-10s", field_str);
    }
    shr_pb_printf(pb, "\n");

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display information for given Field Selection working buffer block.
 *
 * This routine displays the working buffer information for given
 * Field Selection block.
 *
 * \param [in] block Field Selection working buffer block.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
block_fs_dump(const bcmltm_wb_block_fs_t *block,
              char *prefix,
              shr_pb_t *pb)
{
    char *block_str;
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    block_str = "FS";

    shr_pb_printf(pb, "%s%s:\n", prefix, block_str);

    shr_pb_printf(pb, "%s  map_index_base_ofs=%-5d\n",
                  prefix, block->map_index_base_offset);
    shr_pb_printf(pb, "%s  num_sels=%d\n",
                  prefix, block->num_sels);
    shr_pb_printf(pb, "%s  Field Selection ID:\n",
                  prefix);
    for (i = 0; i < block->num_sels; i++) {
        if ((i%10) == 0) {
            if (i != 0) {
                shr_pb_printf(pb, "\n");
            }
            shr_pb_printf(pb, "%s    ", prefix);
        } else {
            shr_pb_printf(pb, " ");
        }
        shr_pb_printf(pb, "%5d", block->sel_id_list[i]);
    }
    shr_pb_printf(pb, "\n");

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display information for given working buffer block.
 *
 * This routine displays the working buffer information for given block.
 *
 * \param [in] block Working buffer block.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
block_dump(const bcmltm_wb_block_t *block, char *prefix,
               shr_pb_t *pb)
{
    char new_prefix[PREFIX_STRING_SIZE];
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%sBlock ID: %d\n", prefix, block->id);

    sal_snprintf(new_prefix, sizeof(new_prefix), "%s  ", prefix);
    shr_pb_printf(pb, "%swsize=%d  base_ofs=%d  type=%d\n",
                  new_prefix, block->wsize, block->base_offset, block->type);

    switch(block->type) {
    case BCMLTM_WB_BLOCK_TYPE_PTM_INDEX:
    case BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU:
    case BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT:
        block_ptm_dump(&block->u.ptm, block->type, new_prefix, pb);
        break;

    case BCMLTM_WB_BLOCK_TYPE_LT_PVT:
        block_lt_pvt_dump(&block->u.lt_pvt, new_prefix, pb);
        break;

    case BCMLTM_WB_BLOCK_TYPE_LTA_CTH:
    case BCMLTM_WB_BLOCK_TYPE_LTA_XFRM:
    case BCMLTM_WB_BLOCK_TYPE_LTA_VAL:
        block_lta_dump(&block->u.lta, block->type, new_prefix, pb);
        break;

    case BCMLTM_WB_BLOCK_TYPE_LTM:
        block_ltm_dump(&block->u.ltm, new_prefix, pb);
        break;

    case BCMLTM_WB_BLOCK_TYPE_APIC:
        block_apic_dump(&block->u.apic, new_prefix, pb);
        break;

    case BCMLTM_WB_BLOCK_TYPE_FS:
        block_fs_dump(&block->u.fs, new_prefix, pb);
        break;

    default:
        break;
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Cleanup a working buffer block.
 *
 * This function frees any memory allocated inside the given working
 * buffer block type.
 *
 * \param [in] block Working buffer block.
 */
static void
block_cleanup(bcmltm_wb_block_t *block)
{
    if (block->type == BCMLTM_WB_BLOCK_TYPE_APIC) {
        bcmltm_wb_block_apic_t *apic = &block->u.apic;
        SHR_FREE(apic->fields);
    } else if (block->type == BCMLTM_WB_BLOCK_TYPE_FS) {
        bcmltm_wb_block_fs_t *fs = &block->u.fs;
        SHR_FREE(fs->sel_id_list);
    }

    return;
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_handle_init(uint32_t sid,
                      bool logical,
                      bcmltm_wb_handle_t *handle)
{
    if  (handle == NULL) {
        return SHR_E_PARAM;
    }

    sal_memset(handle, 0, sizeof(*handle));

    /*
     * Working buffer reserves 1 word.
     *
     * This is to done to reserve the offset value of 0 as invalid.
     * It also indicates handle has been initialized (> 0).
     */
    handle->wsize = 1;         /* Reserve 1 word */
    handle->sid = sid;
    handle->logical = logical;
    handle->num_blocks = 0;
    handle->blocks = NULL;

    return SHR_E_NONE;
}


void
bcmltm_wb_handle_cleanup(bcmltm_wb_handle_t *handle)
{
    bcmltm_wb_block_t *block;
    bcmltm_wb_block_t *next;

    if  (handle == NULL) {
        return;
    }

    block = handle->blocks;
    while (block != NULL) {
        next = block->next;

        block_cleanup(block);
        SHR_FREE(block);
        block = next;
    }

    handle->wsize = 0;
    handle->sid = 0;
    handle->num_blocks = 0;
    handle->blocks = NULL;
}


int
bcmltm_wb_handle_validate(bcmltm_wb_handle_t *handle)
{
    if  (handle == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid null working buffer handle\n")));
        return SHR_E_INTERNAL;
    }

    if (handle->wsize == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM working buffer handle has not been "
                            "initialized\n")));
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}


uint32_t
bcmltm_wb_wsize_get(const bcmltm_wb_handle_t *handle)
{
    if  (handle == NULL) {
        return 0;
    }

    return handle->wsize;
}


bcmltm_wb_block_t *
bcmltm_wb_block_add(uint32_t wsize, bcmltm_wb_block_type_t type,
                    bcmltm_wb_handle_t *handle)
{
    bcmltm_wb_block_t *block = NULL;

    if  (handle == NULL) {
        return NULL;
    }

    block = sal_alloc(sizeof(*block), "bcmltmWbBlock");
    if (block == NULL) {
        return NULL;
    }

    sal_memset(block, 0, sizeof(*block));

    /* Set generic block information */
    block->id = handle->num_blocks;
    block->wsize = wsize;
    block->base_offset = handle->wsize;
    block->type = type;
    block->next = NULL;

    /* Add block to end of list */
    if (handle->blocks == NULL) {
        handle->blocks = block;
    } else {
        bcmltm_wb_block_t *last_block = handle->blocks;

        while (last_block->next != NULL) {
            last_block = last_block->next;
        }
        last_block->next = block;
    }

    /* Update handler info */
    handle->wsize += block->wsize;
    handle->num_blocks++;

    return block;
}


const bcmltm_wb_block_t *
bcmltm_wb_block_find(const bcmltm_wb_handle_t *handle,
                     bcmltm_wb_block_id_t block_id)
{
    const bcmltm_wb_block_t *block = NULL;

    if  (handle == NULL) {
        return NULL;
    }

    block = handle->blocks;
    while (block != NULL) {
        if (block->id == block_id) {
            break;
        }
        block = block->next;
    }

    return block;
}

uint32_t
bcmltm_wb_block_type_count(const bcmltm_wb_handle_t *handle,
                           bcmltm_wb_block_type_t block_type)
{
    const bcmltm_wb_block_t *block = NULL;
    uint32_t count = 0;

    if  (handle == NULL) {
        return count;
    }

    block = handle->blocks;
    while (block != NULL) {
        if (block->type == block_type) {
            count++;
        }
        block = block->next;
    }

    return count;
}

int
bcmltm_wb_block_list_get(const bcmltm_wb_handle_t *handle,
                         bcmltm_wb_block_type_t block_type,
                         uint32_t list_max,
                         const bcmltm_wb_block_t **block_list,
                         uint32_t *num)
{
    const bcmltm_wb_block_t *block = NULL;
    uint32_t idx = 0;

    *num = 0;
    if  (handle == NULL) {
        return SHR_E_NONE;
    }

    block = handle->blocks;
    while (block != NULL) {
        if (block->type == block_type) {
            if (idx >= list_max) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Block list array size of %d is "
                                    "not large enough\n"),
                           list_max));
                return SHR_E_INTERNAL;
            }
            block_list[idx] = block;
            idx++;
        }
        block = block->next;
    }

    *num = idx;

    return SHR_E_NONE;
}

void
bcmltm_wb_dump(const bcmltm_wb_handle_t *handle,
               shr_pb_t *pb)
{
    bcmltm_wb_block_t *block = NULL;
    char *prefix;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Working Buffer ---\n");

    if  (handle == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "Total Size (words): %d\n", handle->wsize);
    shr_pb_printf(pb, "Table ID: %d\n", handle->sid);
    shr_pb_printf(pb, "Blocks: %d\n", handle->num_blocks);

    prefix = "  ";
    block = handle->blocks;
    while (block != NULL) {
        block_dump(block, prefix, pb);
        block = block->next;
    }

    SHR_PB_LOCAL_DONE();
}

