#ifndef __CINT_LOGGER_REPLAY_H__
#define __CINT_LOGGER_REPLAY_H__
/* { */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#define CINT_LOGGER_MAX_NESTED_APIS 5

#define CINT_LOGGER_MAX_REPLAY_THREADS 512

/*
 * Structure used by the logger in Replay mode to hold information related to
 * one API call. The structure is allocated when the call1 logs for an API
 * call, as identified by its call_id, are found. The structure is freed once
 * the corresponding call2 logs are found.
 */
typedef struct cint_logger_replay_info_s {

    /*
     * Whether this replay structure is in use
     */
    int in_use;

    /*
     * The call_id
     */
    int call_id;

    /*
     * Relevant call_ctxt pointers
     */
    cint_logger_call_ctxt_t *call1, *call2;

    /*
     * buffer holding record of inputs
     */
    void *call1_buffer;

    /*
     * size of the call1 buffer
     */
    int call1_bufsz;

    /*
     * buffer holding record of outputs
     */
    void *call2_buffer;

    /*
     * size of the call2 buffer
     */
    int call2_bufsz;

    /*
     * buffers allocated by replay helper for inputs/outputs
     */
    void *alloc_bufs[CINT_CONFIG_MAX_FPARAMS+1];

} cint_logger_replay_info_t;

/*
 * Structure used by the logger in Replay mode to track API calls per original
 * thread as identified by the original caller_tid. It holds instances of
 * cint_logger_replay_info_t structures, each representing one API call in the
 * thread. Since each replay_info structure is freed once a call2 entry of the
 * corresponding API is found, the number of instances represents the number of
 * nested API calls possible in the BCM SDK. This is arbitrarily set to 5 for
 * now (see CINT_LOGGER_MAX_NESTED_APIS).
 */
typedef struct cint_logger_replay_thread_info_s {

    /*
     * Thread ID
     */
    sal_thread_t caller_tid;

    /*
     * Whether this thread structure is in use
     */
    int in_use;

    /*
     * next available index info replay_info array
     */
    int napis;

    /*
     * replay info for APIs, upto max nested APIs
     */
    cint_logger_replay_info_t replay_info[CINT_LOGGER_MAX_NESTED_APIS];

} cint_logger_replay_thread_info_t;

/*
 * Given the call_ctxt structure, find an existing (i.e. for call2) or new
 * replay_info structure.
 */
extern void cint_logger_call_ctxt_to_replay_info(cint_logger_call_ctxt_t *call_ctxt,
                                                 cint_logger_replay_info_t **p_replay_info);

/*
 * Called by the logger to indicate that one API replay is now complete and the
 * replay_info structure may be free'd.
 */
extern void cint_logger_call_ctxt_to_replay_info_done(cint_logger_call_ctxt_t *call_ctxt,
                                                      cint_logger_replay_info_t *replay_info);

extern int cint_logger_is_replay(cint_logger_thread_data_t *tdata,
                                 cint_logger_call_ctxt_t *call_ctxt,
                                 cint_logger_api_params_t *ctxt,
                                 cint_logger_call_ctxt_t **p_orig_call_ctxt,
                                 void *arg_ptrs_cint[CINT_CONFIG_MAX_FPARAMS+1],
                                 void *dummy_ptrs_cint[CINT_CONFIG_MAX_FPARAMS+1]);

/*
 * Names for call context variables....ordinarily
 */
#define REPLAY_CURR_CTXT "this"
/*
 * ...during replay of call1 logs
 */
#define REPLAY_LOG1_CTXT "log1"
/*
 * ...during replay of call2 logs
 */
#define REPLAY_LOG2_CTXT "log2"
/*
 * prefixes to variables names injected during Replay of respective call1 and
 * call2 logs
 */
#define REPLAY_LOG1_VPFX "log1_"
#define REPLAY_LOG2_VPFX "log2_"

/* } */
#endif

/* } */
#endif
