/*! \file bcmcfg_read.c
 *
 * YAML file reader
 *
 * The BCMCFG YAML reader is divided into two parts. The main reader
 * based on LibYAML is here, and calls out to specialized readers
 * based on a map key.
 *
 * See http://pyyaml.org/wiki/LibYAML to understand the LibYAML
 * interface. The design of this reader is based on the valid YAML
 * event grammar described at the link above and reproduced here:
 *
 *    stream ::= STREAM-START document* STREAM-END
 *    document ::= DOCUMENT-START node DOCUMENT-END
 *    node ::= ALIAS | SCALAR | sequence | mapping
 *    sequence ::= SEQUENCE-START node* SEQUENCE-END
 *    mapping ::= MAPPING-START (node node)* MAPPING-END
 *
 * This interface uses LibYAML events to drive a reader state machine,
 * and the reader state machine uses a "context stack" to keep track
 * of the reader state. LibYAML events are read in a loop until an
 * event indicates that event processing is complete or some error was
 * detected. All common event handling, such as managing context,
 * separating map keys and values, and sequence counting are handled
 * in this reader. Almost all events push and pop elements on the
 * context stack, and events are dispatched via the context stack
 * to the read handlers.
 *
 * The config file itself consists of a single stream and zero or more
 * documents in that stream. Each document contains a map, where the
 * keys of the map indicate which specialized reader to call for
 * subsequent events. The specific interpretation of the YAML values
 * is handled by the specialized readers themselves.
 *
 * Each specialized reader is associated with a reader handler. The
 * reader handler indicates which outer key it handles, has a set of
 * driver functions for each YAML event, and a pointer to private user
 * data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/bcmcfg_read_util.h>
#include <bcmcfg/comp/bcmcfg_cfg_error_control.h>
#include <bcmcfg/bcmcfg_player.h>

/* External OSS dependency */
#include <yaml.h>

/* YAML library dependencies for File IO */
#include <stdio.h>
#include <errno.h>

#include <bcmcfg/bcmcfg_yaml_anchor.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_READ

#if !defined(BCMCFG_USE_YAML_FILE)
#define BCMCFG_USE_YAML_FILE 1
#endif

/*!
 * \brief YAML merge operator state.
 */
typedef enum bcmcfg_merge_op_state_e {
    /*! YAML merge operator not read. */
    BCMCFG_MERGE_OP_NONE,

    /*! YAML merge operator with single map. */
    BCMCFG_MERGE_OP_SINGLE,

    /*! YAML merge operator with multi maps. */
    BCMCFG_MERGE_OP_MULTI,
} bcmcfg_merge_op_state_t;

/*!
 * \brief YAML anchor node list for merge operator.
 */
typedef struct bcmcfg_merge_anchor_node_s {
    /*! Anchor database. */
    bcmcfg_yaml_anchor_db_t *anc_db;

    /*! Next pointer. */
    struct bcmcfg_merge_anchor_node_s *next;
} bcmcfg_merge_anchor_node_t;

/*! YAML Merge Operator. */
#define YAML_MERGE_OP       "<<"

/* Keep track of these events for diagnostic purposes. */
static int bcmcfg_stream_count;
static int bcmcfg_no_event_count;

/* Reader context stack. */
static bcmcfg_read_context_t bcmcfg_context;

/* Merge operator read state. */
static bcmcfg_merge_op_state_t bcmcfg_merge_op_state = BCMCFG_MERGE_OP_NONE;

/* Merge anchor node. */
static bcmcfg_merge_anchor_node_t *merge_anchor = NULL;

/* Input parse request lists. */
static bcmcfg_read_data_t *bcmcfg_read_data_head;
static bcmcfg_read_data_t *bcmcfg_read_data_tail;

/* Scan error for unit tests. Deliberately not static so unit tests
   can access. */
int bcmcfg_scan_rv = SHR_E_NONE;


/*******************************************************************************
 * Private functions
 */

static int
bcmcfg_handle_event(const bcmcfg_read_handler_t *handler,
                    bcmcfg_read_context_t *context,
                    const yaml_event_t *event,
                    bool *done);

static int
bcmcfg_abort(const bcmcfg_read_handler_t *handler,
             bcmcfg_read_context_t *context);

/*!
 * \brief Push reader state.
 *
 * Push the reader state on the context stack. By default, the
 * handler and sequence container is the same as the previous
 * element on the stack. Will return an error of the context
 * stack overflows.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     state        Context state.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_push_state(bcmcfg_read_context_t *context,
                  bcmcfg_read_state_t state)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (context->level < (BCMCFG_MAX_LEVEL - 2)) {
        context->level++;
        info = context->info + context->level;
        info->state = state;
        info->handler = (info-1)->handler;
        info->seq_container = (info-1)->seq_container;
        info->locus = (info-1)->locus;
        info->line = (info-1)->line;
        info->column = (info-1)->column;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META("Context level overflow\n")));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pop reader state.
 *
 * Reset items on the top element of reader context stack, and then
 * pop it off the stack. Will return an error if the context stack
 * underflows.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_pop_state(bcmcfg_read_context_t *context)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (context->level > 0) {
        info->state = BCMCFG_READ_STATE_NONE;
        info->handler = NULL;
        info->user_data = NULL;
        context->level--;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META("Context level underflow\n")));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set YAML merge operator state.
 *
 * \param [in]     state        Merge operator read state..
 */
static void
bcmcfg_merge_op_state_set(bcmcfg_merge_op_state_t state)
{
    bcmcfg_merge_op_state = state;
    return;
}

/*!
 * \brief check if given state is equal to bcmcfg_merge_op_state.
 *
 * \param [in]     state        Merge operator read state..
 *
 * \retval TRUE    bcmcfg_merge_op_state == state.
 * \retval FALSE   bcmcfg_merge_op_state != state.
 */
static bool
is_bcmcfg_merge_op_state(bcmcfg_merge_op_state_t state)
{
    return (bcmcfg_merge_op_state == state);
}

/*!
 * \brief Create anchor merge database and link it to merge_anchor list.
 *
 * \param [in]     db    Anchor database.
 *
 * \retval TRUE    bcmcfg_merge_op_state == state.
 * \retval FALSE   bcmcfg_merge_op_state != state.
 */
static int
bcmcfg_yaml_anchor_merge_db_create(bcmcfg_yaml_anchor_db_t *db)
{
    bcmcfg_merge_anchor_node_t *node = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_ALLOC(node, sizeof(*node), "bcmcfgYamlMergeAnchorNode");
    SHR_NULL_CHECK(node, SHR_E_MEMORY);

    /* Insert node to head of linked list. */
    node->anc_db = db;
    node->next   = merge_anchor;

    merge_anchor = node;

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(node);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle all events within the anchor database.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     event        LibYAML event.
 * \param [in]     list         Anchor database.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_yaml_handle_anchor_events(bcmcfg_read_context_t* context,
                                 const yaml_event_t *event,
                                 bcmcfg_yaml_anchor_db_t *list)
{
    bcmcfg_yaml_event_list_t *cur_event_list = NULL;
    int nrv = 0;
    bool done = false;
    bcmcfg_read_level_info_t *info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    info = context->info + context->level;
    info->locus = ""; /* locus */
    info->line = (int)event->start_mark.line+1;
    info->column = (int)event->start_mark.column+1;
    cur_event_list = list->event_list;
    while (cur_event_list) {
        /* Skip unnecessary events while referencing an anchor database. */
        if (!IS_ANCHOR_REF_COLLECTED_EVENT_TYPE(cur_event_list->event->type)) {
            cur_event_list = cur_event_list->next;
            continue;
        }

        if (info->handler) {
            nrv = bcmcfg_handle_event(info->handler,
                                      context,
                                      cur_event_list->event,
                                      &done);
            if (SHR_FAILURE(nrv)) {
                LOG_ERROR
                    (BSL_LOG_MODULE,
                     (BSL_META("%s:%d:%d: syntax error.\n"),
                      "", /* locus */
                      info->line,
                      info->column));
                if (info->handler) {
                    (void)bcmcfg_abort(info->handler, context);
                }
            }
            SHR_IF_ERR_EXIT(nrv);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        cur_event_list = cur_event_list->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy merge_anchor.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     event        YAML event.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static void
bcmcfg_yaml_anchor_merge_db_destroy(void)
{
    bcmcfg_merge_anchor_node_t *cur = merge_anchor;
    bcmcfg_merge_anchor_node_t *next = NULL;

    while(cur) {
        next = cur->next;

        SHR_FREE(cur);

        cur = next;
    }

    merge_anchor = NULL;

    return;
}

/*!
 * \brief Handle merge_anchor node.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     event        YAML event.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_yaml_anchor_merge_db_handle(bcmcfg_read_context_t* context,
                                   const yaml_event_t *event)
{
    bcmcfg_merge_anchor_node_t *cur = merge_anchor;
    bcmcfg_merge_anchor_node_t *next = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    while(cur) {
        next = cur->next;
        if (cur->anc_db) {
            SHR_IF_ERR_EXIT
                (bcmcfg_yaml_handle_anchor_events(context, event, cur->anc_db));
        }
        cur = next;
    }

 exit:
    bcmcfg_yaml_anchor_merge_db_destroy();

    SHR_FUNC_EXIT();
}


/*!
 * \brief Handle YAML alias events.
 *
 * If merge operator specified with multiple anchor maps,
 * create anchor merge database to linked list.
 * Otherwise, process all events in anchor database.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     event        LibYAML event.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_alias(bcmcfg_read_context_t *context,
                    const yaml_event_t *event)
{
    bcmcfg_yaml_anchor_db_t *anc_db = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        anc_db = bcmcfg_yaml_anchor_find((char * const) event->data.scalar.anchor);
        if (!anc_db) {
            LOG_ERROR
                (BSL_LOG_MODULE,
                 (BSL_META("Anchor %s is not defined\n"), event->data.scalar.anchor));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (is_bcmcfg_merge_op_state(BCMCFG_MERGE_OP_MULTI)) {
            SHR_IF_ERR_EXIT
                (bcmcfg_yaml_anchor_merge_db_create(anc_db));
            break;
        } else {
            SHR_IF_ERR_EXIT
                (bcmcfg_yaml_handle_anchor_events(context, event, anc_db));
        }
    } while(0);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML scalar events.
 *
 * Dispatch to the scalar event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     event        LibYAML event.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_scalar(const bcmcfg_read_handler_t *handler,
                     bcmcfg_read_context_t *context,
                     const yaml_event_t *event)
{
    const char* svalue = (char*) event->data.scalar.value;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Scalar value is merge operator, "<<".
     * Set YAML merge operator state. */
    if (sal_strcmp(svalue, YAML_MERGE_OP) == 0) {

        bcmcfg_merge_op_state_set(BCMCFG_MERGE_OP_SINGLE);
        SHR_EXIT();
    }

    if (handler->scalar &&
        !bcmcfg_is_read_skip(context)) {
        SHR_IF_ERR_EXIT
            (handler->scalar((char *) svalue,
                             context,
                             handler->user_data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML sequence events.
 *
 * Initialize the sequence container, sequence index, and then
 * dispatch to the sequence event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_sequence(const bcmcfg_read_handler_t *handler,
                       bcmcfg_read_context_t *context,
                       bool start)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* indicate current sequence container */
    context->info[context->level].seq_container = context->level;
    /* start seq_idx at -1 - immediately following level will increment
       seq_idx to 0, next to 1 etc. */
    context->info[context->level].seq_idx = ((uint32_t)-1);

    if (handler->seq &&
        !bcmcfg_is_read_skip(context)) {
        SHR_IF_ERR_EXIT
            (handler->seq(start,
                          context,
                          handler->user_data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML map events.
 *
 * Dispatch to the map event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_map(const bcmcfg_read_handler_t *handler,
                  bcmcfg_read_context_t *context,
                  bool start)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (handler->map) {
        SHR_IF_ERR_EXIT
            (handler->map(start,
                          context,
                          handler->user_data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML stream events.
 *
 * Track the stream count, and set done if the stream has ended.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 * \param [out]    done         Done flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_stream(bcmcfg_read_context_t *context,
                     bool start,
                     bool *done)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (start) {
        if (bcmcfg_stream_count == 0) {
            bcmcfg_stream_count++;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else {
        if (bcmcfg_stream_count == 1) {
            *done = true;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML doc events.
 *
 * Dispatch to the doc event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_doc(const bcmcfg_read_handler_t *handler,
                  bcmcfg_read_context_t *context,
                  bool start)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (handler->doc) {
        SHR_IF_ERR_EXIT(handler->doc(start,
                                     context,
                                     handler->user_data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML no-event events.
 *
 * Increment the no-event counter.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_no_event(bcmcfg_read_context_t *context)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    bcmcfg_no_event_count++;
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML map node state transitions.
 *
 * If the current state is MAP, then transition to MAP_V
 * If the current state is MAP_V, then transition to MAP
 * Otherwise make no change to the state.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_map_state(bcmcfg_read_context_t *context)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (info->state == BCMCFG_READ_STATE_MAP) {
        info->state = BCMCFG_READ_STATE_MAP_V;
    } else if (info->state == BCMCFG_READ_STATE_MAP_V) {
        info->state = BCMCFG_READ_STATE_MAP;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Increment sequence index.
 *
 * Increment the sequence index if we are at the same level as the
 * sequence container.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_incr_seq_idx(bcmcfg_read_context_t *context)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (info->seq_container == context->level) {
        context->info[info->seq_container].seq_idx++;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief LibYAML event type to string.
 *
 * Convert a LibYAML event type enum to a string.
 *
 * \param [in]     event_type   Event type.
 *
 * \retval string
 */
static char *
y_event_str(yaml_event_type_t event_type)
{
    char *s = "UNKNOWN";

    switch (event_type) {
    case YAML_NO_EVENT:
        s = "NO_EVENT";
        break;

    case YAML_STREAM_START_EVENT:
        s = "STREAM_START";
        break;

    case YAML_STREAM_END_EVENT:
        s = "STREAM_END";
        break;

    case YAML_DOCUMENT_START_EVENT:
        s = "DOCUMENT_START";
        break;

    case YAML_DOCUMENT_END_EVENT:
        s = "DOCUMENT_END";
        break;

    case YAML_ALIAS_EVENT:
        s = "ALIAS";
        break;

    case YAML_SCALAR_EVENT:
        s = "SCALAR";
        break;

    case YAML_SEQUENCE_START_EVENT:
        s = "SEQUENCE_START";
        break;

    case YAML_SEQUENCE_END_EVENT:
        s = "SEQUENCE_END";
        break;

    case YAML_MAPPING_START_EVENT:
        s = "MAPPING_START";
        break;

    case YAML_MAPPING_END_EVENT:
        s = "MAPPING_END";
        break;
    }

    return s;
}

/*!
 * \brief Handle LibYAML events.
 *
 * Manage event context and dispatch to event handlers.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     event        LibYAML event.
 * \param [out]    done         Done flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_event(const bcmcfg_read_handler_t *handler,
                    bcmcfg_read_context_t *context,
                    const yaml_event_t *event,
                    bool *done)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (event->type == YAML_SCALAR_EVENT) {
        LOG_DEBUG
            (BSL_LOG_MODULE,
             (BSL_META("%02d %s '%s'\n"),
              context->level,
              y_event_str(event->type),
              event->data.scalar.value));
    } else {
        LOG_DEBUG
            (BSL_LOG_MODULE,
             (BSL_META("%02d %s\n"),
              context->level, y_event_str(event->type)));
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_yaml_anchor_register(event));

    SHR_IF_ERR_EXIT
        (bcmcfg_yaml_anchor_event_process(event));

    SHR_IF_ERR_EXIT
        (bcmcfg_yaml_anchor_db_update());

    switch (event->type) {
    case YAML_NO_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_no_event(context));
        break;

    case YAML_STREAM_START_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_stream(context, true, done));
        break;

    case YAML_STREAM_END_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_stream(context, false, done));
        break;

    case YAML_DOCUMENT_START_EVENT:
        bcmcfg_read_skip_document_clear();
        SHR_IF_ERR_EXIT
            (bcmcfg_push_state(context, BCMCFG_READ_STATE_DOC));
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_doc(handler, context, true));
        break;

    case YAML_DOCUMENT_END_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_doc(handler, context, false));
        SHR_IF_ERR_EXIT
            (bcmcfg_pop_state(context));
        break;

    case YAML_ALIAS_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_alias(context, event));
        if (is_bcmcfg_merge_op_state(BCMCFG_MERGE_OP_SINGLE)) {
            bcmcfg_merge_op_state_set(BCMCFG_MERGE_OP_NONE);
        }
        break;

    case YAML_SCALAR_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_incr_seq_idx(context));
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_scalar(handler, context, event));
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_map_state(context));
        break;

    case YAML_SEQUENCE_START_EVENT:
        if (is_bcmcfg_merge_op_state(BCMCFG_MERGE_OP_SINGLE)) {
            bcmcfg_merge_op_state_set(BCMCFG_MERGE_OP_MULTI);
        }
        SHR_IF_ERR_EXIT
            (bcmcfg_incr_seq_idx(context));
        SHR_IF_ERR_EXIT
            (bcmcfg_push_state(context, BCMCFG_READ_STATE_SEQ));
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_sequence(handler, context, true));
        break;

    case YAML_SEQUENCE_END_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_sequence(handler, context, false));
        SHR_IF_ERR_EXIT
            (bcmcfg_pop_state(context));
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_map_state(context));
        if (is_bcmcfg_merge_op_state(BCMCFG_MERGE_OP_MULTI)) {
            SHR_IF_ERR_EXIT
                (bcmcfg_yaml_anchor_merge_db_handle(context, event));
        }
        bcmcfg_merge_op_state_set(BCMCFG_MERGE_OP_NONE);
        break;

    case YAML_MAPPING_START_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_incr_seq_idx(context));
        SHR_IF_ERR_EXIT
            (bcmcfg_push_state(context, BCMCFG_READ_STATE_MAP));
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_map(handler, context, true));
        break;

    case YAML_MAPPING_END_EVENT:
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_map(handler, context, false));
        SHR_IF_ERR_EXIT
            (bcmcfg_pop_state(context));
        SHR_IF_ERR_EXIT
            (bcmcfg_handle_map_state(context));
        break;

    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LibYAML parse loop.
 *
 * Parse and handle LibYAML events until done or an error occurs.
 * If there is an error from a handler, display an error message.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     locus        Error locus.
 * \param [in,out] parser       LibYAML parser.
 * \param [out]    error        Error flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */

/*!
 * \brief Dispatch abort notification.
 *
 * Dispatch abort notification to event handlers.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_abort(const bcmcfg_read_handler_t *handler,
             bcmcfg_read_context_t *context)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (handler->abort) {
        SHR_IF_ERR_EXIT
            (handler->abort(context,
                            handler->user_data));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief LibYAML parse loop.
 *
 * Parse and handle LibYAML events until done or an error occurs.
 * If there is an error from a handler, display an error message.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     locus        Error locus.
 * \param [in,out] parser       LibYAML parser.
 * \param [out]    error        Error flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse_loop(bcmcfg_read_context_t *context,
                  const char *locus,
                  yaml_parser_t *parser,
                  bool *error)
{
    yaml_event_t event;
    int yrv = 0;
    int nrv = 0;
    int rc = SHR_E_NONE;
    bool done = false;
    bool err_report = false;
    bcmcfg_read_level_info_t *info;
    bcmcfg_cfg_error_control_config_t ec;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        bcmcfg_cfg_error_control_config_get(&ec));
    if (ec.stop_playback_on_error) {
        err_report = true;
    }

    /* Initialize anchor database. */
    bcmcfg_yaml_anchor_db_init();

    do {
        yrv = yaml_parser_parse(parser, &event);
        if (!yrv) {
            *error = true;
            break;
        }

        /* Get the current handler. */
        info = context->info + context->level;
        info->locus = locus;
        info->line = (int)event.start_mark.line+1;
        info->column = (int)event.start_mark.column+1;
        if (info->handler) {
            nrv = bcmcfg_handle_event(info->handler,
                                      context,
                                      &event,
                                      &done);

            if (SHR_FAILURE(nrv)) {
                LOG_ERROR
                    (BSL_LOG_MODULE,
                     (BSL_META("%s:%d:%d: syntax error.\n"),
                      locus,
                      info->line,
                      info->column));
                if (info->handler) {
                    (void)bcmcfg_abort(info->handler, context);
                }
            }
        } else {
            /* handler is NULL? */
            nrv = SHR_E_INTERNAL;
        }

        yaml_event_delete(&event);

        if (SHR_FAILURE(nrv)) {
            if (err_report) {
                SHR_IF_ERR_EXIT(nrv);
            }
            /* Store error code. */
            rc = nrv;
            /* Skip events until next DOCUMENT_START. */
            bcmcfg_read_skip_document_set();
        }
    } while (!done);

    SHR_IF_ERR_EXIT(rc);

exit:
    /* Cleanup anchor database. */
    bcmcfg_yaml_anchor_db_cleanup();

    SHR_FUNC_EXIT();
}

/*!
 * \brief LibYAML parser.
 *
 * Initialize and run LibYAML parser.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     locus        Error locus.
 * \param [in,out] parser       LibYAML parser.
 * \param [out]    error        Error flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse(bcmcfg_read_context_t *context,
             const char *locus,
             yaml_parser_t *parser,
             bool *error)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Initialize stream count. */
    bcmcfg_stream_count = 0;
    /* Initialize context. */
    sal_memset(context->info, 0x0,
               sizeof(bcmcfg_read_level_info_t) * BCMCFG_MAX_LEVEL);
    context->level = 0;
    context->info[context->level].state = BCMCFG_READ_STATE_STREAM;
    /* seq_container will always be > 0 because, by definition, the
       first two levels are stream and doc, so 0 is suitable to
       indicate there is no sequence container. */
    context->info[context->level].seq_container = 0;
    context->info[context->level].seq_idx = 0;

    bcmcfg_read_skip_clear(context);

    /* Initialize outer reader context. */
    SHR_IF_ERR_EXIT
        (bcmcfg_read_outer_init_context(context));

    /* Run the parse loop. */
    SHR_IF_ERR_EXIT
        (bcmcfg_parse_loop(context, locus, parser, error));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Issue parser error.
 *
 * Error message shows error locus (filename or "<string>"), error
 * location, and any additional context information if available.
 *
 * \param [in]    locus         Error locus such as file name.
 * \param [in]    parser        LibYAML parser.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse_error(const char *locus, const yaml_parser_t *parser)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (parser->context) {
        LOG_ERROR
            (BSL_LOG_MODULE,
             (BSL_META("%s:%d:%d: %s %s\n"),
              locus,
              (int)parser->problem_mark.line+1,
              (int)parser->problem_mark.column+1,
              parser->problem,
              parser->context));
    } else {
        LOG_ERROR
            (BSL_LOG_MODULE,
             (BSL_META("%s:%d:%d: %s\n"),
              locus,
              (int)parser->problem_mark.line+1,
              (int)parser->problem_mark.column+1,
              parser->problem));
    }
    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

#if BCMCFG_USE_YAML_FILE
static long int
bcmcfg_read_file_size(FILE *f)
{
    long int size = 0;

    do {
        if (f == NULL) {
            break;
        }

        if (fseek(f, 0, SEEK_END) != 0) {
            break;
        }
        size = ftell(f);
        if (fseek(f, 0, SEEK_SET) != 0) {
            break;
        }
    } while (0);

    return size;
}

static int
bcmcfg_read_file_to_string(const char *file, const char **str)
{
    char *buffer = 0;
    long int length, res;
    FILE *f;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if ((f = fopen(file, "r")) != NULL) {
        length = bcmcfg_read_file_size(f);
        if (length <= 0) {
            fclose (f);
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_ALLOC(buffer, length + 1, "bcmcfgFileString");
        if (buffer == NULL) {
            fclose (f);
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        res = (long) fread(buffer, 1, length, f);
        if (res != length || ferror(f)) {
            fclose (f);
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        buffer[length] = '\0';
        fclose (f);
        *str = buffer;
    } else if (errno == ENOENT) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(buffer);
    }
    SHR_FUNC_EXIT();
}
#endif

/*!
 * \brief Append YAML data to reader list.
 *
 * Append YAML data to reader list.
 *
 * \param [in]    data          File path or YAML string.
 * \param [in]    format        Reader format.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_data_append(const char *data, bcmcfg_read_format_t format)
{
    bcmcfg_read_data_t *read;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Alloc record. */
    read = sal_alloc(sizeof(*read), "bcmcfgReadDataAppend");
    if (read == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Initialize record. */
    sal_memset(read, 0, sizeof(*read));
    read->format = format;
    switch (format) {
#if BCMCFG_USE_YAML_FILE
    case FILE_INPUT:
        read->fpath = sal_strdup(data);
        rv = bcmcfg_read_file_to_string(read->fpath, &read->data);
        if (rv == SHR_E_NOT_FOUND) {
            /*
             * File not found. Reset error code in order to
             * continue to bcmcfg_scan.
             */
            rv = SHR_E_NONE;
        } else {
            /*
             * Change format type to STRING_INPUT in order to read
             * cached data in the next attach process.
             */
            read->format = STRING_INPUT;
        }
        break;
#endif
    case STRING_INPUT:
        read->fpath = sal_strdup("<YAML string source>");
        read->data = sal_strdup(data);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (SHR_FAILURE(rv)) {
        SHR_FREE(read->fpath);
        SHR_FREE(read->data);
        SHR_FREE(read);
        SHR_ERR_EXIT(rv);
    }

    /* Link into list. */
    if (bcmcfg_read_data_head == NULL) {
        bcmcfg_read_data_head = read;
        bcmcfg_read_data_tail = read;
    } else {
        bcmcfg_read_data_tail->next = read;
        bcmcfg_read_data_tail = read;
    }

exit:
    SHR_FUNC_EXIT();
}

#if BCMCFG_USE_YAML_FILE
/*!
 * \brief Parse a BCMCFG format YAML file.
 *
 * Initialize parser, file open/close, and so on. Note that this
 * deliberately does not use SAL interfaces, as this is a primary
 * LibYAML interface, and LibYAML doesn't use SAL either. This library
 * may be configured not to use a file interface, in which case, this
 * function is compiled out.
 *
 * \param [in]  file            File to parse.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_scan_file(const char *file)
{
    yaml_parser_t parser;
    bool error = false;
    bool init = false;
    FILE *f;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (yaml_parser_initialize(&parser)) {
        init = true;
        if ((f = fopen(file, "r")) != NULL) {
            yaml_parser_set_input_file(&parser, f);
            SHR_IF_ERR_EXIT
                (bcmcfg_parse(&bcmcfg_context,
                              file, &parser, &error));
            if (error) {
                SHR_IF_ERR_EXIT
                    (bcmcfg_parse_error(file, &parser));
            }
            fclose(f);
        } else if (errno == ENOENT) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (init) {
        yaml_parser_delete(&parser);
    }

    SHR_FUNC_EXIT();
}
#endif


/*!
 * \brief Parse YAML string.
 *
 * Parse the given string as YAML data.
 *
 * \param [in]     str          YAML formatted string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_scan_string(const char *str)
{
    yaml_parser_t parser;
    bool error = false;
    bool init = false;
    const char *locus = "<string>";
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (str != NULL) {
        if (yaml_parser_initialize(&parser)) {
            init = true;
            yaml_parser_set_input_string(&parser,
                                         (const unsigned char *)str,
                                         strlen(str));

            SHR_IF_ERR_EXIT
                (bcmcfg_parse(&bcmcfg_context,
                              locus, &parser, &error));

        }

        if (error) {
            SHR_IF_ERR_EXIT
                (bcmcfg_parse_error(locus, &parser));
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (init) {
        yaml_parser_delete(&parser);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse YAML requests.
 *
 * Parse any YAML requests made via bcmcfg_string_parse() or
 * bcmcfg_file_parse().
 *
 * \param [in]     str          YAML formatted string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_scan(void)
{
    int rv = SHR_E_NONE;
    bcmcfg_read_data_t *read = bcmcfg_read_data_head;
    bcmcfg_read_data_t *next;
    bcmcfg_cfg_error_control_config_t ec;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (SHR_FAILURE(bcmcfg_scan_rv)) {
        SHR_ERR_EXIT(rv);
    }

    while (read) {
        next = read->next;
        switch (read->format) {
#if BCMCFG_USE_YAML_FILE
        case FILE_INPUT:
            bcmcfg_scan_rv = bcmcfg_scan_file(read->fpath);
            break;
#endif
        case STRING_INPUT:
            bcmcfg_scan_rv = bcmcfg_scan_string(read->data);
            break;
        default:
            bcmcfg_scan_rv = SHR_E_INTERNAL;
            break;
        }

        /* If stop_playback_on_error is set, report error. */
        rv = bcmcfg_cfg_error_control_config_get(&ec);
        if (SHR_SUCCESS(rv) && ec.stop_playback_on_error &&
            SHR_FAILURE(bcmcfg_scan_rv)) {
            rv = bcmcfg_scan_rv;
            break;
        }
        read = next;
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();

}


/*******************************************************************************
 * Public functions
 */

/*
 * Stage a YAML file for parsing.
 */
int
bcmcfg_file_parse(const char *file)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (file != NULL) {
#if BCMCFG_USE_YAML_FILE
        SHR_IF_ERR_EXIT
            (bcmcfg_read_data_append(file, FILE_INPUT));
#else
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
#endif
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Stage a YAML string for parsing.
 */
int
bcmcfg_string_parse(const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (str != NULL) {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_data_append(str, STRING_INPUT));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the outer reader, which then initializes all other readers,
 * and then scan staged YAML data.
 */
int
bcmcfg_read_gen_init(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    bcmcfg_scan_rv = SHR_E_NONE;

    SHR_IF_ERR_EXIT
        (bcmcfg_read_outer_gen_init());
    SHR_IF_ERR_EXIT
        (bcmcfg_scan());

exit:
    SHR_FUNC_EXIT();
}

/*
 * Clean up the outer reader, which then cleans up all other readers.
 */
int
bcmcfg_read_gen_cleanup(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_CONT
        (bcmcfg_read_data_cleanup());
    SHR_IF_ERR_CONT
        (bcmcfg_read_outer_gen_cleanup());

    SHR_FUNC_EXIT();
}

int
bcmcfg_read_unit_init(int unit,
                      bcmcfg_init_stage_t stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcfg_read_outer_unit_init(unit, stage));
    SHR_IF_ERR_EXIT
        (bcmcfg_scan());
    if (stage == INIT_STAGE_EARLY) {
        SHR_IF_ERR_EXIT
            (bcmcfg_playback_free(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_read_unit_cleanup(int unit)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmcfg_read_outer_unit_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_read_data_get(const bcmcfg_read_data_t **read)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (read == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *read = bcmcfg_read_data_head;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_read_data_cleanup(void)
{
    bcmcfg_read_data_t *read = bcmcfg_read_data_head;
    bcmcfg_read_data_t *next;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    while (read) {
        next = read->next;

        SHR_FREE(read->fpath);
        SHR_FREE(read->data);
        SHR_FREE(read);

        read = next;
    }

    bcmcfg_read_data_head = NULL;
    bcmcfg_read_data_tail = NULL;
    SHR_FUNC_EXIT();
}

