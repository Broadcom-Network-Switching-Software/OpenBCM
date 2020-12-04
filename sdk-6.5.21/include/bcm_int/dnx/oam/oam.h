/** \file bcm_int/dnx/oam/oam.h
 *
 * Internal DNX OAM APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx_dispatch.h>
#ifndef OAM_H_INCLUDED__
/* { */
#define OAM_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs
 * {
 */
/** Maximum value of OAM default profile */
#define DNX_OAM_DEFAULT_PROFILE_MAX_VALUE 3
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * }
 */
/*
 * }
 */

/**
 * \brief - Set the additional GAL special label for MPLS-TP OAM.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gal - any value from 0 to 15. '13' means no additional GAL.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_additional_gal_special_label_set(
    int unit,
    uint32 gal);
#endif /* OAM_H_INCLUDED__ */
