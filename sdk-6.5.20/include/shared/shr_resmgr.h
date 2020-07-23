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

#ifndef _SHR_RESMGR_H_
#define _SHR_RESMGR_H_

#include <sal/types.h>
#include <shared/idxres_afl.h>
#include <shared/idxres_fl.h>
#include <shared/idxres_mdb.h>
#include <shared/shr_res_bitmap.h>
#include <shared/shr_res_tag_bitmap.h>
#include <bcm/types.h>

/*
 *  WARNING: No locking is performed here (unless it is implied by the
 *  underlying allocator) for any of the shr_mres_* calls.  The caller must
 *  ensure that it is not possible for calls using a specific resource instance
 *  to occur in parallel or timesliced with other calls against the same
 *  specific resource instance.
 *
 *  WARNING: No locking is performed here (unless it is implied by the
 *  underlying allocator) for any of the shr_res_* calls, except for the case
 *  of shr_res_init and shr_res_detach.  For other shr_res_* calls, the caller
 *  must ensure that it is not possible for calls against a specific unit to
 *  occur in parallel or timesliced with other calls against the same unit.
 *
 *  NOTE: The actual mutual exclusion requirement is at the pool granularity,
 *  but since it is possible to have different types share the same pool, and
 *  the caller is not required to know the exact mapping, it can be dangerous
 *  for the caller to assume locking by type is sufficient.  Since allocation
 *  is managed by type, which is mapped internally to pool, the general
 *  assumption of complete exclusivity is safer.
 *
 *  Other than the *_dump calls, the shr_mres_* calls and the shr_res_* calls
 *  are designed to function in parallel or timesliced as long as there are no
 *  multiple concurrent invocations against the same unit or handle.  The
 *  *_dump calls use bsl_print, and suffer from any threading or timeslicing
 *  issues that might imply about concurrent dumps.
 */

/*
 *  Flags supported by the allocation system for each allocation.  If any of
 *  these are provided as group flags, they are automatically applied to the
 *  entire group, whether the individual flags include them or not.
 *
 *  SHR_RES_ALLOC_WITH_ID indicates that the element ID is provided by the
 *  caller, and that specific element (or the specified block starting at that
 *  element) is being requested.  The allocation must fail if the requested
 *  element/block is invalid or already in use.
 *
 *  SHR_RES_ALLOC_ALIGN_ZERO indicates that the _align_ allocators should align
 *  the block of elements with respect to zero, and not relative to the
 *  lowest element in the underlying pool.
 *
 *  SHR_RES_ALLOC_REPLACE is used with SHR_RES_ALLOC_WITH_ID to indicate that
 *  the intent is to replace an existing block.  Note the blocks must be of the
 *  same size (and tag if applicable) -- this does not 'realloc' a block; it
 *  only verifies that the block already exists as per the alloc request.
 */
#define SHR_RES_ALLOC_SINGLE_FLAGS 0x00000007
#define SHR_RES_ALLOC_WITH_ID      0x00000001
#define SHR_RES_ALLOC_ALIGN_ZERO   0x00000002
#define SHR_RES_ALLOC_REPLACE      0x00000004

/*
 *  Flags supported by the allocation system for group allocation.  Note these
 *  are specifically for the group call itself, not the individual blocks
 *  within the group call.
 *
 *  SHR_RES_ALLOC_GROUP_ATOMIC indicates that the allocation is to be
 *  considered atomic -- if there is any failure, back out all of it rather
 *  than stopping at the failure point.  This can only be applied to the
 *  allocation call.  There is no support to undo frees and aborting a check
 *  based upon the result being other than BCM_E_NONE makes no sense because
 *  check should always return some other result code.
 */
#define SHR_RES_ALLOC_GROUP_FLAGS  0x00010000
#define SHR_RES_ALLOC_GROUP_ATOMIC 0x00010000

/*
 *  For optimisation, there is a version of free that returns whether the last
 *  element of a type or pool has been freed by that free.
 *
 *  SHR_RES_FREED_TYPE_LAST_ELEM indicates that the last in-use element of a
 *  type was freed by the free call (or if the free call failed, that there
 *  were no elements of that type in use).
 *
 *  SHR_RES_FREED_POOL_LAST_ELEM indicates that the last in-use element of a
 *  pool was freed by the free call (or if the free call failed, that there
 *  were no elements of that pool in use).
 */
#define SHR_RES_FREED_TYPE_LAST_ELEM 0x00000001
#define SHR_RES_FREED_POOL_LAST_ELEM 0x00000002

/*
 *  This enum defines the supported allocation mechanisms.  Each one has some
 *  of its own advantages and disadvantages.  See the notes for each one if you
 *  need details to choose which might be best for a particular resource pool.
 */
typedef enum shr_res_allocator_e {
    SHR_RES_ALLOCATOR_BITMAP = 0,    /* bitmap based allocator */
    SHR_RES_ALLOCATOR_TAGGED_BITMAP, /* bitmap based allocator with tags */
    SHR_RES_ALLOCATOR_IDXRES,        /* indexed resource, freelist */
    SHR_RES_ALLOCATOR_AIDXRES,       /* indexed resource, aligned freelist */
    SHR_RES_ALLOCATOR_MDB,           /* indexed resource, mdb */
    SHR_RES_ALLOCATOR_COUNT /* last one indicates how many, not valid item */
} shr_res_allocator_t;

/*
 *  This struct contains the extra arguments needed to configure a resource
 *  pool to be managed by the idxres manager.
 *
 *  The scaling factor is the number of elements of the resource that make up
 *  the smallest usable grain of the resources.  Normally it would be 1.
 */
typedef struct shr_res_idxres_extras_s {
    int scaling_factor;         /* scaling factor for the resource */
} shr_res_idxres_extras_t;

/*
 *  This struct contains the extra arguments needed to configure a resource
 *  pool to be managed by the aidxres manager.
 */
typedef struct shr_res_aidxres_extras_s {
    int blocking_factor;        /* power of two for largest block to support */
} shr_res_aidxres_extras_t;

/*
 *  This struct contains the extra arguments needed to configure a resource
 *  pool to be managed by the mdb manager.  Note that many features of the mdb
 *  manager are not exposed through this abstraction.
 *
 *  Note that all of the rules for mdb must also be followed, inclduing the
 *  free lists include an implied free list of block size 1, and all of the
 *  other free lists must be of increasing block size.  User lists are not
 *  exposed through this abstraction (and so will not be allocated), and the
 *  assumption is non-locking so the caller must provide access exclusion.
 */
typedef struct shr_res_mdb_extras_s {
    shr_mdb_elem_bank_index_t bank_size;       /* elements in a single bank */
    shr_mdb_elem_bank_index_t free_lists;      /* additional free lists */
    shr_mdb_elem_bank_index_t free_counts[15]; /* free elem per blk per list */
} shr_res_mdb_extras_t;

/*
 *  This struct contains the extra arguments needed to configure a resource
 *  pool to be managed by the tagged_bitmap allocator.
 */
typedef struct shr_res_tagged_bitmap_extras_s {
    int tag_length;             /* tag length in bytes */
    int grain_size;             /* number of elements per grain */
} shr_res_tagged_bitmap_extras_t;

/*
 *  When fetching additional data about a pool (not available with 'pool_get'),
 *  the caller provides one of this structure to be filled in.
 */
typedef struct shr_res_pool_info_s {
    int used;
    int free;
} shr_res_pool_info_t;

/*
 *  When fetching additional data about a pool (not available with 'pool_get'),
 *  the caller provides one of this structure to be filled in.
 *
 *  Since more than one type can be using a single pool, and each type can use
 *  a different number of elements of the underlying pool per each of its own
 *  elements, a value for 'free' would require some possibly considerable
 *  effort to figure out how many nonoverlapping requests could be made for the
 *  number of pool elements that one type element represents.  It is simpler
 *  and reasonably consistent to not include it.
 */
typedef struct shr_res_type_info_s {
    int used;
} shr_res_type_info_t;

/*
 *  This describes how a single resource maps to an underlying pool.
 *
 *  resPoolId is the ID of the resource pool on this unit from which this
 *  resource is drawn.
 *
 *  resElemSize is the number of elements in the specified resource pool that
 *  must be taken to make a single element of this resource.  Basically, any
 *  alloc/free of this resource will multiply the number of elements by this
 *  value to determine how many to alloc/free of the underlying pool.
 *
 *  name is a string that names this resource.  It is used only for diagnostic
 *  purposes.  Internally, the provided name will be copied to the same cell as
 *  the structure, so the name array is really variable length.
 */
typedef struct _shr_res_type_desc_s {
    int resPoolId;              /* unit specific resource pool ID */
    int resElemSize;            /* how many elems of this pool per this res */
    int refCount;               /* number of elements allocated currently */
    char name[1];               /* descriptive name (for diagnostics) */
} _shr_res_type_desc_t;

/*
 *  This describes a single resource pool on a unit.
 *
 *  resManagerType is the ID of the reousrce manager that will be used to
 *  manage this resource pool on this unit.
 *
 *  low is the minimum valid element of this resource.
 *
 *  count is the number of valid elements of this resource.
 *
 *  extras points to a struct (which will be appended to this during setup of
 *  this pool) that provides additional arguments to how the pool needs to be
 *  managed.  This is provided because some of the supported allocation
 *  managers require more information than just the range of valid IDs.
 *
 *  name is a string that names this resource.  It is used only for diagnostic
 *  purposes.  Internally, the provided name will be copied to the same cell as
 *  the structure, so the name array is really variable length.
 *
 *  Note that the extras struct will be appended to the same memory cell as
 *  this struct, after the name, and so the pointer will not need to be freed
 *  when this is destroyed.
 */
typedef struct _shr_res_pool_desc_s {
    shr_res_allocator_t resManagerType; /* which resoource manager to use */
    int low;                            /* minimum available element */
    int count;                          /* number of available elements */
    int refCount;                       /* number of types using this pool */
    int inuse;                          /* number of active elems this pool */
    void *resHandle;                    /* handle for this resource */
    void *extras;                       /* additional config per resmgr type */
    char name[1];                       /* descriptive name for diagnostics */
} _shr_res_pool_desc_t;


/*
 *  This structure describes attributes about the unit, and includes lists
 *  that are applicable to the unit.  Note that while resource IDs and resource
 *  types are unit-specific, the resource managers themselves are globally
 *  available to all units.
 *
 *  resTypeCount indicates how many different resources on this unit are being
 *  managed through this system.  It is possible for more than one resource to
 *  use the same resource pool, but it is not possible for one resource to use
 *  more than one resource pool.
 *
 *  resPoolCount indicates how many different resource pools are being managed
 *  on this unit.
 *
 *  res points to an array of resource descriptors,  Each descriptor will map a
 *  single resource on the unit to its underlying resource pool.
 *
 *  pool points to an array of resource pool descriptors.  Each of these will
 *  describe a single resource pool on the unit and map to the manager that
 *  will be used for that particular pool.
 */
typedef struct _shr_res_unit_desc_s {
    uint16 resTypeCount;          /* maximum presented resource ID */
    uint16 resPoolCount;          /* maximum resource pool ID */
    _shr_res_type_desc_t **res;   /* array of type -> pool map pointers */
    _shr_res_pool_desc_t **pool;  /* array of pool description pointers */
} _shr_res_unit_desc_t;

/* A handle for a non-unit-based resource manager instance */
typedef _shr_res_unit_desc_t *shr_mres_handle_t;

extern _shr_res_unit_desc_t *_g_unitResDesc[BCM_LOCAL_UNITS_MAX];

/******************************************************************************
 *
 *  Handle based resource management (private/common based upon handle usage)
 *
 *  These functions are intended to use when a specific module needs its own
 *  private resources, or if a module may need to share resources across units
 *  (this is accomplished by sharing the handle).
 */

/*
 *   Function
 *      shr_mres_create
 *   Purpose
 *      Create a resource manager instance and provide a handle for it
 *   Parameters
 *      (OUT) handle       : where to put the instance handle
 *      (IN) num_res_types : number of resource types for the unit
 *      (IN) num_res_pools : number of resource pools for the unit
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Resource types map to resource pools, from which the actual elements of
 *      the resources are drawn, and more than one type can pull elements from
 *      the same pool, so there should always be at least as many types as
 *      there are pools.
 *
 *      The instance will be limited to the number of types and pools specified
 *      here, as the descriptor space will be allocated and cleared by this
 *      function.  See below for how to configure types and pools.
 */
extern int
shr_mres_create(shr_mres_handle_t *handle,
                int num_res_types,
                int num_res_pools);

/*
 *   Function
 *      shr_mres_get
 *   Purpose
 *      Get number of resource pools and types configured for a handle
 *   Parameters
 *      (IN) handle         : handle for the instance to query
 *      (OUT) num_res_types : where to put number of resource types for unit
 *      (OUT) num_res_pools : where to put number of resource pools for unit
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_mres_get(shr_mres_handle_t handle,
             int *num_res_types,
             int *num_res_pools);

/*
 *   Function
 *      shr_mres_pool_set
 *   Purpose
 *      Configure a resource pool for the handle
 *   Parameters
 *      (IN) handle  : handle for the instance to access
 *      (IN) pool_id : which pool to configure (0..max_res_pools-1)
 *      (IN) manager : which manager to use for this pool
 *      (IN) low_id  : lowest valid resource element ID in this pool
 *      (IN) count   : number of valid resource elements in this pool
 *      (IN) extras  : pointer to extra information for the manager type
 *      (IN) name    : pointer to string naming the pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      A resource pool must be configured before any resources can point to
 *      it, and can not be changed after resources point to it.
 *
 *      The underlying resource management information will be created during
 *      this call.  If this call is made again for the same resource pool, and
 *      there are no resources using this pool, the old information will be
 *      destroyed and the new will be created in its stead, but if resources
 *      are already using this pool, the call will fail.
 *
 *      Most of the allocation managers require some extra arguments, so the
 *      extras argument is likely obligatory.  Each allocation manager has its
 *      own addiitonal arguments, for specific behavioural control or
 *      optimisation, and the correct extras type must be used accordingly.
 *
 *      This must be called after create, and before a pool can be used, even
 *      before a resource can be assigned to the pool.
 *
 *      Note that name and extras will be copied internally, and so the memory
 *      underlying those can be reused by the caller once this call completes.
 */
extern int
shr_mres_pool_set(shr_mres_handle_t handle,
                  int pool_id,
                  shr_res_allocator_t manager,
                  int low_id,
                  int count,
                  const void *extras,
                  const char *name);

/*
 *   Function
 *      shr_mres_pool_unset
 *   Purpose
 *      Destroy a resource pool for the handle
 *   Parameters
 *      (IN) handle  : handle for the instance to access
 *      (IN) pool_id : which pool to configure (0..max_res_pools-1)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Destroys the resource pool only if there are no types using the pool.
 *
 *      It will report an error (though the pool will be destroyed) if there
 *      appear to be elements in use but no types using the pool.
 */
extern int
shr_mres_pool_unset(shr_mres_handle_t handle,
                    int pool_id);

/*
 *   Function
 *      shr_mres_pool_get
 *   Purpose
 *      Get configuration for a resource pool on a particular handle
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) pool_id  : which pool to query (0..max_res_pools-1)
 *      (OUT) manager : where to put manager that is used for this pool
 *      (OUT) low_id  : where to put low ID value for this pool
 *      (OUT) count   : where to put count value for this pool
 *      (OUT) extras  : where to put pointer to extras for this pool
 *      (OUT) name    : where to put pointer to name for this pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Both extras and name will point to internal data for the pool and MUST
 *      NOT BE MODIFIED by the caller.  If the caller wants to change these
 *      data, the pool will have to be reconfigured with shr_mres_pool_set.  If
 *      the caller wants to use the data in a destructive manner, the caller
 *      must copy the data to a local buffer first and use that buffer.
 *
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_mres_pool_get(shr_mres_handle_t handle,
                  int pool_id,
                  shr_res_allocator_t *manger,
                  int *low_id,
                  int *count,
                  const void **extras,
                  const char **name);

/*
 *   Function
 *      shr_mres_pool_info_get
 *   Purpose
 *      Get status information for a resource pool on a particular handle
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) pool_id  : which pool to query (0..max_res_pools-1)
 *      (OUT) info    : where to put information about this pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_mres_pool_info_get(shr_mres_handle_t handle,
                       int pool_id,
                       shr_res_pool_info_t *info);

/*
 *   Function
 *      shr_mres_type_set
 *   Purpose
 *      Configure a resource type
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to configure (0..max_res_types-1)
 *      (IN) pool_id   : which pool this resource uses (0..max_res_pools-1)
 *      (IN) elem_size : number of pool elements per element of this resource
 *      (IN) name      : pointer to string naming the type
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      More than one resource can use the same pool, but a single resource can
 *      only use one pool.
 *
 *      If this is called after a resource has elements allocated, it will
 *      fail.  If it is called before a resource has elements allocated, it
 *      will map the resource so it uses the specified pool.
 *
 *      This must be called after init and after the pool it uses has been
 *      configured, and before the associted resource can be used.
 *
 *      elem_size indicates how many of the underlying pool elements are
 *      required to make a single element of this resource.  Some allocators do
 *      not support more than a single element being allocated at a time, and
 *      so the resource manager will try to allocate contiguous blocks of the
 *      specified number of pool elements, but it is possible this will fail.
 *
 *      Note that name will be copied internally, and so the underlying memory
 *      can be reused by the caller once this call completes.
 */
extern int
shr_mres_type_set(shr_mres_handle_t handle,
                  int res_id,
                  int pool_id,
                  int elem_size,
                  const char *name);

/*
 *   Function
 *      shr_mres_type_unset
 *   Purpose
 *      Destroy a resource type
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to configure (0..max_res_types-1)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Will not destroy a resource type if elements of that resource are still
 *      in use.
 */
extern int
shr_mres_type_unset(shr_mres_handle_t handle,
                    int res_id);

/*
 *   Function
 *      shr_mres_type_get
 *   Purpose
 *      Get information about a resource type
 *   Parameters
 *      (IN) handle     : handle for the instance to access
 *      (IN) res_id     : which resource to query (0..max_res_types-1)
 *      (OUT) pool_id   : where to put pool ID
 *      (OUT) elem_size : where to put element size
 *      (OUT) name      : where to put name pointer
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Name will point to internal data for the type and MUST NOT BE MODIFIED
 *      by the caller.  If the caller wants to change the name, the type will
 *      have to be reconfigured with shr_res_type_set.  If the caller wants to
 *      use the name in a destructive manner, the caller must copy the name to
 *      a local buffer first and use that buffer.
 *
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_mres_type_get(shr_mres_handle_t handle,
                  int res_id,
                  int *pool_id,
                  int *elem_size,
                  const char **name);

/*
 *   Function
 *      shr_mres_type_info_get
 *   Purpose
 *      Get status information for a resource type on a particular handle
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) res_id   : which resource to query (0..max_res_types-1)
 *      (OUT) info    : where to put information about this type
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_mres_type_info_get(shr_mres_handle_t handle,
                       int res_id,
                       shr_res_type_info_t *info);

/*
 *   Function
 *      shr_mres_destroy
 *   Purpose
 *      Destroy a resource instance handle and its associated information
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will destroy all of the resource pools, then tear down the rest of
 *      the resource management for the instance.
 */
extern int
shr_mres_destroy(shr_mres_handle_t handle);

/*
 *   Function
 *      shr_mres_alloc
 *   Purpose
 *      Allocate an element or block of elements of a particular resource
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).  Some underlying managers do not support more
 *      than single element blocks, and while the manager will try to allocate
 *      a contiguous block in such case, it is possible that it will fail even
 *      if there are enough underlying elements available for the request.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 */
extern int
shr_mres_alloc(shr_mres_handle_t handle,
               int res_id,
               uint32 flags,
               int count,
               int *elem);

/*
 *   Function
 *      shr_mres_alloc_tag
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      assuring all of the elements have the same tag.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) tag      : pointer to tag value to use
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.
 *
 *      The tag pointer is a pointer to the value that will be used for tagging
 *      the requested block of elements.  Note that since tags are a number of
 *      bytes starting at the pointer, the tag must already be masked as needed
 *      and should be stored in something no larger than the number of bytes
 *      that was specified as tag size when setting up the pool.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).  Some underlying managers do not support more
 *      than single element blocks, and while the manager will try to allocate
 *      a contiguous block in such case, it is possible that it will fail even
 *      if there are enough underlying elements available for the request.
 *
 *      Some allocators do not support tagged mode.  If allocating elements
 *      from a resource that has tagging, either this call or the alloc_tag
 *      call must be used so the tag can be specified.  If neither this nor the
 *      alloc_tag call is used, the tag will be assumed to be all zeroes.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 */
extern int
shr_mres_alloc_tag(shr_mres_handle_t handle,
                   int res_id,
                   uint32 flags,
                   const void *tag,
                   int count,
                   int *elem);

/*
 *   Function
 *      shr_mres_alloc_align
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.  If WITH_ID is specified, and the
 *      requested base element does not comply with the indicated alignment,
 *      BCM_E_PARAM will be returned.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).  Some underlying managers do not support more
 *      than single element blocks, and while the manager will try to allocate
 *      a contiguous block in such case, it is possible that it will fail even
 *      if there are enough underlying elements available for the request.
 *
 *      The first element of the returned block will be at ((n * align) +
 *      offset), where n is some integer.  If it is not possible to allocate a
 *      block with the requested constraints, the call will fail.  Note that
 *      the alignment is within the specified range of the resource, and not
 *      specifically aligned against the absolute value zero; to request the
 *      alignment be against zero, specify the ALIGN_ZERO flag.
 *
 *      If offset >= align, BCM_E_PARAM.  If align is zero or negative, it will
 *      be treated as if it were 1.
 *
 *      Some allocators may not support this feature; some may place
 *      restrictions, such as the base alignment being a power of two, or not
 *      supporting the ALIGN_ZERO flag.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 */
extern int
shr_mres_alloc_align(shr_mres_handle_t handle,
                     int res_id,
                     uint32 flags,
                     int align,
                     int offset,
                     int count,
                     int *elem);

/*
 *   Function
 *      shr_mres_alloc_align_sparse
 *   Purpose
 *      Allocate a sparse block of elements of a particular resource, using a
 *      base alignment, an offset, a pattern, length, and repeat count.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) pattern  : bitmap pattern of elements to include
 *      (IN) length   : length of pattern
 *      (IN) repeats  : number of time pattern repeats
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.  If WITH_ID is specified, and the
 *      requested base element does not comply with the indicated alignment,
 *      BCM_E_PARAM will be returned.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource.
 *
 *      This function can not be used with a 'scaled' allocator.
 *
 *      The first element of the returned block will be at ((n * align) +
 *      offset), where n is some integer.  If it is not possible to allocate a
 *      block with the requested constraints, the call will fail.  Note that
 *      the alignment is within the specified range of the resource, and not
 *      specifically aligned against the absolute value zero; to request the
 *      alignment be against zero, specify the ALIGN_ZERO flag.
 *
 *      If offset >= align, BCM_E_PARAM.  If align is zero or negative, it will
 *      be treated as if it were 1.
 *
 *      Some allocators may not support this feature; some may place
 *      restrictions, such as the base alignment being a power of two, or not
 *      supporting the ALIGN_ZERO flag.
 *
 *      Partial blocks will not be allocated.
 *
 *      The pattern argument is a bitmap of the elements that are of interest
 *      in a single iteration of the pattern (and only the least significant
 *      'length' bits are used; higher bits are ignored).  The bit with value
 *      (1 << k) set indicates the element at (elem + k) must be in the block;
 *      clear it indicates the element at (elem + k) is not in the block.  This
 *      repeats for as many iterations as indicated by 'repeats'.
 *
 *      For example:
 *        align = 4, offs = 0, pattern = 0x7, length = 8, repeats = 2 would
 *        request a block of three elements, a gap of one element, and then
 *        another block of three elements, with the first allocated element
 *        aligned to a multiple of four.
 *
 *        align = 8, offs = 1, pattern = 0x1, length = 2, repeats = 4 would
 *        request four oddly numbered elements with the first one allocated at
 *        (8 * n) + 1 where n is some arbitrary number.
 *
 *      Note the bitmap is considered based at the requested alignment+offset,
 *      even if the least significant bit is not set, so it is probably
 *      simplest to ensure the LSb of pattern is set.
 *
 *      For example:
 *        align = 4, offs = 0, pattern = 0xA, length = 4, repeats = 1 would
 *        request two elements, but the returned base element number would be
 *        (4 * n), while the actually allocated elements would be (4 * n) + 1
 *        and (4 * n) + 3.
 *
 *      The caller must track the pattern, length and repeats values and
 *      provide these values along with the elem value when freeing the block.
 *
 *      Any allocation made through this function must be freed using the
 *      shr_mres_free_sparse function.
 *
 *      Sparse functions do not work with scaled resources.
 */
extern int
shr_mres_alloc_align_sparse(shr_mres_handle_t handle,
                            int res_id,
                            uint32 flags,
                            int align,
                            int offset,
                            uint32 pattern,
                            int length,
                            int repeats,
                            int *elem);

/*
 *   Function
 *      shr_mres_alloc_align_tag
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset, and assuring the elements all
 *      have the same tag.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) tag      : pointer to tag value to use
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.  If WITH_ID is specified, and the
 *      requested base element does not comply with the indicated alignment,
 *      BCM_E_PARAM will be returned.
 *
 *      The tag pointer is a pointer to the value that will be used for tagging
 *      the requested block of elements.  Note that since tags are a number of
 *      bytes starting at the pointer, the tag must already be masked as needed
 *      and should be stored in something no larger than the number of bytes
 *      that was specified as tag size when setting up the pool.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool), and ensuring all have the same tag (elements can
 *      be within the same grain as elements from other blocks only if the tag
 *      of the partial grain is equal to the tag for the new elements).  Some
 *      underlying managers do not support more than single element blocks, and
 *      while the manager will try to allocate a contiguous block in such case,
 *      it is possible that it will fail even if there are enough underlying
 *      elements available for the request.
 *
 *      The first element of the returned block will be at ((n * align) +
 *      offset), where n is some integer.  If it is not possible to allocate a
 *      block with the requested constraints, the call will fail.  Note that
 *      the alignment is within the specified range of the resource, and not
 *      specifically aligned against the absolute value zero; to request the
 *      alignment be against zero, specify the ALIGN_ZERO flag.
 *
 *      If offset >= align, BCM_E_PARAM.  If align is zero or negative, it will
 *      be treated as if it were 1.
 *
 *      Some allocators may not support this feature; some may place
 *      restrictions, such as the base alignment being a power of two, or not
 *      supporting the ALIGN_ZERO flag.
 *
 *      Some allocators do not support tagged mode.  If allocating elements
 *      from a resource that has tagging, either this call or the alloc_tag
 *      call must be used so the tag can be specified.  If neither this nor the
 *      alloc_tag call is used, the tag will be assumed to be all zeroes.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 */
extern int
shr_mres_alloc_align_tag(shr_mres_handle_t handle,
                         int res_id,
                         uint32 flags,
                         int align,
                         int offset,
                         const void *tag,
                         int count,
                         int *elem);

/*
 *   Function
 *      shr_mres_free
 *   Purpose
 *      Free an element or block of elements of a particular resource
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to free
 *      (IN) count  : elements in the block to free
 *      (IN) elem   : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will free a single block of the requested number of elements,
 *      starting at the specified element.  Some of the allocators do not deal
 *      with blocks and so must be told how large a block was, so it is
 *      obligatory that the caller be able to provide such information.
 *
 *      This should only be called with valid data (base element and element
 *      count) against known allocated blocks.  Trying to free a block that is
 *      not in use or trying to free something that spans multiple allocated
 *      blocks may not work.
 *
 *      Must not be used to free sparse blocks.
 */
extern int
shr_mres_free(shr_mres_handle_t handle,
              int res_id,
              int count,
              int elem);

/*
 *   Function
 *      shr_mres_free_and_status
 *   Purpose
 *      Free an element or block of elements of a particular resource, then get
 *      flags about that resource.
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to free
 *      (IN) count  : elements in the block to free
 *      (IN) elem   : the element to free (or base of the block to free)
 *      (OUT) flags : where to put the flags
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will free a single block of the requested number of elements,
 *      starting at the specified element.  Some of the allocators do not deal
 *      with blocks and so must be told how large a block was, so it is
 *      obligatory that the caller be able to provide such information.
 *
 *      This should only be called with valid data (base element and element
 *      count) against known allocated blocks.  Trying to free a block that is
 *      not in use or trying to free something that spans multiple allocated
 *      blocks may not work.
 *
 *      The value at status will generally be updated to reflect the proper
 *      status unless the type is not defined at the time of the call.  The
 *      values for the individual flags are of the SHR_RES_FREED_* form.
 *
 *      Must not be used to free sparse blocks.
 */
extern int
shr_mres_free_and_status(shr_mres_handle_t handle,
                         int res_id,
                         int count,
                         int elem,
                         uint32 *status);

/*
 *   Function
 *      shr_mres_free_sparse
 *   Purpose
 *      Free a sparse block of elements of a particular resource
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to free
 *      (IN) pattern: pattern of elements
 *      (IN) length : length of pattern
 *      (IN) repeats: number of iterations of the pattern
 *      (IN) elem   : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will free a sparse block of elements, starting at the specified
 *      element and proceeding as specified.
 *
 *      This function can not be used with a 'scaled' allocator.
 *
 *      This should only be called with valid data (base element, pattern,
 *      length, repeats).  Trying to free a block that is not in use or trying
 *      to free something that spans multiple allocated blocks may not work.
 *
 *      Anything allocated with shr_mres_alloc_align_sparse must be freed using
 *      this function.  While it is possible to use this function to free
 *      blocks allocated with other functions, it is not advised.
 *
 *      Sparse functions do not work with scaled resources.
 *
 *      While this can be used to free non-sparse blocks, it is probably best
 *      to use the non-sparse free functions for that purpose.
 */
extern int
shr_mres_free_sparse(shr_mres_handle_t handle,
                     int res_id,
                     uint32 pattern,
                     int length,
                     int repeats,
                     int elem);

/*
 *   Function
 *      shr_mres_free_sparse
 *   Purpose
 *      Free a sparse block of elements of a particular resource and request
 *      status flags from the free.
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to free
 *      (IN) pattern: pattern of elements
 *      (IN) length : length of pattern
 *      (IN) repeats: number of iterations of the pattern
 *      (IN) elem   : the element to free (or base of the block to free)
 *      (OUT) status: where to put the status flags
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will free a sparse block of elements, starting at the specified
 *      element and proceeding as specified.
 *
 *      This function can not be used with a 'scaled' allocator.
 *
 *      This should only be called with valid data (base element, pattern,
 *      length, repeats).  Trying to free a block that is not in use or trying
 *      to free something that spans multiple allocated blocks may not work.
 *
 *      Anything allocated with shr_mres_alloc_align_sparse must be freed using
 *      this function.  While it is possible to use this function to free
 *      blocks allocated with other functions, it is not advised.
 *
 *      Sparse functions do not work with scaled resources.
 *
 *      While this can be used to free non-sparse blocks, it is probably best
 *      to use the non-sparse free functions for that purpose.
 */
extern int
shr_mres_free_sparse_and_status(shr_mres_handle_t handle,
                                int res_id,
                                uint32 pattern,
                                int length,
                                int repeats,
                                int elem,
                                uint32 *status);

/*
 *   Function
 *      shr_mres_check
 *   Purpose
 *      Check the status of a specific element
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to check
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_NOT_FOUND if the element is not in use
 *      BCM_E_EXISTS if the element is in use
 *      BCM_E_PARAM if the element is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check whether the requested block of the resource is
 *      allocated.  Note that if any element of the resource in the range of
 *      [elem..(elem+count-1)] (inclusive) is not free, it returns
 *      BCM_E_EXISTS; it will only return BCM_E_NOT_FOUND if all elements
 *      within the specified block are free.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID.
 *
 *      Unlike shr_mres_check_all, this considers the whole set of elements in
 *      use if any of them is in use.
 */
extern int
shr_mres_check(shr_mres_handle_t handle,
               int res_id,
               int count,
               int elem);

/*
 *   Function
 *      shr_mres_check_all
 *   Purpose
 *      Check the status of a group of elements
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to check
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check all of the elements (and blocks, on those allocators
 *      supporting block tracking) in the specified range, and return a result
 *      based upon that examination.
 *
 *      Normally, this would be called in case of intent to directly replace a
 *      possibly existing block of elements with a new one.
 *
 *      Unlike shr_mres_check, this does not immediately return if it finds an
 *      in-use element.  Instead, it continues through the specified range and
 *      returns a more detailed result, considering the requested range as if
 *      it were intended for immediate direct reuse (such as WITH_ID+REPLACE).
 *
 *      For TAGGED resources, this assumes the tag to be used is the tag
 *      already associated with the first element in the block.  If the default
 *      tag behaviour is desired instead, use shr_mres_check_all_tag with the
 *      tag pointer set to NULL.
 */
extern int
shr_mres_check_all(shr_mres_handle_t handle,
                   int res_id,
                   int count,
                   int elem);

/*
 *   Function
 *      shr_mres_check_all_sparse
 *   Purpose
 *      Check the status of a sparse group of elements
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to check
 *      (IN) pattern: pattern of elements to check
 *      (IN) length : length of the pattern
 *      (IN) repeats: number of times the pattern repeats
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check whether the requested sparse block of the resource is
 *      allocated.  If all of the specified elements are allocated, it will
 *      return BCM_E_FULL; if all of the specified elements are free, it will
 *      return BCM_E_EMPTY; if some of the specified elements are allocated and
 *      some are free, it will return BCM_E_EXISTS.
 *
 *      See shr_res_bitmap_sparse_alloc_align_sparse for information about
 *      how sparse patterns are specified.
 *
 *      Normally, this would be called in case of intent to directly replace a
 *      possibly existing block of elements with a new one.
 *
 *      Unlike shr_mres_check, this does not immediately return if it finds an
 *      in-use element.  Instead, it continues through the specified range and
 *      returns a more detailed result, considering the requested range as if
 *      it were intended for immediate direct reuse (such as WITH_ID+REPLACE).
 *
 *      For TAGGED resources, this assumes the tag to be used is the tag
 *      already associated with the first element in the block.  If the default
 *      tag behaviour is desired instead, use shr_mres_check_all_tag with the
 *      tag pointer set to NULL.
 *
 *      Sparse functions do not work with scaled resources.
 */
extern int
shr_mres_check_all_sparse(shr_mres_handle_t handle,
                          int res_id,
                          uint32 pattern,
                          int length,
                          int repeats,
                          int elem);

/*
 *   Function
 *      shr_mres_check_all_tag
 *   Purpose
 *      Check the status of a group of elements
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) res_id : which resource to check
 *      (IN) tag    : the tag to consider during the check
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s)/tag(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check all of the elements (and blocks, on those allocators
 *      supporting block tracking) in the specified range, and return a result
 *      based upon that examination.
 *
 *      Normally, this would be called in case of intent to directly replace a
 *      possibly existing block of elements with a new one.
 *
 *      Unlike shr_mres_check, this does not immediately return if it finds an
 *      in-use element.  Instead, it continues through the specified range and
 *      returns a more detailed result, considering the requested range as if
 *      it were intended for immediate direct reuse (such as WITH_ID+REPLACE).
 */
extern int
shr_mres_check_all_tag(shr_mres_handle_t handle,
                       int res_id,
                       const void *tag,
                       int count,
                       int elem);

/*
 *   Function
 *      shr_mres_alloc_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *count    : elements to allocate in this block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The flags, count, and elem all point to arrays.  This function will act
 *      as if shr_mres_alloc were called exactly once with each member from
 *      these arrays as its arguments, including the behaviour for elem.
 *
 *      If all goes well, grp_done will be set equal to grp_size; if something
 *      goes wrong, grp_done will indicate how many of the members were
 *      successfully allocated before the failure.
 *
 *      If the SHR_RES_ALLOC_GROUP_ATOMIC flag is set in the group flags, any
 *      failure will be taken as complete, and all elements/blocks allocated up
 *      to that point will be freed before the error is returned.  If it is not
 *      set in the group flags, any failure will stop further allocations, but
 *      will leave existing allocations alone.
 *
 *      This function is essentially a wrapper around shr_mres_alloc.
 */
extern int
shr_mres_alloc_group(shr_mres_handle_t handle,
                     int res_id,
                     uint32 grp_flags,
                     int grp_size,
                     int *grp_done,
                     const uint32 *flags,
                     const int *count,
                     int *elem);

/*
 *   Function
 *      shr_mres_alloc_tag_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *tag      : tags to use for each block
 *      (IN) *count    : elements to allocate in each block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The flags, count, and elem all point to arrays.  This function will act
 *      as if shr_mres_alloc were called exactly once with each member from
 *      these arrays as its arguments, including the behaviour for elem.
 *
 *      If all goes well, grp_done will be set equal to grp_size; if something
 *      goes wrong, grp_done will indicate how many of the members were
 *      successfully allocated before the failure.
 *
 *      If the SHR_RES_ALLOC_GROUP_ATOMIC flag is set in the group flags, any
 *      failure will be taken as complete, and all elements/blocks allocated up
 *      to that point will be freed before the error is returned.  If it is not
 *      set in the group flags, any failure will stop further allocations, but
 *      will leave existing allocations alone.
 *
 *      This function is essentially a wrapper around shr_mres_alloc.
 */
extern int
shr_mres_alloc_tag_group(shr_mres_handle_t handle,
                         int res_id,
                         uint32 grp_flags,
                         int grp_size,
                         int *grp_done,
                         const uint32 *flags,
                         const void** tag,
                         const int *count,
                         int *elem);

/*
 *   Function
 *      shr_mres_alloc_align_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource, with specified alignment and offset
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *align    : base alignment in this block
 *      (IN) *offset   : offest from base alignment for first elem in this blk
 *      (IN) *count    : elements to allocate in this block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The flags, align, offset, count, and elem all point to arrays.  This
 *      function will act as if shr_mres_alloc_align were called exactly once
 *      with each member from these arrays as its arguments, including the
 *      behaviour for elem.
 *
 *      If all goes well, grp_done will be set equal to grp_size; if something
 *      goes wrong, grp_done will indicate how many of the members were
 *      successfully allocated before the failure.
 *
 *      If the SHR_RES_ALLOC_GROUP_ATOMIC flag is set in the group flags, any
 *      failure will be taken as complete, and all elements/blocks allocated up
 *      to that point will be freed before the error is returned.  If it is not
 *      set in the group flags, any failure will stop further allocations, but
 *      will leave existing allocations alone.
 *
 *      This function is essentially a wrapper around shr_mres_alloc_align.
 */
extern int
shr_mres_alloc_align_group(shr_mres_handle_t handle,
                           int res_id,
                           uint32 grp_flags,
                           int grp_size,
                           int *grp_done,
                           const uint32 *flags,
                           const int *align,
                           const int *offset,
                           const int *count,
                           int *elem);

/*
 *   Function
 *      shr_mres_alloc_align_tag_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource, with specified alignment and offset
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *align    : base alignment in this block
 *      (IN) *offset   : offest from base alignment for first elem in this blk
 *      (IN) *tag      : pointers to the tags for each block
 *      (IN) *count    : elements to allocate in this block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The flags, align, offset, count, and elem all point to arrays.  This
 *      function will act as if shr_mres_alloc_align were called exactly once
 *      with each member from these arrays as its arguments, including the
 *      behaviour for elem.
 *
 *      If all goes well, grp_done will be set equal to grp_size; if something
 *      goes wrong, grp_done will indicate how many of the members were
 *      successfully allocated before the failure.
 *
 *      If the SHR_RES_ALLOC_GROUP_ATOMIC flag is set in the group flags, any
 *      failure will be taken as complete, and all elements/blocks allocated up
 *      to that point will be freed before the error is returned.  If it is not
 *      set in the group flags, any failure will stop further allocations, but
 *      will leave existing allocations alone.
 *
 *      This function is essentially a wrapper around shr_mres_alloc_align.
 */
extern int
shr_mres_alloc_align_tag_group(shr_mres_handle_t handle,
                               int res_id,
                               uint32 grp_flags,
                               int grp_size,
                               int *grp_done,
                               const uint32 *flags,
                               const int *align,
                               const int *offset,
                               const void **tag,
                               const int *count,
                               int *elem);

/*
 *   Function
 *      shr_mres_free_group
 *   Purpose
 *      Free a group of elements or blocks of elements of a particular resource
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to free
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to free
 *      (IN) *elem     : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The flags, count, and elem all point to arrays.  This function will act
 *      as if shr_mres_free were called exactly once with each member from
 *      these arrays as its arguments, including the behaviour for elem.
 *
 *      If all goes well, grp_done will be set equal to grp_size; if something
 *      goes wrong, grp_done will indicate how many of the members were
 *      successfully freed before the failure.
 *
 *      This function is essentially a wrapper around shr_mres_free.
 */
extern int
shr_mres_free_group(shr_mres_handle_t handle,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const int *count,
                    const int *elem);

/*
 *   Function
 *      shr_mres_free_group_and_status
 *   Purpose
 *      Free a group of elements or blocks of elements of a particular resource
 *      and then get status flags
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to free
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to free
 *      (IN) *elem     : the element to free (or base of the block to free)
 *      (OUT) status   : where to put the status flags
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The flags, count, and elem all point to arrays.  This function will act
 *      as if shr_mres_free were called exactly once with each member from
 *      these arrays as its arguments, including the behaviour for elem.
 *
 *      If all goes well, grp_done will be set equal to grp_size; if something
 *      goes wrong, grp_done will indicate how many of the members were
 *      successfully freed before the failure.
 *
 *      This function is essentially a wrapper around shr_mres_free_and_status.
 *
 *      The value at status will generally be updated to reflect the proper
 *      status unless the type is not defined at the time of the call.  The
 *      values for the individual flags are of the SHR_RES_FREED_* form.
 */
extern int
shr_mres_free_group_and_status(shr_mres_handle_t handle,
                               int res_id,
                               uint32 grp_flags,
                               int grp_size,
                               int *grp_done,
                               const int *count,
                               const int *elem,
                               uint32 *status);

/*
 *   Function
 *      shr_mres_check_group
 *   Purpose
 *      Check the status of a group of specific elements
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to check
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to check
 *      (IN) *elem     : the element to check (or base of the block to check)
 *      (OUT) *status  : where to put the check results
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This behaves similarly to calling shr_mres_check, except that it places
 *      the result in the provided status array rather than returning the
 *      status for exactly one element.
 *
 *      The result code from this function should be BCM_E_NONE unless there is
 *      something obviously wrong with the arguments or an unexpected result is
 *      encountered.  Normally, grp_done would be set equal to grp_size, but if
 *      there is an unexpected failure, grp_done will indicate how many
 *      elements were checked before the failure.
 *
 *      If the SHR_RES_ALLOC_GROUP_ATOMIC is set, this function will continue
 *      to the end of the provided list despite any errors that occur once it
 *      starts to check the provided list, and will return BCM_E_NONE if it
 *      gets that far, but it will still return an appropriate error if it is
 *      unable to begin checking the list due to some error.
 *
 *      This function is essentially a wrapper around shr_mres_check.
 */
extern int
shr_mres_check_group(shr_mres_handle_t handle,
                     int res_id,
                     uint32 grp_flags,
                     int grp_size,
                     int *grp_done,
                     const int *count,
                     const int *elem,
                     int *status);

/*
 *   Function
 *      shr_mres_check_all_group
 *   Purpose
 *      Check the status of a group of specific elements
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to check
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to check
 *      (IN) *elem     : the element to check (or base of the block to check)
 *      (OUT) *status  : where to put the check results
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This behaves similarly to calling shr_mres_check, except that it places
 *      the result in the provided status array rather than returning the
 *      status for exactly one element.
 *
 *      The result code from this function should be BCM_E_NONE unless there is
 *      something obviously wrong with the arguments or an unexpected result is
 *      encountered.  Normally, grp_done would be set equal to grp_size, but if
 *      there is an unexpected failure, grp_done will indicate how many
 *      elements were checked before the failure.
 *
 *      If the SHR_RES_ALLOC_GROUP_ATOMIC is set, this function will continue
 *      to the end of the provided list despite any errors that occur once it
 *      starts to check the provided list, and will return BCM_E_NONE if it
 *      gets that far, but it will still return an appropriate error if it is
 *      unable to begin checking the list due to some error.
 *
 *      This function is essentially a wrapper around shr_mres_check_all.
 */
extern int
shr_mres_check_all_group(shr_mres_handle_t handle,
                         int res_id,
                         uint32 grp_flags,
                         int grp_size,
                         int *grp_done,
                         const int *count,
                         const int *elem,
                         int *status);

/*
 *   Function
 *      shr_mres_check_all_tag_group
 *   Purpose
 *      Check the status of a group of specific elements
 *   Parameters
 *      (IN) handle    : handle for the instance to access
 *      (IN) res_id    : which resource to check
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) **tag     : the tags to be used in the check
 *      (IN) *count    : elements in the block to check
 *      (IN) *elem     : the element to check (or base of the block to check)
 *      (OUT) *status  : where to put the check results
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This behaves similarly to calling shr_mres_check, except that it places
 *      the result in the provided status array rather than returning the
 *      status for exactly one element.
 *
 *      The result code from this function should be BCM_E_NONE unless there is
 *      something obviously wrong with the arguments or an unexpected result is
 *      encountered.  Normally, grp_done would be set equal to grp_size, but if
 *      there is an unexpected failure, grp_done will indicate how many
 *      elements were checked before the failure.
 *
 *      If the SHR_RES_ALLOC_GROUP_ATOMIC is set, this function will continue
 *      to the end of the provided list despite any errors that occur once it
 *      starts to check the provided list, and will return BCM_E_NONE if it
 *      gets that far, but it will still return an appropriate error if it is
 *      unable to begin checking the list due to some error.
 *
 *      This function is essentially a wrapper around shr_mres_check_all_tag.
 */
extern int
shr_mres_check_all_tag_group(shr_mres_handle_t handle,
                             int res_id,
                             uint32 grp_flags,
                             int grp_size,
                             int *grp_done,
                             const void **tag,
                             const int *count,
                             const int *elem,
                             int *status);

/*
 *   Function
 *      shr_mres_dump
 *   Purpose
 *      Diagnostic dump of a unit's resource management information
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 */
extern int
shr_mres_dump(shr_mres_handle_t handle);

/******************************************************************************
 *
 *  Unit based shared resource management (global within a unit)
 *
 *  These functions are intended for use when resources are common between
 *  multiple modules on a single unit, or can be used in that way.
 */

/*
 *   Function
 *      shr_res_init
 *   Purpose
 *      Initialize the resource manager for the unit
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) num_res_types : number of resource types for the unit
 *      (IN) num_res_pools : number of resource pools for the unit
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_create for the unit global resource intance.
 */
extern int
shr_res_init(int unit,
             int num_res_types,
             int num_res_pools);

/*
 *   Function
 *      shr_res_get
 *   Purpose
 *      Get number of resource pools and types configured for a unit
 *   Parameters
 *      (IN) unit           : unit number of the device
 *      (OUT) num_res_types : where to put number of resource types for unit
 *      (OUT) num_res_pools : where to put number of resource pools for unit
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_get against the unit global resource instance.
 */
extern int
shr_res_get(int unit,
            int *num_res_types,
            int *num_res_pools);

/*
 *   Function
 *      shr_res_pool_set
 *   Purpose
 *      Configure a resource pool for the unit
 *   Parameters
 *      (IN) unit    : unit number of the device
 *      (IN) pool_id : which pool to configure (0..max_res_pools-1)
 *      (IN) manager : which manager to use for this pool
 *      (IN) low_id  : lowest valid resource element ID in this pool
 *      (IN) count   : number of valid resource elements in this pool
 *      (IN) extras  : pointer to extra information for the manager type
 *      (IN) name    : pointer to string naming the pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_pool_set against the unit global resource instance.
 */
extern int
shr_res_pool_set(int unit,
                 int pool_id,
                 shr_res_allocator_t manager,
                 int low_id,
                 int count,
                 const void *extras,
                 const char *name);

/*
 *   Function
 *      shr_res_pool_unset
 *   Purpose
 *      Destroy a resource pool for the unit
 *   Parameters
 *      (IN) unit    : unit number of the device
 *      (IN) pool_id : which pool to configure (0..max_res_pools-1)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_pool_unset against the unit global resource instance.
 */
extern int
shr_res_pool_unset(int unit,
                   int pool_id);

/*
 *   Function
 *      shr_res_pool_get
 *   Purpose
 *      Get configuration for a resource pool on a particular unit
 *   Parameters
 *      (IN) unit     : unit number of the device
 *      (IN) pool_id  : which pool to query (0..max_res_pools-1)
 *      (OUT) manager : where to put manager that is used for this pool
 *      (OUT) low_id  : where to put low ID value for this pool
 *      (OUT) count   : where to put count value for this pool
 *      (OUT) extras  : where to put pointer to extras for this pool
 *      (OUT) name    : where to put pointer to name for this pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_pool_get against the unit global resource instance.
 */
extern int
shr_res_pool_get(int unit,
                 int pool_id,
                 shr_res_allocator_t *manger,
                 int *low_id,
                 int *count,
                 const void **extras,
                 const char **name);

/*
 *   Function
 *      shr_res_pool_info_get
 *   Purpose
 *      Get status information for a resource pool
 *   Parameters
 *      (IN) unit     : unit number of the device
 *      (IN) pool_id  : which pool to query (0..max_res_pools-1)
 *      (OUT) info    : where to put information about this pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_res_pool_info_get(int unit,
                      int pool_id,
                      shr_res_pool_info_t *info);

/*
 *   Function
 *      shr_res_type_set
 *   Purpose
 *      Configure a resource type
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to configure (0..max_res_types-1)
 *      (IN) pool_id   : which pool this resource uses (0..max_res_pools-1)
 *      (IN) elem_size : number of pool elements per element of this resource
 *      (IN) name      : pointer to string naming the type
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_type_set against the unit global resource instance.
 */
extern int
shr_res_type_set(int unit,
                 int res_id,
                 int pool_id,
                 int elem_size,
                 const char *name);

/*
 *   Function
 *      shr_res_type_unset
 *   Purpose
 *      Destroy a resource type
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to configure (0..max_res_types-1)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_type_unset against the unit global resource instance.
 */
extern int
shr_res_type_unset(int unit,
                   int res_id);

/*
 *   Function
 *      shr_res_type_get
 *   Purpose
 *      Get information about a resource type
 *   Parameters
 *      (IN) unit       : unit number of the device
 *      (IN) res_id     : which resource to query (0..max_res_types-1)
 *      (OUT) pool_id   : where to put pool ID
 *      (OUT) elem_size : where to put element size
 *      (OUT) name      : where to put name pointer
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_type_get against the unit global resource instance.
 */
extern int
shr_res_type_get(int unit,
                 int res_id,
                 int *pool_id,
                 int *elem_size,
                 const char **name);

/*
 *   Function
 *      shr_res_type_info_get
 *   Purpose
 *      Get status information for a resource type
 *   Parameters
 *      (IN) unit       : unit number of the device
 *      (IN) res_id   : which resource to query (0..max_res_types-1)
 *      (OUT) info    : where to put information about this type
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_res_type_info_get(int unit,
                      int res_id,
                      shr_res_type_info_t *info);

/*
 *   Function
 *      shr_res_detach
 *   Purpose
 *      Remove all resource management for a unit
 *   Parameters
 *      (IN) unit : unit number of the device
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_destroy against the unit global resource instance, and
 *      then overwrites the handle with NULL.
 */
extern int
shr_res_detach(int unit);

/*
 *   Function
 *      shr_res_alloc
 *   Purpose
 *      Allocate an element or block of elements of a particular resource
 *   Parameters
 *      (IN) unit     : unit number of the device
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc against the unit global resource instance.
 */
extern int
shr_res_alloc(int unit,
              int res_id,
              uint32 flags,
              int count,
              int *elem);

/*
 *   Function
 *      shr_res_alloc_tag
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      assuring all of the elements have the same tag.
 *   Parameters
 *      (IN) unit     : unit number of the device
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) tag      : pointer to the tag for the elements
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_tag against the unit global resource instance.
 */
extern int
shr_res_alloc_tag(int unit,
                  int res_id,
                  uint32 flags,
                  const void *tag,
                  int count,
                  int *elem);

/*
 *   Function
 *      shr_res_alloc_align
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset.
 *   Parameters
 *      (IN) unit     : unit number of the device
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_align against the unit global resource instance.
 */
extern int
shr_res_alloc_align(int unit,
                    int res_id,
                    uint32 flags,
                    int align,
                    int offset,
                    int count,
                    int *elem);

/*
 *   Function
 *      shr_res_alloc_align_sparse
 *   Purpose
 *      Allocate a sparse block of elements of a particular resource, using a
 *      base alignment and an offset.
 *   Parameters
 *      (IN) unit     : unit number of the device
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) pattern  : pattern of elements to allocate
 *      (IN) length   : length of pattern
 *      (IN) repeats  : number of iterations of the pattern
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_align_sparse against the unit global resource
 *      instance.
 */
extern int
shr_res_alloc_align_sparse(int unit,
                           int res_id,
                           uint32 flags,
                           int align,
                           int offset,
                           uint32 pattern,
                           int length,
                           int repeats,
                           int *elem);

/*
 *   Function
 *      shr_res_alloc_align_tag
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset, and assuring the elements all
 *      have the same tag.
 *   Parameters
 *      (IN) unit     : unit number of the device
 *      (IN) res_id   : which resource to allocate
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) tag      : pointer to the tag for the elements
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_align_tag against the unit global resource
 *      instance.
 */
extern int
shr_res_alloc_align_tag(int unit,
                        int res_id,
                        uint32 flags,
                        int align,
                        int offset,
                        const void *tag,
                        int count,
                        int *elem);

/*
 *   Function
 *      shr_res_free
 *   Purpose
 *      Free an element or block of elements of a particular resource
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to free
 *      (IN) count  : elements in the block to free
 *      (IN) elem   : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_free against the unit global resource instance.
 */
extern int
shr_res_free(int unit,
             int res_id,
             int count,
             int elem);

/*
 *   Function
 *      shr_res_free
 *   Purpose
 *      Free an element or block of elements of a particular resource, then get
 *      flags about that resource.
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to free
 *      (IN) count  : elements in the block to free
 *      (IN) elem   : the element to free (or base of the block to free)
 *      (OUT) flags : where to put the flags
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_free_and_status against the unit global resource
 *      instance.
 */
extern int
shr_res_free_and_status(int unit,
                        int res_id,
                        int count,
                        int elem,
                        uint32 *flags);

/*
 *   Function
 *      shr_res_free
 *   Purpose
 *      Free a sparse block of elements of a particular resource
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to free
 *      (IN) pattern: pattern of elements to free
 *      (IN) length : length of the pattern
 *      (IN) repeats: number of iterations of the pattern
 *      (IN) elem   : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_free_sparse against the unit global resource instance.
 */
extern int
shr_res_free_sparse(int unit,
                    int res_id,
                    uint32 pattern,
                    int length,
                    int repeats,
                    int elem);

/*
 *   Function
 *      shr_res_free
 *   Purpose
 *      Free a sparse block of elements of a particular resource, then get
 *      flags about that resource.
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to free
 *      (IN) pattern: pattern of elements to free
 *      (IN) length : length of the pattern
 *      (IN) repeats: number of iterations of the pattern
 *      (IN) elem   : the element to free (or base of the block to free)
 *      (OUT) flags : where to put the flags
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_free_sparse_and_status against the unit global
 *      resource instance.
 */
extern int
shr_res_free_sparse_and_status(int unit,
                               int res_id,
                               uint32 pattern,
                               int length,
                               int repeats,
                               int elem,
                               uint32 *flags);

/*
 *   Function
 *      shr_res_check
 *   Purpose
 *      Check the status of a specific element
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to check
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_NOT_FOUND if the element is not in use
 *      BCM_E_EXISTS if the element is in use
 *      BCM_E_PARAM if the element is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_check against the unit global resource instance.
 */
extern int
shr_res_check(int unit,
              int res_id,
              int count,
              int elem);

/*
 *   Function
 *      shr_res_check_all
 *   Purpose
 *      Check the status of a group of elements
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to check
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_check_all against the unit global resource instance.
 */
extern int
shr_res_check_all(int unit,
                  int res_id,
                  int count,
                  int elem);

/*
 *   Function
 *      shr_res_check_all
 *   Purpose
 *      Check the status of a group of elements
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to check
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_check_all_sparse against the unit global resource
 *      instance.
 */
extern int
shr_res_check_all_sparse(int unit,
                         int res_id,
                         uint32 pattern,
                         int length,
                         int repeats,
                         int elem);

/*
 *   Function
 *      shr_res_check_all_tag
 *   Purpose
 *      Check the status of a group of elements
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) res_id : which resource to check
 *      (IN) tag    : tag for checking
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s)/tag(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_check_all against the unit global resource instance.
 */
extern int
shr_res_check_all_tag(int unit,
                      int res_id,
                      const void *tag,
                      int count,
                      int elem);

/*
 *   Function
 *      shr_res_alloc_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *count    : elements to allocate in this block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_group against the unit global resource instance.
 */
extern int
shr_res_alloc_group(int unit,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const uint32 *flags,
                    const int *count,
                    int *elem);

/*
 *   Function
 *      shr_res_alloc_tag_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *tag      : pointers to the tags for each block
 *      (IN) *count    : elements to allocate in this block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_group against the unit global resource instance.
 */
extern int
shr_res_alloc_tag_group(int unit,
                        int res_id,
                        uint32 grp_flags,
                        int grp_size,
                        int *grp_done,
                        const uint32 *flags,
                        const void **tag,
                        const int *count,
                        int *elem);

/*
 *   Function
 *      shr_res_alloc_align_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource, with specified alignment and offset
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *align    : base alignment in this block
 *      (IN) *offset   : offest from base alignment for first elem in this blk
 *      (IN) *count    : elements to allocate in this block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_align_group against the unit global resource
 *      instance.
 */
extern int
shr_res_alloc_align_group(int unit,
                          int res_id,
                          uint32 grp_flags,
                          int grp_size,
                          int *grp_done,
                          const uint32 *flags,
                          const int *align,
                          const int *offset,
                          const int *count,
                          int *elem);

/*
 *   Function
 *      shr_res_alloc_align_tag_group
 *   Purpose
 *      Allocate an group of elements or blocks of elements of a particular
 *      resource, with specified alignment and offset
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to allocate
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members of the group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *flags    : flags providing specifics of what/how to allocate
 *      (IN) *align    : base alignment in this block
 *      (IN) *offset   : offest from base alignment for first elem in this blk
 *      (IN) *tag      : pointers to the tags for each block
 *      (IN) *count    : elements to allocate in this block
 *      (IN/OUT) *elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_alloc_align_group against the unit global resource
 *      instance.
 */
extern int
shr_res_alloc_align_tag_group(int unit,
                              int res_id,
                              uint32 grp_flags,
                              int grp_size,
                              int *grp_done,
                              const uint32 *flags,
                              const int *align,
                              const int *offset,
                              const void **tag,
                              const int *count,
                              int *elem);

/*
 *   Function
 *      shr_res_free_group
 *   Purpose
 *      Free a group of elements or blocks of elements of a particular resource
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to free
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to free
 *      (IN) *elem     : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_free_group against the unit global resource instance.
 */
extern int
shr_res_free_group(int unit,
                   int res_id,
                   uint32 grp_flags,
                   int grp_size,
                   int *grp_done,
                   const int *count,
                   const int *elem);

/*
 *   Function
 *      shr_res_free_group_and_status
 *   Purpose
 *      Free a group of elements or blocks of elements of a particular resource
 *      and get status flags
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to free
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to free
 *      (IN) *elem     : the element to free (or base of the block to free)
 *      (OUT) *status  : where to put the status flags
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_free_group_and_status against the unit global resource
 *      instance.
 */
extern int
shr_res_free_group_and_status(int unit,
                              int res_id,
                              uint32 grp_flags,
                              int grp_size,
                              int *grp_done,
                              const int *count,
                              const int *elem,
                              uint32 *status);

/*
 *   Function
 *      shr_res_check_group
 *   Purpose
 *      Check the status of a group of specific elements
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to check
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to check
 *      (IN) *elem     : the element to check (or base of the block to check)
 *      (OUT) *status  : where to put the check results
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_check_group against the unit global resource instance.
 */
extern int
shr_res_check_group(int unit,
                    int res_id,
                    uint32 grp_flags,
                    int grp_size,
                    int *grp_done,
                    const int *count,
                    const int *elem,
                    int *status);

/*
 *   Function
 *      shr_res_check_all_group
 *   Purpose
 *      Check the status of a group of specific blocks of elements
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to check
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) *count    : elements in the block to check
 *      (IN) *elem     : the element to check (or base of the block to check)
 *      (OUT) *status  : where to put the check results
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_check_all_group against the unit global resource
 *      instance.
 */
extern int
shr_res_check_all_group(int unit,
                        int res_id,
                        uint32 grp_flags,
                        int grp_size,
                        int *grp_done,
                        const int *count,
                        const int *elem,
                        int *status);

/*
 *   Function
 *      shr_res_check_all_tag_group
 *   Purpose
 *      Check the status of a group of specific blocks of elements
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which resource to check
 *      (IN) grp_flags : flags to apply to the whole group
 *      (IN) grp_size  : number of members this group
 *      (OUT) grp_done : where to put how many members were completed
 *      (IN) **tag     : tags for the blocks to check
 *      (IN) *count    : elements in the block to check
 *      (IN) *elem     : the element to check (or base of the block to check)
 *      (OUT) *status  : where to put the check results
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_check_all_group against the unit global resource
 *      instance.
 */
extern int
shr_res_check_all_tag_group(int unit,
                            int res_id,
                            uint32 grp_flags,
                            int grp_size,
                            int *grp_done,
                            const void **tag,
                            const int *count,
                            const int *elem,
                            int *status);

/*
 *   Function
 *      shr_res_dump
 *   Purpose
 *      Diagnostic dump of a unit's resource management information
 *   Parameters
 *      (IN) unit      : unit number of the device
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Invokes shr_mres_dump against the unit global resource instance.
 */
extern int
shr_res_dump(int unit);

#endif /* ndef _SHR_RESMGR_H */

