/*
 * $Id: cint_logger_circular_buffer.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_circular_buffer.c
 * Purpose:     CINT logger circular buffer infra, also see
 *              cint_logger_circular_buffer.h
 *
 * Here we implement a circular buffer infrastructure to store API call data. A
 * configurable amount of memory is allocated for the circular buffer; the
 * logger infrastructure requests buffers to store API parameter data from
 * here. Memory blocks are strung together in a doubly linked list. Blocks
 * allocated to the first configurable N requests are never rolled over,
 * remaining buffers are freed and reused in essentially first in first out
 * fashion.
 */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <sal/core/libc.h>
#include <sal/core/time.h>
#include <appl/diag/cmdlist.h>

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_logger.h"
#include "cint_logger_circular_buffer.h"

/*
 * The test function requests random sized buffers millions or billions of
 * times... logging information at each call just slows the whole test down.
 * These macros are to print logs every so many requests.
 */
#define _LOG_INFO_EVERY(fmt,...) \
    if (cbuf->print_every &&                \
        !(cbuf->count % cbuf->print_every)) \
        LOG_INFO(BSL_LS_APPL_CINTAPILOGGER, (BSL_META(fmt "%s"), ## __VA_ARGS__))

#define LOG_INFO_EVERY(...) \
    _LOG_INFO_EVERY(__VA_ARGS__,"")

#define ASSERT(expr) \
    if (!(expr)) \
        LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER, \
                  (BSL_META("((%s) ASSERT (%s))"), cbuf->name, #expr))

/*
 * Update statistics  on addition or deletion of entries in the circular
 * buffer. Also, when adding update the BASE until initial nfreeze buffers are
 * used up. BASE then represents beginning of the circular buffer
 *
 * Parameters
 *   [in] cbuf
 *     circular buffer handle which to update
 *   [in] le
 *     list element for the entry being added. NOTE: ignored when deleting
 *     buffers
 *   [in] cnt
 *     number of buffers added or deleted. NOTE: This is ignored when adding
 *     buffers and assumed to be 1. This is because we have no case of adding
 *     buffers in any fashion other than one by one.
 *   [in] sz
 *     total size of buffers added or deleted
 *   [in] add_or_del
 *     boolean whether buffers were added or deleted. TRUE=add else del
 *
 * Usage statistics are updated. Also when adding buffers, the circular buffer
 * BASE may also be updated based on whether nfreeze entries had been created
 * or not.
 */
static void
cbuf_update_buffer_statistics (circular_buffer_t *cbuf, el_t *le, int cnt, int sz, int add)
{
    if (add) {

        /*
         * a buffer was added...
         *
         * ...update number of successful requests
         */
        cbuf->nreqs++;
        /*
         * ...update total number of entries in the circular buffer
         */
        cbuf->bufcnt++;
        /*
         * ...update the total size of entries in the circular buffer
         */
        cbuf->bufused += sz;
        /*
         * ...update the BASE if nfreeze entries have not yet been created
         */
        if (cbuf->nreqs <= cbuf->nfreeze) {

            /*
             * If nfreeze entries have not yet been created, then buffer could
             * not have wrapped and therefore HEAD must be at the beginning of
             * the total memory allocated to operate the circular buffer
             * scheme. Use this opportunity to perform this check...
             */
            ASSERT(cbuf->head == cbuf->buffer_start);

            /*
             * ...and update the BASE
             */
            cbuf->base = FLOOR(le);

        } else if (cbuf->nreqs == (cbuf->nfreeze+1)) {

            /*
             * First entry after nfreeze entries have been created. Update the
             * HEAD of the circular queue of entries.
             */
            cbuf->head = cbuf->base;
        }

    } else {

        /*
         * A buffer is never really "deleted". On it being freed up, only
         * buffer usage statistics need to be updated...
         *
         * ...update the total number of entries in the circular buffer
         */
        cbuf->bufcnt -= cnt;

        /*
         * ...and update the total size of entries in the circular buffer
         */
        cbuf->bufused -= sz;
    }
}

/*
 * Free list entries so that the space may be reused for the next entry.
 *
 * Parameters
 *   [inout] elf_start
 *     pointer to the pointer to first element to start freeing, e.g. HEAD. If
 *     the list does not run out, return the element next to last free'd
 *     element
 *   [in] top
 *     typically top of the buffer. Indicates when the list wrapped.
 *   [in] free_sz
 *     space to be freed
 *
 * Returns
 *   returns whether required space was freed (TRUE) or not (FALSE)
 *   also, if the list did not run out then returns in elf_start pointer to the
 *   list entry next to the last entry that was freed. Useful for updating HEAD
 *   if elements were freed starting from HEAD.
 */
static int
cbuf_free_les (circular_buffer_t *cbuf, el_t **elf_start, el_t *top, int free_sz)
{
    el_t *ler;
    int found, f_space;
    int ii, delcnt, delsz, avlbl;

    f_space = 0;
    delcnt = 0;
    delsz = 0;
    found = 0;

    /*
     * Start from elf_start; stop once list runs out or wraps
     */

    for (ler = *elf_start; ler && ler >= *elf_start; ler = ler->next) {

        f_space += STORREQ(ler);

        delcnt++;
        delsz += STORREQ(ler);
        ASSERT(STORREQ(ler) == ((char*)FLOOR(ler) - (char*)ler));

        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("((%s) freeing %d,%d f_space %d)"), cbuf->name, OFS(ler), (int)STORREQ(ler), f_space));

        if (f_space >= free_sz) {
            found = 1;
            break;
        }
    }

    CBUF_UNLOCK;

    /*
     * In the loop below we wait for the consumer semaphore which is given by
     * the consumer thread, delcnt number of times. However, this could become
     * an infinite wait if the consumer thread exits prematurely. Hence check
     * for its status every wait period and continue without waiting further if
     * the thread no longer exists.
 */
    for (ii = 0; (cbuf->consumer_thread_id != SAL_THREAD_ERROR) && (ii < delcnt); ii++) {
        if (sal_sem_take(cbuf->consumer, MILLISECOND_USEC) < 0) {
            /*
             * TBD: should add statistics / messages here to record that
             * consumer thread was too slow and buffer allocation logic was
             * stalled because of it. This affects the caller's run-time.
             * Options to avoid this situation? Optimizing the consumer
             * callback for speed of execution, or adding more buffer to absorb
             * burst of requests (API calls).
             */
            ii--;
        }
    }

    CBUF_LOCK;

    LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
              (BSL_META("((%s) total freed %d) "), cbuf->name, delsz));

    /*
     * Accounting stuff - update number of entries and total buffer used,
     * whether we freed up enough space or not.
     */

    cbuf_update_buffer_statistics(cbuf, NULL, delcnt, delsz, 0);

    if (!found) {

        avlbl = (char*)top - (char*)*elf_start;

        if (avlbl >= free_sz) {
            *elf_start = ler;
            found = 1;
        }
    } else {

        *elf_start = ler->next;
    }

    return found;
}


/*
 * The place where actual buffer allocation logic resides. See inline comments
 * for all the cases that exist wrt the request and possible current state of
 * the circular buffer.
 *
 * Parameters
 *   [in] cbuf
 *     control structure for the circular buffer from where to allocate a
 *     buffer to store req bytes
 *   [in] req
 *     requested buffer size
 *
 * Returns
 *    pointer to the requested buffer, or NULL if none allocated 
 */
static inline void*
cbuf_get_buffer_internal (circular_buffer_t *cbuf, int req)
{
    el_t *le, *ler;
    int avlbl;
    int stor_req = REQ2STORREQ(req);

    /*
     * Case 0:
     * Required storage is bigger than the maximum possible available space.
     * Fail immediately.
     */

    if (stor_req > (OFS(cbuf->buffer_end) - OFS(cbuf->base))) {

        cbuf->case_counts[0][0]++;

        LOG_INFO_EVERY("(case 0)");
        cbuf->nfailed_reqs++;
        return NULL;
    }

    /*
     * Case 1:
     * Empty buffer with Zero entries. Allocate from the top of the buffer.
     */

    if (!cbuf->head || !cbuf->tail) {

        cbuf->case_counts[1][0]++;

        le = cbuf->base;
        le->prev = le->next = NULL;
        le->sz = req;
        le->flags_and_guard_pattern &= ~CONSUMED_MASK;
        cbuf->head = cbuf->tail = le;
        LOG_INFO_EVERY("case 1 ... ");

        cbuf_update_buffer_statistics(cbuf, le, 1, stor_req, 1);

        return PLD(le);

    }

    le = FLOOR(cbuf->tail);

    /*
     * Case 2:
     * Buffer Tail is beyond the buffer Head. Storage may be allocated from
     * Tail until the End of buffer; if there is not enough space until End of
     * buffer, then need to start freeing up storage from the Top of the
     * buffer.
     */

    if (cbuf->tail >= cbuf->head) {

        avlbl = (char*)cbuf->buffer_end - (char*)le;

        if (avlbl >= stor_req) {

            /*
             * Case 2.1:
             * There is enough space beyond Tail until End of buffer
             */

            cbuf->case_counts[2][1]++;

            le->prev = cbuf->tail;
            le->next = NULL;
            le->sz = req;
            le->flags_and_guard_pattern &= ~CONSUMED_MASK;
            cbuf->tail->next = le;
            cbuf->tail = le;
            LOG_INFO_EVERY("case 2.1 ... ");
        } else {

            /*
             * Case 2.2,2.3:
             * There wasn't enough space beyond Tail until End of buffer. Start
             * freeing space from the Base of buffer. We also know that the
             * allocated space will be at the Base of the buffer.
             */

            ASSERT(cbuf->head == cbuf->base);
            le = cbuf->base;
            ler = cbuf->base;

            if (cbuf_free_les(cbuf, &ler, cbuf->buffer_end, stor_req)) {

                if (ler) {

                    /*
                     * Case 2.2:
                     * While freeing up space the list as not emptied, i.e.
                     * there are still valid entries left in the buffer
                     */

                    cbuf->case_counts[2][2]++;

                    ler->prev = cbuf->head->prev;
                    if (cbuf->head->prev) {
                        cbuf->head->prev->next = ler;
                    }
                    cbuf->head = ler;
                    le->prev = cbuf->tail;
                    cbuf->tail->next = le;
                    LOG_INFO_EVERY("case 2.2 ... ");
                } else {

                    /*
                     * Case 2.3:
                     * While freeing up space the list was emptied, i.e.  there
                     * are no more valid entries left in the buffer
                     */

                    cbuf->case_counts[2][3]++;

                    le->prev = cbuf->head->prev;
                    if (cbuf->head->prev) {
                        cbuf->head->prev->next = le;
                    }
                    cbuf->head = le;
                    LOG_INFO_EVERY("case 2.3 ... ");
                }
                le->next = NULL;
                le->sz = req;
                le->flags_and_guard_pattern &= ~CONSUMED_MASK;
                cbuf->tail = le;
            } else {

                /*
                 * Case 2.4:
                 * This should actually never happen because the Top of Buffer
                 * was passed to the free buffers routine above. If there
                 * wasn't enough space from the Base of the buffer to the Top
                 * of buffer then Case 0 should have bailed us out, and control
                 * will never reach here.
                 */

                cbuf->case_counts[2][4]++;

                if (cbuf->head->prev) {
                    cbuf->head->prev->next = le;
                }
                le->prev = cbuf->head->prev;
                le->next = NULL;
                le->sz = req;
                le->flags_and_guard_pattern &= ~CONSUMED_MASK;
                cbuf->head = cbuf->tail = le;
                LOG_INFO_EVERY("case 2.4 ... ");
            }
        }

        cbuf_update_buffer_statistics(cbuf, le, 1, stor_req, 1);

        return PLD(le);

    } else {

        /*
         * Case 3:
         * Head is beyond Tail, the buffer definitely wrapped. First check
         * whether there's enough space beyond Tail until Head. If not, start
         * freeing space from Head towards the Top of buffer. If thi also does
         * not help, start freeing from Base towards the Top of buffer.
         */

        avlbl = (char*)cbuf->head - (char*)FLOOR(cbuf->tail);

        if (avlbl >= stor_req) {

            /*
             * Case 3.1:
             * There was enough space from Tail until Head. No entries needed
             * to be freed
             */

            cbuf->case_counts[3][1]++;

            le->prev = cbuf->tail;
            le->next = NULL;
            le->sz = req;
            le->flags_and_guard_pattern &= ~CONSUMED_MASK;
            cbuf->tail->next = le;
            cbuf->tail = le;
            LOG_INFO_EVERY("case 3.1 ... ");
        } else {

            ler = cbuf->head;

            /*
             * Case 3.2:
             * Start freeing up required additional space from Head towards Top
             * of buffer. Not guaranteed to succeed.
             */

            if (cbuf_free_les(cbuf, &ler, cbuf->buffer_end, stor_req - avlbl)) {

                if (ler) {

                    /*
                     * Case 3.2:
                     * the list did not run out before finding the required space.
                     */

                    cbuf->case_counts[3][2]++;

                    ler->prev = cbuf->head->prev;
                    if (cbuf->head->prev) {
                        cbuf->head->prev->next = ler;
                    }
                    cbuf->head = ler;
                    LOG_INFO_EVERY("case 3.2 ... ");
                } else {

                    /*
                     * Case 3.3:
                     * the list ran out, so space was found until Top of
                     * buffer. This should never happen because here Head is
                     * beyond Tail.
                     */

                    cbuf->case_counts[3][3]++;

                    le = cbuf->base;
                    LOG_INFO_EVERY("case 3.3 ... ");
                }
                ASSERT(ler);
                le->prev = cbuf->tail;
                le->next = NULL;
                le->sz = req;
                le->flags_and_guard_pattern &= ~CONSUMED_MASK;
                cbuf->tail->next = le;
                cbuf->tail = le;

            } else {

                /*
                 * Case 4:
                 * Required space was not found. All entries from Head until
                 * end of buffer were deleted by the free routine. Start
                 * freeing from the Base of buffer.
                 */

                le = cbuf->base;
                ler = cbuf->base;

                if (cbuf_free_les(cbuf, &ler, cbuf->buffer_end, stor_req)) {

                    if (ler) {

                        /*
                         * Case 4.1:
                         * While freeing from Base, the list did not run out.
                         */

                        cbuf->case_counts[4][1]++;

                        ler->prev = cbuf->head->prev;
                        if (cbuf->head->prev) {
                            cbuf->head->prev->next = ler;
                        }
                        cbuf->head = ler;
                        le->prev = cbuf->tail;
                        cbuf->tail->next = le;
                        LOG_INFO_EVERY("case 4.1 ... ");
                    } else {

                        /*
                         * Case 4.2:
                         * While freeing from Base, the list ran out. Update Head.
                         */

                        cbuf->case_counts[4][2]++;

                        le->prev = cbuf->head->prev;
                        if (cbuf->head->prev) {
                            cbuf->head->prev->next = le;
                        }
                        cbuf->head = le;
                        LOG_INFO_EVERY("case 4.2 ... ");
                    }
                    le->next = NULL;
                    le->sz = req;
                    le->flags_and_guard_pattern &= ~CONSUMED_MASK;
                    cbuf->tail = le;
                } else {

                    /*
                     * Case 4.3:
                     * This should never happen because we started looking for
                     * space starting from Base of buffer until End of buffer.
                     * If required space was not available then Case 0 should
                     * have bailed us out.
                     */

                    cbuf->case_counts[4][3]++;

                    LOG_INFO_EVERY("case 4.3 ... ");
                }

            }

        }

        cbuf_update_buffer_statistics(cbuf, le, 1, stor_req, 1);

        return PLD(le);

    }

    cbuf->case_counts[5][0]++;

    LOG_INFO_EVERY("(case 5)");
    cbuf->nfailed_reqs++;
    return NULL;
}

/*
 * Consistency checks over the entire circular buffer. Involves list walks over
 * all entries in the circular buffer. Check inline comments for all the
 * consistency checks that are performed.
 *
 * Parameters
 *   [in] cbuf
 *     circular_buffer_t over which to run the consistency checks
 *
 * Returns
 *   None
 *
 * This check is performed only when requested by the caller of cbuf_get_buffer
 * with the do_checks boolean set to TRUE.
 */
void cbuf_do_consistency_checks (circular_buffer_t *cbuf)
{
    el_t *le;
    int fwdcnt, bckcnt;
    int fwdused, bckused;
    uint32 header_guard_pattern, footer_guard_pattern;

    /*
     * of TAIL must be NULL
     */

    if (cbuf->tail->next) {
        LOG_INFO_EVERY("tail->next is no longer NULL = %d, %p\n", OFS(cbuf->tail->next), (void*)(cbuf->tail->next));
    }
    ASSERT((cbuf->tail->next == NULL));

    /*
     * 1. count number of entries in the list starting from start-of-buffer
     * i.e. the first entry ever created, if NFREEZE was not ZERO. Else start
     * from the HEAD. This count must be same as the running counter BUFCNT
     *
     * 2. While counting also add up the buffer used by each list entry. This
     * sum must be same as the running counter BUFUSED
     *
     * 3. FLOOR of each entry must be exactly at offset STORREQ from it
     *
     * 4. if entry has a next entry which is at a higher address than the
     * current entry and that next entry is not the current HEAD, then the next
     * entry must be at the FLOOR of current entry.
     *
     * 5. check guard patterns of each entry for buffer overflows
 */

    for (fwdcnt = 0, fwdused = 0, le = (cbuf->nfreeze) ? cbuf->buffer_start : cbuf->head;
         le;
         fwdcnt++, fwdused += STORREQ(le), le = le->next) {

        ASSERT(STORREQ(le) == ((char*)FLOOR(le) - (char*)le));

        if (le->next && (le->next > le) && (le->next != cbuf->head)) {
            ASSERT(FLOOR(le) == le->next);
        }

        header_guard_pattern = (le->flags_and_guard_pattern & ~CONSUMED_MASK);
        footer_guard_pattern = *(uint32*)(PLD(le) + le->sz);

        if (header_guard_pattern != GUARD_PATTERN_1) {
            LOG_INFO_EVERY("fwd(%d,%d):found pattern 0x%08x instead of 0x%08x\n", fwdcnt, OFS(le), header_guard_pattern, GUARD_PATTERN_1);
        }
        ASSERT(header_guard_pattern == GUARD_PATTERN_1);

        if (footer_guard_pattern != GUARD_PATTERN_2) {
            LOG_INFO_EVERY("fwd(%d,%d):found pattern 0x%08x instead of 0x%08x\n", fwdcnt, OFS(le), footer_guard_pattern, GUARD_PATTERN_2);
        }
        ASSERT(footer_guard_pattern == GUARD_PATTERN_2);
    }

    if (fwdcnt != cbuf->bufcnt) {
        LOG_INFO_EVERY("fwdcnt = %d, BUFCNT = %d\n", fwdcnt, cbuf->bufcnt);
    }
    ASSERT(fwdcnt == cbuf->bufcnt);

    if (fwdused != cbuf->bufused) {
        LOG_INFO_EVERY("fwdused = %d, BUFUSED = %d\n", fwdused, cbuf->bufused);
    }
    ASSERT(fwdused == cbuf->bufused);

    /*
     * 1. count number of entries in the list starting from TAIL, i.e. the last
     * entry created. This count must be same as the running counter BUFCNT
     *
     * 2. While counting also add up the buffer used by each list entry. This
     * sum must be same as the running counter BUFUSED
     *
     * 3. FLOOR of each entry must be exact at offset STORREQ from it
     *
     * 4. if the current entry has a previous entry which is at a lower address
     * than the current entry and the current entry is not the current HEAD,
     * then the current entry must be at the FLOOR of the previous entry.
     *
     * 5. check guard patterns of each entry for buffer overflows
 */
    for (bckcnt = 0, bckused = 0, le = cbuf->tail;
         le;
         bckcnt++, bckused += STORREQ(le), le = le->prev) {

        ASSERT(STORREQ(le) == ((char*)FLOOR(le) - (char*)le));

        if (le->prev && (le->prev < le) && (le != cbuf->head)) {
            ASSERT(FLOOR(le->prev) == le);
        }

        header_guard_pattern = (le->flags_and_guard_pattern & ~CONSUMED_MASK);
        footer_guard_pattern = *(uint32*)(PLD(le) + le->sz);

        if (header_guard_pattern != GUARD_PATTERN_1) {
            LOG_INFO_EVERY("bck(%d,%d):found pattern 0x%08x instead of 0x%08x\n", bckcnt, OFS(le), header_guard_pattern, GUARD_PATTERN_1);
        }
        ASSERT(header_guard_pattern == GUARD_PATTERN_1);

        if (footer_guard_pattern != GUARD_PATTERN_2) {
            LOG_INFO_EVERY("bck(%d,%d):found pattern 0x%08x instead of 0x%08x\n", bckcnt, OFS(le), footer_guard_pattern, GUARD_PATTERN_2);
        }
        ASSERT(footer_guard_pattern == GUARD_PATTERN_2);

        if (!le->prev && cbuf->nfreeze) {

            /*
             * end of the backward walk should be at the start-of-buffer where
             * the first entry ever created was allocated, if NFREEZE is not
             * ZERO.
             */
            ASSERT(le == cbuf->buffer_start);
        }
    }

    if (bckcnt != cbuf->bufcnt) {
        LOG_INFO_EVERY("bckcnt = %d, BUFCNT = %d\n", bckcnt, cbuf->bufcnt);
    }
    ASSERT(bckcnt == cbuf->bufcnt);

    if (bckused != cbuf->bufused) {
        LOG_INFO_EVERY("bckused = %d, BUFUSED = %d\n", bckused, cbuf->bufused);
    }
    ASSERT(bckused == cbuf->bufused);

    /*
     * the running statistic of buffer used (in bytes) must not be negative or
     * bigger than the total buffer available.
     */
    if (!((cbuf->bufused >= 0) && (cbuf->bufused <= cbuf->totalsz))) {
        LOG_INFO_EVERY("BUFUSED,TOTALSZ = %d,%d\n", cbuf->bufused, cbuf->totalsz);
    }
    ASSERT((cbuf->bufused >= 0) && (cbuf->bufused <= cbuf->totalsz));
}

/*
 * Top-level function to request a buffer from a circular_buffer_t
 *
 * Parameters
 *   [in] cbuf
 *     circular_buffer_t from which to allocate a buffer
 *   [in] req
 *     size of the buffer to allocate
 *   [in] do_checks
 *     whether to perform consistency checks over the entire circular_buffer_t
 *
 * Returns
 *   pointer to allocated buffer, or NULL on failure
 */
void*
cbuf_get_buffer (circular_buffer_t *cbuf, int req, int do_checks)
{
    void *buf;
    el_t *le;

    cbuf->count++;

    LOG_INFO_EVERY("%5llu (%3d,%5d): allocating %5d bytes buffer ... ", cbuf->count, cbuf->bufcnt, cbuf->bufused, req);

    CBUF_LOCK;

    buf = cbuf_get_buffer_internal(cbuf, req);

    if (buf) {

        le = PLD2LE(buf);

        ASSERT(le == cbuf->tail);

        /*
         * Set guard patterns at head and foot of the memory allocated for the
         * client of circular buffer infrastructure to fill up
         */
        le->flags_and_guard_pattern   &= CONSUMED_MASK;
        le->flags_and_guard_pattern   |= (GUARD_PATTERN_1 & ~CONSUMED_MASK);
        *((uint32*)(PLD(le) + le->sz)) = GUARD_PATTERN_2;
    }

    if (!buf || !do_checks) {
        CBUF_UNLOCK;
        return buf;
    }

    /*
     * consistency checks and related outputs
     *
     * first, offset of the returned buffer plus status of HEAD, TAIL, BASE,
     * CONSUMER, BUFUSED and BUFCNT
     */

    LOG_INFO_EVERY("offset = %5d (head = %d (%d,%d,%d) tail = %d (%d,%d,%d) base = %d consumer = %d) (%d,%d)\n", OFS(buf),
           OFS(cbuf->head), OFS(cbuf->head->next), OFS(cbuf->head->prev), OFS(FLOOR(cbuf->head)),
           OFS(cbuf->tail), OFS(cbuf->tail->next), OFS(cbuf->tail->prev), OFS(FLOOR(cbuf->tail)),
           OFS(cbuf->base), OFS(cbuf->cle),
           cbuf->bufcnt, cbuf->bufused);

    /*
     * offset of the returned pointer should be within the total circular
     * buffer space
     */

    ASSERT((OFS(buf) >= 0) && (OFS(buf) < cbuf->totalsz));

    /*
     * returned buffer must be the tail of the circular buffer
     */

    ASSERT(OFS(buf) == OFS(PLD(cbuf->tail)));

    cbuf_do_consistency_checks(cbuf);

    CBUF_UNLOCK;

    return buf;
}

/*
 * A buffer requested using cbuf_get_buffer once filled up by the application
 * must be acknowledged using cbuf_get_buffer_done. This indicates to circular
 * buffer infrastructure that the buffer is ready and any consumers may now
 * read it.
 *
 * Parameters
 *   [in] cbuf
 *     circular_buffer_t of which the buffer is now ready
 *   [in] buf
 *     buffer which is ready for consumption
 *
 * Returns
 *   None
 */
void
cbuf_get_buffer_done (circular_buffer_t *cbuf, void *buf)
{
    el_t *le;
    uint32 header_guard_pattern, footer_guard_pattern;

    if (buf != PLD(cbuf->tail)) {
        LOG_INFO_EVERY("FATAL: buffer is not at the TAIL\n");
    }
    ASSERT(buf == PLD(cbuf->tail));

    le = PLD2LE(buf);

    header_guard_pattern = (le->flags_and_guard_pattern & ~CONSUMED_MASK);
    footer_guard_pattern = *(uint32*)(PLD(le) + le->sz);

    if (header_guard_pattern != GUARD_PATTERN_1) {

        LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL(%s): HEADER guard pattern was corrupted (0x%08x vs 0x%08x expected)\n"),
                   cbuf->name, header_guard_pattern, GUARD_PATTERN_1));
    }
    ASSERT(header_guard_pattern == GUARD_PATTERN_1);

    if (footer_guard_pattern != GUARD_PATTERN_2) {

        LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL(%s): FOOTER guard pattern was corrupted (0x%08x vs 0x%08x expected)\n"),
                   cbuf->name, footer_guard_pattern, GUARD_PATTERN_2));
    }
    ASSERT(footer_guard_pattern == GUARD_PATTERN_2);

    CBUF_LOCK;

    if (!cbuf->cle) {
        cbuf->cle = cbuf->tail;
    }

    CBUF_UNLOCK;

    sal_sem_give(cbuf->producer);
}

/*
 * For every circular_buffer_t created, a consumer thread is also created which
 * "consumes" each entry added to its circular queue. If a callback is
 * registered, payload of each entry is passed to the callback before marking
 * it as consumed. This function implements this consumer logic. Entries which
 * are not yet consumed cannot be reclaimed for storing newer entries,
 * therefore callback must be efficient in order to not block the
 * cbuf_get_buffer requests.
 *
 * Parameters
 *   Same as required by sal_thread_create
 *
 * Returns
 *   None
 */
static void
cbuf_consumer_thread (void *arg)
{
    circular_buffer_t *cbuf = arg;
    int consumed = 0;

    do {

        if (sal_sem_take(cbuf->producer, sal_sem_FOREVER) < 0) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR(%s): error taking PRODUCER semaphore"), cbuf->name));
            break;
        }

        if (!cbuf->cle) {

            if (cbuf->done) {
                cbuf->consumer_thread_id = SAL_THREAD_ERROR;
                return;
            }

            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("(ERROR(%s): consumer element is NULL)"), cbuf->name));
            break;
        }

        if (cbuf->cle->flags_and_guard_pattern & CONSUMED_MASK) {

            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("(ERROR(%s): found an already consumed entry %d)"), cbuf->name, OFS(cbuf->cle)));
            break;
        }

        if (cbuf->consumer_cb) {

            cbuf->consumer_cb(cbuf->consumer_opaque, cbuf, PLD(cbuf->cle), cbuf->cle->sz);
        }

        CBUF_LOCK;

        cbuf->cle->flags_and_guard_pattern |= CONSUMED_MASK;

        cbuf->cle = cbuf->cle->next;

        CBUF_UNLOCK;

        consumed++;

        if (consumed > cbuf->nfreeze) {

            sal_sem_give(cbuf->consumer);
        }

    } while (TRUE);

    LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
              (BSL_META("(ERROR(%s): consumer thread exiting)"), cbuf->name));

    cbuf->consumer_thread_id = SAL_THREAD_ERROR;
}

/*
 * Create a new circular buffer with entries copied from the provided circular
 * buffer.
 *
 * Parameters
 *   [in] cbuf
 *     Source circular buffer from which to copy entries
 *   [in] offset
 *     Number of entries to skip from the beginning/end of the circular queue
 *     before copying |count| i.e. absolute value of count,  number of
 *     entries.
 *   [in] count
 *     Number of entries to copy. A positive value means from the HEAD,
 *     negative means from TAIL.
 *   [in] copy_f
 *     Pointer to a function which caller must supply... this function should
 *     copy the entry and update the copy if required, for example updating
 *     pointers. This could be NULL. Entry size must not increase
 *
 * Following parameters are the same as for circular_buffer_t_create and are
 * passed as-is to that API.
 *
 *   [in] ocbuf_name
 *     Name to assign to the copy circular buffer
 *   [in] consumer_thread_stksz
 *     Stack size to assign for consumer thread
 *   [in] consumer_thread_prio
 *     A consumer thread is spawned for each circular_buffer_t. This provides
 *     the priority of the consumer thread
 *   [in] consumer_cb
 *     Callback function to invoke for each entry from the consumer thread,
 *     before marking the entry "consumed" and therefore ready to be reclaimed
 *     for newer entries.
 *   [in] consumer_opaque
 *     Opaque value which is passed as-is to the consumer_cb
 *
 * If a copy is successfully created, pointer to that copy is returned. This is
 * a circular buffer which never wrapped. Obviously the actual number of
 * entries in it may be less than |count|, e.g. when there aren't as many
 * entries in the source buffer to begin with.
 */
circular_buffer_t *
cbuf_splice_entries (circular_buffer_t *cbuf,
                     int offset, int count,
                     circular_buffer_entry_copy_cb_t copy_f,
                     char *ocbuf_name,
                     int consumer_thread_stksz,
                     int consumer_thread_prio,
                     circular_buffer_consumer_cb_t consumer_cb,
                     void *consumer_opaque)
{
    circular_buffer_t *ocbuf = NULL;
    int stor_req, count_save;
    el_t *ler, *copy_head;
    void *buf;
    cint_time_t t1, t2, rt;

    if (!cbuf) {

        LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
                 (BSL_META("INFO: no source cbuf, no copy will be created\n")));
        return NULL;
    }

    if (!cbuf->bufcnt) {

        LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
                 (BSL_META("INFO: no cbuf has no entries, no copy will be created\n")));
        return NULL;
    }

    stor_req = 0;

    copy_head = NULL;

    CBUF_LOCK;

    cint_gettime(&t1);

    if (count > 0) {

        count = count_save = count;

        for (ler = (cbuf->nfreeze) ? cbuf->buffer_start : cbuf->head;
             ler && count_save;
             ler = ler->next) {

            if (offset) {
                offset--;
                continue;
            }

            if (!copy_head) {
                copy_head = ler;
            }

            stor_req += STORREQ(ler);

            count_save--;
        }

    } else {

        count = count_save = -count;

        for (ler = cbuf->tail; ler && count_save; ler = ler->prev) {

            if (offset) {
                offset--;
                continue;
            }

            stor_req += STORREQ(ler);

            count_save--;

        }

        if (ler) {

            copy_head = ler->next;

        } else if (!offset) {

            copy_head = (cbuf->nfreeze) ? cbuf->buffer_start : cbuf->head;

        }

    }

    if (!stor_req) {
        LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
                 (BSL_META("INFO: computed storage required is 0, no copy will be created\n")));
        CBUF_UNLOCK;
        return NULL;
    }


    ocbuf = cbuf_circular_buffer_t_create(ocbuf_name, stor_req, 0,
                                          consumer_thread_stksz,
                                          consumer_thread_prio,
                                          consumer_cb,
                                          consumer_opaque);

    if (!ocbuf) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR: failed to allocate circular buffer %s of size %d\n"),
                   ocbuf_name, stor_req));
        CBUF_UNLOCK;
        return NULL;
    }

    count = count_save = count - count_save;

    LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
             (BSL_META("INFO: splicing %d entries from cbuf \"%s\" to new cbuf \"%s\", size %d\n"),
              count, cbuf->name, ocbuf->name, ocbuf->totalsz));

    for (ler = copy_head; ler && count_save; ler = ler->next, count_save--) {

        buf = cbuf_get_buffer(ocbuf, ler->sz, 0);

        if (!buf) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR: failed to allocate buffer of size %d from circular buffer \"%s\"\n"),
                       ler->sz, ocbuf->name));
            cbuf_circular_buffer_t_destroy(ocbuf);
            CBUF_UNLOCK;
            return NULL;
        }

        if (copy_f) {

            copy_f(buf, PLD(ler), ler->sz);

        } else {

            CINT_MEMCPY(buf, PLD(ler), ler->sz);
        }

        cbuf_get_buffer_done(ocbuf, buf);
    }

    cint_gettime(&t2);

    CBUF_UNLOCK;

    cint_timersub(&t2, &t1, &rt);

    LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
             (BSL_META("INFO: splicing %d entries from cbuf \"%s\" to new cbuf \"%s\", size %d ... Done (%ld.%06ld seconds)\n"),
              count, cbuf->name, ocbuf->name, ocbuf->totalsz, (long)rt.tv_sec, rt.tv_usec));

    return ocbuf;
}


/*
 * Main function clients of this circular buffer infrastructure will use to
 * create a circular_buffer_t.
 *
 * Parameters
 *   [in] name
 *     A handy name for the circular buffer being allocated
 *   [in] size
 *     Size of the circular buffer. This is malloc'd
 *   [in] nfreeze
 *     User can elect to make first nfreeze entries permanent; i.e. those are
 *     never reclaimed for storing newer entries. This can be Zero.
 *   [in] consumer_thread_stksz
 *     Stack size to assign for consumer thread
 *   [in] consumer_thread_prio
 *     A consumer thread is spawned for each circular_buffer_t. This provides
 *     the priority of the consumer thread
 *   [in] consumer_cb
 *     Callback function to invoke for each entry from the consumer thread,
 *     before marking the entry "consumed" and therefore ready to be reclaimed
 *     for newer entries.
 *   [in] consumer_opaque
 *     Opaque value which is passed as-is to the consumer_cb
 *
 * Returns
 *   Pointer to the handle for newly created circular_buffer_t
 */

circular_buffer_t *
cbuf_circular_buffer_t_create (char* name, int size, int nfreeze,
                               int consumer_thread_stksz,
                               int consumer_thread_prio,
                               circular_buffer_consumer_cb_t consumer_cb,
                               void *consumer_opaque)
{
    circular_buffer_t *cbuf;

    cbuf = CINT_MALLOC(sizeof(*cbuf));
    if (!cbuf) {
        return NULL;
    }
    CINT_MEMSET(cbuf, 0, sizeof(*cbuf));

    cbuf->done = 0;
    cbuf->name = name;

    cbuf->buffer_start = CINT_MALLOC(size);
    CINT_MEMSET(cbuf->buffer_start, 0, size);
    cbuf->base = cbuf->buffer_start;
    cbuf->buffer_end = (char*)cbuf->buffer_start + size;
    cbuf->totalsz = size;
    cbuf->nfreeze = nfreeze;

    cbuf->consumer_cb = consumer_cb;
    cbuf->consumer_opaque = consumer_opaque;
    cbuf->consumer_thread_id = SAL_THREAD_ERROR;


    cbuf->mutex = sal_mutex_create("cbuf_mutex");
    cbuf->producer = sal_sem_create("cbuf_producer_sem", FALSE, 0);
    cbuf->consumer = sal_sem_create("cbuf_consumer_sem", FALSE, 0);

    cbuf->consumer_thread_id = sal_thread_create(name,
                                                 consumer_thread_stksz,
                                                 consumer_thread_prio,
                                                 cbuf_consumer_thread, cbuf);

    return cbuf;
}

/*
 * Destroy routine for a circular_buffer_t which was previously allocated using
 * the _create function.
 *
 * Parameters
 *   [in] cbuf
 *     circular_buffer_t to destroy.
 *
 * Returns
 *   None
 */
void
cbuf_circular_buffer_t_destroy (circular_buffer_t *cbuf)
{
    if (!cbuf) {
        return;
    }

    cbuf->done = 1;

    sal_sem_give(cbuf->producer);

    while (cbuf->consumer_thread_id != SAL_THREAD_ERROR) {
        sal_usleep(10*MILLISECOND_USEC);
    }

    if (cbuf->consumer) {
        sal_sem_destroy(cbuf->consumer);
    }

    if (cbuf->producer) {
        sal_sem_destroy(cbuf->producer);
    }

    if (cbuf->mutex) {
        sal_mutex_destroy(cbuf->mutex);
    }

    if (cbuf->buffer_start) {
        CINT_FREE(cbuf->buffer_start);
    }

    CINT_FREE(cbuf);
}

/*
 * Fill the supplied buffer with random bytes.
 *
 * Parameters
 *   [in] buffer
 *     Buffer to fill with random bytes
 *   [in] len
 *     Number of random bytes to fill
 *   [in] isstring
 *     Whether random byte must be printable characters.
 *
 * Returns
 *   None
 */
static void
cint_logger_fill_random (void *buffer, int len, int isstring)
{
   uint8 c, *dst;

   for (dst = buffer; dst && (len > 0); ) {

        c = CINT_RAND() & 0xFF;

        *dst = c;
        dst++;
        len--;
   }
}

/*
 * The consumer_cb for the circular buffer test.
 *
 * Parameters
 *   Same as any consumer_cb
 *
 * Returns
 *   None
 */
static void
cbuf_circular_buffer_test_verifier (void *opaque, circular_buffer_t* cbuf, void *buf, int bufsz)
{
    unsigned int sv = *(unsigned int*) buf;
    circular_buffer_test_t *ti = opaque;

    if (ti->cons != sv) {

        ti->mismatches++;

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("(ERROR(%s): stored value %u , expected %u -- %u mismatches so far)"), cbuf->name, sv, ti->cons, ti->mismatches));
    }

    LOG_INFO_EVERY("(processed %u)", sv);

    ti->cons++;
}

/*
 * Test function invoked from a shell command for testing the circular buffer
 * infrastructure. The test strategy involves the following:
 *
 * 1. Creates circular_buffer with user supplied attributes - size, nfreeze,
 * thread_prio.
 *
 * 2. allocates random sized buffers from the circular_buffer
 *
 * 3. Maintains a running count of produced and consumed entries. The current
 * produced count is written to each entry in the test function.
 *
 * 4. In the consumer_cb count in the entry is compared against the current
 * consumed count - these must match.
 * 
 * 5. cbuf_get_buffer_internal has several cases for handling buffer allocation
 * given the buffer size requested and current status of HEAD and TAIL
 * pointers in the circular queue. case_counts are incremented for each case
 * that was encountered.
 *
 * 6. Finally the test-case reports the case counts, whether the consumer found
 * any unexpected/out of order entries, and whether the number of produced and
 * consumed counts are equal.
 *
 * Expectation is that all cases will be tested, some cases should never occur
 * and their case counts must be ZERO, and produced and consumed counts should
 * be equal with no mismatches. Finally, the allocated circular_buffer_t is
 * destroyed.
 *
 * Parameters
 *   [in] totalsz
 *     Size of the circular buffer to allocate
 *   [in] nfreeze
 *     Number of initial entries which should be permanent in that they are not
 *     reclaimed for storing newer entries.
 *   [in] alloc
 *     Basic allocation unit. Several multiples and fractions of this basic
 *     allocation unit are randomly requested to test the circular buffer code 
 *   [in] iter
 *     Number of random sized buffer requests to place
 *   [in] pe
 *     print_every...since iter can be in millions, test logs are printed only
 *     every print_every iterations. This significantly speeds up the test run.
 *   [in] consumer_thread_prio
 *     priority to indicate for the consumer thread. NOTE: Unix SAL ignores
 *     this input. It is retained here only for those systems that support it.
 *   [in] seed
 *     Seed for the random number generator. If ZERO, seed is set to sal_time()
 *     which is seconds since epoch.
 *
 * Returns
 *   CMD_OK / CMD_FAIL indicating whether the test-case passed or failed
 */
cmd_result_t
cbuf_circular_buffer_test (int totalsz, int nfreeze, int alloc,
                           int iter, int pe,
                           int consumer_thread_prio, int seed)
{
    circular_buffer_t *cbuf;
    int ii, req, diff;
    char *buf;
    circular_buffer_test_t test_info = { 0, 0, 0 };
    cmd_result_t result = CMD_OK;

    cbuf = cbuf_circular_buffer_t_create("cbtest buffer", totalsz, nfreeze,
                                         SAL_THREAD_STKSZ,
                                         consumer_thread_prio,
                                         cbuf_circular_buffer_test_verifier,
                                         &test_info);


    if (seed) {
        cli_out("RANDOM NUMBER SEED = %d\n", seed);
        sal_srand(seed);
    }

    CINT_MEMSET(&cbuf->case_counts, 0, sizeof(cbuf->case_counts));

    cbuf->count = 0;
    cbuf->print_every = pe;

    for (ii = 0; ii < iter; ii++) {

        req = (sal_rand() % alloc);

        switch (sal_rand() % 15) {
        case 0: req *= 1; break;
        case 1: req *= 2; break;
        case 2: req *= 3; break;
        case 3: req *= 4; break;
        case 4: req *= 5; break;
        case 5: req /= 5; break;
        case 6: req /= 4; break;
        case 7: req /= 3; break;
        case 8: req /= 2; break;
        case 9:
            if (cbuf->bufcnt >= cbuf->nfreeze) {
                req = STORREQ2REQ(cbuf->totalsz - ((int)((char*)cbuf->base - (char*)cbuf->buffer_start)));

                switch (sal_rand() % 4) {
                case 0: req = (req*9)/10; break;
                case 1: req = (req*8)/10; break;
                case 2: req = (req*7)/10; break;
                case 3: default: req = req + 1; break;
                }
            } else {
                req /= 1;
            }
            break;
        case 10: req /= 10; break;
        case 11: req /= 9; break;
        case 12: req /= 8; break;
        case 13: req /= 7; break;
        case 14: req /= 6; break;
        }

        if (req < sizeof(int)) {
            ii--;
            continue;
        }

        buf = cbuf_get_buffer(cbuf, req, 1);
        if (!buf) {
            LOG_INFO_EVERY("failed to allocate buffer: %d bytes\n", req);
            continue;
        }

        cint_logger_fill_random(buf, req, 0);

        *(unsigned int*)buf = test_info.prod;

        cbuf_get_buffer_done(cbuf, buf);

        test_info.prod++;
    }

    cli_out("Case 0      : %llu\n", cbuf->case_counts[0][0]);
    cli_out("Case 1      : %llu\n", cbuf->case_counts[1][0]);
    cli_out("Case 2.1    : %llu\n", cbuf->case_counts[2][1]);
    cli_out("Case 2.2    : %llu\n", cbuf->case_counts[2][2]);
    cli_out("Case 2.3    : %llu\n", cbuf->case_counts[2][3]);
    cli_out("Case 2.4   !: %llu\n", cbuf->case_counts[2][4]);
    cli_out("Case 3.1    : %llu\n", cbuf->case_counts[3][1]);
    cli_out("Case 3.2    : %llu\n", cbuf->case_counts[3][2]);
    cli_out("Case 3.3   !: %llu\n", cbuf->case_counts[3][3]);
    cli_out("Case 4.1    : %llu\n", cbuf->case_counts[4][1]);
    cli_out("Case 4.2    : %llu\n", cbuf->case_counts[4][2]);
    cli_out("Case 4.3   !: %llu\n", cbuf->case_counts[4][3]);
    cli_out("Case 5     !: %llu\n", cbuf->case_counts[5][0]);

    /*
     * Cases 2.4, 3.3, 4.3 and 5.0 should not occur, by design
     */
    if (cbuf->case_counts[2][4] ||
        cbuf->case_counts[3][3] ||
        cbuf->case_counts[4][3] ||
        cbuf->case_counts[5][0]) {

        cli_out("FAILED: unexpected case was hit, check statistics above.\n");
        result = CMD_FAIL;
    }

    /*
     * All other cases must have been encountered...
     */
    if (!(cbuf->case_counts[0][0] &&
          cbuf->case_counts[1][0] &&
          cbuf->case_counts[2][1] &&
          cbuf->case_counts[2][2] &&
          cbuf->case_counts[2][3] &&
          cbuf->case_counts[3][1] &&
          cbuf->case_counts[3][2] &&
          cbuf->case_counts[4][1] &&
          cbuf->case_counts[4][2])) {

        cli_out("FAILED: all cases were not tested, recheck inputs including iterations\n");
        result = CMD_FAIL;
    }

    /*
     * Consumer statistics
     */
    if (cbuf->consumer_thread_id == SAL_THREAD_ERROR) {
        cli_out("FAILED: consumer thread had exitted!!\n");
        result = CMD_FAIL;
    }

    if (test_info.mismatches != 0) {
        cli_out("FAILED: mismatched count is not equal to ZERO\n");
        result = CMD_FAIL;
    }

    diff = test_info.prod - test_info.cons;

    if (diff) {

        cli_out("INFO: number of produced and consumed do not match. (prod - cons = %d)\n", diff);

        if (cbuf->consumer_thread_id != SAL_THREAD_ERROR) {

            cli_out("INFO: give a chance to consumer to finish... wait at least 10 seconds before giving up\n");
        }

        for (ii = 0; (cbuf->consumer_thread_id != SAL_THREAD_ERROR) && diff && (ii < 1000); ii++) {

            sal_usleep(10*MILLISECOND_USEC);
            diff = test_info.prod - test_info.cons;
        }

        cli_out("\n");

        if (diff) {
            cli_out("FAILED: number of produced and consumed objects differ (prod - cons = %d)\n", diff);
            result = CMD_FAIL;
        }
    }

    cli_out("Produced    : %u\n", test_info.prod);
    cli_out("Consumed    : %u\n", test_info.cons);
    cli_out("Mismatches !: %u\n", test_info.mismatches);

    cbuf_circular_buffer_t_destroy(cbuf);

    return result;

}

/* } */
#else
/* { */
/* Make ISO compilers happy. */
typedef int cint_logger_circular_buffer_c_not_empty;
/* } */
#endif
