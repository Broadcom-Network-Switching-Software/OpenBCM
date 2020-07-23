/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        api_mode.h
 * Purpose:     API mode header
 */

#ifndef   _API_MODE_H_
#define   _API_MODE_H_

#include "tokenizer.h"
#include "cint_datatypes.h"
#include "shared/error.h"

#define API_MODE_E_NONE         _SHR_E_NONE
#define API_MODE_E_INTERNAL     _SHR_E_INTERNAL
#define API_MODE_E_MEMORY       _SHR_E_MEMORY
#define API_MODE_E_UNIT         _SHR_E_UNIT
#define API_MODE_E_PARAM        _SHR_E_PARAM
#define API_MODE_E_EMPTY        _SHR_E_EMPTY
#define API_MODE_E_FULL         _SHR_E_FULL
#define API_MODE_E_NOT_FOUND    _SHR_E_NOT_FOUND
#define API_MODE_E_EXISTS       _SHR_E_EXISTS
#define API_MODE_E_TIMEOUT      _SHR_E_TIMEOUT
#define API_MODE_E_BUSY         _SHR_E_BUSY
#define API_MODE_E_FAIL         _SHR_E_FAIL
#define API_MODE_E_DISABLED     _SHR_E_DISABLED
#define API_MODE_E_BADID        _SHR_E_BADID
#define API_MODE_E_RESOURCE     _SHR_E_RESOURCE
#define API_MODE_E_CONFIG       _SHR_E_CONFIG
#define API_MODE_E_UNAVAIL      _SHR_E_UNAVAIL
#define API_MODE_E_INIT         _SHR_E_INIT
#define API_MODE_E_PORT         _SHR_E_PORT

#define API_MODE_SUCCESS(_rv) _SHR_E_SUCCESS(_rv) 
#define API_MODE_FAILURE(_rv) _SHR_E_FAILURE(_rv) 
#define API_MODE_IF_ERROR_RETURN(_rv) _SHR_E_IF_ERROR_RETURN(_rv) 

#define HELP '?'
#define INFO '!'

#define IS_FIRST     0x0001
#define IS_VAR       0x0002
#define IS_TYPE      0x0004

typedef struct api_mode_arg_s {
    int flags;
    int kind;
    const char *value;
    api_mode_token_t *token;
    const cint_datatype_t *dt;
    struct api_mode_arg_s *sub;         /* composite or keyword:value */
    struct api_mode_arg_s *parent;      /* parent of sub */
    struct api_mode_arg_s *next;        /* next in arg list */
    struct api_mode_arg_s *mm;          /* next in memory management list */
} api_mode_arg_t;

typedef int (*api_mode_arg_cb_t)(api_mode_arg_t *arg, void *user_data);

typedef struct api_mode_parse_s {
    int verbose;
    int done;
    int result;
    api_mode_arg_t *root;
    api_mode_arg_t *base;
    api_mode_arg_cb_t callback;
    void *user_data;
} api_mode_parse_t;

extern int api_mode_debug;

extern int api_mode_token(const char *s, void *parser,
                          int kind, api_mode_arg_t **arg);
extern int api_mode_get(void *p, char *buf, int max_size);

extern api_mode_arg_t *api_mode_node(void *prs, const char *value, int kind);
extern api_mode_arg_t *api_mode_execute(void *prs, api_mode_arg_t *arg);
extern api_mode_arg_t *api_mode_mark(api_mode_arg_t *arg, int flag);
extern api_mode_arg_t *api_mode_sub(api_mode_arg_t *pri, api_mode_arg_t *sec);
extern api_mode_arg_t *api_mode_key_value(api_mode_arg_t *key,
                                          api_mode_arg_t *value);
extern api_mode_arg_t *api_mode_range(api_mode_arg_t *from,
                                      api_mode_arg_t *to,
                                      api_mode_arg_t *times,
                                      api_mode_arg_t *incr);
extern api_mode_arg_t *api_mode_append(api_mode_arg_t *src,
                                       api_mode_arg_t *arg);
extern api_mode_arg_t *api_mode_sub_append(api_mode_arg_t *src,
                                           api_mode_arg_t *arg);

extern api_mode_arg_t *api_mode_arg_next(api_mode_arg_t *arg);

#define PARSE_VERBOSE 0x0001
#define PARSE_DEBUG   0x0002
#define SCAN_DEBUG    0x0004

extern int api_mode_parse_string(const char *input, int flags,
                                 api_mode_arg_cb_t cb, void *user_data);
extern void api_mode_show(int indent, api_mode_arg_t *start);
extern int api_mode_process_command(int u, char *s);
extern int api_mode_unexpected(void);

#endif /* _API_MODE_H_ */
