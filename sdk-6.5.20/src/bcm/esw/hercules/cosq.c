/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cosq.c
 */

#include <soc/defs.h>
#if defined(BCM_HERCULES_SUPPORT)
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/debug.h>
#include <soc/hercules.h>

#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/module.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/hercules.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

static int _num_cosq[SOC_MAX_NUM_DEVICES];

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_hercules_cosq_reinit
 * Purpose:
 *      Recover COSQ software state.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_hercules_cosq_reinit(int unit)
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
        int cos, lossless;

        /* Pick from config */
        lossless = soc_property_get(unit, spn_LOSSLESS_MODE, 0);
        cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

        if (cos < 1) {
       	    cos = 1;
        } else if (lossless && cos > 4) {
      	    cos = 4;
        } else if (cos > NUM_COS(unit)) {
       	    cos = NUM_COS(unit);
        }
        _num_cosq[unit] = cos;
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

int
bcm_hercules_cosq_init(int unit)
{
    int		lossless, num_cos;
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
        return _bcm_hercules_cosq_reinit(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    lossless = soc_property_get(unit, spn_LOSSLESS_MODE, 0);

    num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (num_cos < 1) {
	num_cos = 1;
    } else if (lossless && num_cos > 4) {
	num_cos = 4;
    } else if (num_cos > NUM_COS(unit)) {
	num_cos = NUM_COS(unit);
    }

    return bcm_hercules_cosq_config_set(unit, num_cos);
}

int
bcm_hercules_cosq_config_set(int unit, int numq)
{
    int		lossless, num_ports;
    int		cos, prio, ratio, remain;
    soc_port_t	port;

    /* Validate cosq num */
    if (numq < 1) {
        return BCM_E_PARAM;
    }

    lossless = soc_property_get(unit, spn_LOSSLESS_MODE, 0);

    SOC_PBMP_COUNT(PBMP_HG_ALL(unit), num_ports);

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_hercules15_mmu_limits_config(unit, port, num_ports, 
                                              numq, lossless));
    }

    /* Map the eight internal priority levels to the active cosqs */
    ratio = 8 / numq;
    remain = 8 % numq;
    cos = 0;
    for (prio = 0; prio < 8; prio++) {
        BCM_IF_ERROR_RETURN
            (bcm_hercules_cosq_mapping_set(unit, -1, prio, cos));
        if ((prio + 1) == (((cos + 1) * ratio) +
                           ((remain < (numq - cos)) ? 0 :
                            (remain - (numq - cos) + 1)))) {
            cos++;
        }
    }

    _num_cosq[unit] = numq;

    return BCM_E_NONE;
}

int
bcm_hercules_cosq_config_get(int unit, int *numq)
{
    if (_num_cosq[unit] == 0) {
	return BCM_E_INIT;
    }

    if (numq != NULL) {
	*numq = _num_cosq[unit];
    }

    return BCM_E_NONE;
}

int
bcm_hercules_cosq_mapping_set(int unit, bcm_port_t port,
			      bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    soc_field_t	f;
    uint32	reg, oreg;

    if (cosq < 0 || cosq >= NUM_COS(unit)) {
	return BCM_E_PARAM;
    }
    switch (priority) {
    case 0:	f = FIELD0f; break;
    case 1:	f = FIELD1f; break;
    case 2:	f = FIELD2f; break;
    case 3:	f = FIELD3f; break;
    case 4:	f = FIELD4f; break;
    case 5:	f = FIELD5f; break;
    case 6:	f = FIELD6f; break;
    case 7:	f = FIELD7f; break;
    default:	return BCM_E_PARAM;
    }

    if (port == -1) {
	PBMP_HG_ITER(unit, port) {
	    SOC_IF_ERROR_RETURN(READ_ING_COS_MAPr(unit, port, &reg));
	    oreg = reg;
	    soc_reg_field_set(unit, ING_COS_MAPr, &reg, f, cosq);
	    if (reg != oreg) {
		SOC_IF_ERROR_RETURN(WRITE_ING_COS_MAPr(unit, port, reg));
	    }
	}
    } else if (SOC_PORT_VALID(unit, port) && IS_HG_PORT(unit, port)) {
	SOC_IF_ERROR_RETURN(READ_ING_COS_MAPr(unit, port, &reg));
	oreg = reg;
	soc_reg_field_set(unit, ING_COS_MAPr, &reg, f, cosq);
	if (reg != oreg) {
	    SOC_IF_ERROR_RETURN(WRITE_ING_COS_MAPr(unit, port, reg));
	}
    } else {
	return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

int
bcm_hercules_cosq_mapping_get(int unit, bcm_port_t port,
			      bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    soc_field_t	f;
    uint32	reg;

    switch (priority) {
    case 0:	f = FIELD0f; break;
    case 1:	f = FIELD1f; break;
    case 2:	f = FIELD2f; break;
    case 3:	f = FIELD3f; break;
    case 4:	f = FIELD4f; break;
    case 5:	f = FIELD5f; break;
    case 6:	f = FIELD6f; break;
    case 7:	f = FIELD7f; break;
    default:	return BCM_E_PARAM;
    }

    if (port == -1) {
	reg = 0;
	PBMP_HG_ITER(unit, port) {
	    SOC_IF_ERROR_RETURN(READ_ING_COS_MAPr(unit, port, &reg));
	    break;
	}
	*cosq = soc_reg_field_get(unit, ING_COS_MAPr, reg, f);
    } else if (SOC_PORT_VALID(unit, port) && IS_HG_PORT(unit, port)) {
	SOC_IF_ERROR_RETURN(READ_ING_COS_MAPr(unit, port, &reg));
	*cosq = soc_reg_field_get(unit, ING_COS_MAPr, reg, f);
    } else {
	return BCM_E_PORT;
    }
    return BCM_E_NONE;
}

int
bcm_hercules_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
				 int mode, const int weights[], int delay)
{
    int			port;
    uint32		primod;
    int			mbits, cos;

    COMPILER_REFERENCE(delay);

    switch (mode) {
    case BCM_COSQ_STRICT:
	mbits = 0;
	break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
	mbits = 1;
	/*
	 * Weight values for 5670/75 are 32-bits.
	 * If weight is 0, this queue is run in strict mode,
	 * others run in WRR mode.
	 */
	break;
    default:
        mbits = -1;
	return BCM_E_PARAM;
    }

    PBMP_ITER(pbm, port) {
        SOC_IF_ERROR_RETURN(READ_MMU_EGS_PRIMODr(unit, port, &primod));
	soc_reg_field_set(unit, MMU_EGS_PRIMODr, &primod, PRIf, mbits);
        SOC_IF_ERROR_RETURN(WRITE_MMU_EGS_PRIMODr(unit, port, primod));
    }

    if (mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) {
	/*
	 * Weighted Fair Queueing scheduling
	 */
	PBMP_ITER(pbm, port) {
            for (cos = 0; cos < 8; cos++) {
                SOC_IF_ERROR_RETURN(WRITE_MMU_EGS_WGTCOSr(unit, port, cos,
                                                          weights[cos]));
            }
	}
    }

    return BCM_E_NONE;
}

int
bcm_hercules_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
				 int *mode, int weights[], int *delay)
{
    uint32		primod, rval;
    int			port, mbits = -1, cos;

    PBMP_ITER(pbm, port) {
        SOC_IF_ERROR_RETURN(READ_MMU_EGS_PRIMODr(unit, port, &primod));
	mbits = soc_reg_field_get(unit, MMU_EGS_PRIMODr, primod, PRIf);
        break;
    }

    switch (mbits) {
    case 0:
        *mode = BCM_COSQ_STRICT;
	break;
    case 1:
	*mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        for (cos = 0; cos < 8; cos++) {
            SOC_IF_ERROR_RETURN(READ_MMU_EGS_WGTCOSr(unit, port, cos, &rval));
            weights[cos] =
                soc_reg_field_get(unit, MMU_EGS_WGTCOSr, rval, WGTf);
        }
	break;
    default:
	return BCM_E_INTERNAL;
    }

    if (delay) {
	*delay = 0;
    }
    return BCM_E_NONE;
}

int
bcm_hercules_cosq_sched_weight_max_get(int unit,int mode,
                                            int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
	*weight_max = BCM_COSQ_WEIGHT_STRICT;
	break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
	*weight_max = BCM_COSQ_WEIGHT_UNLIMITED;
        break;
    default:
	*weight_max = BCM_COSQ_WEIGHT_UNLIMITED;
	return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
bcm_hercules_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                     bcm_cos_queue_t cosq,
                                     uint32 kbits_sec_min,
                                     uint32 kbits_sec_max,
                                     uint32 kbits_sec_burst,
                                     uint32 flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                     bcm_cos_queue_t cosq,
                                     uint32 *kbits_sec_min,
                                     uint32 *kbits_sec_max,
                                     uint32 *kbits_sec_burst,
                                     uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_cosq_discard_set(int unit, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_cosq_discard_get(int unit, uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_cosq_discard_port_set(int unit, bcm_port_t port,
                                   bcm_cos_queue_t cosq,
                                   uint32 color,
                                   int drop_start,
                                   int drop_slope,
                                   int average_time)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_cosq_discard_port_get(int unit, bcm_port_t port,
                                   bcm_cos_queue_t cosq,
                                   uint32 color,
                                   int *drop_start,
                                   int *drop_slope,
                                   int *average_time)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_cosq_detach(int unit, int software_state_only)
{
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
bcm_hercules_cosq_sync(int unit)
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
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
void
bcm_hercules_cosq_sw_dump(int unit)
{
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "    Number: %d\n"), _num_cosq[unit]));

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#else /* BCM_HERCULES_SUPPORT */
typedef int _hercules_cosq_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_HERCULES_SUPPORT */
