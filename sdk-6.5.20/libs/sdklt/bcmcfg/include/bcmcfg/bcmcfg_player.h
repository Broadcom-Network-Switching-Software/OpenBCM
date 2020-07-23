/*! \file bcmcfg_player.h
 *
 * BCMCFG interface for BCMLTP player.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_PLAYER_H
#define BCMCFG_PLAYER_H

#include <shr/shr_fmm.h>
#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_lt_types.h>

/*! Index Key field, used for memories or array registers. */
#define BCMCFG_PT_FIELD_KEY_INDEX  ((bcmdrd_fid_t)-1)

/*! Port Key field, used for port-based registers. */
#define BCMCFG_PT_FIELD_KEY_PORT   ((bcmdrd_fid_t)-2)

/*!
 * \brief Table data to play back.
 */
typedef struct bcmcfg_tbl_list_s {
    /*! Table ID. */
    uint32_t sid;

    /*! Table Name. */
    const char *name;

    /*! Unit - true if unit in the set */
    bool unit_set[BCMDRD_CONFIG_MAX_UNITS];

    /*! Field data */
    shr_fmm_t *field;

    /*! Reader line number. */
    int line;

    /*! Reader column number. */
    int column;

    /*! Next table in list */
    struct bcmcfg_tbl_list_s *next;
} bcmcfg_tbl_list_t;

/*!
 * \brief Linked list of enum strings.
 */
typedef struct bcmcfg_playback_enum_s {
    /*! Enum value string */
    const char *value;

    /*! Next value in enum list */
    struct bcmcfg_playback_enum_s *next;
} bcmcfg_playback_enum_t;

/*!
 * \brief Physical and logical tables to play back.
 */
typedef struct bcmcfg_playback_list_s {
    /*! Locus (file or string). */
    const char *locus;

    /*! Logical table list. */
    bcmcfg_tbl_list_t *lt;

    /*! Enum list length. */
    size_t enum_count;

    /*! Enum list head. */
    bcmcfg_playback_enum_t *enum_head;

    /*! Enum list tail. */
    bcmcfg_playback_enum_t *enum_tail;

    /*! Physical table list. */
    bcmcfg_tbl_list_t *pt;

    /*! Next playback set in list */
    struct bcmcfg_playback_list_s *next;
} bcmcfg_playback_list_t;

/*!
 * \brief Get a BCMCFG playback list.
 *
 * Get a pointer to the BCMCFG playback list, if available.
 *
 * \param [in]  unit            Unit number.
 * \param [out] list            Pointer to playback list.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_playback_get(int unit, const bcmcfg_playback_list_t **list);

/*!
 * \brief Free BCMCFG playback list.
 *
 * Free the BCMCFG playback list.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_playback_free(int unit);

#endif /* BCMCFG_PLAYER_H */
