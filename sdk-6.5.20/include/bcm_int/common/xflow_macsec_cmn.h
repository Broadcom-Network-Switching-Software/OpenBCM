/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef XFLOW_MACSEC_CMN_H
#define XFLOW_MACSEC_CMN_H

#if defined(INCLUDE_XFLOW_MACSEC)

#include <bcm/xflow_macsec.h>
#include <esw/xflow_macsec_esw_defs.h>

#define BCM_XFLOW_MACSEC_SUBPORT_ID_INDEX_GET(subport_id)   \
        XFLOW_MACSEC_SUBPORT_ID_INDEX_GET(subport_id)

#define BCM_XFLOW_MACSEC_SUBPORT_ID_DIR_TYPE_GET(subport_id) \
        XFLOW_MACSEC_DIR_TYPE_GET(subport_id)

#define BCM_XFLOW_MACSEC_SUBPORT_ID_CREATE(flag, id) \
        XFLOW_MACSEC_SUBPORT_ID_CREATE(flag, id)

extern int 
_bcm_common_xflow_macsec_init(int unit);

extern int
_bcm_common_xflow_macsec_intr_register(int unit);

extern int
_bcm_common_xflow_macsec_deinit(int unit);

extern int
_bcm_common_xflow_macsec_sync(int unit);

#endif /* defined(INCLUDE_XFLOW_MACSEC) */

#endif /* XFLOW_MACSEC_CMN_H */

