/*! \file bcmbd_db_intr.h
 *
 * IDB-EDB interrupt driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_DB_INTR_H
#define BCMBD_DB_INTR_H

#include <bcmbd/bcmbd_intr.h>

/*! \cond BCMBD_DB_INTR */
#define DB_INTR_INST_SHIFT     16
/*! \endcond */

/*!
 * \name DB interrupt parameters.
 *
 * To support situations where a single DB interrupt source has
 * multiple instances, the interrupt handler will also receive the
 * instance information along with the standard interrupt parameter.
 *
 * The instance information and the interrupt parameter are packed
 * into the same 32-bit word, so the client driver must extract these
 * two data entities through macros.
 *
 * Example:
 *
 * \code{.c}
 * void my_intr_handler(int unit, uint32_t param)
 * {
 *     uint32_t intr_param = DB_INTR_PARAM_GET(param);
 *     uint32_t intr_inst = DB_INTR_INST_GET(param);
 *     ...
 * }
 *
 * void my_init(int unit)
 * {
 *     ...
 *     bcmbd_db_intr_func_set(unit, INTR_NUM_0, intr_handler, 0);
 *     bcmbd_db_intr_func_set(unit, INTR_NUM_1, intr_handler, 1);
 *     ...
 * }
 * \endcode
 *
 * \{
 */

/*!
 * Macro to retrieve the parameter passed in by \ref
 * bcmbd_db_intr_func_set.
 */
#define DB_INTR_PARAM_GET(_prm) \
    ((_prm) & ((1 << DB_INTR_INST_SHIFT) - 1))

/*!
 * Macro to retrieve the interrupt number passed in by \ref
 * bcmbd_db_intr_clear in case the interrupt instance is encoded into
 * the parameter.
 */
#define DB_INTR_NUM_GET(_prm) \
    ((_prm) & ((1 << DB_INTR_INST_SHIFT) - 1))

/*!
 * Macro to retrieve the instance number for the interrupt. Instances
 * are numbered starting from zero.
 */
#define DB_INTR_INST_GET(_prm) \
    ((_prm) >> DB_INTR_INST_SHIFT)

/*!
 * Macro to encode the instance number for the interrupt. Instances
 * are numbered starting from zero.
 *
 * Note that this macro assumes that _prm does not already contain a
 * non-zero instance number.
 */
#define DB_INTR_INST_SET(_prm, _inst) \
    ((_prm) |= ((_inst) << DB_INTR_INST_SHIFT))

/*!
 * \}
 */

/*!
 * \brief Assign per-source DB interrupt handler.
 *
 * Assign an interrupt handler for a specific DB interrupt source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num DB interrupt source number.
 * \param [in] intr_func Per-source DB interrupt handler function.
 * \param [in] intr_param Per-source DB interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_db_intr_func_set(int unit, unsigned int intr_num,
                       bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable DB interrupt source.
 *
 * Allow a given DB interrupt source to generate hardware interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (See \ref bcmbd_db_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_db_intr_enable(int unit, unsigned int intr_num);

/*!
 * \brief Disable DB interrupt source.
 *
 * Prevent a given DB interrupt source from generating hardware
 * interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num DB interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_db_intr_disable(int unit, unsigned int intr_num);

/*!
 * \brief Clear interrupt source.
 *
 * Some interrupts must be cleared explicitly in the interrupt status
 * register after the interrupt condition has been resolved.
 *
 * If an interrupt has multiple instances, then the instance number
 * must be encoded using \ref DB_INTR_INST_SET.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number.
 *
 * \return Nothing.
 */
extern int
bcmbd_db_intr_clear(int unit, unsigned int intr_num);

#endif /* BCMBD_DB_INTR_H */
