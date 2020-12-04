/*! \file bcmbd_fwm_proc.c
 *
 * Functions of processor driver of Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <shr/shr_util.h>
#include <bcmdrd_config.h>
#include <bcmbd/bcmbd_fwm.h>
#include "./bcmbd_fwm_internal.h"

/* Log source for this component. */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_FWM

static bcmbd_fwm_processors_t *bcmbd_fwm_proc_db[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Internal functions
 */
/* Get the processor drivers. */
int
bcmbd_fwm_proc_drv_get(int unit, int proc_idx, bcmbd_fwm_proc_t **proc_drv)
{
    bcmbd_fwm_processors_t *processors = NULL;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    processors = bcmbd_fwm_proc_db[unit];
    SHR_NULL_CHECK(processors, SHR_E_INIT);

    SHR_NULL_CHECK(processors->proc_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(processors->proc_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    if ((proc_idx < 0) || (proc_idx >= processors->proc_num)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (proc_drv) {
        *proc_drv = &processors->proc_drv[proc_idx];
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked && processors->proc_mutex) {
        sal_mutex_give(processors->proc_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Get index of processor database and processor instance ID. */
int
bcmbd_fwm_proc_index_get(int unit, const char *processor,
                         int *proc_idx, int *inst_idx)
{
    int i;
    char *proc_str = NULL;
    char processor_name_copy[BCMBD_FWM_PROC_INST_NAME_MAX];
    char *ret_str;
    char *save_str = NULL;
    int p_idx = -1;
    int i_idx = -1;
    bcmbd_fwm_processors_t *processors = NULL;
    bcmbd_fwm_proc_t *proc_drv;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(processor, SHR_E_PARAM);
    SHR_NULL_CHECK(proc_idx, SHR_E_PARAM);
    SHR_NULL_CHECK(inst_idx, SHR_E_PARAM);

    processors = bcmbd_fwm_proc_db[unit];
    SHR_NULL_CHECK(processors, SHR_E_INIT);

    sal_snprintf(processor_name_copy, BCMBD_FWM_PROC_INST_NAME_MAX,
                 "%s", processor);

    ret_str = sal_strchr(processor_name_copy, '.');
    if (ret_str == NULL) {
        /* No instance specified, assigned instance 0. */
        proc_str = processor_name_copy;
        i_idx = 0;
    } else {
        /* Processor.Instance */
        proc_str = sal_strtok_r((char *)processor_name_copy, ".", &save_str);
        i_idx = sal_ctoi(sal_strtok_r(NULL, ".", &save_str), NULL);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Get proc index of processor %s.%d\n"),
                            proc_str, i_idx));

    SHR_NULL_CHECK(processors->proc_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(processors->proc_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    /* Match the processor */
    for (i = 0; i < processors->proc_num; i++) {
        proc_drv = &processors->proc_drv[i];
        if (!sal_strcasecmp(proc_str, proc_drv->name)) {
            p_idx = i;
            break;
        }
    }

    /* Return if no processor matched. */
    if (p_idx < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Processor %s is not registered.\n"), processor));
        SHR_IF_ERR_EXIT(SHR_E_EMPTY);
    }

    /* Return if instance not matched. */
    if ((i_idx < 0) || (i_idx >= (int)processors->proc_inst_num[p_idx])) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    *proc_idx = p_idx;
    *inst_idx = i_idx;

exit:
    if (locked && processors->proc_mutex) {
        sal_mutex_give(processors->proc_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Initialize processor database. */
int
bcmbd_fwm_proc_init(int unit)
{
    int i;
    bcmbd_fwm_processors_t *processors = NULL;

    SHR_FUNC_ENTER(unit);

    processors = sal_alloc(sizeof(bcmbd_fwm_processors_t), "bcmbdFwmProc");
    SHR_NULL_CHECK(processors, SHR_E_MEMORY);

    processors->proc_mutex = sal_mutex_create("bcmbdFwmProc");
    SHR_NULL_CHECK(processors->proc_mutex, SHR_E_MEMORY);

    processors->proc_num = 0;
    for (i = 0; i < BCMBD_FWM_PROC_NUM_MAX; i++) {
        processors->proc_inst_num[i] = 0;
    }

    /* Update to core database. */
    bcmbd_fwm_proc_db[unit] = processors;

exit:
    if (SHR_FUNC_ERR() && processors) {
        if (processors->proc_mutex) {
            sal_mutex_destroy(processors->proc_mutex);
        }
        sal_free(processors);
    }

    SHR_FUNC_EXIT();
}

/* Cleanup processor database. */
int
bcmbd_fwm_proc_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmbd_fwm_proc_db[unit]) {
        if (bcmbd_fwm_proc_db[unit]->proc_mutex) {
            sal_mutex_destroy(bcmbd_fwm_proc_db[unit]->proc_mutex);
        }
        sal_free(bcmbd_fwm_proc_db[unit]);
        bcmbd_fwm_proc_db[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

/* Callback register function for processor driver. */
int
bcmbd_fwm_proc_register(int unit, const bcmbd_fwm_proc_t *proc, int instances)
{
    bcmbd_fwm_processors_t *processors = NULL;
    int proc_num;

    SHR_FUNC_ENTER(unit);

    processors = bcmbd_fwm_proc_db[unit];
    SHR_NULL_CHECK(processors, SHR_E_INIT);

    SHR_NULL_CHECK(proc, SHR_E_PARAM);
    if (instances <= 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "No processor instance number assigned. "
                             "Set default 1 instance.\n")));
        instances = 1;
    }

    SHR_NULL_CHECK(processors->proc_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(processors->proc_mutex, SAL_MUTEX_FOREVER));

    proc_num = processors->proc_num;

    sal_memcpy(&processors->proc_drv[proc_num], proc, sizeof(bcmbd_fwm_proc_t));

    processors->proc_inst_num[proc_num] = instances;

    processors->proc_num++;

    sal_mutex_give(processors->proc_mutex);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/* Perform processor operations. */
int
bcmbd_fwm_proc_control(int unit, const char *processor,
                       bcmbd_fwm_proc_control_t ctrl)
{
    bcmbd_fwm_proc_t *proc_drv = NULL;
    char proc_name[BCMBD_FWM_PROC_INST_NAME_MAX];
    int proc_idx = -1;
    int inst_idx = -1;

    SHR_FUNC_ENTER(unit);

    sal_snprintf(proc_name, BCMBD_FWM_PROC_INST_NAME_MAX, "%s", processor);
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_index_get(unit, proc_name, &proc_idx, &inst_idx));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_drv_get(unit, proc_idx, &proc_drv));

    if (!proc_drv->control) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Processor driver not registered.\n")));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    switch (ctrl) {
        case BCMBD_FWM_PROC_CONTROL_RESET:
        case BCMBD_FWM_PROC_CONTROL_START:
        case BCMBD_FWM_PROC_CONTROL_STOP:
            SHR_IF_ERR_EXIT
                (proc_drv->control(unit, inst_idx, ctrl));
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Get processor state. */
int
bcmbd_fwm_proc_state_get(int unit, const char *processor,
                         bcmbd_fwm_proc_state_t *state)
{
    bcmbd_fwm_proc_t *proc_drv = NULL;
    int proc_idx = -1;
    int inst_idx = -1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_index_get(unit, processor, &proc_idx, &inst_idx));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_drv_get(unit, proc_idx, &proc_drv));

    if (!proc_drv->state) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Processor driver not registered.\n")));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (proc_drv->state(unit, inst_idx, state));

exit:
    SHR_FUNC_EXIT();
}

/* Get the number of processor types. */
int
bcmbd_fwm_proc_count_get(int unit, int *count)
{
    bcmbd_fwm_processors_t *processors = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    processors = bcmbd_fwm_proc_db[unit];
    SHR_NULL_CHECK(processors, SHR_E_INIT);

    SHR_NULL_CHECK(processors->proc_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(processors->proc_mutex, SAL_MUTEX_FOREVER));

    *count = processors->proc_num;

    sal_mutex_give(processors->proc_mutex);

exit:
    SHR_FUNC_EXIT();
}

/* Get the processor information. */
int
bcmbd_fwm_proc_get(int unit, int proc_idx, bcmbd_fwm_proc_t *proc,
                   int *num_instances)
{
    bcmbd_fwm_processors_t *processors = NULL;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Parameter validation */
    SHR_NULL_CHECK(proc, SHR_E_PARAM);
    SHR_NULL_CHECK(num_instances, SHR_E_PARAM);

    processors = bcmbd_fwm_proc_db[unit];
    SHR_NULL_CHECK(processors, SHR_E_INIT);

    SHR_NULL_CHECK(processors->proc_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(processors->proc_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    if ((proc_idx < 0) || (proc_idx >= processors->proc_num)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(proc, &processors->proc_drv[proc_idx], sizeof(bcmbd_fwm_proc_t));

    *num_instances = processors->proc_inst_num[proc_idx];

exit:
    if (locked && processors->proc_mutex) {
        sal_mutex_give(processors->proc_mutex);
    }
    SHR_FUNC_EXIT();
}
