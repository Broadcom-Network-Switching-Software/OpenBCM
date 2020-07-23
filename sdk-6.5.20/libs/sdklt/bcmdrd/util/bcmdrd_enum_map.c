/*! \file bcmdrd_enum_map.c
 *
 * BCMDRD utility functions for enum name mapping.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_types.h>

#include <bcmdrd/bcmdrd_enum_map.h>

/*******************************************************************************
 * Local variables
 */

/* Enum for symbol flags */
static shr_enum_map_t sym_flags_table[] = {
    { "register",          BCMDRD_SYMBOL_FLAG_REGISTER        },
    { "port",              BCMDRD_SYMBOL_FLAG_PORT            },
    { "counter",           BCMDRD_SYMBOL_FLAG_COUNTER         },
    { "memory",            BCMDRD_SYMBOL_FLAG_MEMORY          },
    { "big-endian",        BCMDRD_SYMBOL_FLAG_BIG_ENDIAN      },
    { "soft-port",         BCMDRD_SYMBOL_FLAG_SOFT_PORT       },
    { "memmapped",         BCMDRD_SYMBOL_FLAG_MEMMAPPED       },
    { "cam",               BCMDRD_SYMBOL_FLAG_CAM             },
    { "ext-cam",           BCMDRD_SYMBOL_FLAG_EXT_CAM         },
    { "hashed",            BCMDRD_SYMBOL_FLAG_HASHED          },
    { "cacheable",         BCMDRD_SYMBOL_FLAG_CACHEABLE       },
    { "fifo",              BCMDRD_SYMBOL_FLAG_FIFO            },
    { "read-only",         BCMDRD_SYMBOL_FLAG_READONLY        },
    { "overlay",           BCMDRD_SYMBOL_FLAG_OVERLAY         },
    { "iproc",             BCMDRD_SYMBOL_FLAG_IPROC           },
    { "notest",            BCMDRD_SYMBOL_FLAG_NOTEST          },
    { "write-only",        BCMDRD_SYMBOL_FLAG_WRITEONLY       },
    { "sub-pipe",          BCMDRD_SYMBOL_FLAG_SUB_PIPE        },
    { "acc-unique-pipe",   BCMDRD_SYMBOL_FLAG_ACC_UNIQUE_PIPE },
    { NULL, 0 }
};

/*******************************************************************************
 * Private functions
 */

static const char *
enum_val2name(shr_enum_map_t *e, int value)
{
    for (; e->name; e++) {
        if (e->val == value) {
            break;
        }
    }
    return e->name;
}

static int
enum_name2val(shr_enum_map_t *e, const char *name)
{
    for (; e->name; e++) {
        if (sal_strcmp(e->name, name) == 0) {
            break;
        }
    }
    return e->val;
}

/*******************************************************************************
 * Public functions
 */

const char *
bcmdrd_enum_sym_flag_name(uint32_t flag)
{
    shr_enum_map_t *e = sym_flags_table;

    return enum_val2name(e, flag);
}

uint32_t
bcmdrd_enum_sym_flag_val(const char *name)
{
    shr_enum_map_t *e = sym_flags_table;

    return (uint32_t)enum_name2val(e, name);
}
