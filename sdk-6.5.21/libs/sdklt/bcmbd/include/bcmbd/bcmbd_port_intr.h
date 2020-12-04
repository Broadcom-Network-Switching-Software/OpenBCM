/*! \file bcmbd_port_intr.h
 *
 * Port interrupt driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PORT_INTR_H
#define BCMBD_PORT_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*! \cond BCMBD_PORT_INTR */
#define PORT_INTR_INST_SHIFT     16
/*! \endcond */

/*!
 * \name Port interrupt parameters.
 *
 * Since port interrupts typically are instantiated by port number,
 * the interrupt handler will also receive the physical port number
 * along with the standard interrupt parameter. Even if the interrupt
 * is instantiated by port macro, the interrupt handler will still
 * receive the (first) physical port number associated with the given
 * port macro.
 *
 * The port number and the interrupt parameter are packed into the
 * same 32-bit word, so the client driver must extract these two data
 * entities through macros.
 *
 * Example:
 *
 * \code{.c}
 * void my_intr_handler(int unit, uint32_t param)
 * {
 *     uint32_t intr_param = PORT_INTR_PARAM_GET(param);
 *     uint32_t port = PORT_INTR_INST_GET(param);
 *     ...
 * }
 *
 * void my_init(int unit)
 * {
 *     ...
 *     bcmbd_port_intr_func_set(unit, INTR_NUM_0, intr_handler, 0);
 *     bcmbd_port_intr_func_set(unit, INTR_NUM_1, intr_handler, 1);
 *     ...
 * }
 * \endcode
 *
 * \{
 */

/*!
 * Macro to retrieve the parameter passed in by \ref
 * bcmbd_port_intr_func_set.
 */
#define PORT_INTR_PARAM_GET(_prm) \
    ((_prm) & ((1 << PORT_INTR_INST_SHIFT) - 1))

/*!
 * Macro to retrieve the interrupt number passed in by \ref
 * bcmbd_port_intr_clear in case the interrupt instance (physical port
 * number) is encoded into the parameter.
 */
#define PORT_INTR_NUM_GET(_inum) \
    ((_inum) & ((1 << PORT_INTR_INST_SHIFT) - 1))

/*!
 * Macro to retrieve the instance number for the interrupt. The
 * instance number for port interrupts is always the physical port
 * number.
 */
#define PORT_INTR_INST_GET(_prm) \
    ((_prm) >> PORT_INTR_INST_SHIFT)

/*!
 * Macro to encode the instance number for the interrupt. The instance
 * number for port interrupts is always the physical port number.
 *
 * Note that this macro assumes that _prm does not already contain a
 * non-zero instance number.
 */
#define PORT_INTR_INST_SET(_prm, _inst) \
    ((_prm) |= ((_inst) << PORT_INTR_INST_SHIFT))

/*!
 * \}
 */

/*!
 * \brief Assign per-source PORT interrupt handler.
 *
 * Assign an interrupt handler for a specific PORT interrupt source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num PORT interrupt source number.
 * \param [in] intr_func Per-source PORT interrupt handler function.
 * \param [in] intr_param Per-source PORT interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_port_intr_func_set(int unit, unsigned int intr_num,
                        bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable PORT interrupt source.
 *
 * Allow a given PORT interrupt source to generate hardware interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (See \ref bcmbd_port_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * The phsyical port number associated with the interrupt must be
 * encoded using \ref PORT_INTR_INST_SET. Note that this also implies
 * that a given interrupt source must be enabled separately for all
 * ports that should support this interrupt source.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_port_intr_enable(int unit, unsigned int intr_num);

/*!
 * \brief Disable PORT interrupt source.
 *
 * Prevent a given PORT interrupt source from generating hardware
 * interrupts.
 *
 * Must not be called from interrupt context.
 *
 * The phsyical port number associated with the interrupt must be
 * encoded using \ref PORT_INTR_INST_SET. Note that this also implies
 * that a given interrupt source must be enabled separately for all
 * ports that should support this interrupt source.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num PORT interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_port_intr_disable(int unit, unsigned int intr_num);

/*!
 * \brief Clear interrupt source.
 *
 * Some interrupts must be cleared explicitly in the interrupt status
 * register after the interrupt condition has been resolved.
 *
 * The phsyical port number associated with the interrupt must be
 * encoded using \ref PORT_INTR_INST_SET.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_port_intr_clear(int unit, unsigned int intr_num);

#endif /* BCMBD_PORT_INTR_H */
