/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    range.c
 * Purpose: Manages RANGE_CHECKER (IFP_RANGE_CHECK)
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>

#include <bcm/range.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm_int/esw/range.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/xgs4.h>
#include <bcm_int/esw_dispatch.h>

#define RANGE_MALLOC(_p_, _t_, _sz_, _desc_)                \
    do {                                                    \
        if (!_p_) {                                         \
            (_p_) = (_t_ *) sal_alloc((_sz_), (_desc_));    \
        }                                                   \
        if (_p_) {                                          \
            sal_memset((_p_), 0, (_sz_));                   \
        }                                                   \
    } while(0)

#define BCM_IF_NULL_RETURN_PARAM(_p_) \
    do {                                                    \
        if ((_p_) == NULL) {                                \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)

/* Global Range control structure pointers */
bcmi_xgs5_range_ctrl_t *range_control[BCM_MAX_NUM_UNITS];

/* STATIC function declarations */
STATIC int bcmi_xgs5_range_ctrl_init(int unit);

STATIC int bcmi_xgs5_range_hw_init(int unit);

STATIC int bcmi_xgs5_range_ctrl_free(int unit, bcmi_xgs5_range_ctrl_t *rangec);

soc_field_t range_group_fields[] =
           {
                IFP_RANGE_CHECK_MASK_0f,
                IFP_RANGE_CHECK_MASK_1f,
                IFP_RANGE_CHECK_MASK_2f,
                IFP_RANGE_CHECK_MASK_3f,
                IFP_RANGE_CHECK_MASK_4f,
                IFP_RANGE_CHECK_MASK_5f,
                IFP_RANGE_CHECK_MASK_6f,
                IFP_RANGE_CHECK_MASK_7f,
                IFP_RANGE_CHECK_MASK_8f,
                IFP_RANGE_CHECK_MASK_9f,
                IFP_RANGE_CHECK_MASK_10f,
                IFP_RANGE_CHECK_MASK_11f,
                IFP_RANGE_CHECK_MASK_12f,
                IFP_RANGE_CHECK_MASK_13f,
                IFP_RANGE_CHECK_MASK_14f,
                IFP_RANGE_CHECK_MASK_15f
           };


/*  Function    :   bcmi_xgs5_range_ctrl_free
 *  Purpose     :   Free Range control structure
 *  Parameters  :   unit - (IN) BCM device number
 *                  rangec  - (IN)  Range control structure.
 *  Returns     :   BCM_E_XXX
 */

STATIC int
bcmi_xgs5_range_ctrl_free(int unit, bcmi_xgs5_range_ctrl_t *rangec)
{
    LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_ctrl_free\n")));
    if (NULL == rangec) {
        /* Already freed */
        return BCM_E_NONE;
    }

    /* Free mutex */
    if (NULL != rangec->range_mutex) {
        sal_mutex_destroy(rangec->range_mutex);
        rangec->range_mutex = NULL;
    }

    /* Free the range control structure */
    sal_free(rangec);

    range_control[unit] = NULL;

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_ctrl_init
 *  Purpose     :   Allocate and Initialise range control structure.
 *  Parameters  :   unit     - (IN)     BCM unit
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_ctrl_init(int unit)
{
    int                     alloc_sz = 0;           /* Allocation size */
    soc_mem_t               range_mem = INVALIDm;   /* Range Checker Memory ID */
    bcmi_xgs5_range_ctrl_t  *range_ctrl = NULL;     /* Range Control Structure */

    range_mem = IFP_RANGE_CHECKm;

    if (range_control[unit] != NULL) {
        bcmi_xgs5_range_detach(unit);
    }

    if (range_control[unit] != NULL) {
        (void)bcmi_xgs5_range_ctrl_free(unit, range_control[unit]);
    }

    /* Allocating memory of Range control Structure */
    alloc_sz = sizeof(bcmi_xgs5_range_ctrl_t);
    RANGE_MALLOC(range_ctrl, bcmi_xgs5_range_ctrl_t, alloc_sz, "range control");
    if(range_ctrl == NULL) {
        LOG_ERROR(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "Range Module(unit %d) Error: Module alloc failed.\n"),
                 unit));
        return BCM_E_MEMORY;
    }

    range_control[unit] = range_ctrl;

    /* Create Range Mutex*/
    if(range_ctrl->range_mutex == NULL) {
        range_ctrl->range_mutex = sal_mutex_create("range_mutex");
        if (range_ctrl->range_mutex == NULL) {
            /*Free range ctrl structure. */
            (void)bcmi_xgs5_range_ctrl_free(unit, range_ctrl);
            LOG_ERROR(BSL_LS_BCM_RANGE,
                    (BSL_META_U(unit,
                                "Range Module(unit %d) Error: Mutex alloc failed.\n"),
                     unit));
            return BCM_E_MEMORY;
        }
    }

    range_ctrl->range_mem = range_mem;

    range_ctrl->max_ranges = _RANGE_ID_MAX(unit);

    range_ctrl->max_groups = _RANGE_GROUP_MAX(unit);

    range_ctrl->range_used_by_module = RANGE_USED_BY_FIELD_MODULE;

    if (soc_feature(unit, soc_feature_range_check_group_support)) {
        range_ctrl->range_used_by_module = RANGE_USED_BY_RANGE_MODULE;
    }

    sal_memset(range_ctrl->range_group_used_bitmap, 0,
               (sizeof(SHR_BITDCL) * SOC_MAX_NUM_PIPES));

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_hw_init
 *  Purpose     :   Clear hardware Range Checker memory
 *  Parameters  :   unit - (IN) BCM device number.
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_hw_init(int unit)
{
    int rv;         /* Operation return status. */

    /* Clear RANGE_CHECK mem*/
    rv = soc_mem_clear(unit, RANGE_CTRL(unit)->range_mem, MEM_BLOCK_ALL, TRUE);
    SOC_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

#if defined (BCM_WARM_BOOT_SUPPORT)
#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_1_1     SOC_SCACHE_VERSION(1, 1)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_1_1

/*  Function    :   bcmi_xgs5_range_wb_scache_size_get_1_0
 *  Purpose     :   Returns required scache size in bytes for version = 1_0
 *  Parameters  :   unit      - (IN) BCM device number.
 *                  req_scache_size -(OUT) Required scache size
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_wb_scache_size_get_1_0(int unit, int *req_scache_size)
{
    int         alloc_size = 0;     /* Allocation size */
    _range_t    range;              /* Range structure */

    bcmi_xgs5_range_ctrl_t *range_ctrl;

    range_ctrl = RANGE_CTRL(unit);

    /* The following fields from the Range Control Structure needs to
     be synced for recovery after Warmboot:
     1. range_oper_mode - to know whether the module was operating in
                          Per Pipe/ Global Mode to read appropriate
                          Memory.
     2. num_ranges      - Number of Ranges existing in the System.
     3. range_used_by_module - Whether Range resource is being used by
                          Field Module or Range Module.*/
    alloc_size += sizeof(range_ctrl->range_oper_mode);
    alloc_size += sizeof(range_ctrl->num_ranges);
    alloc_size += sizeof(range_ctrl->range_used_by_module);

    /* The following fields from the Range Structure needs to
       be synced for recovery :
     1. Range Id,
     2. Range Offset,
     3. Range Width,
     4. UDF id mapped with the Range,
     5. pipe_instance to which this range is mapped to.
     Rest all fields in the range structure can be recovered from the Hardware.
     */

    alloc_size += RANGE_CTRL(unit)->max_ranges * sizeof(range.rid);
    alloc_size += RANGE_CTRL(unit)->max_ranges * sizeof(range.offset);
    alloc_size += RANGE_CTRL(unit)->max_ranges * sizeof(range.width);
    alloc_size += RANGE_CTRL(unit)->max_ranges * sizeof(range.udf_id);
    alloc_size += RANGE_CTRL(unit)->max_ranges * sizeof(range.pipe_instance);

    *req_scache_size = alloc_size;

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_wb_scache_size_get_1_1
 *  Purpose     :   Returns required scache size in bytes for version = 1_1
 *  Parameters  :   unit      - (IN) BCM device number.
 *                  req_scache_size -(OUT) Required scache size
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_wb_scache_size_get_1_1(int unit, int *req_scache_size)
{

    bcmi_xgs5_range_ctrl_t *range_ctrl;

    range_ctrl = RANGE_CTRL(unit);

    /* The following fields from the Range Control Structure needs to
     be synced for recovery after Warmboot:
     range_group_used_bitmap - Used HW masks of the range group */

    *req_scache_size += sizeof(range_ctrl->range_group_used_bitmap);

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_wb_scache_size_get
 *  Purpose     :   Returns required scache size
 *  Parameters  :   unit      - (IN) BCM device number.
 *                  req_scache_size -(OUT) Required scache size
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_wb_scache_size_get(int unit, int *req_scache_size)
{
    BCM_IF_ERROR_RETURN(
            bcmi_xgs5_range_wb_scache_size_get_1_0(unit, req_scache_size));
    BCM_IF_ERROR_RETURN(
            bcmi_xgs5_range_wb_scache_size_get_1_1(unit, req_scache_size));

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_wb_alloc
 *  Purpose     :   Allocates required scache size for the Range module recovery
 *  Paramters   :   unit      - (IN) BCM device number.
 *  Returns     :   BCM_E_XXX
 */
STATIC int bcmi_xgs5_range_wb_alloc(int unit)
{
    int         rv = BCM_E_NONE;    /* Operation return status. */
    int         req_scache_size;    /* Required Scache Size.    */
    uint8       *scache_ptr;        /* Scache ptr.              */
    soc_scache_handle_t scache_handle;  /* Scache handle.       */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_RANGE, 0);

    /*Obtain the required size for scache sync*/
    rv = bcmi_xgs5_range_wb_scache_size_get(unit, &req_scache_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
            req_scache_size, &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    return rv;
}

/*  Function    :   bcmi_xgs5_range_wb_reinit_1_0
 *  Purpose     :   Recovers Range warmboot state from scache
 *  Parameters  :   unit            - (IN)  Unit number.
 *                  scache_ptr      - (IN/OUT)  Pointer to scache
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_wb_reinit_1_0(int unit, uint8 **scache_ptr)
{
    uint8           *scache_p;          /* Pointer to scache */
    soc_mem_t       mem = RANGE_CTRL(unit)->range_mem;  /* Range Checker Memory ID */
    int             idx;                /* Range iteration index */
    int             index_min;          /* Range memory minimum index */
    int             index_max;          /* Range memory maximum index */
    uint32          enable;             /* Whether the entry is valid or not.*/
    uint32          type;               /* Range checker type. */
    uint32          tbl_entry[SOC_MAX_MEM_FIELD_WORDS];  /* Range checker entry buffer. */
    _range_t        *range_p;           /* Pointer to a range. */
    _range_t        **last_range_p;     /* Pointer to the last range reinit. */
    bcm_port_config_t   pc;             /* Port configuration.      */
    int             rv;                 /* Operation return value */

    /* Initialize port configuration structure. */
    bcm_port_config_t_init(&pc);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &pc);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    scache_p = (uint8 *)(*scache_ptr);

    /* Range module oper mode */
    RANGE_CTRL(unit)->range_oper_mode = *scache_p;
    scache_p++;

    /* Number of Ranges created */
    RANGE_CTRL(unit)->num_ranges = *scache_p;
    scache_p++;

    /* Range resource is being used by which module */
    RANGE_CTRL(unit)->range_used_by_module = *scache_p;
    scache_p++;

    last_range_p = &(RANGE_CTRL(unit)->ranges);
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    for (idx = index_min; idx <= index_max; idx ++) {
        sal_memset(tbl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tbl_entry);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        soc_mem_field_get(unit, mem, tbl_entry, ENABLEf, &enable);
        if (enable == 0) {
            continue;
        }

        /* Create a new range entry for the list */
        if ((range_p = sal_alloc(sizeof (*range_p), "range")) == NULL) {
            return (BCM_E_MEMORY);
        }
        sal_memset(range_p, 0, sizeof(_range_t));

        /* Copy Pipe instance */
        range_p->pipe_instance = *scache_p;
        scache_p++;

        /* Copy the ports bitmap used by the range. */
        range_p->ports = pc.all;

        if(RANGE_CTRL(unit)->range_oper_mode == bcmRangeOperModePipeLocal) {
            /*Fetch the memory based on the pipe instance. */
            if (range_p->pipe_instance < SOC_MAX_NUM_PIPES) {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                    IFP_RANGE_CHECKm,
                                    range_p->pipe_instance,
                                    &mem));
            } else {
                sal_free(range_p);
                return BCM_E_INTERNAL;
            }

            /*In Perpipe mode, only that particular pipe's PBMP will be used*/
            range_p->ports = pc.per_pipe[range_p->pipe_instance];
        }

        /* Populate the structure */
        /* Get the Range Id */
        sal_memcpy(&(range_p->rid), scache_p, sizeof(uint32));
        scache_p += sizeof(uint32);

        /* Get the UDF Id mapped to the range */
        sal_memcpy(&(range_p->udf_id), scache_p, sizeof(bcm_udf_id_t));
        scache_p += sizeof(bcm_udf_id_t);

        /* Get the UDF offset */
        range_p->offset = *scache_p;
        scache_p++;

        /* Get the UDF width */
        range_p->width = *scache_p;
        scache_p++;

        /* Get the hw_index the range maps to */
        range_p->hw_index = idx;

        /* Read the min value from the hw */
        soc_mem_field_get(unit, mem, tbl_entry, LOWER_BOUNDSf,
                (uint32 *)&(range_p->min));

        /* Read the max value from the hw */
        soc_mem_field_get(unit, mem, tbl_entry, UPPER_BOUNDSf,
                (uint32 *)&(range_p->max));

        /* Read the Range Checker Type from the hw*/
        if ((SOC_MEM_FIELD_VALID(unit, mem, FIELD_SELECTf))) {
            soc_mem_field_get(unit, mem, tbl_entry, FIELD_SELECTf, (uint32 *)&(range_p->rtype));
        } else {
            soc_mem_field_get(unit, mem, tbl_entry,
                    SOURCE_DESTINATION_SELECTf, &type);
            if (type == 0) {
                range_p->rtype = bcmRangeTypeL4SrcPort;
            } else if (type == 1) {
                range_p->rtype = bcmRangeTypeL4DstPort;
            }
        }

        *last_range_p = range_p;
        last_range_p = &range_p->next;
    }

    /* fast forward the parameter pointer to where this function finished */
    *scache_ptr = scache_p;

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_wb_reinit_1_1
 *  Purpose     :   Recovers Range warmboot state from scache
 *  Parameters  :   unit            - (IN)  Unit number.
 *                  scache_ptr      - (IN/OUT)  Pointer to scache
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_wb_reinit_1_1(int unit, uint8 **scache_ptr)
{
    uint8       *scache_p;  /* Scache pointer */

    scache_p = (uint8 *)(*scache_ptr);
    sal_memcpy(&(RANGE_CTRL(unit)->range_group_used_bitmap),
               scache_p,
               (sizeof(SHR_BITDCL) * SOC_MAX_NUM_PIPES));
    scache_p++;

    /* fast forward the parameter pointer to where this function finished */
    *scache_ptr = scache_p;

    return BCM_E_NONE;

}

/*  Function    :   bcmi_xgs5_range_wb_sync_1_0
 *  Purpose     :   Sync Range module info into scache for version = 1_0
 *  Parameters  :   unit            - (IN)  Unit number
 *                  scache_ptr      - (IN/OUT)  Pointer to scache
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_wb_sync_1_0(int unit, uint8 **scache_ptr)
{
    uint8       *scache_p;  /* Scache pointer */
    _range_t    *range_p;   /* Pointer to a range */

    scache_p = (uint8 *)(*scache_ptr);

    /* Sync the Range Module Oper Mode */
    *scache_p = RANGE_CTRL(unit)->range_oper_mode;
    scache_p++;

    /* Sync the number of Ranges in the system */
    *scache_p = RANGE_CTRL(unit)->num_ranges;
    scache_p++;

    /* Sync the information on which module uses the Range resources */
    *scache_p = RANGE_CTRL(unit)->range_used_by_module;
    scache_p++;

    range_p = RANGE_CTRL(unit)->ranges;

    while(range_p) {
        /* The following fields from the Range Structure needs to
           be synced for recovery :
           1. pipe_instance to which this range is mapped to.
           2. Range Id,
           3. UDF id mapped with the Range,
           4. Range Offset,
           5. Range Width,
           Rest all fields in the range structure can be recovered from the Hardware.
         */
        *scache_p = range_p->pipe_instance;
        scache_p++;

        sal_memcpy(scache_p, &(range_p->rid), sizeof(uint32));
        scache_p += sizeof(uint32);

        sal_memcpy(scache_p, &(range_p->udf_id), sizeof(bcm_udf_id_t));
        scache_p += sizeof(bcm_udf_id_t);

        *scache_p = range_p->offset;
        scache_p++;

        *scache_p = range_p->width;
        scache_p++;

        range_p = range_p->next;
    }

    /* fast forward the parameter pointer to where this function finished */
    *scache_ptr = scache_p;

    return BCM_E_NONE;
}


/*  Function    :   bcmi_xgs5_range_wb_sync_1_1
 *  Purpose     :   Sync Range module info into scache for version = 1_0
 *  Parameters  :   unit            - (IN)  Unit number
 *                  scache_ptr      - (IN/OUT)  Pointer to scache
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_wb_sync_1_1(int unit, uint8 **scache_ptr)
{
    uint8       *scache_p;  /* Scache pointer */

    scache_p = (uint8 *)(*scache_ptr);
    sal_memcpy(scache_p,
               &(RANGE_CTRL(unit)->range_group_used_bitmap),
               (sizeof(SHR_BITDCL) * SOC_MAX_NUM_PIPES));
    scache_p++;

    /* fast forward the parameter pointer to where this function finished */
    *scache_ptr = scache_p;

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_wb_sync
 *  Purpose     :   Sync Range module warmboot state to scache
 *  Parameters  :   unit            - (IN)  Unit number.
 *  Returns     :   BCM_E_XXX
 */
int
bcmi_xgs5_range_wb_sync(int unit)
{
    uint8               *scache_ptr;    /* Scache pointer */
    soc_scache_handle_t scache_handle;  /* Scache handle */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_RANGE, 0);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));

    BCM_IF_ERROR_RETURN(bcmi_xgs5_range_wb_sync_1_0(unit, &scache_ptr));
    BCM_IF_ERROR_RETURN(bcmi_xgs5_range_wb_sync_1_1(unit, &scache_ptr));
    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_reinit
 *  Purpose     :   Recovers Range module warmboot state from scache
 *  Parameters  :   unit            - (IN)  Unit number.
 *  Returns     :   BCM_E_XXX
 */
STATIC int
bcmi_xgs5_range_reinit(int unit)
{
    int                 rv = BCM_E_INTERNAL;    /* Operation return value */
    uint8               *scache_ptr;            /* Scache pointer         */
    uint16              recovered_ver = 0;      /* Recovered warmboot version */
    soc_scache_handle_t scache_handle;          /* Scache handle */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_RANGE, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    } else if (BCM_E_NONE == rv) {
        if (recovered_ver >= BCM_WB_VERSION_1_0) {
            BCM_IF_ERROR_RETURN(bcmi_xgs5_range_wb_reinit_1_0(unit, &scache_ptr));
        }
        if (recovered_ver >= BCM_WB_VERSION_1_1) {
            BCM_IF_ERROR_RETURN(bcmi_xgs5_range_wb_reinit_1_1(unit, &scache_ptr));
        }
    }

    return rv;
}

#endif /*BCM_WARM_BOOT_SUPPORT*/

/* Function : bcmi_xgs5_range_init
 *
 * Purpose  : Initialize range module
 *
 * Parameters : unit - (IN) Unit number.
 *
 * Returns:
 *          BCM_E_NONE     : Range Module initialized successfully.
 *          BCM_E_UNIT     : Invalid BCM Unit number.
 *          BCM_E_XXX      : Standard error code.
 */
int
bcmi_xgs5_range_init( int unit )
{
    int rv;     /* Operation return value */

    /* Init control structures */
    rv = bcmi_xgs5_range_ctrl_init(unit);
    if (BCM_E_NONE != rv) {
        LOG_ERROR(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "Range(unit %d) Error: Module ctrl init failed.\n"),
                 unit));
    }
    BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        rv = bcmi_xgs5_range_reinit(unit);
        if (BCM_E_NONE != rv) {
            LOG_ERROR(BSL_LS_BCM_RANGE,
                    (BSL_META_U(unit,
                                "Range(unit %d) Error: Module reinit failed.\n"),
                    unit));
        }
        BCM_IF_ERROR_RETURN(rv);
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        rv = bcmi_xgs5_range_hw_init(unit);
        if (BCM_E_NONE != rv) {
             LOG_ERROR(BSL_LS_BCM_RANGE,
                    (BSL_META_U(unit,
                                "Range(unit %d) Error: Module hw init failed.\n"),
                     unit));
        }
        BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_WARM_BOOT_SUPPORT)
        rv = bcmi_xgs5_range_wb_alloc(unit);
        if (BCM_E_NONE != rv) {
            LOG_ERROR(BSL_LS_BCM_RANGE,
                    (BSL_META_U(unit,
                                "Range(unit %d) Error: Module warmboot alloc failed.\n"),
                     unit));
        }
        BCM_IF_ERROR_RETURN(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
    }

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_detach
 *  Purpose     :   Detaches Range module and cleans software state
 *  Parameters  :   unit            - (IN)      Unit number
 *  Returns     :   BCM_E_XXX
 */
int
bcmi_xgs5_range_detach(int unit)
{
    RANGE_INIT_CHECK(unit);

    /* Free resources related to Range module */
    BCM_IF_ERROR_RETURN(bcmi_xgs5_range_ctrl_free(unit, RANGE_CTRL(unit)));

    return BCM_E_NONE;
}

/*  Function    :   bcmi_range_id_generate
 *  Purpose     :   Find an unused range ID.
 *  Parameters  :   unit            - (IN)      Unit number.
 *                  rid             - (OUT)     Range ID.
 *  Returns     :   BCM_E_XXX
 */
int bcmi_range_id_generate(
        int unit,
        bcm_range_t *rid)
{
    bcm_range_config_t range_config; /* Range config structure */

    *rid = _RANGE_ID_BASE;

    memset(&range_config, 0, sizeof(range_config));
    while(*rid <= RANGE_CTRL(unit)->max_ranges) {
        range_config.rid = *rid;

        if(BCM_E_NONE != (bcmi_xgs5_range_get(unit, &range_config)))
        {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
                    (BSL_META_U(unit,
                                "bcmi_range_id_generate: Generated Rid = %d \n"), range_config.rid));
            return BCM_E_NONE;
        }
        else {
            (*rid)++;
        }
    }

    if (*rid > RANGE_CTRL(unit)->max_ranges) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_range_id_generate: All ranges are used up\n")));
        return BCM_E_RESOURCE;
    }

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_create
 *  Purpose     :   Create a Range object
 *  Parameters  :   unit - (IN) Unit number.
 *                  flags   - (IN) Flags to create a Range
 *                  range_config   - (IN) Range config structure.
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_create(
        int unit,
        int flags,
        bcm_range_config_t *range_config)
{
    int                 rv;             /* Operation return value */
    uint32              max_field_val;  /* Max value that the field in memory can hold */
    SHR_BITDCL          *range_bmap;    /* Used indexes map.        */
    int                 hw_index  = -1; /* Free/matching rang index.*/
    int                 idx_max;        /* Loop termination index.  */
    _range_t            *tmp = NULL;    /* Range pointer */
    soc_mem_t           mem = RANGE_CTRL(unit)->range_mem; /* Range Checker Memory ID */
    bcmi_xgs4_udf_offset_info_t           *udf_offset_info;/* UDF offset info */
    uint16              udf_mask = ~0;  /* UDF mask */
    bcm_range_oper_mode_t   oper_mode = 0;  /*Range module operational mode */
    int pipe_instance = 0;                   /* Pipe instance */


    /* Input parameters check */
    BCM_IF_NULL_RETURN_PARAM(range_config);

    if (RANGE_CTRL(unit)->range_used_by_module == RANGE_USED_BY_FIELD_MODULE) {
        LOG_CLI((BSL_META("Range resource is used by FIELD module. \n")));
        return BCM_E_UNAVAIL;
    }

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    LOG_DEBUG(BSL_LS_BCM_RANGE,
            (BSL_META_U(unit,
                        "bcmi_xgs5_range_create: Error fetching Range Oper Mode \n")));
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_xgs5_range_validate_port_config (unit, oper_mode, range_config->ports, &pipe_instance) ;
    LOG_DEBUG(BSL_LS_BCM_RANGE,
            (BSL_META_U(unit,
                        "bcmi_xgs5_range_create: Error Validating Port Config \n")));
    BCM_IF_ERROR_RETURN (rv);

    max_field_val = (1 << soc_mem_field_length(unit, mem, LOWER_BOUNDSf)) - 1 ;
    if(range_config->min > max_field_val) {
        return (BCM_E_PARAM);
    }

    max_field_val = (1 << soc_mem_field_length(unit, mem, UPPER_BOUNDSf)) - 1 ;
    if(range_config->max > max_field_val) {
        return (BCM_E_PARAM);
    }

    if((range_config->rtype < bcmRangeTypeL4SrcPort)
            || (range_config->rtype > bcmRangeTypeUdf)) {
        return (BCM_E_PARAM);
    }

    if (range_config->rtype == bcmRangeTypeUdf) {
        if (soc_feature(unit, soc_feature_udf_td3x_support)) {
           uint8 num_chunks;
           uint32 chunk_bmap;
           bcm_udf_chunk_info_t udf_chunk_info;

           rv = bcm_esw_udf_chunk_info_get(unit, range_config->udf_id, &udf_chunk_info);
           BCM_IF_ERROR_RETURN(rv);

           rv = bcm_esw_udf_range_checker_chunk_info_get(unit, &num_chunks, &chunk_bmap);
           BCM_IF_ERROR_RETURN(rv);

           if (udf_chunk_info.chunk_bmap != chunk_bmap) {
              return BCM_E_PARAM;
           }
        } else if (soc_feature(unit, soc_feature_udf_support)) {
           /* Check UDF chunk id */
           memset(&udf_offset_info, 0, sizeof(udf_offset_info));

           rv = bcmi_xgs4_udf_offset_node_get(unit, range_config->udf_id, &udf_offset_info);
           LOG_DEBUG(BSL_LS_BCM_RANGE,
                   (BSL_META_U(unit,
                               "bcmi_xgs5_range_create: Error fetching Udf Offset Node \n")));
           BCM_IF_ERROR_RETURN(rv);

           if(!(udf_offset_info->flags & BCMI_XGS4_UDF_OFFSET_RANGE_CHECK)) {
               /* UDF should be created with Range Check hints */
               return (BCM_E_PARAM);
           }

           /* UDF1 Chunk 2: (47 - 32 bits) which is UDF_OFFSET5*/
           if(udf_offset_info->hw_bmap != 0x20) {
               return BCM_E_PARAM;
           }
        } else {
           return BCM_E_PARAM;
        }
    }

    idx_max = soc_mem_index_max(unit, mem);
    range_bmap = NULL;

    RANGE_MALLOC(range_bmap, SHR_BITDCL, SHR_BITALLOCSIZE(idx_max + 1), "Valid Ranges");
    if (NULL == range_bmap) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                        "bcmi_xgs5_range_create: Error Allocating memory for range_bmap \n")));
        return (BCM_E_MEMORY);
    }

    /* Search existing ranges */
    if (flags & BCM_RANGE_CREATE_WITH_ID) {
        if ((soc_feature(unit, soc_feature_th3_style_fp)) &&
              (range_config->rid > RANGE_CTRL(unit)->max_ranges)) {
              sal_free(range_bmap);
              return (BCM_E_PARAM);
        }
        for (tmp = RANGE_CTRL(unit)->ranges; tmp != NULL; tmp = tmp->next) {
            if(tmp->rid == range_config->rid) {
                LOG_DEBUG(BSL_LS_BCM_RANGE,
                    (BSL_META_U(unit,
                            "bcmi_xgs5_range_create: Duplicate Range Id \n")));
                sal_free(range_bmap);
                return BCM_E_EXISTS;
            }
        }
    } else {
        rv = bcmi_range_id_generate(unit, &(range_config->rid));
        if (BCM_FAILURE(rv)) {
            sal_free(range_bmap);
            return rv;
        }
    }

    /* Search existing ranges */
    for (tmp = RANGE_CTRL(unit)->ranges; tmp != NULL; tmp = tmp->next) {
        SHR_BITSET(range_bmap, tmp->hw_index);
        /* Found an exisiting match so use it. */
        if (tmp->rtype == range_config->rtype && tmp->min == range_config->min
                && tmp->max == range_config->max) {
            hw_index = tmp->hw_index;
        }
    }

    /* If no match found, allocate a new hardware index. */
    if (hw_index < 0) {
        hw_index = soc_mem_index_min(unit, RANGE_CTRL(unit)->range_mem);
        for (; hw_index <= idx_max; hw_index++) {
            if (0 == SHR_BITGET(range_bmap, hw_index)) {
                break;
            }
        }
        /* No hardware indexes left. */
        if (hw_index == (idx_max + 1)) {
            sal_free(range_bmap);
            return (BCM_E_RESOURCE);
        }
    }

    /* Create a new range entry for the list */
    if ((tmp = sal_alloc(sizeof(*tmp), "Range")) == NULL) {
        sal_free(range_bmap);
        return (BCM_E_MEMORY);
    }


    memset(tmp, 0, sizeof(_range_t));
    udf_mask <<= (range_config->width);
    udf_mask = ~udf_mask;
    udf_mask <<= range_config->offset;

    tmp->rid = range_config->rid;
    tmp->hw_index = hw_index;
    tmp->min = range_config->min;
    tmp->max = range_config->max;
    tmp->rtype  = range_config->rtype;
    tmp->offset = range_config->offset;
    tmp->width = range_config->width;
    tmp->udf_id = range_config->udf_id;

    if (oper_mode == bcmRangeOperModeGlobal) {
        tmp->ports = PBMP_ALL(unit);
    } else if (oper_mode == bcmRangeOperModePipeLocal) {
        tmp->ports = range_config->ports;
    }

    tmp->pipe_instance = pipe_instance;

    rv = bcmi_range_check_set(unit, tmp, hw_index, 1, udf_mask);

    if (BCM_FAILURE(rv)) {
        sal_free(range_bmap);
        sal_free(tmp);
        return rv;
    }

    /* Add to list of range checkers. */
    {
        _range_t *p, **pp;
        for (pp = &RANGE_CTRL(unit)->ranges; (p = *pp) && tmp->hw_index > p->hw_index; pp = &p->next);
        (*pp = tmp)->next = p;
    }

    LOG_DEBUG(BSL_LS_BCM_RANGE,
            (BSL_META_U(unit,
                        "bcmi_xgs5_range_create: Range Create Successful \n")));

    sal_free(range_bmap);
    return rv;

}

/*  Function    :   bcmi_xgs5_range_get
 *  Purpose     :   Retrieve range info given the range ID
 *  Parameters  :   unit            - (IN)      Unit number.
 *                  range_config    - (OUT)     range config structure
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_get(
        int unit,
        bcm_range_config_t *range_config)
{
    int rv;     /* Operation return value */

    /* Input parameters check */
    BCM_IF_NULL_RETURN_PARAM(range_config);

    rv = bcmi_xgs5_range_node_get(unit, range_config);

    return rv;
}

/*  Function    :   bcmi_xgs5_range_destroy
 *  Purpose     :   To destroy range object
 *  Parameters  :   unit            - (IN)      Unit number
 *                  rid             - (IN)      Range Id to be destroyed
 */
int bcmi_xgs5_range_destroy(
        int unit,
        bcm_range_t rid)
{
    int         rv;         /* Operation return value */
    _range_t    *cur = NULL;    /* Ptr to Current range being traversed */
    _range_t    *prev = NULL;   /* Ptr to Previous range traversed */
    _range_t    range_tmp;      /* Range structure */

    memset(&range_tmp, 0, sizeof(range_tmp));

    if (RANGE_CTRL(unit)->range_used_by_module == RANGE_USED_BY_FIELD_MODULE) {
        LOG_CLI((BSL_META("Range resource is used by FIELD module. \n")));
        return BCM_E_UNAVAIL;
    }

    for (cur = RANGE_CTRL(unit)->ranges; cur != NULL; cur = cur->next) {
        if(cur->rid == rid) {
            /* Found the node. */
            break;
        }
        prev = cur;
    }

    /* Matching node not found */
    if (cur == NULL) {
        return BCM_E_NOT_FOUND;
    }

    range_tmp.rid       = cur->rid;
    range_tmp.pipe_instance = cur->pipe_instance;
    range_tmp.hw_index = cur->hw_index;

    rv = bcmi_range_check_set(unit, &range_tmp, range_tmp.hw_index, 0, 0);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free the cur node */
    if (prev == NULL) {
        RANGE_CTRL(unit)->ranges = cur->next;
    } else {
        prev->next = cur->next;
    }
    sal_free(cur);

    RANGE_CTRL(unit)->num_ranges -= 1;

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_node_get
 *  Purpose     :   Retrieve range node given the range id
 *  Parameters  :   unit            - (IN)      Unit number
 *                  range_config    - (IN)      Range config structure
 *  Returns     :   BCM_E_XXX
 */
int
bcmi_xgs5_range_node_get(
        int unit,
        bcm_range_config_t *range_config)
{
    _range_t                *tmp = NULL;        /* Range structure. */
    int                     rv   = BCM_E_NONE;  /* Operation return value. */

    tmp = RANGE_CTRL(unit)->ranges;

    while (tmp != NULL) {
        if(tmp->rid == range_config->rid) {
            /* Found the node. */
            break;
        }
        tmp = tmp->next;
    }

    if (tmp == NULL) {
        rv = BCM_E_NOT_FOUND;
    } else {
        range_config->rtype     = tmp->rtype;
        range_config->min       = tmp->min;
        range_config->max       = tmp->max;
        range_config->offset    = tmp->offset;
        range_config->width     = tmp->width;
        range_config->udf_id    = tmp->udf_id;
        range_config->ports     = tmp->ports;
    }

    return rv;

}

/*  Function    :   bcmi_range_check_set
 *  Purpose     :   Write the range check parameters into the memory
 *  Parameters  :   unit    - (IN) BCM unit number
 *                  range   - (IN) range structure
 *                  hw_index - (IN) Range hw index
 *                  enable  - (IN) True or False
 *                  udf_mask - (IN) Udf mask
 *  Returns     :   BCM_E_XXX
 */
int
bcmi_range_check_set(
        int unit,
        _range_t *range,
        int     hw_index,
        int enable,
        uint16 udf_mask)
{
    int                     rv;                         /* Operation return value */
    soc_mem_t               range_check_mem = INVALIDm; /* Range Checker Memory ID */
    uint32                  tbl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Range checker entry buffer. */
    bcm_range_oper_mode_t   oper_mode = 0;              /* Range module operational mode */
    uint32                  mask = udf_mask;

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the memory to which the configurations need to be written to. */
    if (oper_mode == bcmRangeOperModeGlobal) {
        range_check_mem = RANGE_CTRL(unit)->range_mem;
    } else if(oper_mode == bcmRangeOperModePipeLocal) {
        if (range->pipe_instance < SOC_MAX_NUM_PIPES) {
            BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                IFP_RANGE_CHECKm,
                                range->pipe_instance,
                                &range_check_mem));
        } else {
            return BCM_E_INTERNAL;
        }

    }

    if (INVALIDm == range_check_mem) {
        return (BCM_E_UNAVAIL);
    }

    /* Index sanity check. */
    if (!soc_mem_index_valid(unit, range_check_mem, hw_index)) {
        return (BCM_E_PARAM);
    }

    /* write range check memory */
    sal_memset(tbl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Set source/destination port selection. */
    soc_mem_field_set(unit, range_check_mem, tbl_entry,
            FIELD_SELECTf, &(range->rtype));

    if (SOC_MEM_FIELD_VALID(unit, range_check_mem, ENABLEf)) {
        soc_mem_field_set(unit, range_check_mem, tbl_entry,
                ENABLEf, (uint32 *) &enable);
    }

    /* Set range min value. */
    soc_mem_field_set(unit, range_check_mem, tbl_entry,
            LOWER_BOUNDSf, (uint32 *) &(range->min));

    /* Set range max value. */
    soc_mem_field_set(unit, range_check_mem, tbl_entry,
            UPPER_BOUNDSf, (uint32 *) &(range->max));

    if(range->rtype == bcmRangeTypeUdf) {
        soc_mem_field_set(unit, range_check_mem, tbl_entry,
                UDF_MASKf, (uint32 *) &(mask));
    }

    /* Write entry back to hw. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, range_check_mem,
                MEM_BLOCK_ALL, hw_index, tbl_entry));

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_range_hw_clear
 *  Purpose     :   Clear hardware
 *  Parameters  :   unit    -   (IN) BCM unit number
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_hw_clear(
        int unit)
{
    /* DO NOT CLEAR HW when in warmboot mode. */
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(soc_mem_clear(unit, RANGE_CTRL(unit)->range_mem,
                COPYNO_ALL, TRUE));

    return BCM_E_NONE;
}

/*
 * Function     :   bcmi_xgs5_range_oper_mode_set
 * Purpose      :   Configure Range Module Operational Mode
 * Parameters   :   unit        - (IN) BCM Device number.
 *                  oper_mode   - (IN) Range Operational Mode enum value.
 * Returns      :   BCM_E_XXX
 */
int bcmi_xgs5_range_oper_mode_set(
        int unit,
        bcm_range_oper_mode_t oper_mode)
{
    /* Validate input parameters */
    if ((oper_mode != bcmRangeOperModeGlobal) &&
        (oper_mode != bcmRangeOperModePipeLocal)){
        return BCM_E_PARAM;
    }

    /* Range Module Init Check */
    RANGE_INIT_CHECK(unit);

    /* Verify whether mode is already set */
    if (RANGE_CTRL(unit)->range_oper_mode == oper_mode) {
        return BCM_E_NONE;
    }

    /* Range Module Lock */
    RANGE_LOCK(unit);

    if (RANGE_CTRL(unit)->ranges) {
        /*Cannot set opermode when there are already existing ranges*/
        return BCM_E_BUSY;
    }

    /* Hw_clear*/
    BCM_IF_ERROR_RETURN(bcmi_xgs5_range_hw_clear(unit));

    /* Set Oper mode */
    RANGE_CTRL(unit)->range_oper_mode = oper_mode;

    /* Unlock Range Module */
    RANGE_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function     :  bcmi_xgs5_range_oper_mode_get
 * Purpose      :  Retrieve Range Module Operational Mode
 * Parameters   :  unit    - (IN)  BCM Device number.
 *                 mode    - (OUT) Reference to Range Operational Mode value.
 * Returns      :  BCM_E_XXX
 */
int bcmi_xgs5_range_oper_mode_get(
        int unit,
        bcm_range_oper_mode_t *oper_mode)
{
    /* Validate Input parameters */
    if (oper_mode == NULL) {
        return BCM_E_PARAM;
    }

    /* Range Module Init Check */
    RANGE_INIT_CHECK(unit);

    /* Fetch the mode */
    *oper_mode = RANGE_CTRL(unit)->range_oper_mode;

    return BCM_E_NONE;
}

/*
 * Function     :   bcmi_xgs5_range_traverse
 * Purpose      :   Traverse all the ranges in the system, calling a specified
 *                  callback for each one
 * Parameters   :   unit - (IN) Unit number.
 *                  callback - (IN) A pointer to the callback function to call for each range
 *                  user_data - (IN) Pointer to user data to supply in the callback
 * Returns      :   BCM_E_XXX
 */
int bcmi_xgs5_range_traverse(
        int unit,
        bcm_range_traverse_cb callback,
        void *user_data)
{
    bcm_range_config_t    range_config;     /* Range config structure */
    int             rv = BCM_E_NONE;        /* Operation return value */
    _range_t        *tmp = NULL;            /* Range structure        */

    /* Validate Input Parameters */
    if (NULL == callback) {
        return BCM_E_PARAM;
    }

    /* Range Module Init Check */
    RANGE_INIT_CHECK(unit);

    /* Range Module Lock */
    RANGE_LOCK(unit);

    tmp = RANGE_CTRL(unit)->ranges;

    if (tmp == NULL) {
        RANGE_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    while (tmp) {
        memset (&range_config, 0, sizeof(range_config));
        range_config.rid       = tmp->rid;
        range_config.rtype     = tmp->rtype;
        range_config.min       = tmp->min;
        range_config.max       = tmp->max;
        range_config.offset    = tmp->offset;
        range_config.width     = tmp->width;
        range_config.udf_id    = tmp->udf_id;
        range_config.ports     = tmp->ports;

        /* Call user callback. */
        rv = (callback)(unit, &range_config, user_data);
        if (BCM_FAILURE(rv)) {
            break;
        }
        tmp = tmp->next;
    }

    RANGE_UNLOCK(unit);
    return (BCM_E_NONE);
}

/*  Function    :   bcmi_xgs5_range_get_hw_index
 *  Purpose     :   Retrieve hardware index used by a range id
 *  Parameters  :   unit - (IN) Unit number.
 *                  rid  - (IN) Range ID.
 *                  hw_index    - (OUT) hw index
 *  Returns     :   BCM_E_XXX
 */
int
bcmi_xgs5_range_get_hw_index(
        int unit,
        bcm_range_t rid,
        int *hw_index)
{
    _range_t *tmp = NULL;   /* Range structure */

    if(!hw_index) {
        return BCM_E_PARAM;
    }

    for (tmp = RANGE_CTRL(unit)->ranges; tmp != NULL; tmp = tmp->next) {
        /* Find the matching Range Id */
        if(tmp->rid == rid) {
            *hw_index = tmp->hw_index;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*  Function    :   bcmi_xgs5_range_validate_port_config
 *  Purpose     :   Validate the ports config against the Oper mode of the Range module.
 *                  When Global mode is set, only all port bitmaps are allowed.
 *                  When perpipe mode is set, only perpipe port bitmaps are allowed.
 *  Parameters  :   unit - (IN) Unit number.
 *                  oper_mode - (IN) Range module Operational mode
 *                  ports - (IN) PBMP given by user
 *                  pipe_instance - (OUT) Pipe number when in PerPipe mode
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_validate_port_config (
        int unit,
        bcm_range_oper_mode_t oper_mode,
        bcm_pbmp_t ports,
        int *pipe_instance)
{
    int                 pipe;   /* Device pipe iterator.    */
    bcm_port_config_t   pc;     /* Port configuration.      */
    int rv;                     /* Operation return value.  */

    /* Initialize port configuration structure. */
    bcm_port_config_t_init(&pc);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &pc);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (oper_mode == bcmRangeOperModeGlobal) {
        /* Global Mode : All ports must be part of IPBM value, if supplied. */
        if (BCM_PBMP_NEQ(pc.all, ports)) {
            return BCM_E_PARAM;
        }
        /* Default value is 0*/
        *pipe_instance = 0;
    }
    else if (oper_mode == bcmRangeOperModePipeLocal) {
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            if (BCM_PBMP_EQ(pc.per_pipe[pipe], ports)) {
                *pipe_instance = pipe;
                break;
            }
        }
        if (NUM_PIPE(unit) == pipe) {
            return BCM_E_PARAM;
        }
    }
    return (BCM_E_NONE);
}

/*  Function    :   bcmi_xgs5_range_validate_field_stage_oper_mode
 *  Purpose     :   Validate Field Stage Oper Mode against Range Module Oper Mode
 *  Parameters  :   unit - (IN) Unit number.
 *                  oper_mode  - (IN) Operational mode of the Field Group's Stage
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_validate_field_stage_oper_mode (
        int unit,
        bcm_field_group_oper_mode_t grp_oper_mode)
{
    int         rv;     /* Operation return value */
    bcm_range_oper_mode_t   range_oper_mode = 0; /* Range module operational mode */

    rv = bcmi_xgs5_range_oper_mode_get(unit, &range_oper_mode);
    BCM_IF_ERROR_RETURN(rv);

    if (((grp_oper_mode == bcmFieldGroupOperModeGlobal) &&
        (range_oper_mode != bcmRangeOperModeGlobal)))
    {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*  Function    :   bcmi_xgs5_validate_field_group_instance
 *  Purpose     :   Retrieve hardware index used by a range id
 *  Parameters  :   unit - (IN) Unit number.
 *                  rid  - (IN) Range ID.
 *                  grp_instance - (IN) pipe instance, the field group is configured on
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_validate_field_group_instance(
        int unit,
        bcm_range_t rid,
        int grp_instance)
{
    int rv;
    _range_t *tmp = NULL;   /* Range structure */
    bcm_range_oper_mode_t   range_oper_mode = 0; /* Range module operational mode */

    tmp = RANGE_CTRL(unit)->ranges;

    if (tmp == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = bcmi_xgs5_range_oper_mode_get(unit, &range_oper_mode);
    BCM_IF_ERROR_RETURN(rv);

    if (range_oper_mode == bcmRangeOperModeGlobal) {
        return BCM_E_NONE;
    }

    for (; tmp != NULL; tmp = tmp->next) {
        if(tmp->rid == rid) {
            if (tmp->pipe_instance == grp_instance) {
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_PARAM;
}

/*
 * Function     :   bcmi_xgs5_range_RangeCheckersAPIType_set
 * Purpose      :   Configure Switch Control for RangeCheckersAPIType
 * Parameters   :   unit        - (IN) BCM Device number.
 *                  api_type    - (IN) Setting this switch control to 0 allows to use only field API's.
 *                                     Setting it to 1 allows to use only new range module API's.
 * Returns      :   BCM_E_XXX
 */
int bcmi_xgs5_range_RangeCheckersAPIType_set (
        int unit,
        int api_type)
{

    RANGE_INIT_CHECK(unit);

    if ((api_type < 0) || (api_type >1)) {
        /* Value of 0 or 1 only is allowed*/
        return BCM_E_PARAM;
    }

    if (api_type == 0) {
        if (RANGE_CTRL(unit)->ranges) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "Range resource is used by RANGE module. \n")));
            return BCM_E_UNAVAIL;
        }
        RANGE_CTRL(unit)->range_used_by_module = RANGE_USED_BY_FIELD_MODULE;
    }
    else {
        if (_bcm_field_is_stage_range_check_exist(unit,
                    _BCM_FIELD_STAGE_INGRESS) == BCM_E_EXISTS) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "Range resource is used by FIELD module. \n")));
            return BCM_E_UNAVAIL;
        }
        RANGE_CTRL(unit)->range_used_by_module = RANGE_USED_BY_RANGE_MODULE;
    }

    return BCM_E_NONE;
}

/*
 * Function     :   bcmi_xgs5_range_RangeCheckersAPIType_get
 * Purpose      :   Configure Switch Control for RangeCheckersAPIType
 * Parameters   :   unit        - (IN) BCM Device number.
 *                  api_type    - (OUT) Get this switch control value.
 *                                      0 - Only field API's are allowed.
 *                                      1 - Only new range module API's are allowed.
 * Returns      :   BCM_E_XXX
 */
int bcmi_xgs5_range_RangeCheckersAPIType_get (
        int unit,
        int *api_type)
{
    RANGE_INIT_CHECK(unit);

    *api_type = RANGE_CTRL(unit)->range_used_by_module;
    return BCM_E_NONE;
}

/* Function to install group  bitmap to HW */
int bcmi_range_group_hw_install(int unit,
                                uint32 range_group_id,
                                SHR_BITDCL *range_hw_bmap,
                                bcm_range_oper_mode_t oper_mode,
                                int pipe_instance)
{
    soc_mem_t mem = INVALIDm;                   /* Memory for Range check */
    uint32 tbl_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};  /* Range checker entry buffer. */

    if (SOC_MEM_IS_VALID(unit, IFP_RANGE_CHECK_MASKm)) {
        mem = (oper_mode == bcmRangeOperModeGlobal)     \
              ? (IFP_RANGE_CHECK_MASKm)                 \
              : (SOC_MEM_UNIQUE_ACC(unit, IFP_RANGE_CHECK_MASKm)[pipe_instance]) ;
        sal_memset(tbl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, range_group_id, tbl_entry));
        soc_mem_field_set(unit, mem, tbl_entry, MASKf, range_hw_bmap);

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, range_group_id, tbl_entry));

        return BCM_E_NONE;
    }

    if (oper_mode == bcmRangeOperModeGlobal) {
        if (range_group_id < (RANGE_CTRL(unit)->max_groups/2)) {
             mem = IFP_RANGE_CHECK_MASK_Am;
         } else {
             mem = IFP_RANGE_CHECK_MASK_Bm;
         }
    } else if (oper_mode == bcmRangeOperModePipeLocal) {
        if (range_group_id < (RANGE_CTRL(unit)->max_groups/2)) {
             BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                 IFP_RANGE_CHECK_MASK_Am,
                                                 pipe_instance,
                                                 &mem));
         } else {
             BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                 IFP_RANGE_CHECK_MASK_Bm,
                                                 pipe_instance,
                                                 &mem));
         }
    } else {
         return (BCM_E_INTERNAL);
    }


    /* Write range check memory */
    sal_memset(tbl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, tbl_entry));

    soc_mem_field_set(unit, mem, tbl_entry,
                      range_group_fields[range_group_id],
                      range_hw_bmap);

        /* Write entry back to hw. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                        MEM_BLOCK_ALL, 0, tbl_entry));

    return BCM_E_NONE;
}

/* Function to install group  bitmap to HW */
int bcmi_range_group_hw_get(int unit,
                            uint32 range_group_id,
                            SHR_BITDCL *range_hw_bmap,
                            bcm_range_oper_mode_t oper_mode,
                            int pipe_instance)
{
    soc_mem_t mem = INVALIDm;                   /* Memory for Range check */
    uint32 tbl_entry[SOC_MAX_MEM_FIELD_WORDS];  /* Range checker entry buffer. */

    if (SOC_MEM_IS_VALID(unit, IFP_RANGE_CHECK_MASKm)) {
        mem = (oper_mode == bcmRangeOperModeGlobal)     \
              ? (IFP_RANGE_CHECK_MASKm)                 \
              : (SOC_MEM_UNIQUE_ACC(unit, IFP_RANGE_CHECK_MASKm)[pipe_instance]);
        sal_memset(tbl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, range_group_id, tbl_entry));
        soc_mem_field_get(unit, mem, tbl_entry, MASKf, range_hw_bmap);
        return BCM_E_NONE;
    }

    if (oper_mode == bcmRangeOperModeGlobal) {
        if (range_group_id < (RANGE_CTRL(unit)->max_groups/2)) {
             mem = IFP_RANGE_CHECK_MASK_Am;
         } else {
             mem = IFP_RANGE_CHECK_MASK_Bm;
         }
    } else if (oper_mode == bcmRangeOperModePipeLocal) {
        if (range_group_id < (RANGE_CTRL(unit)->max_groups/2)) {
             BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                 IFP_RANGE_CHECK_MASK_Am,
                                                 pipe_instance,
                                                 &mem));
         } else {
             BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                 IFP_RANGE_CHECK_MASK_Bm,
                                                 pipe_instance,
                                                 &mem));
         }
    } else {
         return (BCM_E_INTERNAL);
    }

    /* Write range check memory */
    sal_memset(tbl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, tbl_entry));

    soc_mem_field_get(unit, mem, tbl_entry,
                      range_group_fields[range_group_id],
                      range_hw_bmap);
    return BCM_E_NONE;
}


/*
 * This API will help to combine different Range Checkers into a single
 * group. It returns a range group identifier that can be passed to
 * 'bcm_field_qualify_RangeGroup' API, to qualify a Field entry with
 * Range Group match criteria. The Individual RangeCheck conditions and
 * its IDs has to be created separately using bcm_range_create API.
 */
int bcmi_xgs5_range_group_create(
    int unit,
    bcm_range_group_config_t *range_group_config)
{
    SHR_BITDCL          *range_hw_bmap;    /* Used indexes map.        */
    SHR_BITDCL          *range_hw_bmap_cmp;/* Used indexes map.        */
    int                 idx;            /* Loop termination index.  */
    _range_t            *tmp = NULL;    /* Range pointer */
    int                 idx_max;        /* Loop termination index.  */
    soc_mem_t           mem = RANGE_CTRL(unit)->range_mem; /* Range Checker Memory ID */
    bcm_range_oper_mode_t   oper_mode = 0;  /*Range module operational mode */
    int pipe_instance = 0;
    uint8 resource_avail = 0;
    int rv = 0;
    uint32 range_group_id = -1;

    /* Input parameters check */
    BCM_IF_NULL_RETURN_PARAM(range_group_config);

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_group_create: Error Fetching Range Oper Mode \n")));
        return (rv);
    }

    rv = bcmi_xgs5_range_validate_port_config(unit, oper_mode,
                                              range_group_config->ports, &pipe_instance);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_group_create: Error Validating Port Config \n")));
        return (rv);
    }

    /* Validate resource availability */
    for (idx = 0; idx < RANGE_CTRL(unit)->max_groups; idx++) {
         if (0 == SHR_BITGET(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance), idx)) {
             resource_avail = 1;
             range_group_id = idx;
             break;
         }
    }
    if (!resource_avail) {
        LOG_CLI((BSL_META("bcmi_xgs5_range_group_create: Range group resources are not available\n")));
        return BCM_E_RESOURCE;
    }

    idx_max = soc_mem_index_max(unit, mem);
    range_hw_bmap = NULL;

    RANGE_MALLOC(range_hw_bmap, SHR_BITDCL, SHR_BITALLOCSIZE(idx_max + 1), "Valid Ranges");
    if (NULL == range_hw_bmap) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_create: Error Allocating memory for range_bmap \n")));
        return (BCM_E_MEMORY);
    }

    range_hw_bmap_cmp = NULL;

    RANGE_MALLOC(range_hw_bmap_cmp, SHR_BITDCL, SHR_BITALLOCSIZE(idx_max + 1), "Valid Ranges");
    if (NULL == range_hw_bmap_cmp) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_create: Error Allocating memory for range_bmap_cmp \n")));
        sal_free(range_hw_bmap);
        return (BCM_E_MEMORY);
    }


    /* Get HW index of range ids in bitmap */
    for (idx = 0; idx < RANGE_CTRL(unit)->max_ranges; idx ++ ) {
         if (BCM_RANGE_ID_BMP_TEST(range_group_config->range_bmp, idx)) {
             for (tmp = RANGE_CTRL(unit)->ranges; tmp != NULL; tmp = tmp->next) {
                  if (idx == tmp->rid) {
                      SHR_BITSET(range_hw_bmap, tmp->hw_index);
                  }
             }
         }
    }

    /* Check if same range group already exists */
    for (idx = 0; idx < RANGE_CTRL(unit)->max_groups; idx++) {
         if (SHR_BITGET(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance), idx)) {
             rv = bcmi_range_group_hw_get(unit, idx, range_hw_bmap_cmp,
                                          oper_mode, pipe_instance);
             if (BCM_FAILURE(rv)) {
                 sal_free(range_hw_bmap);
                 sal_free(range_hw_bmap_cmp);
                 return rv;
             }

             if (!(sal_memcmp(range_hw_bmap, range_hw_bmap_cmp,
                            (SHR_BITALLOCSIZE(idx_max + 1))))) {
                 sal_free(range_hw_bmap);
                 sal_free(range_hw_bmap_cmp);
                 return (BCM_E_EXISTS);

             }
         }
    }

    /* Call install function with mask id[range group id] */

    rv = bcmi_range_group_hw_install(unit, range_group_id,
                                     range_hw_bmap, oper_mode, pipe_instance);
    if (BCM_SUCCESS(rv)) {
        SHR_BITSET(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance),
                     range_group_id);
        range_group_config->range_group_id = (range_group_id
                                              + (RANGE_CTRL(unit)->max_groups * pipe_instance));
    }

    LOG_DEBUG(BSL_LS_BCM_RANGE,
            (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_create: Range Create Successful \n")));

    sal_free(range_hw_bmap);
    sal_free(range_hw_bmap_cmp);
    return rv;
}


/*
 * This API will help to delete the range group created using
 * bcm_range_group_create.
 */
int bcmi_xgs5_range_group_delete(
    int unit,
    bcm_range_group_config_t *range_group_config)
{
    SHR_BITDCL          *range_hw_bmap;    /* Used indexes map.        */
    int                 idx_max;        /* Loop termination index.  */
    soc_mem_t           mem = RANGE_CTRL(unit)->range_mem; /* Range Checker Memory ID */
    bcm_range_oper_mode_t   oper_mode = 0;  /*Range module operational mode */
    int pipe_instance = 0;
    int rv = 0;
    uint32 range_group_id = 0;

    /* Input parameters check */
    BCM_IF_NULL_RETURN_PARAM(range_group_config);

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_group_delete: Error Fetching Range Oper Mode \n")));
        return (rv);
    }

    pipe_instance = (range_group_config->range_group_id / RANGE_CTRL(unit)->max_groups);
    range_group_id = (range_group_config->range_group_id % RANGE_CTRL(unit)->max_groups);

    if ((pipe_instance < 0) || (pipe_instance >= SOC_MAX_NUM_PIPES)) {
         LOG_DEBUG(BSL_LS_BCM_RANGE,
                 (BSL_META_U(unit,
                             "bcmi_xgs5_range_group_delete: Error Validating Range Group ID \n")));
         return (BCM_E_INTERNAL);
    }

    BCM_IF_ERROR_RETURN (rv);

    idx_max = soc_mem_index_max(unit, mem);
    range_hw_bmap = NULL;

    RANGE_MALLOC(range_hw_bmap, SHR_BITDCL, SHR_BITALLOCSIZE(idx_max + 1), "Valid Ranges");
    if (NULL == range_hw_bmap) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_delete: Error Allocating memory for range_bmap \n")));
        return (BCM_E_MEMORY);
    }

    if (SHR_BITGET(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance),
                     range_group_id)) {
        rv = bcmi_range_group_hw_install(unit, range_group_id,
                                         range_hw_bmap, oper_mode, pipe_instance);
        SHR_BITCLR(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance),
                     range_group_id);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    LOG_DEBUG(BSL_LS_BCM_RANGE,
            (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_delete: Range Create Successful \n")));

    sal_free(range_hw_bmap);
    return rv;
}

/*
 * This API will help to update the range IDs in the range group created
 * using bcm_range_group_create.
 */
int bcmi_xgs5_range_group_update(
    int unit,
    bcm_range_group_config_t *range_group_config)
{
    SHR_BITDCL          *range_hw_bmap;    /* Used indexes map.        */
    SHR_BITDCL          *range_hw_bmap_cmp;/* Used indexes map.        */
    int                 idx;            /* Loop termination index.  */
    _range_t            *tmp = NULL;    /* Range pointer */
    int                 idx_max;        /* Loop termination index.  */
    soc_mem_t           mem = RANGE_CTRL(unit)->range_mem; /* Range Checker Memory ID */
    bcm_range_oper_mode_t   oper_mode = 0;  /*Range module operational mode */
    int pipe_instance = 0;
    int rv = 0;
    uint32 range_group_id = 0;

    /* Input parameters check */
    BCM_IF_NULL_RETURN_PARAM(range_group_config);

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_group_update: Error Fetching Range Oper Mode \n")));
        return (rv);
    }

    pipe_instance = (range_group_config->range_group_id / RANGE_CTRL(unit)->max_groups);
    range_group_id = (range_group_config->range_group_id % RANGE_CTRL(unit)->max_groups);

    if ((pipe_instance < 0) || (pipe_instance >= SOC_MAX_NUM_PIPES)) {
         LOG_DEBUG(BSL_LS_BCM_RANGE,
                 (BSL_META_U(unit,
                             "bcmi_xgs5_range_group_update: Error Validating Range Group ID \n")));
         return (BCM_E_INTERNAL);
    }

    idx_max = soc_mem_index_max(unit, mem);
    range_hw_bmap = NULL;

    RANGE_MALLOC(range_hw_bmap, SHR_BITDCL, SHR_BITALLOCSIZE(idx_max + 1), "Valid Ranges");
    if (NULL == range_hw_bmap) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_update: Error Allocating memory for range_bmap \n")));
        return (BCM_E_MEMORY);
    }

    range_hw_bmap_cmp = NULL;

    RANGE_MALLOC(range_hw_bmap_cmp, SHR_BITDCL, SHR_BITALLOCSIZE(idx_max + 1), "Valid Ranges");
    if (NULL == range_hw_bmap_cmp) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_update: Error Allocating memory for range_bmap_cmp \n")));
        sal_free(range_hw_bmap);
        return (BCM_E_MEMORY);
    }

    /* Get HW index of range ids in bitmap */
    for (idx = 0; idx < RANGE_CTRL(unit)->max_ranges; idx ++ ) {
         if (BCM_RANGE_ID_BMP_TEST(range_group_config->range_bmp, idx)) {
             for (tmp = RANGE_CTRL(unit)->ranges; tmp != NULL; tmp = tmp->next) {
                  if (idx == tmp->rid) {
                      SHR_BITSET(range_hw_bmap, tmp->hw_index);
                  }
             }
         }
    }

    /* Check if same range group already exists */
    for (idx = 0; idx < RANGE_CTRL(unit)->max_groups; idx++) {
         if (SHR_BITGET(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance), idx)) {
             rv = bcmi_range_group_hw_get(unit, idx, range_hw_bmap_cmp,
                                          oper_mode, pipe_instance);
             if (BCM_FAILURE(rv)) {
                 sal_free(range_hw_bmap);
                 sal_free(range_hw_bmap_cmp);
                 return rv;
             }

             if (!(sal_memcmp(range_hw_bmap, range_hw_bmap_cmp,
                            (SHR_BITALLOCSIZE(idx_max + 1))))) {
                 sal_free(range_hw_bmap);
                 sal_free(range_hw_bmap_cmp);
                 return (BCM_E_EXISTS);

             }
         }
    }

    if (SHR_BITGET(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance),
                     range_group_id)) {
        rv = bcmi_range_group_hw_install(unit, range_group_id,
                                         range_hw_bmap, oper_mode, pipe_instance);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    LOG_DEBUG(BSL_LS_BCM_RANGE,
            (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_update: Range Create Successful \n")));

    sal_free(range_hw_bmap);
    sal_free(range_hw_bmap_cmp);
    return rv;

}

/*
 * This API will help to fetch the range group configuration created
 * using bcm_range_group_create.
 */
int bcmi_xgs5_range_group_get(
    int unit,
    bcm_range_group_config_t *range_group_config)
{
    SHR_BITDCL          *range_hw_bmap;    /* Used indexes map.        */
    int                 idx;            /* Loop termination index.  */
    _range_t            *tmp = NULL;    /* Range pointer */
    int                 idx_max;        /* Loop termination index.  */
    soc_mem_t           mem = RANGE_CTRL(unit)->range_mem; /* Range Checker Memory ID */
    bcm_range_oper_mode_t   oper_mode = 0;  /*Range module operational mode */
    int pipe_instance = 0;
    int rv = 0;
    uint32 range_group_id = 0;

    /* Input parameters check */
    BCM_IF_NULL_RETURN_PARAM(range_group_config);

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_group_get: Error Fetching Range Oper Mode \n")));
        return (rv);
    }

    pipe_instance = (range_group_config->range_group_id / RANGE_CTRL(unit)->max_groups);
    range_group_id = (range_group_config->range_group_id % RANGE_CTRL(unit)->max_groups);

    if ((pipe_instance < 0) || (pipe_instance >= SOC_MAX_NUM_PIPES)) {
         LOG_DEBUG(BSL_LS_BCM_RANGE,
                 (BSL_META_U(unit,
                             "bcmi_xgs5_range_group_get: Error Validating Range Group ID \n")));
         return (BCM_E_INTERNAL);
    }

    idx_max = soc_mem_index_max(unit, mem);
    range_hw_bmap = NULL;

    RANGE_MALLOC(range_hw_bmap, SHR_BITDCL, SHR_BITALLOCSIZE(idx_max + 1), "Valid Ranges");
    if (NULL == range_hw_bmap) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_get: Error Allocating memory for range_bmap \n")));
        return (BCM_E_MEMORY);
    }

    if (SHR_BITGET(((RANGE_CTRL(unit)->range_group_used_bitmap) + pipe_instance),
                     range_group_id)) {
        rv = bcmi_range_group_hw_get(unit, range_group_id,
                                     range_hw_bmap, oper_mode, pipe_instance);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    /* Convert hw bmap to range id bmap */
    for (idx = 0; idx < RANGE_CTRL(unit)->max_ranges; idx ++ ) {
         if (SHR_BITGET(range_hw_bmap, idx)) {
             for (tmp = RANGE_CTRL(unit)->ranges; tmp != NULL; tmp = tmp->next) {
                  if (idx == tmp->hw_index) {
                      BCM_RANGE_ID_BMP_ADD(range_group_config->range_bmp, tmp->rid);
                  }
             }
         }
    }

    LOG_DEBUG(BSL_LS_BCM_RANGE,
            (BSL_META_U(unit,
                        "bcmi_xgs5_range_group_get: Range Create Successful \n")));

    sal_free(range_hw_bmap);
    return rv;
}

/*  Function    :   bcmi_xgs5_range_group_hw_idx_get
 *  Purpose     :   Fetch Range Group HW Index
 *  Parameters  :   unit - (IN) Unit number.
 *                  range_group_id  - (IN) Range Group ID.
                    pipe_id - (IN) Pipe Instance
                    hw_index - (OUT) Hardware index
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_group_hw_idx_get(
        int unit,
        uint8 range_grp_id,
        uint8 pipe_id,
        uint8 *hw_index)
{
    uint32 range_group_id = 0;
    int pipe_instance = 0;
    bcm_range_oper_mode_t   oper_mode = 0;  /*Range module operational mode */
    int rv = BCM_E_NONE;

    if (range_grp_id > ((RANGE_CTRL(unit)->max_groups * SOC_MAX_NUM_PIPES) - 1)) {
         return BCM_E_PARAM;
    }

    pipe_instance = (range_grp_id / RANGE_CTRL(unit)->max_groups);
    range_group_id = (range_grp_id % RANGE_CTRL(unit)->max_groups);

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_group_hw_idx_get: Error Fetching Range Oper Mode \n")));
        return (rv);
    }

    if ((pipe_instance != pipe_id) && (oper_mode == bcmRangeOperModePipeLocal)) {
         LOG_DEBUG(BSL_LS_BCM_RANGE,
                 (BSL_META_U(unit,
                             "bcmi_xgs5_range_group_hw_idx_get: Error Validating Pipe Instance \n")));
         return BCM_E_PARAM;
    }

    if ((pipe_instance < 0) || (pipe_instance >= SOC_MAX_NUM_PIPES)) {
         LOG_DEBUG(BSL_LS_BCM_RANGE,
                 (BSL_META_U(unit,
                             "bcmi_xgs5_range_group_hw_idx_get: Error Validating Range Group ID \n")));
         return BCM_E_PARAM;
    }

    if (!(SHR_BITGET(((RANGE_CTRL(unit)->range_group_used_bitmap)
          + pipe_instance), range_group_id))) {
        return BCM_E_PARAM;
    }

    *hw_index = range_group_id;
    return rv;
}

/*  Function    :   bcmi_xgs5_range_group_id_get
 *  Purpose     :   Fetch Range Group Id
 *  Parameters  :   unit - (IN) Unit number.
                    pipe_id - (IN) Pipe Instance.
                    hw_index - (IN) Hardware index
 *                  range_group_id  - (OUT) Range Group ID.
 *  Returns     :   BCM_E_XXX
 */
int bcmi_xgs5_range_group_id_get(
        int unit,
        uint8 pipe_id,
        uint8 *range_grp_id)
{
    int rv = BCM_E_NONE;
    bcm_range_oper_mode_t   oper_mode = 0;  /*Range module operational mode */

    rv = bcmi_xgs5_range_oper_mode_get(unit, &oper_mode);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE,
                (BSL_META_U(unit,
                            "bcmi_xgs5_range_group_hw_idx_get: Error Fetching Range Oper Mode \n")));
        return (rv);
    }

    if (oper_mode == bcmRangeOperModePipeLocal) {
        *range_grp_id = (*range_grp_id)
                 + (RANGE_CTRL(unit)->max_groups * pipe_id);
    }

    return rv;
}

