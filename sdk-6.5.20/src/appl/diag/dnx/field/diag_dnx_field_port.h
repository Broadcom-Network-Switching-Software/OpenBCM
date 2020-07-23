/** \file diag_dnx_field_port.h
 *
 * Diagnostics procedures, for DNX, for 'port' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_PORT_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_PORT_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/**
 * \brief
 * This enum shows different port_types.
 */
typedef enum
{
     /**
      *  Should be first.
      */
    INVALID_PORT_TYPE_E = -1,
    FIRST_PORT_TYPE_E = 0,
     /**
      * Port type LIF
      */
    PORT_TYPE_LIF = FIRST_PORT_TYPE_E,
     /**
      *  Port type PP
      */
    PORT_TYPE_PP,
     /**
      *  Port type TM
      */
    PORT_TYPE_TM,
    NUM_PORT_TYPE_E
} port_type_e;
/*
 * }
 */
/**
 * \brief
 *   List of shell options for 'port' shell commands (display, ...)
 * \remark
 *   * For now, just passive display of allocated field ports and relevant info.
 */
extern sh_sand_cmd_t Sh_dnx_field_port_cmds[];

#endif /* DIAG_DNX_PORT_H_INCLUDED */
