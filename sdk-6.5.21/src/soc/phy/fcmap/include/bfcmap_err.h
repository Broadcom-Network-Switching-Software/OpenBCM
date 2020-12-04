/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFCMAP_ERR_H
#define BFCMAP_ERR_H

typedef enum {
    BFCMAP_E_NONE		=  0,
    BFCMAP_E_INTERNAL	= -1,
    BFCMAP_E_MEMORY		= -2,
    BFCMAP_E_PARAM		= -3,
    BFCMAP_E_EMPTY          = -4,
    BFCMAP_E_FULL		= -5,
    BFCMAP_E_NOT_FOUND		= -6,
    BFCMAP_E_EXISTS		= -7,
    BFCMAP_E_TIMEOUT		= -8,
    BFCMAP_E_FAIL		= -9, 
    BFCMAP_E_DISABLED		= -10,
    BFCMAP_E_BADID		= -11,
    BFCMAP_E_RESOURCE		= -12,
    BFCMAP_E_CONFIG		= -13,
    BFCMAP_E_UNAVAIL		= -14,
    BFCMAP_E_INIT		= -15,
    BFCMAP_E_PORT		= -16,
    BFCMAP_E_UNKNOWN           = -17
} bfcmap_error_t;

#define	BFCMAP_ERRMSG_INIT	{ \
	"Ok",				/* E_NONE */ \
	"Internal error",		/* E_INTERNAL */ \
	"Out of memory",		/* E_MEMORY */ \
	"Invalid parameter",		/* E_PARAM */ \
	"Table empty",			/* E_EMPTY */ \
	"Table full",			/* E_FULL */ \
	"Entry not found",		/* E_NOT_FOUND */ \
	"Entry exists",			/* E_EXISTS */ \
	"Operation timed out",		/* E_TIMEOUT */ \
	"Operation failed",		/* E_FAIL */ \
	"Operation disabled",		/* E_DISABLED */ \
	"Invalid identifier",		/* E_BADID */ \
	"No resources for operation",	/* E_RESOURCE */ \
	"Invalid configuration",	/* E_CONFIG */ \
	"Feature unavailable",		/* E_UNAVAIL */ \
	"Feature not initialized",	/* E_INIT */ \
	"Invalid port",			/* E_PORT */ \
	"Unknown error",		/* E_UNKNOWN*/ \
	}

extern char *bfcmap_errmsg[];

#define	BFCMAP_ERRMSG(r)		\
	bfcmap_errmsg[((r) <= 0 && (r) > BFCMAP_E_UNKNOWN) ? -(r) : -BFCMAP_E_UNKNOWN]

#define BFCMAP_E_SUCCESS(rv)		((rv) >= 0)
#define BFCMAP_E_FAILURE(rv)		((rv) < 0)

/*
 * Macro:
 *	BFCMAP_E_IF_ERROR_RETURN
 * Purpose:
 *	Evaluate _op as an expression, and if an error, return.
 * Notes:
 *	This macro uses a do-while construct to maintain expected
 *	"C" blocking, and evaluates "op" ONLY ONCE so it may be
 *	a function call that has side affects.
 */

#define BFCMAP_E_IF_ERROR_RETURN(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) return(__rv__); } while(0)

#define BFCMAP_E_IF_ERROR_NOT_UNAVAIL_RETURN(op)                        \
    do {                                                                \
        int __rv__;                                                     \
        if (((__rv__ = (op)) < 0) && (__rv__ != BFCMAP_E_UNAVAIL)) {      \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

#endif /* BFCMAP_ERR_H */


