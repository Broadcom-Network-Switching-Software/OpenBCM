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

#include <sal/sal_types.h>

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

/*!
 * \brief Simulator hook for reading register/memory.
 *
 * \param [in] unit Unit number
 * \param [in] addrx Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] vptr Data element to store.
 * \param [in] size Size of data element.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int (*bcmbd_simhook_read)(int unit, uint32_t addrx, uint32_t addr,
                                 void *vptr, int size);

/*!
 * \brief Simulator hook for writing register/memory.
 *
 * \param [in] unit Unit number
 * \param [in] addrx Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] vptr Buffer for retrieved data element.
 * \param [in] size Size of buffer.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int (*bcmbd_simhook_write)(int unit, uint32_t addrx, uint32_t addr,
                                  void *vptr, int size);

/*!
 * \brief Simulator hook for S-channel operations.
 *
 * \param [in] unit Unit number.
 * \param [in,out] schan_msg Buffer holding the S-channel message.
 * \param [in] dwc_write Number of 32-bit words to write.
 * \param [in] dwc_read Number of 32-bit words to read.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int (*bcmbd_simhook_schan_op)(int unit, uint32_t *schan_msg,
                                     int dwc_write, int dwc_read);
#endif /* BCMBD_SIMHOOK_H */
