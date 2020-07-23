/*! \file bcmdrd_tile.h
 *
 * This file contains the header information for flex tiles.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_TILE_H
#define BCMDRD_TILE_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>


/*! Maximum number of sections that are connected to the output of the Mux. */
#define BCMDRD_MUX_SECTION_OUT_MAX  2

/*! Maximum number of sections that are connected to the input of the Mux. */
#define BCMDRD_MUX_SECTION_IN_MAX   1

/*! Invalid TILE identifier. */
#define BCMDRD_INVALID_TILE         0xFF

/*!
 * \brief Multiplexer information for the physcial tile.
 *
 * This structure provides mux information for each multiplexer
 * available in the physical tile that handles the operation on
 * the logical table.
 */
typedef struct bcmdrd_tile_mux_info_s {
    /*! Level of the multiplexer */
    uint8_t level;

    /*! Granularity of the multiplexer*/
    uint8_t gran;

    /*!
     * Unique id of a multiplexer which is local
     * to the input sections.
     */
    uint8_t id;

    /*! Number of in sections of a multiplexer. */
    uint8_t num_in_sections;

    /*! Array of in section id of a multiplexer. */
    uint8_t in_sec_id[BCMDRD_MUX_SECTION_IN_MAX];

    /*! Number of out sections of a multiplexer. */
    uint8_t num_out_sections;

    /*! Array of out section ids of a multiplexer */
    uint8_t out_sec_id[BCMDRD_MUX_SECTION_OUT_MAX];

    /*! Array of offsets in out section ids of a multiplexer*/
    uint16_t out_sec_offset[BCMDRD_MUX_SECTION_OUT_MAX];
} bcmdrd_tile_mux_info_t;

/*!
 * \brief Multiplexer information for the physcial tile.
 *
 * This structure provides mux information for each multiplexer
 * available in the physical tile that handles the operation on
 * the logical table.
 */
typedef struct bcmdrd_tile_mux_info_db_s {
    /*! Name of the tile. */
    const char *name;

    /*! Tile identifier. */
    uint32_t id;

    /*! Number of multiplexers. */
    uint16_t count;

    /*! Mux information for each multiplexer. */
    const bcmdrd_tile_mux_info_t *info;
}bcmdrd_tile_mux_info_db_t;

/*!
 * \brief Device configuration for TILE info.
 *
 * Per-device TILE info configuration.
 */
typedef struct bcmdrd_tile_mux_info_db_list_s {
    /*! Tile configuration. */
    const bcmdrd_tile_mux_info_db_t *conf;
} bcmdrd_tile_mux_info_db_list_t;

/*!
 * \brief Update TILE multiplexer information.
 *
 * Update TILE multiplexer information for the given device.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  mux_info        Multiplexer information.
 */
extern void
bcmdrd_tile_info_update(int unit,
                        const bcmdrd_tile_mux_info_db_t **mux_info);

/*!
 * \brief Retrieve the multiplexer information.
 *
 * Retrieve the multiplexer information for the given tile.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  tile_id         Tile identifier.
 * \param [out] mux_info_db     Pointer to the mux information.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNAVAIL        Multiplexer info not available.
 */
extern int
bcmdrd_tile_mux_info_db_get(int unit,
                            uint32_t tile_id,
                            const bcmdrd_tile_mux_info_db_t **mux_info_db);
#endif /* BCMDRD_TILE_H */
