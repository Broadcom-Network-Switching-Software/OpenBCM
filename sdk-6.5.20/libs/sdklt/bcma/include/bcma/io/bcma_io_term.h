/*! \file bcma_io_term.h
 *
 * System-dependent functions for terminal I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_IO_TERM_H
#define BCMA_IO_TERM_H

#include <stdarg.h>

/*!
 * \brief Print formatted string to terminal.
 *
 * This function is the default output function for the BSL console
 * sink. All SDK output is done via the BSL logging system for which
 * one or more output sinks can be configured.
 *
 * \param [in] fmt Format string (vprintf-compatible).
 * \param [in] varg Argument list (vprintf-compatible).
 *
 * \return Number fof characters written or negative value on error.
 */
extern int
bcma_io_term_vprintf(const char *fmt, va_list varg);

/*!
 * \brief Read characters from terminal.
 *
 * The function must return either when the read buffer is full or the
 * command line is complete (usually indicated by the user pressing
 * the Enter key).
 *
 * \param [in] buf Buffer for characters read.
 * \param [in] max Size of read buffer.
 *
 * \return Number of bytes read or -1 if error.
 */
extern int
bcma_io_term_read(void *buf, int max);

/*!
 * \brief Write characters to terminal.
 *
 * \param [in] buf Buffer with characters to write.
 * \param [in] count Number of characters to write.
 *
 * \return Number of bytes written or -1 if error.
 */
extern int
bcma_io_term_write(const void *buf, int count);

/*!
 * \brief Read a line from terminal.

 * \param [in] buf Buffer for line read.
 * \param [in] max Size of read buffer.
 *
 * \return \c buf on success, and NULL on error or EOF.
 */
extern char *
bcma_io_term_gets(void *buf, int max);

/*!
 * \brief Prepare terminal for for command line editing.
 *
 * Thie first time called, this function must save the current
 * terminal mode, and if/when the function is called subsequently with
 * reset=1, the original terminal settings must be restored.
 *
 * If reset=0, the terminal must be configured for readline-style
 * command line editing by disabling local echo and buffering
 * (line-at-a-time mode).
 *
 * \param [in] reset Restore original terminal settings.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to set terminal mode.
 */
extern int
bcma_io_term_mode_set(int reset);

/*!
 * \brief Get terminal window dimensions.
 *
 * \param [out] cols Number of columns (characters per line).
 * \param [out] rows Number of rows (lines).
 *
 * \retval 0 No errors.
 * \retval -1 Unable to retrieve terminal data.
 */
extern int
bcma_io_term_winsize_get(int *cols, int *rows);

/*!
 * \brief Check whether stdin refers to a terminal.
 *
 * \return 1 if stdin is referring to a terminal, otherwise 0.
 */
extern int
bcma_io_term_is_tty(void);

#endif /* BCMA_IO_TERM_H */
