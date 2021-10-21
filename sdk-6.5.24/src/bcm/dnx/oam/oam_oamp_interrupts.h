/** \file oam_oamp_interrupts.h
 * 
 *
 * OAM utilities procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef _OAM_OAMP_INTERRUPTS_H_INCLUDED__
/*
 * {
 */
#define _OAM_OAMP_INTERRUPTS_H_INCLUDED__

 /*
  * Include files.
  * {
  */
#include <sal/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * Max number of JR2 format events is 13 since 640 bits/48 bits = 13.
 */
#define DNX_OAM_OAMP_MAX_JR2_FORMAT_EVENTS 13

/*
 * MACROs
 * {
 */

/*
 * }
 */

/** JR2 EVENT FORMAT */
typedef struct dnx_oam_oamp_interrupt_event_data_s
{
    /** Pointer to the OAMP_RMEP_DB for the RMEP for which event was raised */
    uint16 rmep_db_ptr;
    /** 00 - No LOC event 01 - LOC event 10 - Almost LOC event 11 - LOC Clear event */
    dbal_enum_value_field_loc_event_e loc_evt;
    /** Indicates RDI set event */
    uint8 rdi_set;
    /** Indicates RDI clear event */
    uint8 rdi_clear;
    /** Indicates there is a change in RMEP state */
    uint8 rmep_state_change;
    /** Indicates if the event is valid or not */
    uint8 is_valid;
    /** If RMEP_STATE_CHANGE is true, this gives the changed RMEP STATE for OAM/BFD */
    union
    {
        dnx_oam_oamp_oam_state_t eth_state;

        dnx_oam_oamp_bfd_state_t bfd_state;
    } rmep_state;
    /** Indicates if the event is oam or not */
    uint8 is_oam;
} dnx_oam_oamp_interrupt_event_data_t;

#endif /* _OAM_OAMP_INTERRUPTS_H_INCLUDED__ */
