/*! \file bcmbd_config.h
 *
 * Config Base Driver (BD) APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CONFIG_H
#define BCMBD_CONFIG_H

#include <sal/sal_types.h>

/*!
 * \brief Get uint32_t configuration parameter value.
 *
 * \param [in] unit Unit number.
 * \param [in] def_val Default value if parameter is undefined.
 * \param [in] tbl_name Configuration parameter table name.
 * \param [in] fld_name Configuration parameter field name.
 *
 * \return Parameter value.
 */
uint32_t
bcmbd_config_get(int unit, uint32_t def_val,
                 const char *tbl_name, const char *fld_name);

/*!
 * \brief Get uint64_t configuration parameter value.
 *
 * \param [in] unit Unit number.
 * \param [in] def_val Default value if parameter is undefined.
 * \param [in] tbl_name Configuration parameter table name.
 * \param [in] fld_name Configuration parameter field name.
 *
 * \return Parameter value.
 */
uint64_t
bcmbd_config64_get(int unit, uint64_t def_val,
                   const char *tbl_name, const char *fld_name);

#endif /* BCMBD_CONFIG_H */
