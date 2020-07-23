/*! \file bcmbd_cmicx_hotswap.h
 *
 * Hotswap management for iProc/CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief PAXB hotswap recovery.
 *
 * This function detects pending hotswap events and cleans up the PAXB
 * state accordingly. If hotswap events are not handled, the PAXB will
 * not be able to perform any DMA operations.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O error.
 * \retval SHR_E_TIMEOUT Timeout waiting for hotswap state to recover.
 */
int
bcmbd_cmicx_hotswap_recover(int unit);
