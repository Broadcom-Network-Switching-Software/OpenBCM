/*! \file bcmcth_oam_util.h
 *
 * OAM component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_OAM_UTIL_H
#define BCMCTH_OAM_UTIL_H

/*! HA subcomponent ID for BFD control. */
#define BCMOAM_BFD_CTRL_SUB_COMP_ID (1)

/*! HA subcomponent ID for BFD SP authentication. */
#define BCMOAM_BFD_AUTH_SP_SUB_COMP_ID (2)

/*! HA subcomponent ID for BFD SHA1 authentication. */
#define BCMOAM_BFD_AUTH_SHA1_SUB_COMP_ID (3)

/*! HA subcomponent ID for BFD endpoints. */
#define BCMOAM_BFD_ENDPOINTS_SUB_COMP_ID (4)

/*! HA subcomponent ID for the BFD event reporting. */
#define BCMOAM_BFD_EVENT_REPORT_SUB_COMP_ID (5)

#endif /* BCMCTH_OAM_UTIL_H */
