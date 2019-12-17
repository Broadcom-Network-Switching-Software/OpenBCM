/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 * File:    alpm_tr.c
 * Purpose: ALPM trace & log
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/format.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>

#include <shared/util.h>
#include <shared/l3.h>

#if defined(ALPM_ENABLE)

#include <bcm/l3.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/alpm.h>
#include <bcm_int/esw/alpm_util.h>

#include <sal/appl/io.h>

_alpm_trace_t *alpm_trace[SOC_MAX_NUM_DEVICES];

/* ALPM trace log enable */
int
alpm_trace_set(int u, int val)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    ALPMTR_TRACE_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace wrap enable */
int
alpm_trace_wrap_set(int u, int val)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    ALPMTR_WRAP_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace log clear */
int
alpm_trace_clear(int u)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    if (ALPMTR_INITED(u)) {
        ALPMTR_CNT(u) = 0;
        ALPMTR_CURR(u) = ALPMTR_START(u) = ALPMTR_BUF(u);
        cli_out("alpm trace log deleted\n");
    }

    return BCM_E_NONE;
}

/* ALPM trace periodic sanity check (route add & delete)
   enable = -1 for show trace sanity,
   enable = 0 means trace sanity check disabled
   enable > 0 means sanity check freq
*/
int
alpm_trace_sanity(int u, int enable, int start, int end)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    if (enable >= 0) {
        ALPMTR_SANITY_EN(u) = enable;
        ALPMTR_SANITY_START(u) = start;
        ALPMTR_SANITY_END(u) = end;
    }

    if (ALPMTR_SANITY_EN(u) == 0) {
        cli_out("alpm trace sanity disabled\n");
    } else {
        cli_out("alpm trace sanity enabled (enable:%d start:%d end:%d)\n",
                ALPMTR_SANITY_EN(u), ALPMTR_SANITY_START(u),
                ALPMTR_SANITY_END(u));
    }

    return BCM_E_NONE;
}

/* ALPM trace log for defip route op (ALPM_TRACE_ADD or ALPM_TRACE_DELETE) */
int
alpm_trace_log(int u, int op, _bcm_defip_cfg_t *cfg, int nh_idx, int rc)
{
    int i, alloc_sz;
    int rv = BCM_E_NONE;

    if (ALPMTR(u) == NULL) {
        return BCM_E_INIT;
    }
    if (!ALPMTR_TRACE_EN(u)) {
        return rv;
    }
    if ((!ALPMTR_WRAP_EN(u)) && ALPMTR_WRAP(u)) {
        return rv; /* if trace wrap is disabled and next log will be wrapped */
    }

    if (!ALPMTR_INITED(u)) {
        ALPMTR_CNT(u) = 0;
        /* Use ALPM_TRACE_SIZE+1 just for reaching buf limit check */
        alloc_sz = sizeof(_alpm_log_t) * (ALPM_TRACE_SIZE + 1);
        ALPM_ALLOC_EG(ALPMTR_BUF(u), alloc_sz, "alpm trace log");
        ALPMTR_CURR(u) = ALPMTR_START(u) = ALPMTR_BUF(u);
        ALPMTR_INITED(u) = TRUE;
    }

    if (ALPMTR_BUF(u) == NULL) {
        return rv;
    }

    ALPMTR_CURR(u)->op      = op;
    ALPMTR_CURR(u)->rc      = rc;
    if (cfg) {
        ALPMTR_CURR(u)->flags   = cfg->defip_flags;
        ALPMTR_CURR(u)->vrf     = cfg->defip_vrf;
        ALPMTR_CURR(u)->nh_idx  = nh_idx;
        ALPMTR_CURR(u)->sublen  = (uint16)(cfg->defip_sub_len & 0xffff);
        if (cfg->defip_flags & BCM_L3_IP6) {
            for (i = 0; i < 16; i++) {
                ALPMTR_CURR(u)->u.ip6[i] = cfg->defip_ip6_addr[i];
            }
        } else {
            ALPMTR_CURR(u)->u.ip = cfg->defip_ip_addr;
        }
    }
    /* When current log reaches start log (rollover), except
       the initial setting curr=start (without log cnt) */
    if ((ALPMTR_CURR(u) == ALPMTR_START(u)) && ALPMTR_CNT(u)) {
        ALPMTR_START(u)++; /* advancing start log */
        /* when start log reaches buf limit, it goes to buf[0] */
        if (ALPMTR_START(u) == &ALPMTR(u)->buf[ALPM_TRACE_SIZE]) {
            ALPMTR_START(u) = ALPMTR_BUF(u);
        }
    }

    ALPMTR_CURR(u)++;
    /* when current log reaches buf limit, it goes to buf[0] */
    if (ALPMTR_CURR(u) == &ALPMTR(u)->buf[ALPM_TRACE_SIZE]) {
        ALPMTR_CURR(u) = ALPMTR_BUF(u);
        ALPMTR_WRAP(u) = TRUE; /* next log will be wrapped */
    }

    ALPMTR_CNT(u)++;

    /* Periodic sanity check */
    if (ALPMTR_SANITY_EN(u) && (ALPMTR_CNT(u) >= ALPMTR_SANITY_START(u)) &&
        (((ALPMTR_CNT(u) - ALPMTR_SANITY_START(u))  % ALPMTR_SANITY_EN(u)) == 0) &&
        (ALPMTR_SANITY_END(u) <= 0 || ALPMTR_CNT(u) <= ALPMTR_SANITY_END(u))) {
        rv = bcm_esw_alpm_sanity_check(u, 0, 0, 1);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("ALPM trace sanity failed!\n"));
        }
    }

    return rv;

bad:
    if (ALPMTR_BUF(u) != NULL) {
        alpm_util_free(ALPMTR_BUF(u));
        ALPMTR_BUF(u) = NULL;
    }
    return rv;
}

/* ALPM trace log (pointer p) display in (data) string */
void
alpm_trace_print(int u, int showflags, _alpm_log_t *p, char *data)
{
    int egress_idx_min;
    char flags_st[14] = {0};
    char rc_st[20] = {0};
    char ip_st[SAL_IPADDR_STR_LEN]; /* only for IPv4 */
    char mask_st[SAL_IPADDR_STR_LEN];

    egress_idx_min = (p->flags & BCM_L3_MULTIPATH) ? /* 200000 : 100000 */
        BCM_XGS3_MPATH_EGRESS_IDX_MIN(u) : BCM_XGS3_EGRESS_IDX_MIN(u);

    rc_st[0] = 0;
    if (BCM_FAILURE(p->rc)) {
        sal_sprintf(rc_st, "; # return failed (%d)", p->rc);
    }

    flags_st[0] = 0;
    if (showflags) {
        sal_sprintf(flags_st, " f=0x%08x", p->flags);
    }

    if (p->op == ALPM_TRACE_OP_DELETE_ALL) {
        sal_sprintf(data, "l3 defip clear %s\n", rc_st);
    } else {
        if (p->flags & BCM_L3_IP6) { /* IPv6 */
            if (p->op == ALPM_TRACE_OP_ADD) { /* l3 ip6route add */
                sal_sprintf(data, "l3 ip6route add vrf=%d ip=%02x%02x:%02x%02x:%02x%02x"
                    ":%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x "
                    "masklen=%d intf=%d replace=%d ecmp=%d%s%s\n",
                    p->vrf,
                    p->u.ip6[0],
                    p->u.ip6[1],
                    p->u.ip6[2],
                    p->u.ip6[3],
                    p->u.ip6[4],
                    p->u.ip6[5],
                    p->u.ip6[6],
                    p->u.ip6[7],
                    p->u.ip6[8],
                    p->u.ip6[9],
                    p->u.ip6[10],
                    p->u.ip6[11],
                    p->u.ip6[12],
                    p->u.ip6[13],
                    p->u.ip6[14],
                    p->u.ip6[15],
                    p->sublen,
                    p->nh_idx + egress_idx_min,
                    p->flags & BCM_L3_REPLACE ? 1 : 0,
                    p->flags & BCM_L3_MULTIPATH ? 1 : 0,
                    flags_st, rc_st);
            } else { /* l3 ip6route delete */
                sal_sprintf(data, "l3 ip6route delete vrf=%d ip=%02x%02x:%02x%02x:"
                    "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                    "%02x%02x masklen=%d%s%s\n",
                    p->vrf,
                    p->u.ip6[0],
                    p->u.ip6[1],
                    p->u.ip6[2],
                    p->u.ip6[3],
                    p->u.ip6[4],
                    p->u.ip6[5],
                    p->u.ip6[6],
                    p->u.ip6[7],
                    p->u.ip6[8],
                    p->u.ip6[9],
                    p->u.ip6[10],
                    p->u.ip6[11],
                    p->u.ip6[12],
                    p->u.ip6[13],
                    p->u.ip6[14],
                    p->u.ip6[15],
                    p->sublen,
                    flags_st, rc_st);
            }
        } else { /* IPv4 */
            alpm_util_fmt_ipaddr(ip_st, p->u.ip);
            alpm_util_fmt_ipaddr(mask_st, bcm_ip_mask_create(p->sublen));
            if (p->op == ALPM_TRACE_OP_ADD) { /* l3 defip add */
                sal_sprintf(data, "l3 defip add vrf=%d ip=%s mask=%s intf=%d replace=%d ecmp=%d%s%s\n",
                    p->vrf,
                    ip_st,
                    mask_st,
                    p->nh_idx + egress_idx_min,
                    p->flags & BCM_L3_REPLACE ? 1 : 0,
                    p->flags & BCM_L3_MULTIPATH ? 1 : 0,
                    flags_st, rc_st);
            } else { /* l3 defip delete */
                sal_sprintf(data, "l3 defip delete vrf=%d ip=%s mask=%s%s%s\n",
                    p->vrf,
                    ip_st,
                    mask_st,
                    flags_st, rc_st);
           }
        }
    }
}

int
bcm_esw_alpm_trace_init(int u)
{
    int rv = BCM_E_NONE;
    int alloc_sz;

    alloc_sz = sizeof(_alpm_trace_t);
    ALPM_ALLOC_EG(ALPMTR(u), alloc_sz, "ALPMTR");

    /* Init ALPMTR variables */
    ALPMTR_TRACE_EN(u) = soc_property_get(u, "alpm_trace_enable", 0);
    ALPMTR_SANITY_EN(u) = soc_property_get(u, "alpm_trace_sanity", 0);
    ALPMTR_WRAP_EN(u) = soc_property_get(u, "alpm_trace_wrap", 0);

    ALPM_INFO(("bcm_esw_alpm_trace_init: trace %d wrap %d\n",
               ALPMTR_TRACE_EN(u), ALPMTR_WRAP_EN(u)));
    return rv;
bad:
    bcm_esw_alpm_trace_deinit(u);
    return rv;
}

int
bcm_esw_alpm_trace_deinit(int u)
{
    int rv = BCM_E_NONE;

    if (ALPMTR(u) != NULL) {

        if (ALPMTR_BUF(u) != NULL) {
            alpm_util_free(ALPMTR_BUF(u));
            ALPMTR_BUF(u) = NULL;
        }

        alpm_util_free(ALPMTR(u));
        ALPMTR(u) = NULL;
    }

    ALPM_INFO(("bcm_esw_alpm_trace_deinit!\n"));
    return rv;
}

#endif /* ALPM_ENABLE */
