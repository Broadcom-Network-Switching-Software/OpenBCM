/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 * Based on Draco/Draco 1.5 code
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/firebolt.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * MMU Configuration default settings.
 * These can be overridden at runtime using config properties.
 * Also see soc/esw/firebolt.c (soc_firebolt_mmu_init) for
 * one-time XQ settings (HOLCOSPKTSETLIMIT)
 */
#define	MMU_CELL_SIZE		128	/* not overrideable */
#define MMU_FLOW_PERCENT	90
#define MMU_FLOW_FANIN		4
#define MMU_FLOW_PKTMIN		8	/* not overrideable */
#define	MMU_RED_DROP_PERCENT	60
#define	MMU_YELLOW_DROP_PERCENT	80
#define	MMU_STATIC_BYTES	(1536 * 1)
#define	MMU_STATIC_PERCENT	50	/* not used if STATIC_BYTES set */
#define	MMU_RESET_BYTES		(1536 * 2)
#define	MMU_RESET_MIN_PERCENT	50	/* not overrideable */
#define	MMU_OVERCOMMIT		1	/* non-stack dynamic cell overcommit */
#define	MMU_OVERCOMMIT_STACK	2	/* stack dynamic cell overcommit */

#if defined(BCM_FIREBOLT_SUPPORT)

static int _num_cosq[SOC_MAX_NUM_DEVICES];

#define FB_DRR_KBYTES   (1)
#define FB_DRR_MBYTES   (1024)
#define FB_DRR_WEIGHT_MAX 0xf
#define FB_WRR_WEIGHT_MAX 0xf

static int drr_weight_to_kbytes[FB_DRR_WEIGHT_MAX + 1] = {
    /* Weight         Kbytes    */
    /*  0 */    0 * FB_DRR_KBYTES,  /*    0K bytes - pure priority scheduling */
    /*  1 */   10 * FB_DRR_KBYTES,  /*   10K bytes */
    /*  2 */   20 * FB_DRR_KBYTES,  /*   20K bytes */
    /*  3 */   40 * FB_DRR_KBYTES,  /*   40K bytes */
    /*  4 */   80 * FB_DRR_KBYTES,  /*   80K bytes */
    /*  5 */  160 * FB_DRR_KBYTES,  /*  160K bytes */
    /*  6 */  320 * FB_DRR_KBYTES,  /*  320K bytes */
    /*  7 */  640 * FB_DRR_KBYTES,  /*  640K bytes */
    /*  8 */ 1280 * FB_DRR_KBYTES,  /* 1280K bytes */
    /*  9 */ 2560 * FB_DRR_KBYTES,  /* 2560K bytes */
    /* 10 */ 5120 * FB_DRR_KBYTES,  /* 5120K bytes */
    /* 11 */   10 * FB_DRR_MBYTES,  /*   10M bytes */
    /* 12 */   20 * FB_DRR_MBYTES,  /*   20M bytes */
    /* 13 */   40 * FB_DRR_MBYTES,  /*   40M bytes */
    /* 14 */   80 * FB_DRR_MBYTES,  /*   80M bytes */
    /* 15 */  160 * FB_DRR_MBYTES   /*  160M bytes */
};

#if defined(BCM_RAPTOR_SUPPORT)

#define BCM_MTU_QUANTA_ID_COUNT 4

static int MTU_Quanta[BCM_MTU_QUANTA_ID_COUNT] = {
    2 *  FB_DRR_KBYTES,     /* Quanta of 2048 bytes */
    4 *  FB_DRR_KBYTES,     /* Quanta of 4096 bytes */
    8 *  FB_DRR_KBYTES,     /* Quanta of 8192 bytes */
   16 *  FB_DRR_KBYTES      /* Quanta of 16384 bytes */
};

#endif /* defined(BCM_RAPTOR_SUPPORT) */

#if defined(BCM_FIREBOLT2_SUPPORT)
#define BCM_FB2_MTU_QUANTA_SMALL                2
#define BCM_FB2_MTU_QUANTA_LARGE                16
#define BCM_FB2_COS_WEIGHT_MAX                  0xff
#define BCM_FB2_CNG_CELL_LIMIT_GRANULARITY      8
#define BCM_FB2_CNG_CELL_LIMIT_MAX              0x1ffc
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
#define BCM_FB2_CNG_PKT_LIMIT_GRANULARITY       4
#endif

#undef FB_DRR_KBYTES
#undef FB_DRR_MBYTES

/*
 * Convert the number of kbytes (1024 bytes) that can transmtted in one run
 * to weight encoding as per table above
 */
static int
_bcm_fb_cos_drr_kbytes_to_weight(int kbytes)
{
    int weight;
    for (weight = 0; weight <= FB_DRR_WEIGHT_MAX; weight++) {
        if (kbytes <= drr_weight_to_kbytes[weight]) {
            break;
        }
    }

    if (weight > FB_DRR_WEIGHT_MAX) {
        weight = FB_DRR_WEIGHT_MAX;
    }

    return weight;
}

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
#define BCM_COS_WEIGHT_MAX      0x7f

static int
_bcm_cos_max_weight(const int weight[])
{
    int i, max_weight = 0;

    /* find max weight */
    for (i = 0; i < 8; i++ ) {
        if (max_weight < weight[i]) {
            max_weight = weight[i];
        }
    }

    return max_weight;
}
#endif

#if defined(BCM_RAPTOR_SUPPORT)
/*
 * Calculates the Quanta needed for COS according to its weight
 */
static int
_bcm_rp_cos_drr_weights_to_quanta(const int weight[])
{
    int i, max_weight = 0;

    max_weight = _bcm_cos_max_weight(weight);

    /* Search for the right quanta for max_wight*/
    for (i = 0; i < BCM_MTU_QUANTA_ID_COUNT; i++ ) {
        if (max_weight <= MTU_Quanta[i] * BCM_COS_WEIGHT_MAX) {
            return i;   /* Right Quanta was found */
        }
    }

    return -1;  /* In case of too big weight return negative value */
}

/*
 * Convert the number of kbytes (1024 bytes) that can transmtted in one run
 * to weight encoding for Raptor
 */
static int
_bcm_rp_cos_drr_kbytes_to_weight(int kbytes, int MTUQuantaSel)
{
    int cos_weight = 0;
    int weight_found = FALSE, low_weight_boundary, hi_weight_boundary;

    /* Search for right weight */
    low_weight_boundary = 1;
    hi_weight_boundary = BCM_COS_WEIGHT_MAX;

    /* Boundary conditions */
    if (kbytes >= hi_weight_boundary * MTU_Quanta[MTUQuantaSel]) {
        cos_weight = hi_weight_boundary;
        weight_found = TRUE;
    }
    if (kbytes <= low_weight_boundary * MTU_Quanta[MTUQuantaSel]) {
        cos_weight = low_weight_boundary;
        weight_found = TRUE;
    }

    while (!weight_found) {
        cos_weight =  (hi_weight_boundary + low_weight_boundary) / 2;
        if (kbytes <= cos_weight * MTU_Quanta[MTUQuantaSel]) {
           if (kbytes > ((cos_weight - 1) * MTU_Quanta[MTUQuantaSel])) {
               weight_found = TRUE;
           } else {
               hi_weight_boundary = cos_weight;
           }
        } else {
            if (kbytes <= ((cos_weight + 1) * MTU_Quanta[MTUQuantaSel])) {
                cos_weight++;
                weight_found = TRUE;
            } else {
                low_weight_boundary = cos_weight;
            }
        }
    } /* Here weight should be found */

    return cos_weight;
}

/*
 * Convert the encoded weights to number of kbytes that can transmtted
 * in one run.
 */
static int
_bcm_rp_cos_drr_weight_to_kbytes(int weight, int MTUQuantaSel)
{
    return (weight * MTU_Quanta[MTUQuantaSel]);
}
#endif     /* defined(BCM_RAPTOR_SUPPORT) */

/*
 * Convert the encoded weights to number of kbytes that can transmtted
 * in one run.
 */
static int
_bcm_fb_cos_drr_weight_to_kbytes(int weight)
{
    assert(weight <= FB_DRR_WEIGHT_MAX);

    return(drr_weight_to_kbytes[weight]);
}

#if defined(BCM_FIREBOLT2_SUPPORT)
/*
 * Calculates the Quanta needed for COS according to its weight
 */
static int
_bcm_fb2_cos_drr_weights_to_quanta(const int weight[])
{
    int max_weight = 0;

    max_weight = _bcm_cos_max_weight(weight);

    /* Search for the right quanta for max_weight*/
    if (max_weight <= (BCM_FB2_MTU_QUANTA_SMALL * BCM_FB2_COS_WEIGHT_MAX)) {
        return BCM_FB2_MTU_QUANTA_SMALL;
    } else if (max_weight <=
               (BCM_FB2_MTU_QUANTA_LARGE * BCM_FB2_COS_WEIGHT_MAX)) {
        return BCM_FB2_MTU_QUANTA_LARGE;
    }

    return -1;  /* In case of too big weight return negative value */
}

/*
 * Convert the number of kbytes (1024 bytes) that can transmtted in one run
 * to weight encoding for Firebolt2
 */
static int
_bcm_fb2_cos_drr_kbytes_to_weight(int kbytes, int MTUQuanta)
{
    int cos_weight = 0;
    int weight_found = FALSE, low_weight_boundary, hi_weight_boundary;

    /* Search for right weight */
    low_weight_boundary = 1;
    hi_weight_boundary = BCM_FB2_COS_WEIGHT_MAX;

    /* Boundary conditions */
    if (kbytes >= hi_weight_boundary * MTUQuanta) {
        cos_weight = hi_weight_boundary;
        weight_found = TRUE;
    }
    if (kbytes <= low_weight_boundary * MTUQuanta) {
        cos_weight = low_weight_boundary;
        weight_found = TRUE;
    }

    while (!weight_found) {
        cos_weight =  (hi_weight_boundary + low_weight_boundary) / 2;
        if (kbytes <= cos_weight * MTUQuanta) {
           if (kbytes > ((cos_weight - 1) * MTUQuanta)) {
               weight_found = TRUE;
           } else {
               hi_weight_boundary = cos_weight;
           }
        } else {
            if (kbytes <= ((cos_weight + 1) * MTUQuanta)) {
                cos_weight++;
                weight_found = TRUE;
            } else {
                low_weight_boundary = cos_weight;
            }
        }
    } /* Here weight should be found */

    return cos_weight;
}
#endif     /* defined(BCM_FIREBOLT2_SUPPORT) */


#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      bcm_fb_cosq_sync
 * Purpose:
 *      Record COSQ module persistent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fb_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8               *cosq_scache_ptr;
    uint32              num_cosq;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &cosq_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL));
    
    /* Number COSQ */
    num_cosq = _num_cosq[unit];
    sal_memcpy(cosq_scache_ptr, &num_cosq, sizeof(num_cosq));
        
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb_cosq_reinit
 * Purpose:
 *      Recover COSQ software state.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fb_cosq_reinit(int unit)
{
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint8               *cosq_scache_ptr;
    uint32              num_cosq;
 	 
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &cosq_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (rv == BCM_E_NOT_FOUND) {
        cosq_scache_ptr = NULL;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (cosq_scache_ptr != NULL) {
        /* Number COSQ */
        sal_memcpy(&num_cosq, cosq_scache_ptr, sizeof(num_cosq));
        _num_cosq[unit] = num_cosq;

    } else {
        int cos;
        uint32 val, csl;

        /* Make best guess */
        if (soc_reg_field_valid(unit, LWMCOSCELLSETLIMITr,
                                CELLSETLIMITf)) {
            for (cos = 0; cos < NUM_COS(unit); cos++) {
                SOC_IF_ERROR_RETURN
                    (READ_LWMCOSCELLSETLIMITr(unit, REG_PORT_ANY, cos, &val));
                csl = soc_reg_field_get(unit, LWMCOSCELLSETLIMITr,
                                        val, CELLSETLIMITf);
                if (csl == 0) {
                    /* No more COSq info for this COS value, so we're done */
                    break;
                }
            }
        } else {
            cos = _bcm_esw_cosq_config_property_get(unit);
        }

        _num_cosq[unit] = cos;
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_fb_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_cosq_init(int unit)
{
    int         num_cos;
#ifdef BCM_WARM_BOOT_SUPPORT
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint32              cosq_scache_size;
    uint8               *cosq_scache_ptr;
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Warm boot level 2 cache size */
    cosq_scache_size = sizeof(uint32); /* Number COSQ */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                                 (0 == SOC_WARM_BOOT(unit)),
                                 cosq_scache_size, &cosq_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    if (SOC_WARM_BOOT(unit)) {
        return _bcm_fb_cosq_reinit(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    num_cos = _bcm_esw_cosq_config_property_get(unit);

    return bcm_fb_cosq_config_set(unit, num_cos);
}

/*
 * MMU Programming for 56500 style memory managers.
 * Includes 56100, 56300, 56200 devices.
 *
 * The MMU is always configured for dynamic mode.
 *
 * Per-port/per-cos limits are programmed in soc_firebolt_mmu_init
 * based on the mmu_xq_weight config parameters.  This has to happen
 * one time during initialization.  The remaining parameters can be
 * updated multiple times after initialization.
 *
 * Default MMU xq (packet) controls allow for 8 queues to be used
 * even less than 8 are currently configured.  If less than 8 queues
 * will ever be used then the remaining queues mmu_xq_weight_cosN
 * values should be set to 0.  A 4 queue configuration should set
 * mmu_xq_weight_cosN=0 for N=4, 5, 6, 7.
 *
 * MMU Controls used here have defaults based on the defines at the
 * top of this file.  Each of those defaults can be overridden using
 * config properties.
 *
 * mmu_flow_percent=90		percentage of per-port cells useable
 *				before flow control starts
 * mmu_flow_fanin=4		number of simulteneous senders to each
 *				port for flow control purposes
 * mmu_red_drop_percent=60
 * mmu_yellow_drop_percent=80	percentage of per-port/per-cos packets
 *				used before red or yellow packets will
 *				be dropped
 * mmu_static_bytes=1536	per-port/per-cos static reserved limit.
 *				Rounded up from bytes to next cell size.
 *				Remaining cells are put in dynamic pool.
 *				If 0, then mmu_static_percent is used.
 * mmu_static_percent=50	Percentage of per-port/per-cos cells to
 *				use as static reserved limit.
 *				Remaining cells are put in dynamic pool.
 *				Only used if mmu_static_bytes is 0.
 * mmu_reset_bytes=3072		(1536*2)
 *				offset from dynamic cell set limits for
 *				reset (enable) limits.
 *				Rounded up from bytes to next cell size.
 * mmu_overcommit=1		non-stack port overcommit factor for
 *				dynamic pool
 * mmu_overcommit_stack=2	stack port overcommit factor for
 *				dynamic pool.  If 0, then use the
 *				mmu_overcommit value for stack ports
 */

STATIC int
_bcm_fb_cosq_config_set(int unit, int numq)
{
    int         ncells, nports, xq_per_port, cos;
    int         n, nreset, used_cells;
    int		scells, dcells;
    soc_port_t  port;
    uint32      val, limit_red, limit_yel;
    soc_mem_t   xq_mem;
    int		mmu_flow_percent;
    int		mmu_flow_fanin;
    int		mmu_red_drop_percent;
    int		mmu_yellow_drop_percent;
    int		mmu_static_bytes;
    int		mmu_static_cells;
    int		mmu_static_percent;
    int		mmu_reset_bytes;
    int		mmu_reset_cells = 0;
    int		mmu_overcommit;
    int		mmu_overcommit_stack;
#if defined (BCM_HAWKEYE_SUPPORT)
    int		mmu_dyncellsetlimit = 0;
    int		mmu_xoff_pkt_threshold = 0;
    int		mmu_xoff_cell_threshold = 0;
    int		xq_per_cos = 0;
#endif /* BCM_HAWKEYE_SUPPORT */
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_FELIX_SUPPORT)  || defined(BCM_FIREBOLT2_SUPPORT)
    int		sreset;
#endif

    /* Validate cosq num */
    if (numq < 1) {
        return BCM_E_PARAM;
    }

    ncells = soc_mem_index_count(unit, MMU_CBPDATA0m);
#if defined (BCM_HAWKEYE_SUPPORT)
    if(SOC_IS_HAWKEYE(unit)){
        nports = NUM_E_PORT(unit);
        /* These special variables are for Hawkeye only. 
            Those values are retrieved from HAWKEYE MMU setting document*/
        mmu_dyncellsetlimit = 2400;
        mmu_xoff_pkt_threshold = 12;
        mmu_xoff_cell_threshold = 74;
        xq_per_cos = 8;
    } else 
#endif /* BCM_HAWKEYE_SUPPORT */
    {
        nports = NUM_ALL_PORT(unit);
    }

    /* get mmu control config values */
    mmu_flow_percent = soc_property_get(unit,
					spn_MMU_FLOW_PERCENT,
					MMU_FLOW_PERCENT);
    mmu_flow_fanin = soc_property_get(unit,
				      spn_MMU_FLOW_FANIN,
				      MMU_FLOW_FANIN);
    mmu_red_drop_percent = soc_property_get(unit,
					    spn_MMU_RED_DROP_PERCENT,
					    MMU_RED_DROP_PERCENT);
    mmu_yellow_drop_percent = soc_property_get(unit,
					       spn_MMU_YELLOW_DROP_PERCENT,
					       MMU_YELLOW_DROP_PERCENT);
    mmu_static_bytes = soc_property_get(unit,
					spn_MMU_STATIC_BYTES,
					MMU_STATIC_BYTES);
    mmu_static_percent = soc_property_get(unit,
					  spn_MMU_STATIC_PERCENT,
					  MMU_STATIC_PERCENT);
    mmu_reset_bytes = soc_property_get(unit,
				       spn_MMU_RESET_BYTES,
				       MMU_RESET_BYTES);
    mmu_overcommit = soc_property_get(unit,
				      spn_MMU_OVERCOMMIT,
				      MMU_OVERCOMMIT);
    mmu_overcommit_stack = soc_property_get(unit,
					    spn_MMU_OVERCOMMIT_STACK,
					    MMU_OVERCOMMIT_STACK);

    /* sanity check config values */
    if (mmu_flow_percent <= 0 || mmu_flow_percent > 100) {
	mmu_flow_percent = 100;
    }
    if (mmu_red_drop_percent <= 0 || mmu_red_drop_percent > 100) {
	mmu_red_drop_percent = 100;
    }
    if (mmu_yellow_drop_percent <= 0 || mmu_yellow_drop_percent > 100) {
	mmu_yellow_drop_percent = 100;
    }
    if (mmu_static_percent <= 0 || mmu_static_percent > 100) {
	mmu_static_percent = MMU_STATIC_PERCENT;
    }
    if (mmu_overcommit <= 0) {
	mmu_overcommit = 1;
    }
    if (mmu_overcommit_stack <= 0) {
	mmu_overcommit_stack = mmu_overcommit;
    }
    mmu_static_cells = (mmu_static_bytes + MMU_CELL_SIZE - 1) / MMU_CELL_SIZE;
    mmu_reset_cells = (mmu_reset_bytes + MMU_CELL_SIZE - 1) / MMU_CELL_SIZE;

    /* calculate static limits first */
    if (mmu_static_cells > 0) {
	scells = mmu_static_cells;
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_FELIX_SUPPORT)  || defined(BCM_FIREBOLT2_SUPPORT)
	sreset = scells;		/* 100% */
#endif
    } else {
	scells = (ncells / nports / numq) * mmu_static_percent / 100;
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_FELIX_SUPPORT)  || defined(BCM_FIREBOLT2_SUPPORT)
	sreset = scells * 75 / 100;	/* 75% */
#endif
    }
    used_cells = scells * numq * nports;

    /* remaining cells are dynamic */
    n = ncells - used_cells;
    if (n <= 0) {
	n = 0;
    }
    dcells = n / nports;
    nreset = n - mmu_reset_cells;
    if (nreset < (n * MMU_RESET_MIN_PERCENT / 100)) {
	nreset = n * MMU_RESET_MIN_PERCENT / 100;
    }
    if (nreset <= 0) {
	nreset = 1;
    }

    /* device wide dynamic limits */
    if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
#if defined(BCM_RAPTOR_SUPPORT)
        SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLSETLIMITr(unit, &val));
        soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                          &val, TOTALDYNCELLSETLIMITf, n);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, val));
        SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLRESETLIMITr(unit, &val));
        soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                          &val, TOTALDYNCELLRESETLIMITf, nreset);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, val));
#endif /* BCM_RAPTOR_SUPPORT*/
    } else if (SOC_IS_FIREBOLT2(unit)) {
#if defined(BCM_FIREBOLT2_SUPPORT)
        SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLLIMITr(unit, &val));
        soc_reg_field_set(unit, TOTALDYNCELLLIMITr,
                          &val, SETLIMITf, n);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLLIMITr(unit, val));
        SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLRESETLIMITr(unit, &val));
        soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                          &val, RESETLIMITf, nreset);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, val));

	/* red and yellow drop limits for total dynamic cells */
	limit_red = n * mmu_red_drop_percent / 100;
	limit_yel = n * mmu_yellow_drop_percent / 100;
	limit_red /= BCM_FB2_CNG_CELL_LIMIT_GRANULARITY;
	limit_yel /= BCM_FB2_CNG_CELL_LIMIT_GRANULARITY;

        val = 0;
        soc_reg_field_set(unit, CNGTOTALDYNCELLLIMIT0r,
                          &val, CNGTOTALDYNCELLLIMITf, limit_red);
        SOC_IF_ERROR_RETURN(WRITE_CNGTOTALDYNCELLLIMIT0r(unit, val));

        val = 0;
        soc_reg_field_set(unit, CNGTOTALDYNCELLLIMIT1r,
                          &val, CNGTOTALDYNCELLLIMITf, limit_yel);
        SOC_IF_ERROR_RETURN(WRITE_CNGTOTALDYNCELLLIMIT1r(unit, val));
#endif /* BCM_FIREBOLT2_SUPPORT */
    } else {
        SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLLIMITr(unit, &val));
        soc_reg_field_set(unit, TOTALDYNCELLLIMITr,
                          &val, TOTALDYNCELLLIMITf, n);
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
    defined(BCM_HELIX15_SUPPORT) 
        if (soc_reg_field_valid(unit, TOTALDYNCELLLIMITr,
                                TOTALDYNCELLRESETLIMITf)) {
            soc_reg_field_set(unit, TOTALDYNCELLLIMITr,
                              &val, TOTALDYNCELLRESETLIMITf, nreset);
        }
#endif /* RAPTOR || FELIX1.5 || HELIX1.5 */
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLLIMITr(unit, val));
    }

    /* per-port dynamic limits */
    PBMP_ALL_ITER(unit, port) {
        if (IS_ST_PORT(unit, port)) {
            n = dcells * mmu_overcommit_stack;
        } else {
	    n = dcells * mmu_overcommit;
	}
	if (n >= ncells) {
	    n = ncells - 1;
	}

#if defined (BCM_HAWKEYE_SUPPORT)
	/* For Hawkeye, the value of field DYNCELLLIMIT of register DYNCELLLIMIT is 
	 * min((ncells - used_cells) ,mmu_dyncellsetlimit). */
	if (SOC_IS_HAWKEYE(unit)) {
		if((ncells - used_cells) > mmu_dyncellsetlimit) {
			n = mmu_dyncellsetlimit;
		} else {
			n = ncells - used_cells;
		}
	}
#endif /* BCM_HAWKEYE_SUPPORT */
        
	nreset = n - mmu_reset_cells;
	if (nreset < (n * MMU_RESET_MIN_PERCENT / 100)) {
	    nreset = n * MMU_RESET_MIN_PERCENT / 100;
	}
	if (nreset <= 0) {
	    nreset = 1;
	}

        SOC_IF_ERROR_RETURN(READ_DYNCELLLIMITr(unit, port, &val));
        if (soc_reg_field_valid(unit, DYNCELLLIMITr, DYNCELLLIMITf)) {
            soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                              DYNCELLLIMITf, n);
        }
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_FELIX_SUPPORT)  || defined(BCM_FIREBOLT2_SUPPORT)
        if (soc_reg_field_valid(unit, DYNCELLLIMITr, DYNCELLSETLIMITf)) {
            soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                              DYNCELLSETLIMITf, n);
        }
        if (soc_reg_field_valid(unit, DYNCELLLIMITr, DYNCELLRESETLIMITf)) {
            soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                              DYNCELLRESETLIMITf, nreset);
        }
#endif /* RAPTOR || HELIX || FELIX || FB2  */
        if (soc_reg_field_valid(unit, DYNCELLLIMITr, DYNCELLRESETLIMITSELf)) {
            soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                              DYNCELLRESETLIMITSELf, 0);    /* 75% */
        }

        SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val));

#if defined(BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit)) {
	    /* red and yellow drop limits for per-port dynamic cells */
	    limit_red = n * mmu_red_drop_percent / 100;
	    limit_yel = n * mmu_yellow_drop_percent / 100;
	    limit_red /= BCM_FB2_CNG_CELL_LIMIT_GRANULARITY;
	    limit_yel /= BCM_FB2_CNG_CELL_LIMIT_GRANULARITY;

            val = 0;
            soc_reg_field_set(unit, CNGDYNCELLLIMIT0r, &val,
                              CNGDYNCELLLIMITf, limit_red);
	    SOC_IF_ERROR_RETURN(WRITE_CNGDYNCELLLIMIT0r(unit, port, val));

            val = 0;
            soc_reg_field_set(unit, CNGDYNCELLLIMIT1r, &val,
                              CNGDYNCELLLIMITf, limit_yel);
            SOC_IF_ERROR_RETURN(WRITE_CNGDYNCELLLIMIT1r(unit, port, val));
        }
#endif

        /*
         * Number of transactions allocated to each port,
         * shared between that port's active cosqs.
         */
        SOC_IF_ERROR_RETURN(soc_fb_xq_mem(unit, port, &xq_mem));
        xq_per_port = soc_mem_index_count(unit, xq_mem);

#if defined (BCM_HAWKEYE_SUPPORT)
        if(SOC_IS_HAWKEYE(unit)) {
            SOC_IF_ERROR_RETURN(WRITE_HOLCOS0MINXQCNTr(unit, port, xq_per_cos));
            SOC_IF_ERROR_RETURN(WRITE_HOLCOS1MINXQCNTr(unit, port, xq_per_cos));
            SOC_IF_ERROR_RETURN(WRITE_HOLCOS2MINXQCNTr(unit, port, xq_per_cos));
            SOC_IF_ERROR_RETURN(WRITE_HOLCOS3MINXQCNTr(unit, port, xq_per_cos));
            n = xq_per_port - (xq_per_cos * numq) -4 -5;
            SOC_IF_ERROR_RETURN(WRITE_DYNXQCNTPORTr(unit, port, n));
            n = n + 8;
            for (cos = 0; cos < NUM_COS(unit); cos++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, n));
                SOC_IF_ERROR_RETURN
                    (WRITE_HOLCOSPKTRESETLIMITr(unit, port, cos, (n - 4)));
            }
        }
#endif /* BCM_HAWKEYE_SUPPORT */

        /*
         * Divide weighted per-port packet limits into each cos.
         * Initialize all COSQs, even the inactive ones.
         */
        for (cos = 0; cos < NUM_COS(unit); cos++) {
	    /* set per-port static limits (calculated earlier) */
            SOC_IF_ERROR_RETURN
                (READ_LWMCOSCELLSETLIMITr(unit, port, cos, &val));
            if (soc_reg_field_valid(unit, LWMCOSCELLSETLIMITr,
                                    CELLSETLIMITf)) {
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr,
                                  &val, CELLSETLIMITf,
				  cos < numq ? scells : 0);
            }
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_FELIX_SUPPORT)  || defined(BCM_FIREBOLT2_SUPPORT)
            if (soc_reg_field_valid(unit, LWMCOSCELLSETLIMITr,
                                    CELLRESETLIMITf)) {
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr,
                                  &val, CELLRESETLIMITf,
				  cos < numq ? sreset : 0);
            }
#endif /* RAPTOR || HELIX || FELIX || FB2  */
            SOC_IF_ERROR_RETURN
                (WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, val));


	    /* set red and yellow drop limits */
	    /* set as a percentage of the green drop (HOL) limit */

            SOC_IF_ERROR_RETURN
		(READ_HOLCOSPKTSETLIMITr(unit, port, cos, &val));
            n = soc_reg_field_get(unit, HOLCOSPKTSETLIMITr, val,
                                    PKTSETLIMITf);

            /* Make sure numq will work with the cosq weighting. */
            if ((cos < numq) && (n == 4)) {
                /*
                 * This means that this COSQ has been
                 * disabled due to XQ weighting, but
                 * has been selected as active. Config error.
                 */
                return BCM_E_PARAM;
            }

	    /* per-queue packet red and yellow drop */
            if (cos < numq) {
                limit_red = n * mmu_red_drop_percent / 100;
                limit_yel = n * mmu_yellow_drop_percent / 100;
            } else {
                limit_red = xq_per_port - 1; /* Max limit (disabled) */
                limit_yel = xq_per_port - 1; /* Max limit (disabled) */
            }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
            /* CNG limit for FB2 is set at 4 packet granularity */
            /* CNG limit for Raven and Hawkeye is set at packet granularity */
            if (SOC_IS_FIREBOLT2(unit)) { /* Granularity limitation */
                limit_red /= BCM_FB2_CNG_PKT_LIMIT_GRANULARITY;
                limit_yel /= BCM_FB2_CNG_PKT_LIMIT_GRANULARITY;
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT */

            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &val,
                              CNGPKTSETLIMIT0f, limit_red);
	    SOC_IF_ERROR_RETURN(WRITE_CNGCOSPKTLIMIT0r(unit, port, cos, val));

            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &val,
                              CNGPKTSETLIMIT1f, limit_yel);
	    SOC_IF_ERROR_RETURN(WRITE_CNGCOSPKTLIMIT1r(unit, port, cos, val));

#if defined(BCM_FIREBOLT2_SUPPORT)
	    /* per-queue cell red and yellow drop */
            if (SOC_IS_FIREBOLT2(unit)) {
		/* disabled (set to maximum) */
		limit_red = BCM_FB2_CNG_CELL_LIMIT_MAX;
		limit_yel = BCM_FB2_CNG_CELL_LIMIT_MAX;
		limit_red /= BCM_FB2_CNG_CELL_LIMIT_GRANULARITY;
		limit_yel /= BCM_FB2_CNG_CELL_LIMIT_GRANULARITY;

                val = 0;
                soc_reg_field_set(unit, CNGCOSCELLLIMIT0r, &val,
                                  CNGCELLSETLIMITf, limit_red);
		SOC_IF_ERROR_RETURN
		    (WRITE_CNGCOSCELLLIMIT0r(unit, port, cos, val));

                val = 0;
                soc_reg_field_set(unit, CNGCOSCELLLIMIT1r, &val,
                                  CNGCELLSETLIMITf, limit_yel);
		SOC_IF_ERROR_RETURN
		    (WRITE_CNGCOSCELLLIMIT1r(unit, port, cos, val));
            }
#endif
        }

	/* cell ingress back pressure */
        if (port == CMIC_PORT(unit)) {
            n = ncells-1;
        } else {
	    /* flow control cells (static + dynamic) */
	    n = scells * mmu_flow_percent / 100;
	    n += dcells;
	    if (n < 0 || n >= ncells) {
		n = ncells - 1;
	    }
        }

        SOC_IF_ERROR_RETURN(READ_IBPCELLSETLIMITr(unit, port, &val));
#if defined (BCM_HAWKEYE_SUPPORT)
        if(SOC_IS_HAWKEYE(unit)) {
            n = mmu_xoff_cell_threshold;
            soc_reg_field_set(unit, IBPCELLSETLIMITr, &val,
                              RESETLIMITSELf, 0); /* 75% */
        }
#endif /* BCM_HAWKEYE_SUPPORT */   
        soc_reg_field_set(unit, IBPCELLSETLIMITr, &val,
                          CELLSETLIMITf, n);
        SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, val));

        SOC_IF_ERROR_RETURN(READ_IBPDISCARDSETLIMITr(unit, port, &val));
        soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &val,
                          DISCARDSETLIMITf, ncells-1);
        SOC_IF_ERROR_RETURN(WRITE_IBPDISCARDSETLIMITr(unit, port, val));

	/* packet ingress back pressure */
	if (port == CMIC_PORT(unit) || mmu_flow_fanin <= 0) {
	    n = xq_per_port - 1;
	} else {
	    n = xq_per_port / numq / mmu_flow_fanin;
	    if (n < MMU_FLOW_PKTMIN) {
		n = MMU_FLOW_PKTMIN;
	    }
	}

        SOC_IF_ERROR_RETURN(READ_IBPPKTSETLIMITr(unit, port, &val));
#if defined (BCM_HAWKEYE_SUPPORT)
        if(SOC_IS_HAWKEYE(unit)) {
            n=mmu_xoff_pkt_threshold;
        }
#endif /* BCM_HAWKEYE_SUPPORT */     
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val,
                          PKTSETLIMITf, n);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val,
                          RESETLIMITSELf, 0);	/* 75% */
        SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, val));
    }

    /* Overall MMU controls */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYNAMIC_MEMORY_ENf, 1);
#if defined(BCM_FIREBOLT2_SUPPORT)
    if (soc_reg_field_valid(unit, MISCCONFIGr, SOP_DROP_ONLY_POLICY_ENf)) {
        soc_reg_field_set(unit, MISCCONFIGr, &val, SOP_DROP_ONLY_POLICY_ENf, 1);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
    defined(BCM_HELIX15_SUPPORT)
    if (soc_reg_field_valid(unit, MISCCONFIGr, HOL_CELL_SOP_DROP_ENf)) {
        soc_reg_field_set(unit, MISCCONFIGr, &val, HOL_CELL_SOP_DROP_ENf, 1);
    }
#endif /* BCM_RAPTOR_SUPPORT || BCM_FELIX15_SUPPORT || BCM_HELIX15_SUPPORT */

    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_fb_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - Draco unit number.
 *      numq - number of COS queues (1-8).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_cosq_config_set(int unit, int numq)
{
    int         cos, prio, ratio, remain;

    SOC_IF_ERROR_RETURN(_bcm_fb_cosq_config_set(unit, numq));

    /* Map the eight 802.1 priority levels to the active cosqs */
    ratio = 8 / numq;
    remain = 8 % numq;
    cos = 0;
    for (prio = 0; prio < 8; prio++) {
        BCM_IF_ERROR_RETURN
            (bcm_fb_er_cosq_mapping_set(unit, -1, prio, cos));
        if ((prio + 1) == (((cos + 1) * ratio) +
                           ((remain < (numq - cos)) ? 0 :
                            (remain - (numq - cos) + 1)))) {
            cos++;
        }
    }

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    SOC_IF_ERROR_RETURN(soc_cosq_stack_port_map_disable(unit));
#endif

    /* use identity mapping for CPU priority mapping */
    SOC_IF_ERROR_RETURN(soc_cpu_priority_mapping_init(unit));

    _num_cosq[unit] = numq;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_cosq_config_get(int unit, int *numq)
{
    if (_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _num_cosq[unit];
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_fb_cosq_port_sched_set
 * Purpose:
 *      Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbm - port bitmap
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *      weights - Weights for each COS queue
 *                Only for BCM_COSQ_WEIGHTED_FAIR_ROUND_ROBIN mode.
 *                For DRR Weight is specified in Kbytes
 *      delay - This parameter is not used in 56504
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                              int mode, const int weights[], int delay)
{
    uint32              wrr;
    int                 port, t;
    uint32              cosarbsel;
    int                 mbits = 0;
    int                 i;
    int                 enc_weights[8];
#if defined (BCM_RAPTOR_SUPPORT)
    int                 MTUQuantaSel = 0;
#endif
#if defined (BCM_FIREBOLT2_SUPPORT)
    int                 MTUQuanta = 0;
#endif

    COMPILER_REFERENCE(delay);

    switch (mode) {
    case BCM_COSQ_STRICT:
        mbits = 0;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        mbits = 1;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        mbits = 2;
        /*
         * All weight values must fit within 4 bits.
         * If weight is 0, this queue is run in strict mode,
         * others run in WRR mode.
         */
#if defined (BCM_HAWKEYE_SUPPORT)
        if(SOC_IS_HAWKEYE(unit)) {
            t = weights[0] | weights[1] | weights[2] | weights[3];
        } else 
#endif /* BCM_HAWKEYE_SUPPORT*/
        {
            t = weights[0] | weights[1] | weights[2] | weights[3] |
                weights[4] | weights[5] | weights[6] | weights[7];
        }

#if defined (BCM_RAPTOR_SUPPORT)
        if ((SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit))) {
            if ((t & ~0x7f) != 0) {
                return BCM_E_PARAM;
            }
        } else 
#endif 
#if defined (BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit)) {
            if ((t & ~0xff) != 0) {
                return BCM_E_PARAM;
            }
        } else 
#endif
        if ((t & ~0xf) != 0) {
            return BCM_E_PARAM;
        }
        for(i = 0; i < NUM_COS(unit); i++) {
            enc_weights[i] = weights[i];
        }
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        mbits = 3;
        if (soc_feature(unit, soc_feature_linear_drr_weight)) {
            if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_HAWKEYE(unit)) {
#if defined (BCM_RAPTOR_SUPPORT)
                MTUQuantaSel = _bcm_rp_cos_drr_weights_to_quanta(weights);
                if (MTUQuantaSel < 0) {
                    return BCM_E_PARAM;
                }
                for (i = 0; i < NUM_COS(unit); i++) {
                    if (weights[i]) {
                        enc_weights[i] =
                            _bcm_rp_cos_drr_kbytes_to_weight(weights[i],
                                                             MTUQuantaSel);
                    } else {
                        enc_weights[i] = weights[i];
                    }
                }
#endif
            } else if (SOC_IS_FIREBOLT2(unit)) {
#if defined (BCM_FIREBOLT2_SUPPORT)
                MTUQuanta = _bcm_fb2_cos_drr_weights_to_quanta(weights);
                if (MTUQuanta < 0) {
                    return BCM_E_PARAM;
                }
                for (i = 0; i < 8; i++) {
                    if (weights[i]) {
                        enc_weights[i] =
                            _bcm_fb2_cos_drr_kbytes_to_weight(weights[i],
                                                             MTUQuanta);
                    } else {
                        enc_weights[i] = weights[i];
                    }

                }
#endif
            }
        } else {
            for(i = 0; i < 8; i++) {
                enc_weights[i] = _bcm_fb_cos_drr_kbytes_to_weight(weights[i]);
            }
        }

        break;
    case BCM_COSQ_BOUNDED_DELAY:        /* not supported in xgs */
    default:
        return BCM_E_PARAM;
    }

    PBMP_ITER(pbm, port) {
        cosarbsel = 0;
        soc_reg_field_set(unit, XQCOSARBSELr, &cosarbsel, COSARBf, mbits);
        if ((mode == BCM_COSQ_DEFICIT_ROUND_ROBIN) &&
            (soc_feature(unit, soc_feature_linear_drr_weight))) {

            if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) {
#if defined (BCM_RAPTOR_SUPPORT)
                soc_reg_field_set(unit, XQCOSARBSELr, &cosarbsel,
                                  MTU_QUANTA_SELECTf, MTUQuantaSel);
#endif
            } else if (SOC_IS_FIREBOLT2(unit)) {
#if defined (BCM_FIREBOLT2_SUPPORT)
                soc_reg_field_set(unit, XQCOSARBSELr, &cosarbsel,
                                  MTU_QUANTAf, (MTUQuanta ==
                                      BCM_FB2_MTU_QUANTA_LARGE) ? 1 : 0);
#endif
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XQCOSARBSELr(unit, port, cosarbsel));
    }

    if ((mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ||
        (mode == BCM_COSQ_DEFICIT_ROUND_ROBIN)) {
        /*
         * Weighted Fair Queueing scheduling among vaild COSs
         */
        PBMP_ITER(pbm, port) {
#if defined (BCM_RAPTOR_SUPPORT) || defined (BCM_FIREBOLT2_SUPPORT)
            if (soc_feature(unit, soc_feature_linear_drr_weight)) {
                for (i = 0; i < NUM_COS(unit); i++) {
                    SOC_IF_ERROR_RETURN(READ_WRRWEIGHT_COSr(unit, port,
                                                            i, &wrr));
    /*    coverity[uninit_use_in_call : FALSE]    */
                    soc_reg_field_set(unit, WRRWEIGHT_COSr, &wrr,
                                      WEIGHTf, enc_weights[i]);
                    SOC_IF_ERROR_RETURN(WRITE_WRRWEIGHT_COSr(unit, port,
                                                             i, wrr));
                }
            } else
#endif
            { /* Firebolt and Helix */
                SOC_IF_ERROR_RETURN(READ_WRRWEIGHTSr(unit, port, &wrr));
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS0WEIGHTf, enc_weights[0]);
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS1WEIGHTf, enc_weights[1]);
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS2WEIGHTf, enc_weights[2]);
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS3WEIGHTf, enc_weights[3]);
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS4WEIGHTf, enc_weights[4]);
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS5WEIGHTf, enc_weights[5]);
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS6WEIGHTf, enc_weights[6]);
                soc_reg_field_set(unit, WRRWEIGHTSr, &wrr,
                                  COS7WEIGHTf, enc_weights[7]);
                SOC_IF_ERROR_RETURN(WRITE_WRRWEIGHTSr(unit, port, wrr));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_cosq_port_sched_get
 * Purpose:
 *      Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit     - StrataSwitch unit number.
 *      pbm      - port bitmap
 *      mode     - (output) Scheduling mode, one of BCM_COSQ_xxx
 *      weights  - (output) Weights for each COS queue
 *                          Only for BCM_COSQ_WEIGHTED_ROUND_ROBIN and
 *                          BCM_COSQ_DEFICIT_ROUND_ROBIN mode.
 *                 For DRR Weight is specified in Kbytes
 *      delay    - This parameter is not used in 56504
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Actually just returns data for the first port in the bitmap
 */

int
bcm_fb_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                              int *mode, int weights[], int *delay)
{
    uint32              cosarbsel, wrr;
    int                 mbits, port;
#if defined (BCM_RAPTOR_SUPPORT)
    int                 MTUQuantaSel = -1;
#endif
#if defined (BCM_FIREBOLT2_SUPPORT)
    int                 MTUQuanta = -1;
#endif

    mbits = -1;
    PBMP_ITER(pbm, port) {
        SOC_IF_ERROR_RETURN(READ_XQCOSARBSELr(unit, port, &cosarbsel));
        mbits = soc_reg_field_get(unit, XQCOSARBSELr, cosarbsel, COSARBf);
        if (soc_feature(unit, soc_feature_linear_drr_weight)) {
            if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_HAWKEYE(unit)) {
#if defined (BCM_RAPTOR_SUPPORT)
                MTUQuantaSel =
                    soc_reg_field_get(unit, XQCOSARBSELr, cosarbsel,
                                      MTU_QUANTA_SELECTf);
#endif
            } else if (SOC_IS_FIREBOLT2(unit)) {
#if defined (BCM_FIREBOLT2_SUPPORT)
                MTUQuanta = soc_reg_field_get(unit, XQCOSARBSELr,
                                              cosarbsel, MTU_QUANTAf) ?
                    BCM_FB2_MTU_QUANTA_LARGE : BCM_FB2_MTU_QUANTA_SMALL;
#endif
            }
        }
        break;
    }

    switch (mbits) {
    case 0:
        *mode = BCM_COSQ_STRICT;
        break;
    case 1:
        *mode = BCM_COSQ_ROUND_ROBIN;
        break;
    case 2:
        *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
    case 3:
        *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    if ((mbits == 2) || (mbits == 3)) {
        wrr = 0;
#if defined (BCM_RAPTOR_SUPPORT) || defined (BCM_FIREBOLT2_SUPPORT)
        if (soc_feature(unit, soc_feature_linear_drr_weight)) {
            int i;

            PBMP_ITER(pbm, port) {
                for (i = 0; i < NUM_COS(unit); i++) {
                    SOC_IF_ERROR_RETURN(READ_WRRWEIGHT_COSr(unit, port,
                                                            i, &wrr));
                    weights[i] = soc_reg_field_get(unit, WRRWEIGHT_COSr,
                                                   wrr, WEIGHTf);
                }
            }
        } else
#endif
        {
            PBMP_ITER(pbm, port) {
                SOC_IF_ERROR_RETURN(READ_WRRWEIGHTSr(unit, port, &wrr));
                break;
            }
            weights[0] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS0WEIGHTf);
            weights[1] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS1WEIGHTf);
            weights[2] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS2WEIGHTf);
            weights[3] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS3WEIGHTf);
            weights[4] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS4WEIGHTf);
            weights[5] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS5WEIGHTf);
            weights[6] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS6WEIGHTf);
            weights[7] = soc_reg_field_get(unit, WRRWEIGHTSr, wrr, COS7WEIGHTf);
        }
        if (mbits == 3) {
            int i;
            for(i = 0; i < NUM_COS(unit); i++) {
                if (soc_feature(unit, soc_feature_linear_drr_weight)) {
                    if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) ||
                        SOC_IS_HAWKEYE(unit)) {
#if defined (BCM_RAPTOR_SUPPORT)
                        weights[i] =
                            _bcm_rp_cos_drr_weight_to_kbytes(weights[i],
                                                             MTUQuantaSel);
#endif
                    } else if (SOC_IS_FIREBOLT2(unit)) {
#if defined (BCM_FIREBOLT2_SUPPORT)
                        weights[i] *= MTUQuanta;
#endif
                    }
                } else {
                    weights[i] =
                        _bcm_fb_cos_drr_weight_to_kbytes(weights[i]);
                }
            }
        }
    }

    if (delay) {
        *delay = 0;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_cosq_sched_weight_max_get
 * Purpose:
 *      Retrieve maximum weights for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *      weight_max - (output) Maximum weight for COS queue.
 *              For DRR mode Weight is specified in Kbytes
 *              0 if mode is BCM_COSQ_STRICT.
 *              1 if mode is BCM_COSQ_ROUND_ROBIN.
 *              -1 if not applicable to mode.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        *weight_max = BCM_COSQ_WEIGHT_MIN;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        *weight_max = FB_WRR_WEIGHT_MAX;
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        if (soc_feature(unit, soc_feature_linear_drr_weight)) {
            if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_HAWKEYE(unit)) {
#if defined (BCM_RAPTOR_SUPPORT)
                *weight_max = BCM_COS_WEIGHT_MAX *
                    MTU_Quanta[BCM_MTU_QUANTA_ID_COUNT - 1];
#endif
            } else if (SOC_IS_FIREBOLT2(unit)) {
#if defined (BCM_FIREBOLT2_SUPPORT)
                *weight_max =
                    BCM_FB2_COS_WEIGHT_MAX * BCM_FB2_MTU_QUANTA_LARGE;
#endif
            }
        } else {
            *weight_max =
                _bcm_fb_cos_drr_weight_to_kbytes(FB_DRR_WEIGHT_MAX);
        }
        break;
    default:
        *weight_max = BCM_COSQ_WEIGHT_UNLIMITED;
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#define FB2_GRANULARITY_NUM      3 
#define FB_BW_GRANULARITY        64
#define FB_BW_FIELD_MAX          0x3ffff

int
bcm_fb_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 kbits_sec_min,
                               uint32 kbits_sec_max,
                               uint32 kbits_sec_burst,
                               uint32 flags)
{
    uint32 refresh_rate;
    uint32 regval;
    uint32 bucket_val, regindex;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    int    refresh_bitsize, bucket_bitsize;
    uint32 bucketsize, granularity = 3, meter_flags = 0;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

    if (soc_feature(unit,soc_feature_bucket_support)) {
        /*
         * To set the new Bandwidth settings, the procedure adopted is
         * a. reset MAXBUCKETCONFIG, MINBUCKETCONFIG, MAXBUCKET,MINBUCKET
         * b. update MAXBUCKETCONFIG and MINBUCKETCONFIG with new values passed
         * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
         */

#if defined (BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit)) {
            BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
            if (soc_reg_field_get(unit, MISCCONFIGr,
                                  regval, ITU_MODE_SELf)) {
                meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
            }
        }
#endif

        BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &regval));

        /* Disable egress metering for this port */
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, MAX_REFRESHf, 0);
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, MAX_THD_SELf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIGr(unit, port, cosq, regval));

#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit)) {
            switch (cosq) {
                case 0:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS0CONFIGr(unit, port, 0));
                        break;
                case 1:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS1CONFIGr(unit, port, 0));
                        break;
                case 2:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS2CONFIGr(unit, port, 0)); 
                        break;
                case 3:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS3CONFIGr(unit, port, 0));
                        break;
                default:	return BCM_E_PARAM;
            }
        } else
#endif /* BCM_HAWKEYE_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIGr(unit, port, cosq, &regval));
            soc_reg_field_set(unit, MINBUCKETCONFIGr, &regval, MIN_REFRESHf, 0);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit)) {
                soc_reg_field_set(unit, MINBUCKETCONFIGr,
                                  &regval, MIN_THD_SELf, 0);
            } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */
            {
                soc_reg_field_set(unit, MINBUCKETCONFIGr,
                                  &regval, MIN_HI_THD_SELf, 0);
                soc_reg_field_set(unit, MINBUCKETCONFIGr,
                                  &regval, MIN_LO_THD_SELf, 0);
            }
            BCM_IF_ERROR_RETURN(WRITE_MINBUCKETCONFIGr(unit, port, cosq, regval));
        }
        /*reset the MAXBUCKET register*/
        bucket_val = 0;
        soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, MAX_BUCKETf,0);
        soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, IN_PROFILE_FLAGf,1);
        BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETr(unit, port, cosq, bucket_val));

        /*reset the MINBUCKET register value*/
        bucket_val = 0;
        soc_reg_field_set(unit, MINBUCKETr, &bucket_val, MIN_BUCKETf,0);
        soc_reg_field_set(unit, MINBUCKETr, &bucket_val, IN_PROFILE_FLAGf,0);
        BCM_IF_ERROR_RETURN(WRITE_MINBUCKETr(unit, port, cosq, bucket_val));

        regval = 0;
#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit)) {
            switch (cosq) {
                case 0:	regindex = MINBUCKETCOS0CONFIGr; break;
                case 1:	regindex = MINBUCKETCOS1CONFIGr; break;
                case 2:	regindex = MINBUCKETCOS2CONFIGr; break;
                case 3:	regindex = MINBUCKETCOS3CONFIGr; break;
                default:	return BCM_E_PARAM;
            }
        } else
#endif /* BCM_HAWKEYE_SUPPORT */
        {
            regindex = MINBUCKETCONFIGr;
        }

        
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
            SOC_IS_HAWKEYE(unit)) {
            refresh_bitsize =
                soc_reg_field_length(unit, regindex, MIN_REFRESHf);
            bucket_bitsize =
                soc_reg_field_length(unit, regindex, MIN_THD_SELf);

            BCM_IF_ERROR_RETURN
                (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_min,
                                                   kbits_sec_min,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

            /* regval has MINBUCKETCONFIGr value */
            soc_reg_field_set(unit, regindex, &regval,
                              MIN_REFRESHf, refresh_rate);
            soc_reg_field_set(unit, regindex,
                              &regval, MIN_THD_SELf, bucketsize);
            if (soc_reg_field_valid(unit, regindex, METER_GRANf)) {
                soc_reg_field_set(unit, regindex, &regval,
                                  METER_GRANf, granularity);
            }
        } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */
        {
            /* Check kbits_sec_min upper limit prior to "granularization" */
            if (kbits_sec_min > (0xFFFFFFFF - (FB_BW_GRANULARITY - 1)) ) {
                kbits_sec_min = (0xFFFFFFFF - (FB_BW_GRANULARITY - 1));
            }

            refresh_rate =
                (kbits_sec_min + (FB_BW_GRANULARITY - 1)) /
                FB_BW_GRANULARITY;
            if (refresh_rate > FB_BW_FIELD_MAX) {
                refresh_rate = FB_BW_FIELD_MAX;
            }

            /* regval has MINBUCKETCONFIGr value */
            soc_reg_field_set(unit, regindex, &regval,
                              MIN_REFRESHf, refresh_rate);
            soc_reg_field_set(unit, regindex,
                              &regval, MIN_HI_THD_SELf,
                              _bcm_fb_kbits_to_bucketsize(kbits_sec_min));
            soc_reg_field_set(unit, regindex,
                              &regval, MIN_LO_THD_SELf,
                              _bcm_fb_kbits_to_bucketsize(kbits_sec_min/2));
        }
#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit)) {
            switch (cosq) {
                case 0:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS0CONFIGr(unit, port, regval));
                        break;
                case 1:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS1CONFIGr(unit, port, regval));
                        break;
                case 2:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS2CONFIGr(unit, port, regval)); 
                        break;
                case 3:	BCM_IF_ERROR_RETURN
                            (WRITE_MINBUCKETCOS3CONFIGr(unit, port, regval));
                        break;
                default:	return BCM_E_PARAM;
            }
	} else
#endif /* BCM_HAWKEYE_SUPPORT */
	{
            BCM_IF_ERROR_RETURN
                (WRITE_MINBUCKETCONFIGr(unit, port, cosq, regval));
        }
        regval=0;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
            SOC_IS_HAWKEYE(unit)) {
            refresh_bitsize =
                soc_reg_field_length(unit, MAXBUCKETCONFIGr, MAX_REFRESHf);
            bucket_bitsize =
                soc_reg_field_length(unit, MAXBUCKETCONFIGr, MAX_THD_SELf);

            BCM_IF_ERROR_RETURN
                (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_max,
                                                   kbits_sec_burst,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

            /* regval has MAXBUCKETCONFIGr value */
            soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval,
                              MAX_REFRESHf, refresh_rate);
            soc_reg_field_set(unit, MAXBUCKETCONFIGr,
                              &regval, MAX_THD_SELf, bucketsize);
            if (soc_reg_field_valid(unit, MAXBUCKETCONFIGr, METER_GRANf)) {
                soc_reg_field_set(unit, MAXBUCKETCONFIGr,
                                  &regval, METER_GRANf, granularity);
            }
        } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */
        {
            /* Check kbits_sec_min upper limit prior to "granularization" */
            if (kbits_sec_max > (0xFFFFFFFF - (FB_BW_GRANULARITY - 1)) ) {
                kbits_sec_max = (0xFFFFFFFF - (FB_BW_GRANULARITY - 1));
            }

            refresh_rate =
                (kbits_sec_max + (FB_BW_GRANULARITY - 1)) /
                FB_BW_GRANULARITY;
            if (refresh_rate > FB_BW_FIELD_MAX) {
                refresh_rate = FB_BW_FIELD_MAX;
            }

            /* regval has MAXBUCKETCONFIGr value */
            soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval,
                              MAX_REFRESHf, refresh_rate);
            soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, MAX_THD_SELf,
                              _bcm_fb_kbits_to_bucketsize(kbits_sec_burst));
        }
        BCM_IF_ERROR_RETURN
            (WRITE_MAXBUCKETCONFIGr(unit, port, cosq, regval));

        /* MISCCONFIG.METERING_CLK_EN is set by chip init */

        return BCM_E_NONE;
    } else {
        return BCM_E_UNAVAIL;
    }

}

int
bcm_fb_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *kbits_sec_min,
                               uint32 *kbits_sec_max,
                               uint32 *kbits_sec_burst,
                               uint32 *flags)
{
    uint32 regval, regindex, refresh_rate;
    uint32 bucketsize = 0;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    uint32 kbits_burst, meter_flags = 0;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

    if (soc_feature(unit,soc_feature_bucket_support)){

        if (!kbits_sec_min || !kbits_sec_max || !flags) {
            return (BCM_E_PARAM);
        }

#if defined (BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit)) {
            BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
            if (soc_reg_field_get(unit, MISCCONFIGr,
                                  regval, ITU_MODE_SELf)) {
                meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
            }
        }
#endif

        BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &regval));
        refresh_rate =
            soc_reg_field_get(unit, MAXBUCKETCONFIGr, regval, MAX_REFRESHf);

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
            SOC_IS_HAWKEYE(unit)) {
            bucketsize =
                soc_reg_field_get(unit, MAXBUCKETCONFIGr,
                                  regval, MAX_THD_SELf);
            BCM_IF_ERROR_RETURN
                (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                                   FB2_GRANULARITY_NUM,
                                                   meter_flags,
                                                   kbits_sec_max,
                                                   kbits_sec_burst));
        } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */
        {
            bucketsize =
                soc_reg_field_get(unit, MAXBUCKETCONFIGr,
                                  regval, MAX_THD_SELf);
            /* Convert the REFRESH field to kbits/sec (1000 bits/sec). */
            *kbits_sec_max = FB_BW_GRANULARITY * refresh_rate;
            *kbits_sec_burst = _bcm_fb_bucketsize_to_kbits(bucketsize);
        }

#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit)) {
            switch (cosq) {
                case 0:	regindex = MINBUCKETCOS0CONFIGr;
                        BCM_IF_ERROR_RETURN
                            (READ_MINBUCKETCOS0CONFIGr(unit, port, &regval));
                        break;
                case 1:	regindex = MINBUCKETCOS1CONFIGr;
                        BCM_IF_ERROR_RETURN
                            (READ_MINBUCKETCOS1CONFIGr(unit, port, &regval));
                        break;
                case 2:	regindex = MINBUCKETCOS2CONFIGr;
                        BCM_IF_ERROR_RETURN
                            (READ_MINBUCKETCOS2CONFIGr(unit, port, &regval)); 
                        break;
                case 3:	regindex = MINBUCKETCOS3CONFIGr;
                        BCM_IF_ERROR_RETURN
                            (READ_MINBUCKETCOS3CONFIGr(unit, port, &regval));
                        break;
                default:	return BCM_E_PARAM;
            }
        } else
#endif /* BCM_HAWKEYE_SUPPORT */
        {
            regindex = MINBUCKETCONFIGr;
            BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIGr(unit, port, cosq, &regval));
        }

        refresh_rate =
            soc_reg_field_get(unit, regindex, regval, MIN_REFRESHf);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
            SOC_IS_HAWKEYE(unit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, 0,
                                                   FB2_GRANULARITY_NUM,
                                                   meter_flags,
                                                   kbits_sec_min,
                                                   &kbits_burst));
        } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */
        {
        /* Convert the REFRESH field to kbits/sec (1000 bits/sec). */
            *kbits_sec_min = FB_BW_GRANULARITY * refresh_rate;
        }

        *flags = 0;

        return BCM_E_NONE;
    } else {
        return BCM_E_UNAVAIL;
    }
}

#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
/*      
 *  Convert HW drop probability to percent value
 */     
STATIC int       
_bcm_rv_hw_drop_prob_to_api_val[] = {
    1000,  /* 0  */
    125,   /* 1  */
    63,    /* 2  */
    31,    /* 3  */
    16,    /* 4  */
    8,     /* 5  */
    4,     /* 6  */
    2,     /* 7  */
};

STATIC int
_bcm_rv_api_val_to_hw_drop_prob(int api_val) {
   int i;
   for (i = 7; i > 0 ; i--) {
      if (api_val <= _bcm_rv_hw_drop_prob_to_api_val[i]) {
          break;
      }
   }
   return i;
}

STATIC soc_field_t
_cosfld[] = {
                COS0THDMODEf,
                COS1THDMODEf,
                COS2THDMODEf,
                COS3THDMODEf,
                COS4THDMODEf,
                COS5THDMODEf,
                COS6THDMODEf,
                COS7THDMODEf,
            };

STATIC int
bcm_rv_cosq_discard_port_set(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int drop_start,
                             int drop_slope)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int i, cosq_start, num_cosq;
    uint32 min_thresh, n;
    uint32 rval, encoding;
    soc_reg_t reg;
    soc_field_t fld;

    if (drop_start < 0 || drop_start > 100 || drop_slope > 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &local_port));
        BCM_PBMP_PORT_SET(pbmp, local_port);
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }
    }

    if (cosq < -1 || cosq >= _num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        cosq_start = 0;
        num_cosq = _num_cosq[unit];
    } else {
        cosq_start = cosq;
        num_cosq = 1;
    }

    BCM_PBMP_ITER(pbmp, local_port) {
        for (i = cosq_start; i < (cosq_start + num_cosq); i++) {
            /* Get the start point as a function of the HOL limit */
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTSETLIMITr(unit, local_port, i, &rval));
            n = soc_reg_field_get(unit, HOLCOSPKTSETLIMITr, rval,
                                  PKTSETLIMITf);
            min_thresh = drop_start * n / 100;

            /* Set the drop rate depending on the color */
            if (color == BCM_COSQ_DISCARD_COLOR_GREEN) {
                reg = CNG1COSDROPRATEr;
            } else {
                reg = CNG0COSDROPRATEr;
            }
            BCM_IF_ERROR_RETURN
                (soc_reg32_read(unit, soc_reg_addr
                                (unit, reg, local_port, i), &rval));
            encoding = _bcm_rv_api_val_to_hw_drop_prob(-1 * drop_slope);
            soc_reg_field_set(unit, reg, &rval, DROPRATEf, encoding);
            BCM_IF_ERROR_RETURN
                (soc_reg32_write(unit, soc_reg_addr
                                 (unit, reg, local_port, i), rval));
            /* Enable simple RED for the (port, cos) pair */
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, SIMPLEREDCONFIGr, local_port,
                                        _cosfld[i], 1));

            /* Set the drop threshold depending on the color */
            if (color == BCM_COSQ_DISCARD_COLOR_GREEN) {
                reg = CNGCOSPKTLIMIT1r;
                fld = CNGPKTSETLIMIT1f;
            } else {
                reg = CNGCOSPKTLIMIT0r;
                fld = CNGPKTSETLIMIT0f;
            }
            soc_reg_field_set(unit, reg, &rval, fld, min_thresh);
            BCM_IF_ERROR_RETURN
                (soc_reg32_write(unit, soc_reg_addr
                                 (unit, reg, local_port, i), rval));
        }
        /* For Firebolt 2, also match the cell limits to the drop percentage.
         * Dynamic cell usage is enabled by default */
#if defined(BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit)) {
            /* Fraction of per-port DYNCELLLIMIT */
            BCM_IF_ERROR_RETURN(READ_DYNCELLLIMITr(unit, local_port, &rval));
            n = soc_reg_field_get(unit, DYNCELLLIMITr, rval, DYNCELLSETLIMITf);
            min_thresh = drop_start * n / 100;
            /* Set the drop threshold depending on the color */
            if (color == BCM_COSQ_DISCARD_COLOR_GREEN) {
                reg = CNGDYNCELLLIMIT1r;
                fld = CNGDYNCELLLIMITf;
            } else {
                reg = CNGDYNCELLLIMIT0r;
                fld = CNGDYNCELLLIMITf;
            }
            soc_reg_field_set(unit, reg, &rval, fld, min_thresh);
            BCM_IF_ERROR_RETURN
                (soc_reg32_write(unit, soc_reg_addr
                                 (unit, reg, local_port, 0), rval));
        }
#endif
    }
    return BCM_E_NONE;
}

STATIC int
bcm_rv_cosq_discard_port_get(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int *drop_start,
                             int *drop_slope)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    uint32 rval, n;

    if (drop_start == NULL || drop_slope == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &local_port));
        BCM_PBMP_PORT_SET(pbmp, local_port);
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }
    }
    if (cosq < -1 || cosq >= _num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        cosq = 0;
    }
   
    BCM_PBMP_ITER(pbmp, local_port) {
        /* Get the total HOL limit first */
        BCM_IF_ERROR_RETURN
            (READ_HOLCOSPKTSETLIMITr(unit, local_port, cosq, &rval));
        n = soc_reg_field_get(unit, HOLCOSPKTSETLIMITr, rval, PKTSETLIMITf);

        /* Get the simple RED mode */
        BCM_IF_ERROR_RETURN(READ_SIMPLEREDCONFIGr(unit, local_port, &rval));
        if (soc_reg_field_get(unit, SIMPLEREDCONFIGr, rval, _cosfld[cosq])) {
            if (color == BCM_COSQ_DISCARD_COLOR_GREEN) {
                BCM_IF_ERROR_RETURN
                    (READ_CNGCOSPKTLIMIT1r(unit, local_port, cosq, &rval));
                *drop_start = soc_reg_field_get(unit, CNGCOSPKTLIMIT1r, rval, 
                                                CNGPKTSETLIMIT1f) * 100 / n; 
                BCM_IF_ERROR_RETURN
                    (READ_CNG1COSDROPRATEr(unit, local_port, cosq, &rval));
                *drop_slope = -1 * _bcm_rv_hw_drop_prob_to_api_val[rval];
            } else {
                BCM_IF_ERROR_RETURN
                    (READ_CNGCOSPKTLIMIT0r(unit, local_port, cosq, &rval));
                *drop_start = soc_reg_field_get(unit, CNGCOSPKTLIMIT0r, rval, 
                                                CNGPKTSETLIMIT0f) * 100 / n; 
                BCM_IF_ERROR_RETURN
                    (READ_CNG0COSDROPRATEr(unit, local_port, cosq, &rval));
                *drop_slope = -1 * _bcm_rv_hw_drop_prob_to_api_val[rval];
            }
        }
        break;
    }
    return BCM_E_NONE;
}
#endif

int
bcm_fb_cosq_discard_set(int unit, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_fb_cosq_discard_get(int unit, uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_fb_cosq_discard_port_set(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int drop_start,
                             int drop_slope,
                             int average_time)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_RAVEN(unit) || SOC_IS_FIREBOLT2(unit)) {
        rv = bcm_rv_cosq_discard_port_set(unit, port, cosq, color, 
                                          drop_start, drop_slope);
    }
#if defined(BCM_FIREBOLT2_SUPPORT)
    /* For Firebolt 2, also match the cell limits to the drop percentage. 
     * Dynamic cell usage is enabled by default. */
    if (BCM_SUCCESS(rv) && SOC_IS_FIREBOLT2(unit)) {
        uint32 min_thresh, n, rval;
        soc_reg_t reg;
        soc_field_t fld;
        /* Fraction of TOTALDYNCELLLIMIT */
        BCM_IF_ERROR_RETURN(READ_TOTALDYNCELLLIMITr(unit, &rval));
        n = soc_reg_field_get(unit, TOTALDYNCELLLIMITr, rval, SETLIMITf);
        min_thresh = drop_start * n / 100;                  
        /* Set the drop threshold depending on the color */ 
        if (color == BCM_COSQ_DISCARD_COLOR_GREEN) {
            reg = CNGTOTALDYNCELLLIMIT1r;
            fld = CNGTOTALDYNCELLLIMITf;
        } else {
            reg = CNGTOTALDYNCELLLIMIT0r;
            fld = CNGTOTALDYNCELLLIMITf;
        }
        soc_reg_field_set(unit, reg, &rval, fld, min_thresh);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));     
    }
#endif
#endif
    return rv;
}

int
bcm_fb_cosq_discard_port_get(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int *drop_start,
                             int *drop_slope,
                             int *average_time)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_RAVEN(unit) || SOC_IS_FIREBOLT2(unit)) {
        rv = bcm_rv_cosq_discard_port_get(unit, port, cosq, color, 
                                          drop_start, drop_slope);
    }
#endif
    return rv;
}

int
bcm_fb_cosq_detach(int unit, int software_state_only)
{
    return BCM_E_NONE;
}
#endif /* BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      bcm_fb_er_cosq_mapping_set
 * Purpose:
 *      Set which cosq a given priority should fall into
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_cosq_mapping_set(int unit, bcm_port_t port,
                           bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    uint32 cval32, oval32;
    soc_field_t f;
    bcm_pbmp_t ports;

    switch (priority) {
    case 0:	f = COS0f; break;
    case 1:	f = COS1f; break;
    case 2:	f = COS2f; break;
    case 3:	f = COS3f; break;
    case 4:	f = COS4f; break;
    case 5:	f = COS5f; break;
    case 6:	f = COS6f; break;
    case 7:	f = COS7f; break;
    default:	return BCM_E_PARAM;
    }

    if (cosq < 0 || cosq >= NUM_COS(unit)) {
        return (BCM_E_PARAM);
    }

    if (port == -1) {	/* all ports */
	BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    } else if (SOC_PORT_VALID(unit, port) && IS_ALL_PORT(unit, port)) {
	BCM_PBMP_CLEAR(ports);
	BCM_PBMP_PORT_ADD(ports, port);
    } else {
	return BCM_E_PORT;
    }

    PBMP_ITER(ports, port) {
	SOC_IF_ERROR_RETURN(READ_COS_SELr(unit, port, &cval32));
	oval32 = cval32;
	soc_reg_field_set(unit, COS_SELr, &cval32, f, cosq);
	if (cval32 != oval32) {
	    SOC_IF_ERROR_RETURN(WRITE_COS_SELr(unit, port, cval32));
	}
	if (port == CMIC_PORT(unit)) {
	    SOC_IF_ERROR_RETURN(READ_CPU_COS_SELr(unit, &cval32));
	    oval32 = cval32;
	    soc_reg_field_set(unit, CPU_COS_SELr, &cval32, f, cosq);
	    if (cval32 != oval32) {
		SOC_IF_ERROR_RETURN(WRITE_CPU_COS_SELr(unit, cval32));
	    }
	}
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
	if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
	    SOC_IF_ERROR_RETURN(READ_ICOS_SELr(unit, port, &cval32));
	    oval32 = cval32;
	    soc_reg_field_set(unit, ICOS_SELr, &cval32, f, cosq);
	    if (cval32 != oval32) {
		SOC_IF_ERROR_RETURN(WRITE_ICOS_SELr(unit, port, cval32));
	    }
	}
#endif
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_cosq_mapping_get
 * Purpose:
 *      Determine which COS queue a given priority currently maps to.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value
 *      cosq - (Output) COS queue number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_cosq_mapping_get(int unit, bcm_port_t port,
                           bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    uint32 cval32;
    soc_field_t f;

    switch (priority) {
    case 0:	f = COS0f; break;
    case 1:	f = COS1f; break;
    case 2:	f = COS2f; break;
    case 3:	f = COS3f; break;
    case 4:	f = COS4f; break;
    case 5:	f = COS5f; break;
    case 6:	f = COS6f; break;
    case 7:	f = COS7f; break;
    default:	return BCM_E_PARAM;
    }

    if (port == -1) {
	port = REG_PORT_ANY;
    } else if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
	return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_COS_SELr(unit, port, &cval32));
    *cosq = soc_reg_field_get(unit, COS_SELr, cval32, f);

    return BCM_E_NONE;
}

#ifdef BCM_FIREBOLT2_SUPPORT

#define FB2_PKT_REFRESH_MAX      0xfffff
#define FB2_PKT_THD_MAX          0xfff

STATIC int
_bcm_fb2_cosq_port_packet_bandwidth_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq,
                                        int pps, int burst)
{
    uint32 regval;
    soc_reg_t maxbucket_config_reg, maxbucket_reg;
    soc_field_t refresh_f, thd_sel_f, bucket_f;

    if (cosq < 0) {
        maxbucket_config_reg = PKTPORTMAXBUCKETCONFIGr;
        maxbucket_reg = PKTPORTMAXBUCKETr;
        refresh_f = PKT_PORT_MAX_REFRESHf;
        thd_sel_f = PKT_PORT_MAX_THD_SELf;
        bucket_f = PKT_PORT_MAX_BUCKETf;
        cosq = 0;
    } else {
        maxbucket_config_reg = PKTMAXBUCKETCONFIGr;
        maxbucket_reg = PKTMAXBUCKETr;
        refresh_f = PKT_MAX_REFRESHf;
        thd_sel_f = PKT_MAX_THD_SELf;
        bucket_f = PKT_MAX_BUCKETf;
    }

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG and MAXBUCKET
     * b. update MAXBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    /* Disable egress metering */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, maxbucket_config_reg, port, cosq, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, 0);
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, thd_sel_f, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, maxbucket_config_reg, port, cosq, regval));

    /* reset the MAXBUCKET register*/
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, maxbucket_reg, port, cosq, &regval));
    soc_reg_field_set(unit, maxbucket_reg, &regval, bucket_f, 0);
    soc_reg_field_set(unit, maxbucket_reg, &regval,IN_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, maxbucket_reg, port, cosq, regval));

    /* Check packets-per second upper limit */
    if (pps > FB2_PKT_REFRESH_MAX) {
       pps = FB2_PKT_REFRESH_MAX;
    }

    /* Check burst upper limit */
    if (burst > FB2_PKT_THD_MAX) {
       burst = FB2_PKT_THD_MAX;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, maxbucket_config_reg, port, cosq, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, pps);
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, thd_sel_f, burst);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, maxbucket_config_reg, port, cosq, regval));

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

STATIC int
_bcm_fb2_cosq_port_packet_bandwidth_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq,
                                        int *pps, int *burst)
{
    uint32 regval;
    soc_reg_t maxbucket_config_reg;
    soc_field_t refresh_f, thd_sel_f;

    if (cosq < 0) {
        maxbucket_config_reg = PKTPORTMAXBUCKETCONFIGr;
        refresh_f = PKT_PORT_MAX_REFRESHf;
        thd_sel_f = PKT_PORT_MAX_THD_SELf;
        cosq = 0;
    } else {
        maxbucket_config_reg = PKTMAXBUCKETCONFIGr;
        refresh_f = PKT_MAX_REFRESHf;
        thd_sel_f = PKT_MAX_THD_SELf;
    }

    /* Disable egress metering */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, maxbucket_config_reg, port, cosq, &regval));
    *pps = soc_reg_field_get(unit, maxbucket_config_reg, regval, refresh_f);
    *burst = soc_reg_field_get(unit, maxbucket_config_reg, regval, thd_sel_f);

    return BCM_E_NONE;
}

int
bcm_fb2_cosq_port_pps_set(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq, int pps)
{
    int temp_pps, burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_fb2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                 &temp_pps, &burst));

    /* Replace the current PPS setting, keep BURST the same */
    return _bcm_fb2_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                                   pps, burst);
}

int
bcm_fb2_cosq_port_pps_get(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq, int *pps)
{
    int burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    return _bcm_fb2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                   pps, &burst);
}

int
bcm_fb2_cosq_port_burst_set(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, int burst)
{
    int pps, temp_burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_fb2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                 &pps, &temp_burst));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_fb2_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                                   pps, burst);
}

int
bcm_fb2_cosq_port_burst_get(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, int *burst)
{
    int pps;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    return _bcm_fb2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                   &pps, burst);
}
#endif /* BCM_FIREBOLT2_SUPPORT */


#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_fb_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_fb_cosq_sw_dump(int unit)
{
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "    Number: %d\n"), _num_cosq[unit]));

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */
