/*! \file bcma_io_file.h
 *
 * File I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_IO_FILE_H
#define BCMA_IO_FILE_H

#include <stdarg.h>

/*! Opaque file handle */
typedef void *bcma_io_file_handle_t;

/*!
 * \brief Open a file.
 *
 * Open a file with name 'file' and mode 'mode'.
 *
 * The supported modes strings are shown below.
 *
 * Mode | Description
 * -----|------------
 * r    | Open text file for reading.
 * r+   | Open for reading and writing.
 * w    | Truncate file to zero length or create text file for writing.
 * w+   | Open for reading and writing. The file is created if it does not exist, otherwise it is truncated.
 * a    | Open for appending (writing at end of file). The file is created if it does not exist.
 * a+   | Open for reading and appending (writing at end of file). The file is created if it does not exist.
 *
 * \param [in] filename Name of file to open.
 * \param [in] mode File mode.
 *
 * \return NULL or the opened file handle.
 */
extern bcma_io_file_handle_t
bcma_io_file_open(const char *filename, const char *mode);

/*!
 * \brief Close a file.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 *
 * \return non-zero on error.
 */
extern int
bcma_io_file_close(bcma_io_file_handle_t fh);

/*!
 * \brief Read data from a file.
 *
 * Read 'num' elements of data, each 'size' bytes long from a file.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 * \param [out] buf Output buffer contains data read.
 * \param [in] size Size of each element of data to read.
 * \param [in] num Number of elements of data to read.
 *
 * \return the number of elements successfully read, or -1 on error.
 */
extern int
bcma_io_file_read(bcma_io_file_handle_t fh, void *buf, int size, int num);

/*!
 * \brief Write data to a file.
 *
 * Write 'num' elements of data, each 'size' bytes long to a file.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 * \param [in] buf Buffer contains data to write.
 * \param [in] size Size of each element of data to write.
 * \param [in] num Number of elements of data to write.
 *
 * \return the number of elements successfully write, or -1 on error.
 */
extern int
bcma_io_file_write(bcma_io_file_handle_t fh, const void *buf, int size, int num);

/*!
 * \brief Read string from a file.
 *
 * Read in at most one less than 'size' characters from a file and
 * stores them into the buffer. Reading stops after an EOF or a
 * newline character.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 * \param [out] buf Output buffer contains data read.
 * \param [in] size Size of maximum read characters.
 *
 * \return 'buf' on success, and NULL on error or EOF.
 */
extern char *
bcma_io_file_gets(bcma_io_file_handle_t fh, char *buf, int size);

/*!
 * \brief Read character from a file.
 *
 * Read the next character from a file and return it.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 *
 * \return the character read, or EOF on end-of-file or error.
 */
extern int
bcma_io_file_getc(bcma_io_file_handle_t fh);

/*!
 * \brief Write string to a file.
 *
 * Write string 'buf' to a file without its trailing '\0'.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 * \param [in] buf String to write.
 *
 * \return Non-negative on success.
 */
extern int
bcma_io_file_puts(bcma_io_file_handle_t fh, const char *buf);

/*!
 * \brief Write character to a file.
 *
 * Write the character 'c' to a file.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 * \param [in] c Character to write.
 *
 * \return The character written on success.
 */
extern int
bcma_io_file_putc(bcma_io_file_handle_t fh, int c);

/*!
 * \brief Flush a file.
 *
 * The function forces a write of all user-space buffered-data to a
 * file.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 *
 * \return 0 on success.
 */
extern int
bcma_io_file_flush(bcma_io_file_handle_t fh);

/*!
 * \brief Print formatted string to a file.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 * \param [in] format Format string.
 *
 * \return Number fof characters written or negative value on error.
 */
extern int
bcma_io_file_printf(bcma_io_file_handle_t fh, const char *format, ...);

/*!
 * \brief Print formatted string to a file.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 * \param [in] format Format string (vfprintf-compatible).
 * \param [in] ap Argument list (vfprintf-compatible).
 *
 * \return Number of characters written or negative value on error.
 */
extern int
bcma_io_file_vprintf(bcma_io_file_handle_t fh, const char *format, va_list ap);

/*!
 * \brief Get file size.
 *
 * The file position will be set to the beginning of file after this
 * function has been called.
 *
 * \param [in] fh File handle returned from \ref bcma_io_file_open.
 *
 * \return File size or -1 on error.
 */
extern int
bcma_io_file_size(bcma_io_file_handle_t fh);

/*!
 * \brief Delete a file from the filesystem.
 *
 * \param [in] filename File name to be deleted.
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_file_delete(const char *filename);

/*!
 * \brief Rename a file.
 *
 * \param [in] old_filename Old file name to be renamed from.
 * \param [in] new_filename New file name to be renamed to.
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_file_move(const char *old_filename, const char *new_filename);

/*!
 * \brief Create unique temporary file name.
 *
 * Creates a unique file name from a template string where the ending
 * characters must be "XXXXXX", e.g. "my.tempfile.XXXXXX".
 *
 * \param [in] template Template file name.
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_file_mktemp(char *template);

#endif /* BCMA_IO_FILE_H */
