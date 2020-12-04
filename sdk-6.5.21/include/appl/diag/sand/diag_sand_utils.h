/**
 * \file diag_sand_utils.h
 *
 * Misc. utilities for shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_SAND_UTILS_H_INCLUDED
#define DIAG_SAND_UTILS_H_INCLUDED

#include <sal/core/libc.h>
#include <sal/appl/field_types.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/register.h>
#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>

/**
 * \brief This macro, _SHR_CLI_EXIT, is the actual implementation of \ref SHR_CLI_EXIT
 *   Its documentation applies to \ref SHR_CLI_EXIT below.
 *   This macro is NOT to be used except from within \ref SHR_CLI_EXIT
 */
/**
 * \brief Set exit error code and error-exit on expression error
 * with user log that has any number of parameters on formatting
 * string.
 *
 * \par DIRECT INPUT:
 *   \param [in] _expr              -\n
 *     Expression representing error. One of shr_error_e (_shr_error_t).
 *     If it evaluates to a 'non error' code then no log is created.
 *   \param [in] _formatting_string -
 *     Formatting string (to display) such that it matches the list
 *     of parameters (_p1, _p2, ...)
 *   \param [in] ...
 *     Any number of parameters (_p1,_p2,_p3,...) to display via the
 *     formatting string.
 * \par INDIRECT INPUT:
 *    LOG_ERROR            -             \n
 *      Standard BSL (JR1) LOG macro     \n
 *    _func_unit           -             \n
 *      Value of 'unit' input parameter at entry     \n
 *    BSL_META_U           -                         \n
 *      Standard BSL (JR1) meta data constructor
 * \par INDIRECT OUTPUT:
 *   * Newly set exit error value.
 *   * Full formatted string which may also be logged or printed.
 *   * Exit procedure.
 * \remark
 *   The number of parameters is fleaxible. This is forwarded
 *   to underlying macros using __VA_ARGS__
 *   See also: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 *   (including the explanation for ## before __VA_ARGS__)
 * \remark
 *   Note that with the introduction of the SHR_CLI_EXIT/_SHR_CLI_EXIT pair, the number of input parameters to
 *   _SHR_CLI_EXIT is at least one and it is not necessary to add the '##' before __VA_ARGS__. However, we leave
 *   it there as an example of usage. Also, the formatting string assumes an extra parameter of type 'string"!
 */
#define _SHR_CLI_EXIT(_expr,_formatting_string,...) \
{ \
  int _rv = _expr;    \
  LOG_CLI((BSL_META_U(_func_unit, _formatting_string "%s"), ##__VA_ARGS__) ) ; \
  _func_rv = _rv ;    \
  SHR_EXIT() ;        \
}
/**
 * \brief
 *   Use this macro with the input as described on \ref _SHR_CLI_EXIT above!!!
 *
 *   This is only a 'prelude' for the main macro \ref _SHR_CLI_EXIT and
 *   is only intended TO ADD AN EMPTY PARAMETER.
 *   This is required because a variadic macro does not take invocation with no
 *   parameters when compiled under --pedantic-64
 * \sa https://notmuchmail.org/pipermail/notmuch/2012/007436.html
 */
#define SHR_CLI_EXIT(...) _SHR_CLI_EXIT(__VA_ARGS__, "")

/**
 * The same as above only exit when error
 */
#define _SHR_CLI_EXIT_IF_ERR(_expr,_formatting_string,...) \
{ \
  int _rv = _expr;    \
  if (SHR_FAILURE(_rv)) \
  {             \
      LOG_CLI((BSL_META_U(_func_unit, _formatting_string "%s"), ##__VA_ARGS__) ) ; \
      _func_rv = _rv ;    \
      SHR_EXIT() ;        \
  }\
}

#define SHR_CLI_EXIT_IF_ERR(...) _SHR_CLI_EXIT_IF_ERR(__VA_ARGS__, "")

void diag_sand_utils_collect_comma_args(
    args_t * a,
    char *valstr,
    char *first);

cmd_result_t diag_sand_error_get(
    shr_error_e shr_ret);

void diag_sand_value_to_str(
    uint32 *value,
    int value_bit_size,
    char *value_str,
    int value_str_size);

int diag_sand_value_same(
    uint32 *value_first,
    uint32 *value_second,
    int value_bit_size);

#define SHR_ERR_TO_SHELL    diag_sand_error_get(_func_rv)

shr_error_e diag_sand_compare_init(
    char *match_n,
    void **compare_handle_p);

int diag_sand_compare(
    void *compare_handle,
    char *string);

void diag_sand_compare_close(
    void *compare_handle);

shr_error_e diag_sand_reg_blocks_get(
    int unit,
    soc_reg_info_t * reginfo,
    char *block_str,
    char *match_n);

shr_error_e diag_sand_mem_blocks_get(
    int unit,
    soc_mem_t mem,
    char *block_str,
    char *match_n);

extern char cmd_set_device_usage[];

cmd_result_t cmd_set_device(
    int unit,
    args_t * a);

#endif /* DIAG_SAND_UTILS_H_INCLUDED */
