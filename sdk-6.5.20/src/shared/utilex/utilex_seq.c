/** \file utilex_seq.c
 *
 * Provide the a utlity that helps to run a sequence backward and forward.
 * Adittional tools provided:
 * * Sequence Logging
 * * Time tracking
 * * Time testing
 * * Tests for memory leaks
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*
 * Include files.
 * {
 */

#include <sal/core/thread.h>

#include <shared/bsl.h>
#include <shared/utilex/utilex_seq.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_time_analyzer.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/mem_measure_tool.h>

#include <soc/drv.h>
/*
 * }
 */

/*
 * Defines
 * {
 */
/*
 * Used in the utilex_seq_thread_per_step_dependency_map as max step count
 */
#define UTILEX_SEQ_MAX_NOF_STEP_IDS 512

#define UTILEX_SEQ_IS_MULTITHREAD_EN(seq, current_step)\
(seq->multithread_en || (seq->kbp_multithread_en && utilex_seq_is_kbp_step(current_step->step_name)))

#define UTILEX_SEQ_IS_MULTITHREAD_EN_JOIN(seq)\
(seq->multithread_en || seq->kbp_multithread_en)
/**
 * \brief Input for utilex_seq_run_step_list_forward_in_parallel
 */
typedef struct
{
    int unit;
    char *step_name;
    utilex_seq_t *seq;
    int depth;
    utilex_seq_step_t *step_list;
    sal_usecs_t *time_overall;
    uint32 *swstate_overall;
    uint32 *memory_overall;
    int *is_first_done;
    int *is_last_done;
} utilex_seq_parallel_run_input_t;

/**
 * \brief Input for utilex_seq_run_step_forward_in_parallel
 */
typedef struct
{
    int unit;
    char *step_name;
    utilex_seq_cb_f forward;
} utilex_seq_parallel_execute_input_t;

/*
 * }
 */

static shr_error_e utilex_seq_run_step_list_forward(
    int unit,
    utilex_seq_t * seq,
    int depth,
    utilex_seq_step_t * step_list,
    sal_usecs_t * time_overall,
    uint32 *swstate_overall,
    uint32 *memory_overall,
    int *is_first_done,
    int *is_last_done);

/*
 * Variable to store the mutex flag.
 */
uint8 utilex_seq_multithread_active[SOC_MAX_NUM_DEVICES] = { 0 };

/*
 * Parallel threads dependency map
 */
int utilex_seq_thread_per_step_dependency_map[SOC_MAX_NUM_DEVICES][UTILEX_SEQ_MAX_NOF_STEP_IDS] =
    { {UTILEX_SEQ_INVALID_THREAD_ID} };

/*
 * \brief - set the utilex_seq_multithread_active.
 * \param [in] unit - unit
 * \param [in] flag - value to be set in utilex_seq_multithread_active
 * \return
 *   None
 * \remark
 * \see
 *   * None
 */
static void
utilex_seq_multithread_active_set(
    int unit,
    uint8 flag)
{
    utilex_seq_multithread_active[unit] = flag;
}

/*
 * See .h file
 */
uint8
utilex_seq_multithread_active_get(
    int unit)
{
    return utilex_seq_multithread_active[unit];
}

/*
 * \brief - Return true if the step is kbp
 * \param [in] step_name - Step name
 * \return
 *   None
 * \remark
 * \see
 *   None
 */
static uint8
utilex_seq_is_kbp_step(
    char *step_name)
{
    if (sal_strstr(step_name, "KBP") != NULL)
    {
        return TRUE;
    }
    return FALSE;
}

/*
 * \brief - get the dependency marker for a step.
 * \param [in] unit - unit
 * \param [in] step_id - Step index
 * \return
 *   Thread id.
 * \remark
 * \see
 *   * None
 */
static int
utilex_seq_multithread_dependent_step_id_get(
    int unit,
    int step_id)
{
    if (step_id < UTILEX_SEQ_MAX_NOF_STEP_IDS)
    {
        return utilex_seq_thread_per_step_dependency_map[unit][step_id];
    }

    return UTILEX_SEQ_INVALID_THREAD_ID;
}

/*
 * \brief - set the dependency marker for a step.
 * \param [in] unit - unit
 * \param [in] thread_id - Relevant thread
 * \param [in] step_id - Step index
 * \return
 *   None
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_multithread_dependent_step_id_set(
    int unit,
    int thread_id,
    int step_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (step_id < UTILEX_SEQ_MAX_NOF_STEP_IDS)
    {
        utilex_seq_thread_per_step_dependency_map[unit][step_id] = thread_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Step_Id (%d) is bigger than UTILEX_SEQ_MAX_NOF_STEP_IDS", step_id);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
utilex_seq_t_init(
    int unit,
    utilex_seq_t * seq)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(seq, _SHR_E_INTERNAL, "seq");

    sal_memset(seq, 0, sizeof(utilex_seq_t));
    seq->first_step = UTILEX_SEQ_STEP_INVALID;
    seq->last_step = UTILEX_SEQ_STEP_INVALID;
    seq->control_prefix = NULL;
    seq->log_severity = bslSeverityNormal;
    sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "SEQ:");
    seq->last_passed_step = -1;
    seq->time_thresh_cb = NULL;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - update all substeps to be silent
 *          usage example: silent multithreaded steps
 * \param [in]  unit - unit #.
 * \param [in]  step_list - pointer to a list of steps.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_silent_all_sub_steps(
    int unit,
    utilex_seq_step_t * step_list)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    /** every list must have at least one member - the last one - with step_id == UTILEX_SEQ_STEP_LAST */
    for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
    {
        step_list[step_index].static_flags |= UTILEX_SEQ_STEP_F_SILENT;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_step_t_init(
    int unit,
    utilex_seq_step_t * step)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(step, _SHR_E_INTERNAL, "step");

    sal_memset(step, 0, sizeof(utilex_seq_step_t));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - function to decide if skipped steps should be printed
 *
 * \param [in] unit - unit #
 * \param [in] decider - callback for a decisder function
 *
 * \return
 *  0 - print skipped
 *  1 - don't print skipped
 * \see
 *   * None
 */
static int
utilex_seq_skip_print_decider(
    int unit,
    utilex_seq_skip_print_decide decider)
{
    if (decider)
    {
        return decider(unit);
    }
    else
        return 0;
}

/**
 * \brief - counts the length og step list
 * \param [in]  unit - unit #.
 * \param [in]  step_list - pointer to a list of steps.
 * \param [out] nof_steps - pointer to the required number of steps.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_nof_steps_get(
    int unit,
    utilex_seq_step_t * step_list,
    int *nof_steps)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    /** every list must have at least one member - the last one - with step_id == UTILEX_SEQ_STEP_LAST */
    for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
    {
        /** Do nothing */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify 'utilex_seq_run()' Input
 */
static shr_error_e
utilex_seq_run_verify(
    int unit,
    utilex_seq_t * seq,
    int forward)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(seq, _SHR_E_INTERNAL, "seq");
    SHR_NULL_CHECK(seq->log_prefix, _SHR_E_INTERNAL, "seq->log_prefix");
    SHR_NULL_CHECK(seq->step_list, _SHR_E_INTERNAL, "seq->step_list");

    /**
     * check that MEM_LEAK and TIME_STAMP flags are not used when
     * PARTIAL_INIT is used.
     */
    if ((seq->mem_test_en || seq->time_test_en) &&
        (seq->first_step != UTILEX_SEQ_STEP_INVALID || seq->last_step != UTILEX_SEQ_STEP_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Running time or memory test is not supported on partial sequence\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Managment function for running step list (forward
 *        direction).
 *          The resulted info will be added to total_flags
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in] step - ptr to relevant step
 * \param [in,out] total_flags - flags relevant to this step
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_managment_logic_forward(
    int unit,
    utilex_seq_t * seq,
    utilex_seq_step_t * step,
    uint32 *total_flags,
    int *is_first_done,
    int *is_last_done)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Am I First? */
    if (step->step_id == seq->first_step || seq->first_step == UTILEX_SEQ_STEP_INVALID)
    {
        *is_first_done = 1;
    }

    /** Is first NOT done and NOT a list */
    if (*is_first_done == 0 && step->step_sub_array == NULL)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Is last done? */
    if (*is_last_done == 1)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Am I Last? */
    if (step->step_id == seq->last_step)
    {
        *is_last_done = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to check weather a step is enabled/disabled
 *        according to SOC Property.
 *
 * \param [in] unit - unit #
 * \param [in] log_severity - logging severity for the shell
 *        output
 * \param [in] contol_prefix - prefix of the SOC property,
 *        common for the entire list
 * \param [in] current_step - pointer to the structure with the
 *        parameters of the step
 * \param [in] defl - default value for the SOC Property
 * \param [out] is_enabled - is the step enabled
 *
 * \return
 *  See shr_error_e
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_step_enabled(
    int unit,
    uint32 log_severity,
    char *contol_prefix,
    utilex_seq_step_t * current_step,
    int defl,
    int *is_enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_enabled = soc_property_suffix_num_get(unit, 0, contol_prefix, current_step->soc_prop_suffix, defl);

    SHR_FUNC_EXIT;
}

/**
 * \brief - This function is given to a seperate thread in order to invoke
 *        utilex_seq_run_step_list_forward.
 */
void
utilex_seq_run_step_list_forward_in_parallel(
    void *input)
{
    int rv;
    utilex_seq_parallel_run_input_t *info = (utilex_seq_parallel_run_input_t *) input;

    /** Run init list function */
    rv = utilex_seq_run_step_list_forward
        (info->unit, info->seq, info->depth, info->step_list, info->time_overall, info->swstate_overall,
         info->memory_overall, info->is_first_done, info->is_last_done);

    if (SHR_FAILURE(rv))
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(info->unit, "A parallel init step list (%s) failed on external thread, asserting."),
                   info->step_name));
        assert(0);
    }
    sal_free(input);
    return;
}

/**
 * \brief - This function is given to a seperate thread in order to invoke
 *        step's forward callback.
 */
void
utilex_seq_run_execute_forward_in_parallel(
    void *input)
{
    int rv;

    utilex_seq_parallel_execute_input_t *info = (utilex_seq_parallel_execute_input_t *) input;

    /** Run init list function */
    rv = info->forward(info->unit);

    if (SHR_FAILURE(rv))
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(info->unit, "A parallel init step (%s) failed on external thread, asserting."),
                   info->step_name));
        assert(0);
    }
    sal_free(input);
    return;
}

/**
 * \brief - function to run a list of steps (forward direction)
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in] depth - recursive list depth
 * \param [in] step_list - ptr to relevant step list
 * \param [out] time_overall - total time it took to complete
 *        the step list
 * \param [out] swstate_overall - total memory allocated for SW
 *        state for the step list
 * \param [out] memory_overall - Non-SW state Memory allocated,
 *        excluding for SW state, for the step list
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *  See shr_error_e
 * \remark
 *   the function is also responsible for making decisions regarding
 *   time stamping, memory leak detections and skipping steps according
 *   to seq and step_list.
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_run_step_list_forward(
    int unit,
    utilex_seq_t * seq,
    int depth,
    utilex_seq_step_t * step_list,
    sal_usecs_t * time_overall,
    uint32 *swstate_overall,
    uint32 *memory_overall,
    int *is_first_done,
    int *is_last_done)
{
    int step_index;
    int dynamic_flags = 0;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step;
    int is_enabled = 1;
    int defl;
    int depth_index;
    uint32 log_severity;
    uint32 swstate_allocated;
    unsigned long alloc_start;
    unsigned long free_start;
    unsigned long alloc_end;
    unsigned long free_end;
    unsigned long alloc_bytes_count_offset;
    unsigned long free_bytes_count_offset;
    char measurment_handle[30];
    char seq_prefix[20];
    sal_usecs_t time_elapsed = 0;
    sal_usecs_t time_stamp_start = 0;
    sal_usecs_t time_stamp_end = 0;
    sal_usecs_t time_thresh = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        uint32 total_flags = 0; /* total flags for this step */
        int thread_id = 0;
        int thread_count = 0;

        current_step = &(step_list[step_index]);

        /** Run Flag function if exists */
        total_flags |= current_step->static_flags;
        if (current_step->dyn_flags != NULL)
        {
            dynamic_flags = 0;
            SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            total_flags |= dynamic_flags;
        }

        /** Logging severity */
        log_severity = BSL_SEVERITY_ENC(seq->log_severity);
        if (total_flags & UTILEX_SEQ_STEP_F_VERBOSE)
        {
            log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
        }
        if (total_flags & UTILEX_SEQ_STEP_F_SILENT)
        {
            log_severity = BSL_SEVERITY_ENC(bslSeverityDebug);
        }

        /** Join all active threads if requested by current step*/
        if (UTILEX_SEQ_IS_MULTITHREAD_EN_JOIN(seq) && (total_flags & UTILEX_SEQ_STEP_F_JOIN_ALL_THREADS))
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, (" -> Waiting for all active init steps threads to finish.\n"));

            /*
             * Get the relevant thread for the dependency step.
             */
            thread_id = utilex_seq_multithread_dependent_step_id_get(unit, current_step->depends_on_step_id);
            if (thread_id == UTILEX_SEQ_INVALID_THREAD_ID)
            {
                /*
                 * In case of non existing step_id in the map - join all threads
                 */
                thread_id = 0;
                thread_count = seq->active_threads_nof;
            }
            else
            {
                thread_count = thread_id + 1;
            }

            for (; thread_id < thread_count; thread_id++)
            {
                if (seq->active_threads[thread_id])
                {
                    sal_thread_join(seq->active_threads[thread_id], NULL);
                    seq->active_threads[thread_id] = 0;
                    seq->active_threads_nof--;
                }
            }

            /*
             * this mean that the multithread init is not active
             */
            if (seq->active_threads_nof == 0)
            {
                utilex_seq_multithread_active_set(unit, FALSE);
            }
        }

        /** init internal_info */
        sal_memset(&(current_step->internal_info), 0, sizeof(utilex_seq_step_internal_t));

        if (seq->time_log_en || seq->time_test_en)
        {
            /** Save time stamp and at beginning of step */
            time_stamp_start = sal_time_usecs();
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the beginning of step */
            sal_get_alloc_counters_main_thr(&(alloc_start), &(free_start));
        }

        if (seq->swstate_log_en)
        {
            /** Start the memory allocation measurement for SW state in the beginning of step */
            utilex_str_replace_whitespace(seq_prefix, seq->log_prefix);
            sal_sprintf(measurment_handle, "Utilex%s%d", seq_prefix, current_step->step_id);
            SHR_IF_ERR_EXIT(memory_consumption_start_measurement_dnx(measurment_handle));
        }

        /** Decisions made by the management logic are stronger then ones made by the flag cb function  */
        SHR_IF_ERR_EXIT(utilex_seq_managment_logic_forward
                        (unit, seq, current_step, &total_flags, is_first_done, is_last_done));

        if ((total_flags & UTILEX_SEQ_STEP_F_SKIP) != 0)
        { /** Skip */
            if (utilex_seq_skip_print_decider(unit, seq->skip_print_decider))
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped)\n", current_step->step_name));
            }
        }
        /** Skip in warm boot unless specified not to skip in warm boot */
        else if (seq->warmboot && (total_flags & UTILEX_SEQ_STEP_F_WB_SKIP) != 0)
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped in warm reboot)\n", current_step->step_name));
        }
        /** Skip in access only mode unless specified not to skip in the mode */
        else if (seq->access_only &&
                 ((total_flags &
                   (UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY | UTILEX_SEQ_STEP_F_REQUIRED_FOR_PORT_ACCESS_ONLY)) ==
                  0))
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity,
                    ("%s    # (skipped in access only boot)\n", current_step->step_name));
        }
        /** Skip in heat-up mode unless specified not to skip in the mode */
        else if (seq->heat_up && ((total_flags & UTILEX_SEQ_STEP_F_REQUIRED_FOR_HEAT_UP) == 0))
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s    # (skipped in heat-up boot)\n", current_step->step_name));
        }
        else
        {
            /** If step is disabled with SOC property, skip step */
            if ((total_flags & (UTILEX_SEQ_STEP_F_SOC_PROP_DIS | UTILEX_SEQ_STEP_F_SOC_PROP_EN)) != 0)
            {
                defl = ((total_flags & UTILEX_SEQ_STEP_F_SOC_PROP_EN) ? 1 : 0);
                SHR_IF_ERR_EXIT(utilex_seq_step_enabled
                                (unit, log_severity, seq->control_prefix, current_step, defl, &is_enabled));
            }

            if (is_enabled == 1)
            {

                /** Allow writing to registers and changing SW state during WB*/
                if (seq->warmboot && (total_flags & UTILEX_SEQ_STEP_F_WB_DISABLE_CHECKS) != 0)
                {
                    if (seq->tmp_allow_access_enable != NULL)
                    {
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_DBAL));
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
                    }
                }

                /** Verfiy that we don't run both forward and step_sub_array on a separate thread */
                if (UTILEX_SEQ_IS_MULTITHREAD_EN(seq, current_step)
                    && total_flags & UTILEX_SEQ_STEP_F_RUN_ON_THREAD && current_step->forward != NULL
                    && current_step->step_sub_array != NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Can't run both forward and step_sub_array on a separate thread");
                }

                /** Run Step INIT function if exists  */
                if (current_step->forward != NULL)
                {
                    /** Log info  */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s%s\n", current_step->step_name,
                                                            (UTILEX_SEQ_IS_MULTITHREAD_EN(seq, current_step)
                                                             && (total_flags & UTILEX_SEQ_STEP_F_RUN_ON_THREAD)) ?
                                                            ": (Running in parallel on separate thread)" : ""));

                    /** In case time analyzer operation mode is per step - clear time analyzer DB before running the step*/
                    if (seq->time_analyzer_mode == UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP)
                    {
                        SHR_IF_ERR_EXIT(utilex_time_analyzer_clear(unit));
                    }

                    if (UTILEX_SEQ_IS_MULTITHREAD_EN(seq, current_step)
                        && total_flags & UTILEX_SEQ_STEP_F_RUN_ON_THREAD)
                    {
                        utilex_seq_parallel_execute_input_t *thread_info_p;

                        thread_info_p =
                            (utilex_seq_parallel_execute_input_t *)
                            sal_alloc(sizeof(utilex_seq_parallel_execute_input_t), "seq_par_thread_info");
                        thread_info_p->unit = unit;
                        thread_info_p->step_name = current_step->step_name;
                        thread_info_p->forward = current_step->forward;

                        /*
                         * this mean that the multithread init is active
                         */
                        utilex_seq_multithread_active_set(unit, TRUE);
                        /** Run init function on separate thread */
                        seq->active_threads[seq->active_threads_nof] =
                            sal_thread_create("utilex step", SAL_THREAD_STKSZ, 1,
                                              utilex_seq_run_execute_forward_in_parallel, thread_info_p);

                        if (seq->active_threads[seq->active_threads_nof] == SAL_THREAD_ERROR)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "could not open thread for parallel init step");
                        }

                        SHR_IF_ERR_EXIT(utilex_seq_multithread_dependent_step_id_set(unit, seq->active_threads_nof,
                                                                                     current_step->step_id));
                        seq->active_threads_nof++;
                    }
                    else
                    {
                        /** Run init function */
                        SHR_IF_ERR_EXIT(current_step->forward(unit));

                    }

                    /** Store the last passed-id - used for error recovery */
                    seq->last_passed_step = current_step->step_id;
                }

                /** If list - init list steps  */
                if (current_step->step_sub_array != NULL)
                {
                    /** Log info */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("    - "));
                    }

                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s%s\n", current_step->step_name,
                                                            (UTILEX_SEQ_IS_MULTITHREAD_EN(seq, current_step)
                                                             && (total_flags & UTILEX_SEQ_STEP_F_RUN_ON_THREAD)) ?
                                                            ": (Running on separate thread - sub steps won't be showed)"
                                                            : ""));

                    /** Run init list function */
                    if (UTILEX_SEQ_IS_MULTITHREAD_EN(seq, current_step)
                        && total_flags & UTILEX_SEQ_STEP_F_RUN_ON_THREAD)
                    {
                        utilex_seq_parallel_run_input_t *thread_info_p;

                        thread_info_p =
                            (utilex_seq_parallel_run_input_t *) sal_alloc(sizeof(utilex_seq_parallel_run_input_t),
                                                                          "seq_par_thread_info");
                        thread_info_p->unit = unit;
                        thread_info_p->step_name = current_step->step_name;
                        thread_info_p->seq = seq;
                        thread_info_p->depth = depth + 1;
                        thread_info_p->step_list = current_step->step_sub_array;
                        thread_info_p->time_overall = time_overall;
                        thread_info_p->swstate_overall = swstate_overall;
                        thread_info_p->memory_overall = memory_overall;
                        thread_info_p->is_first_done = is_first_done;
                        thread_info_p->is_last_done = is_last_done;

                        /*
                         * this mean that the multithread init is active
                         */
                        utilex_seq_multithread_active_set(unit, TRUE);
                        /*
                         * all sub steps on parallel thread should be silent
                         */
                        utilex_seq_silent_all_sub_steps(unit, thread_info_p->step_list);

                        seq->active_threads[seq->active_threads_nof] =
                            sal_thread_create("utilex step", SAL_THREAD_STKSZ, 1,
                                              utilex_seq_run_step_list_forward_in_parallel, thread_info_p);

                        if (seq->active_threads[seq->active_threads_nof] == SAL_THREAD_ERROR)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "could not open thread for parallel init step");
                        }
                        SHR_IF_ERR_EXIT(utilex_seq_multithread_dependent_step_id_set(unit, seq->active_threads_nof,
                                                                                     current_step->step_id));
                        seq->active_threads_nof++;

                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(utilex_seq_run_step_list_forward
                                        (unit, seq, depth + 1, current_step->step_sub_array, time_overall,
                                         swstate_overall, memory_overall, is_first_done, is_last_done));
                    }

                }

                /** Return to warmboot normal mode */
                if (seq->warmboot && (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
                    && (total_flags & UTILEX_SEQ_STEP_F_WB_DISABLE_CHECKS) != 0)
                {
                    if (seq->tmp_allow_access_disable != NULL)
                    {
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_DBAL));
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));
                    }
                }

                current_step->disabled_by_soc_prop = FALSE;
            }
            else
            {
                if (utilex_seq_skip_print_decider(unit, seq->skip_print_decider))
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity,
                            ("%s     # (skipped due to SOC property)\n", current_step->step_name));
                }

                current_step->disabled_by_soc_prop = TRUE;
            }
        }

        if (seq->time_log_en || seq->swstate_log_en || seq->mem_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s Done\n", current_step->step_name));

            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("     "));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("Statistics:\n"));
        }

        if (seq->time_log_en || seq->time_test_en)
        {
            /** Get time stamp at the end of the step */
            time_stamp_end = sal_time_usecs();

            /** Calculate how much time the step took */
            time_elapsed = (time_stamp_end - time_stamp_start);

            if (seq->time_log_en)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                if (time_elapsed > 2000)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("Time: %u MILLIseconds\n", (time_elapsed / 1000)));
                }
                else
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("Time: %u microseconds\n", time_elapsed));
                }
            }

            if (current_step->step_sub_array == NULL)
            {
                /** Add the time of each step to the overall time for the sequence */
                *time_overall += time_elapsed;
            }

            if (seq->time_test_en)
            {
                /** Get the time threshold for the current step */
                SHR_IF_ERR_EXIT(seq->time_thresh_cb(unit, current_step->step_id, total_flags, &time_thresh));

                /** Check if the time for the current step is exceeding the user defined threshold */
                if (time_elapsed > time_thresh)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "Overall time to run the step exceeds the permitted threshold!\nTime elapsed: %u microseconds\nThreshold: %u microseconds\n",
                                 time_elapsed, time_thresh);
                }
            }
        }

        if (seq->swstate_log_en)
        {
            /** Stop the memory allocation measurement for SW state at the end of step */
            SHR_IF_ERR_EXIT(memory_measurement_dnx_sw_state_get(measurment_handle, &swstate_allocated, TRUE));
            SHR_IF_ERR_EXIT(memory_consumption_clear_measurement_dnx(measurment_handle));

            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("SW state:  %d bytes allocated\n", swstate_allocated));

            if (current_step->step_sub_array == NULL)
            {
                /** Add the memory allocated for SW state of each step to the overall for the sequence */
                *swstate_overall += swstate_allocated;
            }
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the end of step */
            sal_get_alloc_counters_main_thr(&(alloc_end), &(free_end));

            /** Calculate the total step memory allocation excluding resources allocated for SW state */
            current_step->internal_info.non_swstate_memory_allocation =
                (alloc_end - alloc_start) - (free_end - free_start);

            if (current_step->step_sub_array == NULL)
            {
                /** Add the memory allocated of each step to the overall for the sequence */
                *memory_overall += current_step->internal_info.non_swstate_memory_allocation;
            }

            if (seq->mem_log_en)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("Memory:  %d bytes allocated\n", current_step->internal_info.non_swstate_memory_allocation));
            }

            /** For some steps the memory test is expected to fail, so they are marked with SKIP flag.
              * The memory allocated and freed in these memories is removed from the counters, so that it will not influence
              * the calculations of the following steps and the total allocated and freed memory of the sequence.
              * A debug message is given to the user indicating that the step is skipped and will not be
              * taken into account in any calculations */
            if ((total_flags & UTILEX_SEQ_STEP_F_MEM_TEST_SKIP) != 0)
            {
                alloc_bytes_count_offset = (alloc_end - alloc_start);
                free_bytes_count_offset = (free_end - free_start);
                /** Offset the memory counters, so that this step will not be taken into account */
                sal_set_alloc_counters_offset_main_thr(alloc_bytes_count_offset, free_bytes_count_offset);

                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("     - Step %s is skipped in memory testing and will not be taken into account\n",
                         current_step->step_name));
            }
        }

        if (current_step->forward != NULL && seq->time_analyzer_mode == UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP)
        {
            int nof_modules;
            utilex_time_analyzer_diag_info_t diag_info[UTILEX_TIME_ANALYZER_MAX_NOF_MODULES];
            int module_index;
            char *time_units;
            uint32 time;

            SHR_IF_ERR_EXIT(utilex_time_analyzer_diag_info_get
                            (unit, UTILEX_TIME_ANALYZER_MAX_NOF_MODULES, diag_info, &nof_modules));

            for (module_index = 0; module_index < nof_modules; module_index++)
            {
                if (diag_info[module_index].occurences == 0)
                {
                    continue;
                }

                if (diag_info[module_index].time > /* 2 seconds */ 2 * 1000 * 1000)
                {
                    time = diag_info[module_index].time / (1000 * 1000);
                    time_units = "[sec] ";
                }
                else
                {
                    time = diag_info[module_index].time / 1000;
                    time_units = "[msec]";
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("%-15s: Time %-4u %s | Occ %-6d | Avg Time %-6u [usec]\n", diag_info[module_index].name, time,
                         time_units, diag_info[module_index].occurences,
                         diag_info[module_index].time / diag_info[module_index].occurences));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Managment function for running step list (backward
 *        direction).
 *          The resulted info will be added to total_flags
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in] step - ptr to relevant step
 * \param [in,out] total_flags - flags relevant to this step
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_managment_logic_backward(
    int unit,
    utilex_seq_t * seq,
    utilex_seq_step_t * step,
    uint32 *total_flags,
    int *is_first_done,
    int *is_last_done)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Am I First? - this is the backwards direction - the input last step is actually the first step to be deinit */
    if (step->step_id == seq->last_step || seq->last_step == UTILEX_SEQ_STEP_INVALID)
    {
        *is_first_done = 1;
    }

    /** Is first NOT done and NOT a list */
    if (*is_first_done == 0 && step->step_sub_array == NULL)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Is last done? */
    if (*is_last_done == 1)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Am I Last? - this is the backwards direction - the input first step is actually the last step to be deinit */
    if (step->step_id == seq->first_step)
    {
        *is_last_done = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - function to run a list of steps (backward direction)
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in,out] depth - recursive list depth
 * \param [in,out] step_list - ptr to relevant step list
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *  See shr_error_e
 * \remark
 *   the funtion is also responsible for making decisions regarding
 *   memory leak detections according to seq and step_list.
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_run_step_list_backward(
    int unit,
    utilex_seq_t * seq,
    int depth,
    utilex_seq_step_t * step_list,
    int *is_first_done,
    int *is_last_done)
{
    int step_index;
    int nof_steps_in_list = 0;
    unsigned long total_memory_alocation_end;
    unsigned long total_memory_alocation_start;
    unsigned long total_memory_free_start;
    unsigned long total_memory_free_end;
    unsigned long alloc_bytes_count_offset;
    unsigned long free_bytes_count_offset;
    int memory_freed_during_deinit;
    int memory_alocation_during_init;
    utilex_seq_step_t *current_step;
    int depth_index;
    uint32 log_severity;
    int dynamic_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    /** Iterate over steps */
    for (step_index = nof_steps_in_list - 1; step_index >= 0; --step_index)
    {
        uint32 total_flags = 0;
        current_step = &(step_list[step_index]);

        /** Init step vars */
        total_memory_alocation_end = 0;
        total_memory_alocation_start = 0;
        total_memory_free_end = 0;
        total_memory_free_start = 0;

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the start of deinit */
            sal_get_alloc_counters_main_thr(&(total_memory_alocation_end), &(total_memory_free_end));
        }

        /** At deinit, the only reason to run the management logic is in order to switch the step_list_ptr to the sub-list */
        SHR_IF_ERR_EXIT(utilex_seq_managment_logic_backward
                        (unit, seq, current_step, &total_flags, is_first_done, is_last_done));

        /** Run Flag function if exists (unless step was marked as skipped) */
        total_flags |= current_step->static_flags;
        if ((current_step->dyn_flags != NULL) && !(total_flags & UTILEX_SEQ_STEP_F_SKIP))
        {
            dynamic_flags = 0;
            SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            total_flags |= dynamic_flags;
        }

        /** Logging severity  */
        log_severity = BSL_SEVERITY_ENC(seq->log_severity);
        if (total_flags & UTILEX_SEQ_STEP_F_VERBOSE)
        {
            log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
        }

        if ((total_flags & UTILEX_SEQ_STEP_F_SKIP) != 0)
        {
            if (utilex_seq_skip_print_decider(unit, seq->skip_print_decider))
            {
                /** Skip */
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("    - "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped)\n", current_step->step_name));
            }

        }
        else if (SOC_WARM_BOOT(unit) && (total_flags & UTILEX_SEQ_STEP_F_WB_SKIP) != 0)
        {
            /** Skip due to WB */
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped in warm reboot)\n", current_step->step_name));
        }
        else
        {
            if (current_step->disabled_by_soc_prop == FALSE)
            {
                /** If list - deinit list steps */
                if (current_step->step_sub_array != NULL)
                {
                    /** Log info */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("    - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s: \n", current_step->step_name));

                    /** Run deinit list */
                    SHR_IF_ERR_CONT(utilex_seq_run_step_list_backward
                                    (unit, seq, depth + 1, current_step->step_sub_array, is_first_done, is_last_done));
                }

                /** Run Step DEINIT function if exists */
                if (current_step->backward != NULL)
                {
                    /** Log info */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("    - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s\n", current_step->step_name));

                    /** Run deinit function */
                    SHR_IF_ERR_CONT(current_step->backward(unit));
                    /** Store the last passed-id - used for error recovery */
                    seq->last_passed_step = current_step->step_id;
                }
            }
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the end of deinit */
            sal_get_alloc_counters_main_thr(&(total_memory_alocation_start), &(total_memory_free_start));

            memory_alocation_during_init = current_step->internal_info.non_swstate_memory_allocation;
            memory_freed_during_deinit =
                (total_memory_free_start - total_memory_free_end) - (total_memory_alocation_start -
                                                                     total_memory_alocation_end);

            if (seq->mem_log_en)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("     - Memory:  %d bytes freed\n", memory_freed_during_deinit));
                if (memory_freed_during_deinit != memory_alocation_during_init)
                {
                        /** incase that allocated != freed, print both for convenience  */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity,
                            ("     - Delta was found between init and deinit in step %s\n", current_step->step_name));

                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - Memory allocated at init: %d bytes\n",
                                                            memory_alocation_during_init));

                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - Memory freed at deinit:   %d bytes\n",
                                                            memory_freed_during_deinit));
                }

            }

            /** For some steps the memory test is expected to fail, so they are marked with SKIP flag.
              * Running test for all steps not marked for SKIP */
            if ((seq->mem_test_en) && ((total_flags & UTILEX_SEQ_STEP_F_MEM_TEST_SKIP) == 0))
            {
                if (memory_freed_during_deinit != memory_alocation_during_init)
                {
                    SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory leak detected in step %s, terminating driver\n",
                                 current_step->step_name);
                }
            }

            /** For some steps marked with SKIP flag:
              * The memory allocated and freed in these memories is removed from the counters, so that it will not influence
              * the calculations of the following steps and the total allocated and freed memory of the sequence.
              * A debug message is given to the user indicating that the step is skipped and will not be
              * taken into account in any calculations */
            if ((total_flags & UTILEX_SEQ_STEP_F_MEM_TEST_SKIP) != 0)
            {
                alloc_bytes_count_offset = (total_memory_alocation_start - total_memory_alocation_end);
                free_bytes_count_offset = (total_memory_free_start - total_memory_free_end);
                /** Offset the memory counters, so that this step will not be taken into account */
                sal_set_alloc_counters_offset_main_thr(alloc_bytes_count_offset, free_bytes_count_offset);

                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("     - Step %s is skipped in memory testing and will not be taken into account\n",
                         current_step->step_name));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Run forward or backward function of a given step
 */
shr_error_e
utilex_seq_run_step(
    int unit,
    utilex_seq_step_t * current_step,
    int forward)
{
    uint32 log_severity;
    utilex_seq_step_t *sub_step;
    int step_index, nof_steps_in_list;
    SHR_FUNC_INIT_VARS(unit);

    /** Logging severity  */
    log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
    if (current_step->static_flags & UTILEX_SEQ_STEP_F_VERBOSE)
    {
        log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
    }

    if (forward == 1)
    {
        if (current_step->forward != NULL)
        {
            /** Log info */
            BSL_LOG(BSL_LOG_MODULE | log_severity,
                    ("Running forward function for step: %s\n", current_step->step_name));

            /** Run forward function */
            SHR_IF_ERR_EXIT(current_step->forward(unit));
        }

        if (current_step->step_sub_array != NULL)
        {

            /** Find NOF steps in list */
            SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, current_step->step_sub_array, &nof_steps_in_list));

            /** Run sub steps in forward direction */
            for (step_index = 0; step_index < nof_steps_in_list; step_index++)
            {
                sub_step = &(current_step->step_sub_array[step_index]);
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, sub_step, forward));
            }
        }

    }
    else
    {
        if (current_step->step_sub_array != NULL)
        {

            /** Find NOF steps in list */
            SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, current_step->step_sub_array, &nof_steps_in_list));

            /** Run sub steps in backward direction */
            for (step_index = (nof_steps_in_list - 1); step_index >= 0; step_index--)
            {
                sub_step = &(current_step->step_sub_array[step_index]);
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, sub_step, forward));
            }
        }

        if (current_step->backward != NULL)
        {
            /** Log info */
            BSL_LOG(BSL_LOG_MODULE | log_severity,
                    ("Running backward function for step: %s\n", current_step->step_name));

            /** Run backward function */
            SHR_IF_ERR_EXIT(current_step->backward(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_run_step_by_name(
    int unit,
    utilex_seq_step_t * step_list,
    char *step_name,
    int forward,
    int *step_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *step_found = 0;
    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_run_step_by_name
                            (unit, current_step->step_sub_array, step_name, forward, step_found));

            if ((*step_found) == 1)
            {
                /** Step is found in sub list, no need to continue */
                break;
            }
        }

        if (!sal_strncmp(step_name, current_step->step_name, strlen(step_name)))
        {
            /** Step is found */
            /** Not all steps can be ran alone due to dependancies
            *  between the steps */
            if (current_step->static_flags & UTILEX_SEQ_STEP_F_STANDALONE_EN)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, current_step, forward));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Step %s can not be ran as standalone!\n", current_step->step_name);
            }
            *step_found = 1;
            break;
        }

        if (step_index == (nof_steps_in_list - 1))
        {
            /** Step is not found */
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_run_step_by_id(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    int forward,
    int *step_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *step_found = 0;
    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id
                            (unit, current_step->step_sub_array, step_id, forward, step_found));

            if ((*step_found) == 1)
            {
                /** Step is found in sub list, no need to continue */
                break;
            }
        }

        if (step_id == current_step->step_id)
        {
            /** Step is found */
            /** Not all steps can be ran alone due to dependencies
            *  between the steps */
            if (current_step->static_flags & UTILEX_SEQ_STEP_F_STANDALONE_EN)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, current_step, forward));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Step %d can not be ran as standalone!\n", step_id);
            }
            *step_found = 1;
            break;
        }

        if (step_index == (nof_steps_in_list - 1))
        {
            /** Step is not found */
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
utilex_seq_find_step_by_id(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** step,
    int *is_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = 0;
    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_find_step_by_id(unit, current_step->step_sub_array, step_id, step, is_found));

            if ((*is_found) == 1)
            {
                /** Step is found in sub list, no need to continue */
                break;
            }
        }

        if (step_id == current_step->step_id)
        {
            *step = current_step;
            *is_found = 1;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_run(
    int unit,
    utilex_seq_t * seq,
    int forward)
{
    int is_first_done = 0;
    int is_last_done = 0;
    sal_usecs_t time_overall = 0;
    sal_usecs_t time_thresh = 0;
    uint32 swstate_overall = 0;
    uint32 memory_overall = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_seq_run_verify(unit, seq, forward));

    if (forward)
    {
        /** set warmboot indication */
        seq->warmboot = SOC_WARM_BOOT(unit);
        SHR_IF_ERR_EXIT(utilex_seq_run_step_list_forward
                        (unit, seq, 0, seq->step_list, &time_overall, &swstate_overall, &memory_overall,
                         &is_first_done, &is_last_done));
        if (seq->time_test_en || seq->time_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Total Time: %u MILLIseconds)\n", seq->log_prefix, (time_overall / 1000)));

            if (seq->time_test_en)
            {
                /** Get the time threshold for full sequence */
                SHR_IF_ERR_EXIT(seq->time_thresh_cb(unit, UTILEX_SEQ_STEP_INVALID, 0, &time_thresh));

                if (time_overall > time_thresh)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "Overal time to run the sequence exceeds the permitted threshold!\nOveral time elapsed: %u microseconds\nThreshold: %u microseconds\n",
                                 time_overall, time_thresh);
                }
            }
        }

        if (seq->swstate_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Total SW state: %u bytes allocated)\n", seq->log_prefix, swstate_overall));
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Non-SW state Memory: %u bytes allocated)\n", seq->log_prefix, memory_overall));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(utilex_seq_run_step_list_backward
                                 (unit, seq, 0, seq->step_list, &is_first_done, &is_last_done),
                                 "An Error has occurred in one of the steps above, please check log to understand from which step it originated from\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_next_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** next_step,
    int *is_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = UTILEX_SEQ_STEP_NOT_FOUND;

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_next_step_get(unit, current_step->step_sub_array, step_id, next_step, is_found));

            /** Step is found no need to continue */
            if (*is_found == UTILEX_SEQ_NEXT_STEP_FOUND)
            {
                break;
            }

            /** step found in sub list as the last step */
            if (*is_found == UTILEX_SEQ_STEP_FOUND)
            {
                if (step_index != (nof_steps_in_list - 1))
                {
                    *next_step = &(step_list[step_index + 1]);
                    *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
                }
                break;
            }
        }

        /** step found */
        if (step_id == current_step->step_id)
        {
            /** next step is the first element in the sublist */
            if (current_step->step_sub_array != NULL)
            {
                *next_step = &(current_step->step_sub_array[0]);
                *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
            }
            /** next step is first element in the next sublist */
            else if (step_index == (nof_steps_in_list - 1))
            {
                *is_found = UTILEX_SEQ_STEP_FOUND;
            }
            else
            {
                *next_step = &(step_list[step_index + 1]);
                *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return last active step id given a step list.
 * \param [in] unit - unit #.
 * \param [in] step_list - List of steps
 * \param [out] active_step - pointer to last active step
 * \return
 *   See shr_error_e
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_last_active_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    utilex_seq_step_t ** active_step)
{
    int nof_steps_in_list = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    while (step_list[nof_steps_in_list - 1].step_sub_array != NULL)
    {
        step_list = step_list[nof_steps_in_list - 1].step_sub_array;
        SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));
    }

    *active_step = &step_list[nof_steps_in_list - 1];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return previous step id given current step id.
 * \param [in] unit - unit #.
 * \param [in] step_list - List of steps
 * \param [in] step_id - current step id
 * \param [out] previous_step - pointer to previous step
 * \param [out] is_found - indicates whether step is found
 * \return
 *   See shr_error_e
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_previous_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** previous_step,
    int *is_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = UTILEX_SEQ_STEP_NOT_FOUND;

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any */
            SHR_IF_ERR_EXIT(utilex_seq_previous_step_get
                            (unit, current_step->step_sub_array, step_id, previous_step, is_found));

            /** Step is found no need to continue */
            if (*is_found == UTILEX_SEQ_NEXT_STEP_FOUND)
            {
                break;
            }

            /** step found in sub list as the last step */
            if (*is_found == UTILEX_SEQ_STEP_FOUND)
            {
                if (step_index > 0)
                {
                    if (step_list[step_index - 1].step_sub_array != NULL)
                    {
                        SHR_IF_ERR_EXIT(utilex_seq_last_active_step_get(unit,
                                                                        (step_list[step_index - 1]).step_sub_array,
                                                                        previous_step));
                    }
                    else
                    {
                        *previous_step = &(step_list[step_index - 1]);
                    }

                    *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
                }
                break;
            }
        }

        /** step found */
        if (step_id == current_step->step_id)
        {
            if (step_index == 0)
            {
                *is_found = UTILEX_SEQ_STEP_FOUND;
            }
            else
            {
                if (step_list[step_index - 1].step_sub_array != NULL)
                {
                    SHR_IF_ERR_EXIT(utilex_seq_last_active_step_get(unit, (step_list[step_index - 1]).step_sub_array,
                                                                    previous_step));
                }
                else
                {
                    *previous_step = &(step_list[step_index - 1]);
                }

                *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return indication whether the step or one of its father lists is marked as skipped
 * \param [in] unit - unit #.
 * \param [in] step_list - List of steps
 * \param [in] step_id - current step id
 * \param [out] is_found -indicates whether step was found
 * \param [out] is_skip -indicates whether step should be skipped
 * \return
 *   See shr_error_e
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_step_is_skipped_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    int *is_found,
    int *is_skip)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;
    int dynamic_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = 0;
    *is_skip = 0;

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_step_is_skipped_get
                            (unit, current_step->step_sub_array, step_id, is_found, is_skip));

            /** Step is found no need to continue */
            if ((*is_found) == 1)
            {
                dynamic_flags = 0;
                if (current_step->dyn_flags != NULL)
                {
                    SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
                }
                /** mark as skipped */
                if (_SHR_IS_FLAG_SET(current_step->static_flags, UTILEX_SEQ_STEP_F_SKIP) ||
                    _SHR_IS_FLAG_SET(dynamic_flags, UTILEX_SEQ_STEP_F_SKIP))
                {
                    *is_skip = 1;
                }

                break;
            }
        }

        /** step found */
        if (step_id == current_step->step_id)
        {
            dynamic_flags = 0;
            if (current_step->dyn_flags != NULL)
            {
                SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            }
            /** mark as skipped */
            if (_SHR_IS_FLAG_SET(current_step->static_flags, UTILEX_SEQ_STEP_F_SKIP) ||
                _SHR_IS_FLAG_SET(dynamic_flags, UTILEX_SEQ_STEP_F_SKIP))
            {
                *is_skip = 1;
            }

            *is_found = 1;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_next_active_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** next_step,
    int *is_found)
{
    SHR_FUNC_INIT_VARS(unit);

    /** get next step */
    SHR_IF_ERR_EXIT(utilex_seq_next_step_get(unit, step_list, step_id, next_step, is_found));
    if (*is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
    {
        /** end of list */
        SHR_EXIT();
    }

    while (((*next_step)->forward == NULL) && ((*next_step)->backward == NULL))
    {
        /** get next step */
        SHR_IF_ERR_EXIT(utilex_seq_next_step_get(unit, step_list, (*next_step)->step_id, next_step, is_found));

        if (*is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
        {
            /** end of list */
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_error_recovery_test_run(
    int unit,
    utilex_seq_t * seq,
    int first_step_id,
    const int *steps_to_skip,
    int nof_steps_to_skip,
    utilex_seq_err_recovery_test_t * test_info)
{
    int original_last_step_id, previous_step_id, current_step_id;
    unsigned long alloc_start, free_start, alloc_end, free_end;
    utilex_seq_step_t *current_step, *first_step;
    int is_found, is_skip, ii;
    int rv;
    int first_run = 1; /** required in order to run first step in the list if it was provided explicitly */

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(test_info, 0, sizeof(utilex_seq_err_recovery_test_t));

    /** get original last step id in order to know when to finish the test */
    if (seq->last_step == UTILEX_SEQ_STEP_INVALID)
    {
        original_last_step_id = -1;
    }
    else
    {
        original_last_step_id = seq->last_step;
    }

    /**
     * Find first step, first step can be either first step in list (by default) or provided explicitly
     */
    if (first_step_id == UTILEX_SEQ_STEP_INVALID)
    {
        first_step = &(seq->step_list[0]);
    }
    else                                                                                                                                                                                                                                                                                                                                                 /** test a given step */
    {
        /** lookup first step by its id */
        SHR_IF_ERR_EXIT(utilex_seq_find_step_by_id(unit, seq->step_list, first_step_id, &first_step, &is_found));
        if (is_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "First Step id %d wasn't found in the step list\n", first_step_id);
        }
    }

    /** if first step is not active, find next step which is active and set it as first step */
    if (first_step->forward == NULL && first_step->backward == NULL)
    {
        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq->step_list, first_step_id, &first_step, &is_found));
    }

    /** by default, previous step is the same as first */
    if ((first_step_id == UTILEX_SEQ_STEP_INVALID) || (first_step_id == (seq->step_list[0].step_id)))
    {
        previous_step_id = first_step->step_id;
        current_step_id = previous_step_id;
    }
    else
    {
        current_step_id = first_step->step_id;

        /** find previous step */
        SHR_IF_ERR_EXIT(utilex_seq_previous_step_get(unit, seq->step_list, first_step_id, &current_step, &is_found));
        if (is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "First Step id %d wasn't found in the step list\n", first_step_id);
        }
        previous_step_id = current_step->step_id;
    }

    /** set current step as first */
    current_step = first_step;

    /** iterate until last step id */
    while ((previous_step_id != original_last_step_id) || first_run)
    {
        first_run = 0;

        BSL_LOG(BSL_LOG_MODULE, ("\n"));
        BSL_LOG(BSL_LOG_MODULE, ("\n"));
        BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
        BSL_LOG(BSL_LOG_MODULE, ("Start test for Step %s\n", (current_step->step_name)));
        BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));

        /** run sequence until previous step (in case this is not the first step) */
        if (previous_step_id != current_step->step_id)
        {
            seq->last_step = previous_step_id;
            sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "Test Init:");
            SHR_IF_ERR_EXIT(utilex_seq_run(unit, seq, TRUE));
        }

        /** get memory allocation before running current step */
        sal_get_alloc_counters_main_thr(&(alloc_start), &(free_start));

        /** get indication whether current step should be skipped */
        SHR_IF_ERR_EXIT(utilex_seq_step_is_skipped_get(unit, seq->step_list, current_step_id, &is_found, &is_skip));

        /** check whether the step should be skipped due to test requirements */
        if (is_skip == FALSE)
        {
            for (ii = 0; ii < nof_steps_to_skip; ii++)
            {
                if (steps_to_skip[ii] == current_step_id)
                {
                    is_skip = TRUE;
                    break;
                }
            }
        }

        /** run last step if step should not be skipped */
        BSL_LOG(BSL_LOG_MODULE, ("--------------------------------------------------\n"));
        if (is_skip == FALSE)
        {
            /**
             * Verify error recovery - run backward step function without running the forward function.
             * Errors are allowed, crashes not!
             */
            if (current_step->backward != NULL)
            {
                BSL_LOG(BSL_LOG_MODULE, ("Verify error-recovery for step %s - running backward function before "
                                         "forward\n", current_step->step_name));
                rv = current_step->backward(unit);
                if (rv != _SHR_E_NONE)
                {
                    BSL_LOG(BSL_LOG_MODULE, ("Backward run for step %s returned error %d, continue testing \n",
                                             current_step->step_name, rv));
                }
            }

            /** run forward step function */
            if (current_step->forward != NULL)
            {
                BSL_LOG(BSL_LOG_MODULE, ("Running forward function for step: %s\n", current_step->step_name));
                SHR_IF_ERR_EXIT(current_step->forward(unit));
            }

            /** run backward step function */
            if (current_step->backward != NULL)
            {
                BSL_LOG(BSL_LOG_MODULE, ("Running backward function for step: %s\n", current_step->step_name));
                SHR_IF_ERR_EXIT(current_step->backward(unit));
            }
        }
        else
        {
            BSL_LOG(BSL_LOG_MODULE, ("############ Step %s is not tested since it's skipped ###########\n",
                                     (current_step->step_name)));
        }
        BSL_LOG(BSL_LOG_MODULE, ("--------------------------------------------------\n"));

        /** get memory allocation after deinitializating current step */
        sal_get_alloc_counters_main_thr(&(alloc_end), &(free_end));

        /** run deinit sequence (in case this is not the first step) */
        if (previous_step_id != current_step->step_id)
        {
            sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "Test Deinit:");
            SHR_IF_ERR_EXIT(utilex_seq_run(unit, seq, FALSE));
        }

        /** verify whether deinit freed all allocated memory */
        if ((alloc_start - free_start) != (alloc_end - free_end))
        {
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
            BSL_LOG(BSL_LOG_MODULE, ("Error!!! Step %s Failed!\n", (current_step->step_name)));
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));

            if (test_info->fail_count < UTILEX_SEQ_NOF_FAILED_STEPS)
            {
                test_info->fail_step_name[test_info->fail_count] = current_step->step_name;
                test_info->fail_mem_size[test_info->fail_count] = (alloc_end - free_end) - (alloc_start - free_start);
                test_info->fail_step_id[test_info->fail_count] = current_step->step_id;
                (test_info->fail_count)++;
            }
        }
        else
        {
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
            BSL_LOG(BSL_LOG_MODULE, ("Step %s PASSED!\n", (current_step->step_name)));
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
        }

        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq->step_list, current_step_id, &current_step,
                                                        &is_found));

        /** only current step found, break since we reached end of list */
        if (is_found == UTILEX_SEQ_STEP_FOUND)
        {
            /** end of list */
            break;
        }

        if (is_found == UTILEX_SEQ_STEP_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Step %s wasn't found in the step list\n", current_step->step_name);
        }

        /** Proceed to next step */
        previous_step_id = current_step_id;
        current_step_id = current_step->step_id;
    }

exit:
    SHR_FUNC_EXIT;
}

static void
utilex_seq_update_test_info(
    utilex_seq_time_test_t * test_info,
    int debug_flag,
    utilex_seq_step_t * current_step,
    sal_usecs_t step_time,
    sal_usecs_t time_thresh,
    int coefficient)
{

    test_info->step_name[test_info->count] = current_step->step_name;
    test_info->time[test_info->count] = step_time;
    test_info->step_id[test_info->count] = current_step->step_id;
    test_info->step_thresh[test_info->count] = time_thresh;
    if (debug_flag)
    {
        test_info->computed_thresh[test_info->count] = (coefficient * step_time) / 100;
    }
    (test_info->count)++;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_time_test_run(
    int unit,
    utilex_seq_t * seq,
    int first_step_id,
    int step_nof_iterations,
    utilex_seq_time_test_t * test_info_fail,
    utilex_seq_time_test_t * test_info_success,
    int debug_flag,
    int coefficient)
{
    int original_last_step_id, previous_step_id, current_step_id;
    sal_usecs_t time_stamp_start;
    sal_usecs_t time_stamp_end;
    sal_usecs_t time_thresh;
    sal_usecs_t total_step_time;
    sal_usecs_t step_time;
    utilex_seq_step_t *current_step, *first_step, *previous_step;
    utilex_seq_step_t *sub_step;
    uint32 total_flags = 0;
    int step_index, nof_steps_in_list;
    int is_found = 0, is_skip = 0, dynamic_flags = 0;
    int first_run = 1; /** required in order to run first step in the list if it was provided explicitly */

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(test_info_fail, 0, sizeof(utilex_seq_time_test_t));
    sal_memset(test_info_success, 0, sizeof(utilex_seq_time_test_t));

    /** get original last step id in order to know when to finish the test */
    if (seq->last_step == UTILEX_SEQ_STEP_INVALID)
    {
        original_last_step_id = -1;
    }
    else
    {
        original_last_step_id = seq->last_step;
    }

    /**
     * Find first step, first step can be either first step in list (by default) or provided explicitly
     */
    if (first_step_id == UTILEX_SEQ_STEP_INVALID)
    {
        first_step = &(seq->step_list[0]);
    }
    else
    {
        /** lookup first step by its id */
        SHR_IF_ERR_EXIT(utilex_seq_find_step_by_id(unit, seq->step_list, first_step_id, &first_step, &is_found));
        if (is_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "First Step id %d wasn't found in the step list\n", first_step_id);
        }
    }

    /** by default, previous step is the same as first */
    if ((first_step_id == UTILEX_SEQ_STEP_INVALID) || (first_step_id == (seq->step_list[0].step_id)))
    {
        previous_step_id = first_step->step_id;
        current_step_id = previous_step_id;
    }
    else
    {
        current_step_id = first_step->step_id;

        /** find previous step */
        SHR_IF_ERR_EXIT(utilex_seq_previous_step_get(unit, seq->step_list, first_step_id, &current_step, &is_found));
        if (is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "First Step id %d wasn't found in the step list\n", first_step_id);
        }
        previous_step_id = current_step->step_id;
    }

    /** set current step as first */
    current_step = first_step;

    /** iterate until last step id */
    while ((previous_step_id != original_last_step_id) || first_run)
    {
        total_step_time = 0;
        first_run = 0;

        BSL_LOG(BSL_LOG_MODULE, ("\n"));
        BSL_LOG(BSL_LOG_MODULE, ("\n"));
        BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
        BSL_LOG(BSL_LOG_MODULE, ("Start test for Step %s\n", (current_step->step_name)));
        BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));

        for (int i = 0; i < step_nof_iterations; i++)
        {

            /** run sequence until previous step (in case this is not the first step) */
            if (previous_step_id != current_step->step_id)
            {
                seq->last_step = previous_step_id;
                sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "Test Init:");
                SHR_IF_ERR_EXIT(utilex_seq_run(unit, seq, TRUE));
            }

            /** get indication whether current step should be skipped */
            SHR_IF_ERR_EXIT(utilex_seq_step_is_skipped_get(unit, seq->step_list, current_step_id, &is_found, &is_skip));

            /** run last step if step should not be skipped */
            BSL_LOG(BSL_LOG_MODULE, ("--------------------------------------------------\n"));
            if (is_skip == FALSE)
            {
                /** Save time stamp at beginning of step */
                time_stamp_start = sal_time_usecs();

                BSL_LOG(BSL_LOG_MODULE, ("Running init for step: %s\n", current_step->step_name));
                if ((current_step->step_sub_array != NULL) && (current_step->forward == NULL))
                {
                    /** Find NOF steps in list */
                    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, current_step->step_sub_array, &nof_steps_in_list));

                    /** Run sub steps in forward direction */
                    for (step_index = 0; step_index < nof_steps_in_list; step_index++)
                    {
                        sub_step = &(current_step->step_sub_array[step_index]);
                        /** get indication whether sub step should be skipped */
                        SHR_IF_ERR_EXIT(utilex_seq_step_is_skipped_get
                                        (unit, seq->step_list, sub_step->step_id, &is_found, &is_skip));
                        if (is_skip == FALSE)
                        {
                            SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, sub_step, TRUE));
                        }
                        else
                        {
                            BSL_LOG(BSL_LOG_MODULE, ("#### Sub Step %s is skipped ####\n", (sub_step->step_name)));
                        }
                    }
                }
                else
                {
                    /** Run forward step function */
                    SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, current_step, TRUE));
                }

                /** Save time stamp at the end of the step */
                time_stamp_end = sal_time_usecs();

                total_step_time += (time_stamp_end - time_stamp_start);

                /** Run Flag function if exists */
                total_flags = current_step->static_flags;
                if (current_step->dyn_flags != NULL)
                {
                    dynamic_flags = 0;
                    SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
                    total_flags |= dynamic_flags;
                }
                /** Get the time threshold for the current step */
                SHR_IF_ERR_EXIT(seq->time_thresh_cb(unit, current_step->step_id, total_flags, &time_thresh));

                BSL_LOG(BSL_LOG_MODULE, ("Running deinit for step: %s\n", current_step->step_name));
                if ((current_step->step_sub_array != NULL) && (current_step->backward == NULL))
                {
                    /** Find NOF steps in list */
                    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, current_step->step_sub_array, &nof_steps_in_list));

                    /** Run sub steps in backward direction */
                    for (step_index = (nof_steps_in_list - 1); step_index >= 0; step_index--)
                    {
                        sub_step = &(current_step->step_sub_array[step_index]);
                        /** get indication whether sub step should be skipped */
                        SHR_IF_ERR_EXIT(utilex_seq_step_is_skipped_get
                                        (unit, seq->step_list, sub_step->step_id, &is_found, &is_skip));
                        if (is_skip == FALSE)
                        {
                            SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, sub_step, FALSE));
                        }
                        else
                        {
                            BSL_LOG(BSL_LOG_MODULE, ("#### Sub Step %s is skipped ####\n", (sub_step->step_name)));
                        }
                    }
                }
                else
                {
                    /** Run backward step function */
                    SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, current_step, FALSE));
                }
            }
            else
            {
                BSL_LOG(BSL_LOG_MODULE, ("############ Step %s is not tested since it's skipped ###########\n",
                                         (current_step->step_name)));
            }
            BSL_LOG(BSL_LOG_MODULE, ("--------------------------------------------------\n"));

            /** Run deinit sequence (in case this is not the first step) */
            if (previous_step_id != current_step->step_id)
            {
                /** lookup previous step by its id */
                SHR_IF_ERR_EXIT(utilex_seq_find_step_by_id
                                (unit, seq->step_list, previous_step_id, &previous_step, &is_found));
                if (is_found == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Previous Step id %d wasn't found in the step list\n", first_step_id);
                }

                if ((previous_step->step_sub_array != NULL) && (previous_step->backward == NULL))
                {
                    /** The previous step is not active, the de-init should start from the one before that */
                    /** Find step before the previous (inactive) one */
                    SHR_IF_ERR_EXIT(utilex_seq_previous_step_get
                                    (unit, seq->step_list, previous_step_id, &previous_step, &is_found));
                    if (is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Step id %d wasn't found in the step list\n", first_step_id);
                    }

                    /** Set the last step of the sequence to the ID of the last active step */
                    seq->last_step = previous_step->step_id;
                }
                sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "Test Deinit:");
                SHR_IF_ERR_EXIT(utilex_seq_run(unit, seq, FALSE));
            }
        }

        if (is_skip == FALSE)
        {
            step_time = total_step_time / step_nof_iterations;

            /** Check if the time for the current step is exceeding the user defined threshold */
            if (step_time > time_thresh)
            {
                BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
                BSL_LOG(BSL_LOG_MODULE, ("Error!!! Step %s Failed!\n", (current_step->step_name)));
                BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));

                if (test_info_fail->count < UTILEX_SEQ_NOF_FAILED_STEPS)
                {
                    utilex_seq_update_test_info(test_info_fail, debug_flag, current_step, step_time, time_thresh,
                                                coefficient);
                }
            }
            else
            {
                if (debug_flag && test_info_success->count < UTILEX_SEQ_NOF_FAILED_STEPS)
                {
                    utilex_seq_update_test_info(test_info_success, debug_flag, current_step, step_time, time_thresh,
                                                coefficient);
                }

                BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
                BSL_LOG(BSL_LOG_MODULE, ("Step %s PASSED!\n", (current_step->step_name)));
                BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
            }
        }

        /** Find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_step_get(unit, seq->step_list, current_step_id, &current_step, &is_found));

        /** only current step found, break since we reached end of list */
        if (is_found == UTILEX_SEQ_STEP_FOUND)
        {
            /** end of list */
            break;
        }

        if (is_found == UTILEX_SEQ_STEP_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Step %s wasn't found in the step list\n", current_step->step_name);
        }

        /** Proceed to next step */
        previous_step_id = current_step_id;
        current_step_id = current_step->step_id;
    }

exit:
    SHR_FUNC_EXIT;
}
