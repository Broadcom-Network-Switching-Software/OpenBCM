/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * OOB Flow Control and OOB Stats
 * Purpose: API to set different OOB Flow Control and OOB Stats registers.
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/oob.h>

#include <bcm_int/esw/oob.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/apache.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stat.h>

#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

_bcm_oob_unit_driver_t *_bcm_oob_unit_driver[BCM_MAX_NUM_UNITS];


/*
 * Function:
 *      _bcm_oob_deinit
 * Purpose:
 *      Deinitialize OOB driver functions.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
void _bcm_oob_deinit(int unit)
{
    _bcm_oob_unit_driver_t *oob_driver;

    oob_driver = _BCM_OOB_UNIT_DRIVER(unit);
    if (oob_driver == NULL) {
        return;
    }

    sal_free(oob_driver);
    _BCM_OOB_UNIT_DRIVER(unit) = NULL;

    return;
}


/*
 * Function:
 *      _bcm_oob_init
 * Purpose:
 *      Initialize OOB driver functions.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_oob_init(int unit)
{
    _bcm_oob_unit_driver_t  *oob_driver;
    int                     oob_supported = 0;
    int                     rv = BCM_E_NONE;
    _bcm_oob_deinit(unit);
    oob_driver = _BCM_OOB_UNIT_DRIVER(unit);

    if (oob_driver == NULL) {
        oob_driver = sal_alloc(sizeof(_bcm_oob_unit_driver_t), "oob_unit_driver");
        if (oob_driver == NULL) {
            return BCM_E_MEMORY;
        }
    }

    sal_memset(oob_driver, 0, sizeof(_bcm_oob_unit_driver_t));

    _BCM_OOB_UNIT_DRIVER(unit) = oob_driver;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        rv = bcm_oob_td3_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_oob_deinit(unit);
            return rv;
        }
        oob_supported = 1;
    }
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_oob_th3_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_oob_deinit(unit);
            return rv;
        }
        oob_supported = 1;
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_oob_th_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_oob_deinit(unit);
            return rv;
        }
        oob_supported = 1;
    }
#endif 
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        rv = bcm_oob_kt2_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_oob_deinit(unit);
            return rv;
        }
        oob_supported = 1;
    }
#endif 
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        rv = bcm_oob_ap_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)_bcm_oob_deinit(unit);
            return rv;
        }
        oob_supported = 1; 
    }
#endif
    if (oob_supported == 0) {
        return BCM_E_UNAVAIL;
    }
    return rv;
}

#if defined(BCM_TOMAHAWK3_SUPPORT)
int bcm_oob_th3_init(int unit)
{
    _bcm_oob_unit_driver_t *oob_driver;

    oob_driver = _BCM_OOB_UNIT_DRIVER(unit);
    if (oob_driver == NULL) {
        return BCM_E_MEMORY;
    }

    /* Initializing Driver functions for OOB FC */
    oob_driver->fc_tx_config_set = bcm_th3_oob_fc_tx_config_set;
    oob_driver->fc_tx_config_get = bcm_th3_oob_fc_tx_config_get;
    oob_driver->fc_tx_info_get = bcm_th3_oob_fc_tx_info_get;
     /* OOBSTAT and OOB RX are not supported in TH3 */
    oob_driver->fc_rx_port_tc_mapping_multi_set = NULL;
    oob_driver->fc_rx_port_tc_mapping_multi_get = NULL;
    oob_driver->fc_rx_port_tc_mapping_set = NULL;
    oob_driver->fc_rx_port_tc_mapping_get = NULL;
    oob_driver->fc_rx_config_set = NULL;
    oob_driver->fc_rx_config_get = NULL;
    oob_driver->fc_rx_port_offset_get = NULL;

    oob_driver->stats_config_set = NULL;
    oob_driver->stats_config_get = NULL;
    oob_driver->stats_pool_mapping_multi_set = NULL;
    oob_driver->stats_pool_mapping_multi_get = NULL;
    oob_driver->stats_pool_mapping_set = NULL;
    oob_driver->stats_pool_mapping_get = NULL;
    oob_driver->stats_queue_mapping_multi_set = NULL;
    oob_driver->stats_queue_mapping_multi_get = NULL;
    oob_driver->stats_queue_mapping_set = NULL;
    oob_driver->stats_queue_mapping_get = NULL;

#ifdef BCM_WARM_BOOT_SUPPORT
    if(SOC_WARM_BOOT(unit)) {
        return BCM_E_NONE;
    }
#endif
    /* oob port mapping init if not warmboot*/
    BCM_IF_ERROR_RETURN(bcm_th3_oob_port_mapping_init(unit));
    return BCM_E_NONE;
}
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
/*
 * Function:
 *      _bcm_oob_th_init
 * Purpose:
 *      Initialize OOB driver functions
 *      for Tomahawk (BCM56960).
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_oob_th_init(int unit)
{
    _bcm_oob_unit_driver_t *oob_driver;

    oob_driver = _BCM_OOB_UNIT_DRIVER(unit);
    if (oob_driver == NULL) {
        return BCM_E_MEMORY;
    }

    /* Initializing Driver functions for OOB FC */
    oob_driver->fc_tx_config_set = bcm_th_oob_fc_tx_config_set;
    oob_driver->fc_tx_config_get = bcm_th_oob_fc_tx_config_get;
    oob_driver->fc_tx_info_get = bcm_th_oob_fc_tx_info_get;
    oob_driver->fc_rx_port_tc_mapping_multi_set = bcm_th_oob_fc_rx_port_tc_mapping_multi_set;
    oob_driver->fc_rx_port_tc_mapping_multi_get = bcm_th_oob_fc_rx_port_tc_mapping_multi_get;
    oob_driver->fc_rx_port_tc_mapping_set = bcm_th_oob_fc_rx_port_tc_mapping_set;
    oob_driver->fc_rx_port_tc_mapping_get = bcm_th_oob_fc_rx_port_tc_mapping_get;
    oob_driver->fc_rx_config_set = bcm_th_oob_fc_rx_config_set;
    oob_driver->fc_rx_config_get = bcm_th_oob_fc_rx_config_get;
    oob_driver->fc_rx_port_offset_get = bcm_th_oob_fc_rx_port_offset_get;

    /* oob stats init */
    if (1 != soc_property_get(unit, spn_BUFFER_STATS_COLLECT_TYPE, 1)) {
        return BCM_E_NONE;
    }
    /* Initializing Driver functions for OOB Stats */
    oob_driver->stats_config_set = bcm_th_oob_stats_config_set;
    oob_driver->stats_config_get = bcm_th_oob_stats_config_get;
    oob_driver->stats_pool_mapping_multi_set = bcm_th_oob_stats_pool_mapping_multi_set;
    oob_driver->stats_pool_mapping_multi_get = bcm_th_oob_stats_pool_mapping_multi_get;
    oob_driver->stats_pool_mapping_set = bcm_th_oob_stats_pool_mapping_set;
    oob_driver->stats_pool_mapping_get = bcm_th_oob_stats_pool_mapping_get;
    oob_driver->stats_queue_mapping_multi_set = bcm_th_oob_stats_queue_mapping_multi_set;
    oob_driver->stats_queue_mapping_multi_get = bcm_th_oob_stats_queue_mapping_multi_get;
    oob_driver->stats_queue_mapping_set = bcm_th_oob_stats_queue_mapping_set;
    oob_driver->stats_queue_mapping_get = bcm_th_oob_stats_queue_mapping_get;

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        uint32 flag;

#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            int config, hw_mode;

            /* get oob config from HW */
            SOC_IF_ERROR_RETURN(soc_tomahawk2_mmu_pstats_mode_get(unit, &flag));

            /* HW config is different with config property */
            if (!(flag & _TH2_MMU_PSTATS_ENABLE) ||
                (flag & _TH2_MMU_PSTATS_PKT_MOD)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "WARMBOOT ERROR.\n"
                                      "Current buffer stats mode is not oob.\n")));
                return BCM_E_INIT;
            }

            hw_mode = (flag & _TH2_MMU_PSTATS_HWM_MOD) ? 1 : 0;

            config = soc_property_get(unit, spn_BUFFER_STATS_COLLECT_MODE, 0);
            if (config != hw_mode) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "WARMBOOT WARNING.\n"
                                     "Config mode %d. HW mode %d.\n"),
                                     config, hw_mode));
            }

            /* Reset oob mode in warmboot */
            SOC_IF_ERROR_RETURN(soc_tomahawk2_mmu_pstats_mode_set(unit, flag));
        } else
#endif
        {
            flag = _TH2_MMU_PSTATS_ENABLE;
            if (soc_property_get(unit, spn_BUFFER_STATS_COLLECT_MODE, 0)) {
                flag |= _TH2_MMU_PSTATS_HWM_MOD;
                if (soc_property_get(unit, "oob_pstats_hw_cor_en", 1)) {
                    flag |= _TH2_MMU_PSTATS_RESET_ON_READ;
                }
            }
            SOC_IF_ERROR_RETURN(soc_tomahawk2_mmu_pstats_mode_set(unit, flag));
        }
    }
#endif

    return BCM_E_NONE;
}
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_oob_td3_init
 * Purpose:
 *      Initialize OOB driver functions
 *      for Trident3 (BCM56870).
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_oob_td3_init(int unit)
{
    _bcm_oob_unit_driver_t *oob_driver;

    oob_driver = _BCM_OOB_UNIT_DRIVER(unit);
    if (oob_driver == NULL) {
        return BCM_E_MEMORY;
    }

    /* Initializing Driver functions for OOB FC */
    oob_driver->fc_tx_config_set = bcm_td3_oob_fc_tx_config_set;
    oob_driver->fc_tx_config_get = bcm_td3_oob_fc_tx_config_get;
    oob_driver->fc_tx_info_get = bcm_td3_oob_fc_tx_info_get;
    oob_driver->fc_rx_port_tc_mapping_multi_set = bcm_td3_oob_fc_rx_port_tc_mapping_multi_set;
    oob_driver->fc_rx_port_tc_mapping_multi_get = bcm_td3_oob_fc_rx_port_tc_mapping_multi_get;
    oob_driver->fc_rx_port_tc_mapping_set = bcm_td3_oob_fc_rx_port_tc_mapping_set;
    oob_driver->fc_rx_port_tc_mapping_get = bcm_td3_oob_fc_rx_port_tc_mapping_get;
    oob_driver->fc_rx_config_set = bcm_td3_oob_fc_rx_config_set;
    oob_driver->fc_rx_config_get = bcm_td3_oob_fc_rx_config_get;
    oob_driver->fc_rx_port_offset_get = bcm_td3_oob_fc_rx_port_offset_get;

    /* oob stats init */
    if (1 != soc_property_get(unit, spn_BUFFER_STATS_COLLECT_TYPE, 1)) {
        return BCM_E_NONE;
    }
    /* Initializing Driver functions for OOB Stats */
    oob_driver->stats_config_set = bcm_td3_oob_stats_config_set;
    oob_driver->stats_config_get = bcm_td3_oob_stats_config_get;
    oob_driver->stats_pool_mapping_multi_set = bcm_td3_oob_stats_pool_mapping_multi_set;
    oob_driver->stats_pool_mapping_multi_get = bcm_td3_oob_stats_pool_mapping_multi_get;
    oob_driver->stats_pool_mapping_set = bcm_td3_oob_stats_pool_mapping_set;
    oob_driver->stats_pool_mapping_get = bcm_td3_oob_stats_pool_mapping_get;
    oob_driver->stats_queue_mapping_multi_set = bcm_td3_oob_stats_queue_mapping_multi_set;
    oob_driver->stats_queue_mapping_multi_get = bcm_td3_oob_stats_queue_mapping_multi_get;
    oob_driver->stats_queue_mapping_set = bcm_td3_oob_stats_queue_mapping_set;
    oob_driver->stats_queue_mapping_get = bcm_td3_oob_stats_queue_mapping_get;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        uint32 flag;

#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            int config, hw_mode;

            /* get oob config from HW */
            SOC_IF_ERROR_RETURN(soc_trident3_mmu_pstats_mode_get(unit, &flag));

            /* HW config is different with config property */
            if (!(flag & _TD3_MMU_PSTATS_ENABLE) ||
                (flag & _TD3_MMU_PSTATS_PKT_MOD)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "WARMBOOT ERROR.\n"
                                      "Current buffer stats mode is not oob.\n")));
                return BCM_E_INIT;
            }

            hw_mode = (flag & _TD3_MMU_PSTATS_HWM_MOD) ? 1 : 0;

            config = soc_property_get(unit, spn_BUFFER_STATS_COLLECT_MODE, 0);
            if (config != hw_mode) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "WARMBOOT WARNING.\n"
                                     "Config mode %d. HW mode %d.\n"),
                                     config, hw_mode));
            }

            /* Reset oob mode in warmboot */
            SOC_IF_ERROR_RETURN(soc_trident3_mmu_pstats_mode_set(unit, flag));
        } else
#endif
        {
            flag = _TD3_MMU_PSTATS_ENABLE;
            if (soc_property_get(unit, spn_BUFFER_STATS_COLLECT_MODE, 0)) {
                flag |= _TD3_MMU_PSTATS_HWM_MOD;
                if (soc_property_get(unit, "oob_pstats_hw_cor_en", 1)) {
                    flag |= _TD3_MMU_PSTATS_RESET_ON_READ;
                }
            }
            SOC_IF_ERROR_RETURN(soc_trident3_mmu_pstats_mode_set(unit, flag));
        }
    }
#endif

    return BCM_E_NONE;
}
#endif
#if defined(BCM_APACHE_SUPPORT)
/*
 * Function:
 *      _bcm_oob_ap_init
 * Purpose:
 *      Initialize OOB driver functions
 *      for Tomahawk (BCM56960).
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_oob_ap_init(int unit)
{
    _bcm_oob_unit_driver_t *oob_driver;

    oob_driver = _BCM_OOB_UNIT_DRIVER(unit);
    if (oob_driver == NULL) {
        return BCM_E_MEMORY;
    }

    /* Initializing Driver functions for OOB FC */
    oob_driver->fc_tx_config_set = bcm_ap_oob_fc_tx_config_set;
    oob_driver->fc_tx_config_get = bcm_ap_oob_fc_tx_config_get;
    oob_driver->fc_tx_info_get = bcm_ap_oob_fc_tx_info_get;
    oob_driver->fc_rx_port_tc_mapping_multi_set = bcm_ap_oob_fc_rx_port_tc_mapping_multi_set;
    oob_driver->fc_rx_port_tc_mapping_multi_get = bcm_ap_oob_fc_rx_port_tc_mapping_multi_get;
    oob_driver->fc_rx_port_tc_mapping_set = bcm_ap_oob_fc_rx_port_tc_mapping_set;
    oob_driver->fc_rx_port_tc_mapping_get = bcm_ap_oob_fc_rx_port_tc_mapping_get;
    oob_driver->fc_rx_config_set = bcm_ap_oob_fc_rx_config_set;
    oob_driver->fc_rx_config_get = bcm_ap_oob_fc_rx_config_get;
    oob_driver->fc_rx_port_offset_get = bcm_ap_oob_fc_rx_port_offset_get;
    /* Note that Stats related APIs are not supported on apache */


    return BCM_E_NONE;
}
#endif
#if defined(BCM_KATANA2_SUPPORT)
/*
 * Function:
 *      _bcm_oob_kt2_init
 * Purpose:
 *      Initialize OOB driver functions
 *      for Katana_2 (BCM56450).
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_oob_kt2_init(int unit)
{
    _bcm_oob_unit_driver_t *oob_driver;

    oob_driver = _BCM_OOB_UNIT_DRIVER(unit);
    if (oob_driver == NULL) {
        return BCM_E_MEMORY;
    }

    /* Initializing Driver functions for OOB FC */
    oob_driver->fc_tx_config_set = bcm_kt2_oob_fc_tx_config_set;
    oob_driver->fc_tx_config_get = bcm_kt2_oob_fc_tx_config_get;
    oob_driver->fc_tx_info_get = bcm_kt2_oob_fc_tx_info_get;
    oob_driver->fc_rx_port_tc_mapping_multi_set = bcm_kt2_oob_fc_rx_port_tc_mapping_multi_set;
    oob_driver->fc_rx_port_tc_mapping_multi_get = bcm_kt2_oob_fc_rx_port_tc_mapping_multi_get;
    oob_driver->fc_rx_port_tc_mapping_set = bcm_kt2_oob_fc_rx_port_tc_mapping_set;
    oob_driver->fc_rx_port_tc_mapping_get = bcm_kt2_oob_fc_rx_port_tc_mapping_get;
    oob_driver->fc_rx_config_set = bcm_kt2_oob_fc_rx_config_set;
    oob_driver->fc_rx_config_get = bcm_kt2_oob_fc_rx_config_get;
    oob_driver->fc_rx_port_offset_get = bcm_kt2_oob_fc_rx_port_offset_get;
    /* Note that Stats related APIs are not supported on Kt2 */
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_esw_oob_fc_tx_config_set
 * Purpose:
 *      Set OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (IN) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_tx_config_set(
                 int unit,
                 bcm_oob_fc_tx_config_t *config)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_tx_config_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_tx_config_set(unit,
                                                        config);
}

/*
 * Function:
 *      bcm_esw_oob_fc_tx_config_get
 * Purpose:
 *      Get OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_tx_config_get(
                 int unit,
                 bcm_oob_fc_tx_config_t *config)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_tx_config_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_tx_config_get(unit,
                                                        config);
}

/*
 * Function:
 *      bcm_esw_oob_fc_tx_info_get
 * Purpose:
 *      Get OOB FC Tx global information.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      info - (OUT) OOB FC Tx global information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_tx_info_get(
                 int unit,
                 bcm_oob_fc_tx_info_t *info)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_tx_info_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_tx_info_get(unit,
                                                      info);
}

/*
 * Function:
 *      bcm_esw_oob_fc_rx_port_tc_mapping_multi_set
 * Purpose:
 *      Set the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      array_count - (IN) Number of elements in Traffic Class array
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (IN) Priority Bitmap Array[0-7]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_rx_port_tc_mapping_multi_set(
                 int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_gport_t gport,
                 int array_count,
                 uint32 *tc,
                 uint32 *pri_bmp)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_multi_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_multi_set(
                                unit, intf_id, gport, array_count, tc, pri_bmp);
}

/*
 * Function:
 *      bcm_esw_oob_fc_rx_port_tc_mapping_multi_get
 * Purpose:
 *      Set the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      array_max - (IN) Maximum Length tc and pri_bmp array
 *      tc - (OUT) Traffic Class array[0-7]
 *      pri_bmp - (OUT) Priority Bitmap Array[0-7]
 *      array_count - (OUT) Number of elements in Traffic Class array
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_rx_port_tc_mapping_multi_get(
                 int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_gport_t gport,
                 int array_max,
                 uint32 *tc,
                 uint32 *pri_bmp,
                 int *array_count)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_multi_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_multi_get(
                                unit, intf_id, gport, array_max, tc,
                                pri_bmp, array_count);
}

/*
 * Function:
 *      bcm_esw_oob_fc_rx_port_tc_mapping_set
 * Purpose:
 *      Set the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (IN) Priority Bitmap Array[0-7]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_rx_port_tc_mapping_set(
                 int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_gport_t gport,
                 uint32 tc,
                 uint32 pri_bmp)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_set(
                                unit, intf_id, gport, tc, pri_bmp);
}

/*
 * Function:
 *      bcm_esw_oob_fc_rx_port_tc_mapping_get
 * Purpose:
 *      Set the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (OUT) Priority Bitmap Array[0-7]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_rx_port_tc_mapping_get(
                 int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_gport_t gport,
                 uint32 tc,
                 uint32 *pri_bmp)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_rx_port_tc_mapping_get(
                                unit, intf_id, gport, tc,
                                pri_bmp);
}

/*
 * Function:
 *      bcm_esw_oob_fc_rx_config_set
 * Purpose:
 *      Set the configuration and enable 
 *      each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      config - (IN) OOB FC Rx Interface Config
 *      array_count - (IN) Number of gports in the gport array
 *      gport_array - (IN) Array of modport type gport
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_rx_config_set(
                 int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_oob_fc_rx_config_t *config,
                 int array_count,
                 bcm_gport_t *gport_array)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_rx_config_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_rx_config_set(
                                unit, intf_id, config,
                                array_count, gport_array);
}

/*
 * Function:
 *      bcm_esw_oob_fc_rx_config_get
 * Purpose:
 *      Get the configuration and status
 *      each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      config - (OUT) OOB FC Rx Interface Config
 *      array_max - (IN) Maximum Lenght of Gport array passed
 *      gport_array - (OUT) Array of modport type gport
 *      array_count - (OUT) Number of gports returned in the gport array
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_rx_config_get(
                 int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_oob_fc_rx_config_t *config,
                 int array_max,
                 bcm_gport_t *gport_array,
                 int *array_count)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_rx_config_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_rx_config_get(
                                unit, intf_id, config,
                                array_max, gport_array, array_count);
}

/*
 * Function:
 *      bcm_esw_oob_fc_rx_port_offset_get
 * Purpose:
 *      Get the Channel offset value for a given port
 *      on each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      offset - (OUT) Channel offset value in HCFC message
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_oob_fc_rx_port_offset_get(
                 int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_gport_t gport,
                 uint32 *offset)
{
    if (!soc_feature(unit, soc_feature_oob_fc)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->fc_rx_port_offset_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->fc_rx_port_offset_get(
                                unit, intf_id, gport, offset);
}

/*
 * Function:
 *      bcm_esw_oob_stats_config_set
 * Purpose:
 *      Set the global configuration setting
 *      for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      config - (IN) OOB Stats Configuration structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_config_set(
                int unit,
                bcm_oob_stats_config_t *config)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_config_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_config_set(unit,
                                                        config);
}

/*
 * Function:
 *      bcm_esw_oob_stats_config_get
 * Purpose:
 *      Get the global configuration setting
 *      for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      config - (OUT) OOB Stats Configuration structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_config_get(
                int unit,
                bcm_oob_stats_config_t *config)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_config_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_config_get(unit,
                                                        config);
}

/*
 * Function:
 *      bcm_esw_oob_stats_pool_mapping_multi_set
 * Purpose:
 *      Set the service pool id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_count - (IN) Number of elements in array
 *      offset_array - (IN) Indexes at which service pool
 *                      in the service pool list needs to be added
 *      dir_array - (IN) Direction array
 *      pool_array - (IN) Service Pool ID array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_pool_mapping_multi_set(
                int unit, int array_count, int *offset_array,
                uint8 *dir_array, bcm_service_pool_id_t *pool_array)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_pool_mapping_multi_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_pool_mapping_multi_set(unit,
                                            array_count, offset_array,
                                            dir_array, pool_array);
}

/*
 * Function:
 *      bcm_esw_oob_stats_pool_mapping_multi_get
 * Purpose:
 *      Get the service pool id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_max - (IN) Number of elements in array
 *      offset_array - (IN) Indexes at which service pool
 *                      in the service pool list needs to be added
 *      dir_array - (OUT) Direction array
 *      pool_array - (OUT) Service Pool ID array
 *      array_count - (OUT) Number of elements returned in the array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_pool_mapping_multi_get(
                int unit, int array_max, int *offset_array, uint8 *dir_array,
                bcm_service_pool_id_t *pool_array, int *array_count)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_pool_mapping_multi_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_pool_mapping_multi_get(unit,
                                            array_max, offset_array, dir_array,
                                            pool_array, array_count);
}

/*
 * Function:
 *      bcm_esw_oob_stats_pool_mapping_set
 * Purpose:
 *      Set the service pool id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which service pool
 *               in the service pool list needs to be added
 *      dir - (IN) Direction
 *      pool - (IN) Service Pool ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_pool_mapping_set(
                int unit, int offset, uint8 dir,
                bcm_service_pool_id_t pool)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_pool_mapping_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_pool_mapping_set(unit,
                                                offset, dir, pool);
}

/*
 * Function:
 *      bcm_esw_oob_stats_pool_mapping_get
 * Purpose:
 *      Get the service pool id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which service pool
 *               in the service pool list needs to be added
 *      dir - (OUT) Direction
 *      pool - (OUT) Service Pool ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_pool_mapping_get(
                int unit, int offset, uint8 *dir,
                bcm_service_pool_id_t *pool)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_pool_mapping_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_pool_mapping_get(unit,
                                                offset, dir, pool);
}

/*
 * Function:
 *      bcm_esw_oob_stats_queue_mapping_multi_set
 * Purpose:
 *      Set the queue id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_count - (IN) Number of elements in array
 *      offset_array - (IN) Indexes at which Queue Id
 *                      in the queue list needs to be added
 *      gport_array - (IN) Queue gport array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_queue_mapping_multi_set(
                int unit, int array_count, int *offset_array,
                bcm_gport_t *gport_array)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_queue_mapping_multi_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_queue_mapping_multi_set(unit,
                                                array_count, offset_array,
                                                gport_array);
}

/*
 * Function:
 *      bcm_esw_oob_stats_queue_mapping_multi_get
 * Purpose:
 *      Get the UC queue id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the UC queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_max - (IN) Maximum number of elements in array
 *      offset_array - (IN) Indexes at which Queue Id
 *                      in the queue list needs to be added
 *      gport_array - (OUT) Queue gport array
 *      array_count - (OUT) Number of elements in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_queue_mapping_multi_get(
                int unit, int array_max, int *offset_array,
                bcm_gport_t *gport_array, int *array_count)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_queue_mapping_multi_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_queue_mapping_multi_get(unit,
                                                array_max, offset_array,
                                                gport_array, array_count);
}

/*
 * Function:
 *      bcm_esw_oob_stats_queue_mapping_set
 * Purpose:
 *      Set the queue id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which Queue Id
 *               in the queue list needs to be added
 *      gport - (IN) Queue gport
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_queue_mapping_set(
                int unit, int offset, bcm_gport_t gport)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_queue_mapping_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_queue_mapping_set(unit,
                                                        offset, gport);
}

/*
 * Function:
 *      bcm_esw_oob_stats_queue_mapping_get
 * Purpose:
 *      Get the UC queue id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the UC queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which Queue Id
 *               in the queue list needs to be added
 *      gport - (OUT) Queue gport
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_oob_stats_queue_mapping_get(
                int unit, int offset, bcm_gport_t *gport)
{
    if (!soc_feature(unit, soc_feature_oob_stats)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_oob_unit_driver[unit]->stats_queue_mapping_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return _bcm_oob_unit_driver[unit]->stats_queue_mapping_get(unit,
                                                        offset, gport);
}
