/*! \file rm_alpm_ts.c
 *
 * Trouble shooting functions for alpm
 *
 * This file contains the implementation for trouble shooting
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <shr/shr_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_dump.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>
#include "rm_alpm.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_common.h"
#include "rm_alpm_traverse.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM
#define STATE_S(lpm_state, pfx) (lpm_state[pfx].start)
#define STATE_E(lpm_state, pfx) (lpm_state[pfx].end)
#define STATE_P(lpm_state, pfx) (lpm_state[pfx].prev)
#define STATE_N(lpm_state, pfx) (lpm_state[pfx].next)
#define STATE_V(lpm_state, pfx) (lpm_state[pfx].vent)
#define STATE_F(lpm_state, pfx) (lpm_state[pfx].fent)
#define STATE_H(lpm_state, pfx) (lpm_state[pfx].hent)

#define PREFIX1 "--"
#define PREFIX2 ""

#define INDENT "    "
#define PRE_STR "%s"
#define PRE_STR2 "    %s"

#define PRINT_STR_I(_s, _m) \
    shr_pb_printf(pb, "%s: %d\n", #_m, _s->_m)
#define PRINT_STR_H(_s, _m) \
    shr_pb_printf(pb, "%s: 0x%x\n", #_m, _s->_m)

#define PRINT_STR_I_PRE(_s, _m) \
    shr_pb_printf(pb, "%s%s: %d\n", prefix, #_m, _s->_m)
#define PRINT_STR_H_PRE(_pb, _s, _m) \
    shr_pb_printf(pb, "%s%s: 0x%x\n", prefix, #_m, _s->_m)

#define ALPM_TS_LOG_FILE_PREFIX "alpm_ts_log"
#define ALPM_TS_LOG_FILE_LEN 64

#define DEBUG_PB_ENTER2(_pb)              \
    bool _own_pb = false;                 \
    if (_pb == NULL) {                    \
        _pb = shr_pb_create();            \
        _own_pb = true;                   \
    }


#define DEBUG_PB_EXIT2(_pb)               \
    if (_own_pb) {                        \
        cli_out("%s", shr_pb_str(_pb));   \
        shr_pb_destroy(_pb);              \
    }

#ifdef ALPM_DEBUG_FILE
#define DEBUG_PB_ENTER(_pb)                     \
    bool _own_pb = false;                       \
    char log_file_name[ALPM_TS_LOG_FILE_LEN];   \
    if (_pb == NULL) {                          \
        _pb = shr_pb_create();                  \
        _own_pb = true;                         \
        sal_sprintf(log_file_name,              \
                    "%s_%s_%d",                 \
                    ALPM_TS_LOG_FILE_PREFIX,    \
                    BSL_FUNC,                   \
                    capture_num++);             \
        alpm_ts_start(log_file_name);           \
    }



#define DEBUG_PB_EXIT(_pb)                              \
    if (write_file) {                                   \
        alpm_ts_log_write(shr_pb_str(_pb));             \
    }                                                   \
    if (_own_pb) {                                      \
        alpm_ts_stop();                                 \
        if (!write_file) {                              \
            cli_out("%s", shr_pb_str(_pb));             \
        }                                               \
        shr_pb_destroy(_pb);                            \
    }
#else
#define DEBUG_PB_ENTER  DEBUG_PB_ENTER2
#define DEBUG_PB_EXIT   DEBUG_PB_EXIT2
#endif


/*******************************************************************************
 * Typedefs
 */

typedef struct bkt_entry_sort_helper_s {
    shr_pb_t *pb;
    ln_index_t index;
} bkt_entry_sort_helper_t;

typedef struct alpm_ts_capture_cb_s {
    /*! File descriptor of a file with write permission */
    void *fd;
    /*!
      * Write function to write a number of bytes (nbyte) from the buffer into
      * the file using the file descriptor fd. The function returns the number
      * of bytes that were actually written.
      */
    uint32_t (*write)(void *fd, void *buffer, uint32_t nbyte);
} alpm_ts_capture_cb_t;

/*! Opaque file handle */
typedef void *alpm_ts_file_handle_t;

/*******************************************************************************
 * Externs
 */

extern const cth_alpm_device_info_t *bcmptm_cth_alpm_device_info_get(int u, int m);

/*******************************************************************************
 * Private variables
 */

#ifdef ALPM_DEBUG_FILE
static int capture_num;

/* Handle of the captured file. */
static void *alpm_ts_fh = NULL;

/* True if write log to file */
static bool write_file = false;

static alpm_ts_capture_cb_t alpm_ts_cap;
#endif

static uint32_t alpm_ts_hex_buf[100] = {0};

extern alpm_internals_t
    bcmptm_rm_alpm_internals[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS][LEVELS];

/*******************************************************************************
 * Private Functions
 */
static int
xdigit2i(int digit)
{
    if (digit >= '0' && digit <= '9') return (digit - '0'     );
    if (digit >= 'a' && digit <= 'f') return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F') return (digit - 'A' + 10);
    return 0;
}

static void
reset_hex_buf(int u, int m, const char * name, uint32_t *buf)
{
    bcmdrd_sid_t sid;
    int rv;
    uint32_t entry_size;
    rv = bcmdrd_pt_name_to_sid(u, name, &sid);
    if (SHR_FAILURE(rv)) {
        return;
    }
    entry_size = bcmdrd_pt_entry_size(u, sid);
    sal_memset(buf, 0, entry_size);
}

static void
parse_hex_string(const char *str)
{
    const char     *ptr;
    int  words = 0, sum;

    if (str[0] == '0' && (str[1] == 'x' ||str[1] == 'X')) {
        ptr = str + 2;
    } else {
        ptr = str;
    }

    alpm_ts_hex_buf[words] = 0;
    while (*ptr) {
        uint8_t byte = *ptr++;
        if (byte == '_') {
            alpm_ts_hex_buf[++words] = 0;
            continue;
        }
        alpm_ts_hex_buf[words] = alpm_ts_hex_buf[words] << 4 |
                                 (xdigit2i(byte) & 0xf);
    }

    /* Reverse */
    sum = words;
    assert(sum < COUNTOF(alpm_ts_hex_buf));
    while (words > sum - words) {
        uint32_t val;
        val = alpm_ts_hex_buf[sum - words];
        alpm_ts_hex_buf[sum - words] = alpm_ts_hex_buf[words];
        alpm_ts_hex_buf[words] = val;
        words--;
    }
}

#ifdef ALPM_DEBUG_FILE

static int
alpm_ts_file_write(alpm_ts_file_handle_t fh, const void *buf, int size, int num)
{
    FILE *f = (FILE *)fh;
    int rv;

    if (fh == NULL || buf == NULL) {
        return -1;
    }

    rv = fwrite(buf, size, num, f);

    /* Test if error indicator is set */
    if (ferror(f)) {
        return -1;
    }

    return rv;
}

static int
alpm_ts_file_close(alpm_ts_file_handle_t fh)
{
    if (fh == NULL) {
        return -1;
    }
    return fclose((FILE *)fh);
}

static alpm_ts_file_handle_t
alpm_ts_file_open(const char *filename, const char *mode)
{
    if (filename == NULL || mode == NULL) {
        return NULL;
    }
    if (strlen(filename) == 0 || strlen(mode) == 0) {
        return NULL;
    }
    return (alpm_ts_file_handle_t)fopen(filename, mode);
}

static void
alpm_ts_log_write(const char *buf)
{
    void *fd;
    uint32_t buf_len = sal_strlen(buf);
    uint32_t len;

    if (!alpm_ts_cap.write) {
        return;
    }
    fd = alpm_ts_cap.fd;
    len = alpm_ts_cap.write(fd, (void*) buf, buf_len);
    if (len != buf_len) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to write (%d).\n"), len));

    }
}
static uint32_t
alpm_ts_write(void *fd, void *buffer, uint32_t nbyte)
{
    int wb;

    wb = alpm_ts_file_write(fd, buffer, 1, nbyte);

    return (wb < 0) ? 0 : wb;
}

static void
alpm_ts_cleanup(void)
{
    int rv;

    if (alpm_ts_fh) {
        rv= alpm_ts_file_close(alpm_ts_fh);
        if (rv == 0) {
            alpm_ts_fh = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Failed to close log file (%d).\n"), rv));
        }
    }

}

static void
alpm_ts_stop(void)
{
    if (alpm_ts_fh == NULL) {
        return;
    }

    alpm_ts_cap.write = 0;
    alpm_ts_cleanup();
    cli_out("Stopping log file\n");
}

static void
alpm_ts_start(const char *logfile)
{
    alpm_ts_capture_cb_t alpm_ts_cb;

    if (alpm_ts_fh != NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("ALPM TS capturing exists.\n")));
        return;
    }
    alpm_ts_fh = alpm_ts_file_open(logfile, "w");
    if (alpm_ts_fh == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to open %s\n"), logfile));
        return;
    }

    alpm_ts_cb.fd = alpm_ts_fh;
    alpm_ts_cb.write = alpm_ts_write;

    if (alpm_ts_cap.write) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("ALPM TS capturing busy.\n")));
        return;
    }
    alpm_ts_cap = alpm_ts_cb;

    cli_out("Starting log file %s\n", logfile);
}
#endif
/*!
 * \brief Dump pair tcam statistics
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
l1_stats_dump(int u, int m, uint8_t ldb, int pkm, shr_pb_t *pb, const char *prefix)
{
    int kt, vt;
    alpm_l1_info_t *l1_info;
    block_stats_t *stats;

    l1_info = bcmptm_rm_alpm_l1_info_get(u, m);
    if (!l1_info) {
        return;
    }

    stats = &l1_info->l1_db[ldb].ki[pkm].block_stats;

    shr_pb_printf(pb, PRE_STR "Key-Type  Vrf-Type   Used    Max   Half \n", prefix);
    for (kt = 0; kt < KEY_TYPES; kt++) {
        if (kt <= KEY_IPV6_QUAD) {
            for (vt = 0; vt < VRF_TYPES; vt++) {
                shr_pb_printf(pb, PRE_STR"%-8s  %-8s %5d  %5d  %5d \n", prefix,
                              bcmptm_rm_alpm_kt_name(u, m, kt),
                              bcmptm_rm_alpm_vt_name(u, m, vt),
                              stats->used_count[kt][vt],
                              stats->max_count[kt],
                              stats->half_count[kt][vt]);
            }
        } else {
            vt = VRF_DEFAULT;
            shr_pb_printf(pb, PRE_STR"%-8s  %-8s %5d  %5d  %5d \n", prefix,
                          bcmptm_rm_alpm_kt_name(u, m, kt),
                          "",
                          stats->used_count[kt][vt],
                          stats->max_count[kt],
                          stats->half_count[kt][vt]);
        }
    }
}

/*!
 * \brief Dump device info
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
static void
dev_info_dump(int u, int m, shr_pb_t *pb, const char *prefix)
{
    alpm_dev_info_t *info;

    info = bcmptm_rm_alpm_device_info_get(u, m);
    if (!info) {
        return;
    }
    shr_pb_printf(pb, PRE_STR "== ALPM Dev (Unit=%d) ==\n", prefix, u);
    shr_pb_printf(pb, PRE_STR2"max_vrf          : %d\n", prefix, info->max_vrf);
    shr_pb_printf(pb, PRE_STR2"unpair_sid       : %d\n", prefix, info->unpair_sid);
    shr_pb_printf(pb, PRE_STR2"pair_sid         : %d\n", prefix, info->pair_sid);
    shr_pb_printf(pb, PRE_STR2"unpair_sid name  : %s\n", prefix, bcmdrd_pt_sid_to_name(u, info->unpair_sid));
    shr_pb_printf(pb, PRE_STR2"pair_sid name    : %s\n", prefix, bcmdrd_pt_sid_to_name(u, info->pair_sid));
    shr_pb_printf(pb, PRE_STR2"level2 sid       : %d\n", prefix, info->l2_sid);
    shr_pb_printf(pb, PRE_STR2"level3 sid       : %d\n", prefix, info->l3_sid);
    shr_pb_printf(pb, PRE_STR2"level2 sid name  : %s\n", prefix, bcmdrd_pt_sid_to_name(u, info->l2_sid));
    shr_pb_printf(pb, PRE_STR2"level3 sid name  : %s\n", prefix, bcmdrd_pt_sid_to_name(u, info->l3_sid));
    shr_pb_printf(pb, PRE_STR2"tcam_depth       : %d\n", prefix, info->tcam_depth);
    shr_pb_printf(pb, PRE_STR2"tcam_blocks      : %d\n", prefix, info->tcam_blocks);
    shr_pb_printf(pb, PRE_STR2"feature_pkms     : %d\n", prefix, info->feature_pkms);
    shr_pb_printf(pb, PRE_STR2"feature_urpf     : %d\n", prefix, info->feature_urpf);
    shr_pb_printf(pb, PRE_STR2"pivot_max_format : %d\n", prefix, info->pivot_max_format_1);
    shr_pb_printf(pb, PRE_STR2"route_max_format : %d\n", prefix, info->route_max_format_1);
}


/*!
 * \brief Dump unpair tcam prefix states
 *
 * Follow the next index
 *
 * \param [in] u Device u.
 * \param [in] upkm unpair packing mode
 *
 * \return nothing.
 */
static void
l1_state_dump(int u, int m, int db, int pkm, shr_pb_t *pb, const char *prefix)
{
    int vent = 0;
    int hent = 0;
    int fent = 0;
    int pfx = 0;
    int i;
    block_state_t *st;
    alpm_l1_info_t *l1_info;
    int ipv;
    alpm_vrf_type_t vt;
    int len;
    uint8_t pfx_type;
    uint8_t app;


    l1_info = bcmptm_rm_alpm_l1_info_get(u, m);
    if (!l1_info) {
        return;
    }
    st = l1_info->l1_db[db].ki[pkm].block_state;

    for (i = 0; i < LAYOUT_NUM; i++) {
        pfx = l1_info->l1_db[db].ki[pkm].max_pfx[i];
        while (st && pfx >= 0) {
            bcmptm_rm_alpm_l1_pfx_decode(u, m, pfx, &ipv, &vt, &len, &pfx_type, &app);
            shr_pb_printf(pb, PRE_STR
                          "pfx=%-4d p=%-4d n=%-4d start=%-5d"
                          "end=%-5d vent=%-4d fent=%-4d hent=%d ipv=%d vt=%d len=%d app=%d\n",
                          prefix,
                          pfx,
                          STATE_P(st, pfx),
                          STATE_N(st, pfx),
                          STATE_S(st, pfx),
                          STATE_E(st, pfx),
                          STATE_V(st, pfx),
                          STATE_F(st, pfx),
                          STATE_H(st, pfx),
                          ipv, vt, len, app
                          );

            vent += STATE_V(st, pfx);
            hent += STATE_H(st, pfx);
            fent += STATE_F(st, pfx);
            pfx = STATE_N(st, pfx);
        }
    }

    shr_pb_printf(pb, PRE_STR"%d valid entries, %d free entries, %d half entries.\n",
                  prefix, vent, fent, hent);
}

/*!
 * \brief Dump unpair tcam prefix states
 *
 * Iterate all indexes
 *
 * \param [in] u Device u.
 * \param [in] pkm packing mode
 *
 * \return nothing.
 */
static void
l1_state_dump2(int u, int m, int db, int pkm, shr_pb_t *pb, const char *prefix)
{
    int vent = 0;
    int hent = 0;
    int fent = 0;
    int pfx, max_pfx;
    block_state_t *st;
    alpm_l1_info_t *l1_info;
    int ipv;
    alpm_vrf_type_t vt;
    int len;
    uint8_t pfx_type;
    uint8_t app;

    l1_info = bcmptm_rm_alpm_l1_info_get(u, m);
    if (!l1_info) {
        return;
    }


    st = l1_info->l1_db[db].ki[pkm].block_state;
    max_pfx = l1_info->l1_db[db].ki[pkm].max_pfx[LAYOUT_D];

    for (pfx = max_pfx; pfx >= 0 && st; pfx--) {
        if (STATE_V(st, pfx) != 0 || STATE_F(st, pfx) != 0 ||
            STATE_P(st, pfx) != -1 || STATE_N(st, pfx) != -1 ||
            STATE_H(st, pfx) != 0) {
            bcmptm_rm_alpm_l1_pfx_decode(u, m, pfx, &ipv, &vt, &len, &pfx_type, &app);
            shr_pb_printf(pb, PRE_STR
                          "pfx=%-4d p=%-4d n=%-4d start=%-5d"
                          "end=%-5d vent=%-4d fent=%-4d hent=%d ipv=%d vt=%d len=%d app=%d\n",
                          prefix,
                          pfx,
                          STATE_P(st, pfx),
                          STATE_N(st, pfx),
                          STATE_S(st, pfx),
                          STATE_E(st, pfx),
                          STATE_V(st, pfx),
                          STATE_F(st, pfx),
                          STATE_H(st, pfx),
                          ipv, vt, len, app
                          );
            vent += STATE_V(st, pfx);
            hent += STATE_H(st, pfx);
            fent += STATE_F(st, pfx);
        }
    }
    shr_pb_printf(pb, PRE_STR"%d valid entries, %d free entries, %d half entries.\n",
                  prefix, vent, fent, hent);
}


static void
trie_key_dump(int max_bits, trie_ip_t *trie_ip, shr_pb_t *pb,
              const char *prefix)
{
    int i;
    int key_words = max_bits / 32;

    assert(max_bits);
    shr_pb_printf(pb, "trie_key: ");

    for (i = (max_bits % 32) ? 0 : 1; i <= key_words; i++) {
        shr_pb_printf(pb, "0x%x ", trie_ip->key[i]);
    }
}

/*!
 * \brief Dump Pivot info
 *
 * \param [in] u Device u.
 * \param [in] verbose Dump more info.
 *
 * \return nothing.
 */
static void
l1_pivot_info_dump(int u, int m, int ldb, int verbose, shr_pb_t *pb, const char *prefix)
{
    int pivot_num;
    int i;
    alpm_pivot_t *pivot_info;
    alpm_l1_info_t *l1_info;
    int pkm;

    l1_info = bcmptm_rm_alpm_l1_info_get(u, m);
    if (!l1_info) {
        return;
    }
    for (pkm = 0; pkm < PKM_NUM; pkm++) {
        pivot_num = (l1_info->l1_db[ldb].ki[pkm].block_size) << 1;
    }

    for (i = 0; i < pivot_num; i++) {
        pivot_info = l1_info->pivot_info[i];
        if (!pivot_info) {
            continue;
        }

        shr_pb_printf(pb, PRE_STR \
                      "[%d] index:0x%8x ipv:%d len:%-2d w_vrf:%-4d bkt:%d\n",
                      prefix,
                      i,
                      pivot_info->index,
                      pivot_info->key.t.ipv,
                      pivot_info->key.t.len,
                      pivot_info->key.t.w_vrf,
                      pivot_info->bkt ? pivot_info->bkt->log_bkt : -1);

        if (!verbose) {
            continue;
        }

        trie_key_dump(pivot_info->key.t.max_bits,
                      &pivot_info->key.t.trie_ip,
                      pb,
                      prefix);
    }
}

static int
bkt_entry_stry_cmp(const void *x, const void *y)
{
    bkt_entry_sort_helper_t *p1 = (bkt_entry_sort_helper_t *)x;
    bkt_entry_sort_helper_t *p2 = (bkt_entry_sort_helper_t *)y;

    return (int)p1->index - (int)p2->index;
}

static void
bkt_list_dump(shr_dq_t *blist, bool verbose, shr_pb_t *pb, const char *prefix)
{
    alpm_bkt_elem_t *curr;
    shr_dq_t *elem;
    size_t bkt_count = 0, i;
#define MAX_BUCKET_COUNT 128 /* Number of entries in a logical bucket */
    bkt_entry_sort_helper_t compact_pb[MAX_BUCKET_COUNT] = {{0}};
    DEBUG_PB_ENTER2(pb);

    SHR_DQ_TRAVERSE(blist, elem)
        if (verbose) {
            curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            compact_pb[bkt_count].pb = shr_pb_create();
            compact_pb[bkt_count].index = curr->index;
            if (curr->ad) {
                shr_pb_printf(compact_pb[bkt_count].pb, PRE_STR2 "eidx=0x%-6x len=%-3d dt:%d<=%d ",
                              prefix,
                              curr->index,
                              curr->key.t.len,
                              curr->ad->user_data_type,
                              curr->ad->actual_data_type
                              );
            } else {
                shr_pb_printf(compact_pb[bkt_count].pb, PRE_STR2 "eidx=0x%-6x len=%-3d dt:n/a ",
                              prefix,
                              curr->index,
                              curr->key.t.len
                              );
            }
            trie_key_dump(curr->key.t.max_bits, &curr->key.t.trie_ip,
                          compact_pb[bkt_count].pb, prefix);
            shr_pb_printf(compact_pb[bkt_count].pb, "\n");
        }
        bkt_count++;
    SHR_DQ_TRAVERSE_END(blist, elem);

    /* Sort for side-by-side comparison by eyes */
    sal_qsort(compact_pb,
              bkt_count, /* Keep the last element in place */
              sizeof(compact_pb[0]),
              bkt_entry_stry_cmp);

    for (i = 0; i < bkt_count; i++) {
        if (verbose) {
            shr_pb_printf(pb, PRE_STR2"%s", prefix, shr_pb_str(compact_pb[i].pb));
            shr_pb_destroy(compact_pb[i].pb);
            compact_pb[i].pb = NULL;
            compact_pb[i].index = -1;
        }
    }
    /*    bcmptm_rm_alpm_trie_dump(btrie, NULL, NULL); too much info */
    shr_pb_printf(pb, PRE_STR2 "Bucket entries: %d\n", prefix, (int)bkt_count);
    DEBUG_PB_EXIT2(pb);
}

static void
elem_bkt_dump(elem_bkt_t *bkt, bool verbose, shr_pb_t *pb, const char *prefix)
{
    shr_dq_t *blist;
    alpm_route_t *route;

    blist = &bkt->list;
    route = bkt->bpm;

    shr_pb_printf(pb, PRE_STR2"Bucket: ", prefix);
    if (route) {
        shr_pb_printf(pb,  "log_bkt=%d bpm_len=%d app=%d max_bits=%d ",
                      bkt->log_bkt, route->key.t.len, route->key.t.app,
                      route->key.t.max_bits);
        if (route->key.t.len == -1) {
            shr_pb_printf(pb, "Virtual BPM");
        } else {
            trie_key_dump(route->key.t.max_bits, &route->key.t.trie_ip,
                          pb, prefix);
        }
    } else {
        shr_pb_printf(pb, "No BPM route");
    }
    shr_pb_printf(pb, "\n");

    bkt_list_dump(blist, verbose, pb, prefix);
}

static void
internals_dump(int u, int m, int db, int ln, shr_pb_t *pb, const char *prefix)
{
    alpm_internals_t *internals = &bcmptm_rm_alpm_internals[u][m][db][ln];
    shr_pb_printf(pb, PRE_STR "Internals  \n", prefix);
    shr_pb_printf(pb, PRE_STR2"read                %d \n", prefix, internals->read);
    shr_pb_printf(pb, PRE_STR2"write               %d \n", prefix, internals->write);
    shr_pb_printf(pb, PRE_STR2"split               %d \n", prefix, internals->split);
    shr_pb_printf(pb, PRE_STR2"split_write1        %d \n", prefix, internals->split_write1);
    shr_pb_printf(pb, PRE_STR2"split_write2        %d \n", prefix, internals->split_write2);
    shr_pb_printf(pb, PRE_STR2"rbkt_prepend        %d \n", prefix, internals->rbkt_prepend);
    shr_pb_printf(pb, PRE_STR2"rbkt_append         %d \n", prefix, internals->rbkt_append);
    shr_pb_printf(pb, PRE_STR2"shuffle_a           %d \n", prefix, internals->shuffle_a);
    shr_pb_printf(pb, PRE_STR2"ver0_bucket_share   %d \n", prefix, internals->ver0_bucket_share);

}

static void
ln_stats_dump(bcmptm_rm_alpm_ln_stat_t *stats, shr_pb_t *pb, const char *prefix)
{
    shr_pb_printf(pb, PRE_STR "Stats  \n", prefix);
    shr_pb_printf(pb, PRE_STR2"c_insert            %d \n", prefix, stats->c_insert);
    shr_pb_printf(pb, PRE_STR2"c_update_dt         %d \n", prefix, stats->c_update_dt);
    shr_pb_printf(pb, PRE_STR2"c_update            %d \n", prefix, stats->c_update);
    shr_pb_printf(pb, PRE_STR2"c_delete            %d \n", prefix, stats->c_delete);
    shr_pb_printf(pb, PRE_STR2"c_peak              %d \n", prefix, stats->c_peak);
    shr_pb_printf(pb, PRE_STR2"c_last_peak         %d \n", prefix, stats->c_last_peak);
    shr_pb_printf(pb, PRE_STR2"c_last_full         %d \n", prefix, stats->c_last_full);
    shr_pb_printf(pb, PRE_STR2"c_curr              %d \n", prefix, stats->c_curr);
    shr_pb_printf(pb, PRE_STR2"c_traverse_merge    %d \n", prefix, stats->c_traverse_merge);
}

static void
rbkt_pivot_dump(alpm_pivot_t *pivot, bool verbose, shr_pb_t *pb,
                const char *prefix)
{
    shr_pb_printf(pb, PRE_STR2 "Pivot: ", prefix);
    shr_pb_printf(pb, "pivot_index=0x%-6x pivot_len=%-3d ",
                  pivot->index,
                  pivot->key.t.len
                  );
    trie_key_dump(pivot->key.t.max_bits, &pivot->key.t.trie_ip, pb, prefix);
    shr_pb_printf(pb, PRE_STR "\n", prefix);
}

static void
rbkt_group_brief_dump(int u, int m, rbkt_t *rbkt, shr_pb_t *pb,
                      format_type_t format_type, const char *prefix,
                      int *o_valid_entries, int *o_group_entries)
{
    int i, num_entry;
    int valid_entries = 0;
    int group_entries = 0;
    rbkt_t *rbkt2;
    rbkt2 = rbkt;
    for (i = 0; i < rbkt->valid_banks; i++) {
        valid_entries += shr_util_popcount(rbkt2->entry_bitmap);
        (void)bcmptm_rm_alpm_ln_base_entry_info(u, m, format_type, rbkt2->format,
                                                NULL, NULL, &num_entry);
        group_entries += num_entry;
        rbkt2++;
    }
    shr_pb_printf(pb, PRE_STR2 "g=%-6d : ves=%-12d / mes=%-12d\n",
                  prefix,
                  rbkt->index,
                  valid_entries,
                  group_entries);
    *o_valid_entries += valid_entries;
    *o_group_entries += group_entries;
}

static void
rbkt_group_dump(rbkt_t *rbkt, bool verbose, shr_pb_t *pb, const char *prefix)
{
    alpm_pivot_t *pivot;
    int i;
    int vbanks;
    rbkt_t *rbkt2;
    shr_pb_printf(pb, PRE_STR2 "g=%-6d ve=0x%-4x fmt=%-2d h=%d vbs=%-1d fbs=%-2d\n",
                  prefix,
                  rbkt->index,
                  rbkt->entry_bitmap,
                  rbkt->format,
                  rbkt->is_head,
                  rbkt->valid_banks,
                  rbkt->free_banks);
    vbanks = rbkt->valid_banks;
    rbkt2 = rbkt;
    for (i = 1; i < vbanks; i++) {
        rbkt2++;
        shr_pb_printf(pb, PRE_STR2 "i=%-6d ve=0x%-4x fmt=%-2d h=%d vbs=%-1d fbs=%-2d\n",
                      prefix,
                      rbkt2->index,
                      rbkt2->entry_bitmap,
                      rbkt2->format,
                      rbkt2->is_head,
                      rbkt2->valid_banks,
                      rbkt2->free_banks);
    }
    pivot = rbkt->pivot;
    if (pivot) {
        rbkt_pivot_dump(pivot, verbose, pb, PREFIX2);
        elem_bkt_dump(pivot->bkt, verbose, pb, PREFIX2);
    }
    for (i = 0; i < MAX_SUB_BKTS; i++) {
        pivot = rbkt->pivots[i];
        if (pivot) {
            shr_pb_printf(pb, PRE_STR2 "Pivot info (sub=%d):\n", prefix, i);
            rbkt_pivot_dump(pivot, verbose, pb, PREFIX2);
            elem_bkt_dump(pivot->bkt, verbose, pb, PREFIX2);
        }
    }
    shr_pb_printf(pb, "\n");
}

static void
rbkt_group_list_dump(bkt_hdl_t *bkt_hdl, bool verbose, shr_pb_t *pb, const char *prefix)
{
    shr_dq_t *elem;
    rbkt_t *curr;
    int v_rbkts = 0, f_rbkts = 0;
    SHR_DQ_TRAVERSE(&bkt_hdl->global_group_list, elem)
        curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        v_rbkts += curr->valid_banks;
        f_rbkts += curr->free_banks;
        rbkt_group_dump(curr, verbose, pb, prefix);
    SHR_DQ_TRAVERSE_END(&bkt_hdl->global_group_list, elem);
    shr_pb_printf(pb, PRE_STR2 "Total v_rbkts: %d | f_rbkts: %d \n",
                  prefix, v_rbkts, f_rbkts);
}

static void
rbkt_group_list_brief_dump(int u, int m, bkt_hdl_t *bkt_hdl, shr_pb_t *pb, const char *prefix, bool verbose)
{
    shr_dq_t *elem;
    rbkt_t *curr;
    int v_rbkts = 0, f_rbkts = 0;
    int valid_entries = 0;
    int all_entries = 0;
    shr_pb_t *pb2 = shr_pb_create();

    shr_pb_printf(pb, PRE_STR "\nAll bucket groups brief\n", prefix);
    shr_pb_printf(pb2, PRE_STR2 "Group    : Valid entries     / Maximal entries      \n", prefix);
    SHR_DQ_TRAVERSE(&bkt_hdl->global_group_list, elem)
        curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        v_rbkts += curr->valid_banks;
        f_rbkts += curr->free_banks;
        rbkt_group_brief_dump(u, m, curr, pb2, bkt_hdl->format_type, prefix,
                              &valid_entries, &all_entries);
    SHR_DQ_TRAVERSE_END(&bkt_hdl->global_group_list, elem);
    if (verbose) {
        shr_pb_printf(pb, "%s\n", shr_pb_str(pb2));
    }
    shr_pb_printf(pb, PRE_STR2 "Valid_rbkts=%d Free_rbkts=%d Max_rbkts=%d Valid/Max=%d%%\n",
                  prefix, v_rbkts, f_rbkts, bkt_hdl->rbkts_total,
                  v_rbkts * 100 / bkt_hdl->rbkts_total);
    shr_pb_printf(pb, PRE_STR2 "Valid_entries=%d Max_entries=%d Valid/Max=%d%% (Accounting valid rbkts only)\n",
                  prefix, valid_entries, all_entries,
                  all_entries ? (valid_entries * 100 / all_entries) : 0);
}

static void
rbkt_group_usg_list_dump(shr_dq_t *glist, bool verbose, shr_pb_t *pb,
                         const char *prefix)
{
    shr_dq_t *elem;
    rbkt_t *curr;
    int v_rbkts = 0, f_rbkts = 0;
    SHR_DQ_TRAVERSE(glist, elem)
        curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
        v_rbkts += curr->valid_banks;
        f_rbkts += curr->free_banks;
        rbkt_group_dump(curr, verbose, pb, prefix);
    SHR_DQ_TRAVERSE_END(glist, elem);
    shr_pb_printf(pb, PRE_STR2 "Total v_rbkts: %d | f_rbkts: %d \n",
                  prefix, v_rbkts, f_rbkts);
}

static void
rbkt_fmt_dist_dump(int u, int m, int db, int ln, shr_pb_t *pb, const char *prefix)
{
    int i, vbanks;
    int fmt_cnt[32] = {0};
    int fmt_cnt_all = 0;
    shr_dq_t *elem;
    bkt_hdl_t *bkt_hdl;
    rbkt_t *curr;

    DEBUG_PB_ENTER(pb);
    bcmptm_rm_alpm_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    if (bkt_hdl) {
        SHR_DQ_TRAVERSE(&bkt_hdl->global_group_list, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
            vbanks = curr->valid_banks;
            if (vbanks == 0) {
                continue;
            }
            fmt_cnt[curr->format] += curr->valid_banks;
        SHR_DQ_TRAVERSE_END(&bkt_hdl->global_group_list, elem);
    }

    shr_pb_printf(pb, "\nAll bank format distribution [DB=%d Level=%d]\n", db, ln + 1);
    for (i = 0; i < 32; i++) {
        fmt_cnt_all += fmt_cnt[i];
    }

    if (fmt_cnt_all == 0) {
        shr_pb_printf(pb, PRE_STR2 "<All Zero>\n", prefix);
    } else {
        for (i = 0; i < 32; i++) {
            if (fmt_cnt[i] == 0) {
                continue;
            }
            shr_pb_printf(pb, PRE_STR2 "FMT %2d : %5d (%2d.%02d%%)\n", prefix, i,
                          fmt_cnt[i],
                          fmt_cnt[i] * 100 / fmt_cnt_all,
                          fmt_cnt[i] * 100 % fmt_cnt_all);
        }
    }
    DEBUG_PB_EXIT(pb);
}

static void
bkt_hdl_dump(bkt_hdl_t *bkt_hdl, shr_pb_t *pb, const char *prefix)
{
    shr_pb_printf(pb, PRE_STR "Bucket handle  \n", prefix);
    shr_pb_printf(pb, PRE_STR2"rbkts_total         %d\n", prefix, bkt_hdl->rbkts_total);
    shr_pb_printf(pb, PRE_STR2"rbkts_used          %d\n", prefix, bkt_hdl->rbkts_used);
    shr_pb_printf(pb, PRE_STR2"rbkts utilization   %d%%\n", prefix, bkt_hdl->rbkts_used * 100 / bkt_hdl->rbkts_total);
    shr_pb_printf(pb, PRE_STR2"max_banks           %d\n", prefix, bkt_hdl->max_banks);
    shr_pb_printf(pb, PRE_STR2"bucket_bits         %d\n", prefix, bkt_hdl->bucket_bits);
    shr_pb_printf(pb, PRE_STR2"format_type         %d\n", prefix, bkt_hdl->format_type);
    shr_pb_printf(pb, PRE_STR2"groups_used         %d\n", prefix, bkt_hdl->groups_used);
    shr_pb_printf(pb, PRE_STR2"start_bank          %d\n", prefix, bkt_hdl->start_bank);
    shr_pb_printf(pb, PRE_STR2"last_bank           %d\n", prefix, bkt_hdl->last_bank);
    shr_pb_printf(pb, PRE_STR2"base_rbkt           %d\n", prefix, bkt_hdl->base_rbkt);
    shr_pb_printf(pb, PRE_STR2"thrd_banks          %d\n", prefix, bkt_hdl->thrd_banks);
    shr_pb_printf(pb, PRE_STR2"bank_bitmap       0x%x\n", prefix, bkt_hdl->bank_bitmap);
}


static void
ln_info_dump(int u, int m, int db, shr_pb_t *pb, const char *prefix, int verbose, int bkt_seq, int brief)
{
    bcmptm_rm_alpm_ln_stat_t *stat;
    int level;

    /* Start from Level2 */
    alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, m, db);
    for (level = LEVEL2; level < database_ptr->max_levels; level++) {
        lvl_hdl_t *lvl_hdl;
        if (bcmptm_rm_alpm_lvl_hdl_get(u, m, db, level,
                                          &lvl_hdl) == SHR_E_NOT_FOUND) {
            continue;
        }
        shr_pb_printf(pb, "\n== LN_INFO (Unit=%d DB=%d Level=%d) ==\n", u, db, lvl_hdl->lvl_idx + 1);
        stat = bcmptm_rm_alpm_ln_stats_get(u, m, db);
        if (stat) {
            ln_stats_dump(stat, pb, "");
        }
        internals_dump(u, m, db, level, pb, "");

        if (lvl_hdl->bkt_hdl) {
            bkt_hdl_dump(lvl_hdl->bkt_hdl, pb, "");
        } else {
            shr_pb_printf(pb, INDENT"bkt_hdl not found \n");
        }
        bcmptm_rm_alpm_buckets_brief_dump(u, m, db, level, pb, verbose);

        /* Collect format information */
        rbkt_fmt_dist_dump(u, m, db, level, pb, "");

        if (brief) {
            continue;
        }

        if (bkt_seq == 0) {
            bcmptm_rm_alpm_buckets_dump(u, m, db, level, pb, verbose);
        } else {
            bcmptm_rm_alpm_buckets_dump2(u, m, db, level, pb, verbose);
        }
    }

}

/*!
 * \brief Dump Level-1 info
 *
 * \param [in] u Device u.
 * \param [in] verbose Dump more info.
 *
 * \return nothing.
 */
static void
l1_info_dump(int u, int m, uint8_t ldb, bool verbose, shr_pb_t *pb, const char *prefix)
{
    alpm_l1_info_t *info;
    l1_db_info_t *l1_db;
    l1_key_input_info_t *l1_ki;
    int pkm;
    int state_flag = 1;

    info = bcmptm_rm_alpm_l1_info_get(u, m);
    if (!info) {
        return;
    }
#define L1_INFO_LINE1 "    %-22s %-6d\n"
#define L1_INFO_LINE4
#define L1_INFO_LINE5
    shr_pb_printf(pb, "\n== L1_INFO (Unit=%d DB=%d) ==\n",
                  u, ldb);

    l1_db = &info->l1_db[ldb];
    if (!l1_db->valid) {
        shr_pb_printf(pb, "Empty DB, skip.\n");
        return;
    }

    for (pkm = PKM_NUM - 1; pkm >= 0; pkm--) {
        l1_ki = &l1_db->ki[pkm];
        if (!l1_ki->valid) {
            continue;
        }
        shr_pb_printf(pb, "PKM %s basic\n", bcmptm_rm_alpm_pkm_name(u, m, pkm));
        shr_pb_printf(pb, L1_INFO_LINE1, "blocks", l1_ki->blocks);
        shr_pb_printf(pb, L1_INFO_LINE1, "size", l1_ki->block_size);
        shr_pb_printf(pb, L1_INFO_LINE1, "start", l1_ki->block_start);
        shr_pb_printf(pb, L1_INFO_LINE1, "end", l1_ki->block_end);
        if (bcmptm_rm_alpm_pkm_is_pair(u, m, pkm)) {
            shr_pb_printf(pb, L1_INFO_LINE1, "range1_first", l1_ki->range1_first);
            shr_pb_printf(pb, L1_INFO_LINE1, "range2_first", l1_ki->range2_first);
            shr_pb_printf(pb, L1_INFO_LINE1, "range1_last", l1_ki->range1_last);
            shr_pb_printf(pb, L1_INFO_LINE1, "range1_last_pfx", l1_ki->range1_last_pfx);
            shr_pb_printf(pb, L1_INFO_LINE1, "last_pfx_span", l1_ki->last_pfx_span);

            shr_pb_printf(pb, L1_INFO_LINE1, "max_pfx_d", l1_ki->max_pfx[LAYOUT_D]);
            shr_pb_printf(pb, L1_INFO_LINE1, "max_pfx_sh", l1_ki->max_pfx[LAYOUT_SH]);
            shr_pb_printf(pb, L1_INFO_LINE1, "base_pfx_d", l1_ki->base_pfx[LAYOUT_D]);
            shr_pb_printf(pb, L1_INFO_LINE1, "base_pfx_sh", l1_ki->base_pfx[LAYOUT_SH]);

            shr_pb_printf(pb, "PKM %s state\n", bcmptm_rm_alpm_pkm_name(u, m, pkm));
            if (state_flag) {
                l1_state_dump(u, m, ldb, pkm, pb, INDENT);
            } else {
                l1_state_dump2(u, m, ldb, pkm, pb, INDENT);
            }
            shr_pb_printf(pb, "PKM %s stats\n", bcmptm_rm_alpm_pkm_name(u, m, pkm));
            l1_stats_dump(u, m, ldb, pkm, pb, INDENT);
        } else {
            shr_pb_printf(pb, L1_INFO_LINE1, "max_pfx", l1_ki->max_pfx[LAYOUT_D]);
            shr_pb_printf(pb, L1_INFO_LINE1, "base_pfx", l1_ki->base_pfx[LAYOUT_SH]);

            shr_pb_printf(pb, "PKM %s state\n", bcmptm_rm_alpm_pkm_name(u, m, pkm));
            if (state_flag) {
                l1_state_dump(u, m, ldb, pkm, pb, INDENT);
            } else {
                l1_state_dump2(u, m, ldb, pkm, pb, INDENT);
            }
            shr_pb_printf(pb, "PKM %s stats\n", bcmptm_rm_alpm_pkm_name(u, m, pkm));
            l1_stats_dump(u, m, ldb, pkm, pb, INDENT);
        }
    }
    internals_dump(u, m, ldb, LEVEL1, pb, "");
}

static void
l1_config_show(int u, int m, shr_pb_t *pb)
{
    int d, db, ki, i;
    int l1_blk_cnt, l1_bnk_cnt, l1_row_pblk;
    int db_lvl[8] = {0};
    int use_dev_info = 0;
    char *ki_str[] = {
        /* See bcmptm_cth_alpm_be_internal.h */
        "LPM_DST_QW",
        "LPM_DST_DW",
        "LPM_DST_SW",
        "LPM_SRC_QW",
        "LPM_SRC_DW",
        "LPM_SRC_SW",
        "FP_DST",
        "FP_SRC",
        "IPMC_QW",
        "IPMC_DW",
        "IPMC_SW",
    };

    bcmptm_cth_alpm_control_t alpm_ctrl;
    const cth_alpm_device_info_t *alpm_dev;
    alpm_dev_info_t *dev_info;
    bcmdrd_sid_t sid = 0;

    (void)bcmptm_cth_alpm_control_get(u, m, &alpm_ctrl);
    alpm_dev = bcmptm_cth_alpm_device_info_get(u, m);
    dev_info = bcmptm_rm_alpm_device_info_get(u, m);

    db_lvl[0] = alpm_ctrl.db0_levels;
    db_lvl[1] = alpm_ctrl.db1_levels;
    db_lvl[2] = alpm_ctrl.db2_levels;
    db_lvl[3] = alpm_ctrl.db3_levels;

    l1_blk_cnt = alpm_dev->spec.l1_blocks;
    l1_bnk_cnt = alpm_ctrl.tot.num_l1_banks;
    l1_row_pblk = l1_bnk_cnt / l1_blk_cnt;
    shr_pb_printf(pb, "L1 table config:\n");
    shr_pb_printf(pb, "%5d |----------|----------------|---------------------------|\n", 0);
    for (d = 0; d < l1_blk_cnt; d++) {
        char db_col_str[5][32] = {{0}};
        char ki_col_str[5][32] = {{0}};
        char ix_col_str[5][32] = {{0}};
        const char *sn_col_str[5];
        const char *null_str = "                           ";
        const char *dash_str = "---------------------------";
        db = alpm_ctrl.l1_db[d];
        ki = alpm_ctrl.l1_key_input[d];

        sal_memset(db_col_str, 0, sizeof(db_col_str));
        sal_memset(ki_col_str, 0, sizeof(ki_col_str));
        sal_memset(ix_col_str, 0, sizeof(ix_col_str));
        sal_sprintf(db_col_str[0], "%s", "          ");
        sal_sprintf(db_col_str[1], "%s", "          ");
        sal_sprintf(db_col_str[2], " DB%-2d(%d)  ", db, db_lvl[db]);
        sal_sprintf(db_col_str[3], "%s", "          ");
        sal_sprintf(db_col_str[4], "%s", "----------");

        sal_sprintf(ki_col_str[0], "%s", "                ");
        sal_sprintf(ki_col_str[1], " KEY_INPUT      ");
        sal_sprintf(ki_col_str[2], " %-10s(%2d) ", ki_str[ki], ki);
        sal_sprintf(ki_col_str[3], "%s", "                ");
        sal_sprintf(ki_col_str[4], "%s", "----------------");

        sal_sprintf(ix_col_str[0], "%s", "");
        sal_sprintf(ix_col_str[1], "%s", "");
        sal_sprintf(ix_col_str[2], "%s", "");
        sal_sprintf(ix_col_str[3], "%s", "");
        sal_sprintf(ix_col_str[4], "%5d",
                    ((d + 1) * alpm_dev->spec.tcam_blocks * alpm_dev->spec.tcam_depth / l1_blk_cnt) - 1);

        if (alpm_dev->spec.l1_sid[0] == 0 || alpm_dev->spec.l1_sid[0] == INVALIDm) {
            use_dev_info = 1;
            if (ki == ALPM_KEY_INPUT_LPM_DST_Q ||
                ki == ALPM_KEY_INPUT_LPM_SRC_Q ||
                ki == ALPM_KEY_INPUT_LPM_L3MC_Q) {
                sid = dev_info->pair_sid;
            } else {
                sid = dev_info->unpair_sid;
            }
        }

        if (l1_row_pblk <= 1) {
            sn_col_str[0] = null_str;
            sn_col_str[1] = null_str;
            sn_col_str[3] = null_str;
            sn_col_str[2] = bcmdrd_pt_sid_to_name(u,
                                use_dev_info ? sid : alpm_dev->spec.l1_sid[d * l1_row_pblk]);
        } else if (l1_row_pblk <= 2) {
            sn_col_str[0] = bcmdrd_pt_sid_to_name(u,
                                use_dev_info ? sid : alpm_dev->spec.l1_sid[d * l1_row_pblk + 0]);
            sn_col_str[1] = null_str;
            sn_col_str[2] = bcmdrd_pt_sid_to_name(u,
                                use_dev_info ? sid : alpm_dev->spec.l1_sid[d * l1_row_pblk + 1]);
            sn_col_str[3] = null_str;
        } else if (l1_row_pblk <= 4) {
            sn_col_str[0] = bcmdrd_pt_sid_to_name(u,
                                use_dev_info ? sid : alpm_dev->spec.l1_sid[d * l1_row_pblk + 0]);
            sn_col_str[1] = bcmdrd_pt_sid_to_name(u,
                                use_dev_info ? sid : alpm_dev->spec.l1_sid[d * l1_row_pblk + 1]);
            sn_col_str[2] = bcmdrd_pt_sid_to_name(u,
                                use_dev_info ? sid : alpm_dev->spec.l1_sid[d * l1_row_pblk + 2]);
            sn_col_str[3] = bcmdrd_pt_sid_to_name(u,
                                use_dev_info ? sid : alpm_dev->spec.l1_sid[d * l1_row_pblk + 3]);
        } else {
            /* assert(0) */
        }
        sn_col_str[4] = dash_str;

        for (i = 0; i < 5; i++) {
            shr_pb_printf(pb, "%5s |%s|%s|%-27s|\n",
                          ix_col_str[i], db_col_str[i], ki_col_str[i], sn_col_str[i]);
        }
    }

    return ;
}


static void
l2_config_show(int u, int m, shr_pb_t *pb)
{
    int i;
    int l2_bkt_cnt;
    int db_lvl[8] = {0};

    bcmptm_cth_alpm_control_t alpm_ctrl;
    const cth_alpm_device_info_t *alpm_dev;

    (void)bcmptm_cth_alpm_control_get(u, m, &alpm_ctrl);
    alpm_dev = bcmptm_cth_alpm_device_info_get(u, m);

    db_lvl[0] = alpm_ctrl.db0_levels;
    db_lvl[1] = alpm_ctrl.db1_levels;
    db_lvl[2] = alpm_ctrl.db2_levels;
    db_lvl[3] = alpm_ctrl.db3_levels;

    shr_pb_printf(pb, "\nL2 table config:\n");
    l2_bkt_cnt = 1 << alpm_ctrl.tot.l2_bucket_bits;
    if (alpm_dev->spec.l2_sid[0] != INVALIDm) {
        shr_pb_printf(pb, "%s", bcmdrd_pt_sid_to_name(u, alpm_dev->spec.l2_sid[0]));
    }
    for (i = 0; i < ALPM_LN_BANKS_MAX; i++) {
        if (alpm_dev->spec.l2_sid[i] == INVALIDm) {
            break;
        }
    }
    if ((i - 1) != 0) {
        shr_pb_printf(pb, " ... %s\n", bcmdrd_pt_sid_to_name(u, alpm_dev->spec.l2_sid[i - 1]));
    } else {
        shr_pb_printf(pb, "\n");
    }

    if (db_lvl[0] < 2 && db_lvl[1] < 2 && db_lvl[2] < 2 && db_lvl[3] < 2) {
        return;
    }

    shr_pb_printf(pb, "%5s  ", "");
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        shr_pb_printf(pb, "  B%d  ", i);
    }
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "%5d |", 0);
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        shr_pb_printf(pb, "-----|");
    }
    shr_pb_printf(pb, "\n");

    /* blank line */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        shr_pb_printf(pb, "     |");
    }
    shr_pb_printf(pb, "\n");

    /* DB0 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        if (alpm_ctrl.db0.l2_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB0 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");
    /* DB1 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        if (alpm_ctrl.db1.l2_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB1 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");
    /* DB2 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        if (alpm_ctrl.db2.l2_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB2 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");
    /* DB3 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        if (alpm_ctrl.db3.l2_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB3 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");

    /* blank line */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        shr_pb_printf(pb, "     |");
    }
    shr_pb_printf(pb, "\n");

    shr_pb_printf(pb, "%5d |", l2_bkt_cnt - 1);
    for (i = 0; i < alpm_ctrl.tot.num_l2_banks; i++) {
        shr_pb_printf(pb, "-----|");
    }
    shr_pb_printf(pb, "\n");

    return ;
}


static void
l3_config_show(int u, int m, shr_pb_t *pb)
{
    int i;
    int l3_bkt_cnt;
    int db_lvl[8] = {0};

    bcmptm_cth_alpm_control_t alpm_ctrl;
    const cth_alpm_device_info_t *alpm_dev;

    (void)bcmptm_cth_alpm_control_get(u, m, &alpm_ctrl);
    alpm_dev = bcmptm_cth_alpm_device_info_get(u, m);

    db_lvl[0] = alpm_ctrl.db0_levels;
    db_lvl[1] = alpm_ctrl.db1_levels;
    db_lvl[2] = alpm_ctrl.db2_levels;
    db_lvl[3] = alpm_ctrl.db3_levels;

    shr_pb_printf(pb, "\nL3 table config:\n");
    l3_bkt_cnt = 1 << alpm_ctrl.tot.l3_bucket_bits;
    if (alpm_dev->spec.l3_sid[0] != INVALIDm) {
        shr_pb_printf(pb, "%s", bcmdrd_pt_sid_to_name(u, alpm_dev->spec.l3_sid[0]));
    }
    for (i = 0; i < ALPM_LN_BANKS_MAX; i++) {
        if (alpm_dev->spec.l3_sid[i] == INVALIDm) {
            break;
        }
    }
    if ((i - 1) != 0) {
        shr_pb_printf(pb, " ... %s\n", bcmdrd_pt_sid_to_name(u, alpm_dev->spec.l3_sid[i - 1]));
    } else {
        shr_pb_printf(pb, "\n");
    }

    if (db_lvl[0] < 3 && db_lvl[1] < 3 && db_lvl[2] < 3 && db_lvl[3] < 3) {
        return;
    }

    shr_pb_printf(pb, "%5s  ", "");
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        shr_pb_printf(pb, "  B%d  ", i);
    }
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "%5d |", 0);
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        shr_pb_printf(pb, "-----|");
    }
    shr_pb_printf(pb, "\n");

    /* blank line */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        shr_pb_printf(pb, "     |");
    }
    shr_pb_printf(pb, "\n");

    /* DB0 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        if (alpm_ctrl.db0.l3_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB0 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");
    /* DB1 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        if (alpm_ctrl.db1.l3_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB1 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");
    /* DB2 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        if (alpm_ctrl.db2.l3_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB2 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");
    /* DB3 */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        if (alpm_ctrl.db3.l3_bank_bitmap & (1 << i)) {
            shr_pb_printf(pb, " DB3 |");
        } else {
            shr_pb_printf(pb, "     |");
        }
    }
    shr_pb_printf(pb, "\n");

    /* blank line */
    shr_pb_printf(pb, "%5s |", "");
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        shr_pb_printf(pb, "     |");
    }
    shr_pb_printf(pb, "\n");

    shr_pb_printf(pb, "%5d |", l3_bkt_cnt - 1);
    for (i = 0; i < alpm_ctrl.tot.num_l3_banks; i++) {
        shr_pb_printf(pb, "-----|");
    }
    shr_pb_printf(pb, "\n");

    return ;
}


/*******************************************************************************
 * Public Functions
 */
void
bcmptm_rm_alpm_config_show(int u, int m, shr_pb_t *pb)
{
    l1_config_show(u, m, pb);
    l2_config_show(u, m, pb);
    l3_config_show(u, m, pb);
    return ;
}

void
bcmptm_rm_alpm_l1_pivot_info_dump(int u, int m, uint8_t ldb, shr_pb_t *pb)
{
    DEBUG_PB_ENTER(pb);
    l1_pivot_info_dump(u, m, ldb, 0, pb, "");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_pivot_trie_dump(int u, int m, int db, int ln, int w_vrf,
                               int ipv, shr_pb_t *pb)
{
    rm_alpm_trie_t *pivot_trie;
    DEBUG_PB_ENTER(pb);

    (void) bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln, w_vrf,
                                       ipv, &pivot_trie);
    if (pivot_trie) {
        bcmptm_rm_alpm_trie_dump(pivot_trie, NULL, NULL);
    }

    DEBUG_PB_EXIT(pb);
}

static void
rm_alpm_config_bank_dump(shr_pb_t *pb, bcmptm_cth_alpm_control_bank_t *bank, const char *prefix)
{
    shr_pb_printf(pb, PRE_STR2"l1_bank_bitmap           0x%x \n", prefix, bank->l1_bank_bitmap);
    shr_pb_printf(pb, PRE_STR2"l2_bank_bitmap           0x%x \n", prefix, bank->l2_bank_bitmap);
    shr_pb_printf(pb, PRE_STR2"l3_bank_bitmap           0x%x \n", prefix, bank->l3_bank_bitmap);
    shr_pb_printf(pb, PRE_STR2"num_l1_banks             %d \n", prefix, bank->num_l1_banks);
    shr_pb_printf(pb, PRE_STR2"num_l2_banks             %d \n", prefix, bank->num_l2_banks);
    shr_pb_printf(pb, PRE_STR2"num_l3_banks             %d \n", prefix, bank->num_l3_banks);
    shr_pb_printf(pb, PRE_STR2"l2_bucket_bits           %d \n", prefix, bank->l2_bucket_bits);
    shr_pb_printf(pb, PRE_STR2"l3_bucket_bits           %d \n", prefix, bank->l3_bucket_bits);
}

static void
config_dump(int u, int m, int db, shr_pb_t *pb, alpm_info_t *info, const char *prefix)
{
    shr_pb_printf(pb, PRE_STR"\n== ALPM Config (Unit=%d DB=%d) ==\n", prefix, u, db);
    shr_pb_printf(pb, PRE_STR2"Core:alpm_mode               %d\n", prefix, info->config.core.alpm_mode);
    shr_pb_printf(pb, PRE_STR2"Core:alpm_dbs                %d\n", prefix, info->config.core.alpm_dbs);
    shr_pb_printf(pb, PRE_STR2"Core:db_levels[DB0]          %d\n", prefix, info->config.core.db_levels[DB0]);
    shr_pb_printf(pb, PRE_STR2"Core:db_levels[DB1]          %d\n", prefix, info->config.core.db_levels[DB1]);
    shr_pb_printf(pb, PRE_STR2"Core:db_levels[DB2]          %d\n", prefix, info->config.core.db_levels[DB2]);
    shr_pb_printf(pb, PRE_STR2"Core:db_levels[DB3]          %d\n", prefix, info->config.core.db_levels[DB3]);
    shr_pb_printf(pb, PRE_STR2"Core:large_vrf               %d\n", prefix, info->config.core.large_vrf);

    rm_alpm_config_bank_dump(pb, &info->config.core.tot, "tot ");
    switch (db) {
        case 0: rm_alpm_config_bank_dump(pb, &info->config.core.db0, "db0 "); break;
        case 1: rm_alpm_config_bank_dump(pb, &info->config.core.db1, "db1 "); break;
        case 2: rm_alpm_config_bank_dump(pb, &info->config.core.db2, "db2 "); break;
        case 3: rm_alpm_config_bank_dump(pb, &info->config.core.db3, "db3 "); break;
        default: break;
    }

    shr_pb_printf(pb, PRE_STR2"hit_support                  %d\n", prefix, info->config.hit_support);
    shr_pb_printf(pb, PRE_STR2"ver0_128_enable              %d\n", prefix, info->config.ver0_128_enable);
    shr_pb_printf(pb, PRE_STR2"ipv4_uc_sbr                  %d\n", prefix, info->config.ipv4_uc_sbr);
    shr_pb_printf(pb, PRE_STR2"ipv4_uc_vrf_sbr              %d\n", prefix, info->config.ipv4_uc_vrf_sbr);
    shr_pb_printf(pb, PRE_STR2"ipv4_uc_override_sbr         %d\n", prefix, info->config.ipv4_uc_override_sbr);
    shr_pb_printf(pb, PRE_STR2"ipv6_uc_sbr                  %d\n", prefix, info->config.ipv6_uc_sbr);
    shr_pb_printf(pb, PRE_STR2"ipv6_uc_vrf_sbr              %d\n", prefix, info->config.ipv6_uc_vrf_sbr);
    shr_pb_printf(pb, PRE_STR2"ipv6_uc_override_sbr         %d\n", prefix, info->config.ipv6_uc_override_sbr);
    shr_pb_printf(pb, PRE_STR2"ipv4_comp_sbr                %d\n", prefix, info->config.ipv4_comp_sbr);
    shr_pb_printf(pb, PRE_STR2"ipv6_comp_sbr                %d\n", prefix, info->config.ipv6_comp_sbr);
    shr_pb_printf(pb, PRE_STR2"destination                  %d\n", prefix, info->config.destination);
    shr_pb_printf(pb, PRE_STR2"destination_mask             %d\n", prefix, info->config.destination_mask);
    shr_pb_printf(pb, PRE_STR2"destination_type_match       %d\n", prefix, info->config.destination_type_match);
    shr_pb_printf(pb, PRE_STR2"destination_type_non_match   %d\n", prefix, info->config.destination_type_non_match);

}


static int
rm_alpm_sanity(int u, int m, int ln, alpm_arg_t *arg)
{
    int cur_ln;
    alpm_pivot_t *pivot;
    alpm_pivot_t *pre_pivot = NULL;
    alpm_bkt_elem_t *bkt_elem = NULL;
    int check_errors = 0;
    shr_pb_t *pb1 = NULL, *pb2 = NULL;
    SHR_FUNC_ENTER(u);

    /* Get last level index */
    pivot = arg->pivot[ln];
    arg->index[ln] = pivot->index;


    /* Get previous level (until Level-1) pivot & index */
    cur_ln = PREV_LEVEL(ln);
    while (cur_ln >= LEVEL1) {
        arg->pivot[cur_ln] =
            bcmptm_rm_alpm_bucket_pivot_get(u, m, arg->db,
                                            NEXT_LEVEL(cur_ln),
                                            arg->index[NEXT_LEVEL(cur_ln)]);

        arg->index[cur_ln] = arg->pivot[cur_ln]->index;
        cur_ln--;
    }

    if (arg->index[ln] == INVALID_INDEX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(u,
                              "Route index invalid\n")));
        check_errors++;
    }

    cur_ln = PREV_LEVEL(ln);
    while (cur_ln >= LEVEL1) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, m, arg->db, cur_ln, &arg->key.t,
                                    &pre_pivot));
        if (arg->pivot[cur_ln] != pre_pivot) {
            pb1 = shr_pb_create();
            pb2 = shr_pb_create();
            bcmptm_rm_alpm_key_dump(&arg->pivot[cur_ln]->key.t, pb1);
            bcmptm_rm_alpm_key_dump(&pre_pivot->key.t, pb2);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(u,
                                  "Level %d pivot check fails. \n"
                                  "    Got: %s\n"
                                  "    Expect: %s\n"),
                                  cur_ln + 1,
                                  shr_pb_str(pb1),
                                  shr_pb_str(pb2)));
            shr_pb_destroy(pb1);
            shr_pb_destroy(pb2);
            check_errors++;
        }
        cur_ln--;
    }
    cur_ln = PREV_LEVEL(ln);
    pre_pivot = arg->pivot[cur_ln];
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bkt_lookup(u, m, &arg->key.t, pre_pivot->bkt,
                                  &bkt_elem));
    if (bkt_elem != pivot) {
        pb1 = shr_pb_create();
        pb2 = shr_pb_create();
        bcmptm_rm_alpm_key_dump(&arg->pivot[cur_ln]->key.t, pb1);
        bcmptm_rm_alpm_key_dump(&pre_pivot->key.t, pb2);

        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(u,
                              "Bkt elem check fails. \n"
                              "    From bucket: %s\n"
                              "    From route trie: %s\n"),
                              shr_pb_str(pb1),
                              shr_pb_str(pb2)));
        shr_pb_destroy(pb1);
        shr_pb_destroy(pb2);
        check_errors++;
    }
    if (check_errors) {
        pb1 = shr_pb_create();
        bcmptm_rm_alpm_arg_dump(u, m, arg, true, pb1);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(u,
                              "Sanity errors=%d on %s\n"),
                              check_errors,
                              shr_pb_str(pb1)));
        shr_pb_destroy(pb1);
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_key_dump(key_tuple_t *t, shr_pb_t *pb)
{
    shr_pb_printf(pb, "len:%d ", t->len);
    trie_key_dump(t->max_bits, &t->trie_ip, pb, "");

}


int
bcmptm_rm_alpm_sanity(int u, int m, bcmltd_sid_t ltid, int *num_fails)
{
    int max_levels;
    alpm_arg_t arg;
    int sanity_fails = 0, sanity_oks = 0;
    uint8_t l1v;
    int rv, i;
    int alpm_lts_cnt;
    const lt_mreq_info_t *lrd_info = NULL;
    bcmptm_rm_alpm_req_info_t req_info = {0};;



    if (ltid == BCMLTD_SID_INVALID) {
        alpm_lts_cnt = bcmptm_rm_alpm_lts_count(u, m);
    } else {
        alpm_lts_cnt = 1;
    }
    for (i = 0; i < alpm_lts_cnt; i++) {
        if (ltid == BCMLTD_SID_INVALID || i > 0) {
            ltid = bcmptm_rm_alpm_lt_get(u, m, i);
        }

        rv = bcmlrd_lt_mreq_info_get(u, ltid, &lrd_info);
        if (SHR_FAILURE(rv) || !lrd_info) {
            return SHR_E_INTERNAL;
        }
        req_info.rm_more_info = lrd_info->rm_more_info;
        /*
         * For specified LT, the input m is not honoured, the
         * we will get the correct m value by parsing LT any.
         * For invalid LT, the input m is honoured, and
         * it must be equal to the parsed m.
         */
        rv = bcmptm_rm_alpm_req_arg(u, ltid, BCMPTM_OP_GET_TABLE_INFO,
                                    &req_info, &l1v, &m, &arg);

        if (rv == SHR_E_RESOURCE)  {
            continue;
        }
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        arg.key.t.max_bits =
            bcmptm_rm_alpm_max_key_bits(u, m, arg.key.t.ipv, arg.key.t.app,
                                        arg.key.vt);
        arg.valid = 1; /* For debug sanity */
        max_levels = bcmptm_rm_alpm_max_levels(u, m, arg.db);
        if (arg.key.vt == VRF_OVERRIDE ||
            max_levels <= 1) {
            continue;
        }
        rv = bcmptm_rm_alpm_get_first(u, m, ltid, true, &arg, max_levels);
        while (rv == SHR_E_NONE) {
            if (rm_alpm_sanity(u, m, max_levels - 1, &arg) < 0) {
                sanity_fails++;
            } else {
                sanity_oks++;
            }
            rv = bcmptm_rm_alpm_get_next(u, m, ltid, true, &arg, max_levels);
        }
        /* bcmptm_rm_alpm_buckets_sanity(u, m, arg.db, LEVEL2, -1, 0); */
    }


    if (num_fails) {
        *num_fails = sanity_fails;
    }
    if (sanity_fails) {
        return -1;
    }
    return SHR_E_NONE;
}

void
bcmptm_rm_alpm_info_dump(int u, int m, int db, int verbose, int brief, shr_pb_t *pb)
{
    alpm_info_t *info;

    DEBUG_PB_ENTER(pb);

    info = bcmptm_rm_alpm_info_get(u, m);
    if (!info || db >= DBS) {
        DEBUG_PB_EXIT(pb);
        return;
    }

    dev_info_dump(u, m, pb, "");
    config_dump(u, m, db, pb, info, "");
    bcmptm_rm_alpm_l1_info_dump(u, m, db, pb, verbose, brief);
    bcmptm_rm_alpm_ln_info_dump(u, m, db, pb, verbose, brief);

    shr_pb_printf(pb, "-This is it-\n");
    DEBUG_PB_EXIT(pb);
}


void
bcmptm_rm_alpm_arg_dump(int u, int m, alpm_arg_t *arg, bool compact, shr_pb_t *pb)
{
    char *suffix;
    char *separator[2];
    DEBUG_PB_ENTER2(pb);

    if (!arg) {
        DEBUG_PB_EXIT2(pb);
        return;
    }

    if (compact) {
        suffix = " ";
        separator[0] = "[";
        separator[1] = "]";
    } else {
        suffix = "\n";
        separator[0] = "-------";
        separator[1] = "-------";
    }

    shr_pb_printf(pb, "%s%s", separator[0], suffix);
    shr_pb_printf(pb, "app:%s%s", bcmptm_rm_alpm_app_name(u, m, arg->key.t.app), suffix);
    shr_pb_printf(pb, "db:%d%s", arg->db, suffix);
    shr_pb_printf(pb, "src:%d%s", bcmptm_rm_alpm_db_is_src(u, m, arg->db), suffix);
    shr_pb_printf(pb, "w_vrf:%d%s", arg->key.t.w_vrf, suffix);
    shr_pb_printf(pb, "vt:%s%s", bcmptm_rm_alpm_vt_name(u, m, arg->key.vt),
                  suffix);
    shr_pb_printf(pb, "v6:%d%s", arg->key.t.ipv, suffix);
    shr_pb_printf(pb, "kt:%s%s",
                  bcmptm_rm_alpm_kt_name(u, m, arg->key.kt), suffix);
    trie_key_dump(arg->key.t.max_bits, &arg->key.t.trie_ip, pb, suffix);

    shr_pb_printf(pb, "pfx:%d%s", arg->key.pfx, suffix);
    shr_pb_printf(pb, "len:%d%s", arg->key.t.len, suffix);
    shr_pb_printf(pb, "dt:%s%s", arg->ad.user_data_type == DATA_FULL_3 ? "full_3":
                                 arg->ad.user_data_type == DATA_FULL_2 ? "full_2":
                                 arg->ad.user_data_type == DATA_FULL ? "full" :
                                 "reduced", suffix);

    if (!compact) {
        shr_pb_printf(pb, "hit:%d\n", arg->hit);
        shr_pb_printf(pb, "default_route:%d\n", arg->default_route);
        shr_pb_printf(pb, "hit_idx:%d\n", arg->a1.hit_idx);
        shr_pb_printf(pb, "valid:%d\n", arg->valid);
        shr_pb_printf(pb,
                      "index:0x%x 0x%x 0x%x\n",
                      arg->index[0],
                      arg->index[1],
                      arg->index[2]);
        }
    shr_pb_printf(pb, "%s\n", separator[1]);
    DEBUG_PB_EXIT2(pb);
}


void
bcmptm_rm_alpm_bucket_dump(int u, int m, int db, int ln, rbkt_group_t group,
                               bool compact, shr_pb_t *pb)
{
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);
    bcmptm_rm_alpm_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    rbkt_group_dump(&bkt_hdl->rbkt_array[group], compact, pb, "--");
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_buckets_dump(int u, int m, int db, int ln, shr_pb_t *pb, bool verbose)
{
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);
    shr_pb_printf(pb, "\nAll bucket groups list dump [DB=%d Level=%d]\n", db, ln + 1);

    bcmptm_rm_alpm_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    if (bkt_hdl) {
        rbkt_group_list_dump(bkt_hdl, verbose, pb, "");
    }
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_buckets_dump2(int u, int m, int db, int ln, shr_pb_t *pb, bool verbose)
{
    int i;
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);

    shr_pb_printf(pb, "\nAll bucket groups usage list dump [DB=%d Level=%d]\n", db, ln + 1);

    bcmptm_rm_alpm_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    for (i = 0; i < bkt_hdl->group_usage_count; i++) {
        shr_pb_printf(pb, "  Group usage free count [%d] \n", i);
        rbkt_group_usg_list_dump(&bkt_hdl->group_usage[i].groups,
                                 verbose, pb, "");
    }
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_buckets_brief_dump(int u, int m, int db, int ln, shr_pb_t *pb, bool verbose)
{
    bkt_hdl_t *bkt_hdl;
    DEBUG_PB_ENTER(pb);
    bcmptm_rm_alpm_bkt_hdl_get(u, m, db, ln, &bkt_hdl);
    if (bkt_hdl) {
        rbkt_group_list_brief_dump(u, m, bkt_hdl, pb, "", verbose);
    } else {
        shr_pb_printf(pb, "bkt_hdl not found \n");
    }
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_ln_info_dump(int u, int m, int db, shr_pb_t *pb, int verbose, int brief)
{
    DEBUG_PB_ENTER(pb);
    ln_info_dump(u, m, db, pb, "", verbose, 0, brief);
    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_l1_info_dump(int u, int m, int db, shr_pb_t *pb, int verbose, int brief)
{
    DEBUG_PB_ENTER(pb);
    l1_info_dump(u, m, db, verbose, pb, "");
    if (!brief) {
        l1_pivot_info_dump(u, m, db, verbose, pb, "");
    }

    DEBUG_PB_EXIT(pb);
}

void
bcmptm_rm_alpm_pt_dump(int u, int m, bcmdrd_sid_t sid, uint32_t *data)
{
    int rv;
    shr_pb_t *pb = shr_pb_create();
    rv = bcmdrd_pt_dump_fields(u, sid, data,
                               BCMDRD_SYM_DUMP_BIT_RANGE,
                               pb);
    if (SHR_FAILURE(rv)) {
        shr_pb_destroy(pb);
        return;
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

void
bcmptm_rm_alpm_pt_dump2(int u, int m, const char * name, uint32_t *data)
{
    int rv;
    bcmdrd_sid_t sid;
    shr_pb_t *pb = shr_pb_create();
    rv = bcmdrd_pt_name_to_sid(u, name, &sid);
    if (SHR_FAILURE(rv)) {
        shr_pb_destroy(pb);
        return;
    }
    rv = bcmdrd_pt_dump_fields(u, sid, data,
                               BCMDRD_SYM_DUMP_BIT_RANGE,
                               pb);
    if (SHR_FAILURE(rv)) {
        shr_pb_destroy(pb);
        return;
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

void
bcmptm_rm_alpm_pt_dump3(int u, int m, const char * name, const char *string)
{
    reset_hex_buf(u, m, name, alpm_ts_hex_buf);
    parse_hex_string(string);
    bcmptm_rm_alpm_pt_dump2(u, m, name, alpm_ts_hex_buf);
}

int
bcmptm_rm_alpm_table_dump(int u, int m, int db, int idx, int lvl_cnt)
{
    int rv = SHR_E_NONE;
    int i, k, pfx, start, end, vent;
    int pkm, ipv, len, la_end;
    alpm_vrf_type_t vt;
    uint8_t app;
    block_state_t *st;
    alpm_dev_info_t *dev_info = NULL;
    alpm_l1_info_t *info = NULL;
    l1_db_info_t *l1_db;
    l1_key_input_info_t *l1_ki;
    alpm_arg_t varg;
    alpm_arg_t *arg;

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    info = bcmptm_rm_alpm_l1_info_get(u, m);
    if (!info || !dev_info) {
        cli_out("ALPM TCAM Level1 no initialized yet\n");
        return SHR_E_INIT;
    }
    /*cli_out("alpm dump db=%d index=%d levels=%d\n", db, idx, lvl_cnt);*/

    l1_db = &info->l1_db[db];
    if (!l1_db->valid) {
        cli_out("Empty DB.\n");
        return rv;
    }

    /* check l1_index range */
    if (idx >= dev_info->tcam_blocks * dev_info->tcam_depth) {
        cli_out("l1_index should be less than %d\n",
                dev_info->tcam_blocks * dev_info->tcam_depth);
        return SHR_E_PARAM;
    }

    /* check levels */
    if (lvl_cnt < 1 || lvl_cnt > 3) {
        lvl_cnt = 1;
    }

    for (pkm = PKM_NUM - 1; pkm >= 0; pkm--) {
        l1_ki = &l1_db->ki[pkm];
        if (!l1_ki->valid) {
            continue;
        }

        cli_out("\n-- DB%d PKM:%d (%s) \tL1_index:%d-%d (size:%d)\n",
                db, pkm, bcmptm_rm_alpm_pkm_name(u, m, pkm),
                l1_ki->block_start, l1_ki->block_end, l1_ki->block_size);

        st = l1_ki->block_state;

        /* Pair TCAM traverse pfx both: LAYOUT_D & LAYOUT_SH
           Unpair TCAM traverse pfx only: LAYOUT_D */
        la_end = bcmptm_rm_alpm_pkm_is_pair(u, m, pkm) ? LAYOUT_SH : LAYOUT_D;

        for (i = LAYOUT_D; i >= la_end; i--) {
            pfx = l1_ki->max_pfx[i];

            while (st && pfx >= 0) {
                start = STATE_S(st, pfx);
                end = STATE_E(st, pfx);
                vent = STATE_V(st, pfx);

                if (vent > 0 && start >= 0 && end >= 0  && start <= end) {
                    bcmptm_rm_alpm_l1_pfx_decode(u, m, pfx, &ipv, &vt, &len, NULL, &app);

                    arg = &varg;
                    bcmptm_rm_alpm_arg_init(u, m, arg);

                    arg->db = db;
                    arg->pkm = pkm;
                    arg->key.vt = vt;
                    arg->key.t.app = app;
                    arg->key.t.ipv = ipv;
                    arg->key.t.len = len;

                    for (k = start; k <= end; k++) {
                        if (idx < 0 || k == idx) {

                            rv = bcmptm_rm_alpm_l1_dump(u, m, arg, k, lvl_cnt);
                            if (SHR_FAILURE(rv)) {
                                if (rv == SHR_E_NOT_FOUND) {
                                    continue;
                                }
                                break;
                            }

                            if (k == idx) {
                                return rv; /* end here for a specific idx */
                            }
                        }
                    }
                }

                pfx = STATE_N(st, pfx);
            }
        }
    }

    return rv;
}

#ifdef ALPM_DEBUG_FILE
void
bcmptm_rm_alpm_dump_to_file_set(bool enable)
{
    write_file = enable;
}

bool
bcmptm_rm_alpm_dump_to_file_get()
{
    return write_file;
}
#endif

