/** \file oam_oamp_v1_interrupts.h
 *
 * OAMP Interrupt handling procedures for DNX version 1 .
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _OAM_OAMP_V1_INTERRUPTS_H_INCLUDED__
/*
 * {
 */
#define _OAM_OAMP_V1_INTERRUPTS_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include "include/bcm/oam.h"
#include "include/bcm/bfd.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_oamp_interrupts.h>

/*
 * }
 */
/**
 * \brief - This function gets the OAMP interrupt data in JR2 event format.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_dma_enabled - DMA enabled or not.
 * \param [out] event - Event data information.
 * \param [in,out] valid_event_exist - pointer to event valid existence indication.
 * \param [in] host_entry - Host entry in case of DMA.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function gets the interrupt data from OAMP_INTERRUPT_MESSAGE register.
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_interrupt_event_data_get(
    int unit,
    uint8 is_dma_enabled,
    dnx_oam_oamp_interrupt_event_data_t * event,
    uint8 *valid_event_exist,
    void *host_entry);
/*
 * }
 */
#endif /* _OAM_OAMP_V1_INTERRUPTS_H_INCLUDED__ */
