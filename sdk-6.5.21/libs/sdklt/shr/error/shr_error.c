/*! \file shr_error.c
 *
 * Error message support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>

/*!
 * \brief Standard SDK error message.
 *
 * IMPORTANT: These error messages must match the corresponding error
 * codes in shr_error.h.
 */
static char *errmsg[] = {
    "Ok",                               /* E_NONE */
    "Internal error",                   /* E_INTERNAL */
    "Out of memory",                    /* E_MEMORY */
    "Invalid unit",                     /* E_UNIT */
    "Invalid parameter",                /* E_PARAM */
    "Table empty",                      /* E_EMPTY */
    "Table full",                       /* E_FULL */
    "Entry not found",                  /* E_NOT_FOUND */
    "Entry exists",                     /* E_EXISTS */
    "Operation timed out",              /* E_TIMEOUT */
    "Operation still running",          /* E_BUSY */
    "Operation failed",                 /* E_FAIL */
    "Operation disabled",               /* E_DISABLED */
    "Invalid identifier",               /* E_BADID */
    "No resources for operation",       /* E_RESOURCE */
    "Invalid configuration",            /* E_CONFIG */
    "Feature unavailable",              /* E_UNAVAIL */
    "Feature not initialized",          /* E_INIT */
    "Invalid port",                     /* E_PORT */
    "I/O error",                        /* E_IO */
    "Access error",                     /* E_ACCESS */
    "No handler",                       /* E_NO_HANDLER */
    "Partial success",                  /* E_PARTIAL */
    "Hash collision",                   /* E_COLL */

    "Unknown error"                     /* E_LIMIT */
};

const char *
shr_errmsg(int rv)
{
    int idx = -SHR_E_LIMIT;

    if (rv <= 0 && rv > SHR_E_LIMIT) {
        idx = -rv;
    }
    return errmsg[idx];
}
