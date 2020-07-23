/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BCM_INT_UDF_COMMON_H__
#define __BCM_INT_UDF_COMMON_H__

#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <bcm/module.h>
#include <bcm_int/esw/switch.h>

/*
 * Macro:
 *     _BCM_UDF_LOCK
 * Purpose:
 *     Take the UDF control mutex lock.
 */
#define _BCM_UDF_LOCK(unit) \
    sal_mutex_take(SOC_CONTROL(unit)->udf_lock, sal_mutex_FOREVER)

/*
 * Macro:
 *     _BCM_UDF_UNLOCK
 * Purpose:
 *     Give the UDF control mutex lock.
 */
#define _BCM_UDF_UNLOCK(unit) \
    sal_mutex_give(SOC_CONTROL(unit)->udf_lock);


/* Generic memory allocation routine. */
#define _BCM_UDF_ALLOC(_ptr_,_size_of_,_descr_)               \
       do {                                                   \
           if (NULL == (_ptr_)) {                             \
              (_ptr_) = sal_alloc((_size_of_), (_descr_));    \
           }                                                  \
           if((_ptr_) != NULL) {                              \
               uint32 _size_ = _size_of_;                     \
               sal_memset((_ptr_), 0, (_size_));              \
               LOG_DEBUG(BSL_LS_BCM_UDF, (BSL_META_U(unit,    \
               "UDF Mem Alloc: Addr:%p bytes:%u '%s'\n\r"),   \
                                  _ptr_, _size_, _descr_));   \
           }  else {                                          \
               LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,    \
               "UDF Mem Alloc failure: %s\n\r"), (_descr_))); \
           }                                                  \
       } while (0)

#define _BCM_UDF_FREE(_ptr_)                                 \
       do {                                                  \
           if (NULL != (_ptr_)) {                            \
              LOG_DEBUG(BSL_LS_BCM_UDF, (BSL_META_U(unit,    \
              "UDF Mem Free: Addr:%p\n\r"), _ptr_));         \
              sal_free((_ptr_));                             \
              _ptr_ = NULL;                                  \
           } else {                                          \
              LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,    \
              "UDF Error: Freeing NULL Ptr.\n\r")));         \
           }                                                 \
       } while (0)



/********************** Linked-List Macros ***********************/
/* Double Linked-list node add */
#define _UDF_DLL_NODE_ADD(_u_, _new_, _head_)           \
    do {                                                \
          if (_new_ != NULL) {                          \
             /* Add new node at the tail of the head */ \
             if (NULL == _head_) {                      \
                /* Adding first node */                 \
                _head_ = _new_;                         \
             } else {                                   \
                _head_->next = _new_;                   \
                (_new_)->prev = _head_;                 \
                _head_ = _new_;                         \
             }                                          \
          }                                             \
    } while (0)

/* Double Linked-list node delete */
#define _UDF_DLL_NODE_DEL(_u_, _del_, _head_, _temp_)        \
    do {                                                     \
          _temp_ = _head_;                                   \
          if ((_del_ != NULL) && (_head_ != NULL)) {         \
             while(_temp_ != NULL) {                         \
                if (_del_ == _temp_) {                       \
                   if ((void *)_temp_->prev != NULL) {       \
                      _temp_->prev->next = _del_->next;      \
                   }                                         \
                   if ((void *)_temp_->next != NULL) {       \
                      _temp_->next->prev = _del_->prev;      \
                   }                                         \
                   if (_head_ == _temp_) {                   \
                      _head_ = _temp_->prev;                 \
                   }                                         \
                   break;                                    \
                }                                            \
                _temp_ = _temp_->prev;                       \
             }                                               \
          }                                                  \
    } while (0)

/* Double Linked-list node Get */
#define _UDF_DLL_NODE_GET(_u_, _id_, _head_, _temp_, _node_) \
     do {                                                \
         _node_ = NULL;                                  \
         (_temp_) = _head_;                              \
         while ((_temp_) != NULL) {                      \
            if (_temp_->udf_id == _id_) {                \
               _node_ = _temp_;                          \
               break;                                    \
            }                                            \
            _temp_ = _temp_->prev;                       \
         }                                               \
     } while(0)
/*****************************************************************/

/* Offset Chunk Granularity */
#define _BCM_UDF_OFFSET_GRAN2 2

/* Max number of UDF Chunks Supported */
#define _BCM_UDF_OFFSET_CHUNKS16 16


/*
 * Typedef:
 *     _bcm_udf_funct_t
 * Purpose:
 *     Function pointers to device specific Field functions
 */
typedef struct _bcm_udf_funct_s {
    int(*udf_init)(int);                             /* Initialization function */
    int(*udf_detach)(int);                           /* detach function */
    int(*udf_chunk_create)(                          /* UDF Chunk create */
                           int,
                           bcm_udf_alloc_hints_t *,
                           bcm_udf_chunk_info_t *,
                           bcm_udf_id_t *);
    int(*udf_destroy)(int, bcm_udf_id_t);            /* UDF destroy */
    int(*udf_chunk_info_get)(                        /* UDF Chunk get */
                           int,
                           bcm_udf_id_t,
                           bcm_udf_chunk_info_t *);
    int(*udf_flow_based_chunk_arrange_set) (
                           int unit,
                           bcm_udf_tunnel_term_flow_type_t flow_type,
                           uint32 chunk_bmap);
    int(*udf_flow_based_chunk_arrange_get) (
                           int unit,
                           bcm_udf_tunnel_term_flow_type_t flow_type,
                           uint32 *chunk_bmap);
    int(*udf_abstr_pkt_format_obj_list_get)(         /* UDF Pkt Format Object Get */
                           int,
                           bcm_udf_abstract_pkt_format_t,
                           int,
                           bcm_udf_id_t *,
                           int *);
    int(*udf_abstr_pkt_format_info_get)(             /* UDF Pkt Format Info Get */
                           int,
                           bcm_udf_abstract_pkt_format_t,
                           bcm_udf_abstract_pkt_format_info_t *);
    int(*udf_abstr_pkt_fmt_list_get)(               /* UDF Pkt Fmt list get */
                        int unit,
                        int max,
                        bcm_udf_abstract_pkt_format_t *pkt_fmt_list,
                        int *actual);
    int(*udf_wb_sync)(int);                          /* UDF WB Sync */
    int(*udf_range_checker_chunk_info_get)(          /* UDF Range checker chunk info get */
                           int,
                           uint8 *,
                           uint32 *);
    void(*udf_sw_dump)(int);                          /* UDF SW Dump */
    void(*udf_sw_mem_dump)(int unit, soc_mem_t, int);      /* UDF SW Memory Dump */
} _bcm_udf_funct_t;

/*
 * Typedef:
 *     _bcm_udf_control_t
 * Purpose:
 *     UDF control structure to manage and interact with
 *     device specific driver functions.
 */
typedef struct _bcm_udf_control_s {
    uint8              flags;           /* Module specific flags */
    uint8              gran;            /* UDF granularity */
    uint8              num_chunks;      /* Max number of chunks supported. */
    uint16             max_udfs;        /* Maximum number of UDF objects - For WB purpose. */
    uint16             min_obj_id;      /* Minimum Object ID */
    uint16             max_obj_id;      /* Maximum Object ID */
    uint16             parse_limit;     /* Maximum parse offset limit in bytes */
    uint16             udf_id_running;  /* UDF ID running trackers */
    _bcm_udf_funct_t   functions;       /* Device specific functions. */
} _bcm_udf_control_t;


/*
 * Initialize for UDF control structure for MAX supported units.
 */
extern _bcm_udf_control_t *_udf_control[BCM_MAX_NUM_UNITS];

#define UDF_CONTROL(_u_) _udf_control[(_u_)]

#define _BCM_UDF_INIT_CHECK(_u_) \
    do {                                       \
        if (UDF_CONTROL(_u_) == NULL) {        \
            return BCM_E_INIT;                 \
        }                                      \
    } while(0)

#define _BCM_UDF_OFFSET_GRAN(_u_)  UDF_CONTROL(_u_)->gran

#define _BCM_UDF_ID_MIN(_u_)   UDF_CONTROL(_u_)->min_obj_id
#define _BCM_UDF_ID_MAX(_u_)   UDF_CONTROL(_u_)->max_obj_id

#define _BCM_UDF_ID_VALIDATE(_u_, _id_)               \
    do {                                              \
        if ((_id_ < _BCM_UDF_ID_MIN(_u_)) ||          \
            (_id_ > _BCM_UDF_ID_MAX(_u_))) {          \
            return BCM_E_PARAM;                       \
        }                                             \
    } while(0)

#define _BCM_UDF_OBJ_ID_RUNNING(_u_)                  \
    (UDF_CONTROL(_u_)->udf_id_running +=              \
    (UDF_CONTROL(_u_)->udf_id_running > _BCM_UDF_ID_MAX(_u_) ? 0 : 1))

#define BCM_IF_NULL_RETURN_PARAM(_p_) \
    do {                                                    \
        if ((_p_) == NULL) {                                \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)

#define BCM_IF_ERROR_GOTO_CLEANUP(_rv_) \
    do {                                                    \
        if (BCM_FAILURE(_rv_)) {                            \
            goto cleanup;                                   \
        }                                                   \
    } while (0)


/* Granularity supported. */
#define _BCM_UDF_CTRL_OFFSET_GRAN(_u_)  UDF_CONTROL(_u_)->gran

/* Max number of chunks supported. */
#define _BCM_UDF_CTRL_MAX_UDF_CHUNKS(_u_)  UDF_CONTROL(_u_)->num_chunks

extern int
_bcm_udf_common_init(int unit);
extern int
_bcm_udf_common_detach(int unit);
#endif /* __BCM_INT_UDF_COMMON_H__ */
