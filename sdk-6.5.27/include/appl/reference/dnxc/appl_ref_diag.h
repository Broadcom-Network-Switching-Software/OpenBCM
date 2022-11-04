/** \file appl_ref_diag.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_DIAG_H_INCLUDED
/*
 * {
 */
#define APPL_REF_DIAG_H_INCLUDED
/*
 * }
 */

/*
* Include files.
* {
*/

#include <soc/sand/shrextend/shrextend_debug.h>

/*
 * }
 */


/**
 * \brief - Run provided diag command and write output (in XML format) to provided buffer.
 *
 * \param [in] unit               - Unit ID
 * \param [in] flags              - Flags
 * \param [in] diag               - Diag command to run
 * \param [in] result_buffer_size - The max number of chars to copy to the output buffer
 * \param [out] result_buffer     - Pointer to the buffer to write the output of the diag command to
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_diag_get(
    int unit,
    uint32 flags,
    char *diag,
    uint32 result_buffer_size,
    char *result_buffer);

#endif /* APPL_REF_DIAG_H_INCLUDED */
