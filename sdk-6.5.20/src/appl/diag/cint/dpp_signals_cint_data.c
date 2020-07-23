/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hand-coded support for dpp_dsig_read routine and probable other
 */

#if defined(INCLUDE_LIB_CINT) && defined(BCM_SAND_SUPPORT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <soc/defs.h>
#include <sal/core/libc.h>

#include <soc/sand/sand_signals.h>

/* Macros section */
CINT_FWRAPPER_CREATE_RP8(int, int, 0, 0,
                         dpp_dsig_read,
                         int, int, unit, 0, 0,
                         int, int, core, 0, 0,
                         char *, char, block, 1, 0,
                         char *, char, from, 1, 0,
                         char *, char, to, 1, 0,
                         char *, char, name, 1, 0,
                         uint32 *, uint32, result, 1, 0,
                         int, int, size, 0, 0);

/* Functions and Macros mapping*/
static cint_function_t __cint_functions[] = {
        CINT_FWRAPPER_ENTRY(dpp_dsig_read),
        { NULL }
};

/* Structs section */
static void*
__cint_maddr__match_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    match_t *s = (match_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->block);
        break;
    case 1:
        rv = &(s->from);
        break;
    case 2:
        rv = &(s->to);
        break;
    case 3:
        rv = &(s->name);
        break;
    case 4:
        rv = &(s->flags);
        break;
    case 5:
        rv = &(s->output_order);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__match_t[] = { { "char",
        "block", 1, 0 }, { "char", "from", 1, 0 }, { "char", "to", 1, 0 }, {
        "char", "name", 1, 0 }, { "int", "flags", 0, 0 }, { "int",
        "output_order", 0, 0 }, { NULL } };

static void*
__cint_maddr__signal_address_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    signal_address_t *s = (signal_address_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->high);
        break;
    case 1:
        rv = &(s->low);
        break;
    case 2:
        rv = &(s->msb);
        break;
    case 3:
        rv = &(s->lsb);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__signal_address_t[] = { {
        "int", "high", 0, 0 }, { "int", "low", 0, 0 }, { "int", "msb", 0, 0 }, {
        "int", "lsb", 0, 0 }, { NULL } };

static void*
__cint_maddr__rhentry_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    rhentry_t *s = (rhentry_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->next);
        break;
    case 1:
        rv = &(s->prev);
        break;
    case 2:
        rv = &(s->id);
        break;
    case 3:
        rv = &(s->name);
        break;
    case 4:
        rv = &(s->users);
        break;
    case 5:
        rv = &(s->mode);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__rhentry_t[] = { {
        "rhentry_t", "next", 1, 0 }, { "rhentry_t", "prev", 1, 0 }, { "rhid",
        "id", 0, 0 }, { "char", "name", 0, RHNAME_MAX_SIZE }, { "int", "users",
        0, 0 }, { "int", "mode", 0, 0 }, { NULL } };

static void*
__cint_maddr__rhlist_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    rhlist_t *s = (rhlist_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->entry);
        break;
    case 1:
        rv = &(s->top);
        break;
    case 2:
        rv = &(s->tail);
        break;
    case 3:
        rv = &(s->num);
        break;
    case 4:
        rv = &(s->max_id);
        break;
    case 5:
        rv = &(s->entry_size);
        break;
    case 6:
        rv = &(s->sanity);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__rhlist_t[] = { {
        "rhentry_t", "entry", 0, 0 }, { "rhentry_t", "top", 1, 0 }, {
        "rhentry_t", "tail", 1, 0 }, { "int", "num", 0, 0 }, { "int", "max_id",
        0, 0 }, { "int", "entry_size", 0, 0 }, { "int", "sanity", 0, 0 },
        { NULL } };

static void*
__cint_maddr__debug_signal_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    debug_signal_t *s = (debug_signal_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->size);
        break;
    case 1:
        rv = &(s->block_id);
        break;
    case 2:
        rv = &(s->address);
        break;
    case 3:
        rv = &(s->range_num);
        break;
    case 4:
        rv = &(s->double_flag);
        break;
    case 5:
        rv = &(s->expansion);
        break;
    case 6:
        rv = &(s->resolution);
        break;
    case 7:
        rv = &(s->from);
        break;
    case 8:
        rv = &(s->to);
        break;
    case 9:
        rv = &(s->attribute);
        break;
    case 10:
        rv = &(s->hw);
        break;
    case 11:
        rv = &(s->cond_attribute);
        break;
    case 12:
        rv = &(s->cond_value);
        break;
    case 13:
        rv = &(s->cond_signal);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__debug_signal_t[] = {
        {"int",  "size", 0, 0 },
        {"int",  "block_id", 0, 0 },
        {"signal_address_t", "address", 0, DSIG_MAX_ADDRESS_RANGE_NUM },
        {"int",  "range_num", 0, 0 },
        {"int",  "double_flag", 0, 0 },
        {"char", "expansion", 0, RHNAME_MAX_SIZE },
        {"char", "resolution", 0, RHNAME_MAX_SIZE },
        {"char", "from", 0, RHNAME_MAX_SIZE },
        {"char", "to", 0, RHNAME_MAX_SIZE },
        {"char", "attribute", 0, RHNAME_MAX_SIZE },
        {"char", "hw", 0, RHSTRING_MAX_SIZE },
        {"char", "cond_attribute", 0, RHNAME_MAX_SIZE },
        {"int",  "cond_value", 0, 0 },
        {"struct debug_signal_s", "cond_signal", 1, 0 },
        { NULL } };

static void *
__cint_maddr__signal_output_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    signal_output_t *s = (signal_output_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->entry);
        break;
    case 1:
        rv = &(s->debug_signal);
        break;
    case 2:
        rv = &(s->size);
        break;
    case 3:
        rv = &(s->value);
        break;
    case 4:
        rv = &(s->print_value);
        break;
    case 5:
        rv = &(s->field_list);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__signal_output[] = {
        { "rhentry_t",      "entry",        0, 0 },
        { "debug_signal_t", "debug_signal", 1, 0 },
        { "int",            "size",         0, 0 },
        { "uint32",         "value",        0, DSIG_MAX_SIZE_UINT32},
        { "char",           "print_value",  0, DSIG_MAX_SIZE_STR},
        { "rhlist_t",       "field_list",   0, 0 }, { NULL } };

static cint_struct_type_t __cint_structures[] = { { "match_t", sizeof(match_t),
        __cint_struct_members__match_t, __cint_maddr__match_t },
        { "signal_address_t", sizeof(signal_address_t),
                __cint_struct_members__signal_address_t,
                __cint_maddr__signal_address_t }, { "rhentry_t",
                sizeof(rhentry_t), __cint_struct_members__rhentry_t,
                __cint_maddr__rhentry_t }, { "rhlist_t", sizeof(rhlist_t),
                __cint_struct_members__rhlist_t, __cint_maddr__rhlist_t }, {
                "debug_signal_t", sizeof(debug_signal_t),
                __cint_struct_members__debug_signal_t,
                __cint_maddr__debug_signal_t }, { "signal_output_t",
                sizeof(signal_output_t), __cint_struct_members__signal_output,
                __cint_maddr__signal_output_t }, { NULL } };

/* Enums section */

static cint_enum_type_t __cint_enums[] = { { NULL } };

/* Defines section */
static cint_constants_t __cint_constants[] = { { NULL } };

/* Typedefs section */
static cint_parameter_desc_t __cint_typedefs[] = { { "unsigned int", "uint32",
        0, 0 }, { "int", "rhid", 0, 0 }, { NULL } };

/* Functions pointers section */
/*static cint_function_pointer_t __cint_function_pointers[0];*/

cint_data_t dpp_signals_cint_data = {
NULL, __cint_functions, __cint_structures, __cint_enums, __cint_typedefs,
        __cint_constants,
        NULL };

#else
typedef int _dpp_signals_data_not_empty; /* Make ISO compilers happy. */
#endif /* defined(INCLUDE_LIB_CINT) & defined(BCM_PETRA_SUPPORT) */
