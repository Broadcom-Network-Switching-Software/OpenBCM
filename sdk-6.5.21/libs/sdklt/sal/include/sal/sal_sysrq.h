/*! \file sal_sysrq.h
 *
 * System requests for error handling and application control.
 * Not required for normal operation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_SYSRQ_H
#define SAL_SYSRQ_H

#include <sal_config.h>

/*! Possible system request types. */
typedef enum sal_sysrq_e {

    /*! Normal program exit with error code. */
    SAL_SYSRQ_EXIT = 0,

    /*! Abnormal program exit. */
    SAL_SYSRQ_ABORT,

    /*! Reset/reboot system. */
    SAL_SYSRQ_RESET,

    /*! Show backtrace. */
    SAL_SYSRQ_BACKTRACE,

    /* No more enums beyond this. */
    SAL_SYSRQ_COUNT

} sal_sysrq_t;

/*!
 * \brief Perform system request.
 *
 * This function is used to provide a portable interface to various
 * system functions such as abort, exit, reset, etc.
 *
 * The core SDK libraries will never call this API.
 *
 * \param [in] req System request type.
 * \param [in] prm
 */
extern void
sal_sysrq(sal_sysrq_t req, unsigned long prm);

#endif /* SAL_SYSRQ_H */
