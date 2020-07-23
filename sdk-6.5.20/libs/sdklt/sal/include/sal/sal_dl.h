/*! \file sal_dl.h
 *
 * Dynamic loader API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_DL_H
#define SAL_DL_H

/*! Opaque shared library handle. */
typedef void *sal_dl_t;

/*!
 * \brief Load shared library object.
 *
 * This function loads a shared library object (file) and returns an
 * opaque handle for the loaded object.
 *
 * \param [in] libname Name of the shared library object.
 *
 * \return Handle to the shared library or NULL on error.
 */
extern sal_dl_t
sal_dl_open(const char *libname);

/*!
 * \brief Get symbol address from a shared library object.
 *
 * The shared library object is referenced by the handle returned by
 * \ref sal_dl_open.
 *
 * The function returns NULL if the library handle is invalid or the
 * symbol is not found.
 *
 * \param [in] dlh Handle to the shared library object.
 * \param [in] symname Symbol name (data or function).
 *
 * \return Pointer to the requested symbol or NULL on error.
 */
extern void *
sal_dl_sym(sal_dl_t dlh, const char *symname);

/*!
 * \brief Unload shared library object.
 *
 * Unload previously loaded shared library object or reduce the
 * reference count if the shared library was loaded by more then one
 * process.
 *
 * \param [in] dlh Handle to the shared library object.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
extern int
sal_dl_close(sal_dl_t dlh);

#endif /* SAL_DL_H */
