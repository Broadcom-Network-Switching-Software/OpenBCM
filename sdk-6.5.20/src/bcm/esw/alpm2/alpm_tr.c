/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    alpm_tr.c
 * Purpose: ALPM trace & log
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

/* ALPM trace,log */
#define ALPM_TRACE_OP_DELETE_ALL    2

typedef struct _alpm_log_s {
    uint16  is_del;
    uint16  sublen;
    int     vrf;
    int     nh_idx;
    int     rc; /* is_del returned code */
    int     seq; /* route sequence */
    uint32  flags;
    union u {
        bcm_ip_t    ip;
        bcm_ip6_t   ip6;
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
#define ALPMTR_SEQ(u)       (ALPMTR(u)->seq)
#define ALPMTR_DELETE(u)    (ALPMTR(u)->delete)
#define ALPMTR_CB(u)        (ALPMTR(u)->write_cb)
#define ALPMTR_CB_DATA(u)   (ALPMTR(u)->cb_data)

#define ALPM_TRACE_SIZE         0x400000

typedef struct _alpm_trace_s {
    int inited;
    int trace_en;
    int sanity_en;  /* trace sanity freq_cnt */
    int sanity_start;
    int sanity_end;
    int wrap_en;
    int wrap;       /* log wrapped TRUE/FALSE */
    int cnt;        /* trace logged cnt in buffer */
    int seq;        /* route trace seq number (keep increasing) */
    int delete;     /* delete cnt from last defrag on full */
    bcm_l3_alpm_trace_cb_f write_cb;
    void *cb_data;
    _alpm_log_t *buf;
    _alpm_log_t *curr;
    _alpm_log_t *start;
} _alpm_trace_t;

static _alpm_trace_t *alpm_trace[SOC_MAX_NUM_DEVICES];

/* ALPM resource usage log */
#define ALPMRES(u)              (alpm_res_log[u])
#define ALPMRES_EN(u)           (ALPMRES(u)->enable)
#define ALPMRES_BUF(u)          (ALPMRES(u)->buf)
#define ALPMRES_START(u)        (ALPMRES(u)->start)
#define ALPMRES_CURR(u)         (ALPMRES(u)->curr)

#define ALPM_RES_LOG_SIZE 1000 /* maximum resource usage log */
#define ALPM_RES_LOG_OPS  8192 /* resource usage log per each 8K route opeations */

typedef struct _alpm_res_log_s {
    int enable;     /* resource log enable */
    _alpm_resource_info_t *buf;
    _alpm_resource_info_t *curr;
    _alpm_resource_info_t *start;
} _alpm_res_log_t;

static _alpm_res_log_t *alpm_res_log[SOC_MAX_NUM_DEVICES];

const char *alpm_route_grp_name[] = {
    "IPv4 (Private) ",
    "IPv4 (Global)  ",
    "IPv4 (Override)",
    "IPv6 (Private) ",
    "IPv6 (Global)  ",
    "IPv6 (Override)",
    NULL,
};

const char *alpm_route_comb_name[] = {
    "IPv4 (Prv+Glo) ",
    "IPv4 (Prv+Glo) ",
    "IPv4 (Override)",
    "IPv6 (Prv+Glo) ",
    "IPv6 (Prv+Glo) ",
    "IPv6 (Override)",
    NULL,
};

#define ALPMTR_INIT_CHECK(u)     \
    if (ALPMTR(u) == NULL) {cli_out("alpm trace not initialized yet\n");return BCM_E_INIT;}

#define ALPM_TRACE_LOG_MAKE(u, p, is_del, cfg, nh_idx, rc)      \
    do {                                                        \
        p->is_del  = is_del;                                    \
        p->rc      = rc;                                        \
        p->seq     = ALPMTR_SEQ(u);                             \
        if (cfg) {                                              \
            p->flags   = cfg->defip_flags;                      \
            p->vrf     = cfg->defip_vrf;                        \
            p->nh_idx  = nh_idx;                                \
            p->sublen  = (uint16)(cfg->defip_sub_len & 0xffff); \
            if (cfg->defip_flags & BCM_L3_IP6) {                \
                int i;                                          \
                for (i = 0; i < 16; i++) {                      \
                    p->u.ip6[i] = cfg->defip_ip6_addr[i];       \
                }                                               \
            } else {                                            \
                p->u.ip = cfg->defip_ip_addr;                   \
            }                                                   \
        }                                                       \
    } while (0)


STATIC void alpm_trace_print(int u, int showflags, _alpm_log_t *p, char *data);

/* ALPM trace log enable */
int
bcm_esw_alpm_trace_set(int u, int val)
{
    ALPMTR_INIT_CHECK(u);

    ALPMTR_TRACE_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace wrap enable */
int
bcm_esw_alpm_trace_wrap_set(int u, int val)
{
    ALPMTR_INIT_CHECK(u);

    ALPMTR_WRAP_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace log clear */
int
bcm_esw_alpm_trace_clear(int u)
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
bcm_esw_alpm_trace_sanity(int u, int enable, int start, int end)
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

STATIC int
alpm_trace_callback(int u, int is_del, _bcm_defip_cfg_t *cfg, int nh_idx, int rc)
{
    int rv = BCM_E_NONE;
    char str[300];
    _alpm_log_t log;
    _alpm_log_t *p = &log;

    ALPM_TRACE_LOG_MAKE(u, p, is_del, cfg, nh_idx, rc);

    sal_memset(str, 0, sizeof(str));
    alpm_trace_print(u, 0, p, str);

    rv = ALPMTR_CB(u)(u, str, ALPMTR_CB_DATA(u));

    return rv;
}

/* ALPM trace log for defip route is_del (0 or 1, 2) */
int
bcm_esw_alpm_trace_log(int u, int is_del, _bcm_defip_cfg_t *cfg, int nh_idx, int rc)
{
    int alloc_sz;
    int rv = BCM_E_NONE;

    ALPMTR_INIT_CHECK(u);
    ALPMTR_SEQ(u)++; /* keep increasing trace route sequence number */
    if (is_del) {    /* count delete from last defrag on full */
        if (ALPMTR_DELETE(u) < ALPM_DEFRAG_DELETE_MIN) {
            ALPMTR_DELETE(u)++;
        }
    }
    if (ALPMTR_CB(u)) {
        (void)alpm_trace_callback(u, is_del, cfg, nh_idx, rc);
    }

    /* resource usage sampling per each 8K route operations */
    if (ALPMRES(u) && ALPMRES_EN(u) && ((ALPMTR_SEQ(u) % ALPM_RES_LOG_OPS) == 0)) {

        alpm_resource_info_get(u, ALPMRES_CURR(u));
        ALPMRES_CURR(u)++;
        if (ALPMRES_CURR(u) == &ALPMRES(u)->buf[ALPM_RES_LOG_SIZE]) {
            ALPMRES_CURR(u) = ALPMRES_BUF(u);
        }

        /* When current log reaches start log (rollover) */
        if (ALPMRES_CURR(u) == ALPMRES_START(u)) {
            ALPMRES_START(u)++; /* advancing start log */
            /* when start log reaches buf limit, it goes to buf[0] */
            if (ALPMRES_START(u) == &ALPMRES(u)->buf[ALPM_RES_LOG_SIZE]) {
                ALPMRES_START(u) = ALPMRES_BUF(u);
            }
        }
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

    ALPM_TRACE_LOG_MAKE(u, ALPMTR_CURR(u), is_del, cfg, nh_idx, rc);

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
        rv = bcm_esw_alpm_sanity_check(u, APP0, 0, 0, 1);
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
STATIC void
alpm_trace_print(int u, int showflags, _alpm_log_t *p, char *data)
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
            alpm_util_fmt_ipaddr(ip_st, p->u.ip);
            alpm_util_fmt_ipaddr(mask_st, bcm_ip_mask_create(p->sublen));
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
bcm_esw_alpm_trace_enabled(int u)
{
    if (ALPMTR(u) == NULL) {
        return 0;
    }

    return ALPMTR_TRACE_EN(u);
}

int
bcm_esw_alpm_trace_dump(int u, int showflags, bcm_esw_alpm_trace_dump_cb dump_cb, void *user_data)
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
        sal_memset(str, 0, sizeof(str));
        alpm_trace_print(u, showflags, p, str);
        (void)dump_cb(u, user_data, str);
        p++;
        if (p == &ALPMTR(u)->buf[ALPM_TRACE_SIZE]) {
            p = ALPMTR_BUF(u);
        }
    } while (p != ALPMTR_CURR(u));

done:
    return rv;
}

/* Show current ALPM resource usage */
int
alpm_resource_usage_show(int u)
{
    int rv = BCM_E_NONE;
    int grp;
    int i;

    _alpm_resource_info_t resource;
    _alpm_resource_info_t *info = &resource;

    rv = alpm_resource_info_get(u, info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Show current ALPM resource usage */
    cli_out("- Current ALPM route count & resource usage\n");
    cli_out("\t\t Routes L1_usage L2_usage L3_usage L1_used:total L2_used:total L3_used:total\n");
    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {
        int tot, usage[BCM_L3_ALPM_LEVELS];
        bcm_l3_alpm_resource_t *p = &(info->res[grp]);

        /* calculate level's usage = cnt_used / cnt_total */
        for (i = 0; i < BCM_L3_ALPM_LEVELS; i++) {
            tot = p->lvl_usage[i].cnt_total;
            if (tot == 0) {
                tot = 1; /* avoid div by 0 */
            }
            usage[i] = (p->lvl_usage[i].cnt_used * 10000) / tot; /* usage in 10000% */
        }

        cli_out("%s:%7d  %3d.%02d  %3d.%02d  %3d.%02d %6d:%6d  %6d:%6d  %6d:%6d\n",
                alpm_route_grp_name[grp],
                p->route_cnt,
                usage[0]/100, usage[0]%100,
                usage[1]/100, usage[1]%100,
                usage[2]/100, usage[2]%100,
                p->lvl_usage[0].cnt_used, p->lvl_usage[0].cnt_total,
                p->lvl_usage[1].cnt_used, p->lvl_usage[1].cnt_total,
                p->lvl_usage[2].cnt_used, p->lvl_usage[2].cnt_total);
    }

    return rv;
}

int
alpm_resource_usage_dump(int u)
{
    _alpm_resource_info_t *log, *end;
    bcm_l3_alpm_resource_t *p;
    int grp;
    int tot, i, prev_route_cnt;
    int usage[BCM_L3_ALPM_LEVELS];
    int rv = BCM_E_NONE;

    if (ALPMRES(u) == NULL) {
        return BCM_E_INIT;
    }

    if (ALPMRES_EN(u) == 0) {
        return BCM_E_DISABLED;
    }

    /* Save latest resource counters temporarily in ALPMRES_CURR(u) */
    end = ALPMRES_CURR(u);
    alpm_resource_info_get(u, end);
    end++;
    if (end == &ALPMRES(u)->buf[ALPM_RES_LOG_SIZE]) {
        end = ALPMRES_BUF(u);
    }

    cli_out("ALPM resource usage dump: "
            "L1 - TCAM entries, L2 & L3 - Bucket banks\n");
    cli_out("ALPM mode: %s\n",
            ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) ? "Combined" :
            ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL) ? "Parallel" : "TCAM Mixed");


    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {

        cli_out("%s:\n", alpm_route_grp_name[grp]);
        cli_out(" Routes L1(usage) L2(usage) L3(usage) L1(used:total) L2(used:total) L3(used:total)\n");

        prev_route_cnt = -1;

        log = ALPMRES_START(u);
        do {
            p = &(log->res[grp]);
            /* skip same route_cnt log */
            if (p->route_cnt != prev_route_cnt) {

                prev_route_cnt = p->route_cnt;

                /* calculate level's usage = cnt_used / cnt_total */
                for (i = 0; i < BCM_L3_ALPM_LEVELS; i++) {
                    tot = p->lvl_usage[i].cnt_total;
                    if (tot == 0) {
                        tot = 1; /* avoid div by 0 */
                    }
                    usage[i] = (p->lvl_usage[i].cnt_used * 10000) / tot; /* usage in 10000% */
                }

                cli_out("%7d  %3d.%02d    %3d.%02d    %3d.%02d  %6d %6d  %6d %6d  %6d  %6d\n",
                        p->route_cnt,
                        usage[0]/100, usage[0]%100,
                        usage[1]/100, usage[1]%100,
                        usage[2]/100, usage[2]%100,
                        p->lvl_usage[0].cnt_used, p->lvl_usage[0].cnt_total,
                        p->lvl_usage[1].cnt_used, p->lvl_usage[1].cnt_total,
                        p->lvl_usage[2].cnt_used, p->lvl_usage[2].cnt_total);
            }

            log++;
            if (log == &ALPMRES(u)->buf[ALPM_RES_LOG_SIZE]) {
                log = ALPMRES_BUF(u);
            }

        } while (log != end);

        cli_out("\n");
    }

    return rv;
}

int
bcm_esw_alpm_trace_cnt(int u)
{
    if (ALPMTR(u) == NULL) {
        return 0;
    }
    return ALPMTR_CNT(u);
}

int
bcm_esw_alpm_trace_seq(int u)
{
    if (ALPMTR(u) == NULL) {
        return 0;
    }
    return ALPMTR_SEQ(u);
}

int
bcm_esw_alpm_trace_delete(int u)
{
    if (ALPMTR(u) == NULL) {
        return 0;
    }
    return ALPMTR_DELETE(u);
}

void
bcm_esw_alpm_trace_delete_clear(int u)
{
    if (ALPMTR(u)) {
        ALPMTR_DELETE(u) = 0;
    }
}

int
bcm_esw_alpm_trace_init(int u)
{
    int rv = BCM_E_NONE;
    int alloc_sz;

    if (ALPMTR(u) || ALPMRES(u)) {
        (void) bcm_esw_alpm_trace_deinit(u);
    }

    /* ALPM resource usage log */
    alloc_sz = sizeof(_alpm_res_log_t);
    ALPM_ALLOC_EG(ALPMRES(u), alloc_sz, "ALPMRES");

    ALPMRES_EN(u) = soc_property_get(u, "alpm_resource_log_enable", 1);
    if (ALPMRES_EN(u)) {
        alloc_sz = sizeof(_alpm_resource_info_t) * (ALPM_RES_LOG_SIZE + 1);
        ALPM_ALLOC_EG(ALPMRES_BUF(u) , alloc_sz, "ALPMRES_BUF");
        ALPMRES_CURR(u) = ALPMRES_START(u) = ALPMRES_BUF(u); /* resource log buffer start */
    }

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

    if (ALPMRES(u) != NULL) {

        if (ALPMRES_BUF(u) != NULL) {
            alpm_util_free(ALPMRES_BUF(u));
            ALPMRES_BUF(u) = NULL;
        }

        alpm_util_free(ALPMRES(u));
        ALPMRES(u) = NULL;
    }

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

int
bcm_esw_alpm_trace_cb_register(int u,
                               bcm_l3_alpm_trace_cb_f write_cb,
                               void *user_data)
{
    int rv = BCM_E_NONE;

    L3_LOCK(u);
    if (ALPMTR(u) == NULL) {
        rv = BCM_E_INIT;
    } else {
        ALPMTR_CB_DATA(u) = user_data;
        ALPMTR_CB(u) = write_cb;
    }
    L3_UNLOCK(u);

    return rv;
}

int
bcm_esw_alpm_trace_cb_unregister(int u,
                                 bcm_l3_alpm_trace_cb_f write_cb)
{
    int rv = BCM_E_NONE;

    L3_LOCK(u);
    if (ALPMTR(u) == NULL) {
        rv = BCM_E_INIT;
    } else {
        ALPMTR_CB(u) = NULL;
        ALPMTR_CB_DATA(u) = NULL;
    }
    L3_UNLOCK(u);

    return rv;
}

#else
typedef int bcm_esw_alpm2_alpm_tr_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
