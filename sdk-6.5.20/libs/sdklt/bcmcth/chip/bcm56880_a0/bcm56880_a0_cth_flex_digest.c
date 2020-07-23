/*! \file bcm56880_a0_cth_flex_digest.c
 *
 * Flex Digest drivers for bcm56880 A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmissu/issu_api.h>

#include <bcmcth/bcmcth_flex_digest_drv.h>
#include <bcmcth/generated/flex_digest_ha.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_table.h>

#include "bcm56880_a0_cth_flex_digest_db.h"

/******************************************************************************
* Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEXDIGEST
#define NODE_LEVEL_MAX 40
#define NUM_BINS 16
#define NUM_SETS 3
#define BCMCTH_FLEX_DIGEST_MUX_CONT_MAP_SUB_COMP_ID 1

/* total number of muxs in presel memory */
#define FD_FLEX_KEY_MUX_NUM    18

typedef struct fd_match_id_map_s {
    bcm56880_a0_cth_flex_digest_fmap_t *mmap;
    int size;
} fd_match_id_map_t;

typedef struct fd_flex_key_map_s {
    bcm56880_a0_cth_flex_digest_fkey_fmap_t *kmap;
    int size;
} fd_flex_key_map_t;

typedef struct {
    bcm56880_a0_cth_flex_digest_pkt_node_t *node;
    bcmltd_fid_t fid;
    int cnt;
} pkt_node_level_t;

typedef struct fd_dev_desc_s {
    fd_flex_key_map_t flex_key_map;  /* flex key map data base */
    fd_match_id_map_t match_id_map;  /* match id data base */
    bcm56880_a0_cth_flex_digest_pkt_node_t *root_node;
    int graph_levels;  /* parser graph levels */
    /* LKUP ctrl logical field id to mux container map. index is container num*/
    bcmcth_flex_digest_mux_cont_map_info_t *mux_cont_info;
} fd_dev_desc_t;

static bcmdrd_sid_t fd_hsalt_sid[NUM_SETS][NUM_BINS] = {
       {FLEX_DIGEST_HASH_XOR_SALTS_A_0r, FLEX_DIGEST_HASH_XOR_SALTS_A_1r,
        FLEX_DIGEST_HASH_XOR_SALTS_A_2r, FLEX_DIGEST_HASH_XOR_SALTS_A_3r,
        FLEX_DIGEST_HASH_XOR_SALTS_A_4r, FLEX_DIGEST_HASH_XOR_SALTS_A_5r,
        FLEX_DIGEST_HASH_XOR_SALTS_A_6r, FLEX_DIGEST_HASH_XOR_SALTS_A_7r,
        FLEX_DIGEST_HASH_XOR_SALTS_A_8r, FLEX_DIGEST_HASH_XOR_SALTS_A_9r,
        FLEX_DIGEST_HASH_XOR_SALTS_A_10r, FLEX_DIGEST_HASH_XOR_SALTS_A_11r,
        FLEX_DIGEST_HASH_XOR_SALTS_A_12r, FLEX_DIGEST_HASH_XOR_SALTS_A_13r,
        FLEX_DIGEST_HASH_XOR_SALTS_A_14r, FLEX_DIGEST_HASH_XOR_SALTS_A_15r},

       {FLEX_DIGEST_HASH_XOR_SALTS_B_0r, FLEX_DIGEST_HASH_XOR_SALTS_B_1r,
        FLEX_DIGEST_HASH_XOR_SALTS_B_2r, FLEX_DIGEST_HASH_XOR_SALTS_B_3r,
        FLEX_DIGEST_HASH_XOR_SALTS_B_4r, FLEX_DIGEST_HASH_XOR_SALTS_B_5r,
        FLEX_DIGEST_HASH_XOR_SALTS_B_6r, FLEX_DIGEST_HASH_XOR_SALTS_B_7r,
        FLEX_DIGEST_HASH_XOR_SALTS_B_8r, FLEX_DIGEST_HASH_XOR_SALTS_B_9r,
        FLEX_DIGEST_HASH_XOR_SALTS_B_10r, FLEX_DIGEST_HASH_XOR_SALTS_B_11r,
        FLEX_DIGEST_HASH_XOR_SALTS_B_12r, FLEX_DIGEST_HASH_XOR_SALTS_B_13r,
        FLEX_DIGEST_HASH_XOR_SALTS_B_14r, FLEX_DIGEST_HASH_XOR_SALTS_B_15r},

       {FLEX_DIGEST_HASH_XOR_SALTS_C_0r, FLEX_DIGEST_HASH_XOR_SALTS_C_1r,
        FLEX_DIGEST_HASH_XOR_SALTS_C_2r, FLEX_DIGEST_HASH_XOR_SALTS_C_3r,
        FLEX_DIGEST_HASH_XOR_SALTS_C_4r, FLEX_DIGEST_HASH_XOR_SALTS_C_5r,
        FLEX_DIGEST_HASH_XOR_SALTS_C_6r, FLEX_DIGEST_HASH_XOR_SALTS_C_7r,
        FLEX_DIGEST_HASH_XOR_SALTS_C_8r, FLEX_DIGEST_HASH_XOR_SALTS_C_9r,
        FLEX_DIGEST_HASH_XOR_SALTS_C_10r, FLEX_DIGEST_HASH_XOR_SALTS_C_11r,
        FLEX_DIGEST_HASH_XOR_SALTS_C_12r, FLEX_DIGEST_HASH_XOR_SALTS_C_13r,
        FLEX_DIGEST_HASH_XOR_SALTS_C_14r, FLEX_DIGEST_HASH_XOR_SALTS_C_15r}
};

/* the field mapping should be same for all 56880 type units */
static fd_dev_desc_t *fd_dev_desc[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

bcm56880_a0_cth_flex_digest_fkey_fmap_get_f bcm56880_a0_cth_flex_digest_key_map_get[BCMDRD_CONFIG_MAX_UNITS];
bcm56880_a0_cth_flex_digest_fmap_get_f bcm56880_a0_cth_flex_digest_match_id_map_get[BCMDRD_CONFIG_MAX_UNITS];
bcmcth_flex_digest_variant_field_id_t *bcm56880_a0_cth_flex_digest_variant_field_id_desc[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
* Private functions
 */
static int bcm56880_a0_cth_fd_drv_init(int unit, bool warm)
{
    int rv;
    fd_dev_desc_t *desc;
    bcmcth_flex_digest_mux_cont_map_info_t *mux_cont_map_info = NULL;
    uint32_t ha_alloc_size = 0;
    uint32_t ha_req_size = 0;
    uint32_t offset = 0;

    SHR_FUNC_ENTER(unit);

    /* allocate device descriptor */
    if (fd_dev_desc[unit] != NULL) {;
        SHR_FREE(fd_dev_desc[unit]);
        fd_dev_desc[unit] = NULL;
    }
    desc = NULL;
    SHR_ALLOC(desc, sizeof(fd_dev_desc_t), "bcmCthLbhDevDescBuf");
    SHR_NULL_CHECK(desc, SHR_E_MEMORY);
    sal_memset(desc, 0, sizeof(fd_dev_desc_t));
    fd_dev_desc[unit] = desc;

    ha_req_size = sizeof(bcmcth_flex_digest_mux_cont_map_info_t) +
                  FD_FLEX_KEY_MUX_NUM * sizeof(bcmcth_flex_digest_mux_cont_map_t);
    ha_alloc_size = ha_req_size;

    mux_cont_map_info =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_FLEX_DIGEST_COMP_ID,
                         BCMCTH_FLEX_DIGEST_MUX_CONT_MAP_SUB_COMP_ID,
                         "bcmCthFlexDigestMuxContMap",
                         &ha_alloc_size);
    SHR_NULL_CHECK(mux_cont_map_info, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                             SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(mux_cont_map_info, 0, ha_alloc_size);
        mux_cont_map_info->array_size = FD_FLEX_KEY_MUX_NUM;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_FLEX_DIGEST_COMP_ID,
                                BCMCTH_FLEX_DIGEST_MUX_CONT_MAP_SUB_COMP_ID, 0,
                                ha_req_size, 1,
                                BCMCTH_FLEX_DIGEST_MUX_CONT_MAP_INFO_T_ID));
    } else {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_get(unit,
                             BCMCTH_FLEX_DIGEST_MUX_CONT_MAP_INFO_T_ID,
                             BCMMGMT_FLEX_DIGEST_COMP_ID,
                             BCMCTH_FLEX_DIGEST_MUX_CONT_MAP_SUB_COMP_ID,
                             &offset));
        mux_cont_map_info = (bcmcth_flex_digest_mux_cont_map_info_t *)
                                 ((uint8_t *)mux_cont_map_info + offset);
    }
    desc->mux_cont_info = mux_cont_map_info;

    rv = (*bcm56880_a0_cth_flex_digest_key_map_get[unit])(unit,
                      &desc->flex_key_map.kmap,
                      &desc->flex_key_map.size);

    SHR_IF_ERR_EXIT(rv);
    rv = (*bcm56880_a0_cth_flex_digest_match_id_map_get[unit])(unit,
                      &desc->match_id_map.mmap,
                      &desc->match_id_map.size,
                      &desc->root_node,
                      &desc->graph_levels);
    SHR_IF_ERR_EXIT(rv);
    SHR_FUNC_EXIT();

exit:
    SHR_FREE(fd_dev_desc[unit]);
    fd_dev_desc[unit] = NULL;
    SHR_FUNC_EXIT();
}

/*
 * Get array of hardware information entries for the given logical table
 */
static int bcm56880_a0_cth_fd_hentry_info_get(int unit,
    bcmltd_sid_t lsid,
    int *size,
    bcmcth_flex_digest_hentry_info_t *info)
{
    int rv;
    int arr_size; /* original array buffer size */
    bcmdrd_sid_t drd_sid = 0;
    int i;
    bcmcth_flex_digest_variant_field_id_t *desc;

    SHR_FUNC_ENTER(unit);

    desc = bcm56880_a0_cth_flex_digest_variant_field_id_desc[unit];

    if (size == NULL) {
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }
    /* return number of hardware memory entry descriptor */
    if (info == NULL) {
        if (lsid == desc->presel_sid) {
            *size = 1;
        } else if (lsid == desc->lkup_sid) {
            *size = 3;
        } else if (lsid == FLEX_DIGEST_HASH_SALTt) {
            *size = 48;
        } else {
            *size = 0;
        }
    } else {
        if (lsid == desc->presel_sid) {
            info->sid   = FLEX_DIGEST_LKUP_CTRL_PRE_SELm;
            info->wsize = bcmdrd_pt_entry_wsize(unit, info->sid);
            info->type  = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA;
            *size = 1;
        } else if (lsid == desc->lkup_sid) {
            arr_size = *size;
            info->sid   = FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm;
            info->wsize = bcmdrd_pt_entry_wsize(unit, info->sid);
            info->type  = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_KEY;
            *size = 1;
            info++;

            if (*size < arr_size) {
                info->sid   = FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_DATA_ONLYm;
                info->wsize = bcmdrd_pt_entry_wsize(unit, info->sid);
                info->type  = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA;
                *size += 1;
                info++;
            }
            if (*size < arr_size) {
                info->sid   = FLEX_DIGEST_LKUP_NET_LAYER_EXTRA_POLICY_0m;
                info->wsize = bcmdrd_pt_entry_wsize(unit, info->sid);
                info->type  = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA;
                *size += 1;
                info++;
            }
        } else if (lsid == FLEX_DIGEST_HASH_SALTt) {
            arr_size = *size;
            if (arr_size > NUM_SETS * NUM_BINS) {
                arr_size = NUM_SETS * NUM_BINS;
            }

            for (i = 0; i < arr_size; i++) {
                drd_sid = fd_hsalt_sid[i / NUM_BINS][ i % NUM_BINS];
                info[i].sid   = drd_sid;
                info[i].wsize = 1;
                info[i].type  = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA;
            }
            *size = arr_size;
        } else {
            *size = 0;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Check whether the m_node(matched_node) belongs to the same graph
 * from the top_node. This is a recursive function
 * return FALSE: not in the same graph, TRUE: in the same graph
 */
static int fd_parser_graph_validate(int unit,
            bcm56880_a0_cth_flex_digest_pkt_node_t *top_node,
            bcm56880_a0_cth_flex_digest_pkt_node_t *m_node)
{
    int i;

    if (top_node == NULL) {
        return FALSE;
    }
    if (top_node == m_node) {
        return TRUE;
    }

    if (m_node->level <= top_node->level) {
        return FALSE;
    }

    for (i = 0; i < top_node->nn_size; i++) {
        if (fd_parser_graph_validate(unit,top_node->nn_list[i],m_node)==TRUE) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * Packet header type fields will be checked against the packet parser graph.
 * All fields must come from the same top node in order to be considered valid.
 * The first node not in the graph will be displayed.
 */
static int fd_lkup_entry_validate (int unit, bcmltd_sid_t lsid,
            bcmltd_field_t *data)
{
    int size;
    int i;
    int level;
    bcmltd_field_t *fld_list;
    fd_dev_desc_t *desc = fd_dev_desc[unit];
    const bcmltd_table_rep_t *tbl;
    pkt_node_level_t *node_lvl = NULL;
    bcm56880_a0_cth_flex_digest_fmap_t *mmap;
    bcm56880_a0_cth_flex_digest_pkt_node_t *top_node;
    int idx;

    SHR_FUNC_ENTER(unit);

    size = desc->graph_levels * sizeof(pkt_node_level_t);
    SHR_ALLOC(node_lvl, size, "bcmCthFdNodeTmpBuf");
    SHR_NULL_CHECK(node_lvl, SHR_E_MEMORY);
    sal_memset(node_lvl, 0, size);

    mmap  = desc->match_id_map.mmap;
    if (data == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmltd_table_get(lsid);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fld_list = data;

    /* find the list of fields for match_id */

    while (fld_list) {
        if (!fld_list->data) {
            fld_list = fld_list->next;
            continue;
        }
        idx = -1;
        for (i = 0; i < desc->match_id_map.size; i++) {
            if (fld_list->id == mmap[i].fid) {
                if (idx >= 0) {
                    /* duplicate packet type, don't check */
                    idx = -1;
                    break;
                }
                idx = i;
            }
        }
        if (idx >= 0) {
            /* node level starts with 1 */
            level = mmap[idx].node->level - 1;
            if (level >= desc->graph_levels) {
                LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Flex Digest: Max level exceeds: %s\n"),
                                tbl->field[fld_list->id].name));
                     SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* node cannot be in same level */
            if (node_lvl[level].cnt) {
                LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Flex Digest: Invalid packet type combination: %s\n"),
                                tbl->field[fld_list->id].name));
                     SHR_ERR_EXIT(SHR_E_PARAM);
            }
            node_lvl[level].node = mmap[idx].node;
            node_lvl[level].cnt  = 1;
            node_lvl[level].fid  = fld_list->id;
        }
        fld_list = fld_list->next;
    }

    /* validate the packet types(nodes) in sequence starting at the top.
     * Each node in hierarchical packet parser graph has been assigned
     * with a level. If a node is connected by multiple other nodes, the lowest
     * level is assigned. The nodes in the graph must not form a loop. This
     * should be the case for the packet parser graph.
     */

    top_node = NULL;
    for (i = 0; i < desc->graph_levels; i++) {
        if (node_lvl[i].cnt) {
            if (top_node == NULL) {
                top_node = node_lvl[i].node;
            } else {
                if (fd_parser_graph_validate(unit, top_node,
                     node_lvl[i].node) == FALSE) {
                    LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Flex Digest: Invalid packet type combination: %s\n"),
                          tbl->field[node_lvl[i].fid].name));
                         SHR_ERR_EXIT(SHR_E_PARAM);
                }
                top_node = node_lvl[i].node;
            }
        }
    }
exit:
    SHR_FREE(node_lvl);
    SHR_FUNC_EXIT();
}

/*
 * Program the logical fields(in) into the FLEX_DIGEST_LKUP hardware TCAM entry.
 * The logical fields are packet header types and ctrl_ids which are the key
 * field of the TCAM entry.
 */
static int fd_lkup_entry_set (int unit,
    uint32_t *entry,
    bcmltd_field_t *in)
{
    int i,k;
    uint32_t value;
    bcm56880_a0_cth_flex_digest_fmap_t *mmap;
    uint8_t cont[FD_FLEX_KEY_MUX_NUM];
    uint8_t cont_mask[FD_FLEX_KEY_MUX_NUM];
    uint64_t match_id;
    uint64_t match_id_mask;
    uint64_t val64;
    uint64_t mask64;
    int rv;
    int found;
    bcmltd_field_t *data;
    fd_dev_desc_t *desc = fd_dev_desc[unit];
    uint32_t tmp_buf[2];

    static bcmdrd_fid_t fids[FD_FLEX_KEY_MUX_NUM] = {
        MUX0_SIZE0_OUT_0f,  MUX0_SIZE0_OUT_1f,  MUX0_SIZE0_OUT_2f,
        MUX0_SIZE0_OUT_3f,  MUX0_SIZE0_OUT_4f,  MUX0_SIZE0_OUT_5f,
        MUX0_SIZE0_OUT_6f,  MUX0_SIZE0_OUT_7f,  MUX0_SIZE0_OUT_8f,
        MUX0_SIZE0_OUT_9f,  MUX0_SIZE0_OUT_10f, MUX0_SIZE0_OUT_11f,
        MUX0_SIZE0_OUT_12f, MUX0_SIZE0_OUT_13f, MUX0_SIZE0_OUT_14f,
        MUX0_SIZE0_OUT_15f, MUX0_SIZE0_OUT_16f, MUX0_SIZE0_OUT_17f};
    static bcmdrd_fid_t fid_masks[FD_FLEX_KEY_MUX_NUM] = {
       MUX0_SIZE0_OUT_0_MASKf, MUX0_SIZE0_OUT_1_MASKf, MUX0_SIZE0_OUT_2_MASKf,
       MUX0_SIZE0_OUT_3_MASKf, MUX0_SIZE0_OUT_4_MASKf, MUX0_SIZE0_OUT_5_MASKf,
       MUX0_SIZE0_OUT_6_MASKf, MUX0_SIZE0_OUT_7_MASKf, MUX0_SIZE0_OUT_8_MASKf,
       MUX0_SIZE0_OUT_9_MASKf, MUX0_SIZE0_OUT_10_MASKf,MUX0_SIZE0_OUT_11_MASKf,
       MUX0_SIZE0_OUT_12_MASKf,MUX0_SIZE0_OUT_13_MASKf,MUX0_SIZE0_OUT_14_MASKf,
       MUX0_SIZE0_OUT_15_MASKf,MUX0_SIZE0_OUT_16_MASKf,MUX0_SIZE0_OUT_17_MASKf};

    SHR_FUNC_ENTER(unit);

    mmap  = desc->match_id_map.mmap;

    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < FD_FLEX_KEY_MUX_NUM; i++) {
        cont[i] = 0;
        cont_mask[i] = 0;
    }

    match_id = 0;
    match_id_mask = 0;
    data = in;
    while (data) {
        found = FALSE;
        for (i = 0; i < desc->match_id_map.size; i++) {
            if (data->id == mmap[i].fid) {
                if (data->data) {
                    val64  = (uint64_t)(mmap[i].node->phy_fld->z_value);
                    mask64 = (uint64_t)(mmap[i].node->phy_fld->z_mask);
                    match_id |= (val64 << mmap[i].node->phy_fld->z_minbit);
                    match_id_mask |=
                               (mask64 << mmap[i].node->phy_fld->z_minbit);
                }
                found = TRUE;
                break;
            }
        }

        /* if not match_id, then it is ctrl_ids */
        if (found == FALSE) {
            for (k = 0; k < desc->mux_cont_info->array_count; k++) {
                if (desc->mux_cont_info->mux_cont_map[k].lk_fld == data->id) {
                    cont[k] = data->data;
                    break;
                } else if (desc->mux_cont_info->mux_cont_map[k].lk_fld_mask ==
                     data->id) {
                    cont_mask[k] = data->data;
                    break;
                } else {  /* not found */
                }
            }
        }
        data = data->next;
    }

    tmp_buf[1] = (uint32_t)(match_id >> 32);
    tmp_buf[0] = (uint32_t)(match_id);
    rv = bcmdrd_pt_field_set(unit,
             FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm,
             entry, MATCH_IDf, tmp_buf);
    SHR_IF_ERR_EXIT(rv);

    tmp_buf[1] = (uint32_t)(match_id_mask >> 32);
    tmp_buf[0] = (uint32_t)(match_id_mask);
    rv = bcmdrd_pt_field_set(unit,
             FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm,
             entry, MATCH_ID_MASKf, tmp_buf);
    SHR_IF_ERR_EXIT(rv);

    for (i = 0; i < FD_FLEX_KEY_MUX_NUM; i++) {
        value = cont[i];
        rv = bcmdrd_pt_field_set(unit,
                 FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm,
                 entry, fids[i], &value);
        value = cont_mask[i];
        rv = bcmdrd_pt_field_set(unit,
                 FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm,
                 entry, fid_masks[i], &value);
    }

    value = 3;
    rv = bcmdrd_pt_field_set(unit,
             FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm,
             entry, VALIDf, &value);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Program the logical fields(in) into the FLEX_DIGEST_LKUP hardware TCAM policy
 * table entry. The logical fields are bin array fields which will be programed
 * into bin command fields in the policy table.
 */
static int fd_policy_entry_set (int unit,
    uint32_t **entry_tbl,
    bcmltd_field_t *in,
    bcmcth_flex_digest_variant_field_id_t *desc)
{
    int i;
    uint16_t val16;
    uint32_t val32;
    int rv;
    bcmdrd_fid_t fids[NUM_BINS] = {
        A_CMDS_0f,  A_CMDS_1f,  A_CMDS_2f,  A_CMDS_3f,
        A_CMDS_4f,  A_CMDS_5f,  A_CMDS_6f,  A_CMDS_7f,
        A_CMDS_8f,  A_CMDS_9f,  A_CMDS_10f, A_CMDS_11f,
        A_CMDS_12f, A_CMDS_13f, A_CMDS_14f, A_CMDS_15f};
    bcmdrd_fid_t fids_b[NUM_BINS] = {
        B_CMDS_0f,  B_CMDS_1f,  B_CMDS_2f,  B_CMDS_3f,
        B_CMDS_4f,  B_CMDS_5f,  B_CMDS_6f,  B_CMDS_7f,
        B_CMDS_8f,  B_CMDS_9f,  B_CMDS_10f, B_CMDS_11f,
        B_CMDS_12f, B_CMDS_13f, B_CMDS_14f, B_CMDS_15f};
    uint16_t bin_a[NUM_BINS];
    uint16_t bin_b[NUM_BINS];

    SHR_FUNC_ENTER(unit);

    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i< NUM_BINS; i++) {
        bin_a[i] = 0;
        bin_b[i] = 0;
    }

    while (in) {
        val16 = (uint16_t)(in->data);
        /* FLEX_DIGEST_LKUPt_BIN_Af */
        if (in->id == desc->bin_a_id) {
            if (val16) {
                /* conversion equation: (hid - 1) / 2 */
                val16--;
                val16 /= 2;
                bin_a[in->idx] |= val16 << 1;
                bin_a[in->idx] |= 1;   /* valid bit */
            }

        /* FLEX_DIGEST_LKUPt_BIN_A_FLEX_DIGEST_LKUP_MASK_PROFILE_IDf */
        } else if (in->id == desc->bin_a_mask_prof_id) {
            bin_a[in->idx] |= val16 << 9;

        /* FLEX_DIGEST_LKUPt_BIN_Bf */
        } else if (in->id == desc->bin_b_id) {
            if (val16) {
                val16--;
                val16 /= 2;
                bin_b[in->idx] |= val16 << 1;
                bin_b[in->idx] |= 1;  /* valid bit */
            }

        /* FLEX_DIGEST_LKUPt_BIN_B_FLEX_DIGEST_LKUP_MASK_PROFILE_IDf */
        } else if (in->id == desc->bin_b_mask_prof_id) {
            bin_b[in->idx] |= val16 << 9;

        /* FLEX_DIGEST_LKUPt_BIN_C_MASKf */
        } else if (in->id == desc->bin_c_mask_id) {
            val32 = val16;
            rv = bcmdrd_pt_field_set(unit,
                     FLEX_DIGEST_LKUP_NET_LAYER_EXTRA_POLICY_0m,
                     entry_tbl[1], C_MASKf, &val32);
            SHR_IF_ERR_EXIT(rv);
        }
        in = in->next;
    }

    for (i = 0; i < NUM_BINS; i++) {
        val32 = bin_a[i];
        rv = bcmdrd_pt_field_set(unit,
             FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_DATA_ONLYm,
             entry_tbl[0], fids[i],&val32);
        SHR_IF_ERR_EXIT(rv);
    }

    for (i = 0; i < NUM_BINS; i++) {
        val32 = bin_b[i];
        rv = bcmdrd_pt_field_set(unit,
             FLEX_DIGEST_LKUP_NET_LAYER_EXTRA_POLICY_0m,
             entry_tbl[1], fids_b[i], &val32);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Program the logical fields(in) into the FLEX_DIGEST_LKUP_PRESEL hardware
 * table entry. The logical fields select which fields are part of the
 * FLEX_DIGEST_LKUP TCAM key.
 * The index of mux_cont_map is the physical mux container number.
 * This corresponds to the mux container number in
 * FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm. We should not re-order the mux
 * container value in the subsequent update action.
 */
static int fd_presel_entry_set (int unit,
    uint32_t *entry,
    bcmltd_field_t *in)
{
    int i,k,l;
    int ksize;
    uint32_t value;
    bcm56880_a0_cth_flex_digest_fkey_fmap_t *kmap;
    bcmcth_flex_digest_mux_cont_map_t new_map[FD_FLEX_KEY_MUX_NUM];
    bcmcth_flex_digest_mux_cont_map_t *cur_map;
    int cur_map_size;
    int new_map_size;
    fd_dev_desc_t *desc = fd_dev_desc[unit];
    static bcmdrd_fid_t fids[FD_FLEX_KEY_MUX_NUM] = {
        MUX0_SIZE0_SEL_0f,  MUX0_SIZE0_SEL_1f,  MUX0_SIZE0_SEL_2f,
        MUX0_SIZE0_SEL_3f,  MUX0_SIZE0_SEL_4f,  MUX0_SIZE0_SEL_5f,
        MUX0_SIZE0_SEL_6f,  MUX0_SIZE0_SEL_7f,  MUX0_SIZE0_SEL_8f,
        MUX0_SIZE0_SEL_9f,  MUX0_SIZE0_SEL_10f, MUX0_SIZE0_SEL_11f,
        MUX0_SIZE0_SEL_12f, MUX0_SIZE0_SEL_13f, MUX0_SIZE0_SEL_14f,
        MUX0_SIZE0_SEL_15f, MUX0_SIZE0_SEL_16f, MUX0_SIZE0_SEL_17f};

    SHR_FUNC_ENTER(unit);

    kmap  = desc->flex_key_map.kmap;
    ksize = desc->flex_key_map.size;

    /* delete action */
    if (in == NULL) {
        desc->mux_cont_info->array_count = 0;
        SHR_EXIT();
    }

    /* logical fields to mux container map */
    k = 0;
    while (in) {
        if (in->data) {
            for (l = 0; l < ksize; l++) {
                if (in->id == (uint32_t)kmap[l].l_fld) {
                    new_map[k].cont_id      = kmap[l].hid;
                    new_map[k].lk_fld       = (uint32_t)kmap[l].k_fld;
                    new_map[k].lk_fld_mask  = (uint32_t)kmap[l].k_fld_mask;
                    k++;
                   break;
                }
            }
        }
        in = in->next;
    }
    new_map_size = k;
    cur_map_size = desc->mux_cont_info->array_count;
    cur_map = desc->mux_cont_info->mux_cont_map;

    /* check if the previous fields are deleted, remove it if so */
    for (i = 0; i < cur_map_size; i++) {
        for (l = 0; l < new_map_size; l++) {
            if (new_map[l].cont_id == cur_map[i].cont_id) {
                /* found */
                break;
            }
        }
        if (l == new_map_size) {
            /* not found, remove from the current container map */
            cur_map[i].cont_id = 0;
            cur_map[i].lk_fld  = 0;
            cur_map[i].lk_fld_mask  = 0;
        }
    }

    /* insert the new fields */
    for (l = 0; l < new_map_size; l++) {
        for (i = 0; i < cur_map_size; i++) {
            if (new_map[l].cont_id == cur_map[i].cont_id) {
                /* already exist in the current map. skip */
                break;
            }
        }
        if (i == cur_map_size) {
            /* new field, add to the current map. First find empty slot */
            for (k = 0; k < cur_map_size; k++) {
                if ((!cur_map[k].cont_id) && (!cur_map[k].lk_fld)) {
                    break;
                }
            }
            if (k < FD_FLEX_KEY_MUX_NUM) {
                if (k == cur_map_size) {
                    cur_map_size++;
                }
                cur_map[k].cont_id = new_map[l].cont_id;
                cur_map[k].lk_fld  = new_map[l].lk_fld;
                cur_map[k].lk_fld_mask  = new_map[l].lk_fld_mask;
            } else {
                SHR_EXIT();
            }
        }
    }

    desc->mux_cont_info->array_count = cur_map_size;

    for (i = 0; i < desc->mux_cont_info->array_count; i++) {
        value = desc->mux_cont_info->mux_cont_map[i].cont_id;
        SHR_IF_ERR_EXIT(bcmdrd_pt_field_set(unit,
           FLEX_DIGEST_LKUP_CTRL_PRE_SELm, entry, fids[i], &value));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Program the logical fields(in) into the FLEX_DIGEST_HASH_SALT hardware
 * table entry.
 */
static int fd_hash_salt_entry_set (int unit,
    bcmcth_flex_digest_entry_info_t *info,
    bcmltd_field_t *in)
{
    int i,j;
    uint16_t val16;
    uint32_t value = 0;
    bcmdrd_sid_t drd_sid = FLEX_DIGEST_HASH_XOR_SALTS_A_1r;
    uint16_t bin[NUM_SETS][NUM_BINS];
    int size;

    SHR_FUNC_ENTER(unit);

    if (in == NULL) {
        SHR_EXIT(); /* delete action */
    }

    for (j = 0; j < NUM_SETS; j++) {
        for (i = 0; i < NUM_BINS; i++) {
            bin[j][i] = 0;
        }
    }

    while (in) {
        val16 = (uint16_t)(in->data);
        if (in->id == FLEX_DIGEST_HASH_SALTt_BIN_Af) {
            bin[0][in->idx] = val16;
        } else if (in->id == FLEX_DIGEST_HASH_SALTt_BIN_Bf) {
            bin[1][in->idx] = val16;
        } else if (in->id == FLEX_DIGEST_HASH_SALTt_BIN_Cf) {
            bin[2][in->idx] = val16;
        }
        in = in->next;
    }

    if (info->size > (NUM_SETS * NUM_BINS)) {
        size = (NUM_SETS * NUM_BINS);
    } else {
        size = info->size;
    }
    for (i = 0; i < size; i++) {
        value   = bin[i / NUM_BINS][i % NUM_BINS];
        drd_sid = fd_hsalt_sid[i / NUM_BINS][i % NUM_BINS];
        SHR_IF_ERR_EXIT(bcmdrd_pt_field_set(unit,
            drd_sid, info->tbl[i], VALUEf, &value));
    }
exit:
    SHR_FUNC_EXIT();
}

static int bcm56880_a0_cth_fd_entry_validate(int unit,
    bcmltd_sid_t lsid,
    bcmcth_flex_digest_entry_info_t *info,
    bcmltd_field_t *in)
{
    int rv;
    bcmcth_flex_digest_variant_field_id_t *desc;

    SHR_FUNC_ENTER(unit);

    desc = bcm56880_a0_cth_flex_digest_variant_field_id_desc[unit];
    if (lsid == desc->lkup_sid) {
        rv = fd_lkup_entry_validate(unit, lsid, in);
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

static int bcm56880_a0_cth_fd_drv_cleanup (int unit)
{

    SHR_FUNC_ENTER(unit);

    if (fd_dev_desc[unit] != NULL) {;
        SHR_FREE(fd_dev_desc[unit]);
        fd_dev_desc[unit] = NULL;
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

static int bcm56880_a0_cth_fd_entry_set(int unit,
    bcmltd_sid_t lsid,
    bcmcth_flex_digest_entry_info_t *info,
    bcmltd_field_t *in)
{
    int rv;
    bcmcth_flex_digest_variant_field_id_t *desc;

    SHR_FUNC_ENTER(unit);

    if (info == NULL || info->size == 0 || info->tbl == NULL) {
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }
    desc = bcm56880_a0_cth_flex_digest_variant_field_id_desc[unit];

    if (lsid == desc->presel_sid) {
        rv = fd_presel_entry_set(unit, info->tbl[0], in);
        SHR_IF_ERR_EXIT(rv);
    } else if (lsid == FLEX_DIGEST_HASH_SALTt) {
        rv = fd_hash_salt_entry_set(unit, info, in);
        SHR_IF_ERR_EXIT(rv);
    } else if (lsid == desc->lkup_sid) {
        if (info->type == BCMCTH_FLEX_DIGEST_ENTRY_TYPE_KEY) {
            rv = fd_lkup_entry_set(unit, info->tbl[0], in);
            SHR_IF_ERR_EXIT(rv);
        } else { /* data entry */
            if (info->size < 2) {
                rv = SHR_E_PARAM;
                SHR_ERR_EXIT(rv);
            }
            rv = fd_policy_entry_set (unit, info->tbl, in, desc);
            SHR_IF_ERR_EXIT(rv);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int bcm56880_a0_cth_fd_derived_fields_get(int unit,
    bcmltd_sid_t lsid,
    int *size,
    uint32_t *buf)
{
    fd_dev_desc_t *desc;
    int count;
    int i,j;

    SHR_FUNC_ENTER(unit);

    desc = fd_dev_desc[unit];

    if (*size == 0 && buf != NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    count = desc->flex_key_map.size * 2;

    if (buf == NULL) {
        *size = count;
        SHR_EXIT();
    }

    if (count > *size) {
        count = *size;
    }
    for (i = 0, j = 0; i < count/2; i++) {
        buf[j] = desc->flex_key_map.kmap[i].k_fld;
        j++;
        buf[j] = desc->flex_key_map.kmap[i].k_fld_mask;
        j++;
    }
    *size = (count/2) * 2;

exit:
    SHR_FUNC_EXIT();
}

static int bcm56880_a0_cth_fd_variant_id_get(int unit,
    bcmcth_flex_digest_variant_field_id_t **desc)
{
    *desc = bcm56880_a0_cth_flex_digest_variant_field_id_desc[unit];
    return SHR_E_NONE;
}

bcmcth_flex_digest_drv_t
bcm56880_a0_cth_fd_drv = {
    /* Flex Digest driver */
    .drv_init        = bcm56880_a0_cth_fd_drv_init,
    .hentry_info_get = bcm56880_a0_cth_fd_hentry_info_get,
    .entry_set       = bcm56880_a0_cth_fd_entry_set,
    .entry_validate  = bcm56880_a0_cth_fd_entry_validate,
    .derived_fields_get  = bcm56880_a0_cth_fd_derived_fields_get,
    .variant_id_get  = bcm56880_a0_cth_fd_variant_id_get,
    .drv_cleanup     = bcm56880_a0_cth_fd_drv_cleanup,
};

/******************************************************************************
* Public functions
 */
bcmcth_flex_digest_drv_t * bcm56880_a0_cth_flex_digest_drv_common_get(int unit)
{
    return &bcm56880_a0_cth_fd_drv;
}
