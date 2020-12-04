/*! \file bcma_sal_alloc_debug.h
 *
 * Debug API for sal_alloc and friends.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SAL_ALLOC_DEBUG_H
#define BCMA_SAL_ALLOC_DEBUG_H

#include <sal/sal_types.h>


/*! Debug statistics definition. */
typedef struct bcma_sal_debug_alloc_stats_s {

  /*! Number of successful calls to \c sal_alloc. */
  uint32_t num_allocs;

  /*! Number of successful calls to \c sal_free. */
  uint32_t num_frees;

  /*! Current number of memory chunks allocated. */
  uint32_t cur_chunks;

  /*! Current number of bytes allocated. */
  uint32_t cur_bytes;

  /*! Number of unsuccessful calls to \c sal_alloc. */
  uint32_t alloc_errors;

} bcma_sal_debug_alloc_stats_t;

/*!
 * \brief Set \c sal_alloc trace prefix list.
 *
 * The \c sal_alloc function optionally prints a message on the
 * console each time sal_alloc is called. The trace prefix list is
 * used to filter these messages.
 *
 * If \c prefix is an empty string (""), then no messages will be
 * printed. If the prefix string is "*", then all messages will be
 * printed. Any other prefix string will be matched against the ID
 * string parameter supplied to sal_alloc.
 *
 * Multiple prefixes may be specified as a comma-separated list,
 * e.g. "sal,shr".
 *
 * \param [in] prefix Prefix string(s) to match against sal_alloc ID string.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_trace_prefix_set(const char *prefix);

/*!
 * \brief Set list of trace prefixes to ignore.
 *
 * If \c prefix is an empty string (""), then no prefixes will be
 * ignored.
 *
 * Multiple prefixes may be specified as a comma-separated list,
 * e.g. "sal,shr".
 *
 * See also \ref bcma_sal_alloc_debug_trace_prefix_set.
 *
 * \param [in] prefix Prefix string(s) to ignore.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_notrace_prefix_set(const char *prefix);

/*!
 * \brief Set \c sal_alloc track prefix.
 *
 * The \c sal_alloc function optionally tracks all allocations, which
 * can then be dumped on demand, for example when an application is
 * terminated.
 *
 * The \c prefix determines which allocation records will be printed
 * when the allocation database is dumped. If \c prefix is an empty
 * string (""), then no records will be printed. If the prefix string
 * is "*", then all records will be printed. Any other prefix string
 * will be matched against the ID string parameter supplied to
 * sal_alloc.
 *
 * \param [in] prefix Prefix string to match against sal_alloc ID string.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_track_prefix_set(const char *prefix);

/*!
 * \brief Enable or disable memory fill.
 *
 * If memory fill is enabled, then newly allocated memory will be
 * filled with a fixed pattern. Likewise, another fixed pattern will
 * be filled into the memory immediately before it is freed.
 *
 * \param [in] fill Set to true to enable memory fill.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_fill_enable_set(bool fill);

/*!
 * \brief Enable or disable ID string check.
 *
 * If ID string check is enabled, then the ID string passed into
 * sal_alloc is check for conformance. A warning message will be
 * printed for all missing or invalid ID strings.
 *
 * \param [in] idchk Set to true to enable ID string check.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_idchk_enable_set(bool idchk);

/*!
 * \brief Show memory allocation status.
 *
 * Print allocation statistics and controls to console.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_status(void);

/*!
 * \brief Get memory allocation statistics.
 *
 * Copy statistics to the given pointer.
 *
 * \param [out] stats Filled with with current debug alloc stats
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_stats_get(bcma_sal_debug_alloc_stats_t *stats);

/*!
 * \brief Dump tracked allocations.
 *
 * The tracked allocations will be printed according to the current
 * track prefix. See \ref bcma_sal_alloc_debug_track_prefix_set for
 * details.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_dump(void);

/*!
 * \brief Clean up the sal_alloc debug module.
 *
 * Clean up resources allocated by \ref bcma_sal_alloc_debug_init.
 *
 * \return 0 on success, otherwise -1.
 */
extern int
bcma_sal_alloc_debug_cleanup(void);

/*!
 * \brief Initialize the sal_alloc debug module.
 *
 * This function will install debug hooks into the SAL memory
 * allocation API (sal_alloc, etc.).
 *
 * The debug hooks will add overrun markers around each chunk of
 * allocated memory, and these markers will be checked when a chunk is
 * freed.
 *
 * If tracking is enabled, then all memory allocations are saved to a
 * database, which can be dumped on demand using \ref
 * bcma_sal_alloc_debug_dump.
 *
 * \param [in] track_enable Set to true to enable tracking.
 *
 * \return 0 on success, otherwise -1.
 */
extern int
bcma_sal_alloc_debug_init(bool track_enable);

#endif /* BCMA_SAL_ALLOC_DEBUG_H */
