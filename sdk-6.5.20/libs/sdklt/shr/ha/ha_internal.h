/*! \file ha_internal.h
 *
 * HA internal class definition
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef HA_INTERNAL_H
#define HA_INTERNAL_H

#include <sal/sal_mutex.h>
#include <shr/shr_ha.h>

/* Convenience macro */
#ifndef ONE_KB
#define ONE_KB (1 * 1024)
#endif

/* Convenience macro */
#ifndef ONE_MB
#define ONE_MB (1 * 1024 * 1024)
#endif

#define RESIZE_MEMORY_SIZE (16 * ONE_MB)

/*!
 * \brief Macro to generate block ID from module ID and sub.
 *
 *  Block ID is a combination of the module ID and sub ID
 */
#define BLK_ID_GEN(_mod,_sub) (((shr_ha_mod_id)(_mod)) << 16) | (_sub)

/*!
 * \brief Macro to retrieve the module ID and sub from block ID.
 */
#define BLK_ID_RET(_b_id,_mod,_sub) \
    (_mod) = (shr_ha_mod_id)(_b_id >> 16); \
    (_sub) = (shr_ha_sub_id)(_b_id & 0xFFFF);

/*!
 * \brief Check if HA ID is a valid string
 *
 * The function validates the block ID string and log a warning if the string
 * doesn't match the HA ID guiding rules.
 *
 * \param [in] blk_id The string to validate.
 *
 * \retval None.
 */
extern void shr_ha_id_check(const char *blk_id);

#endif /* HA_INTERNAL_H */
