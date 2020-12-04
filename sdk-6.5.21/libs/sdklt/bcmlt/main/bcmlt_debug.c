/*! \file bcmlt_debug.c
 *
 * BCMLT Entry and transactions debug
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <bsl/bsl.h>
#include <shr/shr_fmm.h>
#include <shr/shr_pb.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include "bcmlt_internal.h"

static const char *ltopcode_to_str(bcmlt_opcode_t opcode) {
    static const char *operation_str[] = {
        "",
        "insert",
        "lookup",
        "delete",
        "update",
        "traverse"
    };
    return operation_str[opcode];
}

static void dump_field_list(bcmtrm_entry_t *entry, char *ident, shr_pb_t *pb)
{
    shr_fmm_t *fld = entry->l_field;
    int rv;
    bcmlrd_client_field_info_t *fld_info;
    size_t num_of_fields;
    bcmlrd_table_attrib_t attrib;
    size_t actual_fields;
    size_t idx;
    const char *sym_val;

    rv = bcmlrd_table_attributes_get(entry->info.unit,
                                     entry->info.table_name,
                                     &attrib);
    if (rv != SHR_E_NONE) {
        return;
    }
    num_of_fields = attrib.number_of_fields;
    fld_info = sal_alloc(sizeof(bcmlrd_client_field_info_t)
                            * num_of_fields, "bcmltDebug");
    if (!fld_info) {
        return;
    }

    rv = bcmlrd_table_fields_def_get(entry->info.unit,
                                     entry->info.table_name,
                                     num_of_fields,
                                     fld_info,
                                     &actual_fields);
    if ((rv != SHR_E_NONE) || (num_of_fields != actual_fields)) {
        sal_free(fld_info);
        return;
    }

    while (fld) {
        /* Find the field name */
        for (idx = 0; idx < num_of_fields; idx++) {
            if (fld_info[idx].id == fld->id) {
                break;
            }
        }
        if (idx >= num_of_fields) {
            sal_free(fld_info);
            return;
        }
        shr_pb_printf(pb,
                      "%sField %s val",
                      ident, fld_info[idx].name);
        if (fld_info[idx].depth > 1) {  /* Field is part of an array */
            uint32_t id = fld->id;

            shr_pb_printf(pb, "\n");
            /* All the array elements linked in order */
            while (fld && fld->id == id) {
                if (fld_info[idx].symbol) {
                    rv = bcmlrd_field_value_to_symbol(entry->info.unit,
                                                      entry->table_id,
                                                      fld->id,
                                                      (uint32_t)fld->data,
                                                      &sym_val);
                    if (rv != SHR_E_NONE) {
                        sal_free(fld_info);
                        return;
                    }
                    shr_pb_printf(pb, "%s   [%u] = %s\n",
                                  ident, fld->idx, sym_val);
                } else {
                    shr_pb_printf(pb, "%s   [%u] = %"PRIu64"\n",
                                  ident, fld->idx, fld->data);
                }
                fld = fld->next;
            }
        } else {
            if (fld_info[idx].symbol) {
                rv = bcmlrd_field_value_to_symbol(entry->info.unit,
                                                  entry->table_id,
                                                  fld->id,
                                                  (uint32_t)fld->data,
                                                  &sym_val);
                if (rv != SHR_E_NONE) {
                    sal_free(fld_info);
                    return;
                }
                shr_pb_printf(pb, " = %s\n", sym_val);
            } else {
                shr_pb_printf(pb, " = %"PRIu64"\n", fld->data);
            }
            fld = fld->next;
        }
    }

    sal_free(fld_info);
}

void bcmlt_dump_entry(bcmtrm_entry_t *entry, void *pb)
{
    char ident[3];
    unsigned int mchk_ = BSL_LS_BCMLT_DETAILS | BSL_INFO;
    int idx;
    shr_pb_t *local_pb;

    ident[0] = (entry->p_trans ? '\t' : '\0');
    ident[1] = '\0';

    if (!pb) {
        local_pb = shr_pb_create();
        if (!local_pb) {
            return;
        }
        shr_pb_printf(local_pb, "Unit=%d ", entry->info.unit);
    } else {
        local_pb = (shr_pb_t *)pb;
        shr_pb_printf(local_pb, "%s", ident);
    }
    if (!entry->pt) {
        shr_pb_printf(local_pb,
                      "%s processing entry for table %s (%s) opcode %s\n",
                      entry->asynch ? "Asynch" : "Sync",
                      entry->info.table_name,
                      entry->interactive ? "interactive" : "modeled",
                      ltopcode_to_str(entry->opcode.lt_opcode));
    } else {
        shr_pb_printf(local_pb,
                      "%s processing PT entry for table %s\n",
                      entry->asynch ? "Asynch" : "Sync",
                      entry->info.table_name);
    }
    idx = (entry->p_trans ? 1 : 0);
    ident[idx] = '\t';
    ident[idx+1] = '\0';
    dump_field_list(entry, ident, local_pb);
    if (!pb) {
        bsl_printf(BSL_META("\n%s"), shr_pb_str(local_pb));
        shr_pb_destroy(local_pb);
    }
}

void bcmlt_dump_trans(bcmtrm_trans_t *trans)
{
    unsigned int mchk_ = BSL_LS_BCMLT_DETAILS | BSL_INFO;
    bcmtrm_entry_t *entry = trans->l_entries;
    shr_pb_t *pb;

    pb = shr_pb_create();
    if (!pb) {
        return;
    }
    shr_pb_printf(pb,
                  "Unit=%d Processing %s transaction %s\n",
                  trans->unit,
                  trans->info.type == BCMLT_TRANS_TYPE_ATOMIC ?
                        "atomic" : "batch",
                  trans->syncronous ? "synchronous" : "asynchronous");

    while (entry) {
        bcmlt_dump_entry(entry, pb);
        entry = entry->next;
    }
    bsl_printf(BSL_META("\n%s"), shr_pb_str(pb));
    shr_pb_destroy(pb);
}

