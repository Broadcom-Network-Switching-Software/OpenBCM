/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Tomahawk Instrumentation API
 */

#include <soc/drv.h>

#ifdef BCM_INSTRUMENTATION_SUPPORT

#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <sal/types.h>
 #include <sal/core/sync.h>
 #include <sal/core/time.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/switch.h>
#include <bcm/tx.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/tx.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/instrumentation.h> 

/* Flag to check initialized status */
STATIC int _pkt_trace_initialized[BCM_MAX_NUM_UNITS];

/* Protection mutexes for each unit */
sal_mutex_t _pkt_trace_mlock[BCM_MAX_NUM_UNITS];

#define PKT_TRACE_LOCK(unit)    sal_mutex_take(_pkt_trace_mlock[unit], sal_mutex_FOREVER)
#define PKT_TRACE_UNLOCK(unit)  sal_mutex_give(_pkt_trace_mlock[unit])

#define UNIT_VALID_CHECK(unit) \
    if (((unit) < 0) || ((unit) >= BCM_MAX_NUM_UNITS)) { return BCM_E_UNIT; }

#define PKT_TRACE_INIT_DONE(unit)    (_pkt_trace_mlock[unit] != NULL)

#define PKT_TRACE_INIT_CHECK(unit) \
    do { \
        UNIT_VALID_CHECK(unit); \
        if (_pkt_trace_mlock[unit] == NULL) { return BCM_E_INIT; } \
    } while (0)

/*
 * Function:
 *      bcm_esw_pkt_trace_hw_reset
 * Purpose:
 *      clean PTR_RESULTS_BUFFER_IVP/ISW1/ISW2 registers
 *      call this functio before sending a 
 *      visibilty packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_esw_pkt_trace_hw_reset(int unit)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Call low level routine to reset HW memories for all pipes */
        return _bcm_th3_pkt_trace_hw_reset(unit, -1);
    } else
#endif
    if (SOC_IS_TOMAHAWKX(unit)) {
        return _bcm_th_pkt_trace_hw_reset(unit);
    }     
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      _bcm_esw_pkt_trace_src_port_set 
 * Purpose:
 *      set the destination port of 
 *      visiblity packet
 *      this must be called before visibilty packet 
 *      is sent to set the pipe to read the resuls 
 * Parameters:
 *      IN :  unit
 *      IN : logical_dst_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_esw_pkt_trace_src_port_set(int unit, uint32 logical_dst_port) 
{
    if (SOC_IS_TOMAHAWKX(unit)) {
        return _bcm_th_pkt_trace_src_port_set(unit, logical_dst_port);
    } else if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
        return _bcm_td3_pkt_trace_src_port_set(unit, logical_dst_port);
    }
    return BCM_E_UNAVAIL; 
 }

/*
 * Function:
 *      _bcm_esw_pkt_trace_src_port_get 
 * Purpose:
 *      get the destination port of 
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_dst_port
 * Returns:
 *      BCM_E_XXXd
 */

int _bcm_esw_pkt_trace_src_port_get(int unit) 
{
    if (SOC_IS_TOMAHAWKX(unit)) {
        return _bcm_th_pkt_trace_src_port_get(unit);
    } else if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
        return _bcm_td3_pkt_trace_src_port_get(unit);
    }
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      _bcm_esw_pkt_trace_src_pipe_get 
 * Purpose:
 *      get the destination pipe of 
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_dst_port
 * Returns:
 *      BCM_E_XXXd
 */
int _bcm_esw_pkt_trace_src_pipe_get(int unit) 
{
    if (SOC_IS_TOMAHAWKX(unit)) {
        return _bcm_th_pkt_trace_src_pipe_get(unit);
    } else if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
        return _bcm_td3_pkt_trace_src_pipe_get(unit);
    }
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      _bcm_esw_pkt_trace_pkt_profile_set 
 * Purpose:
 *      given visibility packet behavior options
 *      setting the profile id for the next visibility packet
 * Parameters:
 *      IN :  unit
 *      IN :  options (BCM_PKT_TRACE_LEARN/NO_IFP/FORWARD)
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_esw_pkt_trace_cpu_profile_set(int unit,uint32 options)
{
    if (SOC_IS_TOMAHAWKX(unit)){
        return _bcm_th_pkt_trace_cpu_profile_set(unit, options);
    }  else if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
        return _bcm_td3_pkt_trace_cpu_profile_set(unit, options);
    }
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      _bcm_esw_pkt_trace_cpu_pkt_profile_get
 * Purpose:
 *      retrieve cpu profile id to be used for the
 *      next visibility packet
 * Parameters:
 *      IN :  unit
 *      INOUT : profile id 
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_esw_pkt_trace_cpu_profile_get(int unit, uint32* profile_id)
{
    if (SOC_IS_TOMAHAWKX(unit)) {
        return _bcm_th_pkt_trace_cpu_profile_get(unit, profile_id);
    } else if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
        return _bcm_td3_pkt_trace_cpu_profile_get(unit, profile_id);
    }
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      _bcm_esw_pkt_trace_info_get
 * Purpose:
 *      Read visibility packet process data from PTR_RESULTS_BUFFER_IVP, ISW1,
 *      and ISW2 and store into bcm_switch_pkt_trace_info_s *
 * Parameters:
 *      unit - (IN) Unit number.
 *      pkt_trace_info - (INOUT) visibility pkt trace prcoess result
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int _bcm_esw_pkt_trace_info_get(int unit, 
                               uint32 options, uint8 port, 
                               int len, uint8 *data, 
                  bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    bcm_pkt_t *pkt; 
    int rv = BCM_E_UNAVAIL;
    int values[6] = {0, 0, 0, 0, 0, 0};
    bcm_module_t mod_id = -1;
    bcm_port_t pp_port = -1;
    bcm_gport_t gport;
    bcm_trunk_t tgid;
    int id;
    UNIT_VALID_CHECK(unit);

    if (soc_feature(unit, soc_feature_visibility)) {
        soc_timeout_t   to; 
        uint8           num_trial = 0;

        if (pkt_trace_info == NULL || 
            data == NULL) {
            return BCM_E_PARAM;
        }
        memset(pkt_trace_info, 0x0, sizeof(bcm_switch_pkt_trace_info_t));
        PKT_TRACE_INIT_CHECK(unit);

        /* mutex lock*/
        PKT_TRACE_LOCK(unit);
        rv = bcm_pkt_alloc(unit, len, 0, &pkt);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }
        pkt->call_back = 0;
        pkt->blk_count = 1;
        pkt->unit = unit;
        
        rv = bcm_pkt_memcpy(pkt, 0, data, len);
        /*new flag indicating that the packet is for visibility or masquerade */
        pkt->flags2 = BCM_PKT_F2_RX_PORT;
        /* adding visibility option to packet flag */
        pkt->flags2 |= BCM_PKT_F2_VISIBILITY_PKT;
        /* setting pkt trace src port number and profile id on the packet */
        pkt->rx_port = port; 

        /* Option 'BCM_PKT_TRACE_RETRIEVE_DATA_ONLY' is only supported on
         * TH3. All other devices should ignore the option. Hence remove it.
         */
        options &= ~BCM_PKT_TRACE_RETRIEVE_DATA_ONLY;

        /* store profile info to be used during tx*/
        _bcm_esw_pkt_trace_cpu_profile_set(unit, options);
        /* Software workaround for TH */
        if (SOC_IS_TOMAHAWK(unit) &&
            soc_feature(unit, soc_feature_th_pkt_trace_sw_war)) {
            rv = _bcm_th_pkt_trace_int_lbport_set(unit, port, 1, values);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
        } else if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            rv = bcm_esw_port_gport_get(unit,
                                        port,
                                        &gport);

            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            rv = _bcm_esw_gport_resolve(unit, gport,
                                       &mod_id, &pp_port,
                                       &tgid, &id);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            rv = _bcm_td3_pkt_trace_source_trunk_map_pp_set(unit,
                           mod_id,
                           pp_port,
                           0);

            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            rv =_bcm_td3_pkt_trace_init(unit);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }

            rv = _bcm_td3_pkt_trace_dop_config_set(unit);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
        }

        /* transmit packet -- support only synchronous mode */
        rv = bcm_esw_tx(unit, pkt, NULL);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        /* Software workaround for TH, recover the original configuration */
        if (SOC_IS_TOMAHAWK(unit) &&
            soc_feature(unit, soc_feature_th_pkt_trace_sw_war)) {
            rv =_bcm_th_pkt_trace_int_lbport_set(unit, port, 0, values);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
        }
        soc_timeout_init(&to, SECOND_USEC, 0);

        if (SOC_IS_TOMAHAWKX(unit)) {
            for (;;num_trial++) {
                rv = _bcm_th_pkt_trace_info_get(unit, pkt_trace_info);
                if (rv == BCM_E_NONE) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    rv = BCM_E_TIMEOUT;
                    break;
                }
                sal_usleep(SECOND_USEC/4);
            }
        } else if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            for (;;num_trial++) {
                rv = _bcm_td3_pkt_trace_info_get(unit, pkt_trace_info);
                if (rv == BCM_E_NONE) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    rv = BCM_E_TIMEOUT;
                    break;
                }
                sal_usleep(SECOND_USEC/4);
            }
        }
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            if (BCM_SUCCESS(rv) && (pp_port > 0)) {
                rv = _bcm_td3_pkt_trace_source_trunk_map_pp_set(unit,
                       mod_id,
                       pp_port,
                       1);
                if (BCM_FAILURE(rv)) {
                    goto clean_up;
                }
            }
        }
    }
clean_up:
    if (soc_feature(unit, soc_feature_visibility)) {
        if (pkt != NULL) {
            bcm_pkt_free(unit, pkt);
        }
        /*mutex unlock */
        PKT_TRACE_UNLOCK(unit);
        return rv;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_pkt_trace_raw_data_get
 * Purpose:
 *      Read visibility packet trace
 *      debug information as raw data.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      options       - (IN) CPU packet profile options for loopback port.
 *      port          - (IN) Source Masquered port.
 *      len           - (IN) Visibility packet data buffer length.
 *      data          - (IN) Visibility packet data buffer.
 *      raw_data_size - (INOUT) Raw data length.
 *      raw_data      - (INOUT) Visibility pkt trace raw data.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int _bcm_esw_pkt_trace_raw_data_get(int unit,
                               uint32 options,
                               uint8 port,
                               int len,
                               uint8 *data,
                               uint32 raw_data_buf_size,
                               uint8 *raw_data,
                               uint32 *raw_data_size)
{

    bcm_pkt_t *pkt;
    int rv = BCM_E_UNAVAIL;
    uint8  num_trial = 0;
    bcm_module_t mod_id;
    bcm_port_t pp_port;
    bcm_gport_t gport;
    bcm_trunk_t tgid;
    int id;

    UNIT_VALID_CHECK(unit);

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (soc_feature(unit, soc_feature_visibility)) {
        soc_timeout_t   to;

        if ((raw_data == NULL) ||
            (data == NULL) ||
            (raw_data_size == NULL)) {
            return BCM_E_PARAM;
        }
        if (raw_data_buf_size == 0) {
            return BCM_E_PARAM;
        }
        memset(raw_data, 0x0, raw_data_buf_size);
        *raw_data_size = raw_data_buf_size;
        PKT_TRACE_INIT_CHECK(unit);

        /* mutex lock*/
        PKT_TRACE_LOCK(unit);
        BCM_IF_ERROR_RETURN(bcm_pkt_alloc(unit, len, 0, &pkt));
        pkt->call_back = 0;
        pkt->blk_count = 1;
        pkt->unit = unit;

        rv = bcm_pkt_memcpy(pkt, 0, data, len);
        /*new flag indicating that the packet is for visibility or masquerade */
        pkt->flags2 = BCM_PKT_F2_RX_PORT;
        /* adding visibility option to packet flag */
        pkt->flags2 |= BCM_PKT_F2_VISIBILITY_PKT;
        /* setting pkt trace src port number and profile id on the packet */
        pkt->rx_port = port;

        /* store profile info to be used during tx*/
        _bcm_esw_pkt_trace_cpu_profile_set(unit, options);
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            SOC_IF_ERROR_RETURN(bcm_esw_port_gport_get(unit, port,
                                                  &gport));
            SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport,
                                                       &mod_id, &pp_port,
                                                       &tgid, &id));
            BCM_IF_ERROR_RETURN(
                _bcm_td3_pkt_trace_source_trunk_map_pp_set(unit,
                           mod_id,
                           pp_port,
                           0));
            BCM_IF_ERROR_RETURN(_bcm_td3_pkt_trace_init(unit));
            BCM_IF_ERROR_RETURN(
                _bcm_td3_pkt_trace_dop_config_set(unit));
        }

        /* transmit packet -- support only synchronous mode */
        BCM_IF_ERROR_RETURN(bcm_esw_tx(unit, pkt, NULL));
        soc_timeout_init(&to, SECOND_USEC, 0);

        if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            for (;;num_trial++) {
                rv = _bcm_td3_pkt_trace_raw_data_get(unit,
                                          raw_data_size,
                                          raw_data);
                if (rv == BCM_E_NONE) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    rv = BCM_E_TIMEOUT;
                    break;
                }
                sal_usleep(SECOND_USEC/4);
            }
        }
        bcm_pkt_free(unit, pkt);
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            BCM_IF_ERROR_RETURN(
            _bcm_td3_pkt_trace_source_trunk_map_pp_set(unit,
                       mod_id,
                       pp_port,
                       1));

        }
        /*mutex unlock */
        PKT_TRACE_UNLOCK(unit);
        return rv;
    }
    return BCM_E_NONE;
}
/*
 * Function:    _bcm_esw_pkt_trace_free_resources
 * Purpose:     Free pkt trace resources
 * Parameters:  unit - SOC unit number
 * Returns:     Nothing
 */
STATIC void
_bcm_esw_pkt_trace_free_resources(int unit)
{
    if (_pkt_trace_mlock[unit]) {
        sal_mutex_destroy(_pkt_trace_mlock[unit]);
        _pkt_trace_mlock[unit] = NULL;
    }
}

/* De-initialize pkt trace module */
STATIC int
_bcm_esw_pkt_trace_cleanup(int unit)
{
    if (soc_feature(unit, soc_feature_visibility)) {
        _bcm_esw_pkt_trace_free_resources(unit);

        _pkt_trace_initialized[unit] = FALSE;

        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

/* Initialize pkt trace cpu profile table */
int 
_bcm_esw_cpu_pkt_profile_init(int unit)
{
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_cpu_profile_init(unit));
        return BCM_E_NONE;
    }  else if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_td3_pkt_trace_cpu_profile_init(unit));
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL; 
}

/* Initialize pkt trace module */
int
bcm_esw_pkt_trace_init(int unit)
{
    UNIT_VALID_CHECK(unit);
    if (soc_feature(unit, soc_feature_visibility)) {
        if (_pkt_trace_initialized[unit]) {
            BCM_IF_ERROR_RETURN(_bcm_esw_pkt_trace_cleanup(unit));
        }
        if (_pkt_trace_mlock[unit] == NULL) {
            _pkt_trace_mlock[unit] = sal_mutex_create("packet trace mutex");
            if (_pkt_trace_mlock[unit] == NULL) {
                _bcm_esw_pkt_trace_cleanup(unit);
                return BCM_E_MEMORY;
            }
        }
        _pkt_trace_initialized[unit] = TRUE;

        BCM_IF_ERROR_RETURN(_bcm_esw_cpu_pkt_profile_init(unit));

        if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_td3_pkt_trace_init(unit));
        }
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;    
}

#if defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * Function:
 *      _bcm_th3_pkt_trace_info_get
 *
 * Purpose:
 *      Generate a visibility packet from the given payload and inject it.
 *      And/Or read the visibility packet's ingress pipeline processing
 *      information from PTR_RESULTS_BUFFER_XXX. Provide back the raw data
 *      and decoded information into the given bcm_switch_pkt_trace_info_t *
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) FLAGS to select pre-configured cpu_pkt_profile register
 *                     values and to provide additional instructions to the SDK.
 *      port - (IN) Logical port # to inject and/or read visibility data from.
 *      len - (IN) Length, in bytes, of the given packet.
 *      data - (IN) Packet bytes including the ETH Header.
 *      pkt_trace_info - (OUT) Visibility pkt trace process result
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      When options flag - 'BCM_PKT_TRACE_RETRIEVE_DATA_ONLY' is set,
 *      1) SDK would just retrieve the captured instrumentation data and not
 *          generate/inject any visibility packet. Hence, parameters 'data' and
 *          'len' are ignored.
 *      2) Parameter 'port' would accept value '-1'. In such scenario, SDK would
 *          poll data on each ingress pipe and return the first available data.
 */
int _bcm_th3_pkt_trace_info_get(int unit, uint32 options,
                                uint8 port, int len, uint8 *data,
                                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    int             rv = BCM_E_UNAVAIL;
    int             pipe = -1;
    bcm_pkt_t       *pkt = NULL;
    soc_timeout_t   to;

    UNIT_VALID_CHECK(unit);

    if (soc_feature(unit, soc_feature_visibility)) {

        /* Check for invalid input */
        if (pkt_trace_info == NULL) {
            return BCM_E_PARAM;
        }
        PKT_TRACE_INIT_CHECK(unit);

        if (!(options & BCM_PKT_TRACE_RETRIEVE_DATA_ONLY)) {
            /* Check if packet payload is not given */
            if (NULL == data) {
                return BCM_E_PARAM;
            }

            /* Check if the given port is invalid or not addressable */
            if (((uint8)-1) == port) {
                return BCM_E_PORT;
            }
            if (!IS_ALL_PORT(unit, port)) {
                return BCM_E_PORT;
            }
        }

        /* If the port is a valid port then extract the pipe from it */
        if (((uint8)-1) != port) {
            if (SOC_FAILURE(soc_port_pipe_get(unit, port, &pipe))) {
                return BCM_E_PORT;
            }
        }

        /* Clear the given buffer to begin-with */
        memset(pkt_trace_info, 0x0, sizeof(bcm_switch_pkt_trace_info_t));

        /* Mutex lock*/
        PKT_TRACE_LOCK(unit);

        /* If Caller asked for synchronous mode then send the packet too */
        if (!(options & BCM_PKT_TRACE_RETRIEVE_DATA_ONLY)) {

            /* Allocate and fill-in pkt data structure */
            BCM_IF_ERROR_RETURN(bcm_pkt_alloc(unit, len, 0, &pkt));
            pkt->call_back = 0;
            pkt->blk_count = 1;
            pkt->unit = unit;
            rv = bcm_pkt_memcpy(pkt, 0, data, len);

            /* New flag indicating that the packet is for visibility or masquerade */
            pkt->flags2 = BCM_PKT_F2_RX_PORT | BCM_PKT_F2_VISIBILITY_PKT;
            /* Setting pkt trace src port number in the packet */
            pkt->rx_port = port;

            /* Store profile info to be used during tx*/
            _bcm_esw_pkt_trace_cpu_profile_set(unit, options);

            /* Transmit packet -- only in synchronous mode */
            BCM_IF_ERROR_RETURN(bcm_esw_tx(unit, pkt, NULL));
        }

        /* Set the time for timeout */
        soc_timeout_init(&to, SECOND_USEC, 0);

        /* Start retrieving the captured information */
        for (;;) {
            rv =
             _bcm_th3_pkt_trace_info_retrieve_data(unit, pipe, pkt_trace_info);
            if (rv == BCM_E_NONE) {
                break;
            }
            if (soc_timeout_check(&to)) {
                rv = BCM_E_TIMEOUT;
                break;
            }
            sal_usleep(SECOND_USEC/4);
        }

        /* De-allocate the packet if allocated */
        if (NULL != pkt) {
            bcm_pkt_free(unit, pkt);
        }

        /* Mutex unlock */
        PKT_TRACE_UNLOCK(unit);
    }
    return rv;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_INSTRUMENTATION_SUPPORT */
