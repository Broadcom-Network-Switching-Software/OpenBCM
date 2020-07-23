/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 *
 * Bradley supports two additional COS queues for high priority flow 
 * control (QM queue) and system control (SC queue.) Currently API
 * access to these two queues is blocked since they do not participate
 * in regular queue scheduling (they are both always using strict
 * priority.) Likewise, internal priorities 14 and 15 are reserved
 * for use with these two COS queues. This means that the COS mapping 
 * API will allow mapping of 14 internal priorities to the 8 normal 
 * COS queues.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/defs.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/bradley.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define HB_DRR_WEIGHT_MAX       0x7f
#define HB_WRR_WEIGHT_MAX       0x7f

#define HB_SC_COS               14
#define HB_QM_COS               15

static const soc_field_t cpu_prio_field[16] = { 
    PRI0f,  PRI1f,  PRI2f,  PRI3f,  PRI4f,  PRI5f,  PRI6f,  PRI7f, 
    PRI8f,  PRI9f,  PRI10f, PRI11f, PRI12f, PRI13f, PRI14f, PRI15f
};

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE

static const soc_field_t cos_field[16] = { 
    COS0f,  COS1f,  COS2f,  COS3f,  COS4f,  COS5f,  COS6f,  COS7f, 
    COS8f,  COS9f,  COS10f, COS11f, COS12f, COS13f, COS14f, COS15f
};

static const soc_field_t hg_cos_val[16] = { 
    0,      1,      2,      3,      4,      5,      6,      7, 
    7,      7,      7,      7,      7,      7,      14,     15
};

#endif


static int _num_cosq[SOC_MAX_NUM_DEVICES];
static int _max_cosq = -1;

/*
 * Convert the number of kbytes (1024 bytes) that can transmtted in one run
 * to weight encoding as per table above
 */
static int
_bcm_hb_cos_drr_kbytes_to_weight(int kbytes)
{
    int weight;

    /* 1 weight-unit ~ 128 bytes */
    weight = 8 * kbytes;

    if (weight > HB_DRR_WEIGHT_MAX) {
        weight = HB_DRR_WEIGHT_MAX;
    }

    return weight;
}

/*
 * Convert the encoded weights to number of kbytes that can transmtted
 * in one run.
 */
static int
_bcm_hb_cos_drr_weight_to_kbytes(int weight)
{
    assert(weight <= HB_DRR_WEIGHT_MAX);

    /* 1 weight-unit ~ 128 bytes */
    return (weight / 8);
}


#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      bcm_bradley_cosq_sync
 * Purpose:
 *      Record COSQ module persistent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_bradley_cosq_sync(int unit)
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
 *      _bcm_bradley_cosq_reinit
 * Purpose:
 *      Recover COSQ software state.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_bradley_cosq_reinit(int unit)
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
        _num_cosq[unit] = _bcm_esw_cosq_config_property_get(unit);
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_bradley_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_bradley_cosq_init(int unit)
{
    int         num_cos;
#ifdef BCM_WARM_BOOT_SUPPORT
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint32              cosq_scache_size;
    uint8               *cosq_scache_ptr;
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (_max_cosq < 0) {
        _max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }

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
        return _bcm_bradley_cosq_reinit(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    num_cos = _bcm_esw_cosq_config_property_get(unit);

    return bcm_bradley_cosq_config_set(unit, num_cos);
}

/*
 * Function:
 *      bcm_cosq_detach
 * Purpose:
 *      Discard all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_bradley_cosq_detach(int unit, int software_state_only)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_bradley_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - Draco unit number.
 *      numq - number of COS queues (1-8).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_bradley_cosq_config_set(int unit, int numq)
{
    int         cos, prio, ratio, remain;
    uint32      val32;
#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    soc_port_t  port;
    uint64      val64;
#endif

    /* Validate cosq num */
    if (numq < 1) {
        return BCM_E_PARAM;
    }

    if (SOC_WARM_BOOT(unit)) {
        _num_cosq[unit] = numq;
        return BCM_E_NONE;
    }

    /* Map the eight 802.1 priority levels to the active cosqs */
    if (numq > 8) {
        numq = 8;
    }
    ratio = 8 / numq;
    remain = 8 % numq;
    cos = 0;
    for (prio = 0; prio < 8; prio++) {
        BCM_IF_ERROR_RETURN
            (bcm_bradley_cosq_mapping_set(unit, -1, prio, cos));
        if ((prio + 1) == (((cos + 1) * ratio) +
                           ((remain < (numq - cos)) ? 0 :
                            (remain - (numq - cos) + 1)))) {
            cos++;
        }
    }

    /* Map remaining internal priority levels to highest priority cosq */
    cos = numq - 1;
    for (prio = 8; prio < 14; prio++) {
        BCM_IF_ERROR_RETURN
            (bcm_bradley_cosq_mapping_set(unit, -1, prio, cos));
    }

    /* Map SC COS queue */
    BCM_IF_ERROR_RETURN
        (bcm_bradley_cosq_mapping_set(unit, -1, 14, HB_SC_COS));

    /* Map QM COS queue */
    BCM_IF_ERROR_RETURN
        (bcm_bradley_cosq_mapping_set(unit, -1, 15, HB_QM_COS));

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    /* identity mapping for higig ports */

    if (SOC_IS_HB_GW(unit)) {
        /* map prio0->cos0, prio1->cos1, ... , prio7->cos7 */
        COMPILER_64_ZERO(val64);
        for (prio = 0; prio < 8; prio++) {
            soc_reg64_field32_set(unit, ICOS_SELr, &val64, 
                                  cos_field[prio], hg_cos_val[prio]);
        }
        PBMP_HG_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_ICOS_SELr(unit, port, val64));
        }
        SOC_IF_ERROR_RETURN(WRITE_ICOS_SELr(unit, (CMIC_PORT(unit)), val64));
    }
#endif

    if (SOC_REG_IS_VALID(unit, CPU_PRIORITY_SELr)) {
        /* use 1:1 CPU priority mapping */
        val32 = 0;
        for (prio = 0; prio < 8; prio++) {
            soc_reg_field_set(unit, CPU_PRIORITY_SELr, &val32, 
                              cpu_prio_field[prio], prio);
        }
        SOC_IF_ERROR_RETURN(WRITE_CPU_PRIORITY_SELr(unit, val32));
        val32 = 0;
        for (prio = 8; prio < 16; prio++) {
            soc_reg_field_set(unit, CPU_PRIORITY_SEL_2r, &val32, 
                              cpu_prio_field[prio], prio);
        }
        SOC_IF_ERROR_RETURN(WRITE_CPU_PRIORITY_SEL_2r(unit, val32));
    }

    _num_cosq[unit] = numq;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_bradley_cosq_mapping_set
 * Purpose:
 *      Set which cosq a given priority should fall into
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Priorities 14 and 15 are blocked unless used in
 *      conjunction with the SC and QM COS queues.
 */

int
bcm_bradley_cosq_mapping_set(int unit, bcm_port_t port,
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
    case 8:	f = COS8f; break;
    case 9:	f = COS9f; break;
    case 10:	f = COS10f; break;
    case 11:	f = COS11f; break;
    case 12:	f = COS12f; break;
    case 13:	f = COS13f; break;
    case 14:	f = COS14f; break;
    case 15:	f = COS15f; break;
    default:	return BCM_E_PARAM;
    }

    if ((cosq < 0 || cosq >= 8) && cosq != HB_SC_COS && cosq != HB_QM_COS) {
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
	if (priority < 8) {
	    SOC_IF_ERROR_RETURN(READ_COS_SELr(unit, port, &cval32));
	    oval32 = cval32;
	    soc_reg_field_set(unit, COS_SELr, &cval32, f, cosq);
	    if (cval32 != oval32) {
		SOC_IF_ERROR_RETURN(WRITE_COS_SELr(unit, port, cval32));
	    }
	    if (port == CMIC_PORT(unit) &&
                SOC_REG_IS_VALID(unit, CPU_COS_SELr)) {
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
	} else {
	    SOC_IF_ERROR_RETURN(READ_COS_SEL_2r(unit, port, &cval32));
	    oval32 = cval32;
	    soc_reg_field_set(unit, COS_SEL_2r, &cval32, f, cosq);
	    if (cval32 != oval32) {
		SOC_IF_ERROR_RETURN(WRITE_COS_SEL_2r(unit, port, cval32));
	    }

	    if (port == CMIC_PORT(unit) &&
                SOC_REG_IS_VALID(unit, CPU_COS_SEL_2r)) {
		SOC_IF_ERROR_RETURN(READ_CPU_COS_SEL_2r(unit, &cval32));
		oval32 = cval32;
		soc_reg_field_set(unit, CPU_COS_SEL_2r, &cval32, f, cosq);
		if (cval32 != oval32) {
		    SOC_IF_ERROR_RETURN(WRITE_CPU_COS_SEL_2r(unit, cval32));
		}
	    }
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
	    if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
		SOC_IF_ERROR_RETURN(READ_ICOS_SEL_2r(unit, port, &cval32));
		oval32 = cval32;
		soc_reg_field_set(unit, ICOS_SEL_2r, &cval32, f, cosq);
		if (cval32 != oval32) {
		    SOC_IF_ERROR_RETURN(WRITE_ICOS_SEL_2r(unit, port, cval32));
		}
	    }
#endif
	}
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_bradley_cosq_mapping_get
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
bcm_bradley_cosq_mapping_get(int unit, bcm_port_t port,
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
    case 8:	f = COS8f; break;
    case 9:	f = COS9f; break;
    case 10:	f = COS10f; break;
    case 11:	f = COS11f; break;
    case 12:	f = COS12f; break;
    case 13:	f = COS13f; break;
    case 14:	f = COS14f; break;
    case 15:	f = COS15f; break;
    default:	return BCM_E_PARAM;
    }

    if (port == -1) {
	port = REG_PORT_ANY;
    } else if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
	return BCM_E_PORT;
    }

    if (priority < 8) {
	SOC_IF_ERROR_RETURN(READ_COS_SELr(unit, port, &cval32));
	*cosq = soc_reg_field_get(unit, COS_SELr, cval32, f);
    } else {
	SOC_IF_ERROR_RETURN(READ_COS_SEL_2r(unit, port, &cval32));
	*cosq = soc_reg_field_get(unit, COS_SEL_2r, cval32, f);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_bradley_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_bradley_cosq_config_get(int unit, int *numq)
{
    if (_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _num_cosq[unit];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_bradley_cosq_port_sched_set
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
bcm_bradley_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                int mode, const int weights[], int delay)
{
    int port, t, i;
    uint32 wrr, escfg, minsp;
    int mbits = 0;
    int enc_weights[8];

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
         * All weight values must fit within 7 bits.
         * If weight is 0, this queue is run in strict mode,
         * others run in WRR mode.
         */

        t = weights[0] | weights[1] | weights[2] | weights[3] |
            weights[4] | weights[5] | weights[6] | weights[7];

        if ((t & ~0x7f) != 0) {
            return BCM_E_PARAM;
        }
        for(i = 0; i < 8; i++) {
            enc_weights[i] = weights[i];
        }
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        mbits = 3;
        for(i = 0; i < 8; i++) {
            enc_weights[i] = _bcm_hb_cos_drr_kbytes_to_weight(weights[i]);
        }
        break;
    case BCM_COSQ_BOUNDED_DELAY:        /* not supported in xgs */
    default:
        return BCM_E_PARAM;
    }

    PBMP_ITER(pbm, port) {
        SOC_IF_ERROR_RETURN
            (READ_ESCONFIGr(unit, port, &escfg));
        soc_reg_field_set(unit, ESCONFIGr, &escfg, SCHEDULING_SELECTf, mbits);
        SOC_IF_ERROR_RETURN
            (WRITE_ESCONFIGr(unit, port, escfg));
    }

    if ((mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ||
        (mode == BCM_COSQ_DEFICIT_ROUND_ROBIN)) {
        /*
         * Weighted Fair Queueing scheduling among vaild COSs
         */
        PBMP_ITER(pbm, port) {
            SOC_IF_ERROR_RETURN
                (READ_MINSPCONFIGr(unit, port, &minsp));
            for(i = 0; i < 8; i++) {
                SOC_IF_ERROR_RETURN
                    (READ_COSWEIGHTSr(unit, port, i, &wrr));
    /*    coverity[uninit_use_in_call : FALSE]    */
                soc_reg_field_set(unit, COSWEIGHTSr, &wrr,
                                  COSWEIGHTSf, enc_weights[i]);
                SOC_IF_ERROR_RETURN
                    (WRITE_COSWEIGHTSr(unit, port, i, wrr));
                /* Set strict priority bit if weight is zero */
                if (enc_weights[i] == 0) {
                    minsp |= (1 << i);
                } else {
                    minsp &= ~(1 << i);
                }
            }
            SOC_IF_ERROR_RETURN
                (WRITE_MINSPCONFIGr(unit, port, minsp));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_bradley_cosq_port_sched_get
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
bcm_bradley_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                int *mode, int weights[], int *delay)
{
    uint32 escfg, wrr;
    int mbits, port, i;

    mbits = -1;
    PBMP_ITER(pbm, port) {
        SOC_IF_ERROR_RETURN
            (READ_ESCONFIGr(unit, port, &escfg));
        mbits = soc_reg_field_get(unit, ESCONFIGr, escfg, SCHEDULING_SELECTf);
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
        PBMP_ITER(pbm, port) {
            for(i = 0; i < 8; i++) {
                SOC_IF_ERROR_RETURN
                    (READ_COSWEIGHTSr(unit, port, i, &wrr));
                weights[i] = soc_reg_field_get(unit, COSWEIGHTSr, wrr,
                                               COSWEIGHTSf);
            }
            break;
        }

        if (mbits == 3) {
            int i;
            for(i = 0; i < 8; i++) {
                weights[i] = _bcm_hb_cos_drr_weight_to_kbytes(weights[i]);
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
 *      bcm_bradley_cosq_sched_weight_max_get
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
bcm_bradley_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        *weight_max = BCM_COSQ_WEIGHT_MIN;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        *weight_max = HB_WRR_WEIGHT_MAX;
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        *weight_max = _bcm_hb_cos_drr_weight_to_kbytes(HB_DRR_WEIGHT_MAX);
        break;
    default:
        *weight_max = BCM_COSQ_WEIGHT_UNLIMITED;
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_bradley_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_bradley_cosq_sw_dump(int unit)
{
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "    Number: %d\n"), _num_cosq[unit]));

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#else /* BCM_BRADLEY_SUPPORT */
typedef int _bradley_cosq_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_BRADLEY_SUPPORT */

