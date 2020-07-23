/*! \file stack.h
 *
 * BCM STACK internal APIs and Structures for XGS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XGS_STACK_H
#define BCMI_LTSW_XGS_STACK_H


/*!
 * \brief Create a specific trunk with identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] my_modid My Module ID Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_stack_set(int unit,
                      int my_modid);



#endif /* BCMI_LTSW_XGS_STACK_H */
