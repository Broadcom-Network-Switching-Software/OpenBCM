/*! \file bcmbd_simhook.h
 *
 * \brief Simulator hooks for register/memory access.
 *
 * The simulator hooks will allow basic testing of register/memory
 * access, even if no real hardware or device simulator (BCMSIM) is
 * available.
 *
 * The simulator hooks will override the hardware drivers with
 * application-provided functions for reading and writing device
 * registers and memories.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SIMHOOK_H
#define BCMBD_SIMHOOK_H

#include <bcmbd/bcmbd.h>

/*!
 * \name Simulator storage types.
 * \anchor BCMBD_SIM_xxx
 *
 * Simulator storage types are used to distinguish various classes of
 * elements stored in the same database. This is needed because the
 * raw addresses may not be unique across all storage classes.
 *
 * \{
 */

/*! Storage element is a directly accessed (typically PCI) register. */
#define BCMBD_SIM_PCI_REG       (0 << 16)

/*! Storage element is a SOC register (indirect access). */
#define BCMBD_SIM_SOC_REG       (1 << 16)

/*! Storage element is a SOC memory (indirect access). */
#define BCMBD_SIM_SOC_MEM       (2 << 16)

/*! \} */

/*! Simulator hooks for reading register/memory. */
extern bcmbd_hmi_read_f bcmbd_simhook_read[BCMDRD_CONFIG_MAX_UNITS];

/*! Simulator hooks for writing register/memory. */
extern bcmbd_hmi_write_f bcmbd_simhook_write[BCMDRD_CONFIG_MAX_UNITS];

/*! Simulator hooks for S-channel operations. */
extern bcmbd_schan_op_f bcmbd_simhook_schan_op[BCMDRD_CONFIG_MAX_UNITS];

#endif /* BCMBD_SIMHOOK_H */
