/*! \file bcmptm_tile_info.h
 *
 * Logical Table tile mapping information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_TILE_INFO_H
#define BCMPTM_TILE_INFO_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Tile access mode value.
 *
 * This structure captures the ACC_MODE value of a tile mode.
 */
typedef struct bcmptm_tile_acc_mode_value_s {

    /*! Tile mode */
    uint32_t tile_mode;

    /*! Corresponding access mode values */
    uint8_t acc_mode_value;
} bcmptm_tile_acc_mode_value_t;

/*!
 * \brief Tile access mode information.
 *
 * This structure captures the ACC_MODE programming details of
 * a single tile.
 */
typedef struct bcmptm_tile_acc_mode_info_s {
    /*! Enum for parent tile */
    uint32_t tile_id;

    /*! SID for physical register which contains the ACC_MODEf */
    bcmdrd_sid_t sid_acc_moder;

    /*! field ID for ACC_MODE field */
    bcmdrd_fid_t fid_acc_modef;

    /*! Number of elements in tile_mode, acc_mode_value arrays */
    uint8_t tile_mode_values_count;

    /*! Array of tile mode values */
    const bcmptm_tile_acc_mode_value_t *tile_mode_values;
} bcmptm_tile_acc_mode_info_t;

/*!
 * \brief Tile access mode list.
 *
 * This structure lists all of the tiles requiring ACC_MODE programming
 * for a single device.
 */
typedef struct bcmptm_tile_acc_mode_list_s {
    /*! Number of tiles managed by ITX.
     *  (Number of elements in tile_acc_mode_info array). */
    uint16_t tile_acc_mode_info_count;

    /*! Array of ptrs to tile_acc_mode_info */
    const bcmptm_tile_acc_mode_info_t *tile_acc_mode_info;
} bcmptm_tile_acc_mode_list_t;

/*!
 * \brief Logical Table Tile Mapping Information.
 *
 * Mapping information for a given physical memory.
 *
 * The instance index indicates the index ordering for the given tile_id.
 * This will range from 0 to the number of memories minus 1, where 0 indicates
 * the index ordering of the memory, where '0' means this is the first memory
 * starting at index 0, '1' means this is the next memory starting at an index
 * after the depth of the first memory, and so on.
 *
 * The tile_id is the tile name associated with this memory.
 */
typedef struct bcmptm_tile_memory_s {
    /*! Physical memory SID. */
    bcmdrd_sid_t sid;

    /*! Physical memory size. */
    uint32_t size;

    /*! Instance index. */
    uint32_t instance;

    /*! Associated tile name. */
    uint32_t tile_id;

    /*! Associated tile mode. */
    uint32_t tile_mode;

    /*! Number of logical entries per physical entry. */
    uint32_t logical_multiplier;

    /* PT index bit size. */
    uint8_t pt_index_bit_size;

} bcmptm_tile_memory_t;

/*!
 * \brief Logical Table Tile Information.
 *
 * Per-physical memory tile information for Logical Table mappings.
 */
typedef struct bcmptm_tile_info_s {
    /*! Logical table SID. */
    uint32_t sid;

    /*! Number of memories. */
    uint32_t memory_count;

    /*! Array of memories. */
    const bcmptm_tile_memory_t *memory;

} bcmptm_tile_info_t;

/*!
 * \brief Logical Table Tile List.
 *
 * Per device list of logical table tile info structures.
 */
typedef struct bcmptm_tile_list_s {
    /*! Number of tile info structures. */
    uint32_t tile_info_count;

    /*! Array of tile info structures. */
    const bcmptm_tile_info_t *tile_info;

    /*! Array of ACC_MODE informations structures. */
    const bcmptm_tile_acc_mode_list_t *tile_acc_mode_list;
} bcmptm_tile_list_t;

#endif /* BCMPTM_TILE_INFO_H */
