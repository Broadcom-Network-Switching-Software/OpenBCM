/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BLMI_ERR_H
#define BLMI_ERR_H

typedef enum {
    BLMI_E_NONE		=  0,
    BLMI_E_INTERNAL		= -1,
    BLMI_E_MEMORY		= -2,
    BLMI_E_PARAM		= -3,
    BLMI_E_EMPTY             = -4,
    BLMI_E_FULL		= -5,
    BLMI_E_NOT_FOUND		= -6,
    BLMI_E_EXISTS		= -7,
    BLMI_E_TIMEOUT		= -8,
    BLMI_E_FAIL		= -9, 
    BLMI_E_DISABLED		= -10,
    BLMI_E_BADID		= -11,
    BLMI_E_RESOURCE		= -12,
    BLMI_E_CONFIG		= -13,
    BLMI_E_UNAVAIL		= -14,
    BLMI_E_INIT		= -15,
    BLMI_E_PORT		= -16,
    BLMI_E_UNKNOWN           = -17
} blmi_error_t;

#define	BLMI_ERRMSG_INIT	{ \
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

extern char *blmi_errmsg[];

#define	BLMI_ERRMSG(r)		\
	blmi_errmsg[((r) <= 0 && (r) > BLMI_E_UNKNOWN) ? -(r) : -BLMI_E_UNKNOWN]

#define BLMI_E_SUCCESS(rv)		((rv) >= 0)
#define BLMI_E_FAILURE(rv)		((rv) < 0)

/*
 * Macro:
 *	BLMI_E_IF_ERROR_RETURN
 * Purpose:
 *	Evaluate _op as an expression, and if an error, return.
 * Notes:
 *	This macro uses a do-while construct to maintain expected
 *	"C" blocking, and evaluates "op" ONLY ONCE so it may be
 *	a function call that has side affects.
 */

#define BLMI_E_IF_ERROR_RETURN(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) return(__rv__); } while(0)

#define BLMI_E_IF_ERROR_NOT_UNAVAIL_RETURN(op)                        \
    do {                                                                \
        int __rv__;                                                     \
        if (((__rv__ = (op)) < 0) && (__rv__ != BLMI_E_UNAVAIL)) {      \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

#endif /* BLMI_ERR_H */


