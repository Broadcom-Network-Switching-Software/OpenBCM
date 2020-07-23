/*! \file shr_exception.h
 *
 * Exception handler support functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_EXCEPTION_H
#define SHR_EXCEPTION_H

/*!
 * \brief Prototype for exception handler function.
 *
 * Please refer to \ref shr_exception for more information.
 *
 * \param [in] unit Unit number.
 * \param [in] err_code Error code.
 * \param [in] file Source file name.
 * \param [in] line Source file line number.
 * \param [in] func Function name.
 */
typedef void (*shr_exception_handler_f)(int unit, int err_code,
                                        const char *file, int line,
                                        const char *func);

/*!
 * \brief Install exception handler function.
 *
 * The application is responsible for processing the exception
 * information provided by a driver function, e.g. showing a backtrace
 * on the console.
 *
 * Please refer to \ref shr_exception for more information.
 *
 * \param [in] func Exception handler function.
 */
extern void
shr_exception_handler_set(shr_exception_handler_f func);

/*!
 * \brief Request conditional exception.
 *
 * This function will request an exception to be processed based on
 * the provided source file information.
 *
 * An application must install an exception handler function via \ref
 * shr_exception_handler_set, and this function is responsible for
 * processing the exception information.
 *
 * The application function may also choose filter the exception
 * requests based on the provided source file information, for
 * example, it may choose to only process exceptions for a specific
 * function or source file.
 *
 * \param [in] unit Unit number.
 * \param [in] err_code Error code.
 * \param [in] file Source file name.
 * \param [in] line Source file line number.
 * \param [in] func Function name.
 */
extern void
shr_exception(int unit, int err_code,
              const char *file, int line, const char *func);

#endif /* SHR_EXCEPTION_H */
