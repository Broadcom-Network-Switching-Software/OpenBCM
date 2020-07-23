/*! \file bcmcfg_yaml_anchor.h
 *
 * BCMCFG YAML anchor, reference interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_YAML_ANCHOR_H
#define BCMCFG_YAML_ANCHOR_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmcfg/bcmcfg_types.h>

#include <yaml.h>

/*!
 * \brief The list of event types which should be collected.
 *
 * When referencing an anchor database, all events with following event
 * types should be collected, but others will not be collected.
 */
#define IS_ANCHOR_REF_COLLECTED_EVENT_TYPE(type) \
    ((type) == YAML_SCALAR_EVENT ||          \
     (type) == YAML_MAPPING_START_EVENT ||   \
     (type) == YAML_MAPPING_END_EVENT   ||   \
     (type) == YAML_MAPPING_END_EVENT   ||   \
     (type) == YAML_SEQUENCE_START_EVENT ||  \
     (type) == YAML_SEQUENCE_END_EVENT)

/*!
 * \brief Linked list of YAML events.
 *
 * This structure is used to record all
 * yaml events for anchor, reference usage.
 */
typedef struct bcmcfg_yaml_event_list_s {
    /*! YAML event. */
    yaml_event_t *event;

    /*! Next pointer. */
    struct bcmcfg_yaml_event_list_s *next;
} bcmcfg_yaml_event_list_t;

/*!
 * \brief Linked list of YAML anchor data.
 */
typedef struct bcmcfg_yaml_anchor_db_s {
    /*! Anchor name. */
    char *name;

    /*! Event count. */
    int event_count;

    /*! YAML event list. */
    bcmcfg_yaml_event_list_t *event_list;

    /*! Next pointer. */
    struct bcmcfg_yaml_anchor_db_s *next;
} bcmcfg_yaml_anchor_db_t;

/*!
 * \brief Find anchor list by anchor name in bcmcfg_yaml_anchor_db.
 *
 * \param [in] name    YAML anchor name.
 *
 * \retval Pointer to the anchor list if found.
 */
extern bcmcfg_yaml_anchor_db_t*
bcmcfg_yaml_anchor_find(char * const name);

/*!
 * \brief Initialize anchor database.
 *
 * \retval SHR_E_NONE  No errors.
 */
extern void
bcmcfg_yaml_anchor_db_init(void);

/*!
 * \brief Cleanup anchor database.
 *
 * \retval SHR_E_NONE  No errors.
 */
extern void
bcmcfg_yaml_anchor_db_cleanup(void);

/*!
 * \brief Update anchor database.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failures.
 */
extern int
bcmcfg_yaml_anchor_db_update(void);

/*!
 * \brief Register anchor data.
 *
 * \param [in] event    YAML event.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failures.
 */
extern int
bcmcfg_yaml_anchor_register(const yaml_event_t *event);

/*!
 * \brief Processing event for anchor data.
 *
 * \param [in] event    YAML event.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failures.
 */
extern int
bcmcfg_yaml_anchor_event_process(const yaml_event_t *event);

#endif /* BCMCFG_YAML_ANCHOR_H */
