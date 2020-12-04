/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        shr_template.h
 * Purpose:     Internal routines to the BCM library for template
 *              gu2 management.
 */

#ifndef _SHR_TEMPLATE_H_
/* { */
#define _SHR_TEMPLATE_H_


#include <sal/types.h>
#include <sal/core/alloc.h>
#include <sal/appl/io.h>

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT))
/* { */
#include <shared/swstate/sw_state.h>


/*
 *  Flags supported by the allocation system for each map set. If any of
 *  these are provided as group flags, they are automatically applied to the
 *  entire group, whether the individual flags include them or not.
 *
 *  SHR_TEMPLATE_MANAGE_SET_WITH_ID indicates that the template ID is provided by the
 *  caller, and that specific template.  The mapping must fail if the requested
 *  template is invalid or already in use.
 */

#define SHR_TEMPLATE_MANAGE_SINGLE_FLAGS     0x00000007
#define SHR_TEMPLATE_MANAGE_SET_WITH_ID      0x00000001
#define SHR_TEMPLATE_MANAGE_IGNORE_DATA      0x00000002
#define SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE 0x00000004

/*
 *  This enum defines the supported template management mechanisms.  Each one has some
 *  of its own advantages and disadvantages.  See the notes for each one if you
 *  need details to choose which might be best for a particular resource template management.
 */
typedef enum shr_template_manage_e {   
    SHR_TEMPLATE_MANAGE_HASH = 0,
    SHR_TEMPLATE_MANAGE_COUNT /* last one indicates how many, not valid item */
} shr_template_manage_t;

/*
 *  This is an index to two callbacks arrays configured during template manager init.
 *  The arrays hold callbacks that indicate how a certain data is being set / get                         .
 *    from/to a typical buffer. The callback in idx given in EXTRA will be used.
 */
typedef int (*shr_template_to_stream_t)(const void *data, void *to_stream, size_t size);
typedef int (*shr_template_from_stream_t)(void *data, const void *from_stream, size_t size);

typedef int shr_template_manage_hash_compare_extras_t;

/*
 *  That's the max length of types and pools' names. Trying to create a type or a pool
 *  with a longer name would return an error.
 */
#define SW_STATE_TEMPLATE_NAME_MAX_LENGTH 50

typedef struct sw_state_template_name_s {
    char name[SW_STATE_TEMPLATE_NAME_MAX_LENGTH];
} sw_state_template_name_t;


/*
 *  This describes how a single resource maps to an underlying pool.
 *
 *  templatePoolId is the ID of the tepmlate pool on this unit from which this
 *  template is drawn.
 *
 *  name is a string that names this resource.  It is used only for diagnostic
 *  purposes.  Internally, the provided name will be copied to the same cell as
 *  the structure, so the name array is really variable length.
 */
typedef struct _shr_template_type_desc_s {
    int templatePoolId;              /* unit specific template pool ID */
    int refCount;               /* number of elements allocated currently */
    sw_state_template_name_t name;               /* descriptive name (for diagnostics) */
} _shr_template_type_desc_t;


/*
 *  This describes a single resource pool on a unit.
 *
 *  templateManagerType is the ID of the reousrce manager that will be used to
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
typedef struct _shr_template_pool_desc_s {
    shr_template_manage_t templateManagerType; /* which resoource manager to use */
    int template_low_id;                /* start template id */
    int template_count;                 /* Number of templates*/
    int max_entities;                   /* Number of Entities */
    size_t data_size;                   /* Data size */
    int refCount;                       /* number of types using this pool */
    uint32 templateHandle;              /* handle for this resource */
    shr_template_manage_hash_compare_extras_t extras; /* additional config per resmgr type */
    sw_state_template_name_t name;      /* descriptive name for diagnostics */
} _shr_template_pool_desc_t;
                         
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
typedef struct _shr_template_unit_desc_s {
    uint16 templateTypeCount;          /* maximum presented resource ID */
    uint16 templatePoolCount;          /* maximum resource pool ID */
    PARSER_HINT_ARR_PTR _shr_template_type_desc_t **template_param;   /* array of type -> pool map pointers */
    PARSER_HINT_ARR_PTR _shr_template_pool_desc_t **pool;  /* array of pool description pointers */
} _shr_template_unit_desc_t;

/*This is the prototype of call back function used to print the template data based on type */
typedef void (*shr_template_print_func_t)(int unit, const void *);

/*
 *   Function
 *      shr_template_init
 *   Purpose
 *      Initialize the tamplate manager for the unit
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) num_template_types : number of template types for the unit
 *      (IN) num_template_pools : number of template pools for the unit
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      template types map to template pools, from which the actual elements of
 *      the resources are drawn, and more than one type can pull elements from
 *      the same pool, so there should always be at least as many types as
 *      there are pools.
 *
 *      The unit will be limited to the number of types and pools specified
 *      here, as the descriptor space will be allocated and cleared by this
 *      function.  See below for how to configure types and pools.
 */
extern int
shr_template_init(int unit,
                  int num_template_types,
                  int num_template_pools,
                  shr_template_to_stream_t *to_stream_cb_arr,
                  shr_template_from_stream_t *from_stream_cb_arr,
                  int cb_arr_length);

/*
 *   Function
 *      shr_template_get
 *   Purpose
 *      Get number of template pools and types configured for a unit
 *   Parameters
 *      (IN) unit           : unit number of the device
 *      (OUT) num_template_types : where to put number of template types for unit
 *      (OUT) num_template_pools : where to put number of template pools for unit
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_template_get(int unit,
                 int *num_template_types,
                 int *num_template_pools);

/*
 *   Function
 *      shr_template_pool_set
 *   Purpose
 *      Configure a template pool for the unit
 *   Parameters
 *      (IN) unit    	         : unit number of the device
 *      (IN) pool_id 	         : which pool to configure (0..max_template_pools-1)
 *      (IN) manager 	         : which manager to use for this pool
 *      (IN) template_low_id     : lowest valid template ID in this pool
 *      (IN) template_count      : number of valid templates in this pool
 *      (IN) max_entities        : the number of maximum referring entities
 *      (IN) data_size           : The size of the template data in this pool
 *      (IN) data_value          : initial value for all templates
 *      (IN) template_init_id    : the initial template for all referring entities
 *      (IN) extras   	         : pointer to extra information for the manager type
 *      (IN) name                : pointer to string naming the pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      A template pool must be configured before any resources can point to
 *      it, and can not be changed after resources point to it.
 *
 *      The underlying template management information will be created during
 *      this call.  If this call is made again for the same template pool, and
 *      there are no resources using this pool, the old information will be
 *      destroyed and the new will be created in its stead, but if resources
 *      are already using this pool, the call will fail.
 *
 *      Most of the allocation managers require some extra arguments, so the
 *      extras argument is likely obligatory.  Each allocation manager has its
 *      own addiitonal arguments, for specific behavioural control or
 *      optimisation, and the correct extras type must be used accordingly.
 *
 *      This must be called after init, and before a pool can be used, even
 *      before a resource can be assigned to the pool.
 *
 *      Note that name and extras will be copied internally, and so the memory
 *      underlying those can be reused by the caller once this call completes.
 *
 *      All templates will be initialised to the value specified by data_value,
 *      unless it is NULL, in which case the templates values will be zeroed.
 *      Templates are not guaranteed to be returned to this value when freed.
 *
 *      The template with ID specified by template_init_id will be considered
 *      allocated with template_init_cnt references.  If this feature is not
 *      to be used, set template_init_cnt to zero.
 */
extern int
shr_template_pool_set(int unit,
                 int pool_id,
                 shr_template_manage_t manager,
                 int template_low_id,
                 int template_count,
                 int max_entities,
                 uint32 global_max,
                 size_t data_size,
                 const void *extras,
                 const char *name);

/*
 *   Function
 *      shr_template_pool_get
 *   Purpose
 *      Get configuration for a resource pool on a particular unit
 *   Parameters
 *      (IN) unit             : unit number of the device
 *      (IN) pool_id          : which pool to query (0..max_template_pools-1)
 *      (OUT) manager         : where to put manager that is used for this pool
 *      (OUT) template_low_id : where to put template low ID value for this pool
 *      (OUT) template_count  : where to put template count value for this pool
 *      (OUT) data_size       : where to put data size
 *      (OUT) extras          : where to put pointer to extras for this pool
 *      (OUT) name            : where to put pointer to name for this pool
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Both extras and name will point to internal data for the pool and MUST
 *      NOT BE MODIFIED by the caller.  If the caller wants to change these
 *      data, the pool will have to be reconfigured with shr_template_pool_set.  If
 *      the caller wants to use the data in a destructive manner, the caller
 *      must copy the data to a local buffer first and use that buffer.
 *
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 *
 *      Since data_value, template_init_id, and template_init_count are only used
 *      to establish initial condition, they are not tracked afterward and not
 *      available via this call.
 */
extern int
shr_template_pool_get(int unit,
                      int pool_id,
                      shr_template_manage_t *manager,
                      int *template_low_id,
                      int *template_count,
                      int *max_entities,
                      size_t *data_size,
                      shr_template_manage_hash_compare_extras_t *extras,
                      sw_state_template_name_t *name);

/*
 *   Function
 *      shr_template_pool_unset
 *   Purpose
 *      Destroy a template pool for the unit
 *   Parameters
 *      (IN) unit    : unit number of the device
 *      (IN) pool_id : which pool to configure (0..max_template_pools-1)
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
shr_template_pool_unset(int unit,
                   int pool_id);


/*
 *   Function
 *      shr_template_type_set
 *   Purpose
 *      Configure a template type
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template to configure (0..max_template_types-1)
 *      (IN) pool_id   	   : which pool this resource uses (0..max_template_pools-1)
 *      (IN) name          : pointer to string naming the type
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      More than one resource can use the same pool, but a single resource can
 *      only use one pool.
 *
 *      If this is called after a resource has templates mapped, it will
 *      fail.  If it is called before a resource has templates mapped, it
 *      will map the resource so it uses the specified pool.
 *
 *      This must be called after init and after the pool it uses has been
 *      configured, and before the associted resource can be used.
 *
 *      Note that name will be copied internally, and so the underlying memory
 *      can be reused by the caller once this call completes.
 */
extern int
shr_template_type_set(int unit,
                      int template_type,
                      int pool_id,
                      const char *name);

/*
 *   Function
 *      shr_template_type_get
 *   Purpose
 *      Get information about a template type
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which resource to query (0..max_template_types-1)
 *      (OUT) pool_id      : where to put pool ID
 *      (OUT) name         : where to put name pointer
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Name will point to internal data for the type and MUST NOT BE MODIFIED
 *      by the caller.  If the caller wants to change the name, the type will
 *      have to be reconfigured with shr_template_type_set.  If the caller wants to
 *      use the name in a destructive manner, the caller must copy the name to
 *      a local buffer first and use that buffer.
 *
 *      Outbound arguments may be NULL for this call; any NULL outbound
 *      argument(s) will simply not be filled in.
 */
extern int
shr_template_type_get(int unit,
                      int template_type,
                      int *pool_id,
                      sw_state_template_name_t *name);

/*
 *   Function
 *      shr_template_type_unset
 *   Purpose
 *      Destroy a template type
 *   Parameters
 *      (IN) unit      : unit number of the device
 *      (IN) res_id    : which template to configure (0..max_template_types-1)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Will not destroy a template type if elements of that template are still
 *      in use.
 */
extern int
shr_template_type_unset(int unit,
                   int res_id);

/*
 *   Function
 *      shr_template_detach
 *   Purpose
 *      Remove all template management for a unit
 *   Parameters
 *      (IN) unit : unit number of the device
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will destroy all of the template pools, then tear down the rest of
 *      the resource management for the unit.
 */
extern int
shr_template_detach(int unit);

/*
 *   Function
 *      shr_template_allocate
 *   Purpose
 *      Find a template to assign to the requested data.
 *   Parameters
 *      (IN) unit     	   : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) flags    	   : flags providing specifics of what/how to set
 *      (IN) data          : data to be in the template
 *      (OUT) is_allocated : where to put 'first alloc' flag
 *      (IN/OUT) template_param  : where to map the resource.
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The template_param argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.
 *
 *      If WITH_ID is set and the specified template is not yet allocated, this
 *      will allocate it and set its data to the specified data.
 *
 *      If WITH_ID is set and the specified template is allocated, this will first
 *      verify that the new data are 'equal' to the existing data for the template,
 *      and: if so, will increment the reference count for the specified template;
 *      if not, will return BCM_E_EXISTS.
 *
 *      If WITH_ID is clear, this will look for a in-use template whose data are
 *      'equal' to the specified data.  If it finds such a template, it will
 *      increment that template's reference count and return it.  If it does not
 *      find such a template, it marks an available template as in use, sets its
 *      reference count to 1, copies the data to the template's data, and returns
 *      this template.  If there is no 'equal' or free template, BCM_E_RESOURCE.
 *
 *      Whether WITH_ID is provided or not, on success the int pointed to by the
 *      is_allocated argument will be updated.  It will be set TRUE if the template
 *      was free before, and FALSE if the template was already in use.  If the
 *      value is TRUE, appropriate resources should be updated by the caller (such
 *      as programming the data values to hardware registers).
 *
 *      It is not valid to specify IGNORE_DATA with this call.
 */
extern int
shr_template_allocate(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int *is_allocated,
                      int *template_param);

/*
 *   Function
 *      shr_template_free
 *   Purpose
 *      Release a template (referrer no longer points to it)
 *   Parameters
 *      (IN) unit     	   : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) template_param: which template to release
 *      (OUT) is_last      : is the template last pointed
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      If the template is not currently in use, BCM_E_NOT_FOUND.
 *
 *      If the template is currently in use, this decrements the reference count,
 *      marking the template as free if the reference count hits zero.
 */
extern int
shr_template_free(int unit,
                  int template_type,
                  int template_param,
                  int *is_last);


/*
 *   Function
 *      shr_template_free_group
 *   Purpose
 *      Release several templates (nof referrers no longer points to it)
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) template_param: which template to release
 *      (IN) nof_deductions: how many referrers to remove.
 *      (OUT) is_last      : is the template last pointed
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      If the template is not currently in use, BCM_E_NOT_FOUND.
 *
 *      If the template is currently in use, this decrements the reference count,
 *      marking the template as free if the reference count hits zero.
 */
extern int
shr_template_free_group(int unit,
                  int template_type,
                  int template_param,
                  int nof_deductions,
                  int *is_last);

/*
 *   Function
 *      shr_template_exchange
 *   Purpose
 *      Free a current template and allocate a new one for new data
 *   Parameters
 *      (IN) unit     	   : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) flags    	   : flags providing specifics of what/how to set
 *      (IN) data          : data to be in the template
 *      (IN) old_template  : is the old template to free
 *      (OUT) is_last      : is the template last pointed
 *      (IN/OUT) template_param  : where to map the resource.
 *      (OUT) is_allocated : is the new template first pointed
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_NOT_FOUND, BCM_E_EXISTS - See notes.
 *      BCM_E_INTERNAL - Internal error.
 *      BCM_E_MEMORY - No memory.
 *      BCM_E_PARAM - Invalid parameters.
 *      BCM_E_* - Other (and also these) errors may be returned from copying the hash key.
 *   Notes
 *      The template_param argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.
 * 
 *      Whether WITH_ID is provided or not, if the allocation / update was successful,
 *      the template with ID provided in old_template will be freed.  Note this only
 *      happens if the selection of a new template succeeds, so the old template
 *      should not be reclaimed if this function fails.
 *      If old_template wasn't allocated before, and the flag IGNORE_NOT_EXIST_OLD_TEMPLATE
 *      is not set, then BCM_E_PARAM will be returned. If the flag is set, then a new refrence
 *      will be allocated without deleting the old one.
 * 
 *      If WITH_ID is set and the template specified in *template_param is not yet allocated,
 *      this will allocate it and set its data to the specified data. 
 * 
 *      If WITH_ID is set but IGNORE_DATA is not, and the specified template in *template_param is
 *      allocated, the data of the template will be changed to the new data.
 *      However if the data already exists with a different index, then BCM_E_EXISTS
 *      is returned (if the user wishes for the same key to be used for two different
 *      indices, it is possible to insert the index itself in the data which will
 *      automatically differentiate them).
 *
 *      If WITH_ID and IGNORE_DATA are both set, this will increment the reference
 *      count for the specified template.
 *      If the template does not exist then BCM_E_NOT_FOUND is returned.
 * 
 *      If WITH_ID is clear, this will look for a in-use template whose data are
 *      'equal' to the specified data.  If it finds such a template, it will
 *      increment that template's reference count and return it.  If it does not
 *      find such a template, it marks an available template as in use, sets its
 *      reference count to 1, copies the data to the template's data, and returns
 *      this template.  If there is no 'equal' or free template, BCM_E_RESOURCE.
 *
 *
 *      Whether WITH_ID is provided or not, on success the int pointed to by the
 *      is_allocated argument will be updated.  It will be set TRUE if the new
 *      template was free before, and FALSE if the new template was already in use.
 *      If the value is TRUE, appropriate resources should be updated by the caller
 *      (such as programming the data values to hardware registers).
 *
 *      It is not valid to specify IGNORE_DATA without WITH_ID.
 */
extern int
shr_template_exchange(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int old_template,
                      int *is_last,
                      int *template_param,
                      int *is_allocated);
        
                      
/*
 *   Function
 *      shr_template_exchange_test
 *   Purpose
 *      Tests if it is possible to free a current template and allocate a new one for new data.
 *   Parameters
 *      (IN) unit     	   : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) flags    	   : flags providing specifics of what/how to set
 *      (IN) data          : data to be in the template
 *      (IN) old_template  : is the old template to free
 *      (OUT) is_last      : is the template last pointed
 *      (IN/OUT) template_param  : where to map the resource.
 *      (OUT) is_allocated : is the new template first pointed
 *   Returns
 *      BCM_E_NONE if Test is successful.
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This function only tests the the following configuration asked can be set,
 *      without space problem.
 *      
 *      The template_param argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.
 *
 *      If WITH_ID is set and the specified template is not yet allocated, this
 *      will allocate it and set its data to the specified data.
 *
 *      If WITH_ID is set but IGNORE_DATA is not, and the specified template is
 *      allocated, this will first verify that the new data are 'equal' to the
 *      existing data for the template, and: if so, will increment the reference
 *      count for the specified template; if not, will return BCM_E_EXISTS.
 *
 *      If WITH_ID and IGNORE_DATA are both set, this will increment the reference
 *      count for the specified template.  Note that this mode is intended only to
 *      be used as a shortcut for 'freeing' the referring entity while mapping its
 *      template back to a 'standard' or 'default' template.
 *
 *      If WITH_ID is clear, this will look for a in-use template whose data are
 *      'equal' to the specified data.  If it finds such a template, it will
 *      increment that template's reference count and return it.  If it does not
 *      find such a template, it marks an available template as in use, sets its
 *      reference count to 1, copies the data to the template's data, and returns
 *      this template.  If there is no 'equal' or free template, BCM_E_RESOURCE.
 *
 *      If the new template is successfully chosen, the template with ID provided
 *      in old_template will be freed.  Note this only happens if the selection of
 *      a new template succeeds, so the old template should not be reclaimed if
 *      this function fails.
 *
 *      Whether WITH_ID is provided or not, on success the int pointed to by the
 *      is_allocated argument will be updated.  It will be set TRUE if the new
 *      template was free before, and FALSE if the new template was already in use.
 *      If the value is TRUE, appropriate resources should be updated by the caller
 *      (such as programming the data values to hardware registers).
 *
 *      It is not valid to specify IGNORE_DATA without WITH_ID.
 */
extern int
shr_template_exchange_test(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int old_template,
                      int *is_last,
                      int *template_param,
                      int *is_allocated);

/*
 *   Function
 *      shr_template_data_get
 *   Purpose
 *      Get the data from a specific template
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) template_param: which template to get
 *      (OUT) data         : where to put the template's data
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_NOT_FOUND if the template was not found.
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
extern int
shr_template_data_get(int unit,
                      int template_type,
                      int template_param,
                      void *data);

/*
 *   Function
 *      shr_template_template_get
 *   Purpose
 *      Get the template from a specific data
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) data         : the template's data
 *      (OUT) template_param : which template
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
int
shr_template_template_get(int unit,
                      int template_type,
                      const void *data,
                      int *template_param);

/*
 *   Function
 *      shr_template_ref_count_get
 *   Purpose
 *      Get the reference counter from a specific template id
 *   Parameters
 *      (IN) unit          : unit number of the device
 *      (IN) template_type : which template type to use
 *      (IN) template_param: which template to get the reference count
 *      (OUT) ref_count    : where to put the template's reference count
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
extern int
shr_template_ref_count_get(int unit,
                      int template_type,
                      int template_param,
                      uint32 *ref_count);


extern int
shr_template_allocate_group(int unit,
                      int template_type,
                      uint32 flags,
                      const void *data,
                      int nof_additions,
                      int *is_allocated,
                      int *template_param);

/*
 *   Function
 *      shr_template_clear
 *   Purpose
 *      Free all templates located in this template_type
 *      (without free memory of the template resource)
 *   Parameters
 *      (IN) unit     	   : unit number of the device
 *      (IN) template_type : which template type to use
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
extern int
shr_template_clear(int unit,
                  int template_type);
                  

/*
 *   Function
 *      shr_template_dump
 *   Purpose
 *      Diagnostic dump of a unit's resource management information
 *   Parameters
 *      (IN) unit      : unit number of the device
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 */
extern int
shr_template_dump(int unit, int template_type);
/* } */
#endif /* defined(BCM_PETRA_SUPPORT) */
/* } */
#endif /* ndef _SHR_RESMGR_H */
