/*! \file bcmcfg_read_outer.c
 *
 * The BCMCFG outer reader is responsible for handling the outermost
 * map keys in the configuration file, and dispatching to the next
 * level of the read handlers.
 *
 * The outer reader is dispatched to additional readers to the
 * bcmcfg_read_handler structure.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_types.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/bcmcfg_read_handler.h>
#include <bcmcfg/bcmcfg_read_util.h>
#include <bcmcfg/bcmcfg_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_READ

/* Index of inner handlers */
#define INNER_H 1

/*!
 * \brief Linked list of read handlers.
 *
 * Linked list of read handlers.
 * A read handler can be added to this list by
 * handler registration function.
 */
typedef struct bcmcfg_read_handler_list_s {
    /*! Handler pointer. */
    const bcmcfg_read_handler_t *handler;
    /*! Next pointer. */
    struct bcmcfg_read_handler_list_s *next;
} bcmcfg_read_handler_list_t;

/*!
 * \brief Reader outer data.
 *
 * Reader outer data.
 */
typedef struct bcmcfg_read_outer_data_s {
    /*! Number of read handlers. */
    int count;
    /*! List of read handlers. */
    bcmcfg_read_handler_list_t *list;
} bcmcfg_read_outer_data_t;

/* Outer data */
static bcmcfg_read_outer_data_t bcmcfg_read_outer_data;

/* Forward declarations of the outer reader handler functions and data
   structure. */
static int
bcmcfg_read_outer_setup(bool start,
                        bcmcfg_parse_info_t *info,
                        void *user_data);
static int
bcmcfg_read_outer_scalar(const char *value,
                         bcmcfg_read_context_t *context,
                         void *user_data);
static int
bcmcfg_read_outer_seq(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data);
static int
bcmcfg_read_outer_map(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data);

static int
bcmcfg_read_outer_doc(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data);

static int
bcmcfg_read_outer_abort(bcmcfg_read_context_t *context,
                        void *user_data);

/* Handler List head */
static bcmcfg_read_handler_list_t* list_head = NULL;
/* Handler List tail */
static bcmcfg_read_handler_list_t* list_tail = NULL;

/* Outer reader handler. */
static const bcmcfg_read_handler_t bcmcfg_read_outer = {
    .key = "outer",
    .setup  = bcmcfg_read_outer_setup,
    .doc    = bcmcfg_read_outer_doc,
    .scalar = bcmcfg_read_outer_scalar,
    .seq    = bcmcfg_read_outer_seq,
    .map    = bcmcfg_read_outer_map,
    .abort  = bcmcfg_read_outer_abort,
    .user_data = &bcmcfg_read_outer_data
};

/*!
 * \brief Handler pointer with parsing info.
 */
typedef struct bcmcfg_read_handler_info_s {
    /*! Handler pointer. */
    const bcmcfg_read_handler_t *handler;

    /*! Parse mode. */
    bcmcfg_parse_mode_t mode;
} bcmcfg_read_handler_info_t;

/*!
 * \brief List of read handlers.
 */
static bcmcfg_read_handler_info_t all_read_handlers[] = {
    {
        .handler = &bcmcfg_read_component,
        .mode    = PARSE_MODE_GEN,
    },
    {
        .handler = &bcmcfg_read_device,
        .mode    = PARSE_MODE_UNIT_EARLY,
    },
    {
        .handler = &bcmcfg_read_device,
        .mode    = PARSE_MODE_UNIT,
    },
};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if the string is valid read handler key.
 *
 * \param [in]    str           Key string.
 *
 * \retval True  Valid key string.
 * \retval False Invalid key string.
 */
static bool
bcmcfg_is_valid_read_handler_key(const char *str)
{
    uint32_t num;
    uint32_t i;
    bcmcfg_read_handler_info_t *hi = NULL;
    bool valid = false;

    num = sizeof(all_read_handlers)/sizeof(bcmcfg_read_handler_info_t);

    for (i = 0; i < num; i++) {
        hi = &all_read_handlers[i];
        if (!sal_strcmp(str, hi->handler->key)) {
            valid = true;
        }
    }

    return valid;
}

/*!
 * \brief Register all handlers by parse mode.
 *
 * \param [in]    mod           Parse mode.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_handler_register_by_mode(bcmcfg_parse_mode_t mode)
{
    uint32_t num;
    uint32_t i;
    bcmcfg_read_handler_info_t *hi = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    num = sizeof(all_read_handlers)/sizeof(bcmcfg_read_handler_info_t);

    for (i = 0; i < num; i++) {
        hi = &all_read_handlers[i];
        if (hi->mode != mode) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_read_handler_register(hi->handler));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Unregister all handlers for other parse mode.
 *
 * \param [in]    mod           Parse mode.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_handler_unregister_by_other_mode(bcmcfg_parse_mode_t mode)
{
    uint32_t num;
    uint32_t i;
    bcmcfg_read_handler_info_t *hi = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    num = sizeof(all_read_handlers)/sizeof(bcmcfg_read_handler_info_t);

    for (i = 0; i < num; i++) {
        hi = &all_read_handlers[i];
        if (hi->mode == mode) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_read_handler_unregister(hi->handler));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader init/cleanup handler.
 *
 * Initialize or cleanup outer reader handler. Currently nothing to
 * do.
 *
 * \param [in]    start         true if init, false if cleanup.
 * \param [in]    user_data     Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_setup(bool start,
                        bcmcfg_parse_info_t *info,
                        void *user_data)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(start);
    COMPILER_REFERENCE(info);
    COMPILER_REFERENCE(user_data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader scalar handler.
 *
 * Only map keys need to be dealt with, so search for an inner reader
 * handler with the given key, starting with read handler 1 (this
 * handler is at zero).
 *
 * \param [in]     value        Scalar value.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_scalar(const char *value,
                         bcmcfg_read_context_t *context,
                         void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_read_handler_list_t* list = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    list = list_head;
    while (list) {
        if (list == list_head) {
            /* Skip first outer list.*/
            list = list->next;
            continue;
        }

        if (!sal_strcmp(value, list->handler->key)) {
            break;
        }
        /* next */
        list = list->next;
    }

    if (list) {
        /* transition to next read handler - the handler is set in
           user_data at this level, and the subsequent map handler
           will retrieve it. */
        info->user_data = (void *)(list->handler);
    } else {
        /* check if scalar key is valid key. */
        if (!bcmcfg_is_valid_read_handler_key(value)) {
            SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
        } else {
            bcmcfg_read_skip_set(context);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader sequence handler.
 *
 * Currently nothing to do.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_seq(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(start);
    COMPILER_REFERENCE(context);
    COMPILER_REFERENCE(user_data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader map handler.
 *
 * Start a new mapping by picking of the new reader handler from the
 * previous level user_data set by bcmcfg_read_outer_scalar().
 *
 * \param [in]     start        true if init, false if cleanup.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_map(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_read_handler_t *new_handler;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    /* handler for this level is in user_data of previous if it exists
       - otherwise don't touch the handler. */
    if (start) {
        new_handler = (bcmcfg_read_handler_t *)((info-1)->user_data);
        if (new_handler) {
            info->handler = new_handler;
            info->user_data = NULL;
        }
    } else {
        bcmcfg_read_skip_clear(context);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader doc handler.
 *
 * Call all the inner doc handlers.
 *
 * \param [in]     start        true if init, false if cleanup.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_doc(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data)
{
    bcmcfg_read_outer_data_t *data = (bcmcfg_read_outer_data_t *)user_data;
    bcmcfg_read_handler_list_t *list = data->list;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (list) {
        if (list == list_head) {
            list = list->next;
            continue;
        }

        if (list->handler && list->handler->doc) {
            int rv = (list->handler->doc)(start,
                                          context,
                                          list->handler->user_data);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_CONT(rv);
            }
        }
        list = list->next;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader abort handler.
 *
 * Call all the inner abort handlers.
 *
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_abort(bcmcfg_read_context_t *context,
                        void *user_data)
{
    bcmcfg_read_outer_data_t *data = (bcmcfg_read_outer_data_t *)user_data;
    bcmcfg_read_handler_list_t *list = data->list;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (list) {
        if (list == list_head) {
            list = list->next;
            continue;
        }

        if (list->handler && list->handler->abort) {
            int rv = (list->handler->abort)(context,
                                            list->handler->user_data);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_CONT(rv);
            }
        }
        list = list->next;
    }

    SHR_FUNC_EXIT();
}

static int
bcmcfg_read_outer_data_initialize(void)
{
    int count = 0;
    bcmcfg_read_handler_list_t* list = NULL;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    list = list_head;
    do {
        while (list) {
            count++;
            list = list->next;
        }

        bcmcfg_read_outer_data.count = count;
        bcmcfg_read_outer_data.list = list_head;
    } while (0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Registered reader setup and cleanup.
 *
 * Call all the registered reader setup functions.
 *
 * \param [in]    start         true if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_setup_cleanup(bool start, bcmcfg_parse_info_t *info)
{
    bcmcfg_read_handler_list_t *list = NULL;
    const bcmcfg_read_handler_t *handler = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(info, SHR_E_INTERNAL);

    if (start) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_read_handler_unregister_by_other_mode(info->mode));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_read_handler_register_by_mode(info->mode));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_read_outer_data_initialize());
    }
    list = list_head;
    while (list) {
        handler = list->handler;
        if (!handler->setup) {
            /* setup not needed */
            continue;
        }
        handler->setup(start, info, handler->user_data);
        /* next */
        list = list->next;
    }
    if (!start) {
        if (info->mode == PARSE_MODE_GEN) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmcfg_read_handler_unregister_all());
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find read handler by key.
 *
 * Find read handler by key.
 *
 * \param [in]     handler      Read handler.
 *
 * \retval 0  NOT FOUND
 * \retval 1  FOUND
 */
static int
bcmcfg_read_handler_find(const bcmcfg_read_handler_t *handler)
{
    bcmcfg_read_handler_list_t *list = list_head;
    int found = 0;

    while (list) {
        if (!sal_strcmp(handler->key, list->handler->key)) {
            found = 1;
            break;
        }
        list = list->next;
    }

    return found;
}

/*!
 * \brief Add handler to the read handler list.
 *
 * Add handler to the read handler list.
 *
 * \param [in]     handler      Read handler.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_handler_list_add(const bcmcfg_read_handler_t *handler)
{
    bcmcfg_read_handler_list_t *list = NULL;
    size_t size = sizeof(bcmcfg_read_handler_list_t);
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        if (bcmcfg_read_handler_find(handler)) {
            break;
        }

        list = sal_alloc(size, "bcmcfgReadHandlerListAlloc");
        if (list) {
            sal_memset(list, 0, size);
        } else {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }
        list->handler = handler;
        if (list_head == NULL) {
            list_head = list;
        }
        if (list_tail) {
            list_tail->next = list;
        }
        list_tail = list;
    } while (0);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(list);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a read handler from list.
 *
 * Delete a read handler from list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_handler_list_delete(const bcmcfg_read_handler_t *handler)
{
    bcmcfg_read_handler_list_t *list = NULL;
    bcmcfg_read_handler_list_t *prev = NULL;
    bcmcfg_read_handler_list_t *next = NULL;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        list = prev = list_head;
        while (list) {
            next = list->next;
            /* Handler found */
            if (!sal_strcmp(handler->key, list->handler->key)) {
                /* Set next, head, tail pointer. */
                prev->next = next;
                if (list == list_head) {
                    list_head = NULL;
                }
                if (list == list_tail) {
                    list_tail = prev;
                }
                /* Free and exit loop. */
                sal_free(list);
                break;
            }
            /* Set prev, and go next. */
            prev = list;
            list = next;
        }
    } while (0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all read handler from list.
 *
 * Delete all read handler from list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_handler_list_delete_all(void)
{
    bcmcfg_read_handler_list_t *list = NULL;
    bcmcfg_read_handler_list_t *next = NULL;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        list = list_head;
        while (list) {
            next = list->next;

            sal_free(list);
            list = next;
        }
        list_head = list_tail = NULL;
    } while (0);

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*
 * Initialize the reader context by start out in stream context, using
 * this outer reader.
 */
int
bcmcfg_read_outer_init_context(bcmcfg_read_context_t *context)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (context) {
        context->info[context->level].handler = &bcmcfg_read_outer;
    } else {
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    }
    SHR_FUNC_EXIT();
}

/*
 * Call all the registered reader setup functions.
 */
int
bcmcfg_read_outer_gen_init(void)
{
    bcmcfg_parse_info_t pm = {
        .mode = PARSE_MODE_GEN,
        .target_unit = BSL_UNIT_UNKNOWN
    };
    return bcmcfg_read_outer_setup_cleanup(true, &pm);
}

/*
 * Call all the registered reader cleanup functions.
 */
int
bcmcfg_read_outer_gen_cleanup(void)
{
    bcmcfg_parse_info_t pm = {
        .mode = PARSE_MODE_GEN,
        .target_unit = BSL_UNIT_UNKNOWN
    };
    return bcmcfg_read_outer_setup_cleanup(false, &pm);
}

int
bcmcfg_read_outer_unit_init(int unit,
                            bcmcfg_init_stage_t stage)
{
    bcmcfg_parse_info_t pm = {
        .mode = PARSE_MODE_UNIT,
        .target_unit = unit
    };
    if (stage == INIT_STAGE_EARLY) {
        pm.mode = PARSE_MODE_UNIT_EARLY;
    }
    return bcmcfg_read_outer_setup_cleanup(true, &pm);
}

int
bcmcfg_read_outer_unit_cleanup(int unit)
{
    bcmcfg_parse_info_t pm = {
        .mode = PARSE_MODE_UNIT,
        .target_unit = unit
    };
    return bcmcfg_read_outer_setup_cleanup(false, &pm);
}

int
bcmcfg_read_handler_register(const bcmcfg_read_handler_t *handler)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (list_head == NULL) {
        /* bcmcfg_read_outer must be first. */
        SHR_IF_ERR_EXIT(
            bcmcfg_read_handler_list_add(&bcmcfg_read_outer));
    }

    SHR_IF_ERR_EXIT(
        bcmcfg_read_handler_list_add(handler));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_read_handler_unregister(const bcmcfg_read_handler_t *handler)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        bcmcfg_read_handler_list_delete(handler));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_read_handler_unregister_all(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        bcmcfg_read_handler_list_delete_all());

exit:
    SHR_FUNC_EXIT();
}
