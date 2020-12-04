/*! \file bcmbd_sim.h
 *
 * Convenience functions for simulation support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SIM_H
#define BCMBD_SIM_H

#include <sal/sal_types.h>

/*!
 * \brief Call S-channel operation hook with debug.
 *
 * This function will print the S-channel buffer before and after
 * calling the installed function.
 *
 * \param [in] unit Unit number.
 * \param [in,out] schan_msg Buffer holding the S-channel message.
 * \param [in] dwc_write Number of 32-bit words to write.
 * \param [in] dwc_read Number of 32-bit words to read.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sim_schan_op(int unit, uint32_t *schan_msg,
                   int dwc_write, int dwc_read);

#endif /* BCMBD_SIM_H */
