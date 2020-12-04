/*! \file bcma_clicmd_rccache.h
 *
 * CLI 'rccache' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_RCCACHE_H
#define BCMA_CLICMD_RCCACHE_H

#include <bcma/io/bcma_io_file.h>

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_RCCACHE_DESC \
    "Save contents of an rc file in memory."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_RCCACHE_SYNOP \
    "show [<File>]  - show cached rc files or contents of 'File'\n" \
    "add <File>     - add rc file 'File' to cached files\n" \
    "addq <File>    - like add 'File' but only if not already cached\n" \
    "update <File>  - reload rc file 'File' to the cache\n" \
    "delete <File>  - delete cached rc file 'File'\n" \
    "clear          - delete all cached rc files"

/*!
 * \brief CLI 'rccache' command implementation.
 *
 * 'rccache' command is mainly to load file content into memory.
 * This command usually collocates with 'rcload' command.
 * If we need to run a script file several times, we can cache the file
 * to memory by 'rccache' command first, then running the script file by
 * 'rcload'. It would then reduce the file I/O times if we cached the file
 * first instead of loading the file every time.
 *
 * All resources allocated by 'rccache' command should be freed by calling
 * \ref bcma_clicmd_rccache_cleanup function.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_rccache(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clear all cached content of files in memory
 *
 * This function will clear all cached content of files in memory if any.
 * If the cached file is locked (in execution by rcload), the cached data
 * will still be freed by this function.
 *
 * \param [in] cli CLI object.
 *
 * \return Always 0.
 */
extern int
bcma_clicmd_rccache_cleanup(bcma_cli_t *cli);

/*!
 * \brief Lock and get cached file content.
 *
 * This function is used to get the file content in memory cached by
 * CLI command 'rccache add'. User needs to call \ref
 * bcma_clicmd_rccache_unlock after finishing the use of the cached file.
 * From the file 'filename' is used to call bcma_clicmd_rccache_lock and till
 * \ref bcma_clicmd_rccache_unlock, we call the file 'filename' is locked in
 * cache.
 * During the time that cached file is locked, it would then fail to remove
 * the file cache by 'rccache delete' command or update the file cache by
 * 'rccache update' command. But we can still force to clear the cache
 * by 'rccache clear'.
 *
 * Return value is NULL if 'filename' is not in cache.
 *
 * \param [in] filename Filename of the cached content.
 *
 * \return Pointer to file cached content.
 * \return NULL if file is not cached.
 */
extern const char *
bcma_clicmd_rccache_lock(const char *filename);

/*!
 * \brief Unlock cached file content.
 *
 * This function should be used in pair with Non-NULL return of function
 * \ref bcma_clicmd_rccache_lock, to release the lock in \ref
 * bcma_clicmd_rccache_lock function.
 *
 * \param [in] filename Filename of the cached content.
 *
 * \return 0 if cache is unlocked successfully.
 * \return -1 missing file in cache or invalid lock status.
 */
extern int
bcma_clicmd_rccache_unlock(const char *filename);

/*!
 * \brief Open a script file.
 *
 * This function will first try to open \c filename if the \c filename
 * string contains path information (slash or colon). If this fails,
 * the function will then try to open \c filename from every path
 * listed in the environment variable \ref BCMA_CLI_VAR_PATH until the
 * file is successfully opened or the path list is exhausted.
 *
 * \param [in] cli CLI object.
 * \param [in] filename Name of script file.
 *
 * \return File handle of the script file. NULL if file not found.
 */
extern bcma_io_file_handle_t
bcma_clicmd_rccache_file_open(bcma_cli_t *cli, const char *filename);

#endif /* BCMA_CLICMD_RCCACHE_H */
