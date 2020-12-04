/*! \file flexdigest.c
 *
 * Flex Digest common interface.
 * This file contains the common interfaces for Flex Digest.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/core/libc.h>
#include <bcm/flexdigest.h>

/*!
 * \brief Initialize bcm_flexdigest_qset_t structure.
 *
 * \param [in] qset Flex digest qualifier set.
 */
void
bcm_flexdigest_qset_t_init(bcm_flexdigest_qset_t *qset)
{
    if (qset) {
        sal_memset(qset, 0, sizeof(bcm_flexdigest_qset_t));
    }
}
