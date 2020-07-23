/** \file bcm_int/dnx/switch/switch_tpid.h
 * 
 * Internal DNX SWITCH-TPID APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SWITCH_TPID_H_INCLUDED
/* { */
#  define SWITCH_TPID_H_INCLUDED

#  ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#  endif

/** TPID invalid index: Table index is [0..7] */
#define BCM_DNX_SWITCH_TPID_INDEX_INVALID   (-1)

/** There are 8 globals TPIDs:
 *  See the Programming Guide PP document for full details   */
#define BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS  (8)

/** There are 7 valid globals TPIDs:
 *  See the Programming Guide PP document for full details   */
#define BCM_DNX_SWITCH_TPID_NUM_OF_VALID_GLOBALS  (7)

/** TPID7 is reserved:
 *  See the Programming Guide PP document for full details   */
#define BCM_DNX_SWITCH_TPID_RESERVED_ENTRY  (7)

/* Tag Protocol Identifier (TPID) value for CoE */
#define DNX_TPID_VALUE_COE              (0x8100)

shr_error_e dnx_switch_tpid_value_get(
    int unit,
    int tpid_index,
    uint16 *tpid_value);

shr_error_e dnx_switch_tpid_value_set(
    int unit,
    int tpid_index,
    uint16 tpid_value);

shr_error_e dnx_switch_tpid_index_get(
    int unit,
    uint16 tpid_value,
    int *tpid_index);

/* } */
#endif /* SWITCH_TPID_H_INCLUDED */
