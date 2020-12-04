/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_OOB_H
#define _BCM_OOB_H

#include <soc/defs.h>
#include <soc/types.h>

#define _BCM_OOB_UNIT_DRIVER(u)   _bcm_oob_unit_driver[(u)]

typedef int (*_bcm_oob_fc_tx_config_set_f)(int, bcm_oob_fc_tx_config_t*);
typedef int (*_bcm_oob_fc_tx_config_get_f)(int, bcm_oob_fc_tx_config_t*);
typedef int (*_bcm_oob_fc_tx_info_get_f)(int, bcm_oob_fc_tx_info_t*);

typedef int (*_bcm_oob_fc_rx_port_tc_mapping_multi_set_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                          bcm_gport_t, int, uint32*,
                                                          uint32*);
typedef int (*_bcm_oob_fc_rx_port_tc_mapping_multi_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                          bcm_gport_t, int, uint32*,
                                                          uint32*, int*);
typedef int (*_bcm_oob_fc_rx_port_tc_mapping_set_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                    bcm_gport_t, uint32, uint32);
typedef int (*_bcm_oob_fc_rx_port_tc_mapping_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                    bcm_gport_t, uint32, uint32*);
typedef int (*_bcm_oob_fc_rx_config_set_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                bcm_oob_fc_rx_config_t*, int,
                                                bcm_gport_t*);
typedef int (*_bcm_oob_fc_rx_config_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                bcm_oob_fc_rx_config_t*, int,
                                                bcm_gport_t*, int*);
typedef int (*_bcm_oob_fc_rx_port_offset_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                bcm_gport_t, uint32*);

typedef int (*_bcm_oob_stats_config_set_f)(int unit, bcm_oob_stats_config_t *config);
typedef int (*_bcm_oob_stats_config_get_f)(int unit, bcm_oob_stats_config_t *config);
typedef int (*_bcm_oob_stats_pool_mapping_multi_set_f)(int unit, int array_count,
                                                       int *offset_array, uint8 *dir_array,
                                                       bcm_service_pool_id_t *pool_array);
typedef int (*_bcm_oob_stats_pool_mapping_multi_get_f)(int unit, int array_max,
                                                       int *offset_array, uint8 *dir_array,
                                                       bcm_service_pool_id_t *pool_array,
                                                       int *array_count);
typedef int (*_bcm_oob_stats_pool_mapping_set_f)(int unit, int offset, uint8 dir,
                                                bcm_service_pool_id_t pool);
typedef int (*_bcm_oob_stats_pool_mapping_get_f)(int unit, int offset, uint8 *dir,
                                                bcm_service_pool_id_t *pool);
typedef int (*_bcm_oob_stats_queue_mapping_multi_set_f)(int unit, int array_count,
                                                     int *offset_array,
                                                     bcm_gport_t *gport_array);
typedef int (*_bcm_oob_stats_queue_mapping_multi_get_f)(int unit, int array_max,
                                                       int *offset_array,
                                                       bcm_gport_t *gport_array,
                                                       int *array_count);
typedef int (*_bcm_oob_stats_queue_mapping_set_f)(int unit, int offset,
                                                  bcm_gport_t gport);
typedef int (*_bcm_oob_stats_queue_mapping_get_f)(int unit, int offset,
                                                  bcm_gport_t *gport);

typedef struct _bcm_oob_unit_driver_s {
    _bcm_oob_fc_tx_config_set_f                     fc_tx_config_set;
    _bcm_oob_fc_tx_config_get_f                     fc_tx_config_get;
    _bcm_oob_fc_tx_info_get_f                       fc_tx_info_get;
    _bcm_oob_fc_rx_port_tc_mapping_multi_set_f      fc_rx_port_tc_mapping_multi_set;
    _bcm_oob_fc_rx_port_tc_mapping_multi_get_f      fc_rx_port_tc_mapping_multi_get;
    _bcm_oob_fc_rx_port_tc_mapping_set_f            fc_rx_port_tc_mapping_set;
    _bcm_oob_fc_rx_port_tc_mapping_get_f            fc_rx_port_tc_mapping_get;
    _bcm_oob_fc_rx_config_set_f                     fc_rx_config_set;
    _bcm_oob_fc_rx_config_get_f                     fc_rx_config_get;
    _bcm_oob_fc_rx_port_offset_get_f                fc_rx_port_offset_get;
    _bcm_oob_stats_config_set_f                     stats_config_set;
    _bcm_oob_stats_config_get_f                     stats_config_get;
    _bcm_oob_stats_pool_mapping_multi_set_f         stats_pool_mapping_multi_set;
    _bcm_oob_stats_pool_mapping_multi_get_f         stats_pool_mapping_multi_get;
    _bcm_oob_stats_pool_mapping_set_f               stats_pool_mapping_set;
    _bcm_oob_stats_pool_mapping_get_f               stats_pool_mapping_get;
    _bcm_oob_stats_queue_mapping_multi_set_f        stats_queue_mapping_multi_set;
    _bcm_oob_stats_queue_mapping_multi_get_f        stats_queue_mapping_multi_get;
    _bcm_oob_stats_queue_mapping_set_f              stats_queue_mapping_set;
    _bcm_oob_stats_queue_mapping_get_f              stats_queue_mapping_get;
} _bcm_oob_unit_driver_t;

extern int _bcm_oob_init(int unit);
extern void _bcm_oob_deinit(int unit);

#if defined(BCM_TRIDENT3_SUPPORT)
extern int bcm_oob_td3_init(int unit);
#endif /* BCM_TOMAHAWK_SUPPORT */


#if defined(BCM_TOMAHAWK_SUPPORT)
extern int bcm_oob_th_init(int unit);
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
extern int bcm_oob_th3_init(int unit);
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_APACHE_SUPPORT)
extern int bcm_oob_ap_init(int unit);
#endif /* BCM_APACHE_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
extern int bcm_oob_kt2_init(int unit);
#endif /* BCM_KATANA2_SUPPORT */
#endif /* _BCM_OOB_H */
