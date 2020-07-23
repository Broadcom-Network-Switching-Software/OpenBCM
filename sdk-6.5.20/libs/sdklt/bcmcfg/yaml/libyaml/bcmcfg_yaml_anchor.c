/*! \file bcmcfg_yaml_anchor.c
 *
 * YAML Anchor/reference utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <shr/shr_error.h>

/* External OSS dependency */
#include <yaml.h>
#include <bcmcfg/bcmcfg_yaml_anchor.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_READ

/*! Yaml anchor input event database.
 *  After anchor is detected, all events will be recorded in this anchor
 *  input database. When START event of MAPPING/SEQUENCE is detected,
 *  event count of all anchors in this input database will be incremented.
 *  When END event of MAPPING/SEQUENCE is detected, event count will be decremented.
 *  This input database has all anchor event currently in recording status.
 *  After event recording is done, the anchor will be removed from this database,
 *  and moved into finalized anchor database, bcmcfg_yaml_anchor_db.
 */
static bcmcfg_yaml_anchor_db_t *bcmcfg_yaml_anchor_input_db = NULL;

/*! Yaml anchor finalized event database. */
static bcmcfg_yaml_anchor_db_t *bcmcfg_yaml_anchor_db = NULL;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Copy yaml event for anchor reference.
 *
 * \param [in] from    Original YAML event.
 * \param [in] to      Copied YAML event.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcfg_yaml_event_copy(const yaml_event_t *from,
                       yaml_event_t **to)
{
    int yrv = 1;

    switch (from->type) {
    case YAML_SCALAR_EVENT:
        yrv = yaml_scalar_event_initialize(*to,
                    NULL, /* Discard anchor info. */
                    from->data.scalar.tag,
                    from->data.scalar.value,
                    from->data.scalar.length,
                    from->data.scalar.plain_implicit,
                    from->data.scalar.quoted_implicit,
                    from->data.scalar.style);
        break;
    case YAML_SEQUENCE_START_EVENT:
        yrv = yaml_sequence_start_event_initialize(*to,
                    NULL, /* Discard anchor info. */
                    from->data.sequence_start.tag,
                    from->data.sequence_start.implicit,
                    from->data.sequence_start.style);
        break;
    case YAML_SEQUENCE_END_EVENT:
        yrv = yaml_sequence_end_event_initialize(*to);
        break;
    case YAML_MAPPING_START_EVENT:
        yrv = yaml_mapping_start_event_initialize(*to,
                    NULL, /* Discard anchor info. */
                    from->data.mapping_start.tag,
                    from->data.mapping_start.implicit,
                    from->data.mapping_start.style);
        break;
    case YAML_MAPPING_END_EVENT:
        yrv = yaml_mapping_end_event_initialize(*to);
        break;
    default:
        break;
    }

    return (yrv) ? SHR_E_NONE : SHR_E_INTERNAL;
}

/*!
 * \brief Push event to anchor data.
 *
 * \param [in] event   YAML event.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcfg_yaml_anchor_event_push(bcmcfg_yaml_anchor_db_t *alist,
                              const yaml_event_t *event)
{
    bcmcfg_yaml_event_list_t *elist = NULL;
    bcmcfg_yaml_event_list_t *cur = NULL;
    yaml_event_t *yevent = NULL;
    uint32_t size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    size = sizeof(bcmcfg_yaml_event_list_t);
    SHR_ALLOC(elist, size, "bcmcfgYamlEventList");
    SHR_NULL_CHECK(elist, SHR_E_MEMORY);

    size = sizeof(yaml_event_t);
    SHR_ALLOC(yevent, size, "bcmcfgYamlEvent");
    SHR_NULL_CHECK(yevent, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT(
        bcmcfg_yaml_event_copy(event, &yevent));

    elist->event = yevent;
    elist->next = NULL;

    /* Insert event list to anchor list. */
    if (alist->event_list == NULL) {
        alist->event_list = elist;
    } else {
        cur = alist->event_list;
        while (cur) {
            if (cur->next == NULL) {
                cur->next = elist;
                break;
            }
            cur = cur->next;
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(elist);
        SHR_FREE(yevent);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Process start event.
 *
 * \param [in] event    YAML event.
 */
static int
bcmcfg_yaml_anchor_start_event_process(const yaml_event_t *event)
{
    bcmcfg_yaml_anchor_db_t *anc = bcmcfg_yaml_anchor_input_db;

    switch (event->type) {
    case YAML_SEQUENCE_START_EVENT:
    case YAML_MAPPING_START_EVENT:
        while (anc) {
            anc->event_count++;
            anc = anc->next;
        }
        break;
    default:
        break;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Process end event.
 *
 * \param [in] event    YAML event.
 */
static int
bcmcfg_yaml_anchor_end_event_process(const yaml_event_t *event)
{
    bcmcfg_yaml_anchor_db_t *anc = bcmcfg_yaml_anchor_input_db;

    switch (event->type) {
    case YAML_SEQUENCE_END_EVENT:
    case YAML_MAPPING_END_EVENT:
        while (anc) {
            anc->event_count--;
            anc = anc->next;
        }
        break;
    default:
        break;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Push YAML event to YAML anchor database.
 *
 * \param [in] event    YAML event.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcfg_yaml_anchor_events_push(const yaml_event_t *event)
{
    bcmcfg_yaml_anchor_db_t *anc = bcmcfg_yaml_anchor_input_db;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (event->type) {
    case YAML_SEQUENCE_START_EVENT:
    case YAML_SEQUENCE_END_EVENT:
    case YAML_MAPPING_START_EVENT:
    case YAML_MAPPING_END_EVENT:
    case YAML_SCALAR_EVENT:
        while (anc) {
            SHR_IF_ERR_EXIT(
                bcmcfg_yaml_anchor_event_push(anc, event));
            anc = anc->next;
        }
    default:
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate the anchor data.
 *
 * \param [in] yaml_anchor_name    YAML anchor name.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failures.
 */
static int
bcmcfg_yaml_anchor_db_create(char * const name)
{
    bcmcfg_yaml_anchor_db_t *alist = NULL;
    char *anchor_name = NULL;
    int len;
    uint32_t size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(name, SHR_E_INTERNAL);

    len = sal_strlen(name);
    if (len == 0) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Alloc anchor list */
    size = sizeof(bcmcfg_yaml_anchor_db_t);
    SHR_ALLOC(alist, size, "bcmcfgYamlAnchorList");
    SHR_NULL_CHECK(alist, SHR_E_MEMORY);
    sal_memset(alist, 0, size);

    /* Alloc anchor name buffer. */
    size = sizeof(char) * (len + 1);
    SHR_ALLOC(anchor_name, size, "bcmcfgYamlAnchorName");
    SHR_NULL_CHECK(anchor_name, SHR_E_MEMORY);
    sal_memcpy(anchor_name, name, len);
    anchor_name[len] = 0;

    /* Add created */
    alist->name = anchor_name;
    alist->event_count = 0;
    alist->next = bcmcfg_yaml_anchor_input_db;
    bcmcfg_yaml_anchor_input_db = alist;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(anchor_name);
        SHR_FREE(alist);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear database events.
 *
 * \param [in] anchor_db    YAML anchor database.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failures.
 */
static void
bcmcfg_yaml_anchor_db_events_clear(bcmcfg_yaml_anchor_db_t *anchor_db)
{
    bcmcfg_yaml_event_list_t  *elist = NULL;
    bcmcfg_yaml_event_list_t  *cur_elist = NULL;

    if (!anchor_db) {
        return;
    }

    /* Cleanup all events */
    elist = anchor_db->event_list;
    while(elist) {
        cur_elist = elist;
        elist = elist->next;
        yaml_event_delete(cur_elist->event);
        SHR_FREE(cur_elist->event);
        SHR_FREE(cur_elist);
    }

    anchor_db->event_list = NULL;

    return;
}

/*!
 * \brief Destroy the anchor database.
 *
 * \param [in] db     YAML anchor database.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failures.
 */
static void
bcmcfg_yaml_anchor_db_destroy(bcmcfg_yaml_anchor_db_t **db)
{
    bcmcfg_yaml_anchor_db_t *alist = *db;
    bcmcfg_yaml_anchor_db_t *current = NULL;

    while (alist) {
        current = alist;
        alist = alist->next;

        bcmcfg_yaml_anchor_db_events_clear(current);
        SHR_FREE(current->name);
        SHR_FREE(current);
    }

    *db = NULL;

    return;
}

/*******************************************************************************
 * Public functions
 */

bcmcfg_yaml_anchor_db_t*
bcmcfg_yaml_anchor_find(char * const name)
{
    bcmcfg_yaml_anchor_db_t *alist = bcmcfg_yaml_anchor_db;
    int len, alen;
    int found = 0;

    if (!name) {
        return NULL;
    }

    len = sal_strlen(name);
    while(alist) {
        /* check name */
        if (!alist->name) {
            alist = alist->next;
            continue;
        }

        alen = sal_strlen(alist->name);
        if (alen != len) {
            alist = alist->next;
            continue;
        }

        if (0 == sal_strncmp(name, alist->name, len)) {
            found = 1;
            break;
        }

        alist = alist->next;
    }

    return (found) ? alist : NULL;
}

void
bcmcfg_yaml_anchor_db_init(void)
{
    bcmcfg_yaml_anchor_db_cleanup();

    return;
}

void
bcmcfg_yaml_anchor_db_cleanup(void)
{
    bcmcfg_yaml_anchor_db_destroy(&bcmcfg_yaml_anchor_input_db);

    bcmcfg_yaml_anchor_db_destroy(&bcmcfg_yaml_anchor_db);

    return;
}

int
bcmcfg_yaml_anchor_db_update(void)
{
    bcmcfg_yaml_anchor_db_t *anc = bcmcfg_yaml_anchor_input_db;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Each YAML anchor should have different event scope.
     * When processing END event, only first anchor event count
     * can be possibly 0. More than 2 anchor scopes cannot end
     * at the same time. So checking first anchor event count
     * should be suffice.  */
    if (anc) {
        if (anc->event_count == 0) {
            /* remove node from bcmcfg_yaml_anchor_input_db. */
            bcmcfg_yaml_anchor_input_db = anc->next;

            /* add node to bcmcfg_yaml_anchor_db. */
            anc->next = bcmcfg_yaml_anchor_db;
            bcmcfg_yaml_anchor_db = anc;
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmcfg_yaml_anchor_register(const yaml_event_t *event)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!event ||
        !event->data.scalar.anchor) {
        SHR_EXIT();
    }

    switch (event->type) {
    case YAML_SCALAR_EVENT:
    case YAML_SEQUENCE_START_EVENT:
    case YAML_MAPPING_START_EVENT:
        {
            char * const name = (char * const) event->data.scalar.anchor;
            /* Check anchor already used. */
            bcmcfg_yaml_anchor_db_t *anchor_db = bcmcfg_yaml_anchor_find(name);
            if (anchor_db) {
                bcmcfg_yaml_anchor_db_events_clear(anchor_db);
            }

            /* Create Anchor DB. */
            SHR_IF_ERR_EXIT(
                bcmcfg_yaml_anchor_db_create(name));
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_yaml_anchor_event_process(const yaml_event_t *event)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        bcmcfg_yaml_anchor_start_event_process(event));

    SHR_IF_ERR_EXIT(
        bcmcfg_yaml_anchor_events_push(event));

    SHR_IF_ERR_EXIT(
        bcmcfg_yaml_anchor_end_event_process(event));

exit:
    SHR_FUNC_EXIT();
}

