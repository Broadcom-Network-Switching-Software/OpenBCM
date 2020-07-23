/*! \file itx_internal.h
 *
 * Defines internal to ITX
 *
 * This file defines data structures for Index Tile-aware Xform LTA.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ITX_INTERNAL_H
#define ITX_INTERNAL_H

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_tile_info.h>

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)   \
    extern const bcmptm_tile_list_t _bd##_vu##_va##_ptm_itm_tile_list;
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Tile index range information.
 *
 * This structure captures the index range of a single tile.
 */
typedef struct bcmptm_itx_tile_range_info_s {
    /*! Physical Tile ID. */
    uint32_t tile_id;

    /*! LT Index range minimum. */
    uint32_t tile_index_min;

    /*! LT Index range maximum. */
    uint32_t tile_index_max;

    /*! Number of logical entries per physical entry. */
    uint32_t logical_multiplier;

    /* PT index bit size. */
    uint8_t pt_index_bit_size;
} bcmptm_itx_tile_range_info_t;

/*!
 * \brief Tile index range list.
 *
 * This structure contains the index map of all physical tiles
 * currently composing a single LT.
 */
typedef struct bcmptm_itx_tile_range_list_s {
    /*! Current number of tiles mapped from this LT. */
    uint32_t tile_num;

    /*! Set of current PT map structures for this LT. */
    bcmptm_itx_tile_range_info_t tile_list[];
} bcmptm_itx_tile_range_list_t;

/*!
 * \brief LT index range information.
 *
 * This structure captures the LT index range of a single physical table.
 */
typedef struct bcmptm_itx_index_range_info_s {
    /*! Physical Table id. */
    uint32_t pt_id;

    /*! Physical Tile ID. */
    uint32_t tile_id;

    /*! LT Index range minimum. */
    uint32_t lt_index_min;

    /*! LT Index range maximum. */
    uint32_t lt_index_max;

    /*! Number of logical entries per physical entry. */
    uint32_t logical_multiplier;
} bcmptm_itx_index_range_info_t;

/*!
 * \brief LT index range list.
 *
 * This structure contains the index map of all physical tables
 * currently composing a single LT.
 */
typedef struct bcmptm_itx_index_range_list_s {
    /*! Current LT entry limit, sum of currently mapped PT entries. */
    uint32_t entry_limit;

    /*! Current number of PTs mapped from this LT. */
    uint32_t pt_num;

    /*! Set of current PT map structures for this LT. */
    bcmptm_itx_index_range_info_t pt_map_list[];
} bcmptm_itx_index_range_list_t;

/*!
 * \brief Index LT information.
 *
 * For each tile-aware index logical table requiring index translation,
 * this structure records the information required to process
 * a tile mode change, and to respond to the LTA transform operations
 * querying the index L2P map.
 */
typedef struct bcmptm_itx_lt_map_s {
    /*! Logical Table id. */
    uint32_t ltid;

    /*! Maximum number of PTs mapped from this LT. */
    uint32_t pt_max;

    /*! Maximum number of tiles mapped from this LT. */
    uint32_t tile_max;

    /*! LT entry maximum, sum of all possible PT entries. */
    uint32_t entry_maximum;

    /*! The current index_range_list array index. */
    uint8_t active_list;

    /*! The inactive index_range_list is a backup for atomic abort. */
    bool backup_valid;

    /* Index L2P map LT index ranges. */
    bcmptm_itx_index_range_list_t *index_range_list[2];

    /* Index L2P map LT index ranges. */
    bcmptm_itx_tile_range_list_t *tile_range_list;

    /* Potential L2P map info for this LT. */
    const bcmptm_tile_info_t *hw_tile_info;
} bcmptm_itx_lt_map_t;

/*!
 * \brief One unit's index LT information.
 *
 * The list of tile-aware index LT map structures s for a given unit.
 */
typedef struct bcmptm_itx_lt_map_list_s {
    /* Number of index L2P map. */
    uint32_t lt_num;

    /* Each array element is the tile mode map for a single LT. */
    bcmptm_itx_lt_map_t lt_maps[];
} bcmptm_itx_lt_map_list_t;

#endif /* ITX_INTERNAL_H */
