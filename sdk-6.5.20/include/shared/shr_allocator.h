/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        allocator.h
 * Purpose:     Internal routines to the BCM library for allocating
 *              gu2 resources.
 */

#ifndef _SHR_ALLOCATOR_H_
#define _SHR_ALLOCATOR_H_

#include <sal/types.h>
#include <sal/core/alloc.h>
#include <shared/idxres_afl.h>
#include <shared/idxres_fl.h>
#include <shared/hash_tbl.h>


#define  _SHR_RES_FLAGS_CONTIGOUS   0x1
#define  _SHR_RES_FLAGS_RESERVE     0x2

#define SHR_ALLOC_STYLE_NONE           0
#define SHR_ALLOC_STYLE_SINGLE         1
#define SHR_ALLOC_STYLE_SCALED         2
#define SHR_ALLOC_STYLE_VERSATILE      3

#define SHR_DYNAMIC_BASE_ADDR       ~0
#define SHR_DYNAMIC_MAX_COUNT       ~0
#define SHR_DYNAMIC_SIZE            ~0

/*
 * Resource types visible to the user
 */
typedef enum {
    SHR_USR_RES_RES0,
    SHR_USR_RES_RES1,
    SHR_USR_RES_RES2,
    SHR_USR_RES_RES3,
    SHR_USR_RES_RES4,
    SHR_USR_RES_RES5,
    SHR_USR_RES_RES6,
    SHR_USR_RES_RES7,
    SHR_USR_RES_RES8,
    SHR_USR_RES_RES9,
    SHR_USR_RES_RES10,
    SHR_USR_RES_RES11,
    SHR_USR_RES_RES12,
    SHR_USR_RES_RES13,
    SHR_USR_RES_RES14,
    SHR_USR_RES_RES15,
    SHR_USR_RES_RES16,
    SHR_USR_RES_RES17,
    SHR_USR_RES_RES18,
    SHR_USR_RES_RES19,
    SHR_USR_RES_RES20,
    SHR_USR_RES_MAX
} _shr_usr_res_types_t;

typedef enum {
    SHR_HW_RES_RES0,
    SHR_HW_RES_RES1,
    SHR_HW_RES_RES2,
    SHR_HW_RES_RES3,
    SHR_HW_RES_RES4,
    SHR_HW_RES_RES5,
    SHR_HW_RES_RES6,
    SHR_HW_RES_RES7,
    SHR_HW_RES_RES8,
    SHR_HW_RES_RES9,
    SHR_HW_RES_RES10,
    SHR_HW_RES_RES11,
    SHR_HW_RES_RES12,
    SHR_HW_RES_RES13,
    SHR_HW_RES_RES14,
    SHR_HW_RES_MAX
} _shr_hw_res_t;

typedef union shr_allocator_idxres_list_handle_u {
    shr_aidxres_list_handle_t   handle_aidx;
    shr_idxres_list_handle_t    handle_idx;
} shr_allocator_idxres_list_handle_t;

typedef struct {
    uint32                          alloc_style;
    shr_allocator_idxres_list_handle_t        u;
} _shr_res_handle_t;
#define aidx_handle u.handle_aidx
#define idx_handle  u.handle_idx



typedef struct {
    uint32                   mask;
    char                     *name;
} _shr_hw_table_attrs_t;

/* convert from external representation to internal */
typedef uint32 (*_shr_ext_to_intl_f)(int unit, uint32 ext);
/* convert from internal representation to external */
typedef uint32 (*_shr_intl_to_ext_f)(int unit, uint32 intl);


typedef struct {
    uint32                    alloc_style;
    uint32                    base;
    uint32                    max_count;
    uint32                    blocking_factor; /* max possible block in aidx */
    uint32                    scaling_factor;
    char                      *name;
    uint32                    tables;
    uint32                    reservedHigh;
    uint32                    reservedLow;
    _shr_ext_to_intl_f    e2i;
    _shr_intl_to_ext_f    i2e;
} _shr_hw_res_attrs_t;



/*
 *   Function
 *      _shr_resource_init
 *   Purpose
 *      Initialize the shr resource manager for all HW resources
 *      for the unit
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE all resources are successfully initialized
 *       BCM_E_* as appropriate otherwise
 *   Notes
 *       Returns error is any of the resources cannot be initialized
 */
extern int
_shr_resource_init(int unit);


/*
 *   Function
 *      _shr_resource_alloc
 *   Purpose
 *      Allocate "count" number of resources.
 *   Parameters
 *      unit    - (IN)  unit number of the device
 *      type    - (IN)  resource type
 *                      (one of _shr_usr_res_types_t)
 *      count   - (IN)  Number of resources required
 *      elements- (OUT) Resource Index return by the underlying allocator
 *      flags   - (IN)  _SHR_RES_FLAGS_CONTIGOUS
 *                      if the resources need to be contigous
 *   Returns
 *      BCM_E_NONE if element allocated successfully
 *      BCM_E_* as appropriate otherwise
 */
extern int
_shr_resource_alloc(int                        unit,
                        _shr_usr_res_types_t   type,
                        uint32                     count,
                        uint32                    *elements,
                        uint32                     flags);

/*
 *   Function
 *      _shr_resource_free
 *   Purpose
 *      Free the resources specified in the array
 *   Parameters
 *      unit    - (IN)  unit number of the device
 *      type    - (IN)  resource type
 *                      (one of _shr_usr_res_types_t)
 *      count   - (IN)  Number of resources to be freed
 *      elements- (IN)  Resource Ids to be freed
 *      flags   - (IN)  _SHR_RES_FLAGS_CONTIGOUS
 *   Returns
 *      BCM_E_NONE if element freed successfully
 *      BCM_E_* as appropriate otherwise
 *   NOTE:
 *      In case of block allocations, the user must pass the
 *      array back. Just the base element will not do.
 *      i.e. the array must be 'count' elements long
 */
extern int
_shr_resource_free(int                        unit,
                       _shr_usr_res_types_t   type,
                       uint32                     count,
                       uint32                    *elements,
                       uint32                     flags);
/*
 *   Function
 *      _shr_resource_test
 *   Purpose
 *      Check to see if the specified resource is available
 *   Parameters
 *      unit    - (IN)  unit number of the device
 *      type    - (IN)  resource type
 *                      (one of _shr_usr_res_types_t)
 *      element - (IN)  Resource Id to be checked
 *   Returns
 *      BCM_E_NOT_FOUND if resource is available
 *      BCM_E_EXIST     if resource is in use
 *      BCM_E_*         as appropriate otherwise
 */
extern int
_shr_resource_test(int                        unit,
                       _shr_usr_res_types_t   type,
                       uint32                     element);
/*
 *   Function
 *      _shr_shr_resource_uninit
 *   Purpose
 *      Destroy the all the resource managers for this unit
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE all resources are successfully destroyed
 *       BCM_E_* as appropriate otherwise
 *   Notes
 *       Returns error is any of the resources cannot be destroyed
 */
extern int
_shr_resource_uninit(int unit);


/*
 *   Function
 *      _shr_alloc_range_get
 *   Purpose
 *     Retrieve the range of valid IDs for the given resource
 *
 *   Parameters
 *      (IN)  unit   : unit number of the device
 *      (OUT) first  : first valid ID
 *      (OUT) last   : last valid ID
 *   Returns
 *       BCM_E_NONE - All required resources are allocated
 *       BCM_E_*    - failure
 *   Notes
 */
extern int 
_shr_alloc_range_get(int unit, _shr_usr_res_types_t type,
                     uint32 *first, uint32 *last);

/*
 *   Function
 *      _shr_alloc_range_get
 *   Purpose
 *     Retrieve the range of valid IDs for the given resource
 *
 *   Parameters
 *      (IN)  unit   : unit number of the device
 *      (OUT) first  : first valid ID
 *      (OUT) last   : last valid ID
 *   Returns
 *       BCM_E_NONE - All required resources are allocated
 *       BCM_E_*    - failure
 *   Notes
 */
extern int 
_shr_range_reserve(int unit, _shr_usr_res_types_t type,
                       int highOrLow, uint32 val);

#endif /* _SHR_ALLOCATOR_H_ */
