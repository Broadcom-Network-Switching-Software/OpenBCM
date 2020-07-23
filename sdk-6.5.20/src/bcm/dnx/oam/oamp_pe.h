/** \file oamp_pe.h
 * OAMP PE support - contains internal functions and definitions 
 * for support of the OAMP PE functionality
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _OAMP_PE_H_INCLUDED__
/*
 * {
 */
#define _OAMP_PE_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_error.h>
#include "include/bcm/oam.h"
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */

/**
* \brief
*  Initialize OAMP-PE with all of its configurations.
*   \param [in] unit  -  Relevant unit.
*   \param [in] profile  -  MEP-PE profile name in SW state
*   \param [out] profile_id  -  MEP-PE profile saved id in HW
* \return
*   shr_error_e
*/
shr_error_e dnx_oamp_pe_sw_mep_pe_profile_get(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e profile,
    uint32 *profile_id);

/**
* \brief
*  Get preconfigured UDH header size.
*   \param [in] unit  -  Relevant unit.
*   \param [out] udh_size  -  preconfigured UDH size
* \return
*   shr_error_e
*/
shr_error_e oamp_pe_udh_header_size_get(
    int unit,
    uint32 *udh_size);

/**
* \brief
*  Initialize OAMP-PE with all of its configurations.
*   \param [in] unit  -  Relevant unit.
* \return
*   shr_error_e
*/
shr_error_e dnx_oamp_pe_module_init(
    int unit);

/*
 * }
 */
#endif /* _OAMP_PE_H_INCLUDED__ */
