/*! \file bcma_io_dir.h
 *
 * Functions related to directory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_IO_DIR_H
#define BCMA_IO_DIR_H

/*! Opaque directory handle */
typedef void *bcma_io_dir_handle_t;

/*!
 * \brief Get current working directory.
 *
 * The retrieved directory through this function is an absolute pathname
 * of current working directory with trailing slash included.
 *
 * \param [in] buf Buffer to get current working directory
 * \param [in] size Size of Buffer
 *
 * \return \c buf on success or NULL on failure.
 */
extern char *
bcma_io_dir_pwd(char *buf, int size);

/*!
 * \brief Change working directory.
 *
 * Change the current working directory to \c path. If the specified \c path
 * is NULL, the working directory will be changed to home directory.
 * If home directory is not able to be retrieved, the working directory
 * will then be changed to the root directory.
 *
 * \param [in] path The target directory path to be changed to
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_dir_cd(const char *path);

/*!
 * \brief Create a directory.
 *
 * \param [in] path Directory to be created
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_dir_mkdir(const char *path);

/*!
 * \brief Delete a directory.
 *
 * The directory to be deleted must be empty.
 *
 * \param [in] path Directory to be deleted
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_dir_rmdir(const char *path);

/*!
 * \brief List directory contents.
 *
 * The \c bcma_io_dir_ls function is implemented by invoking a platform "ls"
 * command. The supported \c flags are platform specfic. For example,
 * \c flags might be "-la" in POSIX system to list directory contents in
 * long format with hidden files included. Leave \c flags to NULL if no
 * special options for listing directory contents are required.
 *
 * \param [in] name Name of direcotry or file to be list
 * \param [in] flags Options for list directory contents.
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_dir_ls(const char *name, const char *flags);

/*!
 * \brief Open a directory.
 *
 * \param [in] path Path of directory to open.
 *
 * \return NULL or the opened directory handle.
 */
extern bcma_io_dir_handle_t
bcma_io_dir_open(const char *path);

/*!
 * \brief Close a directory..
 *
 * \param [in] dh Directory handle returned from \ref bcma_io_dir_open.
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_dir_close(bcma_io_dir_handle_t dh);

/*!
 * \brief Read a directory entry name.
 *
 * Read the next entry name in the directory handler.
 *
 * \param [in] dh Directory handle returned from \ref bcma_io_dir_open.
 * \param [in] sz The size of the output name buffer.
 * \param [out] name Name buffer.
 *
 * \return 0 on success or -1 on error.
 */
extern int
bcma_io_dir_read(bcma_io_dir_handle_t dh, int sz, char *name);

#endif /* BCMA_IO_DIR_H */
