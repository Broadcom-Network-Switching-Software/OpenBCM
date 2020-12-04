/**
* \file        field_tcam.h
*
* DESCRIPTION :
*       This file implements the general functionality for TCAM in field.
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    sw_state_init( unit )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_TCAM_H_INCLUDED
/** { */
#define FIELD_TCAM_H_INCLUDED

/*
 * { Defines
 */

/**
 * Indicated whether or not we keep a "shadow" for TCAM, that is keep all of the TCAM's info in SW state to be 
 * accessed in addition to the HW.
 */
#define DNX_FIELD_TCAM_SHADOW_KEEP  (1)

/**
 * Indicated whether or not we read TCAM entries from the "shadow" instead of from the HW.
 * Can only be true if DBX_FIELD_TCAM_SHADOW_KEEP is true.
 */
#define DNX_FIELD_TCAM_SHADOW_READ  (1 && DNX_FIELD_TCAM_SHADOW_KEEP)

/*
 * } Defines
 */

/**
 * \brief
 * shr_error_e dnx_field_tcam_sw_state_init( unit )
 * Inits SW STATE for the whole TCAM module
 *
 * \param [in] unit
 * \return
 *  \retval _SHR_E_NONE success
 * \remark
 * SPECIAL OUTPUT:
 *      SWSTATE:
 *          dnx_field_tcam_access_sw  The TCAM access general SWSTATE
 */
shr_error_e dnx_field_tcam_sw_state_init(
    int unit);

/** } */
#endif
