/*
 * $Id: cint_logger_replay.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_test.c
 * Purpose:     CINT logger Replay helper code
 *
 */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#include "cint_config.h"
#include "cint_interpreter.h"
#include "cint_porting.h"
#include "cint_variables.h"
#include "cint_error.h"
#include "cint_eval_asts.h"
#include "cint_internal.h"
#include "cint_datatypes.h"
#include "cint_debug.h"
#include "cint_parser.h"
#include "cint_logger.h"
#include "cint_logger_replay.h"

static cint_logger_replay_thread_info_t thread_info[CINT_LOGGER_MAX_REPLAY_THREADS];

/*
 * Routine to find thread_info corresponding to the thread ID indicated in the
 * call_ctxt structure. An already allocated slot, or a free slot from
 * thread_info array is returned.
 *
 * Parameters
 *   [in] call_ctxt
 *     Call Ctxt to map. Only thre caller_tid field is used as key
 *   [out] p_thread
 *     Pointer to an already allocated, or Free slot is returned here.
 */
static void
cint_logger_replay_call_ctxt_to_thread_info (cint_logger_call_ctxt_t *call_ctxt,
                                             cint_logger_replay_thread_info_t **p_thread)
{
    int ii, found;
    cint_logger_replay_thread_info_t *thread;

    if (!p_thread) {

        return;
    }

    *p_thread = NULL;

    found = 0;

    /*
     * Scan the entire array to look for a slot previously allocated to the
     * thread ID.
     */
    for (ii = 0, thread = &thread_info[ii];
         ii < CINT_LOGGER_MAX_REPLAY_THREADS;
         ii++, thread = &thread_info[ii]) {

        if ((thread->caller_tid == call_ctxt->caller_tid) && thread->in_use) {
            found = 1;
            break;
        }
    }

    if (found) {

        *p_thread = thread;
        return;
    }


    /*
     * A previously allocated slot was not found. Scan the entire array to look
     * for a free slot. If a free slot is not found, likely the original logs
     * have too many simultaneously active threads and the solution may be to
     * increase CINT_LOGGER_MAX_REPLAY_THREADS as needed.
     */
    for (ii = 0, thread = &thread_info[ii];
         ii < CINT_LOGGER_MAX_REPLAY_THREADS;
         ii++, thread = &thread_info[ii]) {

        if (!thread->in_use) {

            CINT_MEMSET(thread, 0, sizeof(*thread));
            thread->caller_tid = call_ctxt->caller_tid;
            thread->in_use = 1;
            found = 1;
            break;
        }
    }

    if (found) {

        *p_thread = thread;
    }
}

/*
 * Routine called by the the replay Mode logger to find a replay_info structure
 * for an API replayed. Since API inputs and outputs are in different logging
 * entries, we need a place to hold replay related information (specifically
 * parameter values) until the output entry is found and the API may be
 * replayed.
 *
 * Parameters
 *   [in] call_ctxt
 *     call_ctxt structure of the API being replayed. The caller thread ID and
 *     call ID are used as key for lookup.
 *   [out] p_replay_info
 *     A new or previously allocated replay entry is returned here.
 *
 * A two level database is created -- first a thread entry is found based on
 * the original caller thread ID; within the thread entry there is space for
 * CINT_LOGGER_MAX_NESTED_APIS replay_info structures which are allocated based
 * on the call ID. The allocation scheme ensures that a replay_info structure
 * and thread_info structure are marked not in_use , once there is no active
 * API executing on them.
 *
 */
void
cint_logger_call_ctxt_to_replay_info (cint_logger_call_ctxt_t *call_ctxt,
                                      cint_logger_replay_info_t **p_replay_info)
{
    cint_logger_replay_thread_info_t *thread;
    cint_logger_replay_info_t *replay_info;
    int ii, found;

    if (!p_replay_info) {

        return;
    }

    *p_replay_info = NULL;

    /*
     * find a previously allocated or free thread_info structure for the
     * original caller thread ID.
     */
    cint_logger_replay_call_ctxt_to_thread_info(call_ctxt, &thread);

    if (!thread) {

        return;
    }

    found = 0;

    /*
     * Find a previously allocated or free replay_info structure for the
     * original call ID, within the list of thread_info
     */
    for (ii = 0, replay_info = &thread->replay_info[ii];
         ii < CINT_LOGGER_MAX_NESTED_APIS;
         ii++, replay_info = &thread->replay_info[ii]) {

        if ((replay_info->call_id == call_ctxt->call_id) && replay_info->in_use) {

            found = 1;
            break;
        }
    }

    if (found) {

        *p_replay_info = replay_info;
        return;
    }

    /*
     * If a previously allocated slot is not found, find and return a free
     * slot. While doing so, account an additional API as "running" against the
     * thread.
     */
    for (ii = 0, replay_info = &thread->replay_info[ii];
         ii < CINT_LOGGER_MAX_NESTED_APIS;
         ii++, replay_info = &thread->replay_info[ii]) {

        if (!replay_info->in_use) {

            CINT_MEMSET(replay_info, 0, sizeof(*replay_info));
            replay_info->call_id = call_ctxt->call_id;
            replay_info->in_use = 1;
            thread->napis++;
            found = 1;
            break;
        }
    }

    if (found) {

        *p_replay_info = replay_info;
    }
}

/*
 * Once the logger is done with a replay_info entry, i.e. when the call2
 * logging entry has been succesfully processed, the replay_info may be freed
 * after freeing up any resources allocated by the logger.
 *
 * Parameters
 *   [in] call_ctxt
 *     original call_ctxt of the API being replayed
 *   [in] replay_info
 *     replay_info that was originally allocated for the call_ctxt
 *
 * This routine will find the thread_info corresponding to the call_ctxt caller
 * thread ID, and then validate that the replay_info came from the same
 * thread_info structure. This is needed for both consistency checking, and
 * also to free the thread_info if no more APIs are being tracked as "running"
 * on it.
 */
void
cint_logger_call_ctxt_to_replay_info_done (cint_logger_call_ctxt_t *call_ctxt,
                                           cint_logger_replay_info_t *replay_info)
{
    int ii;
    cint_logger_replay_thread_info_t *thread;
    void **alloc_bufs;

    if (!call_ctxt) {

        return;
    }

    /*
     * Find the thread_info slot
     */
    cint_logger_replay_call_ctxt_to_thread_info(call_ctxt, &thread);

    if (!thread) {

        return;
    }

    /*
     * Each thread_info structure has CINT_LOGGER_MAX_NESTED_APIS replay_info
     * slots. Verify that the one provided belongs in the set for the thread
     */
    ii = (int)(replay_info - thread->replay_info);

    if ((ii < 0) || (ii >= CINT_LOGGER_MAX_NESTED_APIS)) {

        return;
    }

    /*
     * Free the call1 buffer if it exists...
     */
    if (replay_info->call1_buffer) {

        CINT_FREE(replay_info->call1_buffer);
        replay_info->call1_buffer = NULL;
        replay_info->call1_bufsz = 0;
    }

    /*
     * Free the call2 buffer if it exists...
     */
    if (replay_info->call2_buffer) {

        CINT_FREE(replay_info->call2_buffer);
        replay_info->call2_buffer = NULL;
        replay_info->call2_bufsz = 0;
    }

    alloc_bufs = replay_info->alloc_bufs;

    /*
     * Free the buffers allocated for holding input and output parameters that
     * are pointers
     */
    for (ii = 0; ii < (CINT_CONFIG_MAX_FPARAMS+1); ii++) {

        if (alloc_bufs[ii]) {

            CINT_FREE(alloc_bufs[ii]);
            alloc_bufs[ii] = NULL;
        }
    }

    /*
     * All malloc'd child data structures are freed, zero the structure
     */
    CINT_MEMSET(replay_info, 0, sizeof(*replay_info));

    /*
     * Decrement the number of APIs active in the thread. If the number falls
     * to zero, the thread_info structure is no longer in use. Mark it free to
     * be reused for replay of another log.
     */
    thread->napis--;

    thread->in_use = (thread->napis > 0);

    if (!thread->in_use) {

        CINT_MEMSET(thread, 0, sizeof(*thread));
    }
}

/*
 * Helper routine to determine if a Replay might be in progress, and if so,
 * loads up the original call_ctxt and pointers to parameter values into
 * supplied pointers/array orig_call_ctxt and arg_ptrs_cint.
 *
 * Parameters
 *   [in] tdata
 *     Thread specific data for the calling thread.
 *   [in] call_ctxt
 *     context specific to the API call
 *   [in] ctxt
 *     The api_params structure
 *   [out] p_orig_call_ctxt
 *     Pointer to original call_ctxt structure is returned here
 *   [out] arg_ptrs_cint
 *     Array of pointers to the API arguments' values as found in the CINT
 *     interpreter
 *   [out] dummy_ptrs_cint
 *     For parameters that are array, arg_ptrs_cint[ii] cannot point to the
 *     data; instead it needs to point to a pointer which in turns points to
 *     the data. Placeholder for such pointer.
 *
 * Returns
 *   TRUE if all tests passed, i.e. Replay may be attempted
 *   FALSE otherwise
 *
 */
int
cint_logger_is_replay (cint_logger_thread_data_t *tdata,
                       cint_logger_call_ctxt_t *call_ctxt,
                       cint_logger_api_params_t *ctxt,
                       cint_logger_call_ctxt_t **p_orig_call_ctxt,
                       void *arg_ptrs_cint[CINT_CONFIG_MAX_FPARAMS+1],
                       void *dummy_ptrs_cint[CINT_CONFIG_MAX_FPARAMS+1])
{
    cint_logger_call_ctxt_t *orig_call_ctxt = NULL;
    cint_parameter_desc_t *params;
    cint_parameter_desc_t call_ctxt_pds = { "cint_logger_call_ctxt_t", NULL, 0, 0 };
    cint_variable_t *vr;
    cint_datatype_t dt;
    int ii, nargs;

    /*
     * Mode must be Replay
     */
    if (Cint_logger_cfg.mode != cintLoggerModeReplay) {

        return FALSE;
    }

    /*
     * A variable "this" of type cint_logger_call_ctxt_t must exist in the
     * immediate current scope of the CINT interpreter. If so, that is the
     * original call_ctxt, i.e. call_ctxt from the time of log generation.
     */
    CINT_MEMSET(&dt, 0, sizeof(dt));
    vr = cint_variable_find(REPLAY_CURR_CTXT, 1);
    if (!vr ||
        (cint_datatype_find(call_ctxt_pds.basetype, &dt) != CINT_E_NONE) ||
        (cint_type_check(&vr->dt, &dt) == 0)) {

        return FALSE;

    }

    orig_call_ctxt = vr->data;

    nargs = ctxt->nargs;
    params = ctxt->params;

    /*
     * If the original call_ctxt indicates call2 logs, a variable named "r"
     * must be found in the current scope of CINT interpreter.
     */
    CINT_MEMSET(&dt, 0, sizeof(dt));
    vr = cint_variable_find(params[0].name, 1);
    if (!vr ||
        (cint_datatype_find(ctxt->params[0].basetype, &dt) != CINT_E_NONE) ||
        (cint_type_check(&vr->dt, &dt) == 0)) {

        if (orig_call_ctxt && (orig_call_ctxt->flags & CINT_PARAM_OUT)) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("Failed to find variable \"%s\" of type \"%s\" in replay mode\n"), params[0].name, params[0].basetype));
            return FALSE;
        }

    } else {

        arg_ptrs_cint[0] = vr->data;
    }

    /*
     * Similarly, variables by the same name as the parameter names must be
     * found in the current scope. The variable data points to the value of
     * such parameters. However, if the basetype is an array (e.g. bcm_mac_t)
     * then we need an intermediate pointer to point to the data, because
     * elsewhere in the logger code a dereference is performed first to get to
     * the value of such parameters.
     */
    for (ii = 1; (ii < nargs) && params[ii].name; ii++) {

        vr = cint_variable_find(params[ii].name, 1);
        if (!vr) {

            return FALSE;
        }

        if (!params[ii].pcount && SHR_BITGET(ctxt->is_basetype_arr, ii)) {

            dummy_ptrs_cint[ii] = vr->data;
            arg_ptrs_cint[ii] = &dummy_ptrs_cint[ii];
        } else {

            arg_ptrs_cint[ii] = vr->data;
        }
    }

    *p_orig_call_ctxt = orig_call_ctxt;

    return TRUE;
}

/* } */
#else
/* { */
/* Make ISO compilers happy. */
typedef int cint_logger_replay_c_not_empty;
/* } */
#endif
