/*! \file shr_ha_types.h
 *
 * SHR HA public API types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_HA_TYPES_H
#define SHR_HA_TYPES_H

#include <sal/sal_types.h>

/*!
 * \brief The maximal length of block ID string.
 */
#define MAX_BLK_ID_STR_LEN 16

/*!
 * \brief HA module ID type.
 */
typedef uint16_t shr_ha_mod_id;

/*!
 * \brief HA sub module ID type.
 */
typedef uint16_t shr_ha_sub_id;

/*! \brief Maximum number of sub-component IDs. */
#define SHR_HA_SUB_IDS  1024

/*!
 * \brief HA Block report
 */
typedef struct {
    /*! The size of the block. */
    uint32_t size;
    /*! Block ID as it was given by the component. */
    const char *blk_id;
} shr_ha_usage_report_t;

#endif /* SHR_HA_TYPES_H */
