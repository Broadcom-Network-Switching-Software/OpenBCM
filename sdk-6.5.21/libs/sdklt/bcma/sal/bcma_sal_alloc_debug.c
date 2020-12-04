/*! \file bcma_sal_alloc_debug.c
 *
 * The SAL alloc debug framework adds a header in front of all
 * allocatations and inserts corruption sentinels around both the
 * header and the allocated memory block (as seen from the
 * application).
 *
 *   +-------------------+ <----- Start of system memory block
 *   |    Sentinel (5)   |
 *   +-------------------+
 *   |                   |
 *   |      Header       |
 *   |                   |
 *   +-------------------+
 *   |    Sentinel (a)   |
 *   +-------------------+ <----- Start of application memory block
 *   |                   |
 *   |    Application    |
 *   |       Data        |
 *   |                   |
 *   +-------------------+ <----- End of application memory block
 *   |    Sentinel (b)   |
 *   +-------------------+ <----- End of system memory block
 *
 * The sentinels are validated when memory is freed.
 *
 * The framework can optionally enable memory tracking, in which case
 * all the memory blocks are kept in a linked list.
 *
 * If memory tracking is enabled, the sentinels can also be checked on
 * demand.
 *
 * To aid memory corruption analysis, newly allocated memory is filled
 * with a fixed pattern, and a different pattern will be filled in
 * immediately before the memory is freed.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_sysrq.h>
#include <sal/sal_assert.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_alloc.h>
#include <sal/sal_alloc_debug.h>

#include <bcma/sal/bcma_sal_alloc_debug.h>

/*******************************************************************************
 * Double linked list definitions
 */

typedef struct dll_list_s {
    struct dll_list_s *next;
    struct dll_list_s *prev;
} dll_list_t;

static void
dll_list_init(dll_list_t *head)
{
    if (head) {
        head->next = head;
        head->prev = head;
    }
}

static void
dll_list_add(dll_list_t *head, dll_list_t *entry)
{
    dll_list_t *next = head->next;
    dll_list_t *prev = head;

    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

static void
dll_list_del(dll_list_t *entry)
{
    dll_list_t *next = entry->next;
    dll_list_t *prev = entry->prev;

    prev->next = next;
    next->prev = prev;
}

static bool
dll_list_is_empty(dll_list_t *head)
{
    return (head->next == head);
}

static bool
dll_list_is_last(dll_list_t *head, dll_list_t *entry)
{
    return (entry->next == head);
}

#define DLL_LIST_HEAD(_h) _h = { &_h, &_h }

static dll_list_t DLL_LIST_HEAD(alloc_list);

/*******************************************************************************
 * Local definitions
 */

/*! Markers for sanity checks. */
#define SIGNATURE_SENTINEL      0x55555555
#define UNDERRUN_SENTINEL       0xaaaaaaaa
#define OVERRUN_SENTINEL        0xbbbbbbbb
#define DATA_FREED              0xcccccccc

/* Fill patterns for allocated and freed memory */
#define ALLOC_BYTE              0xa5
#define FREE_BYTE               0x5a

/*! Maximum length of prefix list string. */
#define PREFIX_LIST_LEN_MAX     64

/*! Prefix separator for prefix string lists. */
#define SEP                     ','

/*! Special symbol to match any uppercase character */
#define ANYUPR                  '~'

/*! Simple check for valid memory block address. */
#define ADDR_VALID(_addr) \
    ((unsigned long)(_addr) >= 0x1000)

/*! Memory block header. */
typedef struct alloc_dbg_s {

    /*! Block signature for sanity and interception control. */
    uint32_t sig;

    /*! Unique number for each allocated block. */
    uint32_t seqno;

    /*! ID string passed to \c sal_alloc. */
    const char *idstr;

    /*! Double linked list for tracking. */
    dll_list_t list;

    /*! Memory block size requested by application. */
    uint32_t size;

    /*! Actual size of allocated memory block. */
    uint32_t aligned_size;

    /*!
     * Align return address (data) to sixteen 32-bit words for both
     * 32-bit and 64-bit systems. Since this structure contains three
     * pointers (idstr and list), the padding must be three words
     * shorted for 64-bit systems.
     */
    uint32_t pad[8 - (3 * (sizeof(void *) / 8))];

    /*! Underrun sentinel used for header sanity check. */
    uint32_t sentinel;

    /*! Start of application memory block. */
    uint32_t data[];

} alloc_dbg_t;

/*! Debug control data definition. */
typedef struct bcma_sal_debug_alloc_ctrl_s {

    /*! Enable memory allocation debug (main control). */
    bool debug_enable;

    /*! Fill memory with fixed pattern on allo/free. */
    bool fill_enable;

    /*! Check \ref sal_alloc ID strings for conformance. */
    bool idchk_enable;

    /*! Trace prefixes for filtering trace messages. */
    char trace_prefix[PREFIX_LIST_LEN_MAX];

    /*! Trace prefixes to be ignored. */
    char notrace_prefix[PREFIX_LIST_LEN_MAX];

    /*! Enable memory allocation tracking. */
    bool track_enable;

    /*! Track prefixes for filtering track dump output. */
    char track_prefix[PREFIX_LIST_LEN_MAX];

} bcma_sal_debug_alloc_ctrl_t;

/*! Sequence number for allocated memory blocks. */
static uint32_t seqno;

/*! Lock for tracking database. */
static sal_spinlock_t lock;

/*! Debug control data. */
static bcma_sal_debug_alloc_ctrl_t dbg_ctrl;

/*! Debug statistics. */
static bcma_sal_debug_alloc_stats_t dbg_stats;

/*******************************************************************************
 * Local functions
 */

/*!
 * \brief Match ID string against list of prefixes.
 *
 * For example if the prefix string is "sal,shr", then ID strings
 * "salStrdup" and shrPbAlloc" will match, while "bcmaCliCtok" will
 * not match.
 *
 * \param [in] idstr ID string.
 * \param [in] prefix String of comma-separated prefix strings.
 *
 * \retval true ID string matches a prefix.
 * \retval false ID string does not match any prefix.
 */
static bool
prefix_match(const char *idstr, const char *prefix)
{
    const char *pstr;
    const char *istr;

    if (!idstr) {
        return false;
    }

    /* Reset list of prefixes */
    pstr = prefix;

    do {
        /* Reset ID string and start comparing */
        istr = idstr;
        while (*pstr && *istr &&
               (*pstr == *istr ||
                (*pstr == ANYUPR && sal_isupper(*istr)))) {
            istr++;
            pstr++;
        }
        /* Match if at the end of current prefix */
        if (*pstr == '\0' || *pstr == SEP) {
            return true;
        }
        /* Skip to next prefix separator */
        while (*pstr && *pstr != SEP) {
            pstr++;
        }
        /* Skip over prefix separator(s) */
        while (*pstr && *pstr == SEP) {
            pstr++;
        }
    } while (*pstr);

    return false;
}

/*!
 * \brief Compare \c sal_alloc ID string to a trace/track prefix.
 *
 * The prefix string may contain a single prefix or multiple
 * comma-separated prefixes, e.g. "cli", "cli,shr,bcma".
 *
 * An optional exclusion list may be supplied as well. This list takes
 * precendece over the prefix list.
 *
 * \param [in] idstr ID string.
 * \param [in] prefix String of comma-separated prefix strings to match.
 * \param [in] excl String of comma-separated prefix strings to skip.
 *
 * \retval true ID string matches a prefix.
 * \retval false ID string does not match any prefix.
 */
static bool
idstr_match(const char *idstr, const char *prefix, const char *excl)
{
    /* Empty string never matches */
    if (prefix[0] == '\0') {
        return false;
    }

    /* Skip if exclusion list has a match */
    if (excl && excl[0]) {
        if (prefix_match(idstr, excl)) {
            return false;
        }
    }

    /* Check for wildcard prefix */
    if (prefix[0] == '*') {
        return true;
    }

    /* Missing ID string never matches */
    if (!idstr) {
        return false;
    }

    /* Check against list of prefixes */
    return prefix_match(idstr, prefix);
}

/*!
 * \brief Compare \c sal_alloc ID string to current trace prefix.
 *
 * \param [in] idstr ID string.
 *
 * \retval true ID string matches trace prefix.
 * \retval false ID string does not match trace prefix.
 */
static bool
idstr_match_trace(const char *idstr)
{
    return idstr_match(idstr, dbg_ctrl.trace_prefix, dbg_ctrl.notrace_prefix);
}

/*!
 * \brief Compare \c sal_alloc ID string to current track prefix.
 *
 * \param [in] idstr ID string.
 *
 * \retval true ID string matches track prefix.
 * \retval false ID string does not match track prefix.
 */
static bool
idstr_match_track(const char *idstr)
{
    return idstr_match(idstr, dbg_ctrl.track_prefix, NULL);
}

/*!
 * \brief Add chunk to tracking database.
 *
 * \param [in] dbg Debug header.
 *
 * \return Nothing.
 */
static void
track_add(alloc_dbg_t *dbg)
{
    if (dbg_ctrl.track_enable) {
        sal_spinlock_lock(lock);
        dll_list_add(&alloc_list, &dbg->list);
        sal_spinlock_unlock(lock);
    }
}

/*!
 * \brief Delete chunk from tracking database.
 *
 * \param [in] dbg Debug header.
 *
 * \return Nothing.
 */
static void
track_del(alloc_dbg_t *dbg)
{
    if (dbg_ctrl.track_enable) {
        sal_spinlock_lock(lock);
        dll_list_del(&dbg->list);
        sal_spinlock_unlock(lock);
    }
}

/*!
 * \brief Print memory corrupt error message.
 *
 * \param [in] msg Primary error message.
 * \param [in] addr Address of corrupted memory block.
 * \param [in] idstr ID string of the corrupted memory block.
 */
static void
msg_corrupt(const char *msg, void *addr, const char *idstr)
{
    int memory_corruption = 0;

    LOG_ERROR(BSL_LS_APPL_SHELL,
              (BSL_META("%s %p (%s)\n"), msg, addr, idstr));
    assert(memory_corruption);
}

/*! Add signature and sentinels for memory block. */
static void
sig_add(alloc_dbg_t *dbg)
{
    dbg->sentinel = UNDERRUN_SENTINEL;
    dbg->sig = SIGNATURE_SENTINEL;
    dbg->data[dbg->aligned_size / 4] = OVERRUN_SENTINEL;
}

/*! CHeck signature and sentinels for memory block. */
static int
sig_check(alloc_dbg_t *dbg)
{
    void *addr = &dbg->data;
    const char *idstr = dbg->idstr;

    if (dbg->sentinel == DATA_FREED) {
        msg_corrupt("Memory already freed\n", addr, idstr);
        return -1;
    }
    if (dbg->sentinel != UNDERRUN_SENTINEL || dbg->sig != SIGNATURE_SENTINEL) {
        msg_corrupt("Memory corruption (low)\n", addr, "?");
        dbg->idstr = "?";
        dbg->size = 0;
    } else if (dbg->data[dbg->aligned_size / 4] != OVERRUN_SENTINEL) {
        msg_corrupt("Memory corruption (high)\n", addr, idstr);
    }
    return 0;
}

/*! Debug hook for \ref sal_alloc. */
static void *
dbg_alloc(size_t sz, char *s)
{
    uint32_t aligned_size;
    uint32_t alloc_size;
    alloc_dbg_t *dbg;

    if (dbg_ctrl.idchk_enable) {
        /* Check for conforming ID string */
        if (s == NULL || *s == '\0') {
            LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META("Missing ID string\n")));
            sal_sysrq(SAL_SYSRQ_BACKTRACE, 10);
        } else if ((sal_strncmp(s, "bcm", 3) != 0 &&
                    sal_strncmp(s, "shr", 3) != 0 &&
                    sal_strncmp(s, "sal", 3) != 0) ||
                   sal_strchr(s, ' ') ||
                   sal_strchr(s, '_')) {
            LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META("Invalid ID string: '%s'\n"), s));
        }
    }

    if (sz == 0) {
        LOG_WARN(BSL_LS_APPL_SHELL,
                 (BSL_META("Attempting to allocate 0 bytes (%s)\n"), s));
        /* Reserve one word for debug signature */
        sz = 4;
    }

    /* Overrun sentinel must be on an aligned address */
    aligned_size = (sz + 3) & ~0x3;

    /* Add room for header and overrun sentinel */
    alloc_size = aligned_size + sizeof(alloc_dbg_t) + sizeof(uint32_t);

    /* Allocate memory via real SAL function */
    if ((dbg = sal_alloc(alloc_size, "#")) == NULL) {
        dbg_stats.alloc_errors++;
        return NULL;
    }

    /* Fill out header information */
    dbg->seqno = seqno++;
    dbg->idstr = (s && *s) ? s : "<unknown>";
    dbg->size = sz;
    dbg->aligned_size = aligned_size;
    sig_add(dbg);

    /* Print trace string if enabled */
    if (idstr_match_trace(dbg->idstr)) {
        cli_out("[sal_alloc]: %s allocated %"PRId32" bytes (#%d)\n",
                dbg->idstr, dbg->size, dbg->seqno);
    }

    /* Update statistics */
    dbg_stats.num_allocs++;
    dbg_stats.cur_chunks++;
    dbg_stats.cur_bytes += dbg->size;

    /* Add chunk to tracking database */
    track_add(dbg);

    /* Sanity check for proper alignment */
    assert((((unsigned long)dbg->data) & (sizeof(void *) - 1)) == 0);

    /* The sal_free hook will check this value */
    SAL_ALLOC_SIG_CLR(dbg->data);

    /* Add recognizable pattern for newly allocated memory */
    if (dbg_ctrl.fill_enable) {
        sal_memset(dbg->data, ALLOC_BYTE, dbg->size);
    }

    /* Return pointer to application memory block */
    return dbg->data;
}

/*! Debug hook for \ref sal_free. */
static void
dbg_free(void *addr)
{
    char *real_addr = addr;
    alloc_dbg_t *dbg;

    /* Do not touch header if pointer is obviously corrupted (e.g. NULL) */
    if (!ADDR_VALID(addr)) {
        msg_corrupt("Attempt to free invalid address", addr, "?");
        return;
    }

    /* Find header from application pointer */
    real_addr -= offsetof(alloc_dbg_t, data);
    dbg = (alloc_dbg_t *)real_addr;

    /* Sanity check memory block */
    if (sig_check(dbg) < 0) {
        return;
    }

    /* Print trace string if enabled */
    if (idstr_match_trace(dbg->idstr)) {
        cli_out("[sal_free]: Freeing #%d (%s)\n", dbg->seqno, dbg->idstr);
    }

    /* Update statistics */
    dbg_stats.num_frees++;
    dbg_stats.cur_chunks--;
    dbg_stats.cur_bytes -= dbg->size;

    /* Prevent interception */
    dbg->sentinel = DATA_FREED;
    SAL_ALLOC_SIG_SET(dbg);

    /* Remove chunk from tracking database */
    track_del(dbg);

    /* Add recognizable pattern for newly freed memory */
    if (dbg_ctrl.fill_enable) {
        sal_memset(dbg->data, FREE_BYTE, dbg->size);
    }

    /* Free memory via real SAL function */
    sal_free(real_addr);
}

/*******************************************************************************
 * Public functions
 */

int
bcma_sal_alloc_debug_trace_prefix_set(const char *prefix)
{
    if (prefix) {
        sal_strncpy(dbg_ctrl.trace_prefix, prefix,
                    sizeof(dbg_ctrl.trace_prefix) - 1);
    }
    return 0;
}

int
bcma_sal_alloc_debug_notrace_prefix_set(const char *prefix)
{
    if (prefix) {
        sal_strncpy(dbg_ctrl.notrace_prefix, prefix,
                    sizeof(dbg_ctrl.notrace_prefix) - 1);
    }
    return 0;
}

int
bcma_sal_alloc_debug_track_prefix_set(const char *prefix)
{
    if (prefix) {
        sal_strncpy(dbg_ctrl.track_prefix, prefix,
                    sizeof(dbg_ctrl.track_prefix) - 1);
    }
    return 0;
}

int
bcma_sal_alloc_debug_fill_enable_set(bool fill)
{
    dbg_ctrl.fill_enable = fill;
    return 0;
}

int
bcma_sal_alloc_debug_idchk_enable_set(bool idchk)
{
    dbg_ctrl.idchk_enable = idchk;
    return 0;
}

int
bcma_sal_alloc_debug_stats_get(bcma_sal_debug_alloc_stats_t *stats)
{
    *stats = dbg_stats;
    return 0;
}

int
bcma_sal_alloc_debug_status(void)
{
    uint32_t hbytes;
    char *hstr;

    if (!dbg_ctrl.debug_enable) {
        cli_out("SAL Debug: Memory debug not enabled.\n");
        return 0;
    }

    hbytes = dbg_stats.cur_bytes;
    hbytes /= 1024;
    hstr = "K";
    if (hbytes >= 1024) {
        hbytes /= 1024;
        hstr = "M";
    }

    cli_out("%-32s %10"PRId32"\n",
            "Successful calls to sal_alloc:", dbg_stats.num_allocs);
    cli_out("%-32s %10"PRId32"\n",
            "Successful calls to sal_free:", dbg_stats.num_frees);
    cli_out("%-32s %10"PRId32"\n",
            "Number of chunks allocated:", dbg_stats.cur_chunks);
    cli_out("%-32s %10"PRId32" (%"PRId32"%s)\n",
            "Number of bytes allocated:", dbg_stats.cur_bytes, hbytes, hstr);
    cli_out("%-32s %10"PRId32"\n",
            "Failed calls to sal_alloc:", dbg_stats.alloc_errors);
    cli_out("%-32s %10s\n",
            "Trace prefix(es):", dbg_ctrl.trace_prefix);
    cli_out("%-32s %10s\n",
            "No-trace prefix(es):", dbg_ctrl.notrace_prefix);
    cli_out("%-32s %10s\n",
            "Tracking:", dbg_ctrl.track_enable ? "enabled" : "disabled");
    cli_out("%-32s %10s\n",
            "Tracking prefix(es):", dbg_ctrl.track_prefix);
    cli_out("%-32s %10s\n",
            "Pattern fill:", dbg_ctrl.fill_enable ? "enabled" : "disabled");

    return 0;
}

int
bcma_sal_alloc_debug_dump(void)
{
    dll_list_t *entry;
    alloc_dbg_t *dbg;
    uint32_t chunks, bytes, hbytes;
    char *hstr;

    if (!dbg_ctrl.track_enable) {
        cli_out("SAL Debug: Memory tracking not enabled.\n");
        return 0;
    }

    if (dll_list_is_empty(&alloc_list)) {
        cli_out("SAL Debug: No tracked allocations.\n");
    } else {
        cli_out("SAL Debug: Current tracked allocations (%s):\n",
                dbg_ctrl.track_prefix);
        chunks = 0;
        bytes = 0;
        entry = &alloc_list;
        do {
            entry = entry->next;
            dbg = (alloc_dbg_t *)(((char *)entry) - offsetof(alloc_dbg_t, list));
            if (idstr_match_track(dbg->idstr)) {
                cli_out("%s allocated %"PRId32" bytes (#%d)\n",
                        dbg->idstr, dbg->size, dbg->seqno);
                chunks++;
                bytes += dbg->size;
            }
        } while (!dll_list_is_last(&alloc_list, entry));
        hbytes = bytes;
        hbytes /= 1024;
        hstr = "K";
        if (hbytes >= 1024) {
            hbytes /= 1024;
            hstr = "M";
        }
        cli_out("Total chunks: %10"PRId32"\n", chunks);
        cli_out("Total bytes:  %10"PRId32" (%"PRId32"%s)\n",
                bytes, hbytes, hstr);
    }

    return 0;
}

int
bcma_sal_alloc_debug_cleanup(void)
{
    SAL_ALLOC_DEBUG_ALLOC_SET(NULL);
    SAL_ALLOC_DEBUG_FREE_SET(NULL);

    if (lock && sal_spinlock_destroy(lock) == 0) {
        lock = NULL;
    }

    dbg_ctrl.track_enable = false;
    dbg_ctrl.debug_enable = false;

    return 0;
}

int
bcma_sal_alloc_debug_init(bool track_enable)
{
    if (dbg_ctrl.debug_enable) {
        return -1;
    }

    /* Optionally enable memory tracking */
    if (track_enable) {
        dll_list_init(&alloc_list);
        lock = sal_spinlock_create("bcmaSalAllocDebug");
        if (lock) {
            dbg_ctrl.track_enable = true;
        }
    }

    SAL_ALLOC_DEBUG_ALLOC_SET(dbg_alloc);
    SAL_ALLOC_DEBUG_FREE_SET(dbg_free);

    dbg_ctrl.fill_enable = true;
    dbg_ctrl.debug_enable = true;

    return 0;
}
