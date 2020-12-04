/*! \file diag_dnx_knet.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_KNET_H_INCLUDED
#define DIAG_DNX_KNET_H_INCLUDED

/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern char cmd_dnx_knet_usage[];

/*************
 * FUNCTIONS *
 */

/*!
 * \brief
 *   Manage kernel network functions
 *
 *  \par DIRECT INPUT:
 *    \param [in] u -
 *      Unit number.
 *    \param [in] *a -
 *      Pointer to struct with command content.
 *  \par DIRECT OUTPUT:
 *    cmd_result_t - \n
 *      Return value of knetctrl call.
 *  \remark
 *    None
 *  \see
 *    None
 */
cmd_result_t cmd_dnx_knet(
    int u,
    args_t * a);

#endif /* DIAG_DNX_KNET_H_INCLUDED */
