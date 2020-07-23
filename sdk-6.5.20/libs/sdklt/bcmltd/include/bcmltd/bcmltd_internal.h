/*! \file bcmltd_internal.h
 *
 * Internal logical table definition
 *
 * Not intended for callers outside of LTD.
 *
 * Not optimized for space.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTD_INTERNAL_H
#define BCMLTD_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief Default Logical table definitions and count
 *
 * Definitions are sorted by name, count is part of the structure
 */
extern const bcmltd_table_conf_t bcmltd_default_table_conf;

/*!
 * \brief Logical table definitions and count
 *
 * Pointer to active logical table definitions
 */
extern const bcmltd_table_conf_t *bcmltd_table_conf;

/*!
 * \brief Set table configuration data.
 *
 * Set table configuration data.
 *
 * \param [in] table_conf       Table configuration data.
 *
 * \retval Pointer to the old table configuration data.
 */

const bcmltd_table_conf_t*
bcmltd_table_conf_set(const bcmltd_table_conf_t *table_conf);

/*!
 * \brief Resolve enum by table field data.
 *
 * Resolve enum by table field data.
 *
 * \param [in]  sid    Logical table ID.
 * \param [in]  fid    Logical field ID.
 * \param [in]  str    Enum string to be resolved.
 * \param [out] value  Resovled value.
 *
 * \retval SHR_E_NONE      OK
 * \retval SHR_E_UNAVAIL   Not resolved.
 * \retval SHR_E_PARAM     SID, FID is not valid.
 */
extern int
bcmltd_resolve_enum(const uint32_t sid,
                    const uint32_t fid,
                    const char *str,
                    uint64_t *value);

#endif /* BCMLTD_INTERNAL_H */
