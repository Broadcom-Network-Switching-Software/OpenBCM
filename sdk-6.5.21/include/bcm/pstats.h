/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_PSTATS_H__
#define __BCM_PSTATS_H__

#if defined(INCLUDE_PSTATS)

#include <bcm/types.h>

/* PSTATS time stamp structure. */
typedef struct bcm_pstats_timestamp_s {
    uint64 seconds;     /* Seconds */
    uint32 nanoseconds; /* Nanoseconds */
} bcm_pstats_timestamp_t;

/* Definition for session element type. */
typedef enum bcm_pstats_session_element_type_e {
    bcmPStatsSessionUnicastQueue = 1, 
    bcmPStatsSessionIngPool = 2, 
    bcmPStatsSessionEgrPool = 3, 
    bcmPStatsSessionTypeCount = 4       /* Max value defined for this enum */
} bcm_pstats_session_element_type_t;

/* PSTATS session element structure. */
typedef struct bcm_pstats_session_element_s {
    bcm_pstats_session_element_type_t type; /* Session element type */
    bcm_gport_t gport;                  /* UC queue gport */
    bcm_cos_queue_t cosq;               /* Cos queue */
    bcm_service_pool_id_t pool;         /* Service Pool ID */
} bcm_pstats_session_element_t;

#define BCM_PSTATS_BUFFER_COUNT 8          /* PSTATS buffer count */

/* PSTATS use counts sturcture. */
typedef struct bcm_pstats_data_s {
    uint32 array_count;                 /* Number of elements in array */
    uint32 use_counts_array[BCM_PSTATS_BUFFER_COUNT]; /* PSTATS use counts array */
} bcm_pstats_data_t;

#define BCM_PSTATS_OPTIONS_WITH_ID  0x00000001 /* With ID option creation */
#define BCM_PSTATS_OPTIONS_REPLACE  0x00000002 /* Replace existing entry */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a Pstats session instance for SDK management. */
extern int bcm_pstats_session_create(
    int unit, 
    int options, 
    int array_count, 
    bcm_pstats_session_element_t *element_array, 
    bcm_pstats_session_id_t *session_id);

/* Destroy an existing Pstats session. */
extern int bcm_pstats_session_destroy(
    int unit, 
    bcm_pstats_session_id_t session_id);

/* Get an existing Pstats session elements. */
extern int bcm_pstats_session_get(
    int unit, 
    bcm_pstats_session_id_t session_id, 
    int array_max, 
    bcm_pstats_session_element_t *element_array, 
    int *array_count);

/* Start a Pstats sync operation. */
extern int bcm_pstats_data_sync(
    int unit);

/* Get an existing Pstats session data. */
extern int bcm_pstats_session_data_get(
    int unit, 
    bcm_pstats_session_id_t session_id, 
    bcm_pstats_timestamp_t *timestamp, 
    int array_max, 
    bcm_pstats_data_t *data_array, 
    int *array_count);

/* 
 * Start a session sync operation first. Then get an existing Pstats
 * session data.
 */
extern int bcm_pstats_session_data_sync_get(
    int unit, 
    bcm_pstats_session_id_t session_id, 
    bcm_pstats_timestamp_t *timestamp, 
    int array_max, 
    bcm_pstats_data_t *data_array, 
    int *array_count);

/* 
 * Clear an existing Pstats session hw use counts and sw use counts
 * buffer.
 */
extern int bcm_pstats_session_data_clear(
    int unit, 
    bcm_pstats_session_id_t session_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_pstats_session_traverse_cb */
typedef int (*bcm_pstats_session_traverse_cb)(
    int unit, 
    bcm_pstats_session_id_t session_id, 
    int array_count, 
    bcm_pstats_session_element_t *element_array, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse through the pstats sessions and run callback at each session. */
extern int bcm_pstats_session_traverse(
    int unit, 
    bcm_pstats_session_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a pstats timestamp structure. */
extern void bcm_pstats_timestamp_t_init(
    bcm_pstats_timestamp_t *timestamp);

/* Initialize a pstats session element structure. */
extern void bcm_pstats_session_element_t_init(
    bcm_pstats_session_element_t *element);

/* Initialize a pstats data structure. */
extern void bcm_pstats_data_t_init(
    bcm_pstats_data_t *data);

#endif /* defined(INCLUDE_PSTATS) */

#endif /* __BCM_PSTATS_H__ */
