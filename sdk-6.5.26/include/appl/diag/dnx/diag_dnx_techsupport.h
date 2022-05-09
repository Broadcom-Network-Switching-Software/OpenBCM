
/*! \file diag_dnx_techsupport.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_TECHSUPPORT_H_INCLUDED
#define DIAG_DNX_TECHSUPPORT_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
/*************
 *  MACROES  *
 *************/

#define DNX_DIAG_TECHSUPPORT_BLOCK_NAME_LENGTH 8

/*************
 *  DEFINES  *
 *************/
typedef enum
{
    /** block table dump*/
    dnx_techsupport_cgm = 0,
    dnx_techsupport_spb,
    dnx_techsupport_ecgm,
    dnx_techsupport_pqp,
    dnx_techsupport_rqp,
    dnx_techsupport_fqp,
    dnx_techsupport_ebs,
    dnx_techsupport_eps,
    dnx_techsupport_sch,
    dnx_techsupport_ips,
    dnx_techsupport_ipt,
    /** must be at last */
    dnx_techsupport_block_count
} dnx_diag_techsupport_block;

#endif /* DIAG_DNX_TECHSUPPORT_H_INCLUDED */
