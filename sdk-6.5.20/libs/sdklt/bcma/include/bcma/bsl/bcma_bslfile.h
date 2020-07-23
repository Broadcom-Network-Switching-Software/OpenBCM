/*! \file bcma_bslfile.h
 *
 * Log file sink.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLFILE_H
#define BCMA_BSLFILE_H

/*!
 * \brief Initialize log file sink.
 *
 * \retval 0 No errors
 */
int
bcma_bslfile_init(void);

/*!
 * \brief Get current log file name.
 *
 * \return Pointer to log file name.
 */
char *
bcma_bslfile_name(void);

/*!
 * \brief Close log file.
 *
 * \retval 0 No errors
 */
int
bcma_bslfile_close(void);

/*!
 * \brief Open log file.
 *
 * If \c append is non-zero, then log messages will be appended to the
 * existing log file, otherwise any existing log file will be deleted
 * first.
 *
 * If the log file does not exist, it will be created.
 *
 * \param [in] filename Log file name.
 * \param [in] append Set to non-zero to append to existing log file.
 *
 * \retval 0 No errors.
 */
int
bcma_bslfile_open(char *filename, int append);

/*!
 * \brief Check if log file is enabled.
 *
 * Log messages will be dropped unconditionally if the log file is not
 * enabled.
 *
 * \retval 1 Log file is enabled.
 * \retval 0 Log file is not enabled.
 */
int
bcma_bslfile_is_enabled(void);

/*!
 * \brief Enable or disable log file output.
 *
 * \param [in] enable Set to 0 to disable log file output.
 *
 * \return Previous enable value.
 */
int
bcma_bslfile_enable(int enable);

#endif /* BCMA_BSLFILE_H */
