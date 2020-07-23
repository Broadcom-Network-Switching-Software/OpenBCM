/*! \file bcma_bcmbd_ser.c
 *
 * BCMBD command ser utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>

#include <bcmdrd/bcmdrd_ser.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

typedef struct ser_map_s {
    const char *name;
    int value;
} ser_map_t;

static ser_map_t ser_type_table[] = {
    { "undefined",     BCMDRD_SER_TYPE_UNDEFINED     },
    { "buffer",        BCMDRD_SER_TYPE_BUFFER        },
    { "bus",           BCMDRD_SER_TYPE_BUS           },
    { "direct",        BCMDRD_SER_TYPE_DIRECT        },
    { "direct_lp",     BCMDRD_SER_TYPE_DIRECT_LP     },
    { "direct_xor",    BCMDRD_SER_TYPE_DIRECT_XOR    },
    { "direct_xor_lp", BCMDRD_SER_TYPE_DIRECT_XOR_LP },
    { "hash",          BCMDRD_SER_TYPE_HASH          },
    { "hash_lp",       BCMDRD_SER_TYPE_HASH_LP       },
    { "hash_xor",      BCMDRD_SER_TYPE_HASH_XOR      },
    { "hash_xor_lp",   BCMDRD_SER_TYPE_HASH_XOR_LP   },
    { "tcam",          BCMDRD_SER_TYPE_TCAM          },
    { NULL,            0                             }
};

static ser_map_t ser_resp_table[] = {
    { "none",                BCMDRD_SER_RESP_NONE                },
    { "entry_clear",         BCMDRD_SER_RESP_ENTRY_CLEAR         },
    { "cache_restore",       BCMDRD_SER_RESP_CACHE_RESTORE       },
    { "ecc_correctable",     BCMDRD_SER_RESP_ECC_CORRECTABLE     },
    { "write_cache_restore", BCMDRD_SER_RESP_WRITE_CACHE_RESTORE },
    { "special",             BCMDRD_SER_RESP_SPECIAL             },
    { "ecc_parity",          BCMDRD_SER_RESP_ECC_PARITY          },
    { NULL,                  0                                   }
};

static ser_map_t ser_en_type_table[] = {
    { "EC enable:",    BCMDRD_SER_EN_TYPE_EC    },
    { "1b enable:",    BCMDRD_SER_EN_TYPE_ECC1B },
    { "Force enable:", BCMDRD_SER_EN_TYPE_FE    },
    { "TCAM enable:",  BCMDRD_SER_EN_TYPE_TCAM  },
    { NULL,            0                        }
};

static const char *
ser_enum2name(ser_map_t *e, int value)
{
    for (; e->name; e++) {
        if (e->value == value) {
            return e->name;
        }
    }
    return "<N/A>";
}

const char *
bcma_bcmbd_ser_type2name(bcmdrd_ser_type_t ser_type)
{
    ser_map_t *e = ser_type_table;

    return ser_enum2name(e, ser_type);
}

const char *
bcma_bcmbd_ser_resp_type2name(bcmdrd_ser_resp_t ser_resp_type)
{
    ser_map_t *e = ser_resp_table;

    return ser_enum2name(e, ser_resp_type);
}

const char *
bcma_bcmbd_ser_en_type2name(bcmdrd_ser_en_type_t ser_en_type)
{
    ser_map_t *e = ser_en_type_table;

    return ser_enum2name(e, ser_en_type);
}
