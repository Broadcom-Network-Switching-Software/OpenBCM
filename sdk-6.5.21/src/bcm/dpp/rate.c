/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tx.c
 * Purpose: Implementation of bcm_petra_tx* API for dune devices
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_POLICER
#include <shared/bsl.h>
#include <bcm/rate.h>


#include <bcm_int/control.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>

#include <soc/dpp/PPD/ppd_api_metering.h>
#include <soc/dpp/JER/JER_PP/jer_pp_metering.h>

#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/gport_mgmt.h>

#include <bcm/debug.h>
#include <bcm_int/common/debug.h>

/***************************************************************/

/*
 * Local defines
 */



#define DPP_RATE_MSG(string)   _ERR_MSG_MODULE_NAME, unit, "%s[%d]: " string, __FILE__, __LINE__

#define DPP_RATE_UNIT_CHECK(unit) \
do { \
    if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("%s: Invalid unit"), FUNCTION_NAME())); \
    } \
} while (0)


#define DPP_RATE_INIT_CHECK(unit) \
do { \
    DPP_RATE_UNIT_CHECK(unit); \
    if (!_dpp_rate_state[unit].lock) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("%s: rates unitialized on unit:%d"), FUNCTION_NAME(), unit)); \
    } \
} while (0)

#define DPP_RATE_UNIT_LOCK(unit) \
do { \
    if (sal_mutex_take(_dpp_rate_state[unit].lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    } \
    _lock_taken = 1;  \
} while (0)


#define DPP_RATE_UNIT_UNLOCK(unit) \
do { \
    if(1 == _lock_taken) { \
        _lock_taken = 0;  \
        if (sal_mutex_give(_dpp_rate_state[unit].lock)) { \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_give failed for unit %d."), FUNCTION_NAME(), unit)); \
        } \
    } \
} while (0)

#define DPP_RATE_LEGAL_FLAGS (  BCM_RATE_MCAST | BCM_RATE_UNKNOWN_MCAST | BCM_RATE_BCAST | \
                                BCM_RATE_UCAST | BCM_RATE_DLF | BCM_RATE_MODE_PACKETS )



/* convert defines*/
                   /* kbits to bytes, for burst BCM TO DPP*/
#define _DPP_RATE_KBITS_TO_BYTES(__bcm_rate) ((__bcm_rate == 0) ? 64 :((__bcm_rate)*125)) 
/* kbits to bytes, for burst DPP TO BCM */
#define _DPP_RATE_BYTES_TO_KBITS(__bcm_rate) ((__bcm_rate)/125) 

#define _DPP_RATE_KBITS_TO_BYTES_FACTOR   125
#define _DPP_RATE_PACKETS_TO_BYTES_FACTOR 64

/* 
* sw state for rate
*/  

typedef struct _dpp_rate_state_s {
    sal_mutex_t lock;
} _dpp_rate_state_t;

static _dpp_rate_state_t _dpp_rate_state[BCM_MAX_NUM_UNITS];
SOC_PPC_MTR_BW_PROFILE_INFO diag_pp_eth_policer_config;

STATIC uint32 _bcm_petra_rate_translate_kbits_to_packets(uint32 kbits)
{
	uint32 packets= kbits*_DPP_RATE_KBITS_TO_BYTES_FACTOR;
	packets /= _DPP_RATE_PACKETS_TO_BYTES_FACTOR;
	return packets;
}

STATIC uint32 _bcm_petra_rate_translate_packets_to_kbits(uint32 packets)
{
	uint32 kbits = packets*_DPP_RATE_PACKETS_TO_BYTES_FACTOR;
	kbits /= _DPP_RATE_KBITS_TO_BYTES_FACTOR;
	return kbits;
}

STATIC uint32 _bcm_petra_rate_translate_packets_to_bytes(uint32 packets)
{
	uint32 bytes = packets*_DPP_RATE_PACKETS_TO_BYTES_FACTOR;
	return bytes;
}

STATIC uint32 _bcm_petra_rate_translate_bytes_to_packets(uint32 bytes)
{
	uint32 packets = bytes/_DPP_RATE_PACKETS_TO_BYTES_FACTOR;
	return packets;
}

int 
bcm_petra_rate_init(int unit)
{
    _dpp_rate_state_t *temp_state = &_dpp_rate_state[unit];

    BCMDNX_INIT_FUNC_DEFS;
    DPP_RATE_UNIT_CHECK(unit);

    sal_memset(temp_state, 0, sizeof(_dpp_rate_state_t));
    temp_state->lock = sal_mutex_create("_dpp_meter_unit_lock");
    if (!temp_state->lock) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Failed to create mutex\r\n")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_rate_bandwidth_set
 * Description:
 *      Set rate bandwidth limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Bitmask with one of the following:
 *              BCM_RATE_BCAST
 *              BCM_RATE_MCAST
 *              BCM_RATE_UCAST
 *              BCM_RATE_DLF
 *              BCM_RATE_UNKNOWN_MCAST
 *      kbits_sec - Rate in kilobits (1000 bits) per second.
 *                  Rate of 0 disables rate limiting.
 *      kbits_burst - Maximum burst size in kilobits(1000 bits)
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_UNAVAIL - Not supported
 *      BCM_E_XXX - Error.
 */

int 
bcm_petra_rate_bandwidth_set(int unit, bcm_port_t port, int flags, 
                           uint32 kbits_sec, uint32 kbits_burst)
{
    SOC_PPC_MTR_BW_PROFILE_INFO policer;
    int port_i;
    SOC_PPC_MTR_ETH_TYPE  types[5];
    int nof_types = 0;
    int type_indx = 0;
	int core_id;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;
	soc_dpp_config_meter_t *dpp_meter; 

	uint32 pp_port;
    uint32 legal_flags = DPP_RATE_LEGAL_FLAGS;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_RATE_INIT_CHECK(unit);

    SOC_PPC_MTR_BW_PROFILE_INFO_clear(&policer);

	/* check flags */
    if (SOC_IS_ARADPLUS(unit)) {
        legal_flags |= BCM_RATE_COLOR_BLIND;
    }
	if (SOC_IS_JERICHO(unit)) {
		legal_flags |= BCM_RATE_PKT_ADJ_HEADER_TRUNCATE;
	}
	if ((flags & ~legal_flags) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_RATE_MSG("unsupported flags specified\n")));
    }

    sal_memset(types, 0, sizeof(types));

    policer.is_packet_mode = flags & BCM_RATE_MODE_PACKETS ? TRUE : FALSE;
    if (SOC_IS_ARADPLUS(unit)) {
        policer.color_mode = flags & BCM_RATE_COLOR_BLIND ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
    }
    policer.is_pkt_truncate = flags & BCM_RATE_PKT_ADJ_HEADER_TRUNCATE ? TRUE : FALSE;

	if (kbits_burst != 0) {
        dpp_meter = &(SOC_DPP_CONFIG(unit))->meter;

        /* set policer rate, CIR and CBS, relevant only */
        if (policer.is_packet_mode == TRUE) {
            policer.cir = _bcm_petra_rate_translate_packets_to_kbits(kbits_sec);
            policer.cbs = _bcm_petra_rate_translate_packets_to_bytes(kbits_burst);
        }else{
            policer.cir = kbits_sec;
            policer.cbs = _DPP_RATE_KBITS_TO_BYTES(kbits_burst);
        }
        /* verify cir and cbs is in valid range */
        if(kbits_sec != 0)
        {
            if (policer.cir < dpp_meter->policer_min_rate) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_RATE_MSG("rate is lower than the device min-rate: %d kbits/sec, %d packets/sec\n"), dpp_meter->policer_min_rate,_bcm_petra_rate_translate_kbits_to_packets(dpp_meter->policer_min_rate)));
            }
            if (policer.cir > dpp_meter->max_rate) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_RATE_MSG("rate (%d kbits/sec) is bigger than the device max-rate: %d kbits/sec, %d packets/sec\n"), policer.cir , dpp_meter->max_rate,_bcm_petra_rate_translate_kbits_to_packets(dpp_meter->max_rate)));
            }            
        }
        if (policer.cbs < dpp_meter->min_burst)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_RATE_MSG("burst (%d bytes) is lower than the device min_burst: %d bytes\n"), policer.cbs, dpp_meter->min_burst));
        }
        if (policer.cbs > dpp_meter->max_burst)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_RATE_MSG("burst (%d bytes) is bigger than the device max_burst: %d bytes\n"), policer.cbs, dpp_meter->max_burst));
        }
    }else{
		if (SOC_IS_JERICHO(unit)) {
			policer.cir = 0;
			policer.cbs = 0;
			policer.disable_cir = 1; /* to disable the validity bit under IDR table*/
			policer.color_mode = 0;
			policer.is_packet_mode = 0;
			policer.is_pkt_truncate = 0;
		}else{
			/* set policer rate,to max */
			policer.cir = SOC_DPP_CONFIG(unit)->meter.max_rate;
			policer.cbs = SOC_DPP_CONFIG(unit)->meter.max_burst;
			policer.disable_cir = 1; /* no rate*/
			policer.color_mode = SOC_PPC_MTR_COLOR_MODE_AWARE;
			policer.is_packet_mode = FALSE;
		}
    }

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    if(flags & BCM_RATE_DLF) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC;
    }
    if(flags & BCM_RATE_UNKNOWN_MCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC;
    }
    if(flags & BCM_RATE_MCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_KNOW_MC;
    }
    if(flags & BCM_RATE_BCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_BC;
    }
    if(flags & BCM_RATE_UCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_KNOW_UC;
    }
    DPP_RATE_UNIT_LOCK(unit);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        for(type_indx = 0; type_indx < nof_types; ++type_indx) {
			if (SOC_IS_JERICHO(unit)) {
				BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &pp_port, &core_id)));
            	BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mtr_eth_policer_params_set, (unit, core_id, pp_port, types[type_indx], &policer)));
			} else{
				soc_sand_rv = soc_ppd_mtr_eth_policer_params_set(unit, port_i, types[type_indx], &policer);
				SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
			}
        }
    }

 exit:        
    DPP_RATE_UNIT_UNLOCK(unit);      
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_petra_rate_bandwidth_get
 * Description:
 *      Get rate bandwidth limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Bitmask with one of the following:
 *              BCM_RATE_BCAST
 *              BCM_RATE_MCAST
 *              BCM_RATE_UCAST
 *              BCM_RATE_DLF
 *              BCM_RATE_UC
 *      kbits_sec - Rate in kilobits (1000 bits) per second.
 *                  Rate of 0 disabled rate limiting.
 *      kbits_burst - Maximum burst size in kilobits(1000 bits)
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_UNAVAIL - Not supported
 *      BCM_E_XXX - Error.
 * Remarks: 
 *      * There is no way to get the flags (e.g. packet mode or color blind).
 */

int 
bcm_petra_rate_bandwidth_get(int unit, bcm_port_t port, int flags, 
                           uint32 *kbits_sec, uint32 *kbits_burst)
{
    SOC_PPC_MTR_BW_PROFILE_INFO policer;
    int port_i,core_id;
	uint32 pp_port;

    SOC_PPC_MTR_ETH_TYPE  types[5];
    int nof_types = 0;
    int type_indx = 0;
    int soc_sand_rv = 0;
    int rv = BCM_E_NONE;

    _bcm_dpp_gport_info_t gport_info;
	uint32 legal_flags = DPP_RATE_LEGAL_FLAGS;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_RATE_INIT_CHECK(unit);

    SOC_PPC_MTR_BW_PROFILE_INFO_clear(&policer);

    /* check flags */
    if (SOC_IS_ARADPLUS(unit)) {
        legal_flags |= BCM_RATE_COLOR_BLIND;
    }

    if ((flags & ~legal_flags) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_RATE_MSG("unsupported flags specified\n")));
    }

    sal_memset(types, 0, sizeof(types));

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    if(flags & BCM_RATE_DLF) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC;
    }
    else if(flags & BCM_RATE_UNKNOWN_MCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC;
    }
    else if(flags & BCM_RATE_MCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_KNOW_MC;
    }
    else if(flags & BCM_RATE_BCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_BC;
    }
    else if(flags & BCM_RATE_UCAST) {
        types[nof_types++] = SOC_PPC_MTR_ETH_TYPE_KNOW_UC;
    }

    DPP_RATE_UNIT_LOCK(unit);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        /* coverity[unreachable:FALSE] */
        for(type_indx = 0; type_indx < nof_types; type_indx++) {
			if (SOC_IS_JERICHO(unit)) {
				BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &pp_port, &core_id)));
            	BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mtr_eth_policer_params_get, (unit, core_id, pp_port, types[type_indx], &policer)));
			} else{
				soc_sand_rv = soc_ppd_mtr_eth_policer_params_get(unit, port_i, types[type_indx], &policer);
				SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
			}
            break; /* get first type */
        }
        break; /* get first port */
    }

    /* get policer rate */
    if(policer.disable_cir == 0) {
		if (policer.is_packet_mode == TRUE) {
			*kbits_sec   = _bcm_petra_rate_translate_kbits_to_packets(policer.cir);
			*kbits_burst = _bcm_petra_rate_translate_bytes_to_packets(policer.cbs);
		}else{
			*kbits_sec = policer.cir;
			*kbits_burst = _DPP_RATE_BYTES_TO_KBITS(policer.cbs);
		}
    }
	else{
        *kbits_sec = 0;
        *kbits_burst = 0;
    }

	diag_pp_eth_policer_config.cbs = *kbits_burst;
	diag_pp_eth_policer_config.cir = *kbits_sec;
	diag_pp_eth_policer_config.disable_cir = policer.disable_cir;
	diag_pp_eth_policer_config.color_mode  = policer.color_mode;
	diag_pp_eth_policer_config.is_pkt_truncate = policer.is_pkt_truncate;
	diag_pp_eth_policer_config.is_packet_mode = policer.is_packet_mode;

 exit:        
    DPP_RATE_UNIT_UNLOCK(unit);      
    BCMDNX_FUNC_RETURN; 
}


