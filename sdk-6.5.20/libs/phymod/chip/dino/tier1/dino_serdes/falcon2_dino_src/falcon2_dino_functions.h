/****************************************************************************
 * File Name  :  falcon2_dino_functions.h
 * Created On :  29/04/2013
 * Created By :  Kiran Divakar
 * Description:  Header file with API functions for Serdes IPs
 * Revision   :  $Id: falcon2_dino_functions.h 1282 2015-12-04 12:20:52Z eroes $
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * No portions of this material may be reproduced in any form without
 * the written permission of:
 *
 *     Broadcom Corporation
 *     5300 California Avenue
 *     Irvine, CA  92617
 *
 * All information contained in this document is Broadcom Corporation
 * company private proprietary, and trade secret.
 *//**
 * @file
 * Protoypes of all API functions for engineering use
 *//*************************************************************************/



#ifndef FALCON2_DINO_API_FUNCTIONS_H
#define FALCON2_DINO_API_FUNCTIONS_H

/* include all .h files, even though some are redundant */

#include "falcon2_dino_usr_includes.h"

#include "falcon2_dino_dependencies.h"
#include "falcon2_dino_interface.h"
#include "falcon2_dino_debug_functions.h"
#include "falcon2_dino_common.h"
#include "falcon2_dino_api_uc_common.h"
#include "../common/srds_api_uc_common.h"

#include "falcon2_dino_field_access.h"
#include "falcon2_dino_enum.h"
#include "../common/srds_api_err_code.h"




/**
 * Error-trapping macro.
 *
 * In other then SerDes-team post-silicon evaluation builds, simply yields
 * the error code supplied as an argument, without further action.
 */
#define falcon2_dino_error(err_code) falcon2_dino_print_err_msg(err_code)

/**@}*/



/************************************************************************//**
 * @name Error-Code Storage Addresses.
 *
 * Used by error-checking expression-wrapper macros.  Expands to the address
 * where the macros are meant to store the error codes on which they operate,
 * which depends on the target core and type of executable image being built.
 *
 * These are defined well ahead of the error-checking macros themselves to
 * facilitate definition of RAM- and register-access macros generally used
 * in their argyments.
 *//*************************************************************************/
/**@{*/


/**
 * Error-code storage address.
 *
 * This is a standard API build that directs error-checking expression-wrapper
 * macros to use a block-local error codes for efficient. local optimization.
 */
#define FALCON2_DINO_ERR &__err

/**@}*/



/************************************************************************//**
 * @name Register Access Macro Inclusions
 *
 * All cores provide access to hardware control/status registers.
 *//*************************************************************************/
/**@{*/






/**
 * This build includes register access macros for the FALCON2/DINO core.
 */
#include "falcon2_dino_fields.h"



/**@}*/



/************************************************************************//**
 * @name RAM Access Macro Inclusions
 *
 * Some cores also provide access to firmware control/status RAM variables.
 *//*************************************************************************/
/**@{*/

/**
 * This build includes macros to access Falcon and/or Falcon2 microcode RAM
 * variables.
 */
#include "falcon2_dino_api_uc_vars_rdwr_defns.h"

/**@}*/



/************************************************************************//**
 * @name Direct RAM Access
 *
 * Cores/chips with a built-in microcontroller afford direct, memory-mapped
 * access to the firmware control/status RAM variables.
 *//*************************************************************************/
/**@{*/

/** Base of core variable block, FALCON/FALCON2 variant. */
#define FALCON2_DINO_CORE_VAR_RAM_BASE (0x400)
/** Base of lane variable block, FALCON/FALCON2 variant. */
#define FALCON2_DINO_LANE_VAR_RAM_BASE (0x420)
/** Size of lane variable block, FALCON/FALCON2 variant. */
#define FALCON2_DINO_LANE_VAR_RAM_SIZE (0x130)
#define FALCON2_DINO_CORE_VAR_RAM_SIZE (0x40)

/**@}*/



/************************************************************************//**
 * @name Diagnostic Sampling
 *//*************************************************************************/
/**@{*/

#ifdef FALCON2_DINO_STANDALONE_EVENT
#define FALCON2_DINO_DIAG_MAX_SAMPLES (64)
#else
/**
 * Diagnostic sample set size, FALCON/FALCON2 variant.
 *
 * Applies to collections of BER measurements, eye margins, etc.
 */
#define FALCON2_DINO_DIAG_MAX_SAMPLES (64)

#endif

/**@}*/



/************************************************************************//**
 * @name Error-Checking Expression Wrappers
 *
 * These macros simplify checking and forwarding of error codes returned
 * either directly or indirectly in the context of functions that themselves
 * return error codes directly or indirectly.
 *
 * All expand to unterminated statements and dereference `__ERR' (defined as
 * a macro in the same header) to access either private, block-internal error
 * codes (`__err') or a common error-code cache (e.g. `global_err_code' in
 * SerDes team post-silicon evaluation builds).
 *
 * Neither `__err' nor `__ERR' should be used directly outside the API; and
 * their names may change to comply with the C Language standard reservation
 * of identifiers beginning with `__' for use by compiler implementers.
 *
 * Great care is taken to ensure not only that error returns are checked but
 * that use of an error-code cache (as in SerDes team post-silicon evaluation
 * builds) does not cause "unused variable" warnings.
 *//*************************************************************************/
/**@{*/

/**
 * Error-check a function call, returning error codes returned.
 *
 * Evaluates an expression (typically function call), stores its value into
 * `*(__ERR)' and returns it from a containing function if it is unequal to
 * `ERR_CODE_NONE'.
 *
 * EFUN() is intended for use in functions returning error codes directly to
 * check calls to functions also returning error codes directly, e.g.:
 *
 *     err_code_t foo(...) // remaining arguments elided
 *     {
 *         // ...
 *         EFUN(wrc_core_s_rstb(0x0));
 *         // ...
 *         return ERR_CODE_NONE;
 *     }
 */

#define FALCON2_DINO_EFUN(expr) \
    do  { \
        err_code_t __err; \
        *(FALCON2_DINO_ERR) = (expr); \
        if (*(FALCON2_DINO_ERR) != ERR_CODE_NONE) \
            return falcon2_dino_error(*(FALCON2_DINO_ERR)); \
        (void)__err; \
    }   while(0)

/**
 * Error-check a statement, returning error codes forwarded.
 *
 * Evaluates an expression (typically unterminated statement) that may modify
 * `*(__ERR)' and returns it from a containing function if it is unequal to
 * `ERR_CODE_NONE'.
 *
 * ESTM() is intended for use in functions returning error codes directly to
 * check calls to functions returning error codes indirectly, e.g.:
 *
 *     err_code_t foo(...) // remaining arguments elided
 *     {
 *         uint8_t rst;
 *         // ...
 *         ESTM(rst = rdc_core_s_rstb());
 *         // ...
 *         return ERR_CODE_NONE;
 *     }
 */

#define FALCON2_DINO_ESTM(expr) \
    do  { \
        err_code_t __err; \
        *(FALCON2_DINO_ERR) = ERR_CODE_NONE; \
        (expr); \
        if (*(FALCON2_DINO_ERR) != ERR_CODE_NONE) \
            return  falcon2_dino_error(*(FALCON2_DINO_ERR)); \
        (void)__err; \
    }   while(0)

/**
 * Error-check a function call, defaulting when forwarding error codes
 * returned.
 *
 * In a function taking an argument `err_code_t *err_code_p' in lieu of
 * returning an error code directly, evaluates an expression (typically
 * function call), stores its value into `*(__ERR)', combines this (bitwise
 * inclusive ore) into `*(err_code_p)', and returns a default value if either
 * `*(__ERR)' or `*(err_code_p)' is not `ERR_CODE_NONE'.
 *
 * EPFUN2() is intended for use in functions returning error codes indirectly
 * to check calls to functions returning error codes directly, e.g.:
 *
 *     uint8_t foo(err_code_t *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8_t result = 0x0;
 *         // ...
 *         EPFUN2(wrc_core_s_rstb(0x0), 0x1);
 *         // ...
 *         return result;
 *     }
 */

#define FALCON2_DINO_EPFUN2(expr, on_err) \
    do  { \
        err_code_t __err; \
        *(FALCON2_DINO_ERR) = (expr); \
        *(err_code_p) |= *(FALCON2_DINO_ERR); \
        if ((*(err_code_p) != ERR_CODE_NONE) \
         || (*(FALCON2_DINO_ERR)      != ERR_CODE_NONE)) \
             return (on_err); \
        (void)__err; \
    }   while(0)

/**
 * Error-check a statement, defaulting when forwarding error codes forwarded.
 *
 * In a function taking an argument `err_code_t *err_code_p' in lieu of
 * returning an error code directly, evaluates an expression (typically
 * unterminated statement), stores its value into `*(__ERR)', combines this
 * (bitwise inclusive ore) into `*(err_code_p)', and returns a default value
 * if either `*(__ERR)' or `*(err_code_p)' is not `ERR_CODE_NONE'.
 *
 * EPSTM2() is intended for use in functions returning error codes indirectly
 * to check calls to functions also returning error codes indirectly, e.g.:
 *
 *     uint8_t foo(err_code_t *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8_t result;
 *         // ...
 *         EPSTM(result = rdc_core_s_rstb(), 0x1);
 *         // ...
 *         return result;
 *     }
 */

#define FALCON2_DINO_EPSTM2(expr, on_err) \
    do  { \
        err_code_t __err; \
        *(FALCON2_DINO_ERR) = ERR_CODE_NONE; \
        (expr); \
        *(err_code_p) |= *(FALCON2_DINO_ERR); \
        if ((*(err_code_p )!= ERR_CODE_NONE) \
         || (*(FALCON2_DINO_ERR)      != ERR_CODE_NONE)) \
             return (on_err); \
        (void)__err; \
    }   while(0)

/**
 * Error-check a function call, defaulting to zero when forwarding error codes
 * returned.
 *
 * Supplies a default value of zero to EPFUN2() to reduce clutter in the most
 * common case.
 *
 * EPFUN() is intended for use in functions returning error codes indirectly
 * to check calls to functions returning error codes directly, e.g.:
 *
 *     uint8_t foo(err_code_t *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8_t result;  // determined below, detail elided
 *         // ...
 *         EPFUN(wrc_core_s_rstb(0x0));
 *         // ...
 *         return result;
 *     }
 */

#define FALCON2_DINO_EPFUN(expr) FALCON2_DINO_EPFUN2((expr), 0)

/**
 * Error-check a statement, defaulting to zero when forwarding error codes
 * forwarded.
 *
 * Supplies a default value of zero to EPSTM2() to reduce clutter in the most
 * common case.
 *
 * EPSTM() is intended for use in functions returning error codes indirectly
 * to check calls to functions also returning error codes indirectly, e.g.:
 *
 *     uint8_t foo(err_code_t *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8_t result;
 *         // ...
 *         EPSTM(result = rdc_core_s_rstb());
 *         // ...
 *         return result;
 *     }
 */

#define FALCON2_DINO_EPSTM(expr) FALCON2_DINO_EPSTM2((expr), 0)

/**
 * Invoke a function with automatic return of error on NULL result.
 *
 * ENULL() is intended for use in functions returning error codes directly to
 * check calls to functions returning pointers, e.g.:
 *
 *     err_code_t foo(...) // remaining arguments elided
 *     {
 *         // ...
 *         ENULL(strchr("foo", 'q'));
 *         // ...
 *         return ERR_CODE_NONE;
 *     }
 */
#define FALCON2_DINO_ENULL(expr) \
  FALCON2_DINO_EFUN((((void *)0!=(expr))?ERR_CODE_NONE:ERR_CODE_BAD_PTR_OR_INVALID_INPUT))

/**
 * Invoke a function with automatic forward of error on NULL result.
 *
 * EPNULL() is intended for use in functions returning error codes indirectly
 * to check calls to functions returning pointers, e.g.:
 *
 *     uint8_t foo(err_code_t *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8_t result;  // determined below, detail elided
 *         // ...
 *         EPNULL(strchr(foo, 'q'));
 *         // ...
 *         return result;
 *     }
 */
#define FALCON2_DINO_EPNULL(expr) \
  FALCON2_DINO_EPFUN((((void *)0!=(expr))?ERR_CODE_NONE:ERR_CODE_BAD_PTR_OR_INVALID_INPUT))

/**
 * Invoke USR_PRINTF(()) with non-error-code-generating arguments.
 *
 * Note that the single argument is a parenthesized argument list to be
 * passed to USR_PRINTF(()).
 *
 * EFUN_PRINTF(()) is intended for use in functions returning error codes
 * directly, with an argument list the elements of which do not generate
 * error codes of any kind, e.g.:
 *
 *     err_code_t foo(...) // remaining arguments elided
 *     {
 *         // ...
 *         EFUN_PRINTF(("%u", 1));
 *         // ...
 *         return ERR_CODE_NONE;
 *     }
 */
#define FALCON2_DINO_EFUN_PRINTF(paren_arg_list) USR_PRINTF(paren_arg_list)

/**
 * Invoke USR_PRINTF(()) with error-code-generating arguments that would
 * otherwise be handled by ESTM().
 *
 * Note that the single argument is a parenthesized argument list to be
 * passed to USR_PRINTF(()).
 *
 * EFUN_PRINTF(()) is intended for use in functions returning error codes
 * directly, with an argument list the elements of which may generate error
 * codes indirectly, e.g.:
 *
 *     err_code_t foo(...) // remaining arguments elided
 *     {
 *         // ...
 *         ESTM_PRINTF(("%u", rdc_core_s_rstb()));
 *         // ...
 *         return ERR_CODE_NONE;
 *     }
 */
#define FALCON2_DINO_ESTM_PRINTF(paren_arg_list) \
    do  { \
        err_code_t __err; \
        *(FALCON2_DINO_ERR) = ERR_CODE_NONE; \
        USR_PRINTF(paren_arg_list); \
        if (*(FALCON2_DINO_ERR) != ERR_CODE_NONE) \
            return  falcon2_dino_error(*(FALCON2_DINO_ERR)); \
        (void)__err; \
    }   while(0)

/**
 * Invoke possibly-remapped 'memset()' and, if it returns NULL, force an
 * error return.
 *
 * Ordinarily, standard implementations of 'memset' will return NULL only if
 * passed a NULL destination address, and *may already* have overwritten an
 * inappropriate address range before returning:  nevertheless, a specialized
 * implementation could use a NULL return to indicate other failures.  In
 * either case, execution should not be allowed to proceed on NULL return.
 */
#define FALCON2_DINO_ENULL_MEMSET(mem, val, num) FALCON2_DINO_ENULL((USR_MEMSET((mem), (val), (num))))

/**
 * Invoke possibly-remapped 'memset()' and, if it returns NULL, force an
 * error to be forwarded.
 *
 * Ordinarily, standard implementations of 'memset' will return NULL only if
 * passed a NULL destination address, and *may already* have overwritten an
 * inappropriate address range before returning:  nevertheless, a specialized
 * implementation could use a NULL return to indicate other failures.  In
 * either case, execution should not be allowed to proceed on NULL return.
 */
#define FALCON2_DINO_EPNULL_MEMSET(mem, val, num) FALCON2_DINO_EPNULL((USR_MEMSET((mem), (val), (num))))

/**
 * Invoke possibly-remapped 'strcpy()' and, if it returns NULL, force an
 * error return.
 *
 * Ordinarily, standard implementations of 'strcpy' will return NULL only if
 * passed a NULL destination address, and *may already* have overwritten an
 * inappropriate address range before returning:  nevertheless, a specialized
 * implementation could use a NULL return to indicate other failures.  In
 * either case, execution should not be allowed to proceed on NULL return.
 */
#define FALCON2_DINO_ENULL_STRCPY(dst, src) FALCON2_DINO_ENULL((USR_STRCPY((dst), (src))))
#define FALCON2_DINO_ENULL_STRCAT(dst, src) FALCON2_DINO_ENULL((USR_STRCAT((dst), (src))))

/**
 * Invoke possibly-remapped 'strcpy()' and, if it returns NULL, force an
 * error to be forwarded.
 *
 * Ordinarily, standard implementations of 'strcpy' will return NULL only if
 * passed a NULL destination address, and *may already* have overwritten an
 * inappropriate address range before returning:  nevertheless, a specialized
 * implementation could use a NULL return to indicate other failures.  In
 * either case, execution should not be allowed to proceed on NULL return.
 */
#define FALCON2_DINO_EPNULL_STRCPY(dst, src) FALCON2_DINO_EPNULL((USR_STRCPY((dst), (src))))

/**
 * Invoke possibly-remapped 'strcpy()' and, if it returns NULL, force an
 * error return.
 *
 * Ordinarily, standard implementations of 'strncpy' will return NULL only if
 * passed a NULL destination address, and *may already* have overwritten an
 * inappropriate address range before returning:  nevertheless, a specialized
 * implementation could use a NULL return to indicate other failures.  In
 * either case, execution should not be allowed to proceed on NULL return.
 */
#define FALCON2_DINO_ENULL_STRNCAT(dst, src) FALCON2_DINO_ENULL((USR_STRNCAT((dst), (src))))

/**
 * Invoke possibly-remapped 'strcpy()' and, if it returns NULL, force an
 * error to be forwarded.
 *
 * Ordinarily, standard implementations of 'strncpy' will return NULL only if
 * passed a NULL destination address, and *may already* have overwritten an
 * inappropriate address range before returning:  nevertheless, a specialized
 * implementation could use a NULL return to indicate other failures.  In
 * either case, execution should not be allowed to proceed on NULL return.
 */
#define FALCON2_DINO_EPNULL_STRNCAT(dst, src) FALCON2_DINO_EPNULL((USR_STRNCAT((dst), (src))))

/**@}*/

/************************************************************************//**
 * @name Display Utility Macros
 *//*************************************************************************/
/**@{*/

/** Display a signed integer variable. */
#define FALCON2_DINO_DISP(x) FALCON2_DINO_ESTM_PRINTF(("%s = %d\n", #x, x))

/** Display an unsigned integer variable. */
#define FALCON2_DINO_DISPU(x) FALCON2_DINO_ESTM_PRINTF(("%s = %u\n", #x, x))

/** Display a floating point variable. */
#define FALCON2_DINO_DISPF(x) FALCON2_DINO_ESTM_PRINTF(("%s = %f\n", #x, x))

/** Display an integer variable in hex. */
#define FALCON2_DINO_DISPX(x) FALCON2_DINO_ESTM_PRINTF(("%s = %x\n", #x, x))

/** Read and display the value of a lane register field in decimal. */
#define FALCON2_DINO_DISP_REG(x) FALCON2_DINO_ESTM_PRINTF(("%s = %d\n", #x, rd_##x##()))

/** Read and display the value of a lane register field in hex. */
#define FALCON2_DINO_DISP_REGX(x) FALCON2_DINO_ESTM_PRINTF(("%s = %x\n", #x, rd_##x##()))

/** Read and display the value of a core register field in hex. */
#define FALCON2_DINO_DISP_REGC(x) FALCON2_DINO_ESTM_PRINTF(("%s = %x\n", #x, rdc_##x##()))

/** Display a single member of a lane struct. */
#define FALCON2_DINO_DISP_LN_VARS(name,param,format) \
    do { \
        FALCON2_DINO_ESTM_PRINTF(("%-16s\t",name)); \
        for(i=0;i<num_lanes;i++) { \
            FALCON2_DINO_ESTM_PRINTF((format,(lane_st[i].param))); \
        } \
        FALCON2_DINO_EFUN_PRINTF(("\n"));    \
    }   while (0)

/** Display two members of a lane struct. */
#define FALCON2_DINO_DISP_LNQ_VARS(name,param1,param2,format) \
    do { \
        FALCON2_DINO_ESTM_PRINTF(("%-16s\t",name)); \
        for(i=0;i<num_lanes;i++) { \
            FALCON2_DINO_ESTM_PRINTF((format,(lane_st[i].param1),(lane_st[i].param2))); \
        } \
        FALCON2_DINO_EFUN_PRINTF(("\n"));    \
    }   while (0)

/**@}*/

/************************************************************************//**
 * @name Arithmetic Utility Macros
 *//*************************************************************************/
/**@{*/

/**
 * Clockwise difference between phase counters.
 */
#define falcon2_dino_dist_cw(a,b) (((a)<=(b))?((b)-(a)):((uint16_t)256-(a)+(b)))

/**
 * Counter-clockwise difference between phase counters
 */
#define falcon2_dino_dist_ccw(a,b) (((a)>=(b))?((a)-(b)):((uint16_t)256+(a)-(b)))

/**
 * Lesser of two expressions.
 *
 * @warning
 *
 *    May evaluate the selected expression twice.
 */
#define falcon2_dino__min(a,b) (((a)>(b)) ? (b) : (a))

/**
 * Greater of two expressions.
 *
 * @warning
 *
 *    May evaluate the selected expression twice.
 */
#define falcon2_dino__max(a,b) (((a)>(b)) ? (a) : (b))

/**
 * Absolute value of an expression.
 *
 * @warning
 *
 *    May evaluate the given expression twice.
 */
#define falcon2_dino_abs(a) (((a)>0) ? (a) : (-(a)))

/**@}*/
#endif
