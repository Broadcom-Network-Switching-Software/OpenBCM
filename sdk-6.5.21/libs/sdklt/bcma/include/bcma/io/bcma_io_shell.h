/*! \file bcma_io_shell.h
 *
 * Shell commands (if supported by Operating system).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_IO_SHELL_H
#define BCMA_IO_SHELL_H

/*!
 * \brief Start system shell or execute system command.
 *
 * \param [in] cmd System command name (launch system shell if NULL).
 * \param [in] ifile Input file (stdin by default).
 * \param [in] ofile Output file (stdout by default).
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
int
bcma_io_shell(const char *cmd, const char *ifile, const char *ofile);

#endif /* BCMA_IO_SHELL_H */
