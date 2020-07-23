/*! \file field.c
 *
 * Field Module Diag support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/debug.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>

#include <bcm_int/ltsw/field.h>

#ifndef  SHR_BITDCL
#define    SHR_BITDCL        uint32
#define    SHR_BITWID        32

#define    SHR_BITGET(_a, _b)    _SHR_BITOP(_a, _b, &)

#define    _SHR_BITOP(_a, _b, _op)    \
        (((_a)[(_b) / SHR_BITWID]) _op (1U << ((_b) % SHR_BITWID)))
#endif


/*
 * Macro:
 *     FP_GET_NUMB
 * Purpose:
 *     Get a numerical value from stdin.
 */
#define FP_GET_NUMB(numb, str, args) \
    if (((str) = ARG_GET(args)) == NULL) { \
        return CMD_USAGE; \
    } \
    (numb) = parse_integer(str);

STATIC int ltsw_fp_list(int unit, args_t *args);
STATIC int ltsw_fp_list_actions(int unit, args_t *args);
STATIC int ltsw_fp_list_quals(int unit, args_t *args);

/*
 * Function:
 * Purpose:
 * Parmameters:
 * Returns:
 */
STATIC int
ltsw_fp_list(int unit, args_t *args) {
    char*               subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp list actions */
    if(!sal_strcasecmp(subcmd, "actions")) {
        return ltsw_fp_list_actions(unit, args);
    }
    /* BCM.0> fp list qualifiers */
    if(!sal_strcasecmp(subcmd, "qualifiers") ||
       !sal_strcasecmp(subcmd, "quals")) {
        return ltsw_fp_list_quals(unit, args);
    }

    return CMD_USAGE;
}

/*
 * Function:
 *     ltsw_fp_list_actions
 * Purpose:
 *     Display a list of Field actions
 * Parmameters:
 * Returns:
 */
STATIC int
ltsw_fp_list_actions(int unit, args_t *args)
{
    bcm_field_stage_t fp_stage = bcmFieldStageIngress;
    char * stage_s;
    bcm_field_aset_t aset;
    unsigned int action;
    int rv = BCM_E_NONE;
    static char *action_text[] = BCM_FIELD_ACTION_STRINGS;

    if ((stage_s = ARG_GET(args)) != NULL) {
        if(!sal_strcasecmp(stage_s, "lookup") ||
           !sal_strcasecmp(stage_s, "vfp")) {
            fp_stage = bcmFieldStageLookup;
        } else if(!sal_strcasecmp(stage_s, "ingress") ||
                  !sal_strcasecmp(stage_s, "ifp")) {
            fp_stage = bcmFieldStageIngress;
        } else if(!sal_strcasecmp(stage_s, "egress") ||
                  !sal_strcasecmp(stage_s, "efp")) {
            fp_stage = bcmFieldStageEgress;
        } else if(!sal_strcasecmp(stage_s, "exactmatch") ||
                !sal_strcasecmp(stage_s, "exa")) {
            fp_stage = bcmFieldStageIngressExactMatch;
        } else if(!sal_strcasecmp(stage_s, "flowtracker") ||
                !sal_strcasecmp(stage_s, "flo")) {
            fp_stage = bcmFieldStageIngressFlowtracker;
        } else {
            return CMD_NFND;
        }
        LOG_CLI((BSL_META_U(unit,
                    "List of supported actions:\n")));
        BCM_FIELD_ASET_INIT(aset);
        rv = bcm_field_stage_aset_get(unit, fp_stage, &aset);
        if (rv == BCM_E_NONE) {
            for (action=0; action <bcmFieldActionCount; action++) {
                if (BCM_FIELD_QSET_TEST(aset, action)) {
                    LOG_CLI((BSL_META_U(unit, "%s\n"), action_text[action]));
                }
            }
        }
        return CMD_OK;

    } else {
        return CMD_USAGE;
    }
}

/*
 * Function:
 *     ltsw_fp_list_quals
 * Purpose:
 *     Display a list of Field qualifiers
 * Parmameters:
 * Returns:
 */
STATIC int
ltsw_fp_list_quals(int unit, args_t *args)
{
    bcm_field_stage_t fp_stage = bcmFieldStageIngress;
    char * stage_s;
    bcm_field_qset_t qset;
    unsigned int qual;
    static char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
    int rv = 0;
    if ((stage_s = ARG_GET(args)) != NULL)  {
        if(!sal_strcasecmp(stage_s, "lookup") ||
           !sal_strcasecmp(stage_s, "vfp")) {
            fp_stage = bcmFieldStageLookup;
        } else if(!sal_strcasecmp(stage_s, "ingress") ||
                  !sal_strcasecmp(stage_s, "ifp")) {
            fp_stage = bcmFieldStageIngress;
        } else if(!sal_strcasecmp(stage_s, "egress") ||
                  !sal_strcasecmp(stage_s, "efp")) {
            fp_stage = bcmFieldStageEgress;
        } else if(!sal_strcasecmp(stage_s, "exactmatch") ||
                !sal_strcasecmp(stage_s, "exa")) {
            fp_stage = bcmFieldStageIngressExactMatch;
        } else if(!sal_strcasecmp(stage_s, "flowtracker") ||
                !sal_strcasecmp(stage_s, "flo")) {
            fp_stage = bcmFieldStageIngressFlowtracker;
        } else {
            return CMD_NFND;
        }
        LOG_CLI((BSL_META_U(unit,
                    "List of supported qualifiers:\n")));
        BCM_FIELD_QSET_INIT(qset);
        rv = bcm_field_stage_qset_get(unit, fp_stage, 0, &qset);
        if (rv == BCM_E_NONE) {
            for (qual=0; qual < bcmFieldQualifyCount; qual++) {
                if (BCM_FIELD_QSET_TEST(qset, qual)) {
                    LOG_CLI((BSL_META_U(unit, "%s\n"),qual_text[qual]));
                }
            }

        }
        return CMD_OK;

    }else {
        return CMD_USAGE;
    }
}

/*
 * Function:
 *      if_field_proc
 * Purpose:
 *      Manage Field Processor (FP)
 * Parameters:
 *      unit - SOC unit #
 *      args - pointer to command line arguments
 * Returns:
 *    CMD_OK
 */

cmd_result_t
if_ltsw_field_proc(int unit, args_t *args)
{
    char*                       subcmd = NULL;
#ifdef BROADCOM_DEBUG
    bcm_field_group_t           gid;
    bcm_field_entry_t           eid = 0;
#endif /* BROADCOM_DEBUG */
    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fp list ... */
    if(!sal_strcasecmp(subcmd, "list")) {
        return ltsw_fp_list(unit, args);
    }

#ifdef BROADCOM_DEBUG
    /* BCM.0> fp show ...*/
    if(!sal_strcasecmp(subcmd, "show")) {
        if ((subcmd = ARG_GET(args)) != NULL) {
            /* BCM.0> fp show entry ...*/
            if(!sal_strcasecmp(subcmd, "entry")) {
                if ((subcmd = ARG_GET(args)) == NULL) {
                    return CMD_USAGE;
                } else {
                    /* BCM.0> fp show entry 'eid' */
                    eid = parse_integer(subcmd);
                    bcm_field_entry_dump(unit, eid);
                    return CMD_OK;
                }
            }

            /* BCM.0> fp show group ...*/
            if(!sal_strcasecmp(subcmd, "group")) {
                FP_GET_NUMB(gid, subcmd, args);
                /* BCM.0> fp show group <id> brief */
                if (((subcmd = ARG_GET(args)) != NULL) &&
                    (!sal_strcasecmp(subcmd, "brief"))) {
                    bcmi_field_group_dump_brief(unit, gid);
                } else {
                    bcm_field_group_dump(unit, gid);
                }
                return CMD_OK;
            }

            /* BCM.0> fp show brief */
            if(!sal_strcasecmp(subcmd, "brief")) {
                bcm_field_show(unit, "brief");
                return CMD_OK;
            }

            /* BCM.0> fp show presel */
            if(!sal_strcasecmp(subcmd, "presel")) {
                if ((subcmd = ARG_GET(args)) == NULL) {
                    bcmi_field_presel_dump(unit, -1);
                } else {
                    /* BCM.0> fp show presel 'id' */
                    eid = parse_integer(subcmd);
                    bcmi_field_presel_dump(unit, eid);
                }
                return CMD_OK;
            }
            return CMD_NFND;

        } else {
            /* BCM.0> fp show */
            bcm_field_show(unit, "FP");
            return CMD_OK;
        }
    }
#endif /* BROADCOM_DEBUG */
    return CMD_USAGE;

}

