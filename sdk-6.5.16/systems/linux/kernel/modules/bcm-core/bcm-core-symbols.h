/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <bcm/debug.h>
#include <bcm/types.h>
#include <bcm/async.h>
#include <bcm/auth.h>
#include <bcm/bcmi2c.h>
#include <bcm/cosq.h>
#include <bcm/custom.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/init.h>
#include <bcm/ipmc.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/link.h>
#include <bcm/mcast.h>
#include <bcm/mirror.h>
#include <bcm/module.h>
#include <bcm/mpls.h>
#include <bcm/pkt.h>
#include <bcm/port.h>
#include <bcm/proxy.h>
#include <bcm/rate.h>
#include <bcm/rx.h>
#include <bcm/stack.h>
#include <bcm/stat.h>
#include <bcm/stg.h>
#include <bcm/switch.h>
#include <bcm/topo.h>
#include <bcm/trunk.h>
#include <bcm/tx.h>
#include <bcm/vlan.h>
#include <bcm/ipfix.h>
#include <bcm_int/esw_dispatch.h>

#include <soc/mem.h>
#include <soc/format.h>
#ifdef BCM_ESW_SUPPORT
#include <soc/soc_ser_log.h>
#include <soc/format.h>
#endif /* BCM_ESW_SUPPORT */

#ifdef INCLUDE_MACSEC
#include <bcm_int/common/macsec_cmn.h>
#endif /* INCLUDE_MACSEC */

#ifdef BCM_ESW_SUPPORT
#ifndef BCM_SW_STATE_DUMP_DISABLE
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/link.h>
#include <bcm_int/esw/niv.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw/trill.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/mcast.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/extender.h>
#if defined(INCLUDE_FLOWTRACKER)
#include <bcm_int/esw/flowtracker.h>
#endif /* INCLUDE_FLOWTRACKER */
#if defined(BCM_COLLECTOR_SUPPORT)
#include <bcm_int/esw/collector.h>
#endif /* BCM_COLLECTOR_SUPPORT */
#if defined(INCLUDE_TELEMETRY)
#include <bcm_int/esw/telemetry.h>
#endif /* INCLUDE_TELEMETRY */
#ifdef BCM_FLOWTRACKER_SUPPORT
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_group.h>
#endif /* BCM_FLOWTRACKER_SUPPORT */
#include <bcm_int/esw/udf.h>
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#endif /* BCM_ESW_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#ifdef BCM_ESW_SUPPORT
#include <bcm_int/esw/qos.h>
#include <bcm_int/esw/fcoe.h>
#include <bcm_int/esw/ipfix.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/ecn.h>
#if defined(INCLUDE_PSTATS)
#include <bcm_int/esw/pstats.h>
#endif /* INCLUDE_PSTATS */
#ifdef BCM_TRIUMPH3_SUPPORT
#include <bcm_int/esw/l2gre.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/subport.h>
#endif /* BCM_KATANA2_SUPPORT */
#endif /* BCM_ESW_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
#include <soc/td2_td2p.h>
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/common/sw_an.h>
#endif /*BCM_TOMAHAWK_SUPPORT*/

#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#endif /*BCM_TOMAHAWK2_SUPPORT*/

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/mcm/driver.h>
#include <soc/tomahawk3.h>
#endif

#ifdef INCLUDE_PTP
#ifdef BCM_ESW_SUPPORT
#include <bcm_int/esw/ptp.h>
#include <bcm_int/common/ptp.h>
#endif
#endif

#ifdef INCLUDE_BFD
#ifdef BCM_ESW_SUPPORT
#include <bcm_int/esw/bfd.h>
#endif /* BCM_ESW_SUPPORT*/
#endif /* INCLUDE_BFD */


#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_ESW_SUPPORT
#include <bcm_int/common/mbox.h>
#include <bcm_int/common/time.h>
#include <bcm_int/common/time-mbox.h>
#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif

#ifdef BCM_GREYHOUND_SUPPORT
#include <soc/greyhound.h>
#endif

#ifdef BCM_SABER2_SUPPORT
#include <bcm_int/esw/saber2.h>
#include <soc/saber2.h>
#endif

#ifdef BCM_METROLITE_SUPPORT
#include <bcm_int/esw/metrolite.h>
#include <soc/metrolite.h>
#endif

#ifdef CANCUN_SUPPORT
#include <soc/esw/cancun.h>
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/esw/flow_db.h>
#include <soc/esw/flow_db_core.h>
#include <bcm_int/esw/flow.h>
#endif

#include <sal/core/dpc.h>
#include <sal/core/libc.h>
#include <appl/diag/system.h>
int kconfig_get_next(char **name, char **value);
char *kconfig_get(const char *name);
int kconfig_set(char *name, char *value);
char *strtok(char *s, const char *delim) { return strsep(&s,delim); }

/* Export all BCM API symbols */
#include <bcm_export.h>

#include <soc/dport.h>

EXPORT_SYMBOL(soc_dport_to_port);
EXPORT_SYMBOL(soc_dport_from_port);
EXPORT_SYMBOL(soc_dport_from_dport_idx);

#include <bcm_int/api_xlate_port.h>

#ifdef INCLUDE_BCM_API_XLATE_PORT

EXPORT_SYMBOL(_bcm_api_xlate_port_a2p);
EXPORT_SYMBOL(_bcm_api_xlate_port_p2a);
EXPORT_SYMBOL(_bcm_api_xlate_port_pbmp_a2p);
EXPORT_SYMBOL(_bcm_api_xlate_port_pbmp_p2a);

#endif /* INCLUDE_BCM_API_XLATE_PORT */

#include <soc/eyescan.h>

#ifdef PHYMOD_SUPPORT
#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <soc/phy/phymod_ctrl.h>
#include <phymod/acc/phymod_tsc_iblk.h>
EXPORT_SYMBOL(phymod_dbg_addr);
EXPORT_SYMBOL(phymod_dbg_lane);
EXPORT_SYMBOL(phymod_dbg_mask);
EXPORT_SYMBOL(phymod_field_info_count);
EXPORT_SYMBOL(phymod_field_info_decode);
EXPORT_SYMBOL(phymod_field_set);
EXPORT_SYMBOL(phymod_phy_reg_read);
EXPORT_SYMBOL(phymod_phy_reg_write);
EXPORT_SYMBOL(phymod_symbol_show_fields);
EXPORT_SYMBOL(phymod_symbols_iter);
EXPORT_SYMBOL(soc_phymod_phy_create);
EXPORT_SYMBOL(soc_phymod_phy_find_by_id);
EXPORT_SYMBOL(phymod_phy_access_t_init);
EXPORT_SYMBOL(phymod_tsc_iblk_read);
#ifdef PHYMOD_DIAG
#include <phymod/phymod_diag.h>
EXPORT_SYMBOL(phymod_diag_symbols_table_get);
EXPORT_SYMBOL(phymod_diag_dsc);
EXPORT_SYMBOL(phymod_diag_dsc_config);
EXPORT_SYMBOL(phymod_diag_dsc_std);
EXPORT_SYMBOL(phymod_diag_prbs);
EXPORT_SYMBOL(phymod_diag_print_func);
EXPORT_SYMBOL(phymod_diag_reg_read);
EXPORT_SYMBOL(phymod_diag_reg_write);
EXPORT_SYMBOL(phymod_prbs_poly_t_mapping);
EXPORT_SYMBOL(phymod_phy_diagnostics_t_init);
#endif
#endif /* PHYMOD_SUPPORT */

#if defined(PCIEG3_DIAG_SUPPORT)
#include <pcig3_phy_acc.h>
EXPORT_SYMBOL(pcie_phy_diag_reg_read);
EXPORT_SYMBOL(pcie_phy_diag_reg_write);
EXPORT_SYMBOL(pcie_phy_diag_pram_read);
EXPORT_SYMBOL(pcie_phy_diag_state);
EXPORT_SYMBOL(pcie_phy_diag_dsc);
EXPORT_SYMBOL(pcie_phy_diag_eyescan);
#endif


#ifdef BCM_ESW_SUPPORT
#include <soc/triumph.h>
#include <soc/trident.h>
#include <soc/trident2.h>
#include <soc/apache.h>
#include <soc/monterey.h>
#include <soc/td2_td2p.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2.h>
#include <soc/trident3.h>
#ifdef ALPM_ENABLE
#include <soc/alpm.h>
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/alpm.h>
#include <bcm_int/esw/alpm_util.h>
#endif
#endif
#include <soc/katana.h>
#include <soc/katana2.h>
#include <soc/hurricane2.h>
#include <soc/triumph3.h>
#include <soc/hercules.h>
#include <soc/l3x.h>
#include <soc/xaui.h>
#include <soc/mcm/driver.h>
#include <soc/er_cmdmem.h>
#include <soc/memtune.h>
#include <soc/higig.h>
#include <soc/phyctrl.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/mspi.h>
#include <soc/uc.h>
#include <soc/uc_msg.h>
#endif
#ifdef BCM_DDR3_SUPPORT
#include <soc/shmoo_ddr40.h>
#endif


#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/ipfix.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/nat.h>
#include <bcm_int/esw/virtual.h>
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/flex_ctr.h>
#endif
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#include <bcm_int/esw/monterey.h>
#endif
#if defined(BCM_APACHE_SUPPORT)
#include <bcm_int/esw/apache.h>
#endif
#if defined(BCM_HELIX5_SUPPORT)
#include <bcm_int/esw/helix5.h>
#endif
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#endif
#include <bcm_int/control.h>
#endif /* BCM_ESW_SUPPORT */

/*
 * Export system-related symbols and symbols
 * required by the diag shell .
 * ESW defined
 */

EXPORT_SYMBOL(shr_bitop_range_or);
EXPORT_SYMBOL(bcore_debug_set_default);
EXPORT_SYMBOL(bcore_assert_set_default);
EXPORT_SYMBOL(bde);
EXPORT_SYMBOL(_build_date);
EXPORT_SYMBOL(_build_datestamp);
EXPORT_SYMBOL(_build_host);
EXPORT_SYMBOL(_build_release);
EXPORT_SYMBOL(_build_tree);
EXPORT_SYMBOL(_build_user);
EXPORT_SYMBOL(kconfig_get);
EXPORT_SYMBOL(kconfig_get_next);
EXPORT_SYMBOL(kconfig_set);
EXPORT_SYMBOL(phy_port_info);
#if defined(BCM_ESW_SUPPORT) || defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)
#ifdef INCLUDE_I2C
#include <soc/i2c.h>
EXPORT_SYMBOL(soc_i2c_attach);
EXPORT_SYMBOL(soc_i2c_clear_log);
EXPORT_SYMBOL(soc_i2c_device);
EXPORT_SYMBOL(soc_i2c_device_count);
EXPORT_SYMBOL(soc_i2c_devname);
EXPORT_SYMBOL(soc_i2c_is_attached);
EXPORT_SYMBOL(soc_i2c_lm75_monitor);
EXPORT_SYMBOL(soc_i2c_lm75_temperature_show);
EXPORT_SYMBOL(soc_i2c_lm63_monitor);
EXPORT_SYMBOL(soc_i2c_lm63_temperature_show);
EXPORT_SYMBOL(soc_i2c_max664x_monitor);
EXPORT_SYMBOL(soc_i2c_max664x_temperature_show);
EXPORT_SYMBOL(soc_i2c_max669x_monitor);
EXPORT_SYMBOL(soc_i2c_max669x_temperature_show);
extern int soc_i2c_max127_iterations;
EXPORT_SYMBOL(soc_i2c_max127_iterations);
EXPORT_SYMBOL(soc_i2c_probe);
EXPORT_SYMBOL(soc_i2c_read_byte);
EXPORT_SYMBOL(soc_i2c_read_byte_data);
EXPORT_SYMBOL(soc_i2c_read_word_data);
EXPORT_SYMBOL(soc_i2c_reset);
EXPORT_SYMBOL(soc_i2c_show);
EXPORT_SYMBOL(soc_i2c_show_log);
EXPORT_SYMBOL(soc_i2c_show_speeds);
EXPORT_SYMBOL(soc_i2c_write_byte);
EXPORT_SYMBOL(soc_i2c_write_byte_data);
EXPORT_SYMBOL(soc_i2c_write_word_data);
EXPORT_SYMBOL(soc_i2c_block_read);
EXPORT_SYMBOL(soc_i2c_block_write);
EXPORT_SYMBOL(soc_i2c_multi_write);
EXPORT_SYMBOL(soc_i2c_device_present);
EXPORT_SYMBOL(soc_i2c_saddr_to_string);
EXPORT_SYMBOL(soc_i2c_custom_device_add);
EXPORT_SYMBOL(soc_i2c_custom_device_remove);
EXPORT_SYMBOL(soc_i2c_dev_driver_get);
#endif /* INCLUDE_I2C */
#include <soc/phy/phyctrl.h>
#include <soc/phy/phymod_sim.h>
EXPORT_SYMBOL(int_phy_ctrl);
EXPORT_SYMBOL(ext_phy_ctrl);
EXPORT_SYMBOL(soc_phy_cfg_addr_get);
EXPORT_SYMBOL(soc_physim_read);
EXPORT_SYMBOL(soc_physim_wrmask);
#include <soc/soc_flash.h>
EXPORT_SYMBOL(soc_flash_init);
EXPORT_SYMBOL(soc_flash_read);
EXPORT_SYMBOL(soc_flash_write);
EXPORT_SYMBOL(soc_flash_erase);
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT) */
#if defined(BROADCOM_DEBUG) && defined(INCLUDE_BCM_SAL_PROFILE)
EXPORT_SYMBOL(sal_alloc_resource_usage_get);
EXPORT_SYMBOL(sal_mutex_resource_usage_get);
EXPORT_SYMBOL(sal_sem_resource_usage_get);
EXPORT_SYMBOL(sal_thread_resource_usage_get);
#endif /* defined(BROADCOM_DEBUG) && defined(INCLUDE_BCM_SAL_PROFILE) */
EXPORT_SYMBOL(sal_alloc);
EXPORT_SYMBOL(sal_assert_set);
EXPORT_SYMBOL(sal_boot_flags_get);
EXPORT_SYMBOL(sal_boot_flags_set);
EXPORT_SYMBOL(sal_boot_script);
EXPORT_SYMBOL(sal_core_init);
EXPORT_SYMBOL(sal_ctoi);
EXPORT_SYMBOL(sal_dpc_term);
EXPORT_SYMBOL(sal_dpc_cancel);
EXPORT_SYMBOL(sal_dpc_time);
EXPORT_SYMBOL(sal_dpc);
EXPORT_SYMBOL(sal_dpc_enable);
EXPORT_SYMBOL(sal_dpc_disable);
EXPORT_SYMBOL(sal_dpc_disable_and_wait);
EXPORT_SYMBOL(sal_rand);
EXPORT_SYMBOL(sal_srand);
EXPORT_SYMBOL(sal_itoa);
EXPORT_SYMBOL(increment_ip6addr);
EXPORT_SYMBOL(parse_ip6addr);
EXPORT_SYMBOL(parse_ipaddr);
#ifdef sal_free
/* Remove remap from <shared/alloc.h> */
#undef sal_free
#endif
EXPORT_SYMBOL(sal_free);
EXPORT_SYMBOL(sal_free_safe);
EXPORT_SYMBOL(sal_int_locked);
EXPORT_SYMBOL(sal_int_context);
EXPORT_SYMBOL(sal_no_sleep);
#if defined(memcpy)
EXPORT_SYMBOL(sal_memcpy_wrapper);
#endif
EXPORT_SYMBOL(sal_memcmp);
EXPORT_SYMBOL(sal_mutex_create);
EXPORT_SYMBOL(sal_mutex_destroy);
EXPORT_SYMBOL(sal_mutex_give);
EXPORT_SYMBOL(sal_mutex_take);
EXPORT_SYMBOL(sal_os_name);
EXPORT_SYMBOL(sal_printf);
EXPORT_SYMBOL(sal_sem_create);
EXPORT_SYMBOL(sal_sem_destroy);
EXPORT_SYMBOL(sal_sem_give);
EXPORT_SYMBOL(sal_sem_take);
EXPORT_SYMBOL(sal_spinlock_create);
EXPORT_SYMBOL(sal_spinlock_destroy);
EXPORT_SYMBOL(sal_spinlock_lock);
EXPORT_SYMBOL(sal_spinlock_unlock);
EXPORT_SYMBOL(sal_sleep);
EXPORT_SYMBOL(sal_snprintf);
EXPORT_SYMBOL(sal_vsnprintf);
EXPORT_SYMBOL(sal_vsprintf);
EXPORT_SYMBOL(sal_global_lock);
EXPORT_SYMBOL(sal_spl);
EXPORT_SYMBOL(sal_splhi);
EXPORT_SYMBOL(sal_sprintf);
EXPORT_SYMBOL(sal_strdup);
EXPORT_SYMBOL(sal_strtok_r);
EXPORT_SYMBOL(sal_thread_create);
EXPORT_SYMBOL(sal_thread_destroy);
EXPORT_SYMBOL(sal_thread_exit);
EXPORT_SYMBOL(sal_thread_id_get);
EXPORT_SYMBOL(sal_thread_main_get);
EXPORT_SYMBOL(sal_thread_main_set);
EXPORT_SYMBOL(sal_thread_name);
EXPORT_SYMBOL(sal_thread_self);
EXPORT_SYMBOL(sal_thread_yield);
EXPORT_SYMBOL(sal_time);
EXPORT_SYMBOL(sal_time_usecs);
EXPORT_SYMBOL(sal_udelay);
EXPORT_SYMBOL(sal_usleep);
EXPORT_SYMBOL(shr_avl_count);
EXPORT_SYMBOL(shr_avl_delete);
EXPORT_SYMBOL(shr_avl_delete_all);
EXPORT_SYMBOL(shr_avl_insert);
EXPORT_SYMBOL(shr_avl_lookup);
EXPORT_SYMBOL(shr_avl_traverse);
EXPORT_SYMBOL(_sal_assert);
EXPORT_SYMBOL(_shr_atof_exp10);
EXPORT_SYMBOL(shr_bitop_range_clear);
EXPORT_SYMBOL(_shr_crc32);
EXPORT_SYMBOL(_shr_ctoa);
EXPORT_SYMBOL(_shr_ctoi);
EXPORT_SYMBOL(_shr_div_exp10);
EXPORT_SYMBOL(_shr_errmsg);
EXPORT_SYMBOL(_shr_format_uint64_hexa_string);
#ifdef _SHR_DEFINE_PBMP_FUNCTIONS
EXPORT_SYMBOL(_shr_pbmp_bmnull);
EXPORT_SYMBOL(_shr_pbmp_bmeq);
#endif
EXPORT_SYMBOL(_shr_pbmp_decode);
EXPORT_SYMBOL(_shr_pbmp_format);
EXPORT_SYMBOL(_shr_popcount);
EXPORT_SYMBOL(_shr_swap32);
EXPORT_SYMBOL(_shr_swap16);
EXPORT_SYMBOL(_soc_mac_all_ones);
EXPORT_SYMBOL(_soc_mac_all_zeroes);
EXPORT_SYMBOL(soc_ntohs_load);
EXPORT_SYMBOL(soc_ntohl_load);
EXPORT_SYMBOL(soc_htons_store);
EXPORT_SYMBOL(soc_htonl_store);
EXPORT_SYMBOL(soc_letohl_load);
EXPORT_SYMBOL(soc_letohs_load);
EXPORT_SYMBOL(soc_htolel_store);
EXPORT_SYMBOL(soc_htoles_store);
EXPORT_SYMBOL(soc_property_get);
EXPORT_SYMBOL(soc_property_port_get_str);
EXPORT_SYMBOL(soc_property_suffix_num_only_suffix_str_get);
EXPORT_SYMBOL(soc_property_get_pbmp);
EXPORT_SYMBOL(soc_property_get_str);
EXPORT_SYMBOL(soc_property_get_csv);
EXPORT_SYMBOL(soc_property_port_get);
EXPORT_SYMBOL(soc_phy_addr_int_of_port);
EXPORT_SYMBOL(soc_phy_addr_of_port);
EXPORT_SYMBOL(soc_phy_an_timeout_get);
EXPORT_SYMBOL(soc_phy_id0reg_get);
EXPORT_SYMBOL(soc_phy_id1reg_get);
EXPORT_SYMBOL(soc_phy_is_c45_miim);
EXPORT_SYMBOL(soc_phy_name_get);
EXPORT_SYMBOL(soc_init);
EXPORT_SYMBOL(soc_reset_init);
EXPORT_SYMBOL(soc_cm_device);
EXPORT_SYMBOL(soc_cm_display_known_devices);
EXPORT_SYMBOL(soc_cm_get_bus_type);
EXPORT_SYMBOL(soc_cm_get_id);
EXPORT_SYMBOL(soc_cm_get_id_driver);
EXPORT_SYMBOL(soc_cm_get_device_name);
EXPORT_SYMBOL(soc_cm_p2l);
EXPORT_SYMBOL(soc_cm_l2p);
EXPORT_SYMBOL(soc_cm_sflush);
EXPORT_SYMBOL(soc_cm_sinval);
EXPORT_SYMBOL(soc_cm_salloc);
EXPORT_SYMBOL(soc_cm_sfree);
#ifdef BROADCOM_DEBUG
EXPORT_SYMBOL(soc_cm_dump_info);
#endif /* BROADCOM_DEBUG */
EXPORT_SYMBOL(soc_cm_iproc_read);
EXPORT_SYMBOL(soc_cm_iproc_write);
EXPORT_SYMBOL(soc_cm_device_create);
EXPORT_SYMBOL(soc_cm_device_create_id);
EXPORT_SYMBOL(soc_cm_device_destroy);
EXPORT_SYMBOL(soc_cm_device_supported);
EXPORT_SYMBOL(soc_custom_reg_above_64_set);
EXPORT_SYMBOL(soc_custom_reg_above_64_get);
EXPORT_SYMBOL(soc_dev_name);
EXPORT_SYMBOL(soc_block_names);
EXPORT_SYMBOL(soc_block_port_names);
EXPORT_SYMBOL(soc_block_name_lookup_ext);
EXPORT_SYMBOL(soc_misc_init);
EXPORT_SYMBOL(soc_feature_name);
#include <soc/iproc.h>
EXPORT_SYMBOL(soc_mem_config_set);
EXPORT_SYMBOL(soc_mem_array_read);
EXPORT_SYMBOL(soc_mem_array_read_flags);
EXPORT_SYMBOL(soc_mem_array_write);
EXPORT_SYMBOL(soc_mem_array_write_extended);
EXPORT_SYMBOL(soc_mem_datamask_rw_get);
EXPORT_SYMBOL(soc_mem_eccmask_get);
EXPORT_SYMBOL(soc_mem_tcammask_get);
EXPORT_SYMBOL(soc_mem_slamable);
EXPORT_SYMBOL(soc_regtypenames);
EXPORT_SYMBOL(soc_control);
EXPORT_SYMBOL(soc_state);
EXPORT_SYMBOL(soc_mem_field32_get);

EXPORT_SYMBOL(soc_format_field_get);
EXPORT_SYMBOL(soc_format_field_length);
EXPORT_SYMBOL(soc_format_field32_get);
#ifdef BCM_WARM_BOOT_SUPPORT
EXPORT_SYMBOL(soc_wb_mim_state);
#ifdef BCM_ESW_SUPPORT
EXPORT_SYMBOL(soc_shutdown);
#endif
EXPORT_SYMBOL(soc_scache_ptr_get);
EXPORT_SYMBOL(soc_scache_info_dump);
EXPORT_SYMBOL(soc_stable_set);
EXPORT_SYMBOL(soc_stable_size_get);
EXPORT_SYMBOL(soc_stable_size_set);
EXPORT_SYMBOL(soc_stable_used_get);
EXPORT_SYMBOL(soc_scache_handle_used_get);
EXPORT_SYMBOL(soc_scache_handle_used_set);
EXPORT_SYMBOL(soc_switch_stable_register);
EXPORT_SYMBOL(scache_max_partitions);
EXPORT_SYMBOL(soc_scache_dump_state);
#endif /* BCM_WARM_BOOT_SUPPORT */
EXPORT_SYMBOL(soc_miimc45_read);
EXPORT_SYMBOL(soc_miimc45_write);
EXPORT_SYMBOL(soc_chip_type_names);
EXPORT_SYMBOL(soc_all_ndev);
EXPORT_SYMBOL(soc_ndev);
EXPORT_SYMBOL(soc_ndev_idx2dev_map);
EXPORT_SYMBOL(soc_attached);
EXPORT_SYMBOL(soc_timeout_check);
EXPORT_SYMBOL(soc_timeout_init);
#if defined(BCM_ESW_SUPPORT) ||  defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <bcm_int/control.h>
EXPORT_SYMBOL(bcm_control);
#endif
EXPORT_SYMBOL(system_init);
EXPORT_SYMBOL(soc_miim_read);
EXPORT_SYMBOL(soc_miim_write);
EXPORT_SYMBOL(soc_mem_datamask_get);
EXPORT_SYMBOL(SOC_BLOCK_IN_LIST);
EXPORT_SYMBOL(SOC_BLOCK_IS_TYPE);
EXPORT_SYMBOL(SOC_BLOCK_IS_COMPOSITE);
EXPORT_SYMBOL(soc_port_ability_to_mode);
EXPORT_SYMBOL(soc_port_mode_to_ability);
EXPORT_SYMBOL(soc_port_phy_eyescan_extrapolate);
EXPORT_SYMBOL(soc_port_phy_eyescan_run);
EXPORT_SYMBOL(is_eyescan_algorithm_legacy_mode);
EXPORT_SYMBOL(is_eyescan_algorithm_legacy_rpt_mode);
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
EXPORT_SYMBOL(soc_rcpu_miimc45_read);
EXPORT_SYMBOL(soc_rcpu_miimc45_write);
EXPORT_SYMBOL(soc_rcpu_miim_read);
EXPORT_SYMBOL(soc_rcpu_miim_write);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
EXPORT_SYMBOL(_bcm_shutdown);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#if defined(BCM_TRX_SUPPORT)
EXPORT_SYMBOL(_bcm_common_sw_dump);
EXPORT_SYMBOL(_bcm_tr_ext_lpm_sw_dump);
#endif
#ifdef BCM_ESW_SUPPORT
EXPORT_SYMBOL(_bcm_esw_qos_sw_dump);
EXPORT_SYMBOL(_bcm_esw_fcoe_sw_dump);
EXPORT_SYMBOL(_bcm_ipfix_sw_dump);
EXPORT_SYMBOL(_bcm_mirror_sw_dump);
EXPORT_SYMBOL(_bcm_oam_sw_dump);
#if defined(INCLUDE_BFD)
EXPORT_SYMBOL(_bcm_bfd_sw_dump);
#endif /* INCLUDE_BFD */
EXPORT_SYMBOL(_bcm_stat_sw_dump);
EXPORT_SYMBOL(_bcm_stk_sw_dump);
EXPORT_SYMBOL(_bcm_time_sw_dump);
EXPORT_SYMBOL(_bcm_tr2_subport_sw_dump);
EXPORT_SYMBOL(_bcm_tr_subport_sw_dump);
EXPORT_SYMBOL(_bcm_esw_ecn_sw_dump);
#if defined(BCM_TRIUMPH3_SUPPORT)
EXPORT_SYMBOL(_bcm_l2gre_sw_dump);
#if defined(INCLUDE_REGEX)
#include <bcm_int/regex_api.h>
EXPORT_SYMBOL(_bcm_regex_sw_dump);
#endif /* INCLUDE_REGEX */
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(INCLUDE_PSTATS)
EXPORT_SYMBOL(_bcm_esw_pstats_sw_dump);
#endif /* INCLUDE_PSTATS */
#endif /* BCM_ESW_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifdef BCM_ESW_SUPPORT
#if defined(INCLUDE_BFD)
EXPORT_SYMBOL(_bcm_bfd_dump_trace);
#endif /* INCLUDE_BFD */
#ifndef BCM_SW_STATE_DUMP_DISABLE
#ifdef BCM_FIELD_SUPPORT
EXPORT_SYMBOL(_bcm_esw_field_show);
EXPORT_SYMBOL(_bcm_esw_field_range_show);
EXPORT_SYMBOL(_bcm_esw_field_udf_show);
#endif
EXPORT_SYMBOL(_bcm_cosq_sw_dump);
EXPORT_SYMBOL(_bcm_ipmc_sw_dump);
EXPORT_SYMBOL(_bcm_mcast_sw_dump);
EXPORT_SYMBOL(_bcm_multicast_sw_dump);
EXPORT_SYMBOL(_bcm_mpls_sw_dump);
EXPORT_SYMBOL(_bcm_l2_sw_dump);
EXPORT_SYMBOL(_bcm_l3_sw_dump);
EXPORT_SYMBOL(_bcm_vlan_sw_dump);
EXPORT_SYMBOL(_bcm_stg_sw_dump);
EXPORT_SYMBOL(_bcm_port_sw_dump);
EXPORT_SYMBOL(_bcm_esw_flexport_sw_dump);
EXPORT_SYMBOL(_bcm_esw_flexport_sw_pipe_speed_group_dump);
EXPORT_SYMBOL(_bcm_lock);
EXPORT_SYMBOL(_bcm_port_info_access);
EXPORT_SYMBOL(_bcm_trunk_sw_dump);
EXPORT_SYMBOL(_bcm_link_sw_dump);
EXPORT_SYMBOL(_bcm_niv_sw_dump);
EXPORT_SYMBOL(_bcm_tr2_failover_sw_dump);
EXPORT_SYMBOL(_bcm_td_trill_sw_dump);
#if defined(BCM_TRIDENT2_SUPPORT)
EXPORT_SYMBOL(_bcm_vxlan_sw_dump);
EXPORT_SYMBOL(_bcm_td2_cosq_hw_idx_get);
#endif /* BCM_TRIDENT2_SUPPORT */
EXPORT_SYMBOL(_bcm_extender_sw_dump);
#if defined(INCLUDE_FLOWTRACKER)
EXPORT_SYMBOL(_bcm_esw_flowtracker_sw_dump);
EXPORT_SYMBOL(_bcm_esw_flowtracker_dump_stats_learn);
EXPORT_SYMBOL(_bcm_esw_flowtracker_dump_stats_export);
EXPORT_SYMBOL(_bcm_esw_flowtracker_dump_stats_age);
EXPORT_SYMBOL(_bcm_esw_flowtracker_dump_stats_elephant);
#endif /* INCLUDE_FLOWTRACKER */
#if defined(BCM_COLLECTOR_SUPPORT)
EXPORT_SYMBOL(_bcm_esw_collector_sw_dump);
#endif /* BCM_COLLECTOR_SUPPORT */
#if defined(INCLUDE_TELEMETRY)
EXPORT_SYMBOL(_bcm_esw_telemetry_sw_dump);
#endif /* INCLUDE_TELEMETRY */
#ifdef BCM_FLOWTRACKER_SUPPORT
EXPORT_SYMBOL(bcma_ft_setid_template_id_get);
EXPORT_SYMBOL(bcma_ft_export_dma_collector_get);
EXPORT_SYMBOL(bcmi_ft_group_session_data_dump);
EXPORT_SYMBOL(bcmi_ft_table_lookup);
EXPORT_SYMBOL(bcmi_ft_init);
EXPORT_SYMBOL(bcmi_ft_alu32_dump);
EXPORT_SYMBOL(bcma_ft_export_fifo_control);
EXPORT_SYMBOL(bcmi_ft_table_insert);
EXPORT_SYMBOL(bcmi_ft_cleanup);
EXPORT_SYMBOL(bcmi_ft_group_state_dump);
EXPORT_SYMBOL(bcmi_ft_alu16_dump);
EXPORT_SYMBOL(bcmi_ft_alu_load_hash_dump);
EXPORT_SYMBOL(bcmi_esw_flowtracker_sw_dump);
#endif /* BCM_FLOWTRACKER_SUPPORT */
EXPORT_SYMBOL(_bcm_esw_udf_sw_dump);
EXPORT_SYMBOL(_bcm_esw_udf_sw_mem_dump);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#endif /* BCM_ESW_SUPPORT */

#if defined (BCM_ESW_SUPPORT) || defined (BCM_DPP_SUPPORT) || defined (BCM_DFE_SUPPORT)
#include <bcm_int/common/mbox.h>
#include <bcm_int/common/time-mbox.h>
#include <soc/mcm/driver.h>
#include <../src/soc/phy/phy8806x/include/phy8806x_syms.h>
#include <../src/soc/phy/phy8806x/include/phy8806x_funcs.h>
#include <../src/soc/phy/phy8806x/include/phy8806x_ctr.h>
#include <sal/appl/sal.h>
extern int phy_is_8806x(int unit, phy_ctrl_t *pc);
EXPORT_SYMBOL(format_uint64_decimal);

EXPORT_SYMBOL(_bcm_mbox_debug_flag_set);
EXPORT_SYMBOL(_bcm_mbox_debug_flag_get);
EXPORT_SYMBOL(_bcm_mbox_comm_init);
EXPORT_SYMBOL(_bcm_mbox_bs_wb_prep);
EXPORT_SYMBOL(_bcm_time_bs_status_get);
EXPORT_SYMBOL(_bcm_time_bs_log_configure);
EXPORT_SYMBOL(soc_base_driver_table);
EXPORT_SYMBOL(soc_cm_get_dev_type);
EXPORT_SYMBOL(soc_cm_pci_conf_read);
EXPORT_SYMBOL(soc_counter_get);
EXPORT_SYMBOL(soc_counter_get_rate);
EXPORT_SYMBOL(soc_counter_idx_get);
EXPORT_SYMBOL(soc_counter_set32_by_port);
EXPORT_SYMBOL(soc_counter_start);
EXPORT_SYMBOL(soc_counter_stop);
EXPORT_SYMBOL(soc_counter_sync);
EXPORT_SYMBOL(soc_counter_status);
EXPORT_SYMBOL(soc_event_register);
EXPORT_SYMBOL(soc_reg32_get);
EXPORT_SYMBOL(soc_reg32_set);
EXPORT_SYMBOL(soc_reg64_get);
EXPORT_SYMBOL(soc_reg64_set);
EXPORT_SYMBOL(soc_reg_addr_get);
EXPORT_SYMBOL(soc_mem_addr_get);
EXPORT_SYMBOL(soc_mem_entries);
EXPORT_SYMBOL(soc_mem_entry_dump);
EXPORT_SYMBOL(soc_mem_entry_dump_fields);
EXPORT_SYMBOL(soc_mem_entry_dump_if_changed);
EXPORT_SYMBOL(soc_mem_entry_dump_if_changed_fields);
EXPORT_SYMBOL(soc_mem_field_get);
EXPORT_SYMBOL(soc_mem_field_set);
EXPORT_SYMBOL(soc_mem_field_valid);
EXPORT_SYMBOL(soc_mem_iterate);
EXPORT_SYMBOL(soc_mem_read);
EXPORT_SYMBOL(soc_mem_read_physical_index);
EXPORT_SYMBOL(soc_mem_is_shared_mem);
EXPORT_SYMBOL(soc_persist);
EXPORT_SYMBOL(soc_phy_list_get);
EXPORT_SYMBOL(soc_phyctrl_diag_ctrl);
EXPORT_SYMBOL(soc_phyctrl_offset_get);
EXPORT_SYMBOL(soc_port_cmap_get);
EXPORT_SYMBOL(soc_reg64_field32_get);
EXPORT_SYMBOL(soc_reg64_field32_set);
EXPORT_SYMBOL(soc_reg64_field_set);
EXPORT_SYMBOL(soc_reg64_field_get);
EXPORT_SYMBOL(soc_reg_above_64_field_get);
EXPORT_SYMBOL(soc_reg_above_64_field32_set);
EXPORT_SYMBOL(soc_reg_above_64_field32_get);
EXPORT_SYMBOL(soc_reg_above_64_field_set);
EXPORT_SYMBOL(soc_reg_above_64_get);
EXPORT_SYMBOL(soc_reg_above_64_set);
EXPORT_SYMBOL(soc_reg_addr);
EXPORT_SYMBOL(soc_reg_field_get);
EXPORT_SYMBOL(soc_reg_field_set);
EXPORT_SYMBOL(soc_reg_get);
EXPORT_SYMBOL(soc_reg_set);
EXPORT_SYMBOL(soc_reg_sprint_addr);
EXPORT_SYMBOL(soc_sbus_mdio_read);
EXPORT_SYMBOL(soc_sbus_mdio_write);
EXPORT_SYMBOL(soc_sbusdma_desc_create);
EXPORT_SYMBOL(soc_sbusdma_desc_run);
#ifdef BCM_TRIDENT3_SUPPORT
EXPORT_SYMBOL(soc_mem_view_index_min);
EXPORT_SYMBOL(soc_mem_view_index_max);
EXPORT_SYMBOL(soc_mem_view_index_count);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
EXPORT_SYMBOL(bcmi_esw_flow_sw_dump);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
EXPORT_SYMBOL(soc_flow_db_legacy_flow_map_get);
EXPORT_SYMBOL(soc_flow_db_mem_view_field_info_get);
EXPORT_SYMBOL(soc_flow_db_flow_map);
EXPORT_SYMBOL(soc_flow_db_mem_view_info_get);
EXPORT_SYMBOL(soc_flow_db_ffo_to_mem_view_id_get);
EXPORT_SYMBOL(soc_flow_db_mem_view_to_ffo_get);
EXPORT_SYMBOL(soc_flow_db_mem_view_mem_name_get);
EXPORT_SYMBOL(soc_flow_db_mem_view_field_name_get);
EXPORT_SYMBOL(soc_flow_db_mem_view_option_name_get);
EXPORT_SYMBOL(soc_flow_db_flow_handle_get);
EXPORT_SYMBOL(soc_flow_db_mem_view_field_list_get);
EXPORT_SYMBOL(soc_flow_db_mem_to_view_id_get);
EXPORT_SYMBOL(soc_flow_db_status_get);
#endif
EXPORT_SYMBOL(mt2_axi_read);
EXPORT_SYMBOL(mt2_field32_set);
EXPORT_SYMBOL(mt2_field_get);
EXPORT_SYMBOL(mt2_sbus_mem_read);
EXPORT_SYMBOL(mt2_sbus_mem_write);
EXPORT_SYMBOL(mt2_sbus_reg_read);
EXPORT_SYMBOL(mt2_sbus_reg_write);
EXPORT_SYMBOL(mt2_sbus_to_tsc_read);
EXPORT_SYMBOL(mt2_sbus_to_tsc_write);
EXPORT_SYMBOL(mt2_syms_find_name);
EXPORT_SYMBOL(mt2_syms_find_next_name);
EXPORT_SYMBOL(mt2_test_run);
EXPORT_SYMBOL(mt2_diag_run);
EXPORT_SYMBOL(phy8806x_ctr_interval_set);
EXPORT_SYMBOL(phy8806x_ctr_start);
EXPORT_SYMBOL(phy8806x_ctr_stop);
EXPORT_SYMBOL(phy8806x_ctr_show);
EXPORT_SYMBOL(phy_is_8806x);
EXPORT_SYMBOL(phy8806x_fields);
EXPORT_SYMBOL(sal_strncasecmp);
EXPORT_SYMBOL(sal_strcasecmp);
EXPORT_SYMBOL(sal_strcmp);
EXPORT_SYMBOL(sal_strcasestr);

#ifdef BCM_ESW_SUPPORT
EXPORT_SYMBOL(soc_format_entry_dump);
#endif /* BCM_ESW_SUPPORT */

#if !defined(SOC_NO_NAMES)
EXPORT_SYMBOL(soc_fieldnames);
EXPORT_SYMBOL(soc_mem_name);
EXPORT_SYMBOL(soc_format_name);
EXPORT_SYMBOL(soc_mem_ufalias);
EXPORT_SYMBOL(soc_mem_ufname);
EXPORT_SYMBOL(soc_reg_name);
EXPORT_SYMBOL(soc_regaddrlist_alloc);
EXPORT_SYMBOL(soc_regaddrlist_free);
#endif /* !defined(SOC_NO_NAMES) */
#if !defined(SOC_NO_DESC)
EXPORT_SYMBOL(soc_mem_desc);
EXPORT_SYMBOL(soc_reg_desc);
#endif /* !defined(SOC_NO_DESC) */
#if !defined(SOC_NO_ALIAS)
EXPORT_SYMBOL(soc_reg_alias);
#endif /* !defined(SOC_NO_ALIAS) */
#ifdef BCM_RPC_SUPPORT
extern int _rlink_nexthop;
extern int _rpc_nexthop;
extern int  bcm_rlink_start(void);
extern int  bcm_rlink_stop(void);
extern int  bcm_rpc_start(void);
extern int  bcm_rpc_stop(void);
extern void bcm_rpc_dump(void);
EXPORT_SYMBOL(_rlink_nexthop);
EXPORT_SYMBOL(_rpc_nexthop);
EXPORT_SYMBOL(bcm_rlink_stop);
EXPORT_SYMBOL(bcm_rlink_start);
EXPORT_SYMBOL(bcm_rpc_dump);
EXPORT_SYMBOL(bcm_rpc_start);
EXPORT_SYMBOL(bcm_rpc_stop);
EXPORT_SYMBOL(cpudb_key_format);
EXPORT_SYMBOL(cpudb_key_parse);
#endif /* BCM_RPC_SUPPORT */
#endif /* defined (BCM_ESW_SUPPORT) || defined (BCM_DPP_SUPPORT) || defined (BCM_DFE_SUPPORT)*/
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DPP_SUPPORT)
EXPORT_SYMBOL(soc_dma_abort_dv);
EXPORT_SYMBOL(soc_dma_chan_config);
EXPORT_SYMBOL(soc_dma_desc_add);
EXPORT_SYMBOL(soc_dma_desc_end_packet);
EXPORT_SYMBOL(soc_dma_dump_dv);
EXPORT_SYMBOL(soc_dma_dump_pkt);
EXPORT_SYMBOL(soc_dma_dv_alloc);
EXPORT_SYMBOL(soc_dma_dv_free);
EXPORT_SYMBOL(soc_dma_dv_reset);
EXPORT_SYMBOL(soc_dma_ether_dump);
EXPORT_SYMBOL(soc_dma_ep_to_cpu_hdr_dump);
EXPORT_SYMBOL(soc_dma_ep_to_cpu_hdr_decoded_dump);
EXPORT_SYMBOL(soc_dma_usage_get);
EXPORT_SYMBOL(soc_dma_header_size_get);
EXPORT_SYMBOL(soc_dma_max_rx_channels_get);
EXPORT_SYMBOL(soc_dma_init);
EXPORT_SYMBOL(soc_dma_start);
EXPORT_SYMBOL(soc_pci_off2name);
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/uc.h>
EXPORT_SYMBOL(soc_uc_firmware_version);
EXPORT_SYMBOL(soc_uc_in_reset);
EXPORT_SYMBOL(soc_uc_mem_read);
EXPORT_SYMBOL(soc_uc_firmware_thread_info);
EXPORT_SYMBOL(soc_uc_stats_reset);
EXPORT_SYMBOL(soc_uc_console_log);
EXPORT_SYMBOL(soc_uc_ping);
#endif
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/uc_msg.h>
EXPORT_SYMBOL(soc_cmic_uc_msg_start);
EXPORT_SYMBOL(soc_cmic_uc_msg_stop);
EXPORT_SYMBOL(soc_cmic_uc_msg_uc_start);
EXPORT_SYMBOL(soc_cmic_uc_msg_uc_stop);
EXPORT_SYMBOL(soc_cmic_uc_msg_timestamp_get);
EXPORT_SYMBOL(soc_cmic_uc_msg_timestamp_enable);
EXPORT_SYMBOL(soc_cmic_uc_msg_timestamp_disable);
#if defined(BCM_CMICM_SUPPORT)
EXPORT_SYMBOL(soc_cmicm_reg_get);
#endif
EXPORT_SYMBOL(soc_pci_mcs_getreg);
EXPORT_SYMBOL(soc_pci_mcs_read);
EXPORT_SYMBOL(soc_pci_mcs_write);
#endif /* BCM_CMICM_SUPPORT */
#if defined(BCM_CMICX_SUPPORT)
#include <soc/iproc.h>
#include <shared/cmicfw/cmicx_led.h>
#include <shared/cmicfw/cmicx_link.h>
#include <shared/cmicfw/iproc_m0ssq.h>
EXPORT_SYMBOL(soc_cmicx_led_link_status);
EXPORT_SYMBOL(soc_cmicx_led_status_get);
EXPORT_SYMBOL(soc_cmicx_led_status_set);
EXPORT_SYMBOL(soc_cmicx_led_enable);
EXPORT_SYMBOL(soc_cmicx_led_speed);
EXPORT_SYMBOL(soc_cmicx_linkscan_heartbeat);
EXPORT_SYMBOL(soc_cmicx_reg_offset_get);
EXPORT_SYMBOL(soc_iproc_getreg);
EXPORT_SYMBOL(soc_iproc_setreg);
EXPORT_SYMBOL(soc_iproc_m0ssq_init);
EXPORT_SYMBOL(soc_iproc_m0ssq_exit);
#endif /* BCM_CMICX_SUPPORT */
#ifdef BCM_DDR3_SUPPORT
#include <soc/shmoo_ddr40.h>
EXPORT_SYMBOL(soc_ddr40_shmoo_ctl);
EXPORT_SYMBOL(soc_ddr40_shmoo_restorecfg);
EXPORT_SYMBOL(soc_ddr40_shmoo_savecfg);
#endif /* BCM_DDR3_SUPPORT */
#endif /* defined (BCM_ESW_SUPPORT) || defined (BCM_DPP_SUPPORT) */
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx_qspi.h>
EXPORT_SYMBOL(iproc_qspi_set_access_method);
#endif /* BCM_CMICX_SUPPORT */

#ifdef INCLUDE_AVS
#include <soc/avs.h>
EXPORT_SYMBOL(_soc_avs_predict_vpred);
EXPORT_SYMBOL(soc_avs_custom_margin_get);
EXPORT_SYMBOL(soc_avs_custom_margin_set);
EXPORT_SYMBOL(soc_avs_info_dump);
EXPORT_SYMBOL(soc_avs_init);
EXPORT_SYMBOL(soc_avs_osc_count_dump);
EXPORT_SYMBOL(soc_avs_voltage_get);
EXPORT_SYMBOL(soc_avs_voltage_set);
EXPORT_SYMBOL(soc_avs_start);
EXPORT_SYMBOL(soc_avs_track);
EXPORT_SYMBOL(soc_avs_track_start);
EXPORT_SYMBOL(soc_avs_track_stop);
EXPORT_SYMBOL(soc_avs_xbmp_dump);
EXPORT_SYMBOL(soc_avs_xbmp_set);
#endif /* INCLUDE_AVS*/

/* 
 * Export system-related symbols and symbols
 * required by the diag shell .
 * ESW specific
 */

#ifdef BCM_ESW_SUPPORT
#ifdef INCLUDE_PTP
extern STATIC const char* diag_port_state_description(_bcm_ptp_port_state_t state);
EXPORT_SYMBOL(diag_port_state_description);
EXPORT_SYMBOL(_bcm_common_ptp_info);
EXPORT_SYMBOL(_bcm_common_ptp_input_channels_status_get);
EXPORT_SYMBOL(_bcm_common_ptp_unit_array);
EXPORT_SYMBOL(_bcm_ptp_set_max_acceptable_masters);
EXPORT_SYMBOL(_bcm_ptp_set_max_multicast_slave_stats);
EXPORT_SYMBOL(_bcm_ptp_set_max_unicast_masters);
EXPORT_SYMBOL(_bcm_ptp_set_max_unicast_slaves);
EXPORT_SYMBOL(_bcm_ptp_running);
EXPORT_SYMBOL(_bcm_ptp_llu_div);
EXPORT_SYMBOL(_bcm_ptp_function_precheck);
EXPORT_SYMBOL(_bcm_ptp_management_message_send);
#endif
#ifdef BCM_KATANA_SUPPORT
EXPORT_SYMBOL(bcm_esw_stat_group_dump_all);
EXPORT_SYMBOL(bcm_esw_stat_group_dump);
#ifdef INCLUDE_L3
EXPORT_SYMBOL(_bcm_kt_l3_info_dump);
EXPORT_SYMBOL(bcm_kt_dump_port_lls);
#endif /* INCLUDE_L3 */
#endif
#ifdef INCLUDE_L3
EXPORT_SYMBOL(_bcm_l3_bk_info);
EXPORT_SYMBOL(_bcm_esw_l3_nat_egress_reference_get);
#ifdef BCM_RIOT_SUPPORT
EXPORT_SYMBOL(_bcm_vp_is_vfi_type);
#endif
EXPORT_SYMBOL(_bcm_esw_l3_egress_ecmp_get);
EXPORT_SYMBOL(l3_module_data);
#endif /* INCLUDE_L3 */

EXPORT_SYMBOL(_bcm_fb_l2_from_l2x);

EXPORT_SYMBOL(_bcm_esw_gport_resolve);
EXPORT_SYMBOL(_bcm_esw_gport_construct);
EXPORT_SYMBOL(_bcm_esw_port_gport_validate);
EXPORT_SYMBOL(_bcm_esw_modid_is_local);
EXPORT_SYMBOL(_bcm_esw_l2_gport_parse);
EXPORT_SYMBOL(_bcm_esw_l3_egress_reference_get);
EXPORT_SYMBOL(_bcm_gport_modport_hw2api_map);
EXPORT_SYMBOL(bcm_esw_port_control_get);
EXPORT_SYMBOL(_bcm_esw_l3_egress_ref_dlb_update);

#ifdef BCM_FIELD_SUPPORT
#ifdef BROADCOM_DEBUG
EXPORT_SYMBOL(_field_qset_dump);
EXPORT_SYMBOL(_field_aset_dump);
EXPORT_SYMBOL(_bcm_field_presel_dump);
EXPORT_SYMBOL(_field_dump_brief);
EXPORT_SYMBOL(_field_dump_class);
EXPORT_SYMBOL(_field_calc_cache_size);
EXPORT_SYMBOL(_field_group_dump_brief);
#endif /* BROADCOM_DEBUG */
EXPORT_SYMBOL(_field_control_get);
EXPORT_SYMBOL(_field_stage_control_get);
EXPORT_SYMBOL(_field_qset_is_subset);
#endif  /* BCM_FIELD_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
EXPORT_SYMBOL(_bcm_tr_l2_from_l2x);
#endif /*BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */
EXPORT_SYMBOL(bcm_ipfix_dump_export_entry);
EXPORT_SYMBOL(bcm_ipfix_export_fifo_control);
#ifdef  BCM_XGS_SWITCH_SUPPORT
EXPORT_SYMBOL(_soc_mem_cmp_l3x_sync);
#endif  /* BCM_XGS_SWITCH_SUPPORT */
EXPORT_SYMBOL(mbcm_driver);
EXPORT_SYMBOL(soc_anyreg_read);
EXPORT_SYMBOL(soc_anyreg_write);
EXPORT_SYMBOL(soc_bist);
EXPORT_SYMBOL(soc_chip_dump);
#ifdef BCM_DDR3_SUPPORT
EXPORT_SYMBOL(soc_ddr40_phy_reg_ci_modify);
EXPORT_SYMBOL(soc_ddr40_phy_reg_ci_read);
EXPORT_SYMBOL(soc_ddr40_phy_reg_ci_write);
EXPORT_SYMBOL(soc_ddr40_read);
EXPORT_SYMBOL(soc_ddr40_write);
EXPORT_SYMBOL(soc_ddr40_set_shmoo_dram_config);
EXPORT_SYMBOL(soc_ddr40_phy_pll_ctl);
EXPORT_SYMBOL(soc_ddr40_ctlr_ctl);
EXPORT_SYMBOL(soc_ddr40_phy_calibrate);
#endif
EXPORT_SYMBOL(soc_dma_higig_dump);
EXPORT_SYMBOL(soc_dma_rom_dcb_alloc);
EXPORT_SYMBOL(soc_dma_rom_dcb_free);
EXPORT_SYMBOL(soc_dma_rom_detach);
EXPORT_SYMBOL(soc_dma_rom_init);
EXPORT_SYMBOL(soc_dma_rom_rx_poll);
EXPORT_SYMBOL(soc_dma_rom_tx_poll);
EXPORT_SYMBOL(soc_dma_rom_tx_start);
EXPORT_SYMBOL(soc_dma_wait);
EXPORT_SYMBOL(soc_draco_l2x_param_to_key);
#ifdef BROADCOM_DEBUG
EXPORT_SYMBOL(soc_dump);
#endif /* BROADCOM_DEBUG */
#ifdef  BCM_FIREBOLT_SUPPORT
EXPORT_SYMBOL(soc_fb_l2_hash);
EXPORT_SYMBOL(soc_fb_l2x_bank_insert);
EXPORT_SYMBOL(soc_fb_l2x_bank_delete);
EXPORT_SYMBOL(soc_fb_l2x_bank_lookup);
EXPORT_SYMBOL(soc_fb_l3x_bank_delete);
EXPORT_SYMBOL(soc_fb_l3x_bank_insert);
EXPORT_SYMBOL(soc_fb_l3x_bank_lookup);
EXPORT_SYMBOL(soc_fb_l3x_base_entry_to_key);
#endif  /* BCM_FIREBOLT_SUPPORT */
EXPORT_SYMBOL(soc_fb_l3_hash);
#ifdef CANCUN_SUPPORT
EXPORT_SYMBOL(soc_cancun_cch_list);
EXPORT_SYMBOL(soc_cancun_cmh_list);
EXPORT_SYMBOL(soc_cancun_file_load);
EXPORT_SYMBOL(soc_cancun_status_get);
EXPORT_SYMBOL(soc_cancun_file_info_get);
EXPORT_SYMBOL(soc_cancun_cmh_mem_set);
EXPORT_SYMBOL(soc_cancun_cmh_check);
EXPORT_SYMBOL(soc_cancun_app_dest_entry_get);
EXPORT_SYMBOL(soc_cancun_app_dest_entry_set);
EXPORT_SYMBOL(soc_cancun_info_get);
EXPORT_SYMBOL(soc_cancun_access_scan);
EXPORT_SYMBOL(soc_cancun_cch_app_set);
EXPORT_SYMBOL(soc_cancun_branch_id_get);
EXPORT_SYMBOL(soc_cancun_udf_abstr_type_info_get);
#endif /* CANCUN_SUPPORT */
#ifdef BCM_HERCULES_SUPPORT
EXPORT_SYMBOL(soc_hercules15_mmu_limits_config);
EXPORT_SYMBOL(soc_hercules_mem_read_word);
EXPORT_SYMBOL(soc_hercules_mem_write_word);
#endif  /* BCM_HERCULES_SUPPORT */
#ifdef BCM_XGS_SUPPORT
EXPORT_SYMBOL(soc_higig_dump);
#ifdef BCM_HIGIG2_SUPPORT
EXPORT_SYMBOL(soc_higig2_dump);
EXPORT_SYMBOL(soc_higig2_field_set);
#endif /* BCM_HIGIG2_SUPPORT */
EXPORT_SYMBOL(soc_higig_field_set);
#endif /* BCM_XGS_SUPPORT */
#ifdef INCLUDE_I2C
EXPORT_SYMBOL(phy_i2c_miireg_read);
EXPORT_SYMBOL(phy_i2c_miireg_write);
#endif /* INCLUDE_I2C */
EXPORT_SYMBOL(soc_detach);
EXPORT_SYMBOL(soc_intr_disable);
EXPORT_SYMBOL(soc_intr_enable);
#ifdef BCM_XGS_SWITCH_SUPPORT
EXPORT_SYMBOL(soc_l2x_entry_compare_key);
EXPORT_SYMBOL(soc_l2x_entry_dump);
#ifdef BCM_TRIUMPH3_SUPPORT
EXPORT_SYMBOL(soc_tr3_l2_entry_dump);
EXPORT_SYMBOL(soc_tr3_ext_l2_1_entry_dump);
EXPORT_SYMBOL(soc_tr3_ext_l2_2_entry_dump);
EXPORT_SYMBOL(_l2_addr_counts);
#endif
EXPORT_SYMBOL(soc_l2x_start);
EXPORT_SYMBOL(soc_l2x_stop);
EXPORT_SYMBOL(soc_l2x_running);
#endif  /* BCM_XGS_SWITCH_SUPPORT */
EXPORT_SYMBOL(_soc_mem_entry_null_zeroes);
EXPORT_SYMBOL(soc_mem_addr);
EXPORT_SYMBOL(soc_mem_cache_get);
EXPORT_SYMBOL(soc_mem_cache_set);
EXPORT_SYMBOL(soc_mem_cfap_init);
EXPORT_SYMBOL(soc_mem_clear);
EXPORT_SYMBOL(soc_mem_debug_get);
EXPORT_SYMBOL(soc_mem_debug_set);
EXPORT_SYMBOL(soc_mem_delete);
EXPORT_SYMBOL(soc_mem_delete_index);
EXPORT_SYMBOL(soc_mem_dmaable);
EXPORT_SYMBOL(soc_mem_entry_dump_vertical);
EXPORT_SYMBOL(soc_mem_field32_set);
EXPORT_SYMBOL(soc_mem_field_length);
EXPORT_SYMBOL(soc_mem_generic_delete);
EXPORT_SYMBOL(soc_mem_generic_insert);
EXPORT_SYMBOL(soc_mem_generic_lookup);
EXPORT_SYMBOL(soc_mem_index_last);
EXPORT_SYMBOL(soc_mem_insert);
EXPORT_SYMBOL(soc_mem_bank_insert);
EXPORT_SYMBOL(soc_mem_ip6_addr_set);
EXPORT_SYMBOL(soc_mem_mac_addr_set);
EXPORT_SYMBOL(soc_mem_pbmp_field_get);
EXPORT_SYMBOL(soc_mem_pbmp_field_set);
EXPORT_SYMBOL(soc_mem_field64_get);
EXPORT_SYMBOL(soc_mem_field64_set);
EXPORT_SYMBOL(soc_mem_pipe_select_read);
EXPORT_SYMBOL(soc_mem_pipe_select_write);
EXPORT_SYMBOL(soc_mem_pop);
EXPORT_SYMBOL(soc_mem_push);
EXPORT_SYMBOL(soc_mem_read_range);
EXPORT_SYMBOL(soc_mem_read_no_cache);
#ifdef INCLUDE_MEM_SCAN
EXPORT_SYMBOL(soc_mem_scan_running);
EXPORT_SYMBOL(soc_mem_scan_start);
EXPORT_SYMBOL(soc_mem_scan_stop);
#endif /* INCLUDE_MEM_SCAN */
#ifdef BCM_SRAM_SCAN_SUPPORT
EXPORT_SYMBOL(soc_sram_scan_running);
EXPORT_SYMBOL(soc_sram_scan_start);
EXPORT_SYMBOL(soc_sram_scan_stop);
#endif /* BCM_SRAM_SCAN_SUPPORT */
EXPORT_SYMBOL(soc_mem_search);
EXPORT_SYMBOL(soc_event_unregister);
EXPORT_SYMBOL(soc_event_generate);
EXPORT_SYMBOL(soc_mem_snoop_register);
EXPORT_SYMBOL(soc_mem_snoop_unregister);
EXPORT_SYMBOL(soc_mem_write);
EXPORT_SYMBOL(soc_mem_write_extended);
EXPORT_SYMBOL(soc_mem_write_range);
EXPORT_SYMBOL(soc_mem_watch_set);
EXPORT_SYMBOL(soc_mmu_init);
#ifdef BCM_CMICM_SUPPORT
EXPORT_SYMBOL(soc_mspi_init);
EXPORT_SYMBOL(soc_mspi_config);
EXPORT_SYMBOL(soc_mspi_writeread8);
EXPORT_SYMBOL(soc_mspi_read8);
EXPORT_SYMBOL(soc_mspi_write8);
#endif
#ifdef BCM_CMICX_SUPPORT
EXPORT_SYMBOL(soc_parse_cmicx_regname);
#endif
#ifdef  SOC_PCI_DEBUG
EXPORT_SYMBOL(soc_pci_read);
EXPORT_SYMBOL(soc_pci_write);
#endif  /* SOC_PCI_DEBUG */
EXPORT_SYMBOL(soc_reg32_read);
EXPORT_SYMBOL(soc_reg32_write);
EXPORT_SYMBOL(soc_reg64_read);
EXPORT_SYMBOL(soc_reg_watch_set);
EXPORT_SYMBOL(_soc_max_blocks_per_entry_get);
EXPORT_SYMBOL(soc_reg_field_valid);
EXPORT_SYMBOL(soc_reg_iterate);
EXPORT_SYMBOL(soc_reg_port_idx_valid);
EXPORT_SYMBOL(soc_reg_snoop_register);
EXPORT_SYMBOL(soc_reg_snoop_unregister);
EXPORT_SYMBOL(soc_regaddrinfo_get);
EXPORT_SYMBOL(soc_regaddrinfo_extended_get);
EXPORT_SYMBOL(soc_reset);
EXPORT_SYMBOL(soc_schan_op);
EXPORT_SYMBOL(soc_ser_inject_error);
EXPORT_SYMBOL(soc_ser_log_print_all);
EXPORT_SYMBOL(soc_ser_log_print_one);
EXPORT_SYMBOL(soc_ser_inject_support);
EXPORT_SYMBOL(soc_tcam_init);
EXPORT_SYMBOL(soc_tcam_get_info);
EXPORT_SYMBOL(soc_tcam_mem_index_to_raw_index);
EXPORT_SYMBOL(soc_tcam_part_index_to_mem_index);
EXPORT_SYMBOL(soc_tcam_read_dbreg);
EXPORT_SYMBOL(soc_tcam_read_entry);
EXPORT_SYMBOL(soc_tcam_read_ima);
EXPORT_SYMBOL(soc_tcam_search_entry);
EXPORT_SYMBOL(soc_tcam_set_valid);
EXPORT_SYMBOL(soc_tcam_write_dbreg);
EXPORT_SYMBOL(soc_tcam_write_entry);
EXPORT_SYMBOL(soc_tcam_write_ib);
EXPORT_SYMBOL(soc_tcam_write_ima);
#ifdef BCM_TRX_SUPPORT
EXPORT_SYMBOL(soc_tr_l2x_base_entry_to_key);
EXPORT_SYMBOL(soc_tr_l2x_hash);
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/hash.h>
EXPORT_SYMBOL(soc_td2x_th_l2x_hash);
EXPORT_SYMBOL(soc_td2x_th_l3x_hash);
#endif
#ifdef BCM_TRIUMPH_SUPPORT
EXPORT_SYMBOL(soc_triumph_ext_sram_enable_set);
EXPORT_SYMBOL(soc_triumph_ext_sram_bist_setup);
EXPORT_SYMBOL(soc_triumph_ext_sram_op);
EXPORT_SYMBOL(soc_triumph_tcam_search_bist);
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
EXPORT_SYMBOL(soc_trident_mmu_config_init);
EXPORT_SYMBOL(soc_trident_show_material_process);
EXPORT_SYMBOL(soc_trident_show_ring_osc);
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef ALPM_ENABLE
EXPORT_SYMBOL(soc_alpm_debug_show);
EXPORT_SYMBOL(soc_alpm_debug_brief_show);
EXPORT_SYMBOL(soc_alpm_sanity_check);
EXPORT_SYMBOL(soc_alpm_cmn_banks_get);
EXPORT_SYMBOL(soc_th_alpm_debug_show);
EXPORT_SYMBOL(soc_th_alpm_debug_brief_show);
EXPORT_SYMBOL(soc_th_alpm_sanity_check);
EXPORT_SYMBOL(soc_alpm_cmn_bkt_view_get);
EXPORT_SYMBOL(soc_alpm_cmn_bkt_view_set);
EXPORT_SYMBOL(soc_alpm_l3_defip_index_map);
EXPORT_SYMBOL(soc_alpm_l3_defip_index_remap);
#ifdef BCM_TRIDENT3_SUPPORT
EXPORT_SYMBOL(_soc_trident3_alpm2_bnk_fmt_set);
EXPORT_SYMBOL(_soc_trident3_alpm2_bnk_fmt_get);
EXPORT_SYMBOL(soc_trident3_show_ring_osc);
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
EXPORT_SYMBOL(th_alpm_table_ent_dump);
EXPORT_SYMBOL(alpm_cb_bkt_dump);
EXPORT_SYMBOL(alpm_cb_conf_dump);
EXPORT_SYMBOL(alpm_cb_fmt_dump);
EXPORT_SYMBOL(alpm_cb_pvt_dump);
EXPORT_SYMBOL(alpm_cb_pvtlen_dump);
EXPORT_SYMBOL(alpm_cb_stat_dump);
EXPORT_SYMBOL(alpm_dbg_info_bmp_set);
EXPORT_SYMBOL(alpm_dbg_info_bmp_show);
EXPORT_SYMBOL(alpm_trace_dump);
EXPORT_SYMBOL(alpm_trace_set);
EXPORT_SYMBOL(alpm_trace_wrap_set);
EXPORT_SYMBOL(alpm_trace_clear);
EXPORT_SYMBOL(alpm_trace_sanity);
EXPORT_SYMBOL(alpm_util_fmt_ip6addr);
EXPORT_SYMBOL(alpm_util_fmt_ipaddr);
EXPORT_SYMBOL(alpm_dist_hitbit_enable_set);
EXPORT_SYMBOL(alpm_dist_hitbit_interval_get);
EXPORT_SYMBOL(alpm_table_dump);
EXPORT_SYMBOL(bcm_esw_alpm_tcam_zone_state_dump);
EXPORT_SYMBOL(bcm_esw_alpm_debug_brief_show);
EXPORT_SYMBOL(bcm_esw_alpm_debug_show);
EXPORT_SYMBOL(bcm_esw_alpm_init);
EXPORT_SYMBOL(bcm_esw_alpm_insert);
EXPORT_SYMBOL(bcm_esw_alpm_delete);
EXPORT_SYMBOL(bcm_esw_alpm_update);
EXPORT_SYMBOL(bcm_esw_alpm_lookup);
EXPORT_SYMBOL(bcm_esw_alpm_sanity_check);
#endif /* BCM_TOMAHAWK_SUPPORT */
#endif /* ALPM_ENABLE */
#ifdef FB_LPM_DEBUG
EXPORT_SYMBOL(lpm128_state_verify);
#endif /* FB_LPM_DEBUG */
#ifdef BCM_TRIDENT2_SUPPORT
EXPORT_SYMBOL(soc_trident2_tsc_map_get);
EXPORT_SYMBOL(soc_td2_dump_port_lls);
EXPORT_SYMBOL(soc_td2_dump_port_hsp);
EXPORT_SYMBOL(soc_td2_logical_qnum_hw_qnum);
EXPORT_SYMBOL(_soc_trident2_alpm_bkt_view_get);
EXPORT_SYMBOL(_soc_trident2_alpm_bkt_view_set);
EXPORT_SYMBOL(soc_trident2_show_material_process);
EXPORT_SYMBOL(soc_trident2_show_ring_osc);

#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
EXPORT_SYMBOL(soc_td2p_show_voltage);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
EXPORT_SYMBOL(soc_flexport_sw_dump);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
EXPORT_SYMBOL(soc_apache_logical_qnum_hw_qnum);
EXPORT_SYMBOL(soc_apache_dump_port_hsp);
EXPORT_SYMBOL(soc_apache_dump_port_lls);
EXPORT_SYMBOL(bcm_apache_dump_port_lls_sw);
EXPORT_SYMBOL(soc_apache_show_voltage);
EXPORT_SYMBOL(soc_apache_show_ring_osc);
EXPORT_SYMBOL(soc_apache_show_material_process);
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
EXPORT_SYMBOL(soc_monterey_logical_qnum_hw_qnum);
EXPORT_SYMBOL(soc_monterey_dump_port_lls);
EXPORT_SYMBOL(bcm_monterey_dump_port_lls_sw);
EXPORT_SYMBOL(soc_monterey_dump_port_hsp);
EXPORT_SYMBOL(soc_monterey_show_voltage);
EXPORT_SYMBOL(soc_monterey_show_ring_osc);
EXPORT_SYMBOL(soc_monterey_show_material_process);
#endif
#ifdef BCM_TRIDENT3_SUPPORT
EXPORT_SYMBOL(_soc_trident3_alpm_bkt_view_get);
EXPORT_SYMBOL(_soc_trident3_alpm_bkt_view_set);
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
EXPORT_SYMBOL(_soc_th_lpm_view_get);
EXPORT_SYMBOL(_soc_tomahawk_alpm_bkt_view_get);
EXPORT_SYMBOL(_soc_tomahawk_alpm_bkt_view_set);
EXPORT_SYMBOL(soc_tomahawk_xpe_base_index_check);
EXPORT_SYMBOL(soc_tomahawk_sc_base_index_check);
EXPORT_SYMBOL(soc_tomahawk_show_ring_osc);
EXPORT_SYMBOL(soc_tomahawk_show_voltage);
EXPORT_SYMBOL(soc_th_mem_index_invalid);
EXPORT_SYMBOL(soc_th_mem_is_dual_mode);
EXPORT_SYMBOL(soc_th_ifp_tcam_range_dma_read);
EXPORT_SYMBOL(soc_th_dual_tcam_index_valid);
EXPORT_SYMBOL(bcm_esw_port_asf_diag);
EXPORT_SYMBOL(bcm_esw_port_asf_show);
EXPORT_SYMBOL(bcm_esw_port_asf_mode_get);
EXPORT_SYMBOL(bcm_esw_asf_diag);
EXPORT_SYMBOL(bcmi_esw_switch_latency_show);
EXPORT_SYMBOL(bcmi_esw_switch_latency_diag);
EXPORT_SYMBOL(bcmi_esw_switch_latency_reg_avail);
EXPORT_SYMBOL(bcmi_esw_switch_latency_mem_avail);
EXPORT_SYMBOL(_bcm_esw_ifp_color_map_set);
EXPORT_SYMBOL(_bcm_esw_ifp_color_map_get);
EXPORT_SYMBOL(_bcm_field_th_class_qual_support_check);
#ifdef SW_AUTONEG_SUPPORT  
EXPORT_SYMBOL(bcm_sw_an_enable_get);
EXPORT_SYMBOL(bcm_sw_an_enable_set);
EXPORT_SYMBOL(bcm_sw_an_port_diag);
#endif
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT)
EXPORT_SYMBOL(soc_tomahawk2_sed_base_index_check);
#endif /* BCM_TOMAHAWK2_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
EXPORT_SYMBOL(soc_driver_bcm56980_a0);
EXPORT_SYMBOL(soc_th3_l2_learn_thread_running);
EXPORT_SYMBOL(soc_th3_l2_learn_thread_start);
EXPORT_SYMBOL(_soc_th3_lpm_view_get);
EXPORT_SYMBOL(soc_th3_l2_learn_thread_stop);
EXPORT_SYMBOL(soc_th3_lrn_shadow_show);
EXPORT_SYMBOL(soc_th3_lrn_cache_intr_handler);
EXPORT_SYMBOL(soc_th3_check_idb_portmap);
EXPORT_SYMBOL(soc_th3_check_mmu_portmap);
EXPORT_SYMBOL(soc_th3_check_settings_from_file);
EXPORT_SYMBOL(soc_th3_mmu_chip_port_num);
EXPORT_SYMBOL(soc_tomahawk3_schduler_hier_show);
EXPORT_SYMBOL(soc_th3_mmu_local_port_num);
EXPORT_SYMBOL(soc_th3_retrieve_mmu_mem_list_for_port);
EXPORT_SYMBOL(_bcm_th3_ft_dump_stats);
EXPORT_SYMBOL(_bcm_th3_ft_dump_flow_table);
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
EXPORT_SYMBOL(soc_katana_show_ring_osc);
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
EXPORT_SYMBOL(soc_hu2_show_ring_osc);
EXPORT_SYMBOL(soc_hu2_show_material_process);
#endif /* BCM_HURRICANE2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
EXPORT_SYMBOL(soc_kt2_linkphy_port_reg_blk_idx_get);
EXPORT_SYMBOL(soc_kt2_linkphy_port_blk_idx_get);
EXPORT_SYMBOL(soc_kt2_dump_port_lls);
EXPORT_SYMBOL(soc_kt2_show_ring_osc);
EXPORT_SYMBOL(soc_kt2_show_material_process);
EXPORT_SYMBOL(soc_kt2_show_voltage);
EXPORT_SYMBOL(_soc_kt2_config_get);
EXPORT_SYMBOL(bcm_kt2_subport_port_stat_show);
EXPORT_SYMBOL(bcm_kt2_dump_port_lls);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
EXPORT_SYMBOL(_bcm_kt2_subport_sw_dump);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
EXPORT_SYMBOL(soc_ism_get_banks_for_mem);
EXPORT_SYMBOL(soc_tr3_dump_port_lls);
EXPORT_SYMBOL(soc_tr3_dump_port_hsp);
EXPORT_SYMBOL(soc_tr3_esm_init_read_config);
EXPORT_SYMBOL(soc_tr3_mmu_config_init);
EXPORT_SYMBOL(soc_triumph3_esm_init);
EXPORT_SYMBOL(soc_tr3_esmif_init);
EXPORT_SYMBOL(tr3_tcam_partition_order_num_entries);
EXPORT_SYMBOL(tr3_tcam_partition_order);
EXPORT_SYMBOL(mdio_portid_get);
EXPORT_SYMBOL(nl_mdio_prbs_chk_err);
EXPORT_SYMBOL(nl_mdio_prbs_gen);
EXPORT_SYMBOL(nl_mdio_prbs_chk);
EXPORT_SYMBOL(wcmod_esm_serdes_control_set);
EXPORT_SYMBOL(wcmod_esm_serdes_control_get);
EXPORT_SYMBOL(_bcm_esw_ibod_debug_flag_set);
EXPORT_SYMBOL(_bcm_esw_ibod_sync_recovery_running);
EXPORT_SYMBOL(_bcm_esw_ibod_sync_recovery_start);
EXPORT_SYMBOL(_bcm_esw_ibod_sync_recovery_stop);
EXPORT_SYMBOL(soc_tr3_esm_get_worst_fme);
EXPORT_SYMBOL(hwtl_find_idle_req_rsp_latency);
#endif /* BCM_TRIUMPH3_SUPPORT */
EXPORT_SYMBOL(soc_xaui_config_get);
EXPORT_SYMBOL(soc_xaui_config_set);
EXPORT_SYMBOL(soc_xaui_rxbert_bit_err_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_byte_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_byte_err_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_enable);
EXPORT_SYMBOL(soc_xaui_rxbert_pkt_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_pkt_err_count_get);
EXPORT_SYMBOL(soc_xaui_txbert_byte_count_get);
EXPORT_SYMBOL(soc_xaui_txbert_enable);
EXPORT_SYMBOL(soc_xaui_txbert_pkt_count_get);
#ifdef BCM_XGS3_SWITCH_SUPPORT
EXPORT_SYMBOL(soc_xgs3_l3_ecmp_hash);
#endif /* BCM_XGS3_SWITCH_SUPPORT */
#ifdef BCM_ISM_SUPPORT
EXPORT_SYMBOL(soc_generic_get_hash_key);
EXPORT_SYMBOL(soc_generic_hash);
EXPORT_SYMBOL(soc_ism_gen_key_from_keyfields);
EXPORT_SYMBOL(soc_ism_get_hash_bits);
EXPORT_SYMBOL(soc_ism_hash_offset_config);
EXPORT_SYMBOL(soc_ism_hash_offset_config_get);
EXPORT_SYMBOL(soc_ism_mem_hash_config);
EXPORT_SYMBOL(soc_ism_mem_hash_config_get);
EXPORT_SYMBOL(soc_ism_mem_max_key_bits_get);
#endif /* BCM_ISM_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
EXPORT_SYMBOL(soc_hbx_higig2_mcast_sizes_get);
#endif
#ifdef BCM_GREYHOUND_SUPPORT
EXPORT_SYMBOL(soc_greyhound_pgw_reg_blk_index_get);
#endif
#ifdef BCM_SABER2_SUPPORT
EXPORT_SYMBOL(soc_sb2_iecell_port_reg_blk_idx_get);
EXPORT_SYMBOL(soc_sb2_and28_dram_init_reset);
EXPORT_SYMBOL(soc_and28_phy_reg_read);
EXPORT_SYMBOL(soc_and28_phy_reg_write);
EXPORT_SYMBOL(soc_and28_shmoo_ctl);
EXPORT_SYMBOL(soc_sb2_and28_dram_restorecfg);
EXPORT_SYMBOL(soc_sb2_and28_dram_savecfg);
EXPORT_SYMBOL(bcm_sb2_sat_oamp_enable_get);
EXPORT_SYMBOL(soc_sb2_show_voltage);
#endif
#ifdef BCM_METROLITE_SUPPORT
EXPORT_SYMBOL(soc_ml_iecell_port_reg_blk_idx_get);
#endif
EXPORT_SYMBOL(_bcm_esw_port_sw_info_display);
EXPORT_SYMBOL(_bcm_esw_port_hw_info_display);
#endif /* BCM_ESW_SUPPORT */

#include <shared/shr_resmgr.h>
#include <shared/bsl.h>
#include <shared/bslext.h>
#include <shared/bslnames.h>

EXPORT_SYMBOL(shr_mres_alloc);
EXPORT_SYMBOL(shr_mres_alloc_align_sparse);
EXPORT_SYMBOL(shr_mres_alloc_tag);
EXPORT_SYMBOL(shr_mres_alloc_align);
EXPORT_SYMBOL(shr_mres_alloc_align_tag);
EXPORT_SYMBOL(shr_mres_check);
EXPORT_SYMBOL(shr_mres_check_all_sparse);
EXPORT_SYMBOL(shr_mres_create);
EXPORT_SYMBOL(shr_mres_destroy);
EXPORT_SYMBOL(shr_mres_dump);
EXPORT_SYMBOL(shr_mres_free);
EXPORT_SYMBOL(shr_mres_free_and_status);
EXPORT_SYMBOL(shr_mres_get);
EXPORT_SYMBOL(shr_mres_pool_get);
EXPORT_SYMBOL(shr_mres_pool_info_get);
EXPORT_SYMBOL(shr_mres_pool_set);
EXPORT_SYMBOL(shr_mres_pool_unset);
EXPORT_SYMBOL(shr_mres_type_get);
EXPORT_SYMBOL(shr_mres_type_info_get);
EXPORT_SYMBOL(shr_mres_type_set);
EXPORT_SYMBOL(shr_mres_type_unset);
EXPORT_SYMBOL(shr_mres_free_sparse);
EXPORT_SYMBOL(shr_res_alloc);
EXPORT_SYMBOL(shr_res_alloc_tag);
EXPORT_SYMBOL(shr_res_alloc_align);
EXPORT_SYMBOL(shr_res_alloc_align_tag);
EXPORT_SYMBOL(shr_res_check);
EXPORT_SYMBOL(shr_res_detach);
EXPORT_SYMBOL(shr_res_dump);
EXPORT_SYMBOL(shr_res_free);
EXPORT_SYMBOL(shr_res_get);
EXPORT_SYMBOL(shr_res_init);
EXPORT_SYMBOL(shr_res_pool_get);
EXPORT_SYMBOL(shr_res_pool_set);
EXPORT_SYMBOL(shr_res_type_get);
EXPORT_SYMBOL(shr_res_type_set);
EXPORT_SYMBOL(_bsl_error);
EXPORT_SYMBOL(_bsl_warn);
EXPORT_SYMBOL(_bsl_verbose);
EXPORT_SYMBOL(_bsl_vverbose);
EXPORT_SYMBOL(bsl_check);
EXPORT_SYMBOL(bsl_fast_check);
EXPORT_SYMBOL(bsl_config_t_init);
EXPORT_SYMBOL(bsl_init);
EXPORT_SYMBOL(bsl_layer2str);
EXPORT_SYMBOL(bsl_source2str);
EXPORT_SYMBOL(bsl_severity2str);
EXPORT_SYMBOL(bsl_vprintf);
EXPORT_SYMBOL(bsl_printf);
EXPORT_SYMBOL(bsl_log_add);
EXPORT_SYMBOL(bsl_log_end);
EXPORT_SYMBOL(bsl_log_start);
#ifdef INCLUDE_MACSEC
EXPORT_SYMBOL(bcm_common_macsec_config_print);
#endif /* INCLUDE_MACSEC */
extern int phy8806x_xmod_debug_cmd;
EXPORT_SYMBOL(phy8806x_xmod_debug_cmd);
#ifdef INCLUDE_FCMAP
extern int bfcmap88060_xmod_debug_cmd;
extern int bfcmap_pcfg_set_get;
EXPORT_SYMBOL(bfcmap88060_xmod_debug_cmd);
EXPORT_SYMBOL(bfcmap_pcfg_set_get);
#endif /* INCLUDE_FCMAP */
#if defined(BCM_WARM_BOOT_SUPPORT)
extern void bfcmap_warmboot_recover(void);
EXPORT_SYMBOL(bfcmap_warmboot_recover);
#endif

#ifdef BCM_CMICM_SUPPORT
#include <soc/pscan.h>
EXPORT_SYMBOL(soc_pscan_delay);
EXPORT_SYMBOL(soc_pscan_port_config);
EXPORT_SYMBOL(soc_pscan_port_enable);
EXPORT_SYMBOL(soc_pscan_update);
EXPORT_SYMBOL(soc_pscan_init);
EXPORT_SYMBOL(soc_pscan_detach);
#endif /* BCM_CMICM_SUPPORT */

#include <soc/i2c.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/intr.h>
#include <soc/register.h>
EXPORT_SYMBOL(soc_deinit);
EXPORT_SYMBOL(soc_device_reset);
#if defined(BCM_ESW_SUPPORT) || defined(BCM_DFE_SUPPORT)|| defined(BCM_PETRA_SUPPORT)
EXPORT_SYMBOL(soc_nof_interrupts);
#endif
EXPORT_SYMBOL(soc_property_suffix_num_get);
EXPORT_SYMBOL(soc_chip_group_names);
EXPORT_SYMBOL(soc_chip_type_map);

#include <soc/phy/phyctrl.h>
EXPORT_SYMBOL(phyident_core_info_t_init);
extern int soc_phy_set_verbose(int unit, int port, int verbose);
EXPORT_SYMBOL(soc_phy_set_verbose);

EXPORT_SYMBOL(shr_bitop_range_and);
EXPORT_SYMBOL(shr_bitop_range_copy);
EXPORT_SYMBOL(shr_bitop_range_eq);
EXPORT_SYMBOL(shr_bitop_range_negate);
EXPORT_SYMBOL(shr_bitop_range_null);
EXPORT_SYMBOL(shr_bitop_range_set);

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined (BCM_DPP_SUPPORT)
#include <soc/uc_msg.h>
EXPORT_SYMBOL(soc_direct_reg_get);
EXPORT_SYMBOL(soc_direct_reg_set);
#endif /* defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined (BCM_DPP_SUPPORT) */
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
EXPORT_SYMBOL(soc_cmic_uc_appl_init);
EXPORT_SYMBOL(soc_cmic_uc_msg_receive);
EXPORT_SYMBOL(soc_cmic_uc_msg_receive_cancel);
EXPORT_SYMBOL(soc_cmic_uc_msg_send);
EXPORT_SYMBOL(soc_cmic_uc_msg_send_receive);
#if defined(BCM_CMICM_SUPPORT)
EXPORT_SYMBOL(soc_cmicm_cmcx_intr0_enable);
EXPORT_SYMBOL(soc_cmicm_cmcx_intr1_enable);
EXPORT_SYMBOL(soc_cmicm_cmcx_intr2_enable);
EXPORT_SYMBOL(soc_cmicm_cmcx_intr0_disable);
EXPORT_SYMBOL(soc_cmicm_cmcx_intr1_disable);
EXPORT_SYMBOL(soc_cmicm_cmcx_intr2_disable);
#endif
#endif /* defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT) */

#if defined(BCM_ESW_SUPPORT) || defined(BCM_DFE_SUPPORT)|| defined(BCM_PETRA_SUPPORT)
EXPORT_SYMBOL(soc_interrupt_disable);
EXPORT_SYMBOL(soc_interrupt_enable);
EXPORT_SYMBOL(soc_interrupt_get);
EXPORT_SYMBOL(soc_interrupt_info_get);
EXPORT_SYMBOL(soc_interrupt_is_enabled);
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_DFE_SUPPORT)|| defined(BCM_PETRA_SUPPORT) */

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <soc/wb_engine.h>
#include <soc/dcmn/dcmn_wb.h>
EXPORT_SYMBOL(soc_wb_engine_dump);
EXPORT_SYMBOL(soc_wb_engine_var_get);
#ifdef BCM_WARM_BOOT_API_TEST
EXPORT_SYMBOL(soc_dcmn_wb_test_skip_multiple_wb_tests_set);
#endif
#endif /* BCM_DPP_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_common.h>

EXPORT_SYMBOL(phymod_phy_rx_pmd_locked_get);
EXPORT_SYMBOL(portmod_port_chain_core_access_get);
EXPORT_SYMBOL(portmod_access_get_params_t_init);
EXPORT_SYMBOL(portmod_port_phy_lane_access_get);
EXPORT_SYMBOL(portmod_dispatch_type_t_mapping);
EXPORT_SYMBOL(portmod_pm_diag_info_get);
EXPORT_SYMBOL(portmod_port_diag_info_get);
EXPORT_SYMBOL(portmod_port_interface_config_get);
EXPORT_SYMBOL(portmod_port_is_legacy_ext_phy_present);
EXPORT_SYMBOL(portmod_port_encoding_get);
EXPORT_SYMBOL(portmod_port_link_get);
EXPORT_SYMBOL(portmod_port_loopback_get);
EXPORT_SYMBOL(portmod_port_autoneg_status_get);
EXPORT_SYMBOL(portmod_port_enable_get);
EXPORT_SYMBOL(portmod_port_local_fault_status_get);
EXPORT_SYMBOL(portmod_port_remote_fault_status_get);
EXPORT_SYMBOL(portmod_port_llfc_control_get);
EXPORT_SYMBOL(portmod_port_pfc_config_get);
EXPORT_SYMBOL(portmod_port_pfc_control_get);
EXPORT_SYMBOL(portmod_port_ability_local_get);
EXPORT_SYMBOL(portmod_port_ability_advert_get);
EXPORT_SYMBOL(portmod_port_ability_remote_get);
EXPORT_SYMBOL(portmod_port_main_core_access_get);
EXPORT_SYMBOL(phymod_core_diagnostics_get);
EXPORT_SYMBOL(phymod_core_firmware_info_get);
EXPORT_SYMBOL(phymod_phy_diagnostics_get);
EXPORT_SYMBOL(portmod_port_prbs_config_set);
EXPORT_SYMBOL(portmod_port_prbs_enable_set);
EXPORT_SYMBOL(portmod_port_prbs_status_get);
EXPORT_SYMBOL(phymod_phy_status_dump);
EXPORT_SYMBOL(phymod_diag_eyescan_run);
EXPORT_SYMBOL(portmod_core_port_mode_t_mapping);
EXPORT_SYMBOL(phymod_ref_clk_t_mapping);
EXPORT_SYMBOL(phymod_osr_mode_t_mapping);
EXPORT_SYMBOL(phymod_pmd_mode_t_mapping);
EXPORT_SYMBOL(portmod_port_to_phyaddr);
EXPORT_SYMBOL(portmod_port_to_phyaddr_int);
EXPORT_SYMBOL(portmod_port_diag_ctrl);
EXPORT_SYMBOL(portmod_port_core_num_get);
EXPORT_SYMBOL(portmod_port_core_access_get);
EXPORT_SYMBOL(phymod_core_info_get);
EXPORT_SYMBOL(phymod_core_version_t_mapping);
EXPORT_SYMBOL(phymod_core_access_t_init);
EXPORT_SYMBOL(phymod_core_info_t_init);
EXPORT_SYMBOL(portmod_port_phy_reg_read);
EXPORT_SYMBOL(portmod_port_check_legacy_phy);
EXPORT_SYMBOL(portmod_port_pm_type_get);
EXPORT_SYMBOL(portmod_port_flags_test);
EXPORT_SYMBOL(portmod_port_phy_reg_write);
EXPORT_SYMBOL(portmod_pm_info_get);
#endif /* PORTMOD_SUPPORT */

#if defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)
#include <soc/dcmn/dcmn_utils_eeprom.h>
#include <sal/appl/i2c.h>
#include <sal/appl/io.h>
#include <appl/diag/test.h>
#include <soc/shared/llm_msg.h>
#include <soc/shared/llm_pack.h>
#include <soc/iproc.h>
#include <soc/shmoo_combo28.h>
#include <soc/dcmn/fabric.h>
#include <soc/dcmn/dcmn_mem.h>


EXPORT_SYMBOL(soc_dcmn_miim_read);
EXPORT_SYMBOL(soc_dcmn_miim_write);
EXPORT_SYMBOL(dcmn_mem_array_wide_access);
EXPORT_SYMBOL(soc_dcmn_fabric_mesh_topology_diag_get);
EXPORT_SYMBOL(soc_combo28_cdr_ctl);
EXPORT_SYMBOL(soc_combo28_shmoo_ctl);
EXPORT_SYMBOL(soc_property_suffix_num_str_get);
EXPORT_SYMBOL(phy_mem_read);
EXPORT_SYMBOL(phy_mem_write);

#if defined(BCM_DPP_SUPPORT)

#include <soc/dpp/DRC/drc_combo28.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/cosq.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_resmgr.h>
#include <shared/swstate/access/sw_state_access.h>
#include <appl/diag/dcmn/diag.h>

EXPORT_SYMBOL(bcm_petra_stk_domain_modid_get);
EXPORT_SYMBOL(soc_dpp_drc_combo28_phy_reg_modify);
EXPORT_SYMBOL(soc_dpp_drc_combo28_phy_reg_read);
EXPORT_SYMBOL(soc_dpp_drc_combo28_phy_reg_write);
EXPORT_SYMBOL(soc_dpp_drc_combo28_shmoo_cfg_set);
EXPORT_SYMBOL(soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc);
EXPORT_SYMBOL(soc_dpp_drc_combo28_cdr_ctl);
EXPORT_SYMBOL(soc_dpp_drc_combo28_cdr_monitor_enable_get);
EXPORT_SYMBOL(soc_dpp_drc_combo28_cdr_monitor_enable_set);
EXPORT_SYMBOL(SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string);
EXPORT_SYMBOL(SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string);
EXPORT_SYMBOL(SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string);
EXPORT_SYMBOL(SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string);
EXPORT_SYMBOL(SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string);
EXPORT_SYMBOL(SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string);
EXPORT_SYMBOL(soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string);
EXPORT_SYMBOL(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear);
EXPORT_SYMBOL(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear);
EXPORT_SYMBOL(SOC_PPC_IP_ROUTING_TABLE_RANGE_clear);
EXPORT_SYMBOL(_bcm_dpp_vlan_edit_action_get);
EXPORT_SYMBOL(_bcm_petra_vlan_edit_eg_command_id_find);
EXPORT_SYMBOL(soc_ppd_eg_vlan_edit_command_info_get);
EXPORT_SYMBOL(soc_ppd_lif_ing_vlan_edit_command_id_get);
EXPORT_SYMBOL(soc_ppd_lif_ing_vlan_edit_command_info_get);
EXPORT_SYMBOL(soc_ppd_frwrd_ipv4_mc_route_get_block);
EXPORT_SYMBOL(arad_pp_frwrd_em_dest_to_fec);
EXPORT_SYMBOL(soc_sand_pp_ip_long_to_string);
EXPORT_SYMBOL(_bcm_petra_cosq_core_get);
EXPORT_SYMBOL(bcm_dpp_am_resource_id_to_pool_id_get);
EXPORT_SYMBOL(bcm_dpp_am_resource_to_nof_pools);
EXPORT_SYMBOL(bcm_petra_vig_display_whole_db);
EXPORT_SYMBOL(_sw_state_access_debug_hook);
EXPORT_SYMBOL(shr_sw_state_init);
EXPORT_SYMBOL(shr_sw_state_deinit);
EXPORT_SYMBOL(shr_sw_state_alloc);
EXPORT_SYMBOL(shr_sw_state_free);
EXPORT_SYMBOL(shr_sw_state_data_block_free_restored_data);
EXPORT_SYMBOL(shr_sw_state_data_block_set);
EXPORT_SYMBOL(shr_sw_state_data_block_get);
EXPORT_SYMBOL(shr_sw_state_data_block_restored_get);
EXPORT_SYMBOL(shr_sw_state_data_block_sync);
EXPORT_SYMBOL(shr_sw_state_block_dump);
EXPORT_SYMBOL(shr_sw_state_auto_sync);
EXPORT_SYMBOL(shr_sw_state_sizes_get);
EXPORT_SYMBOL(soc_sw_state_nof_allocations_get);
EXPORT_SYMBOL(sw_state_res_check);
EXPORT_SYMBOL(sw_state_res_pool_get);
EXPORT_SYMBOL(sw_state_res_pool_info_get);
EXPORT_SYMBOL(sw_state_access);
#endif /* BCM_DPP_SUPPORT */

extern int init_warmboot_test(int u, args_t *a, void *p);
extern int init_warmboot_test_init(int unit, args_t *a, void **p);
extern int init_warmboot_test_done(int unit, void *p);
EXPORT_SYMBOL(init_warmboot_test);
EXPORT_SYMBOL(init_warmboot_test_init);
EXPORT_SYMBOL(init_warmboot_test_done);
extern uint32 tr8_bits_counter_get(void);
extern uint32 tr8_get_bits_num(uint32 number);
extern void tr8_increment_bits_counter(uint32 bits_num);
extern void tr8_reset_bits_counter(void);
extern void tr8_write_dump(const char * _Format);
EXPORT_SYMBOL(tr8_bits_counter_get);
EXPORT_SYMBOL(tr8_get_bits_num);
EXPORT_SYMBOL(tr8_increment_bits_counter);
EXPORT_SYMBOL(tr8_reset_bits_counter);
EXPORT_SYMBOL(tr8_write_dump);
EXPORT_SYMBOL(eeprom_read);
EXPORT_SYMBOL(eeprom_read_str);
EXPORT_SYMBOL(eeprom_write);
EXPORT_SYMBOL(eeprom_write_str);
extern int init_deinit_test(int u, args_t *a, void *p);
EXPORT_SYMBOL(init_deinit_test);
extern int init_deinit_test_init(int unit, args_t *a, void **p);
EXPORT_SYMBOL(init_deinit_test_init);
extern int init_deinit_test_done(int unit, void *p);
EXPORT_SYMBOL(init_deinit_test_done);
EXPORT_SYMBOL(sal_fclose);
EXPORT_SYMBOL(sal_fopen);
EXPORT_SYMBOL(sal_i2c_config_get);
EXPORT_SYMBOL(sal_vprintf);
/* EXPORT_SYMBOL(sal_fprintf); */
EXPORT_SYMBOL(sh_process_command);
extern int test_dispatch(int u, test_t *test, int loops, args_t *a);
EXPORT_SYMBOL(test_dispatch);
EXPORT_SYMBOL(_g_unitResDesc);
EXPORT_SYMBOL(_shr_format_long_integer);
EXPORT_SYMBOL(cpu_i2c_write);
extern int mem_test(int u, args_t *a, void *p);
EXPORT_SYMBOL(mem_test);
extern int mem_test_default_init(int u, soc_mem_t mem, void **p);
EXPORT_SYMBOL(mem_test_default_init);
extern int mem_test_done(int unit, void *p);
EXPORT_SYMBOL(mem_test_done);
extern int mem_test_rw_init(int u, soc_mem_t mem, void **p);
EXPORT_SYMBOL(mem_test_rw_init);
EXPORT_SYMBOL(shr_llm_msg_app_info_get_pack);
EXPORT_SYMBOL(shr_llm_msg_app_info_get_unpack);
EXPORT_SYMBOL(shr_llm_msg_ctrl_init_pack);
EXPORT_SYMBOL(shr_llm_msg_pointer_pool_pack);
EXPORT_SYMBOL(shr_llm_msg_pointer_pool_unpack);
EXPORT_SYMBOL(shr_llm_msg_pon_att_enable_pack);
EXPORT_SYMBOL(shr_llm_msg_pon_att_pack);
EXPORT_SYMBOL(shr_llm_msg_pon_att_unpack);
EXPORT_SYMBOL(shr_llm_msg_pon_db_pack);
EXPORT_SYMBOL(shr_llm_msg_pon_db_unpack);
EXPORT_SYMBOL(shr_max_buffer_get);
EXPORT_SYMBOL(shr_res_pool_info_get);
EXPORT_SYMBOL(soc_property_port_suffix_num_get_str);
EXPORT_SYMBOL(soc_scache_recover);
EXPORT_SYMBOL(soc_iproc_getreg);
EXPORT_SYMBOL(soc_iproc_setreg);
EXPORT_SYMBOL(common_test_cnt);
EXPORT_SYMBOL(common_test_list);
EXPORT_SYMBOL(sand_test_cnt);
EXPORT_SYMBOL(sand_test_list);
EXPORT_SYMBOL(soc_dpp_block_names);
EXPORT_SYMBOL(soc_dpp_block_port_names);
extern int _cpu_write(int d, uint32 addr, uint32 *buf);
extern int _cpu_read(int d, uint32 addr, uint32 *buf);
EXPORT_SYMBOL(_cpu_write);
EXPORT_SYMBOL(_cpu_read);
extern int _cpu_pci_register(int d);
EXPORT_SYMBOL(_cpu_pci_register);
EXPORT_SYMBOL(_soc_arad_mem_array_wide_access);
#endif /* defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT) */

#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/PPC/ppc_api_diag.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/gport_mgmt_sw_db.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/dpp_eyescan.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_egr_prge_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_drv.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dcmn/utils_fpga.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Management/sand_module_management.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/port_sw_db.h>
#include <appl/dpp/FecAllocation/fec_allocation.h>
#include <appl/dpp/FecPerformance/fec_performance.h>
#include <appl/dpp/regs_filter.h>
EXPORT_SYMBOL(fec_allocation_main);
EXPORT_SYMBOL(fec_performance_main);
#include <appl/diag/diag_mmu.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <soc/shared/llm_appl.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif /* CRASH_RECOVERY_SUPPORT */

EXPORT_SYMBOL(block_can_be_disabled);
EXPORT_SYMBOL(blocks_can_be_disabled);
EXPORT_SYMBOL(soc_mem_alias_map);
EXPORT_SYMBOL(cmd_dpp_diag_mmu);
EXPORT_SYMBOL(cmd_dpp_diag_mmu_usage);
EXPORT_SYMBOL(soc_reg_above_64_field64_get);
EXPORT_SYMBOL(shr_template_dump);
EXPORT_SYMBOL(SOC_PPC_FRWRD_DECISION_TYPE_to_string);
EXPORT_SYMBOL(soc_ppc_diag_glem_outlif_print);
EXPORT_SYMBOL(SOC_PPC_MPLS_COMMAND_TYPE_to_string);
EXPORT_SYMBOL(SOC_PPC_OUTLIF_ENCODE_TYPE_to_string);
EXPORT_SYMBOL(SOC_PPC_TRAP_CODE_to_string);
EXPORT_SYMBOL(SOC_PPC_TRAP_CODE_to_api_string);
EXPORT_SYMBOL(SOC_PPC_DIAG_DB_USE_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string);
EXPORT_SYMBOL(SOC_PPC_FP_ACTION_TYPE_to_string);
EXPORT_SYMBOL(SOC_PPC_FP_QUAL_TYPE_to_string);
EXPORT_SYMBOL(SOC_PPC_FP_PACKET_DIAG_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_FP_PACKET_DIAG_INFO_print);
EXPORT_SYMBOL(SOC_PPC_PKT_TERM_TYPE_to_string);
EXPORT_SYMBOL(soc_sand_SAND_PP_IPV6_ADDRESS_print);
EXPORT_SYMBOL(soc_sand_SAND_PP_MAC_ADDRESS_print);
EXPORT_SYMBOL(soc_sand_SAND_PP_SYS_PORT_TYPE_to_string);
EXPORT_SYMBOL(soc_sand_error_handler);
EXPORT_SYMBOL(soc_sand_initialize_error_word);
EXPORT_SYMBOL(soc_sand_set_error_code_into_error_word);
EXPORT_SYMBOL(soc_sand_update_error_code);
EXPORT_SYMBOL(SOC_PPC_FRWRD_DECISION_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_DB_USE_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_ENCAP_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_ENCAP_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_FRWRD_LKUP_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_IPV4_VPN_KEY_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_LEARN_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_LEARN_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_LEM_KEY_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_LEM_KEY_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_LEM_LKUP_TYPE_to_string);
EXPORT_SYMBOL(SOC_PPC_DIAG_LEM_VALUE_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_LEM_VALUE_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_LIF_LKUP_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_LIF_LKUP_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_MODE_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_MODE_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_VSI_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_VSI_INFO_print);
EXPORT_SYMBOL(soc_ppd_diag_vsi_info_get);
EXPORT_SYMBOL(SOC_PPC_DIAG_PARSING_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_PARSING_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_PKT_TM_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_PKT_TM_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_TERM_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_TERM_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_TRAPS_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_TRAPS_INFO_print);
EXPORT_SYMBOL(SOC_PPC_DIAG_VLAN_EDIT_RES_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_VLAN_EDIT_RES_print);
EXPORT_SYMBOL(SOC_PPC_FP_RESOURCE_DIAG_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_FP_RESOURCE_DIAG_INFO_print);
EXPORT_SYMBOL(SOC_PPC_FRWRD_FEC_MATCH_RULE_clear);
EXPORT_SYMBOL(SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_EG_DROP_LOG_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_DIAG_EG_DROP_LOG_INFO_print);
EXPORT_SYMBOL(_bcm_dpp_global_lif_mapping_global_to_local_get);
EXPORT_SYMBOL(_bcm_dpp_rx_trap_hw_id_to_sw_id);
EXPORT_SYMBOL(arad_pp_diag_fwd_decision_in_buffer_parse);
EXPORT_SYMBOL(dpp_get_mcds);
EXPORT_SYMBOL(soc_ppd_frwrd_fec_entry_get);
EXPORT_SYMBOL(soc_ppd_lag_get);
EXPORT_SYMBOL(soc_ppd_trap_frwrd_profile_info_get);
EXPORT_SYMBOL(arad_pp_occ_mgmt_diag_info_get);
EXPORT_SYMBOL(SOC_PPC_OCC_ALL_PROFILES_print);
EXPORT_SYMBOL(SOC_PPC_OCC_PROFILE_USAGE_print);
EXPORT_SYMBOL(SOC_TMC_CNT_MODE_STATISTICS_to_string);
EXPORT_SYMBOL(SOC_TMC_CNT_SRC_TYPE_to_string);
EXPORT_SYMBOL(SOC_TMC_INTERFACE_ID_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_FLOW_AND_UP_INFO_clear);
EXPORT_SYMBOL(SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_FLOW_TYPE_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_GROUP_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_SE_HR_MODE_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_SE_STATE_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_SE_TYPE_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_SLOW_RATE_NDX_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_SUB_FLOW_CL_CLASS_to_string);
EXPORT_SYMBOL(SOC_TMC_SCH_SUB_FLOW_HR_CLASS_to_string);
EXPORT_SYMBOL(SOC_TMC_PORT_HEADER_TYPE_to_string);
EXPORT_SYMBOL(_bcm_dpp_cosq_sw_dump);
EXPORT_SYMBOL(_bcm_dpp_counters_sw_dump);
EXPORT_SYMBOL(_bcm_dpp_lif_ac_match_print);
EXPORT_SYMBOL(_bcm_dpp_lif_mpls_match_print);
EXPORT_SYMBOL(_bcm_dpp_global_lif_mapping_local_to_global_get);
EXPORT_SYMBOL(_bcm_dpp_lif_to_gport);
EXPORT_SYMBOL(_bcm_dpp_port_sw_dump);
EXPORT_SYMBOL(_bcm_dpp_port_encap_to_fwd_decision);
EXPORT_SYMBOL(_bcm_dpp_sw_db_hash_vlan_find);
EXPORT_SYMBOL(_bcm_dpp_sw_db_hash_vlan_print);
EXPORT_SYMBOL(_bcm_dpp_stk_sw_dump);
#ifdef BCM_WARM_BOOT_API_TEST
EXPORT_SYMBOL(soc_dcmn_wb_no_wb_test_set);
EXPORT_SYMBOL(soc_dcmn_wb_test_mode_set);
#endif
EXPORT_SYMBOL(_bcm_dpp_oam_bfd_diagnostics_LM_counters);
EXPORT_SYMBOL(_bcm_dpp_oam_bfd_diagnostics_endpoint_by_id);
EXPORT_SYMBOL(_bcm_dpp_oam_bfd_diagnostics_endpoints);
extern int _bcm_dpp_trunk_sw_dump(int unit);
EXPORT_SYMBOL(_bcm_dpp_trunk_sw_dump);
extern cmd_result_t _bcm_petra_field_test_action_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_action_set);
extern cmd_result_t _bcm_petra_field_test_cascaded(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_cascaded);
extern cmd_result_t _bcm_petra_field_test_data_qualifier_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_data_qualifier_set);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_data_qualifiers);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_data_qualifiers_entry);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_data_qualifiers_entry_traffic);
extern cmd_result_t _bcm_petra_field_test_de_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_de_entry);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_de_entry_traffic);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_direct_table_entry);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_direct_table_entry_traffic);
extern cmd_result_t _bcm_petra_field_test_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_entry);
extern cmd_result_t _bcm_petra_field_test_entry_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_entry_priority);
extern cmd_result_t _bcm_petra_field_test_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_entry_traffic);
extern cmd_result_t _bcm_petra_field_test_entry_traffic_perf(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_entry_traffic_perf);
extern cmd_result_t _bcm_petra_field_test_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_destroy);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy_with_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_destroy_with_traffic);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy_with_traffic_and_de_fg(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_destroy_with_traffic_and_de_fg);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_extraction(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_direct_extraction);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_table(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_direct_table);
extern cmd_result_t _bcm_petra_field_test_field_group_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_presel);
extern cmd_result_t _bcm_petra_field_test_field_group_presel_1(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_presel_1);
extern cmd_result_t _bcm_petra_field_test_field_group_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_field_group_priority);
extern cmd_result_t _bcm_petra_field_test_full_tcam(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_full_tcam);
extern cmd_result_t _bcm_petra_field_test_full_tcam_perf(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_full_tcam_perf);
extern cmd_result_t _bcm_petra_field_test_full_tcam_diff_prio(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_full_tcam_diff_prio);
extern cmd_result_t _bcm_petra_field_test_full_tcam_diff_prio_perf(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_full_tcam_diff_prio_perf);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_itmh_field_group);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_itmh_field_group_traffic);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_itmh_parsing_test);
extern cmd_result_t _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_predefined_data_qualifiers_entry_traffic);
extern cmd_result_t _bcm_petra_field_test_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_presel);
extern cmd_result_t _bcm_petra_field_test_presel_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_presel_set);
extern cmd_result_t _bcm_petra_field_test_qualify_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_qualify_set);
extern cmd_result_t _bcm_petra_field_test_resend_last_packet(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_resend_last_packet);
extern cmd_result_t _bcm_petra_field_test_shared_bank(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_shared_bank);
extern cmd_result_t _bcm_petra_field_test_compress(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_compress);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test_pb(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_itmh_parsing_test_pb);
extern cmd_result_t _bcm_petra_field_test_user_header(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_user_header);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic_large(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
EXPORT_SYMBOL(_bcm_petra_field_test_de_entry_traffic_large);
EXPORT_SYMBOL(_bcm_petra_l2_to_petra_entry);
EXPORT_SYMBOL(_bcm_petra_l2_sw_traverse);
EXPORT_SYMBOL(bcm_petra_port_internal_get);
EXPORT_SYMBOL(bcm_petra_l2_learn_msgs_config_set);
EXPORT_SYMBOL(_bcm_arad_field_qual_info);
EXPORT_SYMBOL(_bcm_counter_thread_is_running);
EXPORT_SYMBOL(_bcm_dpp_field_unit_info);
EXPORT_SYMBOL(_bcm_dpp_field_range_get);
EXPORT_SYMBOL(_bcm_dpp_field_qual_name);
EXPORT_SYMBOL(_bcm_dpp_init_finished_ok);
EXPORT_SYMBOL(_bcm_dpp_mpls_bk_info);
EXPORT_SYMBOL(_bcm_dpp_pool_info);
EXPORT_SYMBOL(_bcm_dpp_port_prbs_mac_mode);
EXPORT_SYMBOL(_bcm_dpp_template_info);
EXPORT_SYMBOL(_bcm_dpp_lif_id_is_mapped);
EXPORT_SYMBOL(_bcm_dpp_am_local_inlif_alloc);
EXPORT_SYMBOL(_bcm_dpp_am_local_inlif_dealloc);
EXPORT_SYMBOL(_bcm_dpp_am_local_inlif_is_alloc);
EXPORT_SYMBOL(_bcm_dpp_am_local_outlif_alloc);
EXPORT_SYMBOL(_bcm_dpp_am_local_outlif_dealloc);
EXPORT_SYMBOL(_bcm_dpp_am_local_inlif_counting_profile_set);
EXPORT_SYMBOL(_bcm_dpp_am_local_outlif_counting_profile_set);
EXPORT_SYMBOL(_bcm_l2_auth_traverse_mac_addrs);
EXPORT_SYMBOL(_bcm_l2_auth_traverse_mac_infos);
EXPORT_SYMBOL(_bcm_l2_traverse_mact_keys);
EXPORT_SYMBOL(_bcm_l2_traverse_mact_vals);
EXPORT_SYMBOL(_bcm_mpls_traverse_ilm_keys);
EXPORT_SYMBOL(_bcm_mpls_traverse_ilm_vals);
EXPORT_SYMBOL(_bcm_petra_l2_freeze_state);
EXPORT_SYMBOL(_cell_id_curr);
EXPORT_SYMBOL(_dpp_phy_addr_multi_get);
EXPORT_SYMBOL(_dpp_port_config);
EXPORT_SYMBOL(_l3_sw_db);
EXPORT_SYMBOL(_mirror_sw_db);
EXPORT_SYMBOL(warmboot_test_mode);
EXPORT_SYMBOL(arad_dbg_autocredit_set_unsafe);
EXPORT_SYMBOL(arad_dbg_egress_shaping_enable_set_unsafe);
EXPORT_SYMBOL(arad_dbg_flow_control_enable_set_unsafe);
EXPORT_SYMBOL(arad_diag_signals_dump);
EXPORT_SYMBOL(arad_dram_buffer_get_info);
EXPORT_SYMBOL(arad_dram_crc_del_buffer_max_reclaims_set);
EXPORT_SYMBOL(arad_dram_crc_delete_buffer_enable);
EXPORT_SYMBOL(arad_dram_delete_buffer_action);
EXPORT_SYMBOL(arad_dram_delete_buffer_read_fifo);
EXPORT_SYMBOL(arad_dram_delete_buffer_test);
EXPORT_SYMBOL(arad_dram_get_buffer_error_cntr_in_list_index);
EXPORT_SYMBOL(arad_dram_init_buffer_error_cntr);
EXPORT_SYMBOL(arad_dram_mmu_indirect_get_logical_address_full);
EXPORT_SYMBOL(arad_dram_mmu_indirect_read);
EXPORT_SYMBOL(arad_dram_mmu_indirect_write);
EXPORT_SYMBOL(arad_dram_rbus_modify);
EXPORT_SYMBOL(arad_dram_rbus_read);
EXPORT_SYMBOL(arad_dram_rbus_write);
EXPORT_SYMBOL(arad_dram_rbus_write_br);
EXPORT_SYMBOL(arad_egr_prog_editor_print_all_programs_data);
EXPORT_SYMBOL(arad_pp_diag_is_valid);
EXPORT_SYMBOL(arad_pp_fp_db_stage_get);
EXPORT_SYMBOL(arad_pp_fp_key_length_get_unsafe);
EXPORT_SYMBOL(arad_egr_prog_editor_print_chosen_program);
EXPORT_SYMBOL(ARAD_PP_DIAG_MTR_INFO_print);
EXPORT_SYMBOL(SOC_PPC_MTR_METER_ID_clear);
EXPORT_SYMBOL(bcm_petra_policer_get);
EXPORT_SYMBOL(arad_pp_diag_prge_first_instr_get);
EXPORT_SYMBOL(arad_egr_prge_mgmt_diag_print);
EXPORT_SYMBOL(arad_fabric_link_status_all_get);
EXPORT_SYMBOL(arad_fabric_link_status_clear);
EXPORT_SYMBOL(arad_iqm_dynamic_tbl_get_unsafe);
EXPORT_SYMBOL(arad_nif_id2type);
EXPORT_SYMBOL(arad_port_header_type_get);
EXPORT_SYMBOL(arad_port_header_type_get_unsafe);
EXPORT_SYMBOL(arad_pp_ihp_mact_entry_count_db_tbl_get_unsafe);
EXPORT_SYMBOL(arad_pp_ihp_mact_fid_counter_db_tbl_get_unsafe);
EXPORT_SYMBOL(arad_pp_isem_access_print_all_programs_data);
EXPORT_SYMBOL(arad_pp_dbal_tables_dump);
EXPORT_SYMBOL(arad_pp_dbal_qualifier_to_instruction_dump);
EXPORT_SYMBOL(arad_pp_dbal_table_print);
EXPORT_SYMBOL(arad_pp_dbal_phisycal_db_dump);
EXPORT_SYMBOL(arad_pp_dbal_lem_prefix_table_dump);
EXPORT_SYMBOL(arad_pp_dbal_isem_prefix_table_dump);
EXPORT_SYMBOL(arad_pp_dbal_tcam_prefix_table_dump);
EXPORT_SYMBOL(arad_pp_dbal_last_packet_dump);
EXPORT_SYMBOL(arad_pp_dbal_table_info_dump);
EXPORT_SYMBOL(arad_pp_dbal_diag_entry_manage);
EXPORT_SYMBOL(arad_pp_signal_mngr_signal_get);
EXPORT_SYMBOL(soc_ppd_rif_info_get);
EXPORT_SYMBOL(SOC_PPC_RIF_INFO_print);
EXPORT_SYMBOL(SOC_PPC_RIF_INFO_clear);
EXPORT_SYMBOL(SOC_PPC_FP_DATABASE_STAGE_to_string);
EXPORT_SYMBOL(SOC_PPC_FP_DATABASE_TYPE_to_string);
EXPORT_SYMBOL(arad_pp_dbal_ce_per_program_dump);
EXPORT_SYMBOL(arad_pp_isem_access_print_last_vtt_program_data);
EXPORT_SYMBOL(arad_pp_diag_get_raw_signal);
EXPORT_SYMBOL(arad_pp_diag_ftmh_cfg_get);
EXPORT_SYMBOL(arad_pp_diag_epni_prge_program_tbl_get);
EXPORT_SYMBOL(arad_pp_diag_dbg_val_get_unsafe);
#ifdef BCM_ARAD_SUPPORT
EXPORT_SYMBOL(arad_pp_diag_cos_info_get_unsafe);
EXPORT_SYMBOL(reg_contain_one_of_the_fields);
#endif /* BCM_ARAD_SUPPORT */
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP)
EXPORT_SYMBOL(arad_kbp_print_diag_all_entries);
EXPORT_SYMBOL(arad_kbp_print_diag_last_packet);
#endif /* defined(BCM_88650_A0) && defined(INCLUDE_KBP) */
EXPORT_SYMBOL(ARAD_PP_DIAG_AGGREGATE_ETH_POLICER_INFO_print);
EXPORT_SYMBOL(ARAD_PP_DIAG_ETH_POLICER_INFO_print);
EXPORT_SYMBOL(arad_pp_flp_access_print_last_programs_data);
EXPORT_SYMBOL(arad_pp_flp_access_print_all_programs_data);
EXPORT_SYMBOL(soc_ppd_oam_diag_print_lookup);
EXPORT_SYMBOL(soc_ppd_oam_diag_print_rx);
EXPORT_SYMBOL(soc_ppd_oam_diag_print_em);
EXPORT_SYMBOL(soc_ppd_oam_diag_print_ak);
EXPORT_SYMBOL(soc_ppd_oam_diag_print_oamp_counter);
EXPORT_SYMBOL(soc_ppd_oam_diag_print_debug);
EXPORT_SYMBOL(soc_ppd_oam_diag_print_oam_id);
EXPORT_SYMBOL(arad_pp_oamp_pe_print_all_programs_data);
EXPORT_SYMBOL(arad_pp_oamp_pe_print_last_program_data);
EXPORT_SYMBOL(arad_sw_db_sw_dump);
EXPORT_SYMBOL(arad_itm_congestion_statistics_get);
EXPORT_SYMBOL(Arad_pp_lem_actual_stamp);
EXPORT_SYMBOL(Arad_sw_db_initialized);
EXPORT_SYMBOL(_arad_device_sw_db);
EXPORT_SYMBOL(soc_dpp_defines);
EXPORT_SYMBOL(soc_dpp_implementation_defines);
EXPORT_SYMBOL(bcm_dpp_counter_alloc);
EXPORT_SYMBOL(bcm_dpp_counter_avail_get);
EXPORT_SYMBOL(bcm_dpp_counter_bg_enable_set);
EXPORT_SYMBOL(bcm_dpp_counter_detach);
EXPORT_SYMBOL(bcm_dpp_counter_diag_info_get);
EXPORT_SYMBOL(bcm_dpp_counter_free);
EXPORT_SYMBOL(bcm_dpp_counter_init);
EXPORT_SYMBOL(bcm_dpp_counter_multi_get);
EXPORT_SYMBOL(bcm_dpp_counter_multi_get_cached);
EXPORT_SYMBOL(bcm_dpp_counter_set);
EXPORT_SYMBOL(bcm_dpp_counter_set_cached);
EXPORT_SYMBOL(bcm_dpp_counter_t_names);
EXPORT_SYMBOL(bcm_dpp_am_cosq_fetch_allocated_resources);
EXPORT_SYMBOL(bcm_dpp_am_cosq_fetch_quad);
EXPORT_SYMBOL(bcm_dpp_am_cosq_get_region_type);
EXPORT_SYMBOL(bcm_dpp_am_global_lif_alloc);
EXPORT_SYMBOL(bcm_dpp_counter_background_collection_enable_set);
EXPORT_SYMBOL(cosq_res_info);
EXPORT_SYMBOL(dpp_saved_counter_2);
EXPORT_SYMBOL(shr_llm_appl_info);
EXPORT_SYMBOL(l2_data);
EXPORT_SYMBOL(l2_initialized);
#ifdef BCM_WARM_BOOT_API_TEST
EXPORT_SYMBOL(override_wb_test);
#endif
EXPORT_SYMBOL(ports_unit_info);
EXPORT_SYMBOL(programs_usage);
EXPORT_SYMBOL(_packet_counter);
EXPORT_SYMBOL(_soc_port_tx_handles);
EXPORT_SYMBOL(_soc_tx_pending);
EXPORT_SYMBOL(_switch_sw_db);
EXPORT_SYMBOL(qos_state);
EXPORT_SYMBOL(soc_dpp_chip_dump);
EXPORT_SYMBOL(soc_dpp_cosq_flow_and_up_info_get);
EXPORT_SYMBOL(soc_dpp_cosq_hr2ps_info_get);
EXPORT_SYMBOL(soc_dpp_cosq_non_empty_queues_info_get);
EXPORT_SYMBOL(soc_dpp_cosq_ingress_queue_category_get);
EXPORT_SYMBOL(soc_dpp_cosq_ingress_queue_info_get);
EXPORT_SYMBOL(soc_dpp_cosq_ingress_queue_to_flow_mapping_get);
EXPORT_SYMBOL(soc_dpp_cosq_ingress_test_tmplt_get);
EXPORT_SYMBOL(soc_dpp_cosq_vsq_index_global_to_group_get);
EXPORT_SYMBOL(soc_dpp_fc_status_info_get);
EXPORT_SYMBOL(soc_dpp_lb_config_info_get);
EXPORT_SYMBOL(soc_dpp_dump);
EXPORT_SYMBOL(soc_dpp_fpga_load);
EXPORT_SYMBOL(soc_dpp_fabric_topology_status_get);
EXPORT_SYMBOL(soc_dpp_fabric_rx_fifo_diag_get);
EXPORT_SYMBOL(soc_dpp_fabric_force_set);
EXPORT_SYMBOL(soc_dpp_reg32_read);
EXPORT_SYMBOL(soc_dpp_reinit);
EXPORT_SYMBOL(soc_ppd_diag_db_lem_lkup_info_get);
EXPORT_SYMBOL(soc_ppd_diag_db_lif_lkup_info_get);
EXPORT_SYMBOL(soc_ppd_diag_encap_info_get);
EXPORT_SYMBOL(soc_ppd_diag_frwrd_decision_trace_get);
EXPORT_SYMBOL(soc_ppd_diag_frwrd_decision_get);
EXPORT_SYMBOL(soc_ppd_diag_frwrd_lkup_info_get);
EXPORT_SYMBOL(soc_ppd_diag_frwrd_lpm_lkup_get);
EXPORT_SYMBOL(soc_ppd_diag_ing_vlan_edit_info_get);
EXPORT_SYMBOL(soc_ppd_diag_learning_info_get);
EXPORT_SYMBOL(soc_ppd_diag_mode_info_get);
EXPORT_SYMBOL(soc_ppd_diag_mode_info_set);
EXPORT_SYMBOL(soc_ppd_diag_parsing_info_get);
EXPORT_SYMBOL(soc_ppd_diag_pkt_associated_tm_info_get);
EXPORT_SYMBOL(soc_ppd_diag_pkt_trace_clear);
EXPORT_SYMBOL(soc_ppd_diag_received_packet_info_get);
EXPORT_SYMBOL(soc_ppd_diag_termination_info_get);
EXPORT_SYMBOL(soc_ppd_diag_traps_info_get);
EXPORT_SYMBOL(soc_ppd_diag_egress_vlan_edit_info_get);
EXPORT_SYMBOL(soc_ppd_diag_eg_drop_log_get);
EXPORT_SYMBOL(soc_ppd_fp_packet_diag_get);
EXPORT_SYMBOL(soc_ppd_fp_resource_diag_get);
EXPORT_SYMBOL(soc_ppd_fp_action_info_show);
EXPORT_SYMBOL(soc_ppd_fp_print_all_fems_for_stage);
EXPORT_SYMBOL(soc_ppd_fp_print_fes_info_for_stage);
EXPORT_SYMBOL(soc_ppd_fp_dbs_action_info_show);
EXPORT_SYMBOL(soc_ppd_fp_database_get);
EXPORT_SYMBOL(soc_ppd_fp_direct_extraction_entry_get);
EXPORT_SYMBOL(soc_ppd_fp_entry_get);
EXPORT_SYMBOL(soc_ppd_frwrd_mact_learn_msg_parse);
EXPORT_SYMBOL(SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print);
EXPORT_SYMBOL(soc_ppd_frwrd_mact_entry_get);
EXPORT_SYMBOL(soc_ppd_frwrd_fec_get_block);
EXPORT_SYMBOL(soc_ppd_frwrd_fec_print_block);
EXPORT_SYMBOL(soc_ppd_lif_table_get_block);
EXPORT_SYMBOL(soc_ppd_lif_table_print_block);
EXPORT_SYMBOL(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_print);
EXPORT_SYMBOL(SOC_PPC_LLP_PARSE_TPID_VALUES_print);
EXPORT_SYMBOL(soc_ppd_llp_parse_tpid_profile_info_get);
EXPORT_SYMBOL(soc_ppd_llp_parse_tpid_values_get);

EXPORT_SYMBOL(bcm_dpp_am_fec_is_alloced);
EXPORT_SYMBOL(_bcm_ppd_frwrd_fec_entry_get);
EXPORT_SYMBOL(bcm_dpp_am_fec_get_usage);

EXPORT_SYMBOL(soc_ppd_port_info_get);
EXPORT_SYMBOL(soc_sand_print_list);
EXPORT_SYMBOL(soc_sand_print_indices);
EXPORT_SYMBOL(soc_sand_bitstream_set);
EXPORT_SYMBOL(soc_sand_bitstream_set_field);
EXPORT_SYMBOL(soc_sand_bitstream_get_any_field);
EXPORT_SYMBOL(soc_sand_device_register_with_id);
EXPORT_SYMBOL(soc_sand_get_error_code_from_error_word);
EXPORT_SYMBOL(soc_sand_os_memset);
EXPORT_SYMBOL(soc_sand_os_mutex_give);
EXPORT_SYMBOL(soc_sand_os_mutex_take);
EXPORT_SYMBOL(soc_sand_os_printf);
EXPORT_SYMBOL(soc_sand_os_qsort);
EXPORT_SYMBOL(soc_sand_os_task_delay_milisec);
EXPORT_SYMBOL(soc_sand_u64_devide_u64_long);
EXPORT_SYMBOL(soc_sand_SAND_TABLE_BLOCK_RANGE_clear);
EXPORT_SYMBOL(handle_sand_result);
EXPORT_SYMBOL(mbcm_dpp_driver);
EXPORT_SYMBOL(mbcm_pp_driver);
EXPORT_SYMBOL(soc_arad_egr_congestion_statistics_get);
EXPORT_SYMBOL(soc_arad_stat_controlled_counter_enable_get);
EXPORT_SYMBOL(soc_arad_user_buffer_dram_read);
EXPORT_SYMBOL(soc_arad_user_buffer_dram_write);
EXPORT_SYMBOL(soc_dpp_avs_value_get);
EXPORT_SYMBOL(soc_dpp_device_core_mode);
EXPORT_SYMBOL(soc_jer_port_pll_type_get);
EXPORT_SYMBOL(soc_jer_portmod_calc_os);
EXPORT_SYMBOL(soc_jer_qsgmii_offsets_remove);
EXPORT_SYMBOL(soc_jer_phy_nif_pll_div_get);
EXPORT_SYMBOL(soc_port_sw_db_protocol_offset_get);
EXPORT_SYMBOL(soc_port_sw_db_valid_ports_get);
EXPORT_SYMBOL(soc_port_sw_db_local_to_tm_port_get);
EXPORT_SYMBOL(soc_port_sw_db_master_channel_get);
EXPORT_SYMBOL(soc_port_sw_db_tm_port_to_base_q_pair_get);
EXPORT_SYMBOL(soc_port_sw_db_tm_port_to_out_port_priority_get);
EXPORT_SYMBOL(soc_port_sw_db_tm_to_local_port_get);
EXPORT_SYMBOL(soc_port_sw_db_first_phy_port_get);
EXPORT_SYMBOL(soc_port_sw_db_flags_get);
EXPORT_SYMBOL(soc_port_sw_db_num_lanes_get);
EXPORT_SYMBOL(soc_port_sw_db_phy_ports_get);
EXPORT_SYMBOL(soc_port_sw_db_print);
EXPORT_SYMBOL(soc_port_sw_db_local_to_pp_port_get);
EXPORT_SYMBOL(soc_port_sw_db_core_get);
EXPORT_SYMBOL(soc_port_sw_db_interface_type_get);
EXPORT_SYMBOL(soc_port_sw_db_high_priority_cal_get);
EXPORT_SYMBOL(soc_port_sw_db_low_priority_cal_get);
EXPORT_SYMBOL(soc_port_sw_db_is_valid_port_get);
EXPORT_SYMBOL(soc_port_sw_db_is_initialized_get);
EXPORT_SYMBOL(soc_port_sw_db_channel_get);
EXPORT_SYMBOL(sand_sal_rand_in_kernel_mode);
EXPORT_SYMBOL(Soc_indirect_module_arr);
EXPORT_SYMBOL(Soc_interrupt_mask_address_arr);
EXPORT_SYMBOL(Soc_register_with_id);
EXPORT_SYMBOL(Soc_sand_array_size);
EXPORT_SYMBOL(Soc_sand_big_endian);
EXPORT_SYMBOL(Soc_sand_big_endian_was_checked);
EXPORT_SYMBOL(Soc_sand_chip_descriptors);
EXPORT_SYMBOL(Soc_sand_driver_is_started);
EXPORT_SYMBOL(Soc_sand_errors_msg_queue_flagged);
EXPORT_SYMBOL(Soc_sand_handles_list);
EXPORT_SYMBOL(Soc_sand_ll_is_any_active);
EXPORT_SYMBOL(Soc_sand_ll_timer_cnt);
EXPORT_SYMBOL(Soc_sand_ll_timer_total);
EXPORT_SYMBOL(Soc_sand_max_num_devices);
EXPORT_SYMBOL(Soc_sand_mem_check_read_write_protect);
EXPORT_SYMBOL(Soc_sand_min_time_between_tcm_activation);
EXPORT_SYMBOL(Soc_sand_nof_repetitions);
EXPORT_SYMBOL(soc_sand_ll_timer_clear);
EXPORT_SYMBOL(soc_sand_ll_timer_print_all);
EXPORT_SYMBOL(soc_sand_ll_timer_set);
EXPORT_SYMBOL(soc_sand_ll_timer_stop);
EXPORT_SYMBOL(soc_sand_ll_timer_stop_all);
EXPORT_SYMBOL(Soc_sand_physical_print_asic_style);
EXPORT_SYMBOL(Soc_sand_physical_print_unit_or_base_address);
EXPORT_SYMBOL(Soc_sand_physical_print_first_reg);
EXPORT_SYMBOL(Soc_sand_physical_print_indirect_write);
EXPORT_SYMBOL(Soc_sand_physical_print_last_reg);
EXPORT_SYMBOL(Soc_sand_physical_print_part_of_indirect_read);
EXPORT_SYMBOL(Soc_sand_physical_print_part_of_indirect_write);
EXPORT_SYMBOL(Soc_sand_physical_print_part_of_read_modify_write);
EXPORT_SYMBOL(Soc_sand_physical_print_when_writing);
EXPORT_SYMBOL(Soc_sand_physical_write_enable);
EXPORT_SYMBOL(Soc_sand_start_module_shut_down_mutex);
EXPORT_SYMBOL(Soc_sand_supplied_error_buffer);
EXPORT_SYMBOL(Soc_sand_supplied_error_handler_is_on);
EXPORT_SYMBOL(Soc_sand_system_ticks_in_ms);
EXPORT_SYMBOL(Soc_sand_tcm_mockup_interrupts);

#ifdef CRASH_RECOVERY_SUPPORT
EXPORT_SYMBOL(soc_hw_log_commit);
EXPORT_SYMBOL(soc_hw_log_purge);
EXPORT_SYMBOL(soc_hw_log_diagshow);
EXPORT_SYMBOL(soc_hw_log_print_list);
EXPORT_SYMBOL(soc_hw_log_reg_test);
EXPORT_SYMBOL(soc_hw_log_mem_test);
EXPORT_SYMBOL(soc_hw_set_immediate_hw_access);
EXPORT_SYMBOL(soc_hw_restore_immediate_hw_access);
EXPORT_SYMBOL(soc_mem_single_test);
#endif /* CRASH_RECOVERY_SUPPORT */

extern  int     reg_test(int u, args_t *, void *);
EXPORT_SYMBOL(reg_test);
EXPORT_SYMBOL(_bcm_dpp_gport_to_hw_resources);
EXPORT_SYMBOL(_bcm_ppd_frwrd_host_print_info);
EXPORT_SYMBOL(_bcm_ppd_frwrd_route_print_info);
EXPORT_SYMBOL(failover_linked_list_print);
extern SOC_PPC_MTR_BW_PROFILE_INFO diag_pp_eth_policer_config;
EXPORT_SYMBOL(diag_pp_eth_policer_config);
EXPORT_SYMBOL(soc_restore_bcm88x7x);
#endif /* BCM_DPP_SUPPORT */

#ifdef BCM_DFE_SUPPORT
#include <bcm_int/dfe/dfe_fifo_type.h>
#include <bcm_int/dfe/fabric.h>
#include <bcm_int/dfe/switch.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/dfe_diag.h>
EXPORT_SYMBOL(bcm_dfe_fifo_type_set);
EXPORT_SYMBOL(bcm_dfe_fifo_type_set_id);
EXPORT_SYMBOL(soc_dfe_avs_value_get);
EXPORT_SYMBOL(soc_dfe_chip_dump);
EXPORT_SYMBOL(soc_dfe_dump);
EXPORT_SYMBOL(soc_dfe_fabric_link_status_all_get);
EXPORT_SYMBOL(soc_dfe_counters_info_init);
EXPORT_SYMBOL(soc_dfe_queues_info_init);
EXPORT_SYMBOL(mbcm_dfe_driver);
EXPORT_SYMBOL(soc_dfe_diag_flag_str_fabric_cell_snake_test_stages);
EXPORT_SYMBOL(soc_dfe_diag_fabric_cell_snake_test_interrupts_name_get);
EXPORT_SYMBOL(soc_dfe_diag_fabric_traffic_profile_get);
EXPORT_SYMBOL(_bcm_dfe_fabric_fifo_info_get);
EXPORT_SYMBOL(soc_dfe_drv_soc_properties_dump);
EXPORT_SYMBOL(soc_dfe_defines);
#endif /* BCM_DFE_SUPPORT */

/* Put ukernel debug module related exports here */
#if defined(SOC_UKERNEL_DEBUG)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

#endif
#endif

#if defined(INCLUDE_BOARD)
#include <board/board.h>
EXPORT_SYMBOL(board_attach);
EXPORT_SYMBOL(board_attribute_get);
EXPORT_SYMBOL(board_attribute_set);
EXPORT_SYMBOL(board_attributes_get);
EXPORT_SYMBOL(board_connections_get);
EXPORT_SYMBOL(board_description);
EXPORT_SYMBOL(board_detach);
EXPORT_SYMBOL(board_driver_add);
EXPORT_SYMBOL(board_driver_builtins_add);
EXPORT_SYMBOL(board_driver_builtins_get);
EXPORT_SYMBOL(board_driver_delete);
EXPORT_SYMBOL(board_drivers_get);
EXPORT_SYMBOL(board_find);
EXPORT_SYMBOL(board_init);
EXPORT_SYMBOL(board_name);
EXPORT_SYMBOL(board_probe);
EXPORT_SYMBOL(board_start);
#endif

#include <sal/appl/config.h>
EXPORT_SYMBOL(sal_config_get);
EXPORT_SYMBOL(sal_config_set);

#ifdef BCM_BPROF_STATS
#include <shared/shr_bprof.h>
EXPORT_SYMBOL(shr_bprof_stats_get);
EXPORT_SYMBOL(shr_bprof_stats_name);
EXPORT_SYMBOL(shr_bprof_stats_time_end);
EXPORT_SYMBOL(shr_bprof_stats_time_taken);
EXPORT_SYMBOL(shr_bprof_stats_timer_start);
EXPORT_SYMBOL(shr_bprof_stats_timer_stop);
#endif

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT)
#include <soc/sbusdma_internal.h>
EXPORT_SYMBOL(cmicm_sbusdma_curr_op_details);
#endif

#ifdef INCLUDE_PTP
#include <bcm_int/common/PTP_feature.h>
EXPORT_SYMBOL(ptp_firmware_version);
#endif

#ifdef INCLUDE_KNET
#include <soc/knet.h>
EXPORT_SYMBOL(soc_knet_debug_pkt_get);
EXPORT_SYMBOL(soc_knet_debug_pkt_set);
#endif

#if defined(BCM_HELIX5_SUPPORT)
EXPORT_SYMBOL(soc_helix5_show_voltage);
EXPORT_SYMBOL(bcm_hx5_dump_port_hsp);
#endif /* BCM_HELIX5_SUPPORT */

