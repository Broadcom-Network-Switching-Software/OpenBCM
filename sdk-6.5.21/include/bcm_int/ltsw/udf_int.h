/*! \file udf.h
 *
 * ltsw UDF module internal header file.
 * Include structure definitions and function declarations used by other modules.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMINT_LTSW_UDF_INT_H
#define BCMINT_LTSW_UDF_INT_H

#include <bcm/udf.h>
#include <sal/sal_mutex.h>
#include <bcm_int/ltsw/generated/udf_ha.h>

/*
 * \brief HA sub-component Ids of UDF
 */
#define BCMINT_UDF_OBJ_SUB_COMP_ID                      0
#define BCMINT_UDF_PKT_FMT_OBJ_SUB_COMP_ID              1

/* Generic memory allocation routine. */
#define BCMINT_UDF_MEMALLOC(_ptr_,_size_of_,_descr_)            \
       do {                                                     \
           uint32_t _size_ = _size_of_;                         \
           if (NULL == (_ptr_)) {                               \
              SHR_ALLOC(_ptr_, _size_of_, _descr_);             \
              SHR_NULL_CHECK(_ptr_, SHR_E_MEMORY);              \
           }                                                    \
           sal_memset((_ptr_), 0, (_size_));                    \
       } while (0)

#define BCMINT_UDF_MEMFREE(_ptr_)                               \
       do {                                                     \
           if (NULL != (_ptr_)) {                               \
              SHR_FREE((_ptr_));                                \
              _ptr_ = NULL;                                     \
           } else {                                             \
              LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,       \
              "UDF Error: Freeing NULL Ptr.\n\r")));            \
           }                                                    \
       } while (0)

/*
 * Typedef:
 *     bcmint_udf_control_info_t
 * Purpose:
 *     UDF control structure to manage and interact with
 *     device specific driver functions.
 */
typedef struct bcmint_udf_control_info_s {
    /*! Module specific flags */
#define BCMINT_UDF_RANGE_CHECK_SUPPORTED            (1 << 1)
#define BCMINT_UDF_FLEXHASH_SUPPORTED               (1 << 2)
#define BCMINT_UDF_POLICER_GROUP_SUPPORTED          (1 << 3)
#define BCMINT_UDF_UDFHASH_SUPPORTED                (1 << 4)
#define BCMINT_UDF_FLEXCTR_SUPPORTED                (1 << 5)
    uint8 flags;

    /*! Maximum number of chunks supported */
    uint8 max_chunks;

    /*! Minimum Object ID */
    uint16 min_obj_id;

    /*! Maximum Object ID */
    uint16 max_obj_id;

    /*! UDF ID running trackers */
    uint16 udf_id_running;

    /*! UDF packet format ID running trackers */
    uint16 udf_pkt_format_id_running;

} bcmint_udf_control_info_t;

/*!
 * \brief UDF control database.
 */
typedef struct bcmint_ltsw_udf_control_s {

    /*! UDF module lock. */
    sal_mutex_t lock;

    /*! UDF control info. */
    bcmint_udf_control_info_t info;
} bcmint_udf_control_t;

/*!
 *\brief UDF Packet format enum lt map db
 */
typedef struct bcm_udf_pkt_format_enum_lt_map_s {

    /*! Udf packet format enum */
    bcm_udf_abstract_pkt_format_t name;

    /*! Mapping lt field value */
    const char *value;
} bcm_udf_pkt_format_enum_lt_map_t;


/*
 * Initialize for UDF control structure for MAX supported units.
 */
extern bcmint_udf_control_t *ltsw_udf_control[BCM_MAX_NUM_UNITS];

#define UDF_CONTROL(_u_) ltsw_udf_control[(_u_)]

/*
 * UDF module INIT check
 */
#define BCMINT_UDF_INIT_CHECK(_u_)                                  \
    do {                                                            \
        if (UDF_CONTROL(_u_) == NULL) {                             \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);                    \
        }                                                           \
    } while(0)

/*
 * UDF Min and Max object values
 */
#define BCMINT_UDF_ID_MIN(_u_)   UDF_CONTROL(_u_)->info.min_obj_id
#define BCMINT_UDF_ID_MAX(_u_)   UDF_CONTROL(_u_)->info.max_obj_id

/* Max number of chunks supported. */
#define BCMINT_UDF_MAX_CHUNKS(_u_) UDF_CONTROL(_u_)->info.max_chunks

/* Macro to validate UDF object */
#define BCMINT_UDF_ID_VALIDATE(_u_, _id_)                           \
    do {                                                            \
        if ((_id_ < BCMINT_UDF_ID_MIN(_u_)) ||                      \
            (_id_ > BCMINT_UDF_ID_MAX(_u_))) {                      \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);                   \
        }                                                           \
    } while(0)

/* Macro to validate UDF packet format object */
#define BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(_u_, _id_)                \
    do {                                                            \
        if ((_id_ < BCMINT_UDF_ID_MIN(_u_)) ||                      \
            (_id_ > BCMINT_UDF_ID_MAX(_u_))) {                      \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);                   \
        }                                                           \
    } while(0)

/* Running UDF Object ID */
#define BCMINT_UDF_OBJ_ID_RUNNING(_u_)                              \
    (UDF_CONTROL(_u_)->info.udf_id_running +=                       \
    (UDF_CONTROL(_u_)->info.udf_id_running > BCMINT_UDF_ID_MAX(_u_) ? 0 : 1))

/* Running UDF packet format Object ID */
#define BCMINT_UDF_PKT_FORMAT_ID_RUNNING(_u_)                       \
    (UDF_CONTROL(_u_)->info.udf_pkt_format_id_running +=               \
    (UDF_CONTROL(_u_)->info.udf_pkt_format_id_running > BCMINT_UDF_ID_MAX(_u_) ? 0 : 1))

/*
 * Macro:
 *     BCMINT_UDF_LOCK
 * Purpose:
 *     Take the UDF control mutex lock.
 */
#define BCMINT_UDF_LOCK(_u_)                                        \
    {                                                               \
       sal_mutex_take(UDF_CONTROL(unit)->lock, SAL_MUTEX_FOREVER);  \
    }

/*
 * Macro:
 *     BCMINT_UDF_UNLOCK
 * Purpose:
 *     Give the UDF control mutex lock.
 */
#define BCMINT_UDF_UNLOCK(unit)                                     \
    {                                                               \
       sal_mutex_give(UDF_CONTROL(unit)->lock);                     \
    }


extern int
bcmint_udf_common_init(int unit);

extern int
bcmint_udf_common_detach(int unit);

extern int
bcmint_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id);

extern int
bcmint_udf_multi_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info);

extern int
bcmint_udf_obj_running_id_alloc(
    int unit,
    bcm_udf_id_t *udf_id);

extern int
bcmint_udf_pkt_format_obj_running_id_alloc(
    int unit,
    bcm_udf_pkt_format_id_t *pkt_format_id);

extern int
bcmint_udf_sanitize_inputs(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_id_t *udf_id);

extern int
bcmint_udf_obj_offset_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcmint_udf_offset_info_t **offset_info);

#endif /* BCMINT_LTSW_UDF_INT_H */
