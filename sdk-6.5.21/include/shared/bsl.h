/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log (bSL)
 *
 * Defines for INTERNAL usage only.
 */

#ifndef _SHR_BSL_H_
#define _SHR_BSL_H_

#include <shared/bsltypes.h>

#ifndef BSL_FILE
#define BSL_FILE    __FILE__
#endif

#ifndef BSL_LINE
#define BSL_LINE    __LINE__
#endif

#ifndef BSL_FUNC
#define BSL_FUNC    FUNCTION_NAME()
#endif


#define bslSinkIgnore   0

/* Message macro which includes the standard meta data */
#define BSL_META(str_) \
    "<c=%uf=%sl=%dF=%s>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC

/* Message macros which include optional meta data */
#define BSL_META_U(u_, str_) \
    "<c=%uf=%sl=%dF=%su=%d>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, u_
#define BSL_META_UP(u_, p_, str_) \
    "<c=%uf=%sl=%dF=%su=%dp=%d>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, u_, p_
#define BSL_META_UPX(u_, p_, x_, str_) \
    "<c=%uf=%sl=%dF=%su=%dp=%dx=%d>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, u_, p_, x_

/* Same as above, but with formatting options */
#define BSL_META_O(opt_, str_) \
    "<c=%uf=%sl=%dF=%so=%u>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_
#define BSL_META_OU(opt_, u_, str_) \
    "<c=%uf=%sl=%dF=%so=%uu=%d>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_, u_
#define BSL_META_OUP(opt_, u_, p_, str_) \
    "<c=%uf=%sl=%dF=%so=%uu=%dp=%d>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_, u_, p_
#define BSL_META_OUPX(opt_, u_, p_, x_, str_) \
    "<c=%uf=%sl=%dF=%so=%uu=%dp=%dx=%d>" str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_, u_, p_, x_

/* Macro for invoking "fast" checker */
#define BSL_LOG(chk_, stuff_) do {              \
        if (bsl_fast_check(chk_)) {             \
            unsigned int mchk_ = chk_;          \
            (void)mchk_;                        \
            bsl_printf stuff_;                  \
        }                                       \
    } while (0)

/* Any layer log macros */
#define LOG_FATAL(ls_, stuff_)          BSL_LOG(ls_|BSL_FATAL, stuff_)
#define LOG_ERROR(ls_, stuff_)          BSL_LOG(ls_|BSL_ERROR, stuff_)
#define LOG_WARN(ls_, stuff_)           BSL_LOG(ls_|BSL_WARN, stuff_)
#define LOG_INFO(ls_, stuff_)           BSL_LOG(ls_|BSL_INFO, stuff_)
#define LOG_VERBOSE(ls_, stuff_)        BSL_LOG(ls_|BSL_VERBOSE, stuff_)
#define LOG_DEBUG(ls_, stuff_)          BSL_LOG(ls_|BSL_DEBUG, stuff_)

/* Shell output from core driver */
#define BSL_LSS_CLI                     (BSL_L_APPL | BSL_S_SHELL | BSL_INFO)
#define LOG_CLI(stuff_)                 BSL_LOG(BSL_LSS_CLI, stuff_)

/* Shell output from CLI (interactive shell) */
#define cli_out bsl_printf

/* Wrapper macro for layer/source/severity checker */
#define LOG_CHECK(packed_meta_)         bsl_fast_check(packed_meta_)

/* 
 * DEPRECATED FUNCTIONS
 */
extern int
_bsl_error(char *file, unsigned int line, const char *func,
          bsl_layer_t layer, int source,
          int unit, const char *msg, ...);

extern int 
_bsl_warn(char *file, unsigned int line, const char *func,
         bsl_layer_t layer, int source,    
         int unit, const char *msg, ...);

extern int
_bsl_verbose(char *file, unsigned int line, const char *func,
            bsl_layer_t layer, int source, 
            int unit, const char *msg, ...);

extern int 
_bsl_vverbose(char *file, unsigned int line, const char *func,
             bsl_layer_t layer, int source, 
             int unit, const char *msg, ...);

extern int
bsl_check(bsl_layer_t layer, int source, bsl_severity_t severity, 
          int unit); 

/* 
 * bsl_start
 * Parameters: as per bsl_log
 * Return value: TRUE if output should occur (as for bsl_check)
 */
extern int
bsl_log_start(char *file, unsigned int line, const char *func, 
              bsl_layer_t layer, int source, bsl_severity_t severity,
              int unit, const char *msg, ...);

/* 
 * bsl_end
 * Incrementally adds additional output to a message previously begun with bsl_start
 * and finishes that message
 * Parameters: as per bsl_log 
 */
extern int
bsl_log_end(char *file, unsigned int line, const char *func, 
            bsl_layer_t layer, int source, bsl_severity_t severity,
            int sinks, int unit, 
            const char *msg, ...);

/* 
 * bsl_add
 * Incrementally adds additional output to a message previously begun with bsl_start 
 * Parameters: as per bsl_log 
 */
extern int
bsl_log_add(char *file, unsigned int line, const char *func, 
            bsl_layer_t layer, int source, bsl_severity_t severity,
            int sinks, int unit, 
            const char *msg, ...);

extern int
bsl_fast_check(bsl_packed_meta_t chk);

extern int
bsl_fast_check_two(uint32 chk1,uint32 chk2);

extern void
bsl_meta_t_init(bsl_meta_t *meta);

extern int
bsl_vprintf(const char *format, va_list args)
    COMPILER_ATTRIBUTE ((format (printf, 1, 0)));

extern int
bsl_printf(const char *format, ...)
    COMPILER_ATTRIBUTE ((format (printf, 1, 2)));

extern int
bsl_vlog(bsl_meta_t *meta, const char *format, va_list args)
    COMPILER_ATTRIBUTE ((format (printf, 2, 0)));

extern int
bsl_log(bsl_meta_t *meta, const char *format, ...)
    COMPILER_ATTRIBUTE ((format (printf, 2, 3)));

#endif /* _SHR_BSL_H_ */
