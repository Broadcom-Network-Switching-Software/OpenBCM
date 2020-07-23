/**
 * \file bcm_int/dnx/instru/instru_sflow.h
 * Internal DNX INSTRU APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INSTRU_SFLOW_H_INCLUDED
/*
 * {
 */
#define INSTRU_SFLOW_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/**
 * \brief
 * SFLOW - Mapping of system-port to Interface is UNKNOWN
 * Note:
 * According SFLOW SPEC V5 - 0 means "interface is not known"
 */
#define SFLOW_PACKET_INTERFACE_UNKNOWN  (0)
/*
 * }
 */

/*
  * Internal functions.
  * {
  */

/*
 * }
 */
#endif /* INSTRU_SFLOW_H_INCLUDED */
