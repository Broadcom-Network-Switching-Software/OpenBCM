/*! \file bcma_bcmlt_enum_map.c
 *
 * Functions related to string mapping to enum in BCMLT module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/bcmlt/bcma_bcmlt_enum_map.h>

/* Logical table operation codes */
static bcma_cli_parse_enum_t lt_cmds_enum[] = {
    { "delete",   BCMLT_OPCODE_DELETE   },
    { "insert",   BCMLT_OPCODE_INSERT   },
    { "update",   BCMLT_OPCODE_UPDATE   },
    { "lookup",   BCMLT_OPCODE_LOOKUP   },
    { "traverse", BCMLT_OPCODE_TRAVERSE },
    { NULL,       BCMLT_OPCODE_NOP      }
};

/* Physical table operation codes */
static bcma_cli_parse_enum_t pt_cmds_enum[] = {
    { "set",    BCMLT_PT_OPCODE_SET       },
    { "get",    BCMLT_PT_OPCODE_GET       },
    { "modify", BCMLT_PT_OPCODE_MODIFY    },
    { "lookup", BCMLT_PT_OPCODE_LOOKUP    },
    { NULL,     BCMLT_PT_OPCODE_NOP       }
};

/* BCMLT entry attributes */
static bcma_cli_parse_enum_t ent_attr_enum[] = {
    { "CACHE_BYPASS",   BCMLT_ENT_ATTR_GET_FROM_HW       },
    { "FILTER_DFLT",    BCMLT_ENT_ATTR_FILTER_DEFAULTS   },
    { "LPM_LOOKUP",     BCMLT_ENT_ATTR_LPM_LOOKUP        },
    { "TRVS_SNAPSHOT",  BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT },
    { "TRVS_DONE",      BCMLT_ENT_ATTR_TRAVERSE_DONE     },
    { NULL,             0                                }
};

/* Transaction type */
static bcma_cli_parse_enum_t trans_type_enum[] = {
    { "Atomic", BCMLT_TRANS_TYPE_ATOMIC },
    { "Batch",  BCMLT_TRANS_TYPE_BATCH  },
    { NULL,     0                       }
};

/* Commit priority */
static bcma_cli_parse_enum_t commit_pri_enum[] = {
    { "Normal", BCMLT_PRIORITY_NORMAL },
    { "High",   BCMLT_PRIORITY_HIGH   },
    { NULL,     0                     }
};

/* Commit notify option */
static bcma_cli_parse_enum_t notify_opt_enum[] = {
    { "Commit",    BCMLT_NOTIF_OPTION_COMMIT   },
    { "Hw",        BCMLT_NOTIF_OPTION_HW       },
    { "All",       BCMLT_NOTIF_OPTION_ALL      },
    { "None",      BCMLT_NOTIF_OPTION_NO_NOTIF },
    { NULL,        0                           }
};

const bcma_cli_parse_enum_t *
bcma_bcmlt_opcode_enum_get(bool physical)
{
    return physical ? pt_cmds_enum : lt_cmds_enum;
}

const bcma_cli_parse_enum_t *
bcma_bcmlt_ent_attr_enum_get(void)
{
    return ent_attr_enum;
}

const bcma_cli_parse_enum_t *
bcma_bcmlt_trans_type_enum_get(void)
{
    return trans_type_enum;
}

const bcma_cli_parse_enum_t *
bcma_bcmlt_commit_pri_enum_get(void)
{
    return commit_pri_enum;
}

const bcma_cli_parse_enum_t *
bcma_bcmlt_notify_opt_enum_get(void)
{
    return notify_opt_enum;
}

const char *
bcma_bcmlt_opcode_format(int opc, bool physical)
{
    bcma_cli_parse_enum_t *e = physical ? pt_cmds_enum : lt_cmds_enum;

    while (e->name != NULL) {
        if (e->val == opc) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid OP>" : e->name;
}

const char *
bcma_bcmlt_opcode_format_by_entry_info(bcmlt_entry_info_t *bcmlt_ei,
                                       int *opcode, bool *physical)
{
    int rv;
    bcmlt_unified_opcode_t uopc;

    if (bcmlt_ei == NULL) {
        return NULL;
    }

    rv = bcmlt_entry_oper_get(bcmlt_ei->entry_hdl, &uopc);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get table %s entry op code: "
                            "%s (%d).\n"),
                   bcmlt_ei->table_name, shr_errmsg(rv), rv));
        return NULL;
    }
    if (opcode) {
        *opcode = uopc.pt ? uopc.opcode.pt : uopc.opcode.lt;
    }
    if (physical) {
        *physical = uopc.pt;
    }

    if (uopc.pt) {
        return bcma_bcmlt_opcode_format(uopc.opcode.pt, uopc.pt);
    }
    return bcma_bcmlt_opcode_format(uopc.opcode.lt, uopc.pt);
}

const char *
bcma_bcmlt_notification_format(bcmlt_notif_option_t ntf)
{
    bcma_cli_parse_enum_t *e = &notify_opt_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_notif_option_t)e->val == ntf) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid NOTIFY>" : e->name;
}

const char *
bcma_bcmlt_priority_format(bcmlt_priority_level_t priority)
{
    bcma_cli_parse_enum_t *e = &commit_pri_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_priority_level_t)e->val == priority) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid PRIORITY>" : e->name;
}

const char *
bcma_bcmlt_trans_type_format(bcmlt_trans_type_t type)
{
    bcma_cli_parse_enum_t *e = &trans_type_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_trans_type_t)e->val == type) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid TRANS_TYPE>" : e->name;
}
