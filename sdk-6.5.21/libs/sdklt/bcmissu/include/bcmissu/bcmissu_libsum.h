/*! \file bcmissu_libsum.h
 *
 * Library checksum string utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmissu/generated/bcmissu_libsum.h>

/*!
 * \brief Get library checksum string.
 *
 * Get ASCII string of checksum generated across all SDK source files.
 *
 * If no checksum is available, an empty string is returned.
 *
 * \return Pointer to checksum string.
 */
extern const char *
bcmissu_libsum_str(void);
