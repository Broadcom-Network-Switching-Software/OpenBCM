/*! \file xflow_macsec.h
 *
 * Field header file.
 * This file contains the management for xflow_macsec.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XFLOW_MACSEC_H
#define BCMI_LTSW_XFLOW_MACSEC_H

/*#include <bcm/xflow_macsec.h>
#include <sal/sal_types.h>*/
extern int
bcm_ltsw_xflow_macsec_init(int unit);

extern int
bcm_ltsw_xflow_macsec_detach(int unit);
#endif /* BCMI_LTSW_XFLOW_MACSEC_H */
