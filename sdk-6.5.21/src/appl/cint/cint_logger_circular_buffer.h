/*
 * $Id: cint_logger_circular_buffer.h
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_circular_buffer.h
 * Purpose:     Header for CINT logger circular buffer infra, see
 *              cint_logger_circular_buffer.c
 */

#ifndef __CINT_LOGGER_CIRCULAR_BUFFER_H__
/* { */
#define __CINT_LOGGER_CIRCULAR_BUFFER_H__

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

/*
 * List Element structure. Each Entry in the Circular Buffer is linked in a
 * doubly linked list using this structure.
 *
 * Following diagram shows the contents of a single variable sized entry,
 * including members of the list element structure and the guard fields.
 * Clients of the circular buffer infrastructure receive a pointer to the
 * client memory space to fill up with their data, and the guard patterns are
 * present so that the infra can detect any memory overrun events. Contents of
 * the client memory space are not processed / accessed by this infrastructure
 *
 * Every Entry in the Circular Buffer infrastructure.
 *
 *
 *     ^       +---------------------------+       ^
 *     |       |          prev             |       |
 *             |         (el_t*)           |       |
 *     l       +---------------------------+       |
 *     i       |          next             |       |
 *     s       |         (el_t*)           |       |
 *     t       +---------------------------+       |
 *             |          size             |       |
 *     e       |        (uint32)           |       |
 *     l       +---------------------------+       |
 *     e       |          flags            |       |
 *     m       |      (uint32 MS 4b)       |       |
 *             +---------------------------+       |
 *     |       |       guard_pattern       |       |
 *     V       |      (uint32 LS 24b)      |       |
 *     -       +---------------------------+       |
 *     ^       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       s
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       t
 *     |       |\\\\\\ CLIENT MEMORY  \\\\\|       o
 *     |       |\\\\\\                \\\\\|       r
 *     |       |\\\\\\                \\\\\|       r
 *             |\\\\\\ CIRCULAR       \\\\\|       e
 *     s       |\\\\\\      BUFFER    \\\\\|       q
 *     i       |\\\\\\                \\\\\|
 *     z       |\\\\\\ INFRA DOES NOT \\\\\|
 *     e       |\\\\\\                \\\\\|
 *             |\\\\\\ INTERPRET THIS \\\\\|
 *             |\\\\\\                \\\\\|
 *     |       |\\\\\\     DATA       \\\\\|
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       |
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       |
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       |
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       |
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       |
 *     |       |\\\\\\\\\\\\\\\\\\\\\\\\\\\|       |
 *     V       +---------------------------+       |
 *             |       guard_pattern       |       |
 *             |           (uint32)        |       |
 *             +---------------------------+       V
 */
typedef struct el_ {
    /*
     * Pointers to list element structures of the previous and next entries.
     */
    struct el_* prev;
    struct el_* next;
    /*
     * Size of client data in this entry
     */
    uint32 sz;
    /*
     * Flags to indicate whether an entry has been visited by the consumer
     * thread. Guard Pattern to detect memory overrun issues.
     */
    uint32 flags_and_guard_pattern;
} el_t;

/*
 * Provides pointer to the client memory space in an entry
 */
#define PLD(le)       (char*)(le+1)
/*
 * Provides pointer to the list element structure of an entry, given the
 * pointer to the client memory
 */
#define PLD2LE(buf)   (((el_t*)buf)-1)

/*
 * number of uint32s to use just past the end of client memory space in one
 * entry. This is intended for detection of buffer overruns. See functions
 * cbuf_get_buffer_done and cbuf_do_consistency_checks in
 * cint_logger_circular_buffer.c
 */
#define GUARD_WORDS      1
/*
 * pointer to memory just past the end of one entry, given the pointer to its
 * list element structure
 */
#define FLOOR(le)        ((el_t*)(PLD(le) + (le)->sz + GUARD_WORDS*sizeof(uint32)))
/*
 * Storage space used by one entry in bytes, given the pointer to its list
 * element structure. This includes the size of list element, client memory
 * space, and GUARD_WORDS number of uint32s meant for catching buffer overruns.
 */
#define STORREQ(le)      ((le)->sz + sizeof(el_t)    + GUARD_WORDS*sizeof(uint32))
/*
 * Find the total memory required for an entry given the size of client memory
 * space requested by a client. This is the size of memory requested by the
 * client, plus size of a list element and GUARD_WORDS number of uint32s meant
 * for detecting buffer overruns.
 */
#define REQ2STORREQ(rsz) (rsz + sizeof(el_t)         + GUARD_WORDS*sizeof(uint32))
/*
 * Find the size of client memory space given the size used of an entry. This
 * is the size of the entry minus the size of a list element structure and
 * GUARD_WORDS number of uint32s
 */
#define STORREQ2REQ(rsz) (rsz - sizeof(el_t)         - GUARD_WORDS*sizeof(uint32))
/*
 * Flag for signalling whether an entry has been visited by the consumer
 * thread. This is for testing / reporting purposes only...this flag is not
 * used for synchronization, synchronization with the consumer thread is using
 * two semaphores 'producer' and 'consumer' in circular_buffer_t
 */
#define CONSUMED_MASK 0x80000000
/*
 * Two uint32 guard patterns, one for header one for footer of an entry
 * respectively
 */
#define GUARD_PATTERN_1 0x0BADC0DE
#define GUARD_PATTERN_2 0x600DC0DE

/*
 * Callback function type. This callback may be provided by the client. It is
 * called from the consumer thread with a client provided opaque value, the
 * circular buffer structure, pointer to the entry payload, i.e. the space
 * allocated for client data, and size of the client data.
 */
typedef void (*circular_buffer_consumer_cb_t)(void *opaque, circular_buffer_t *cbuf, void *buf, int bufsz);

/*
 * Function type for procedures copying client data from one buffer to a new
 * buffer. The client may copy the original buffer contents to new buffer, or
 * transform it in some way while copying. The total buffer space used in new
 * buffer may be less than or equal to the original buffer size, it must not
 * increase else buffer overrun will occur.
 *
 * This type of callback is used when cloning entries from a circular buffer.
 */
typedef void (*circular_buffer_entry_copy_cb_t)(void *new_buf, void *orig_buf, int bufsz);

/*
 * Type for holding case counts. See description below of the case_counts
 * member of cint_circular_buffer_t
 */
typedef unsigned long long case_counts_t[6][5];

/*
 * Handle for every circular buffer structure.
 *
 * The circlar buffer infrastructure maintains a doubly linked list of
 * "Entries" in a memory buffer of size "totalsz". As the buffer starts to fill
 * up with entries, it starts to free the oldest entries first and memory
 * associated with those entries is reused for newer entries.
 *
 * The infra provides option to never reclaim the first "nfreeze" entries. In
 * this case the circular buffer operates within memory remaining after those
 * entries. A client may set nfreeze to Zero.
 *
 * Following diagram depicts one possible state of the circular buffer,
 * with location of the key pointers and some sizes for correlating with the
 * structure definition below.
 *
 *
 *
 * cbuf.buffer - ^ ----- ^ ----> +-----------------------+
 *               |       |       |        Entry 1        |<-------+
 *               |       |       +-----------------------+        |
 *               |       |       |                       |<-------+
 *                       |       |        Entry 2        |
 *               n       |       |                       |<-------+
 *               f       |       +-----------------------+        |
 *               r       |       |        Entry 3        |<-------x
 *               e       |       +-----------------------+        |
 *               e       |       |        Entry 4        |<-------+
 *               z       |       +-----------------------+        |
 *               e       |       |           .           |
 *                       |       |           .           |
 *                       |       |           .           |
 *               |       |       |           .           |
 *               |       |       +-----------------------+        |
 *               |               |        Entry n        |<-------x
 * cbuf.base --- V ------------> +-----------------------+        |
 *                               |                       |<-------|-----+
 *                               |                       |        |     |
 *                       t       |                       |        |     |
 *                       o       |     Head + 3 Entry    |        |     |
 *                       t       |                       |        |     |
 *                       a       |                       |        |     |
 *                       l       |                       |<---+   |     |
 * cbuf.tail ----------- s ----> +-----------------------+    |   |     |
 *                       z       |                       |<---+   |     |
 *                               |      Tail Entry       |        |     |
 *                               |                       |----+   |     |
 *                               +-----------------------+    V   |     |
 *                       |       |///////////////////////|   +-+  |     |
 *                       |       |///////////////////////|   |X|  |     |
 *                       |       |//  Free Space the  ///|   +-+  |     |
 *                       |       |/ next entry can use //|        |     |
 *                       |       |///////////////////////|        |     |
 *                       |       |///////////////////////|        |     |
 * cbuf.head ----------- | ----> +-----------------------+        |     |
 *                       |       |      Head Entry       |<-------x     |
 *                       |       +-----------------------+        |     |
 *                       |       |     Head + 1 Entry    |<-------x     |
 * cbuf.cle  ----------- | ----> +-----------------------+        |     |
 * (consumer thread      |       |                       |<-------+     |
 *  is processing this   |       |     Head + 2 Entry    |<-------------+
 *  entry while new requests     |                       |
 *  result in entries after      +-----------------------+
 *  tail)                |       |///////////////////////|
 *                       |       |/ Unused becase this //|
 *                       |       |/ was not enough space |
 *                       |       |/ to hold the entry. //|
 *                       |       |/Entry created at base/|
 *                       |       |///////////////////////|
 * cbuf.end  ----------- V ----> +-----------------------+
 *
 *
 * cbuf is of type circular_buffer_t defined below.
 */
struct cint_circular_buffer_t {
    /*
     * Head an Tail list entries of the circular buffer / queue
     */
    el_t *head;
    el_t *tail;
    /*
     * List entry which will be processed by the consumer thread next. Once the
     * consumer thread has processed all entries this pointer becomes NULL
     */
    el_t *cle;

    /*
     * A boolean which is polled by the the consumer thread while processing
     * one list entry. The consumer thread is in a continuous loop waiting for
     * the producer semaphore. When the semaphore is received, the entry at cle
     * is processed. However, if this boolean is set the thread instead exists
     * the loop and terminates. This is used when destroying a circular buffer,
     * See the cbuf_circular_buffer_t_destroy
     */
    int done;

    /*
     * Name for the circular buffer, printed along with all logs generated by
     * this infrastructure.
     */
    const char *name;

    /*
     * Total size of memory in which to operate the circular buffer.
     */
    int totalsz;

    /*
     * Pointer to a contiguous block of memory of size totalsz. This is the
     * memory within which the circular buffer / circular queue is maintained.
     */
    void *buffer_start;
    /*
     * Just past the top of the circular buffer memory.
     */
    void *buffer_end;

    /*
     * User can elect to have the first nfreeze entries which are not reclaimed
     * for storing newer entries once the circular buffer wraps. This points
     * just past the nfreeze'th entry in the circular queue. After the initial
     * nfreeze entries are created the circular buffer / circular queue are
     * operated in the memory from 'base' to 'end'. When nfreeze is Zero,
     * 'base' is the same as 'buffer'
     */
    void *base;
    /*
     * Number of first N entries which are not reclaimed for storing newer
     * entries once the circular buffer wraps.
     */
    int nfreeze;
    /*
     * Number of buffer requests which were successfully served, i.e. enough
     * space was successfully allocated for them. This is used for determining
     * when nfreeze entries have been created, and also for reporting buffer
     * statistics.
     */
    unsigned long long nreqs;
    /*
     * Number of buffer requests which could not be served, i.e. enough memory
     * was not available to store the space requested by the client, plus
     * overheads - the list element header and guard patterns at the foot of
     * the client memory. This should happen only when the storage required is
     * more than total storage available.
     */
    unsigned long long nfailed_reqs;

    /*
     * Number of entries current present in the circular queue
     */
    int bufcnt;
    /*
     * Total memory used by the entries in the circular queue
     */
    int bufused;

    /*
     * The circular buffer allocation algorithm needs to handle multiple
     * combinations of the relative positions of base, head, tail and the space
     * requested by a client. Each of those "cases" are handled... here we have
     * a counter for each case, it is incremented each time the case is
     * encountered. Intended for testing / diagnostics and reporting purposes.
     * See cbuf_get_buffer_internal for all the cases.
     */
    case_counts_t case_counts;
    /*
     * This is a free-running count of number of times a buffer allocation
     * request was received. This should equal nreqs + nfailed_reqs. It is
     * being used for printing all logs associated with a request,  only every
     * so many requests. See print_every.
     */
    unsigned long long count;
    /*
     * For diagnostics purposes the infrastructure generates several messages.
     * However, this infrastructure needs to be fast so it can instead issue
     * all diagnostics only every so many requests. All diagnostic messages are
     * generated together essentially !(count % print_every).
     */
    int print_every;


    /*
     * Mutex for all threads requesting memory from the circular buffer
     */
    sal_mutex_t mutex;
    /*
     * Semaphores for coordination with the consumer thread. Every time a new
     * entry is created, producer semaphore is given. Consumer thread takes the
     * producer semaphore before processing each entry.
     *
     * Consumer semaphore is given every time the consumer finishes processing
     * an entry beyond the first nfreeze entries. This semaphore is taken
     * before freeing up entries in the circular buffer.
     *
     * This scheme ensures that all entries are processed or"consumed" before
     * they are reclaimed.
     */
    sal_sem_t producer;
    sal_sem_t consumer;
    /*
     * SAL thread ID of the consumer thread. One consumer thread is spawned for
     * every circular buffer. All entries created in the circular buffer are
     * processed by this thread.
     */
    sal_thread_t consumer_thread_id;

    /*
     * callback function a client may register. This callback is invoked for
     * every entry from the consumer thread. This helps to separate the
     * generation of logging entries, from say the conversion of logging
     * entries into text output.
     * See header for circular_buffer_consumer_cb_t.
     */
    circular_buffer_consumer_cb_t consumer_cb;
    /*
     * An opaque value which is passed as-is to the consumer_cb at each
     * invocation.
     */
    void *consumer_opaque;
};

/*
 * A convenience macro for calculating the offset of a pointer from
 * buffer_start, which is the total memory allocated for holding the circular
 * buffer / circular queue. Typically it should never receive NULL as input...
 * however if the provided pointer is NULL then -1 is returned, as it is easily
 * identifiable in debug logs. This macro is used only for generating debug
 * messages.
 */
#define OFS(p) ((!p) ? -1 : (int)((char*)p - (char*)cbuf->buffer_start))

/*
 * Lock and Unlock per circular buffer mutex
 */
#define CBUF_LOCK   sal_mutex_take(cbuf->mutex, sal_mutex_FOREVER)
#define CBUF_UNLOCK sal_mutex_give(cbuf->mutex)

typedef struct cint_circular_buffer_test_ {
    unsigned int prod;
    unsigned int cons;
    unsigned int mismatches;
} circular_buffer_test_t;


/*
 * Create a circular buffer. The control structure and the buffer itself are
 * both malloc'd from heap.
 */
extern circular_buffer_t* cbuf_circular_buffer_t_create(char *name, int size, int nfreeze,
                                                        int consumer_thread_stksz,
                                                        int consumer_thread_prio,
                                                        circular_buffer_consumer_cb_t cb,
                                                        void *consumer_opaque);
/*
 * Destroy a circular buffer. The buffer and control structure are both freed.
 */
extern void cbuf_circular_buffer_t_destroy(circular_buffer_t *cbuf);
/*
 * Do variety of consistency checks on the circular buffer. Extern because
 * consistency checks may be triggered from a shell command also. See
 * cint_logger_cmd.c
 */
extern void cbuf_do_consistency_checks(circular_buffer_t *cbuf);
/*
 * Request a buffer for storing "req" bytes, from the larger circular buffer.
 * This is performance critical call.  Caller indicates whether a full
 * consistency check on the circular buffer should also be triggered.
 * Consistency checks run over the whole circular buffer so may take time...
 * callers would typically have them run only once in N calls in the interest
 * of speed of execution.
 */
extern void* cbuf_get_buffer(circular_buffer_t *cbuf, int req, int do_checks);
/*
 * A buffer requested using cbuf_get_buffer once filled up by the application
 * must be acknowledged using cbuf_get_buffer_done. This indicates to the
 * circular buffer infrastructure that the buffer is ready and any consumers
 * may now read it.
 */
extern void cbuf_get_buffer_done(circular_buffer_t *cbuf, void * buf);
/*
 * Create a circular buffer with entries from another circular buffer. Most of
 * the arguments are same as ones required for circular_buffer_t_create
 */
extern circular_buffer_t* cbuf_splice_entries(circular_buffer_t *cbuf,
                                              int offset, int count,
                                              circular_buffer_entry_copy_cb_t copy_f,
                                              char *ocbuf_name,
                                              int consumer_thread_stksz,
                                              int consumer_thread_prio,
                                              circular_buffer_consumer_cb_t consumer_cb,
                                              void *consumer_opaque);
/*
 * Test function on circular buffers -- allocates random sized buffer sizes.
 * Code should neither crash, nor fail consistency checks. The number and
 * contents of Produced and Consumed entries must be equal. Invoked from a
 * shell command.
 */
extern cmd_result_t cbuf_circular_buffer_test(int totalsz, int nfreeze, int alloc,
                                              int iter, int pe, int consumer_thread_prio,
                                              int seed);

/* } */
#endif

/* } */
#endif
