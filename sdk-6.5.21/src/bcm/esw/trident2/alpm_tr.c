/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    alpm_tr.c
 * Purpose: ALPM trace & log
 */

#if defined(ALPM_ENABLE)

#include <soc/drv.h>
#include <soc/format.h>
#include <soc/alpm.h>

#include <shared/bslenum.h>

#include <bcm/l3.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/lpmv6.h>
#include <bcm/l3.h>

#include <appl/diag/system.h>

/* ALPM trace,log */
#define ALPM_TRACE_OP_DELETE_ALL    2

typedef struct _alpm_log_s {
    uint16  is_del;
    uint16  sublen;
    int     vrf;
    int     nh_idx;
    int     rc; /* op returned code */
    int     seq; /* route sequence */
    uint32  flags;
    union u {
        uint32    ip;
        uint8     ip6[16];
    } u;
} _alpm_log_t;

#define ALPMTR(u)           (alpm_trace[u])
#define ALPMTR_BUF(u)       (ALPMTR(u)->buf)
#define ALPMTR_START(u)     (ALPMTR(u)->start)
#define ALPMTR_CURR(u)      (ALPMTR(u)->curr)
#define ALPMTR_CNT(u)       (ALPMTR(u)->cnt)
#define ALPMTR_WRAP(u)      (ALPMTR(u)->wrap)
#define ALPMTR_TRACE_EN(u)  (ALPMTR(u)->trace_en)
#define ALPMTR_SANITY_EN(u) (ALPMTR(u)->sanity_en)
#define ALPMTR_SANITY_START(u) (ALPMTR(u)->sanity_start)
#define ALPMTR_SANITY_END(u) (ALPMTR(u)->sanity_end)
#define ALPMTR_WRAP_EN(u)   (ALPMTR(u)->wrap_en)
#define ALPMTR_INITED(u)    (ALPMTR(u)->inited)

#define ALPM_TRACE_SIZE         0x400000

typedef struct _alpm_trace_s {
    int inited;
    int trace_en;
    int sanity_en;  /* trace sanity freq_cnt */
    int sanity_start;
    int sanity_end;
    int wrap_en;
    int wrap;       /* log wrapped TRUE/FALSE */
    int cnt;
    _alpm_log_t *buf;
    _alpm_log_t *curr;
    _alpm_log_t *start;
} _alpm_trace_t;

static _alpm_trace_t *alpm_trace[SOC_MAX_NUM_DEVICES];

#define ALPMTR_INIT_CHECK(u)     \
    if (ALPMTR(u) == NULL) {cli_out("alpm trace not initialized yet\n");return BCM_E_INIT;}

/* ALPM trace log enable */
int
_bcm_td2_alpm_trace_set(int u, int val)
{
    ALPMTR_INIT_CHECK(u);

    ALPMTR_TRACE_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace wrap enable */
int
_bcm_td2_alpm_trace_wrap_set(int u, int val)
{
    ALPMTR_INIT_CHECK(u);

    ALPMTR_WRAP_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace log clear */
int
_bcm_td2_alpm_trace_clear(int u)
{
    ALPMTR_INIT_CHECK(u);

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
_bcm_td2_alpm_trace_sanity(int u, int enable, int start, int end)
{
    ALPMTR_INIT_CHECK(u);

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
_bcm_td2_alpm_trace_log(int u, int is_del, _bcm_defip_cfg_t *cfg, int nh_idx, int rc)
{
    int i, alloc_sz;
    int rv = BCM_E_NONE;

    ALPMTR_INIT_CHECK(u);

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
        ALPMTR_BUF(u) = sal_alloc(alloc_sz, "alpm trace log");
        if (ALPMTR_BUF(u) == NULL) {
            rv = BCM_E_MEMORY;
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u, "ALPMerr: out of memory at %s : %d\n"),
                      FUNCTION_NAME(), __LINE__));
            goto bad;
        }
        sal_memset(ALPMTR_BUF(u), 0, alloc_sz);


        ALPMTR_CURR(u) = ALPMTR_START(u) = ALPMTR_BUF(u);
        ALPMTR_INITED(u) = TRUE;
    }

    ALPMTR_CURR(u)->is_del  = is_del;
    ALPMTR_CURR(u)->rc      = rc;
    ALPMTR_CURR(u)->seq     = ALPMTR_CNT(u);
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
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TOMAHAWKX(u) || SOC_IS_APACHE(u) || SOC_IS_TRIDENT3X(u)) {
            rv = soc_th_alpm_sanity_check(u, 0, -2, 1);
        } else
#endif
        {
            rv = soc_alpm_sanity_check(u, 0, -2, 1);
        }
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u, "ALPM trace sanity failed!\n")));
        }
    }

    return rv;

bad:
    if (ALPMTR_BUF(u) != NULL) {
        sal_free(ALPMTR_BUF(u));
        ALPMTR_BUF(u) = NULL;
    }
    return rv;
}

/* ALPM trace log (pointer p) display in (data) string */
void
_bcm_td2_alpm_trace_print(int u, int showflags, _alpm_log_t *p, char *data)
{
    int egress_idx_min;
    char flags_st[14] = {0};
    char rc_st[40] = {0};
    char ip_st[SAL_IPADDR_STR_LEN]; /* only for IPv4 */
    char mask_st[SAL_IPADDR_STR_LEN];

    egress_idx_min = (p->flags & BCM_L3_MULTIPATH) ? /* 200000 : 100000 */
        BCM_XGS3_MPATH_EGRESS_IDX_MIN(u) : BCM_XGS3_EGRESS_IDX_MIN(u);

    rc_st[0] = 0;
    if (BCM_FAILURE(p->rc)) {
        sal_sprintf(rc_st, "; #seq=%d return failed (%d)", p->seq, p->rc);
    } else {
        sal_sprintf(rc_st, "; #seq=%d", p->seq);
    }

    flags_st[0] = 0;
    if (showflags) {
        sal_sprintf(flags_st, " f=0x%08x", p->flags);
    }

    if (p->is_del == ALPM_TRACE_OP_DELETE_ALL) {
        sal_sprintf(data, "l3 defip clear %s\n", rc_st);
    } else {
        if (p->flags & BCM_L3_IP6) { /* IPv6 */
            if (!p->is_del) { /* l3 ip6route add */
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
            format_ipaddr(ip_st, p->u.ip);
            format_ipaddr(mask_st, bcm_ip_mask_create(p->sublen));
            if (!p->is_del) { /* l3 defip add */
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
_bcm_td2_alpm_trace_enabled(int u)
{
    if (ALPMTR(u) == NULL) {
        return 0;
    }

    return ALPMTR_TRACE_EN(u);
}

int
_bcm_td2_alpm_trace_dump(int u, int showflags, _bcm_td2_alpm_trace_dump_cb dump_cb, void *user_data)
{
    int rv = BCM_E_NONE;
    char str[300];
    _alpm_log_t *p;

    if (ALPMTR(u) == NULL) {
        return BCM_E_INIT;
    }

    sal_memset(str, 0 ,sizeof(str));
    sal_sprintf(str, "### ALPM trace enable:%d wrap enable:%d wrap:%d count:%d showflags:%d\n",
                ALPMTR_TRACE_EN(u), ALPMTR_WRAP_EN(u),
                ALPMTR_WRAP(u), ALPMTR_CNT(u), showflags);
    (void)dump_cb(u, user_data, str);
    if (ALPMTR_CNT(u) == 0) {
        goto done;
    }

    p = ALPMTR_START(u);
    do {
        sal_memset(str, 0 ,sizeof(str));
        _bcm_td2_alpm_trace_print(u, showflags, p, str);
        (void)dump_cb(u, user_data, str);
        p++;
        if (p == &ALPMTR(u)->buf[ALPM_TRACE_SIZE]) {
            p = ALPMTR_BUF(u);
        }
    } while (p != ALPMTR_CURR(u));

done:
    return rv;
}

int
_bcm_td2_alpm_trace_cnt(int u)
{
    if (ALPMTR(u) == NULL) {
        return 0;
    }
    return ALPMTR_CNT(u);
}

int
_bcm_td2_alpm_trace_init(int u)
{
    int rv = BCM_E_NONE;
    int alloc_sz;

    if (ALPMTR(u)) {
        (void) _bcm_td2_alpm_trace_deinit(u);
    }

    alloc_sz = sizeof(_alpm_trace_t);
    ALPMTR(u) = sal_alloc(alloc_sz, "ALPMTR");
    if (ALPMTR(u) == NULL) {
        rv = BCM_E_MEMORY;
        LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,"ALPMerr: out of memory at %s : %d\n"),
                      FUNCTION_NAME(), __LINE__));
        goto bad;
    }
    sal_memset(ALPMTR(u), 0, alloc_sz);

    /* Init ALPMTR variables */
    ALPMTR_TRACE_EN(u) = soc_property_get(u, "alpm_trace_enable", 0);
    ALPMTR_SANITY_EN(u) = soc_property_get(u, "alpm_trace_sanity", 0);
    ALPMTR_WRAP_EN(u) = soc_property_get(u, "alpm_trace_wrap", 0);

    LOG_INFO(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                      "_bcm_td2_alpm_trace_init: trace %d wrap %d\n"),
                      ALPMTR_TRACE_EN(u), ALPMTR_WRAP_EN(u)));
    return rv;
bad:
    _bcm_td2_alpm_trace_deinit(u);
    return rv;
}

int
_bcm_td2_alpm_trace_deinit(int u)
{
    int rv = BCM_E_NONE;

    if (ALPMTR(u) != NULL) {

        if (ALPMTR_BUF(u) != NULL) {
            sal_free(ALPMTR_BUF(u));
            ALPMTR_BUF(u) = NULL;
        }

        sal_free(ALPMTR(u));
        ALPMTR(u) = NULL;
    }

    LOG_INFO(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,"_bcm_td2_alpm_trace_deinit!\n")));
    return rv;
}

#else
typedef int _bcm_td2_alpm2_alpm_tr_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
